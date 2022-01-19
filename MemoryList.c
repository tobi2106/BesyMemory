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
struct MEMORY* current = NULL;

unsigned freeMemorySize = MEMORY_SIZE;


//Gibt an, ob die Liste ein Eintrag enthält
Boolean isEmpty()
{
	return head == NULL;
}

//Gibt die länge der Liste zurück, auch mit Head
int length()
{
	if (head == NULL && tail == NULL)
	{
		printf("[length] Error! Head und Tail sind NULL");
		return -1;
	} 
	else
	{
		int length = 0;
		struct MEMORY* current;

		for (current = head; current != NULL; current = current->prev)
		{
			length++;
		}
		return length;
	}
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
	else if (tail == NULL)
	{
		printf(CYN"[(%d, %u, %u)]\n"RESET, head->freeMemory, head->prozessInfo->pid, head->prozessInfo->size);
		return;
	}
	else
	{
		struct MEMORY* ptr = tail;
		printf("\n[");
		while (ptr != NULL && ptr != head)
		{
			printf("(%d, %u, %u)", ptr->freeMemory, ptr->prozessInfo->pid, ptr->prozessInfo->size);
			ptr = ptr->next;
		}
		if (ptr == head)
		{
			printf(CYN"(%d, %u, %u)"RESET, ptr->freeMemory, ptr->prozessInfo->pid, ptr->prozessInfo->size);
		}
		printf("]\n");
	}
}

//FirstFit
/*Boolean firstFit(MEMORY* link)
{
	//Abfragen, ob es einen head gibt. Eigentlich muss es immer einen Head geben... Mal sehen, ob ich die Abfrage rausnehmen werde. Denke ich ja.
	struct MEMORY* current = tail;
	struct MEMORY* temp = NULL;
	unsigned diff = 0;

	//Die While benötigt man nicht, wenn man First Fit iweo einbaut wo bereits durch die Liste durchiteriert wird
	while (current->next != head && current != link) //Zeite Abfrage durch return eignelitch unnötig
	{
		if (current->isFree == TRUE)
		{
			if (current->memorySize == link->memorySize)
			{
				head->memorySize -= link->memorySize;

				//Update Current //Link in Current reinladen
				current->isFree = link->isFree;
				current->key = link->key;
				current->memorySize = link->memorySize;
				return TRUE;
			}
			else if (current->memorySize >= link->memorySize)
			{
				struct MEMORY* temp = (struct MEMORY*)malloc(sizeof(struct MEMORY));
				head->memorySize -= link->memorySize;

				//X = 500 - 100
				//diff = 400
				diff = current->memorySize - link->memorySize;

				//Verlust von 100 Speicher, dafür muss ein Leerer Prozess hinzugefühgt werden
				//Erstelle leeren Prozess, daher key->-1;
				temp->isFree = TRUE;
				temp->key = -1;
				temp->memorySize = diff;

				//temp zwischen Link und seinem Next einfühgen.
				temp->next = current->next;
				current->next->prev = temp;

				current->isFree = link->isFree;
				current->key = link->key;
				current->memorySize = link->memorySize;

				//Violett
				temp->prev = current;
				current->next = temp;
				return TRUE;
			}
		}
		current = current->next;
	}
	printf("\n[FirstFit] Es wurde kein leerer Prozess gefunden, der gross genug ist!");
	return FALSE;
} */

//Fühgt Hinten (Links) den neusten Eintrag hinzu. Wenn noch kein Last gesetzt ist, wird der nächste Eintrag zu Last
/*void insertLast(Boolean isFree, int key, unsigned memorySize, PCB_t* prozess)
{
	struct MEMORY* link = (struct MEMORY*)malloc(sizeof(struct MEMORY));

	unsigned countFree;
	
	link->isFree = isFree;
	link->key = key;
	link->memorySize = memorySize;
	link->prozessInfo = prozess;

	//Hoffe das zerstörrt nichts
	link->next = NULL;
	link->prev = NULL;

	//Die Abfrage muss später raus, bzw. wo anderes stehen. 
	//Es sollen ja keine Leeren Prozesse reinkommen, oder Prozesse die schon fertig sind.
	if (link->isFree == TRUE)
	{
		printf("\n[InsertLast] Es wird versucht ein bereits fertigen Prozess hinzuzufühgen!");
		//Unschön
		return;
	}

	if (head->memorySize >= link->memorySize)
	{
		if (tail != NULL)
		{
			countFree = check();
			if (countFree >= 1)
			{
				//printf("\n[insertLast] Versuche firstFit!");
				if (firstFit((struct MEMORY*)link) == TRUE)
				{
					//printf("\n[insertLast] FirstFit erfolgreich!");
				
					//Änderungen damit das im Core läuft
					processTable[key].status = running;
					usedMemory = usedMemory + processTable[key].size;
					systemTime = systemTime + LOADING_DURATION;
					runningCount = runningCount + 1;
					//Vielleicht kann man das noch anderes machen
					flagNewProcessStarted();

					//unschön
					return;
				}
				if (countFree >= 2)
				{
					printf("\n[FirstFit] Kompaktiere den Speicher und versuche es erneut!");
					kompaktierung();
					if (firstFit((struct MEMORY*)link) == TRUE)
					{
						//Änderungen damit das im Core läuft
						processTable[key].status = running;
						usedMemory = usedMemory + processTable[key].size;
						systemTime = systemTime + LOADING_DURATION;
						runningCount = runningCount + 1;
						//Vielleicht kann man das noch anderes machen
						flagNewProcessStarted();

						printf("\n[FirstFit] War nach der Kompaktierung erfolgreich!");
						return;
					}
				}
				printf("\n[insertLast] FirstFit nicht erfolgreich");
			}


		}

		//printf("\n[insertLast] InsertLast!");
		head->memorySize = head->memorySize - memorySize;

		if (tail == NULL && head != NULL) 
		{
			link->prev = tail;
			link->next = head;
			head->prev = link;

			//tail = link;
		}
		else 
		{
			link->next = tail;
			link->prev = tail->prev;
			tail->prev = link;

			//tail = link;
		}
		tail = link;

		//Änderungen damit das im Core läuft
		processTable[key].status = running;
		usedMemory = usedMemory + processTable[key].size;
		systemTime = systemTime + LOADING_DURATION;
		runningCount = runningCount + 1;
		//Vielleicht kann man das noch anderes machen
		flagNewProcessStarted();
	}
	else
	{
		printf("\n[insertLast] Error! Speicher zu klein!");
		printf("\n[insertLast] Packe prozess in die Q\n");

		//Änderungen damit das im Core läuft
		logPidMem(key, "Process too large, not started");
		processTable[key].status = blocked;

		enqueue(link);
		displayQ();
	}
} */

