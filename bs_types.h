/* Include-file defining elementary data types used by the 			*/
/* operating system */
#ifndef __BS_TYPES__
#define __BS_TYPES__

typedef enum { FALSE = 0, TRUE } Boolean;

//typedef unsigned int pid_t;
typedef int pid_t;

/* data type for the possible types of processes */
/* the process type determines the IO-characteristic */
typedef enum
{
	os,
	interactive,
	batch,
	background,
	foreground
} ProcessType_t;

/* data type for the process-states used for scheduling and life-	*/
/* cycle management of the processes 								*/
typedef enum
{
	init,
	running,
	ready,
	blocked,
	ended
} Status_t;

/* data type for the different events that cause the scheduler to	*/
/* become active */
typedef enum
{
	completed,
	io,
	quantumOver,
	start,
	none
} SchedulingEvent_t;

/* data type for the Process Control Block */
/* +++ this might need to be extended to support future features	*/
/* like additional schedulers */
struct PCB_t
{
	Boolean valid;
	pid_t pid;
	pid_t ppid;
	unsigned ownerID;
	unsigned start;
	unsigned duration;
	unsigned size;
	unsigned memoryPointer;
	unsigned usedCPU;
	ProcessType_t type;
	Status_t status;
};
typedef struct PCB_t PCB_t;

struct MEMORY
{
	Boolean freeMemory;
	struct MEMORY* next;
	struct MEMORY* prev;
	struct PCB_t* prozessInfo;
};
typedef struct MEMORY MEMORY;

struct queue
{
	struct MEMORY* data;
	struct queue* next;
};
typedef struct queue queue;

#endif  /* __BS_TYPES__ */
