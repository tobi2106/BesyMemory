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

struct queue* tailQ = NULL;

int isQempty()
{
    return tailQ == NULL;
}

void enqueue(MEMORY* value)
{
    queue* q = (struct queue*)malloc(sizeof(struct queue));

    q->data = value;

    if (isQempty())
    {
        tailQ = q;
        q->next = NULL;
    }
    else
    {
        q->next = tailQ;
        tailQ = q;
    }
}

struct MEMORY* dequeue()
{
    queue* temp = (struct queue*)malloc(sizeof(struct queue));
    MEMORY* data = (struct MEMORY*)malloc(sizeof(struct MEMORY));

    if (isQempty())
    {
        printf("[Dequeue] Die Q ist leer");
        return NULL;
    }
    else
    {
        temp = tailQ;
        //Nur ein Eintrag
        if (tailQ->next == NULL)
        {
            data = tailQ->data;
            tailQ = NULL;
        }
        //Zwei Einträge
        else if (tailQ->next->next == NULL)
        {
            data = tailQ->next->data;
            tailQ->next = NULL;
            temp = NULL;
        }
        //Mehr als 2 Einträge
        else
        {
            while (temp->next->next != NULL) temp = temp->next;
            data = temp->next->data;
            temp->next->next = NULL;
            temp->next = NULL;
        }
        printf(CYN"(%d, %d, %d)\n"RESET, data->freeMemory, data->prozessInfo->pid, data->prozessInfo->size);
        return data;
    }
}

void displayQ()
{
    if (tailQ == NULL)
    {
        printf("[display] tailQ ist NULL\n");
    }
    else
    {
        struct queue* ptr = tailQ;
        printf("\n[");
        while (ptr != NULL)
        {
            printf("(%d)", ptr->data->prozessInfo->pid);
            ptr = ptr->next;
        }
        printf("]\n");
    }
}

int mainQueue()
{
    struct MEMORY* a = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    a->freeMemory = FALSE, a->prozessInfo->pid = 1; a->prozessInfo->size = 100;

    struct MEMORY* b = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    b->freeMemory = FALSE, b->prozessInfo->pid = 2; b->prozessInfo->size = 100;

    struct MEMORY* c = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    c->freeMemory = FALSE, c->prozessInfo->pid = 3; c->prozessInfo->size = 100;

    struct MEMORY* d = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    d->freeMemory = FALSE, d->prozessInfo->pid = 4; d->prozessInfo->size = 100;

    struct MEMORY* e = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    e->freeMemory = FALSE, e->prozessInfo->pid = 5; e->prozessInfo->size = 100;

    displayQ();

    enqueue(a);
    displayQ();

    enqueue(b);
    displayQ();

    enqueue(c);
    displayQ();

    enqueue(d);
    displayQ();

    enqueue(e);
    displayQ();

    dequeue();
    displayQ();

    enqueue(a);
    displayQ();

    dequeue();
    displayQ();

    enqueue(a);
    displayQ();

    dequeue();
    displayQ();

    enqueue(a);
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    enqueue(a);
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    enqueue(a);
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    enqueue(a);
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    return 0;
}