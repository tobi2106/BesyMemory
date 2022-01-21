/* Implementation of core functionality of the OS					*/
/* this includes the main scheduling loop							*/
/* for comments on the functions see the associated .h-file */
/* Changes made by Alexander Heffner and Tobias Tobel */

/* ---------------------------------------------------------------- */
/* Include required external definitions */
#include <stdlib.h>
#include "bs_types.h"
#include "globals.h"
#include "loader.h"
#include "core.h"
#include "executer.h"
#include "MemoryList.h"
#include "Queue.h"
#include "log.h"


/* ----------------------------------------------------------------	*/
/* Declarations of global variables visible only in this file 		*/
//extern PCB_t candidateProcess; // only for simulation purposes

PCB_t process;			// the only user process used for batch and FCFS
PCB_t* pNewProcess;		// pointer for new process read from batch


/* ---------------------------------------------------------------- */
/*                Declarations of local helper functions            */


/* ---------------------------------------------------------------- */
/*                Externally available functions                    */
/* ---------------------------------------------------------------- */

void initOS(void)
{
	/* init the status of the OS */
	// mark all process entries invalid
	//TODO sollte unötig sein, alle Prozesse sind eh schon false...
	for (unsigned i = 0; i < MAX_PROCESSES; i++) processTable[i].valid = FALSE;
	process.pid = 0; // reset pid

	//Vielleicht Malloc?
	initMemory();
	logInit("Memory has been initialized!");
}

void coreLoop(void)
{
	SchedulingEvent_t nextEvent;		// scheduling event to process
	unsigned int eventPid;				// pid of a process triggering an event
	PCB_t* candidateProcess = NULL;		// next process to start, already stored in process table
	PCB_t* nextReady = NULL;			// pointer to process that finishes next	
	Boolean isLaunchable = FALSE;		// indicates if the next process in batch is launchable 

	do
	{
		// loop until no more process to run 
		do
		{
			// loop until no currently launchable process is in the batch file
			if (checkForProcessInBatch())
			{

				// there is a process pending
				// predicate if the process is ready to be started is processed by the simulation: 
				if (isNewProcessReady()) // test if the process is ready to be started
				{
					// the process is ready to be started
					isLaunchable = TRUE;
					pid_t newPid = getNextPid();							// Get next valid pid
					initNewProcess(newPid, getNewPCBptr());					// Info on new process provided by simulation

					if (processTable[newPid].size > MEMORY_SIZE)			// Checks if process is too big
					{
						logError("(core) A process is bigger than the memory and can't be loaded!");
						logPid(processTable[newPid].pid, "The process with be deleted!");
						deleteProcess(&processTable[newPid]);				// Process is being deleted from the processTable
						displayMemory();									// Memory is being displayed
						displayQ();											// Queue is being displayed
					}
					else if ((MEMORY_SIZE - usedMemory) >= processTable[newPid].size)		// Checks if memory has enough free space, if false puts process in queue
					{
						if (!(firstFit(&processTable[newPid])))								// Checks if allocation of memory via firstFit successful the first time, compacts and retries if false
						{
							compacting();
							systemTime = systemTime + COMPACT_TIME;							// Adds the compaction time to systemTime
							logGeneric("Successfully compacted memory.");
							displayMemory();
							firstFit(&processTable[newPid]);
						}
						processTable[newPid].status = running;				// Sets process as running
						systemTime = systemTime + LOADING_DURATION;			// Adds Loading time to systemTime
						runningCount = runningCount + 1;					// Increments the running count
						flagNewProcessStarted();							// Deletes and resets the candidateProcess
						
						logPidMem(newPid, "Process started and memory allocated.");
						displayMemory();									// Displays the memory
					}
					else
					{
						logPidMem(newPid, "Process will be inserted into the queue.");
						enqueue(&processTable[newPid]);						// Adds a process to the queue
						processTable[newPid].status = blocked;				// Sets the process status as blocked
						waitingCount = waitingCount + 1;					// Increments the waiting count
					}
				}
				else
				{
					isLaunchable = FALSE;									// Sets flag isLaunchable FALSE, indicates that the process can't run yet
					logRdyToRun("Process read but it is not yet ready to run.");
				}
			}
			// In case the candidate was started, check for another candidate before running the processes
		}
		while ((!batchComplete) && (isLaunchable));

		// The checks for starting a process are done. Now work on the running processes
		// get the process that will be completed next by searching the process table
		unsigned delta = runToNextEvent(&nextEvent, &eventPid);
		// run the existing processes until a new arrives or one terminates

		//Now update OS-stats 
		updateAllVirtualTimes(delta); // update all processes according to elapsed time

		// all running processes are updated
		systemTime = systemTime + delta; // update system time by elapsed physical time
		// now quit the completed process (if any)
		
		if (nextEvent == completed) // check if a process needs to be terminated
		{
			setFinish(eventPid);												// Sets a process as finished in the MemoryList
			logPidMem(eventPid, "Process terminated, memory freed.");
			deleteProcess(&processTable[eventPid]);								// Deletes process from the ProcessTable
			runningCount--;														// Decrements the running count

			displayMemory();													// memory is being displayed
			displayQ();															// queue is being displayed

			if (doseNextQFit())													// After memory has been deallocated, checks if head of the queue fits into the free memory
			{
				eventPid = dequeue();											// gets PID from head of queue and deletes head of queue, sets next entry head of queue
				displayQ();														// queue is being displayed
				waitingCount = waitingCount - 1;								// Decrements the waiting count

				if (!firstFit(&processTable[eventPid]))							// Checks if allocation of memory via firstFit successful the first time, compacts and retries if false
				{
					compacting();											// Compacts the memory
					systemTime = systemTime + COMPACT_TIME;
					logGeneric("Successfully compacted memory.");
					displayMemory();
					firstFit(&processTable[eventPid]);
				}
				processTable[eventPid].status = running;						// Sets process as running
				runningCount = runningCount + 1;								// Increments the running count

				//Dieser log muss noch verschoben werden
				logPidMem(eventPid, "Process started and memory allocated");
				displayMemory();												// memory is being displayed
			}
		}
		// loop until no running processes exist any more and no process is waiting t be started
	}
	while ((runningCount > 0) || (batchComplete == FALSE) || (waitingCount > 0)); // loops the do-while loop until no process is running or waiting and the batch is complete
}

