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
		printf(CYN"[(%d, %d, %d)]"RESET, head->isFree, head->key, head->memorySize);
		return;
	}
	else
	{
		struct MEMORY* ptr = tail;
		printf("\n[");
		while (ptr != NULL && ptr != head)
		{
			printf("(%d, %d, %d)", ptr->isFree, ptr->key, ptr->memorySize);
			ptr = ptr->next;
		}
		if (ptr == head)
		{
			printf(CYN"(%d, %d, %d)"RESET, ptr->isFree, ptr->key, ptr->memorySize);
		}
		printf("]\n");
	}
}

//FirstFit
Boolean firstFit(MEMORY* link)
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
}

//Fühgt Hinten (Links) den neusten Eintrag hinzu. Wenn noch kein Last gesetzt ist, wird der nächste Eintrag zu Last
void insertLast(Boolean isFree, int key, unsigned memorySize, PCB_t* prozess)
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
}

//Setzt den Head. Mehr auch nicht
void setHead(Boolean isFree, int key, unsigned memorySize)
{
	if (head != NULL)
	{
		printf("[insertFirst] Error! Es gibt bereits einen Head!");
	}
	else
	{
		//create a link
		struct MEMORY* link = (struct MEMORY*)malloc(sizeof(struct MEMORY));
		link->isFree = isFree;
		link->key = key;
		link->memorySize = memorySize;

		link->next = head;
		link->prev = tail;
		if (head != NULL)
		{
			head->prev = link;
		}
		else
		{
			head = link;
		}
	}
}

//Checkt, ob es mehr als 2 bereits fertige Prozesse gibt.
unsigned check()
{
	struct MEMORY* current = tail;
	unsigned count = 0;

	while (current->next != NULL && current->next != head)
	{
		if (current->isFree == TRUE)
		{
			count = count + 1;
		}
		current = current->next;
	}
	return count;
}

//Geht durch die Liste, bis Key gefunden wird und löscht dieen Eintrag
void delete(unsigned key)
{
	struct MEMORY* current = tail;
	struct MEMORY* getNext = NULL;

	if (key == 0)
	{
		printf("[delete] Error! Es wird versucht den Key 0 zu löschen, dabei stellt der Eintrag mit Key 0 den freien Speicher in der Liste dar.");
		return;
	}

	if (current == NULL)
	{
		printf("[delete] Error! Es gibt keinen Prozess, Head ist Null");
		return;
	}
	else
	{
		while(current->key != key)
		{
			if(current->next == NULL)
			{
				printf("Der Key %u konnte nicht gefunden werden", key);
				return;
			}
			else
			{
				getNext = current;
				current = current->next;
			}
		}
	}

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
	if (current->next == NULL)	//Das sollte nur der Falls ein, wenn current head ist, und head darf nicht gelöscht werden.
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
	return;
}

//Same as delete, but in Fast
void deleteFast(MEMORY* current)
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
	if (current->next == NULL)	//Das sollte nur der Falls ein, wenn current head ist, und head darf nicht gelöscht werden.
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
	return;
}

