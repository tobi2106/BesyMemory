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
		printf("]");
	}
}

//Fühgt Hinten (Links) den neusten Eintrag hinzu. Wenn noch kein Last gesetzt ist, wird der nächste Eintrag zu Last
void insertLast(Boolean isFree, int key, unsigned memorySize)
{
	if (isFree == FALSE)
	{
		if (memorySize > head->memorySize)
		{
			printf("[insertLast] Error! Speicher zu klein!\n");
		}
		else 
		{
			head->memorySize = head->memorySize - memorySize;
		}
	}

	struct MEMORY* link = (struct MEMORY*)malloc(sizeof(struct MEMORY));
	link->isFree = isFree;
	link->key = key;
	link->memorySize = memorySize;

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

//Weiß nicht, wofür ich den geschrieben habe...
void check()
{
	while (tail->next != NULL)
	{
		struct MEMORY* current = tail;
		if (current->isFree == TRUE)
		{
			unsigned myKey = current->key;
			//delete(myKey);
		}
	}
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
			temp->memorySize += current->memorySize;
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
			temp->memorySize += current->memorySize;
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
			printf("\nGib %u Memory dem Ersten!", current->next->memorySize);
			temp->memorySize += current->memorySize;
			printf("\nGib %u Memory dem Ersten!", current->memorySize);

			//delete(current->next->key);
			//delete(current->key);

			deleteFast((struct MEMORY*) current->next);
			deleteFast((struct MEMORY*)current);
		}
		else if (current->next->isFree == TRUE)
		{
			temp = current->next;
			temp->memorySize += current->memorySize;
			printf("\nGib %u Memory vor mir!", current->memorySize);
			
			//delete(current->key);

			deleteFast((struct MEMORY*)current);
		}
		else if (current->prev->isFree == TRUE)
		{
			temp = current->prev;
			temp->memorySize += current->memorySize;
			printf("\nGib %u Memory hinter mir!", current->memorySize);
			
			//delete(current->key);

			deleteFast((struct MEMORY*)current);
		}
	}
}

//Das Argument isFree vom Prozess wird auf True gesetzt 
//Mit defragmentierung2
void setFinish(unsigned key)
{
	struct MEMORY* current = tail;
	struct MEMORY* next = NULL;

	if (current == NULL)
	{
		return;
	}
	else
	{
		while (current->key != key)
		{
			if (current->next == NULL)
			{
				printf("Der Key %u konnte nicht gefunden werden", key);
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

			defragmentierung2((struct MEMORY*) current);
		}
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

//Alle Prozesse die fertig sind, werden erneut an die Liste hinten rangesetzt und darauf wird der erste Eintrag gelöscht.
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

			//temp = current->next;
			//insertLast(current->isFree, current->key, current->memorySize);
			//deleteFast((struct MEMORY*)current);
			//deleteFast((struct MEMORY*)tail);
			//current = temp;
		}
		else 
		{
			current = current->next;
		}
	}
}

void insertBefore(MEMORY* link)
{

}

//FirstFit
void firstFit(MEMORY* link)
{
	struct MEMORY* current = tail;
	struct MEMORY* temp = NULL;
	unsigned diff = 0;

	while (current->next == FALSE)
	{
		if (current->isFree == TRUE)
		{
			if (current->memorySize == link->memorySize)
			{
				link = current;
			}
			else if (current->memorySize >= link->memorySize)
			{
				//TODO
				diff = current->memorySize - link->memorySize;
				
				link = current;
				temp->isFree = TRUE;
				temp->key = -1;
				temp->memorySize = diff;
				temp->next = link->next;
				temp->prev = link->prev;



				link->memorySize -= diff;
				head->memorySize -= link->memorySize;
				//temp->memorySize = 
			}
		}
	}
}

int mainMemory()
{
	setHead(TRUE, 0, MEMORY_SIZE);
	displayMemory();

	insertLast(TRUE, 1, 100);
	displayMemory();

	insertLast(TRUE, 2, 500);
	displayMemory();

	insertLast(FALSE, 3, 100);
	displayMemory();

	insertLast(FALSE, 4, 300);
	displayMemory();

	insertLast(TRUE, 5, 24);
	displayMemory();
	
	setFinish(5);
	displayMemory();

	setFinish(3);
	displayMemory();

	setFinish(1);
	displayMemory();

	kompaktierung();
	displayMemory();

	return 1;
}