/* ----------------------------------------------------------------- */
unsigned getNextPid()
{
	static unsigned pidCounter = 1;
	unsigned i = 0; // iteration variable;
	// determine next available pid make sure not to search infinitely
	while ((processTable[pidCounter].valid) && (i < MAX_PID))
	{
		// determine next available pid 
		pidCounter = (pidCounter + 1) % MAX_PID;
		if (pidCounter == 0) pidCounter++; // pid=0 is invalid
		i++; // count the checked entries
	}
	if (i == MAX_PID) return 0; // indicate error
	else return pidCounter;
}

int initNewProcess(pid_t newPid, PCB_t* pProcess)
/* Initialized the PCB at the given index of the process table with the		*/
/* process information provided in the PCB-struct given by the pointer		*/
{
	if (pProcess == NULL)
		return 0;
	else
	{
		/* PCB correctly passed, now delete it */
		processTable[newPid].pid = newPid;
		processTable[newPid].ppid = pProcess->ppid;
		processTable[newPid].ownerID = pProcess->ownerID;
		processTable[newPid].start = pProcess->start;
		processTable[newPid].duration = pProcess->duration;
		processTable[newPid].size = pProcess->size;
		processTable[newPid].usedCPU = pProcess->usedCPU;
		processTable[newPid].type = pProcess->type;
		processTable[newPid].status = init;
		processTable[newPid].valid = TRUE;
		// new process is initialized, now invalidate the PCB passed to this function
		pProcess->valid = FALSE;
		return 1;
	}
}

int deleteProcess(PCB_t* pProcess)
/* Voids the PCB handed over in pProcess, this includes setting the valid-	*/
/* flag to invalid and setting other values to invalid values.				*/
/* retuns 0 on error and 1 on success										*/
{
	if (pProcess == NULL)
		return 0;
	else
	{
		/* PCB correctly passed, now delete it */
		pProcess->valid = FALSE;
		pProcess->pid = 0;
		pProcess->ppid = 0;
		pProcess->ownerID = 0;
		pProcess->start = 0;
		pProcess->duration = 0;
		pProcess->size = 0;
		pProcess->usedCPU = 0;
		pProcess->type = os;
		pProcess->status = ended;
		return 1;
	}
}


/* ----------------------------------------------------------------- */
/*                       Local helper functions                      */
/* ----------------------------------------------------------------- */
