// main file, top level of simulation/test environment for the		*/
/* memory management component of the operating system				*/
/* under development												*/
#include "bs_types.h"
#include "globals.h"
#include "loader.h"
#include "core.h"
#include <time.h>
#include "MemoryList.h"
#include "Queue.h"

/* ----------------------------------------------------------------	*/
/* Declare global variables according to definition in globals.h	*/
unsigned systemTime = 0;				// the current system time (up time)
unsigned runningCount = 0;				// counter of currently running processes
unsigned waitingCount = 0;				// counter of currently waiting processes in Queue
extern unsigned usedMemory = 0;			// amount of used physical memory

Boolean batchComplete = FALSE;			// end of pending processes in the file indicator
FILE* processFile;						// file containing the processes to simulate
PCB_t processTable[MAX_PROCESSES];		// the process table

/* ----------------------------------------------------------------	*/
/* Declarations of global variables visible only in this file 		*/
PCB_t candidateProcess;					// only for simulation purposes

/* ---------------------------------------------------------------- */
/*                Declarations of local functions                   */
int initSim();							// initialise the simulation environment

int main(int argc, char* argv[])
{
	//mainMemory();
	mainQueue();
	// starting point, all processing is done in called functions
	printf(GRN "[SYSTEM] Starting system. Available memory: %u\n" RESET, MEMORY_SIZE);
	initSim();							// initialise the simulation
	initOS();							// initialise operating system, itself
	logGeneric("System initialized, starting batch");
	coreLoop();							// start scheduling loop
	displayMemory();
	displayQ();
	logGeneric("Batch complete, shutting down");
	fflush(stdout);						// make sure the output on the console is complete 
	return 1;
}

/* ---------------------------------------------------------------- */
/*                Implementation of local functions                 */

int initSim()
{
	// initializes the simulation environment

	char filename[128] = PROCESS_FILENAME;
	systemTime = 0;						// reset the system time to zero
	
	// open the file with process definitions
	processFile = openConfigFile(processFile, filename);
	srand((unsigned)time(NULL)); // init the random number generator

	return TRUE;
}
