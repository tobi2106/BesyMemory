/* Implementation of the loader. Responsible for reading batch,		*/
/* generating and destroying of processes  */
/* for comments on the functions see the associated .h-file */

#define	_CRT_SECURE_NO_WARNINGS		// suppress legacy warnings 

/* ---------------------------------------------------------------- */
/* Include required external definitions */
#include <math.h>
#include "loader.h"
#include "bs_types.h"
#include "globals.h"
#include "log.h"

/* ----------------------------------------------------------------	*/
/* Declarations of global variables visible only in this file 		*/
extern PCB_t candidateProcess; // only for simulation purposes

/* ---------------------------------------------------------------- */
/*                Declarations of local helper functions            */
// none

/* ---------------------------------------------------------------- */
/*                Externally available functions                    */
/* ---------------------------------------------------------------- */

FILE* openConfigFile(FILE* file, const char* filename)
{
	char linebuffer[129] = "x";								// read buffer for file-input
	char charbuffer = 'x';									// read buffer for file-input
	unsigned ownerID = 0xFF, start = 0xFF, duration = 0xFF; // local buffer for reading precess properties
	char ownerIDStr[21] = "", startStr[21] = "";			// buffers for reading process type-string
	char durationStr[21] = "", processTypeStr[21] = "";		// buffers for reading process type-string

	// try to open file for read
	file = fopen(filename, "r");
	
	// test for success and error handling
	if (file == NULL)
	{
		logError("Trying to open the file with process information." RESET);
		printf(RED "[ERROR] Can not find File: '%s'\n" RESET, *&filename);
		return NULL;
	}
	else
	{
		// skip first line, only a comment
		// feof() checks if there is still data on a stream or if the end-of-file indicator is set
		if (!feof(file))
		{
			fgets(linebuffer, 128, file);
		}
		printf(CYN "[FILE] Reading from File %s: %s" RESET, *&filename, linebuffer);
		logGeneric("Process info file opened");
		return file;
	}
}


Boolean readNextProcess(FILE* f, PCB_t* pProcess)
{
	char linebuffer[129] = "";		// read buffer for file-input
	char processTypeStr[21] = "";	// buffers for reading process type-string
	int readOK;
	
	if (f == NULL)
	{
		logError("File handle not initialized");
		return FALSE;				// error: file handle not initialized
	}
		
	if (feof(f))
	{
		fclose(f);					// close the file on reaching EOF
		// TODO This might not be an error
		logError("reached EOF of file");
		return FALSE;				// error occurred (EOF reached)
	}
	else
	{
		fgets(linebuffer, 128, f);
		if (strcmp(linebuffer, "") == 0)
		{
			logError("Line in file is empty");
			return FALSE;			// error occurred: line is empty
		}
		else
		{
			readOK = sscanf(linebuffer, "%u %u %u %u %s", 
				&pProcess->ownerID, &pProcess->start, &pProcess->duration, &pProcess->size, processTypeStr);

			if (strcmp(processTypeStr, "os") == 0) pProcess->type = os;
			else if (strcmp(processTypeStr, "interactive") == 0) pProcess->type = interactive;
			else if (strcmp(processTypeStr, "batch") == 0) pProcess->type = batch;
			else if (strcmp(processTypeStr, "background") == 0) pProcess->type = background;
			else if (strcmp(processTypeStr, "foreground") == 0) pProcess->type = foreground;
			// TODO Adding an error, if no ProcessType is found
			else pProcess->type = os;
		}
		// pProcess->pid=getNextPid();		-> moved to core loop //TODO why this command?
		logProRead(pProcess->ownerID, pProcess->start, pProcess->duration, pProcess->size, processTypeStr);
	}
	return TRUE;
}

Boolean checkForProcessInBatch()
{
	// select and run a process
	if (!candidateProcess.valid)		// no candidate read from file yet
	{
		logGeneric("Reading next process from file");
		// read the next process for the file and store in process table
		candidateProcess.valid = readNextProcess(processFile, &candidateProcess);
		if (candidateProcess.valid)
		{
			// there are still jobs listed in the file
			logGeneric("Next process loaded from file");
		}
		else // no more processes to be started 
		{
			logGeneric("No further process listed for execution.");
			batchComplete = TRUE;
		}
	}
	return candidateProcess.valid;
}

PCB_t* getNewPCBptr(void)
/* returns a pointer to the PCB of the process to start */
{
	return &candidateProcess;
}
