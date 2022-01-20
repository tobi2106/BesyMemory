/* Include-file defining global defines and constants  	 		*/
/* used by the operating system */
#ifndef __GLOBALS__
#define __GLOBALS__

#define	_CRT_SECURE_NO_WARNINGS		// suppress legacy warnings 

#include "bs_types.h"
#include "core.h"
#include "log.h"
#include "MemoryList.h"


// Number of possible concurrent processes 
#define MAX_PROCESSES (unsigned) 100

// Size of the physical memory
#define MEMORY_SIZE (unsigned) 1024

// Largest valid pid
#define MAX_PID 100

// Maximum duration one process gets the CPU, zero indicates no preemption
#define QUANTUM 0

// Constant value for time consumed by scheduler and dispatcher
#define SCHEDULING_DURATION 5
#define LOADING_DURATION 5

// Mean values between IO events by process type
#define IO_MEAN_OS			40
#define IO_MEAN_INTERACTIVE	5
#define IO_MEAN_BATCH		200
#define IO_MEAN_BACKGROUND	30
#define IO_MEAN_FOREGROUND	15
#define IO_MEAN_DEFAULT		0

// mean value for duration of IO
#define IO_MEAN_DURATION	10

// name of the file with process definitions
#define PROCESS_FILENAME "processes.txt"

// defining colors
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"


/* ----------------------------------------------------------------	*/
/* Define global variables that will be visible in all source files	*/
extern PCB_t processTable[MAX_PROCESSES];	// the process table
extern unsigned systemTime;					// the current system time (up time)
extern unsigned usedMemory;					// amount of used physical memory
extern unsigned runningCount;				// counter of currently running processes
unsigned waitingCount;					// counter of currently waiting processes in Queue
extern Boolean batchComplete;				// end of pending processes in the file indicator
extern FILE* processFile;					// file containing the processes to simulate

#endif  /* __GLOBALS__ */