void insertLast(Boolean freeMemory, PCB_t* prozessInfo) {
	struct MEMORY* link = (struct MEMORY*)malloc(sizeof(struct MEMORY));
	MEMORY* temp = head->prev;

	unsigned countFree;

	link->freeMemory = freeMemory;
	link->prozessInfo = prozessInfo;

	if (link->freeMemory == TRUE)
	{
		printf("\n[InsertLast] Es wird versucht freier Speicher hinzuzufügen!");
	}
	else if (freeMemorySize >= link->prozessInfo->size) {
		if (head->freeMemory) {
			tail = head;
			head = link;

			head->prozessInfo->memoryPointer = 0;
			freeMemorySize = freeMemorySize - head->prozessInfo->size;
			if (freeMemorySize > 0) {
				tail->prozessInfo->size = tail->prozessInfo->size - head->prozessInfo->size;
				head->prev = tail;
				tail->next = head;
			}
			else if (freeMemorySize == 0) {
				delete(tail);
			}
		}
		else {
			do {
				if (temp->freeMemory && temp->prozessInfo->size >= link->prozessInfo->size) {
					temp->prozessInfo->size = temp->prozessInfo->size - link->prozessInfo->size;
					link->next = temp;
					link->prev = temp->prev;
					temp->prev = link;
				}
				else {
					temp = temp->prev;
				}
			} while (temp->prev != NULL);
		}
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
		PCB_t* pcb_t = malloc(sizeof(struct PCB_t));

		pcb_t->size = freeMemorySize;
		pcb_t->pid = 0;
		pcb_t->memoryPointer = 0;

		//create a link
		struct MEMORY* link = (struct MEMORY*)malloc(sizeof(struct MEMORY));
		link->freeMemory = TRUE;
		link->prozessInfo = pcb_t;

		link->prev = NULL;
		link->next = NULL;

		head = link;
	}
}

//Checkt, ob es mehr als 2 bereits fertige Prozesse gibt.
unsigned check()
{
	struct MEMORY* current = tail;
	unsigned count = 0;

	while (current->next != NULL && current->next != head)
	{
		if (current->freeMemory == TRUE)
		{
			count = count + 1;
		}
		current = current->next;
	}
	return count;
}

//Same as delete, but in Fast
void delete(MEMORY* current)
{
	//If Last - Muss noch getestet werden
	if (current->prev == NULL)
	{
		tail = current->next;
	}
	else
	{
		current->prev->next = current->next;
	}

	//If Head - Muss noch getestet werden
	//Der Fall, so wie die if-Abfrage gerade dort steht, darf nicht vorkommen! current->next == NULL
	/*if (current != head)	//Das sollte nur der Falls ein, wenn current head ist, und head darf nicht gelöscht werden.
	{
		head = current->prev;
	}
	else
	{
		current->next->prev = current->prev;
	}
	//Ich glaub die Line darunter ist überflüssing
	//current->next->prev = current->prev;

	free(current);
	return; */
}

