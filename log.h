/* header-file defining the interface of the scheduler */
/* the scheduler contains all functions required to decide which process	*/
/* to run next																*/
/* +++	this version only implements a batch-scheduling using FIFO without	*/
/* +++	SPOOLING. It needs to be extended to support multiprogramming		*/

#ifndef __LOG__
#define __LOG__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bs_types.h"
#include "globals.h"
#include "loader.h"


void logGeneric(char* message);
/* print the given general string to stdout and/or a log file 				*/
/* The time stamp (systemTime) will be prepended automatically				*/
/* the linefeed is automatically appended									*/

void logInit(char* message);
/* Log for System inits. The message will be green.                         */
/* print the given general string to stdout and/or a log file 				*/
/* The time stamp (systemTime) will be prepended automatically				*/
/* the linefeed is automatically appended.				                    */

void logError(char* message);
/* print the given general string to stdout and/or a log file 				*/
/* The time stamp (systemTime) will be prepended automatically				*/
/* the linefeed is automatically appended									*/

void logPid(unsigned pid, char* message);
/* print the given string to stdout and/or a log file 						*/
/* The time stamp (systemTime) and the  PID are prepended automatically		*/
/* the linefeed is automatically appended									*/

void logPidCompleteness(unsigned pid, unsigned done, unsigned length,
                        char* message);
/* print the given string to stdout and/or a log file 						*/
/* The time stamp (systemTime) and the  PID are prepended automatically		*/
/* the fraction of used CPU over process duration is also prepended		    */
/* the linefeed is automatically appended									*/

void logPidMem(unsigned pid, char* message);
/* print the given string to stdout and/or a log file 						*/
/* The time stamp (systemTime) and the  PID are prepended automatically		*/
/* the currently used amount of physical memory is also prepended		    */
/* the linefeed is automatically appended									*/

void logProRead(unsigned ownerID, unsigned start, unsigned duration, unsigned size, char* pType);
/* print the information about the next process in file                     */
/* The time stamp (systemTime) and the  PID are prepended automatically		*/

void logRdyToRun(char* message);
/* print the information about the next process in file                     */
/* The time stamp (systemTime) and the  PID are prepended automatically		*/
/* the remaining time until the process is ready to run                     */

void logMem(char* message);
/* print the Memory out as a List. Green are free memory.                   */
/* The time stamp (systemTime) is printed out automatically                 */

void logQ(char* message);
/* print the Queue out as a List.                                           */
/* The time stamp (systemTime) is printed out automatically                 */

#endif /* __LOG__ */
