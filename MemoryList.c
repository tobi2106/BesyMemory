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

//TODO nicht ganz richtig mit meiner Implementation
Boolean isEmpty()
{
	return head == NULL;
}

unsigned length()
{
	int length = 0;
	struct MEMORY* current;

	for (current = tail; current != NULL; current = current->next)
	{
		length++;
	}
	return length;
}

void displayMemory()
{
	if (head->prev == NULL)
	{
		printf("[(%d, %d, %d)]", head->isFree, head->key, head->memorySize);
		return;
	}

	struct MEMORY* ptr = tail;
	printf("\n[");
	while (ptr != NULL)
	{
		printf("(%d, %d, %d)", ptr->isFree, ptr->key, ptr->memorySize);
		ptr = ptr->next;
	}
	printf("]");
}

void insertLast(Boolean isFree, int key, unsigned memorySize)
{
	if (memorySize > head->memorySize)
	{
		printf("[ERROR] Speicher zu klein!\n");
	}
	else 
	{
		head->memorySize = head->memorySize - memorySize;
	}

	struct MEMORY* link = (struct MEMORY*)malloc(sizeof(struct MEMORY));
	link->isFree = isFree;
	link->key = key;
	link->memorySize = memorySize;

	if (tail == NULL && head != NULL) 
	{
		link->prev = NULL;
		link->next = head;
		head->prev = link;
		tail = link;
		return;
	}
	
	link->next = tail;
	link->prev = tail->prev;
	tail->prev = link;

	tail = link;
}

void insertFirst(Boolean isFree, int key, unsigned memorySize)
{
	//create a link
	struct MEMORY* link = (struct MEMORY*)malloc(sizeof(struct MEMORY));
	link->isFree = isFree;
	link->key = key;
	link->memorySize = memorySize;

	link->next = head;
	link->prev = tail;
	if (head != NULL) head->prev = link;

	head = link;
}

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

//TODO nochmal überarbeiten Liste funktioniert mit Last
void delete(unsigned key)
{
	struct MEMORY* current = tail;
	struct MEMORY* next = NULL;

	if (current == NULL)
	{
		return;
	}

	while(current->key != key)
	{
		if(current->next == NULL)
		{
			printf("Der Key %u konnte nicht gefunden werden", key);
			return;
		}
		else
		{
			next = current;
			current = current->next;
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
	if (current->next == NULL)
	{
		head = current->prev;
	}
	else
	{
		current->next->prev = current->prev;

	}
	current->next->prev = current->prev;
	
	free(current);
	return;
}

void readyList(unsigned key)
{
	struct MEMORY* current = tail;
	struct MEMORY* next = NULL;

	if (current == NULL)
	{
		return;
	}

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

	current->isFree = TRUE;
	head->memorySize =+ current->memorySize;

}

void kompaktierung()
{
	struct MEMORY* current = tail;
	struct MEMORY* next = NULL;

	unsigned l = length();

	if (current == NULL || l < 2)
	{
		return;
	}

	//Auf Edegcases testen
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

void defragmentierung()
{
	unsigned i = 0;
	unsigned l = length() - 1;
	struct MEMORY* current = tail;
	struct MEMORY* last = NULL;


	if (current == NULL || length < 2)
	{
		return;
	}

	//Auf Edegcases testen //Man kann auch bis Key 0 Iterieren
	for (i; i <= l; i = i + 1)
	{
		if (current->isFree == TRUE)
		{
			if (current->next != NULL)
			{
				last = current;
				delete(current->key);
				l = l - 1;
				current->next = last->next;
			}
			else
			{
				delete(current->key);
				return;
			}
		}
		if (current->next == NULL)
		{
			return;
		}		
		if (last == NULL)
		{
			 current = current->next;
		}
	}
}

int mainMemory()
{
	struct MEMORY* freeMemory = (struct MEMORY*)malloc(sizeof(struct MEMORY));


	insertFirst(TRUE, 0, MEMORY_SIZE);
	displayMemory();

	insertLast(FALSE, 1, 100);
	displayMemory();

	insertLast(FALSE, 2, 200);
	displayMemory();

	insertLast(FALSE, 3, 700);
	displayMemory();

	insertLast(FALSE, 4, 24);
	displayMemory();

	readyList(2);
	displayMemory();

	insertLast(FALSE, 5, 100);
	displayMemory();

	readyList(3);
	displayMemory();

	kompaktierung();
	displayMemory();

	insertLast(FALSE, 6, 100);
	displayMemory();

	readyList(1);
	displayMemory();

	defragmentierung();
	displayMemory();
	
	return 1;
}