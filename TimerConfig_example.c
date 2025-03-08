#include <stdint.h>
#include <xc.h>

#include "Timer.h"
#include "TimerConfig.h"


const TimerDescriptor_t Tmr_TimerTypes[TMR_NUM_TIMERS] = {{.type = TmrType_A, .baseAddress = &T1CON, .interruptNumber = _TIMER_1_IRQ, .interruptVector = _TIMER_1_VECTOR}, {.type = TmrType_B_Master, .baseAddress = &T2CON}, {.type = TmrType_B_Slave, .baseAddress = &T3CON}, {.type = TmrType_B_Master, .baseAddress = &T4CON}, {.type = TmrType_B_Slave, .baseAddress = &T5CON}};