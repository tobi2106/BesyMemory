#include <stdio.h>
#include <stdlib.h>
#include "bs_types.h"
#include "globals.h"
#include "core.h"
#include "MemoryList.h"
#include "log.h"

struct MEMORY* head = NULL;
struct MEMORY* tail = NULL;

// Checks if the list is empty
Boolean isEmpty()
{
	return head == NULL;
}

// Returns the length of the list
int length()
{
	int length = 0;
	struct MEMORY* current;

	for (current = head; current != NULL; current = current->prev)
	{
		length++;
	}
	return length;

	return -1;
}

// Prints out the whole list. Free memory is being marked Green
void displayMemory()
{
	if (isEmpty()) logError("(Display) The memory is NULL!");			// Checks if head is empty. Head should never be empty and prints out an error on true
	else
	{
		struct MEMORY* ptr = head;
		printf("\n[MEM] \t%u \t: [", systemTime);
		while (ptr != NULL)
		{
			if (ptr->isMemoryFree) printf(GRN"(%d, %u)"RESET, ptr->memoryPointer, ptr->elementSize);
			else printf("(<%u> %d, %u)", ptr->prozessInfo->pid, ptr->memoryPointer, ptr->elementSize);
			ptr = ptr->prev;
		}
		printf("]\n");
	}
}

// Allocates a process into the memory
Boolean firstFit(PCB_t* process) {
	struct MEMORY* link = malloc(sizeof(struct MEMORY));
	struct MEMORY* space = malloc(sizeof(struct MEMORY));
	MEMORY* current = head;

	unsigned diff = 0;

	link->prozessInfo = process;
	link->isMemoryFree = FALSE;
	link->memoryPointer = 0;
	link->elementSize = process->size;
	link->next = NULL;
	link->prev = NULL;

	do				// Loops through the List
	{
		if (current->isMemoryFree == TRUE)			// Checks if the current entry is free Memory
		{
			//Prozess passt genau in die Lücke rein
			if (current->elementSize == link->prozessInfo->size)		// Checks if the process is exactly the free memory, replaces the free memory completely if true
			{
				usedMemory += link->prozessInfo->size;

				link->next = current->next;
				link->prev = current->prev;

				// Sets the pointers of the entries before and/or after the allocated entry
				if (current == head)									// checks for edge cases
				{
					head->prev->next = link;
					head = link;
				}
				else if (current == tail)
				{
					tail->next->prev = link;
					tail = link;
				}
				else													
				{
					current->next->prev = link;
					current->prev->next = link;
					current = link;
				}
				return TRUE;
			}
			// Checks if the free memory is bigger than the process, generates a smaller free memory element behind the process 
			else if (current->elementSize >= link->prozessInfo->size)
			{
				space->next = NULL;
				space->prev = NULL;
				usedMemory += link->prozessInfo->size;

				// calculates the difference of size between the process and the free memory
				diff = current->elementSize - link->prozessInfo->size;

				// Generates a new free memory element with the difference as it's size
				space->prozessInfo = NULL;
				space->elementSize = diff;
				space->isMemoryFree = TRUE;

				// Calculates where the pointer of the free memory is
				space->memoryPointer = current->memoryPointer + link->elementSize;
				link->memoryPointer = current->memoryPointer;

				// allocation of the process into the list
				// inserts process into head
				if (current == head)
				{
					if (head->prev == NULL) tail = space;
					else head->prev->next = space;

					space->next = link;
					link->prev = space;

					space->prev = head->prev;
					head = link;
					return TRUE;
				}
				// Inserts process into tail
				else if (current == tail)
				{
					space->next = link;
					link->prev = space;

					link->next = current->next;
					current->next->prev = link;
					tail = space;
					return TRUE;
				}
				// Inserts process at the location where the free space was
				else
				{
					space->next = link;
					link->prev = space;

					current->prev->next = space;
					space->prev = current->prev;

					current->next->prev = link;
					link->next = current->next;

					current = link;

					return TRUE;
				}
			}
		}
		// The iterated item is not a free memory element
		current = current->prev;
	} 	while (current != NULL);
	logGeneric("The free memory is big enough, but the process can't be loaded in due to fragmentation.");
	logGeneric("Will compact the memory now and try to insert the process again.");
	return FALSE;
}

