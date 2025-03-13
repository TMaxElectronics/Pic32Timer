#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

#include "Timer.h"
#include "TimerConfig.h"


//register map for the pic32mx1xx and pic32mx2xx series chips
const TimerDescriptor_t Tmr_TimerMap[TMR_NUM_TIMERS] = 
    {   
        [0] = {.type = TmrType_A, .registerMap = &T1CON, .iecReg = &IEC0, .ifsReg = &IFS0, .intMask = _IFS0_T1IF_MASK, .ipcReg = &IPC1, .ipcOffset = _IPC1_T1IS_POSITION, .interruptNumber = _TIMER_1_IRQ, .interruptVector = _TIMER_1_VECTOR}, 
        [1] = {.type = TmrType_A, .registerMap = &T2CON, .iecReg = &IEC0, .ifsReg = &IFS0, .intMask = _IFS0_T2IF_MASK, .ipcReg = &IPC2, .ipcOffset = _IPC2_T2IS_POSITION, .interruptNumber = _TIMER_2_IRQ, .interruptVector = _TIMER_2_VECTOR}, 
        [2] = {.type = TmrType_A, .registerMap = &T3CON, .iecReg = &IEC0, .ifsReg = &IFS0, .intMask = _IFS0_T3IF_MASK, .ipcReg = &IPC3, .ipcOffset = _IPC3_T3IS_POSITION, .interruptNumber = _TIMER_3_IRQ, .interruptVector = _TIMER_3_VECTOR}, 
        [3] = {.type = TmrType_A, .registerMap = &T4CON, .iecReg = &IEC0, .ifsReg = &IFS0, .intMask = _IFS0_T4IF_MASK, .ipcReg = &IPC4, .ipcOffset = _IPC4_T4IS_POSITION, .interruptNumber = _TIMER_4_IRQ, .interruptVector = _TIMER_4_VECTOR}, 
        [4] = {.type = TmrType_A, .registerMap = &T5CON, .iecReg = &IEC0, .ifsReg = &IFS0, .intMask = _IFS0_T5IF_MASK, .ipcReg = &IPC5, .ipcOffset = _IPC5_T5IS_POSITION, .interruptNumber = _TIMER_5_IRQ, .interruptVector = _TIMER_5_VECTOR}
    };





//generated ISR primitives, should be enough for most devices :3 If not it'll throw an error
//special case: If freeRtos is present it will use one of the timers for its core tick and generates its own ISR for the corresponding vector. This ISR must be suppressed in this code to not cause problems with that

#if defined(T1CON) && (configTICK_INTERRUPT_VECTOR != _TIMER_1_VECTOR)
   void __ISR(_TIMER_1_VECTOR) T1ISR(){ 
      IFS0CLR = _IFS0_T1IF_MASK;
      TMR_isrHandler(0);
   } 
#endif 
 
#if defined(T2CON) && configTICK_INTERRUPT_VECTOR != _TIMER_2_VECTOR
   void __ISR(_TIMER_2_VECTOR) T2ISR(){ 
      IFS0CLR = _IFS0_T2IF_MASK;
      TMR_isrHandler(1);
   } 
#endif 
 
#if defined(T3CON) && configTICK_INTERRUPT_VECTOR != _TIMER_3_VECTOR
   void __ISR(_TIMER_3_VECTOR) T3ISR(){ 
      IFS0CLR = _IFS0_T3IF_MASK;
      TMR_isrHandler(2);
   } 
#endif 
 
#if defined(T4CON) && configTICK_INTERRUPT_VECTOR != _TIMER_4_VECTOR
   void __ISR(_TIMER_4_VECTOR) T4ISR(){ 
      IFS0CLR = _IFS0_T4IF_MASK;
      TMR_isrHandler(3);
   } 
#endif 
 
#if defined(T5CON) && configTICK_INTERRUPT_VECTOR != _TIMER_5_VECTOR
   void __ISR(_TIMER_5_VECTOR) T5ISR(){ 
      IFS0CLR = _IFS0_T5IF_MASK;
      TMR_isrHandler(4);
   } 
#endif 
 
#if defined(T6CON) && configTICK_INTERRUPT_VECTOR != _TIMER_6_VECTOR
   void __ISR(_TIMER_6_VECTOR) T6ISR(){ 
      IFS0CLR = _IFS0_T6IF_MASK;
      TMR_isrHandler(5);
   } 
#endif 
 
#if defined(T7CON) && configTICK_INTERRUPT_VECTOR != _TIMER_7_VECTOR
   void __ISR(_TIMER_7_VECTOR) T7ISR(){ 
      IFS0CLR = _IFS0_T7IF_MASK;
      TMR_isrHandler(6);
   } 
#endif 
 
#if defined(T8CON) && configTICK_INTERRUPT_VECTOR != _TIMER_8_VECTOR
   void __ISR(_TIMER_8_VECTOR) T8ISR(){ 
      IFS0CLR = _IFS0_T8IF_MASK;
      TMR_isrHandler(7);
   } 
#endif 
 
#if defined(T9CON) && configTICK_INTERRUPT_VECTOR != _TIMER_9_VECTOR
   void __ISR(_TIMER_9_VECTOR) T9ISR(){ 
      IFS0CLR = _IFS0_T9IF_MASK;
      TMR_isrHandler(8);
   } 
#endif 
 
#if defined(T10CON) && configTICK_INTERRUPT_VECTOR != _TIMER_10_VECTOR
   void __ISR(_TIMER_10_VECTOR) T10ISR(){ 
      IFS0CLR = _IFS0_T10IF_MASK;
      TMR_isrHandler(9);
   } 
#endif 
 
#if defined(T11CON) && configTICK_INTERRUPT_VECTOR != _TIMER_11_VECTOR
   void __ISR(_TIMER_11_VECTOR) T11ISR(){ 
      IFS0CLR = _IFS0_T11IF_MASK;
      TMR_isrHandler(10);
   } 
#endif 
 
#if defined(T12CON) && configTICK_INTERRUPT_VECTOR != _TIMER_12_VECTOR
   void __ISR(_TIMER_12_VECTOR) T12ISR(){ 
      IFS0CLR = _IFS0_T12IF_MASK;
      TMR_isrHandler(11);
   } 
#endif 
 
   
#if defined(T13CON)
    #error There are more timers than generated ISRs :/ If you get this error you should add more in the lines above and update the #if
#endif