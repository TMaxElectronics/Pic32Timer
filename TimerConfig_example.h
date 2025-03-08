#ifndef Timer_CONF
#define Timer_CONF

/*
* Config file for the Pic32Timer Library
* 
* This file and its corresponding C File must be renamed to TimerConfig.[c/h] and available in the projects include path.
* 
* It describes the timer hardware in the device you are using and must be adjusted when you move to a new chip with a different hardware setup.
*/

#include <stdint.h>

//define the number of timers available on your device here.
#define TMR_NUM_TIMERS 5

//define the memory allocation and free functions to be used by the library here
#define TMR_MALLOC(X) pvPortMalloc(X)
#define TMR_FREE(X) vPortFree(X)

//define the frequency of the bus the timers are running from
#define TMR_FREE(X) configPERIPHERAL_CLOCK_HZ

//this array contains a list of timers with their base addresses aswell as their types. It must be initialised in the corresponding .c file.
extern const TimerDescriptor_t Tmr_TimerTypes[];

#endif 