//Defragmentiert die List. Sobald ein Eintrag fertig ist, wird überprüft ob mehrere leere Prozesse nebeneinander stehen.
void defragmentierung2(MEMORY* current)
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
		if (current->next->isFree == TRUE)
		{
			temp = current->next;
			temp->memorySize = temp->memorySize + current->memorySize > MEMORY_SIZE ? MEMORY_SIZE : temp->memorySize + current->memorySize;
			//delete(current->key);

			deleteFast((struct MEMORY*)current);
		}
	}
	//If Prozess bevore Head
	else if (current->next->key == 0)
	{
		if (current->prev->isFree == TRUE)
		{
			temp = current->prev;
			temp->memorySize = temp->memorySize + current->memorySize > MEMORY_SIZE ? MEMORY_SIZE : temp->memorySize + current->memorySize;
			//delete(current->key);

			deleteFast((struct MEMORY*)current);
		}
	}
	else
	{
		if (current->next->isFree == TRUE && current->prev->isFree == TRUE)
		{
			//löscht Prozess vor Current und Current selbst.
			temp = current->prev;

			temp->memorySize += current->next->memorySize;
			//printf("\nGib %u Memory dem Ersten!", current->next->memorySize);
			temp->memorySize = temp->memorySize + current->memorySize > MEMORY_SIZE ? MEMORY_SIZE : temp->memorySize + current->memorySize;
			//printf("\nGib %u Memory dem Ersten!", current->memorySize);

			//delete(current->next->key);
			//delete(current->key);

			deleteFast((struct MEMORY*)current->next);
			deleteFast((struct MEMORY*)current);
		}
		else if (current->next->isFree == TRUE)
		{
			temp = current->next;
			temp->memorySize = temp->memorySize + current->memorySize > MEMORY_SIZE ? MEMORY_SIZE : temp->memorySize + current->memorySize;
			//printf("\nGib %u Memory vor mir!", current->memorySize);
			
			//delete(current->key);

			deleteFast((struct MEMORY*)current);
		}
		else if (current->prev->isFree == TRUE)
		{
			temp = current->prev;
			temp->memorySize = temp->memorySize + current->memorySize > MEMORY_SIZE ? MEMORY_SIZE : temp->memorySize + current->memorySize;
			//printf("\nGib %u Memory hinter mir!", current->memorySize);
			
			//delete(current->key);

			deleteFast((struct MEMORY*)current);
		}
	}
}

//Das Argument isFree vom Prozess wird auf True gesetzt 
//Mit defragmentierung2
void setFinish(int key)
{
	struct MEMORY* current = tail;
	struct MEMORY* next = NULL;

	if (current == NULL)
	{
		printf("[setFinish] Error! Es gibt keine Prozesse, der Prozess mit dem Key: %d kann nicht gefunden werden!", key);
		return;
	}
	else
	{
		while (current->key != key)
		{
			if (current->next == NULL)
			{
				printf("[setFinish] Error! Der Key %d konnte nicht gefunden werden", key);
				return;
			}
			else
			{
				current = current->next;
			}
		}

		if (current->isFree == FALSE)
		{
			current->isFree = TRUE;
			head->memorySize += current->memorySize;

			//Änderungen damit es in Core läuft
			usedMemory = usedMemory - processTable[key].size;

			defragmentierung2((struct MEMORY*) current);
		}
	}
	if (!isQempty())
	{
		struct MEMORY* temp = dequeue();
		insertLast(temp->isFree, temp->key, temp->memorySize, temp->prozessInfo);
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

		if (next->isFree == TRUE && current->isFree == TRUE)
		{
			next->memorySize += current->memorySize;
			delete(current->key);
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
		if (current->isFree == TRUE)
		{
			temp = current->next;
			deleteFast((struct MEMORY*)current);
			current = temp;
		}
		else 
		{
			current = current->next;
		}
	}
}

//Nicht benötigt, leer.
void insertAfter(MEMORY* link)
{

}

int mainMemory()
{
	setHead(TRUE, 0, MEMORY_SIZE);
	displayMemory();
	/*
	insertLast(FALSE, 1, 100);
	displayMemory();

	insertLast(FALSE, 2, 500);
	displayMemory();


	//insertLast(FALSE, 50, 69);
	//displayMemory();

	setFinish(2);
	displayMemory();


	insertLast(FALSE, 3, 100);
	displayMemory();

	insertLast(FALSE, 4, 100);
	displayMemory();

	insertLast(FALSE, 5, 100);
	displayMemory();

	insertLast(FALSE, 6, 100);
	displayMemory();

	insertLast(FALSE, 7, 100);
	displayMemory();

	insertLast(FALSE, 8, 100);
	displayMemory();

	insertLast(FALSE, 9, 300);
	displayMemory();

	insertLast(FALSE, 10, 24);
	displayMemory();

	insertLast(FALSE, 11, 300);
	displayMemory();

	insertLast(FALSE, 12, 300);
	displayMemory();

	insertLast(FALSE, 13, 300);
	displayMemory();
	
	setFinish(5);
	displayMemory();

	setFinish(3);
	displayMemory();

	setFinish(1);
	displayMemory();

	setFinish(6);
	displayMemory();

	setFinish(7);
	displayMemory();

	kompaktierung();
	displayMemory();
	*/
	return 1;
}