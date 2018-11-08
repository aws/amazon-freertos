/* Scheduler includes. */
#include "FreeRTOS.h"

/* Xilinx includes. */
#include "xparameters.h"
#include "xtime_l.h"
#include "xscugic.h"


//#define XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ 666666687
//
///* Global Timer is always clocked at half of the CPU frequency */
#define COUNTS_PER_USECOND  ( XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / ( 2 * 1000000 ) )
// 666666687 / (2*1000000) = 333.3333435


void init_timer( int xTimer )
{
	( void ) xTimer;
}

unsigned long long ullGetHighResolutionTime( void )
{
XTime tCur;

	XTime_GetTime( &tCur );
	tCur /= COUNTS_PER_USECOND;

	return tCur;
}

