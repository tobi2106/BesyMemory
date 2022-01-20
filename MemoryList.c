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
	if(isEmpty()) 
	{
		printf("[Display] Error! Die Liste ist leer");
	}
	else
	{
		struct MEMORY* ptr = head;
		printf("\n[");
		while (ptr != NULL)
		{
			printf("(%d, %d, %u)", ptr->isMemoryFree, ptr->memoryPointer, ptr->elementSize);
			ptr = ptr->prev;
		}
		printf("]\n");
	}
}

Boolean insertLast(PCB_t* prozessInfo) {
	struct MEMORY* link = malloc(sizeof(struct MEMORY));
	struct MEMORY* space = malloc(sizeof(struct MEMORY));
	MEMORY* current = head;

	unsigned diff = 0;

	link->prozessInfo = prozessInfo;
	link->elementSize = prozessInfo->size;
	link->isMemoryFree = FALSE;
	link->memoryPointer = 0;
	link->next = NULL;
	link->prev = NULL;

	if (MEMORY_SIZE - usedMemory >= link->elementSize) 
	{
		do 
		{
			if (current->isMemoryFree == TRUE)
			{
				//Prozess passt genau in die Lücke rein
				if (current->elementSize == link->prozessInfo->size)
				{
					usedMemory += link->prozessInfo->size;
					current = link;
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
						if (head->prev == NULL)
						{
							tail = space;
						}
						else
						{
							head->prev->next = space;
						}
						space->next = link;
						link->prev = space;

						space->prev = head->prev;
						head = link;
						return TRUE;
					}
					//Lücke am Tail
					else if(current == tail)
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
						//Unsicher ob der richtig ist
						space->prev = current->prev;
						current->prev->next = space;

						space->next = current;
						current->prev = space;

						current = link;
						return TRUE;
					}
				}
			}
			//Der Prozess ist keine Lücke
			current = current->prev;
		} 
		while (current != NULL);
		printf("Speicher hat genügend Platz, Prozess kann aber nicht hinzugefügt werden.\n");
		printf("Versuche zu Kompaktieren und Prozess erneut in den Speicher zu schreiben.!\n");
		return FALSE;
	}
	else 
	{
		printf("\nDer Speicher ist zu klein!\n");
		return FALSE;
	}
}

//Setzt den Head. Mehr auch nicht
void setHead()
{
	if (head != NULL)
	{
		printf("[insertFirst] Error! Es gibt bereits einen Head!");
	}
	else
	{
		//create a link
		struct MEMORY* link = (struct MEMORY*)malloc(sizeof(struct MEMORY));
		link->isMemoryFree = TRUE;
		link->prozessInfo = NULL;
		link->memoryPointer = 0;
		link->elementSize = MEMORY_SIZE;

		link->prev = tail;
		link->next = NULL;

		head = link;
	}
}

//Same as delete, but in Fast
void delete(MEMORY* current)
{
	//found a match, update the link
	if (current == head) {
		//change first to point to next link
		head = head->prev;
	}
	else {
		//bypass the current link
		current->next->prev = current->prev;
	}

	if (current == tail) {
		//change last to point to prev link
		tail = current->next;
	}
	else {
		current->prev->next = current->next;
	}
	deleteProcess(current->prozessInfo);
}

//Defragmentiert die List. Sobald ein Eintrag fertig ist, wird überprüft ob mehrere leere Prozesse nebeneinander stehen.
void merge(MEMORY* current)
{
	struct MEMORY* temp = NULL;
	unsigned l = length();

	if (l == 1)
	{
		printf("[merge] Error! Es gibt nur ein Prozess");
		return;
	}
	//If Last
	if (current == tail)
	{
		if (current->next->isMemoryFree == TRUE)
		{
			temp = current->next;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			delete((struct MEMORY*)current);
		}
	}
	//If Head
	else if (current == head)
	{
		if (current->prev->isMemoryFree == TRUE)
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
		if (current->next->isMemoryFree == TRUE && current->prev->isMemoryFree == TRUE)
		{
			temp = current->next;

			temp->elementSize += current->prev->elementSize;
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			//löscht Prozess vor Current und Current selbst.
			delete((struct MEMORY*)current);
			delete((struct MEMORY*)current->prev);
		}
		else if (current->next->isMemoryFree == TRUE)
		{
			temp = current->next; 
			temp->elementSize = temp->elementSize + current->elementSize > MEMORY_SIZE ? MEMORY_SIZE : temp->elementSize + current->elementSize;
			delete((struct MEMORY*)current);
		}
		else if (current->prev->isMemoryFree == TRUE)
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

	if (current == NULL)
	{
		printf("[setFinish] Error! Es gibt keine Prozesse, der Prozess mit der PID: %d kann nicht gefunden werden!", pid);
		return;
	}
	else
	{
		while (current->prev != NULL)
		{
			if (current->prozessInfo != NULL)
			{
				if (current->prozessInfo->pid == pid) break;
			}
			if (current->prev == NULL)
			{
				printf("[setFinish] Error! Die PID %d konnte nicht gefunden werden", pid);
				return;
			}
			else
			{
				current = current->prev;
			}
		}

		if (current->isMemoryFree != TRUE)
		{
			current->isMemoryFree = TRUE;
			usedMemory -= current->elementSize;

			merge((struct MEMORY*) current);
		}
		else
		{
			printf("\n[setFinish] error! Konnte current nicht auf Free stellen!");
		}
	}
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
		if (current->isMemoryFree == TRUE)
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

int mainMemory()
{
	/*
	usedMemory = 0;

	PCB_t* p1 = malloc(sizeof(struct PCB_t));
	p1->pid = 1;
	p1->size = 101;
	p1->valid = TRUE;
	p1->status = ready;

	PCB_t* p2 = malloc(sizeof(struct PCB_t));
	p2->pid = 2;
	p2->size = 102;
	p2->valid = TRUE;
	p2->status = ready;

	PCB_t* p3 = malloc(sizeof(struct PCB_t));
	p3->pid = 3;
	p3->size = 103;
	p3->valid = TRUE;
	p3->status = ready;

	PCB_t* p4 = malloc(sizeof(struct PCB_t));
	p4->pid = 4;
	p4->size = 104;
	p4->valid = TRUE;
	p4->status = ready;

	PCB_t* p5 = malloc(sizeof(struct PCB_t));
	p5->pid = 5;
	p5->size = 105;
	p5->valid = TRUE;
	p5->status = ready;

	PCB_t* p6 = malloc(sizeof(struct PCB_t));
	p6->pid = 6;
	p6->size = 106;
	p6->valid = TRUE;
	p6->status = ready;

	PCB_t* p7 = malloc(sizeof(struct PCB_t));
	p7->pid = 7;
	p7->size = 107;
	p7->valid = TRUE;
	p7->status = ready;

	PCB_t* p8 = malloc(sizeof(struct PCB_t));
	p8->pid = 8;
	p8->size = 108;
	p8->valid = TRUE;
	p8->status = ready;

	PCB_t* p9 = malloc(sizeof(struct PCB_t));
	p9->pid = 9;
	p9->size = 109;
	p9->valid = TRUE;
	p9->status = ready;

	setHead();
	displayMemory();

	insertLast(p1);
	displayMemory();

	insertLast(p2);
	displayMemory();

	insertLast(p3);
	displayMemory();

	insertLast(p4);
	displayMemory();

	insertLast(p5);
	displayMemory();

	insertLast(p6);
	displayMemory();

	insertLast(p7);
	displayMemory();

	insertLast(p8);
	displayMemory();

	insertLast(p9);
	displayMemory();
	*/
	return 1;
}