#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

#if __has_include("FreeRTOS.h")
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#endif

#include "Timer.h"
#include "TimerConfig.h"
#include "util.h"
#include "System.h"

#define Tmr_is32Bit(handle) ((handle->descriptor->type != TmrType_A) && (handle->flags & TMR_FLAG_32BIT_MODE))
#define TMR_REGS (*handle->descriptor->registerMap)

//array to signal which timer is used and which isn'T
static uint32_t availableTimers[TMR_NUM_TIMERS] = {[0 ... (TMR_NUM_TIMERS - 1)] = 1};

//list of timer ISR callbacks
static TimerISRDescriptor_t isrDescriptors[TMR_NUM_TIMERS];

static uint32_t typeAPrescalersShifts[4] = {0, 3, 6, 8};
static uint32_t typeBPrescalersShifts[8] = {0, 1, 2, 3, 4, 5, 6, 8};

//allocates a specified timer
TimerHandle_t * Tmr_init(uint32_t timerNumber, uint32_t enable32BitMode){
	//is the timer even available?
	if(availableTimers[timerNumber - 1] == 0) return NULL;
    
    //check if conditions for 32bit mode are met if its enabled
    if(enable32BitMode){
        //is the timer a type b master timer? (This should also imply there is another timer after this, so we ignore the if(timerNumber >= timercount) check) TODO: evaluate if this might not be a good idea anyway
        if(Tmr_TimerMap[timerNumber - 1].type != TmrType_B_Master) return NULL;
    }
	
	//try to get memory
	TimerHandle_t * ret = TMR_MALLOC(sizeof(TimerHandle_t));
	
	//did we actually get memory?
	if(ret == NULL) return NULL;

	//mark timer(s) as not available
	availableTimers[timerNumber - 1] = 0;
    //32bit mode also uses the slave timer at timerNumber + 1
    if(enable32BitMode) availableTimers[timerNumber] = 0;
    
	
    //initialise variables
    memcpy(&ret->descriptor, &Tmr_TimerMap[timerNumber - 1], sizeof(TimerDescriptor_t));
	ret->number = timerNumber;
	ret->flags = enable32BitMode ? TMR_FLAG_32BIT_MODE : 0;
    ret->currentMode = TmrMode_Off;
    
    //set the 32bit mode bit. If the timer doesn't support it then the write won't do anything
    ret->descriptor->registerMap->TCON.T32 = enable32BitMode;
    
    //return the handle
    return ret;
}

//frees a timer
void TMR_deinit(TimerHandle_t * handle){
    //did we even get a pointer?
    if(handle == NULL) return;
    
    //is there at least a chance the pointer is valid? TODO: perhaps add a software breakpoint here?
    if(handle->number == 0) return;
    
    //check if the timer is even still allocated. If it is we need to reset the regs to their semi-default
    if(availableTimers[handle->number - 1] == 0){
        //reset regs
        TMR_REGS.TCON.w = 0;
        TMR_REGS.PR = 0;
        TMR_REGS.TMR = 0;
        
        
        //is the timer in 32 bit mode?
        if(handle->flags & TMR_FLAG_32BIT_MODE){
            //yes, the timer at number+1 was ours too and also needs to be reset
            Tmr_TimerMap[handle->number].registerMap->TCON.w = 0;
            Tmr_TimerMap[handle->number].registerMap->PR = 0;
            Tmr_TimerMap[handle->number].registerMap->TMR = 0;
        }
    }
    
    //mark the timer as available again
    availableTimers[handle->number - 1] = 1;
    
    //before we free the memory invalidate the pointer, just incase it gets passed to us again on accident
    handle->number = 0;
    
    //and finally free the memory
    TMR_FREE(handle);
}

//set timer mode
void TMR_setMode(TimerHandle_t * handle, TimerMode_t mode){
    handle->currentMode = mode;
}

//switch the timer on or off
void TMR_setEnabled(TimerHandle_t * handle, uint32_t enabled){
    //are we switching the timer on or off?
    if(enabled){
        //clear the on bit (not necessary for the slave timer in 32bit mode as its con register has no effect)
        handle->descriptor->registerMap->TCONSET.w = _T1CON_TON_MASK;
    }else{
        //clear the on bit
        handle->descriptor->registerMap->TCONCLR.w = _T1CON_TON_MASK;
    }
}

//switch the timer on or off
uint32_t TMR_isEnabled(TimerHandle_t * handle){
    return handle->descriptor->registerMap->TCON.ON;
}