//Defragmentiert die List. Sobald ein Eintrag fertig ist, wird überprüft ob mehrere leere Prozesse nebeneinander stehen.
void merge(MEMORY* current)
{
	struct MEMORY* temp = NULL;
	unsigned l = length();

	if (l <= 2)
	{
		printf("[defrag2] Error! Es gibt nur ein Prozess");
		return;
	}

	//If Last
	if (current == tail)
	{
		if (current->next->freeMemory == TRUE)
		{
			temp = current->next;
			temp->prozessInfo->size = temp->prozessInfo->size + current->prozessInfo->size > MEMORY_SIZE ? MEMORY_SIZE : temp->prozessInfo->size + current->prozessInfo->size;
			//delete(current->key);

			delete((struct MEMORY*)current);
		}
	}
	//If Prozess bevore Head
	else if (current->next->prozessInfo->pid == 0)
	{
		if (current->prev->freeMemory == TRUE)
		{
			temp = current->prev;
			temp->prozessInfo->size = temp->prozessInfo->size + current->prozessInfo->size > MEMORY_SIZE ? MEMORY_SIZE : temp->prozessInfo->size + current->prozessInfo->size;
			//delete(current->key);

			delete((struct MEMORY*)current);
		}
	}
	else
	{
		if (current->next->freeMemory == TRUE && current->prev->freeMemory == TRUE)
		{
			//löscht Prozess vor Current und Current selbst.
			temp = current->prev;

			temp->prozessInfo->size += current->next->prozessInfo->size;
			//printf("\nGib %u Memory dem Ersten!", current->next->memorySize);
			temp->prozessInfo->size = temp->prozessInfo->size + current->prozessInfo->size > MEMORY_SIZE ? MEMORY_SIZE : temp->prozessInfo->size + current->prozessInfo->size;
			//printf("\nGib %u Memory dem Ersten!", current->memorySize);

			//delete(current->next->key);
			//delete(current->key);

			delete((struct MEMORY*)current->next);
			delete((struct MEMORY*)current);
		}
		else if (current->next->freeMemory == TRUE)
		{
			temp = current->next;
			temp->prozessInfo->size = temp->prozessInfo->size + current->prozessInfo->size > MEMORY_SIZE ? MEMORY_SIZE : temp->prozessInfo->size + current->prozessInfo->size;
			//printf("\nGib %u Memory vor mir!", current->memorySize);
			
			//delete(current->key);

			delete((struct MEMORY*)current);
		}
		else if (current->prev->freeMemory == TRUE)
		{
			temp = current->prev;
			temp->prozessInfo->size = temp->prozessInfo->size + current->prozessInfo->size > MEMORY_SIZE ? MEMORY_SIZE : temp->prozessInfo->size + current->prozessInfo->size;
			//printf("\nGib %u Memory hinter mir!", current->memorySize);
			
			//delete(current->key);

			delete((struct MEMORY*)current);
		}
	}
}

//Das Argument isFree vom Prozess wird auf True gesetzt 
//Mit defragmentierung2
void setFinish(unsigned pid)
{
	struct MEMORY* current = tail;
	struct MEMORY* next = NULL;

	if (current == NULL)
	{
		printf("[setFinish] Error! Es gibt keine Prozesse, der Prozess mit der PID: %d kann nicht gefunden werden!", pid);
		return;
	}
	else
	{
		while (current->prozessInfo->pid != pid)
		{
			if (current->next == NULL)
			{
				printf("[setFinish] Error! Die PID %d konnte nicht gefunden werden", pid);
				return;
			}
			else
			{
				current = current->next;
			}
		}

		if (current->freeMemory == FALSE)
		{
			current->freeMemory = TRUE;
			freeMemorySize += current->prozessInfo->size;

			merge((struct MEMORY*) current);
		}
	}
	if (!isQempty())
	{
		struct MEMORY* temp = dequeue();
		insertLast(temp->freeMemory, temp->prozessInfo);
	}
}

//Geht jeden Eintrag einmal durch, um zu checken, ob defragmentiert werden muss... Eigentlich nicht nötig. Defrag2 ist eh besser
void defragmentierung()
{
	struct MEMORY* current = tail;
	struct MEMORY* next = NULL;

	unsigned l = length();

	if (current == NULL || l < 2)
	{
		return;
	}

	//Auf Edgecases testen
	while (current->next != NULL)
	{
		next = current->next;

		if (next->freeMemory == TRUE && current->freeMemory == TRUE)
		{
			next->prozessInfo->size += current->prozessInfo->size;
			delete(current);
		}
		current = next;
	}
}

//Alle Prozesse die fertig sind, 
//werden erneut an die Liste hinten rangesetzt und darauf wird der erste Eintrag gelöscht.
void kompaktierung()
{
	struct MEMORY* current = tail;
	struct MEMORY* temp = NULL;

	while (current != head)
	{
		if (current->freeMemory == TRUE)
		{
			temp = current->next;
			delete((struct MEMORY*)current);
			current = temp;
		}
		else 
		{
			current = current->next;
		}
	}
}

int mainMemory()
{
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

	insertLast(FALSE, p1);
	displayMemory();

	insertLast(FALSE, p2);
	displayMemory();

	insertLast(FALSE, p3);
	displayMemory();

	insertLast(FALSE, p4);
	displayMemory();

	insertLast(FALSE, p5);
	displayMemory();

	insertLast(FALSE, p6);
	displayMemory();

	insertLast(FALSE, p7);
	displayMemory();

	insertLast(FALSE, p8);
	displayMemory();

	insertLast(FALSE, p9);
	displayMemory();

	return 1;
}