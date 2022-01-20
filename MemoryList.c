#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bs_types.h"
#include "globals.h"
#include "loader.h"
#include "core.h"
#include "executer.h"
#include "Queue.h"
#include "MemoryList.h"

struct MEMORY* head = NULL;
struct MEMORY* tail = NULL;

//Gibt an, ob die Liste ein Eintrag enthält
Boolean isEmpty()
{
	return head == NULL;
}

//Gibt die länge der Liste zurück, auch mit Head
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

//Printet alle Einträge aus der Liste ab. Geht von Tail nach Head. 
//Wenn nur Head drin ist, wird nur Head geprintet. Head ist Blau makiert.
void displayMemory()
{
	//Die Abfrage stimmt noch nicht ganz
	if (isEmpty()) logError("(Display) The memory is NULL!");
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

Boolean firstFit(PCB_t* prozessInfo) {
	struct MEMORY* link = malloc(sizeof(struct MEMORY));
	struct MEMORY* space = malloc(sizeof(struct MEMORY));
	MEMORY* current = head;

	unsigned diff = 0;

	link->prozessInfo = prozessInfo;
	link->isMemoryFree = FALSE;
	link->memoryPointer = 0;
	link->elementSize = prozessInfo->size;
	link->next = NULL;
	link->prev = NULL;

	do
	{
		if (current->isMemoryFree == TRUE)
		{
			//Prozess passt genau in die Lücke rein
			if (current->elementSize == link->prozessInfo->size)
			{
				usedMemory += link->prozessInfo->size;

				link->next = current->next;
				link->prev = current->prev;

				if (current == head)
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
			//Lücke ist zu Groß. Es muss ein neues Lückenelement erstellt werden.
			else if (current->elementSize >= link->prozessInfo->size)
			{
				space->next = NULL;
				space->prev = NULL;
				usedMemory += link->prozessInfo->size;

				//X = 500 - 100
				//diff = 400
				diff = current->elementSize - link->prozessInfo->size;

				//Verlust von 100 Speicher, dafür muss ein Leerer Prozess hinzugefügt werden
				//Erstelle leeren Prozess, daher key->-1;
				//TODO Seine Delete Methode sollte gut genug sein tbh.
				space->prozessInfo = NULL;
				space->elementSize = diff;
				space->isMemoryFree = TRUE;

				//Current Pointer: 99. Link ist 100 gross. Also begint die Lücke bei 199 + 1
				space->memoryPointer = current->memoryPointer + link->elementSize;
				link->memoryPointer = current->memoryPointer;

				//temp zwischen Link und seinem Prev einfügen.
				//Lücke am Head
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
				//Lücke am Tail
				else if (current == tail)
				{
					space->next = link;
					link->prev = space;

					link->next = current->next;
					current->next->prev = link;
					tail = space;
					return TRUE;
				}
				//Lücke in der Liste
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
		//Der Prozess ist keine Lücke
		current = current->prev;
	} 	while (current != NULL);
	logGeneric("The free memory is big enough, but the process can't be loaded in due to fragmentation.");
	logGeneric("Will compact the memory now and try to insert the process again.");
	return FALSE;
}

//Setzt den Head. Mehr auch nicht
void initMemory()
{
	//create a link
	struct MEMORY* link = malloc(sizeof(struct MEMORY));
	link->isMemoryFree = TRUE;
	link->prozessInfo = NULL;
	link->memoryPointer = 0;
	link->elementSize = MEMORY_SIZE;

	link->prev = tail;
	link->next = NULL;

	head = link;
}

//Same as delete, but in Fast
void delete(MEMORY* current)
{
	if (current == head) head = head->prev;			//change first to point to next link
	else current->next->prev = current->prev; 		//bypass the current link

	if (current == tail) tail = current->next;		//change last to point to prev link
	else current->prev->next = current->next;

	deleteProcess(current->prozessInfo);
}

//Defragmentiert die List. Sobald ein Eintrag fertig ist, wird überprüft ob mehrere leere Prozesse nebeneinander stehen.
void merge(MEMORY* current)
{
	struct MEMORY* temp = NULL;

	if (length() == 1) return;

	if (current == tail)
	{
		if (current->next->isMemoryFree)
		{
			temp = current->next;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			delete((struct MEMORY*)current);
		}
	}
	//If Head
	else if (current == head)
	{
		if (current->prev->isMemoryFree)
		{
			temp = current->prev;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			temp->memoryPointer = current->memoryPointer;
			//delete(current->key);
			delete((struct MEMORY*)current);
		}
	}
	else
	{
		if ((current->next->isMemoryFree) && (current->prev->isMemoryFree))
		{
			temp = current->next;

			temp->elementSize += current->prev->elementSize;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			//löscht Prozess vor Current und Current selbst.
			delete((struct MEMORY*)current);
			delete((struct MEMORY*)current->prev);
		}
		else if (current->next->isMemoryFree)
		{
			temp = current->next;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			delete((struct MEMORY*)current);
		}
		else if (current->prev->isMemoryFree)
		{
			temp = current;
			temp->elementSize = temp->elementSize + current->prev->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->prev->elementSize;
			delete((struct MEMORY*)current->prev);
		}
	}
}

//Das Argument isFree vom Prozess wird auf True gesetzt 
//Mit defragmentierung2
void setFinish(unsigned pid)
{
	struct MEMORY* current = head;
	struct MEMORY* next = NULL;

	while (current->prev != NULL)
	{
		if (current->prozessInfo != NULL)
		{
			if (current->prozessInfo->pid == pid) break;
		}
		if (current->prev == NULL)
		{
			logPid("(setFinish) Error!Die PID % d konnte nicht gefunden werden", pid);
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

//Alle Prozesse die fertig sind, 
//werden erneut an die Liste hinten rangesetzt und darauf wird der erste Eintrag gelöscht.
void kompaktierung()
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