//set the desired period of the timer. Returns 1 on success or 0 if the desired period could not be achieved
uint32_t TMR_setPeriod(TimerHandle_t * handle, uint32_t period_us){
//first we need to find the best matching prescaler
    
    //in order to be able to cover the entire range of possible periods we need to perform the operation in 64bit arithmetic :/
    uint32_t minScaler = (uint32_t) ((uint64_t) ((uint64_t) TMR_CLK_Hz * (uint64_t) period_us) / (uint64_t) ((uint64_t) 0xffffffff * (uint64_t) 1000000));
    
    //can we even achieve this? 256 is the maximum divider for both timer types
    if(minScaler > 256){
        //no, return an error and don't set anything
        return 0;
    }
    
    
    uint32_t selectedScaler = 0;
    //now check which one of the dividers is the lowest one thats over the minimum
    if(handle->descriptor->type == TmrType_A){
        for(uint32_t i = 0; i < arraySize(typeAPrescalersShifts); i++){
            if((1 << typeAPrescalersShifts[i]) >= minScaler){
                selectedScaler = i;
                break;
            }
        }
    }else{
        for(uint32_t i = 0; i < arraySize(typeBPrescalersShifts); i++){
            if((1 << typeBPrescalersShifts[i]) >= minScaler){
                selectedScaler = i;
                break;
            }
        }
    }
    
    
//now calculate the required period register value
    
    //scaler is now selected such that the divider value will not exceed the maximum period count. As that is 32bit we also won't exceed a uint32_t and thus don't need 64bit arithmetic
    uint32_t divider = (TMR_CLK_Hz >> typeAPrescalersShifts[selectedScaler]) * period_us;
    
    
//finally write the values to the timer
    
    //make sure the timer is off while we update the registers
    uint32_t state = TMR_isEnabled(handle);
    TMR_setEnabled(handle, 0);
    
    TMR_setPrescaler(handle, selectedScaler);
    TMR_setPR(handle, divider);
    
    if(state) TMR_setEnabled(handle, 1);
}

uint32_t TMR_setFrequency(TimerHandle_t * handle, uint32_t Frequency_mHz){
    //TODO
}

uint32_t TMR_getPeriod_us(TimerHandle_t * handle){
    //calculate the clocks period in nanoseconds
    uint32_t clockPeriod_ns = 1000000000 / TMR_CLK_Hz;
    
    //get the prescaler
    uint32_t scaler = 1;
    if(handle->descriptor->type == TmrType_A){
        scaler = 1 << typeAPrescalersShifts[handle->descriptor->registerMap->TCON.TYPEA_TCKPS];
    }else{
        scaler = 1 << typeBPrescalersShifts[handle->descriptor->registerMap->TCON.TYPEB_TCKPS];
    }
    
    //calculate the period. This must once again be done in 64bit arithmetic :/
    uint32_t period_us = (uint32_t) ((uint64_t) ((uint64_t) clockPeriod_ns * (uint64_t) scaler * (uint64_t) handle->descriptor->registerMap->PR) / (uint64_t) 1000);
    
    return period_us;
}

uint32_t TMR_setCustomDivider(TimerHandle_t * handle, uint32_t preScaler, uint32_t divider){
    //TODO
}

//assign an interrupt routine to a timer. To de-assign call with isr* = NULL
uint32_t TMR_setISR(TimerHandle_t * handle, TimerISR_t isr){
    //what number does the isr need to be assigned to?
    uint32_t timerNumber = handle->number-1;
    if(Tmr_is32Bit(handle)) timerNumber = handle->number;
    
    //are we assigning or de-assigning?
    if(isr == NULL){
        //write NULL into the isr list for the function and we're done
        isrDescriptors[timerNumber].function = NULL;
    }else{
        //is there already a function assigned? If so we won't overwrite it
        if(isrDescriptors[timerNumber].function != NULL) return pdFAIL;
        
        //assign the functions
        isrDescriptors[timerNumber].function = isr;
    }
    
    return pdPASS;
}

void TMR_setIRQEnabled(TimerHandle_t * handle, uint32_t on){
    TimerDescriptor_t * desc = Tmr_is32Bit(handle) ? &Tmr_TimerMap[handle->number] : handle->descriptor;
    
    if(on){
        desc->iecReg->SET = desc->intMask;
    }else{
        desc->iecReg->CLR = desc->intMask;
    }
}

void TMR_setIRQEnabledByNumber(uint32_t number, uint32_t on){
    TimerDescriptor_t * desc = (Tmr_TimerMap[number-1].registerMap->TCON.T32) ? &Tmr_TimerMap[number] : &Tmr_TimerMap[number - 1];
    
    if(on){
        desc->iecReg->SET = desc->intMask;
    }else{
        desc->iecReg->CLR = desc->intMask;
    }
}

uint32_t TMR_isIRQEnabled(TimerHandle_t * handle){
    //is the timer in 32Bit mode? If so the isr data comes from the slave timer
    if(Tmr_is32Bit(handle)) return (Tmr_TimerMap[handle->number].iecReg->w & Tmr_TimerMap[handle->number].intMask) > 0;
    
    return (handle->descriptor->iecReg->w & handle->descriptor->intMask) > 0;
}

