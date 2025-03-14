#ifndef Timer_INC 
#define Timer_INC 

#include <stdint.h>
#include <xc.h>

#include "System.h"

#if __is_compiling && !__has_include("TimerConfig.h")
	#error "No timer config file found in project!"
#endif

#define TMR_FLAG_USED 0x00000001
#define TMR_FLAG_32BIT_MODE 0x00000002

//Available Timer Types for standard MIPS Pic32 Processors. Type 0 is invalid to prevent accidental access to a timer that was not defined in the timer array
typedef enum {TmrType_INVALID, TmrType_A, TmrType_B_Master, TmrType_B_Slave} TimerType_t;

//timer modes
typedef enum {TmrMode_SingleShot, TmrMode_freeRunning, TmrMode_Off} TimerMode_t;

#define Tmr_reset(handle) handle->registerMap.TMRCLR = 0xffffffff

//TCon Register map. Type A and Type B have compatible bit maps, as all bits are in the same location across both only leaving out those that aren't available
typedef struct{
	struct {
		uint32_t :1;
		uint32_t TCS:1;
		uint32_t TSYNC:1;
		uint32_t T32:1;
		
		uint32_t :3;
		
		uint32_t TGATE:1;
		
		uint32_t :3;
		uint32_t TWIP:1;
		uint32_t TWDIS:1;
		
		uint32_t SIDL:1;
		uint32_t :1;
		uint32_t ON:1;
	};
	struct {
		uint32_t :4;
		uint32_t TYPEA_TCKPS:2;
	};
	struct {
		uint32_t :4;
		uint32_t TYPEB_TCKPS:3;
	};
	struct {
		uint32_t w:32;
	};
} TConMap_t;

//Timer register map, also identical for Type A and Type B
typedef struct{
	TConMap_t TCON;
	TConMap_t TCONCLR;
	TConMap_t TCONSET;
	TConMap_t TCONINV;
	
	uint32_t TMR;
	uint32_t TMRCLR;
	uint32_t TMRSET;
	uint32_t TMRINV;
	
	uint32_t PR;
	uint32_t PRCLR;
	uint32_t PRSET;
	uint32_t PRINV;
} TmrMap_t;

//timer descriptor, used in the timer array in TimerConfig.c
typedef struct{
	TimerType_t type;
	TmrMap_t * registerMap;
    
    Pic32SetClearMap_t * iecReg;
    Pic32SetClearMap_t * ifsReg;
    uint32_t   intMask;
    
    Pic32SetClearMap_t * ipcReg;
    uint32_t ipcOffset;
    
	uint32_t interruptNumber;
	uint32_t interruptVector;
} TimerDescriptor_t;

//Handle for the timer given to the user as a reference.
typedef struct{
	TimerDescriptor_t * descriptor;
    
    TimerMode_t currentMode;
	uint32_t number;
	uint32_t flags;
} TimerHandle_t;

//prototype of a function that can be used as an intterupt service routine
typedef uint32_t (*TimerISR_t)(TimerHandle_t * handle, uint32_t flags, void* data);


//allocates a specified timer
TimerHandle_t * Tmr_init(uint32_t timerNumber, uint32_t enable32BitMode);

//frees a timer
void TMR_deinit(TimerHandle_t * handle);



//set timer mode
void TMR_setMode(TimerHandle_t * handle, TimerMode_t mode);


//switch the timer on or off
void TMR_setEnabled(TimerHandle_t * handle, uint32_t enabled);

//switch the timer on or off
uint32_t TMR_isEnabled(TimerHandle_t * handle);


//set the desired period of the timer. Returns 1 on success or 0 if the desired period could not be achieved
uint32_t TMR_setPeriod(TimerHandle_t * handle, uint32_t period_us);

uint32_t TMR_getPeriod_us(TimerHandle_t * handle);

uint32_t TMR_setFrequency(TimerHandle_t * handle, uint32_t Frequency_mHz);
uint32_t TMR_getFrequency_mHz(TimerHandle_t * handle);


uint32_t TMR_setCustomDivider(TimerHandle_t * handle, uint32_t preScaler, uint32_t divider);

void TMR_setPrescaler(TimerHandle_t * handle, uint32_t scaler);

uint32_t TMR_getCount(TimerHandle_t * handle);

uint32_t TMR_calculatePR(TimerHandle_t * handle, uint32_t period_us, uint32_t divider);

void TMR_setPR(TimerHandle_t * handle, uint32_t prValue);

void TMR_setIRQEnabled(TimerHandle_t * handle, uint32_t on);
void TMR_setIRQEnabledByNumber(uint32_t number, uint32_t on);

uint32_t TMR_isIRQEnabled(TimerHandle_t * handle);

uint32_t TMR_setISR(TimerHandle_t * handle, TimerISR_t isr, void * data);

void TMR_setInterruptPriority(TimerHandle_t * handle, uint32_t priority, uint32_t subPriority);

uint32_t TMR_readIFS(TimerHandle_t * handle);

void TMR_clearIFS(TimerHandle_t * handle);

//functions to get pointers to the timer counter and compare registers
uint32_t * TMR_getPRPointer(TimerHandle_t * handle);
uint32_t * TMR_getTMRPointer(TimerHandle_t * handle);
uint32_t TMR_getInterruptNumber(TimerHandle_t * handle);
uint32_t TMR_getInterruptVector(TimerHandle_t * handle);

void TMR_isrHandler(uint32_t timerIndex);
 
#endif 