// Initializes the memory list and applies a free memory element
void initMemory()
{
	// create a link
	struct MEMORY* link = malloc(sizeof(struct MEMORY));
	link->isMemoryFree = TRUE;
	link->prozessInfo = NULL;
	link->memoryPointer = 0;
	link->elementSize = MEMORY_SIZE;

	link->prev = tail;
	link->next = NULL;

	head = link;
}

// deletes an element from the list
void delete(MEMORY* current)
{
	if (current == head) head = head->prev;			// change first to point to next link
	else current->next->prev = current->prev; 		// bypass the current link

	if (current == tail) tail = current->next;		// change last to point to prev link
	else current->prev->next = current->next;

	deleteProcess(current->prozessInfo);
}

// Checks if there are free memory elements right next to the current free memory object and merges them
void merge(MEMORY* current)
{
	struct MEMORY* temp = NULL;

	if (length() == 1) return;		// Returns if the list has only one entry

	if (current == tail)			// Only checks for the previous entry if the current element is the tail
	{
		if (current->next->isMemoryFree)			// merges the previous element with the current element if the previous element is free memory
		{
			temp = current->next;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			delete((struct MEMORY*)current);
		}
	}
	else if (current == head)		// Only checks for the next entry if the current element is the head
	{
		if (current->prev->isMemoryFree)			// merges the next element with the current element if the previous element is free memory
		{
			temp = current->prev;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			temp->memoryPointer = current->memoryPointer;
			//delete(current->key);
			delete((struct MEMORY*)current);
		}
	}
	else							// Checks if the next and previous entries are free Memory
	{
		if ((current->next->isMemoryFree) && (current->prev->isMemoryFree))		// checks if the previous and next element are free memory
		{
			temp = current->next;

			temp->elementSize += current->prev->elementSize;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			//löscht Prozess vor Current und Current selbst.
			delete((struct MEMORY*)current);
			delete((struct MEMORY*)current->prev);
		}
		else if (current->next->isMemoryFree)									// checks if the previous element is free memory
		{
			temp = current->next;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			delete((struct MEMORY*)current);
		}
		else if (current->prev->isMemoryFree)									// checks if the next element is free memory
		{
			temp = current;
			temp->elementSize = temp->elementSize + current->prev->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->prev->elementSize;
			delete((struct MEMORY*)current->prev);
		}
	}
}

// Sets a process in the memory as finished
void setFinish(unsigned processID)
{
	struct MEMORY* current = head;
	struct MEMORY* next = NULL;

	while (current->prev != NULL)
	{
		if (current->prozessInfo != NULL)
		{
			if (current->prozessInfo->pid == processID) break;
		}
		if (current->prev == NULL)
		{
			logPid("(setFinish) Error!Die PID % d konnte nicht gefunden werden", processID);
			return;
		}
		else
		{
			current = current->prev;
		}
	}

	if (!(current->isMemoryFree))
	{
		current->isMemoryFree = TRUE;
		usedMemory -= current->elementSize;

		merge((struct MEMORY*)current);
	}
	else logError("(setFinish) Der Prozess konnte im Memory nicht Deallokiert werden");
}

// compacts the whole list and merges all free memory elements to one free memory element
void compacting()
{
	struct MEMORY* current = head;
	struct MEMORY* temp = NULL;
	struct MEMORY* tempNext = NULL;

	unsigned int resetMemoryPointer = 0;

	while (current != NULL)
	{
		if (current->isMemoryFree)
		{
			tempNext = current->prev;
			temp = current;

			delete((struct MEMORY*)current);

			temp->next = tail;
			temp->prev = tail->prev;
			tail->prev = temp;
			tail = temp;
			merge(tail);

			current = tempNext;
		}
		else
		{
			current->memoryPointer = resetMemoryPointer;
			resetMemoryPointer = resetMemoryPointer + current->elementSize;
			current = current->prev;
		}
	}
	tail->memoryPointer = resetMemoryPointer;
}