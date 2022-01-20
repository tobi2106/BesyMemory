/* Include-file defining functions of the OS-core 	 		*/
#ifndef __CORE__
#define __CORE__

void initOS(void);
/* all initialization steps are started in this function			*/
/* +++ this must be extended for multiprogramming					*/

void coreLoop(void);
/* the core scheduling loop of the OS								*/
/* +++ this must be extended for multiprogramming					*/

unsigned getNextPid();
/* returns the next available pid.									*/
/* returns 0 in case of an error like a fully occupied process-table*/ //Ich möchte das zu -1 machen. 
/* +++ this must be replaced for multiprogramming					*/

int initNewProcess(pid_t newPid, PCB_t* pProcess);
/* Initialized the PCB at the given index of the process table with the		*/
/* process information provided in the PCB-struct given by the pointer		*/

int deleteProcess(PCB_t* pProcess);
/* Voids the PCB handed over in pProcess, this includes setting the valid-	*/
/* flag to invalid and setting other values to invalid values.				*/
/* returns 0 on error and 1 on success										*/


#endif /* __CORE__ */
