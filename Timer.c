#include <stdint.h>
#include <xc.h>

#include "Timer.h"

static uint32_t availableTimers[TMR_NUM_TIMERS] = {[0 ... (TMR_NUM_TIMERS - 1)] = 1};


//allocates a specified timer
TimerHandle_t * Tmr_init(uint32_t timerNumber){
	//is the timer even available?
	if(availableTimers[timerNumber - 1] == 0) return NULL;
	
	//try to get memory
	TimerHandle_t * ret = TMR_MALLOC(sizeof(TimerHandle_t));
	
	//did we actually get memory?
	if(ret == NULL) return NULL;

	//mark timer as not available
	availableTimers[timerNumber - 1] = 0;
	
	
}

void TMR_setMode(TimerHandle_t * handle, TimerMode_t mode);

void TMR_setEnabled(TimerHandle_t * handle, uint32_t enabled);

void TMR_setISR(TimerHandle_t * handle, TimerISR_t isrFunction, uint32_t priority, uint32_t subPriority);
 
void TMR_setPeriod(TimerHandle_t * handle, uint32_t period_us);

//functions to get pointers to the timer counter and compare registers
uint32_t * TMR_getPRPointer(TimerHandle_t * handle);
uint32_t * TMR_getTMRPointer(TimerHandle_t * handle);

uint32_t TMR_getInterruptNumber(TimerHandle_t * handle);
uint32_t TMR_getInterruptVector(TimerHandle_t * handle);

void TMR_handleInterrupt(uint32_t timerNumber);
 

#ifdef T1CON 
   void __ISR(_TIMER_1_VECTOR) T1ISR(){ 
      IFS0CLR = _IFS0_T1IF_MASK;
      TMR_isrHandler(1);
   } 
#endif 
 
 
#ifdef T2CON 
   void __ISR(_TIMER_2_VECTOR) T2ISR(){ 
      IFS0CLR = _IFS0_T2IF_MASK;
      TMR_isrHandler(2);
   } 
#endif 
 
 
#ifdef T3CON 
   void __ISR(_TIMER_3_VECTOR) T3ISR(){ 
      IFS0CLR = _IFS0_T3IF_MASK;
      TMR_isrHandler(3);
   } 
#endif 
 
 
#ifdef T4CON 
   void __ISR(_TIMER_4_VECTOR) T4ISR(){ 
      IFS0CLR = _IFS0_T4IF_MASK;
      TMR_isrHandler(4);
   } 
#endif 
 
 
#ifdef T5CON 
   void __ISR(_TIMER_5_VECTOR) T5ISR(){ 
      IFS0CLR = _IFS0_T5IF_MASK;
      TMR_isrHandler(5);
   } 
#endif 
 
 
#ifdef T6CON 
   void __ISR(_TIMER_6_VECTOR) T6ISR(){ 
      IFS0CLR = _IFS0_T6IF_MASK;
      TMR_isrHandler(6);
   } 
#endif 
 
 
#ifdef T7CON 
   void __ISR(_TIMER_7_VECTOR) T7ISR(){ 
      IFS0CLR = _IFS0_T7IF_MASK;
      TMR_isrHandler(7);
   } 
#endif 
 
 
#ifdef T8CON 
   void __ISR(_TIMER_8_VECTOR) T8ISR(){ 
      IFS0CLR = _IFS0_T8IF_MASK;
      TMR_isrHandler(8);
   } 
#endif 
 
 
#ifdef T9CON 
   void __ISR(_TIMER_9_VECTOR) T9ISR(){ 
      IFS0CLR = _IFS0_T9IF_MASK;
      TMR_isrHandler(9);
   } 
#endif 
 
 
#ifdef T10CON 
   void __ISR(_TIMER_10_VECTOR) T10ISR(){ 
      IFS0CLR = _IFS0_T10IF_MASK;
      TMR_isrHandler(10);
   } 
#endif 
 
 
#ifdef T11CON 
   void __ISR(_TIMER_11_VECTOR) T11ISR(){ 
      IFS0CLR = _IFS0_T11IF_MASK;
      TMR_isrHandler(11);
   } 
#endif 
 
 
#ifdef T12CON 
   void __ISR(_TIMER_12_VECTOR) T12ISR(){ 
      IFS0CLR = _IFS0_T12IF_MASK;
      TMR_isrHandler(12);
   } 
#endif 
 
 