uint32_t TMR_readIFS(TimerHandle_t * handle){
    //is the timer in 32Bit mode? If so the isr data comes from the slave timer
    if(Tmr_is32Bit(handle)) return (Tmr_TimerMap[handle->number].ifsReg->w & Tmr_TimerMap[handle->number].intMask) > 0;
    
    return (handle->descriptor->ifsReg->w & handle->descriptor->intMask) > 0;
}

void TMR_clearIFS(TimerHandle_t * handle){
    //in 32bit mode the bits from the slave timer need to be cleared
    if(Tmr_is32Bit(handle)){
        Tmr_TimerMap[handle->number].ifsReg->CLR = handle->descriptor->intMask;
    }else{
        handle->descriptor->ifsReg->CLR = handle->descriptor->intMask;
    }
}

void TMR_setInterruptPriority(TimerHandle_t * handle, uint32_t priority, uint32_t subPriority){
    //we will have to perform a read-modify-write on the register to update the priority
    //since this would take forever though we utilise the pics SET and CLR registers together with a mask
    
    //first we need to select the correct descriptor. If the timer is in 32bit mode all interrupt related settings come from the slave timer
    TimerDescriptor_t * desc = Tmr_is32Bit(handle) ? &Tmr_TimerMap[handle->number] : handle->descriptor;
    
    //first we make sure the interrupt is off
    uint32_t irqEnabled = TMR_isIRQEnabled(handle);
    TMR_setIRQEnabled(handle, 0);
    
    //then we clear the priority bits. There are 5 (hence 0b11111) and located at the offset pointed to by ipcOffset
    desc->ipcReg->CLR = 0b11111 << desc->ipcOffset;
    
    //then we generate the new priority bits
    Pic32PrioBits_t map = {.priority = priority, .subPriority = subPriority};
    
    //after that we assign the shifted mask to the SET register to complete the operation
    desc->ipcReg->SET = map.map << desc->ipcOffset;
   
    TMR_setIRQEnabled(handle, irqEnabled);
}

void TMR_setPrescaler(TimerHandle_t * handle, uint32_t scaler){
    //check what type of timer we're dealing with
    if(handle->descriptor->type == TmrType_A){
        TMR_REGS.TCON.TYPEA_TCKPS = scaler;
    }else{
        TMR_REGS.TCON.TYPEB_TCKPS = scaler;
    }
}

uint32_t TMR_getCount(TimerHandle_t * handle){
    return handle->descriptor->registerMap->TMR;
}

void TMR_setPR(TimerHandle_t * handle, uint32_t prValue){
    //write the value to the PR register. If the timer is in 32bit mode the hardware will automatically map the lower and upper bytes accordingly
    handle->descriptor->registerMap->PR = prValue;
    
    //reset the timer counter if the current count is larger than the period compare value
    if(handle->descriptor->registerMap->TMR > prValue) handle->descriptor->registerMap->TMR = 0;
}

//functions to get pointers to the timer counter and compare registers
uint32_t * TMR_getPRPointer(TimerHandle_t * handle){
    return &(TMR_REGS.PR);
}

uint32_t * TMR_getTMRPointer(TimerHandle_t * handle){
    return &(TMR_REGS.TMR);
}

uint32_t TMR_getInterruptNumber(TimerHandle_t * handle){
    //is the timer in 32Bit mode? If so the isr number is that of the slave timer (so number + 1)
    if(Tmr_is32Bit(handle)) return Tmr_TimerMap[handle->number].interruptNumber;
    
    return handle->descriptor->interruptNumber;
}

uint32_t TMR_getInterruptVector(TimerHandle_t * handle){
    //is the timer in 32Bit mode? If so the isr number is that of the slave timer (so number + 1)
    if(Tmr_is32Bit(handle)) return Tmr_TimerMap[handle->number].interruptVector;
    
    return handle->descriptor->interruptVector;
}
 

//function that gets called when an isr occurs. timerIndex is the timerNumber but already decremented by 1 (so the timer array index)
void TMR_isrHandler(uint32_t timerIndex){
    //a timer irq just occurred, check what we need to do to handle it
    
    //is there a handle defined for the timer?
    if(isrDescriptors[timerIndex].handle == NULL){
        //no, switch off the timer and return
        TMR_setIRQEnabledByNumber(timerIndex + 1, 0);
        
        return;
    }
    
    //first get the handle
    TimerHandle_t * handle = isrDescriptors[timerIndex].handle;
    
    //is the timer in single shot mode? If so we should disable it now
    if(handle->currentMode == TmrMode_SingleShot){
        TMR_setIRQEnabled(handle, 0);
    }
    
    //is an isr assigned to this timer?
    if(isrDescriptors[timerIndex].function != NULL){
        //yes! call it
        (*isrDescriptors[timerIndex].function)(handle, 0);
    }
    
}