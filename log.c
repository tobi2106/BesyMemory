/* Implementation of the log functions */
/* for comments on the functions see the associated .h-file */

/* ---------------------------------------------------------------- */
/* Include required external definitions */
#include <math.h>
#include "bs_types.h"
#include "globals.h"
#include "log.h"
#include <stdio.h>

/* ---------------------------------------------------------------- */
/*                Declarations of local helper functions            */
//TODO Not good
extern PCB_t candidateProcess; // only for simulation purposes

/* ---------------------------------------------------------------- */
/* Declarations of global variables visible only in this file 		*/
// array with strings associated to scheduling events for log outputs
char eventString[3][12] = {"completed", "io", "quantumOver"};

/* ---------------------------------------------------------------- */
/*                Externally available functions					*/
/* ---------------------------------------------------------------- */

void logGeneric(char* message)
{
	printf(CYN "[log] \t%u \t: %s\n" RESET, systemTime, message);
}

void logInit(char* message)
{
	printf(GRN "[sys] \t%u \t: %s\n" RESET, systemTime, message);
}

void logRdyToRun(char* message)
{
	printf(CYN "[log] \t%u \t: [READY IN : %u] %s\n" RESET, systemTime, candidateProcess.start - systemTime, message);
}

void logError(char* message)
{
	printf(RED "[log] \t%u \t: %s\n" RESET, systemTime, message);
}

void logPid(unsigned pid, char* message)
{
	printf(CYN "[log] \t%u \t: [PID %u] : %s\n" RESET, systemTime, pid, message);
}

void logPidCompleteness(unsigned pid, unsigned done, unsigned length, char* message)
{
	printf(CYN "[log] \t%u \t: [PID %u] : completeness: %u/%u | %s\n" RESET, systemTime, pid, done, length, message);
}

void logPidMem(unsigned pid, char* message)
{
	printf(YEL "[SIM] \t%u \t: [PID : %u] [USED MEMORY : %u] [FREE MEMORY : %u] %s\n" RESET, systemTime, pid, usedMemory, MEMORY_SIZE - usedMemory,message);
}

void logProRead(unsigned ownerID, unsigned start, unsigned duration, unsigned size, char* pType)
{
	printf(YEL "[SIM] \t%u \t: New process read from File: %u |  %u |  %u |  %u |  %s\n" RESET,
		systemTime, ownerID, start, duration, size, pType);
}

void logMem(char* message) 
{
	printf("[mem] \t%u \t: %s", systemTime, message);
}

void logQ(char* message) 
{
	printf("[que] \t%u \t: %s", systemTime, message);
}


/* ----------------------------------------------------------------- */
/*                       Local helper functions                      */
/* ----------------------------------------------------------------- */
