#ifndef Timer_INC 
#define Timer_INC 

#include <stdint.h>
#include <xc.h>

#!__has_include("TimerConfig.h")
	#error "No timer config file found in project!"
#endif

#define TMR_FLAG_USED 0x00000001

//Available Timer Types for standard MIPS Pic32 Processors. Type 0 is invalid to prevent accidental access to a timer that was not defined in the timer array
typedef enum {TmrType_INVALID, TmrType_A, TmrType_B_Master, TmrType_B_Slave} TimerType_t;

//timer modes
typedef enum {TmrMode_SingleShot, TmrMode_freeRunning} TimerType_t;

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
	uint32_t interruptNumber;
	uint32_t interruptVector;
} TimerDescriptor_t;

//Handle for the timer given to the user as a reference. This only references the timer by number, 
//as it is safer to acces a semi-constant pointer in flash than one passed in a handle that lies in ram
typedef struct{
	uint32_t number;
	uint32_t flags;
} TimerHandle_t;

typedef uint32_t (*TimerISR_t)(TimerHandle_t * handle, uint32_t flags);

TimerHandle_t * Tmr_init(uint32_t timerNumber);

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
 
#endif 
