#include <stdint.h>
#include <xc.h>

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