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
        printf(CYN"(%d, %d, %d)\n"RESET, data->isFree, data->key, data->memorySize);
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
            printf("(%d)", ptr->data->key);
            ptr = ptr->next;
        }
        printf("]\n");
    }
}

int mainQueue()
{
    struct MEMORY* a = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    a->isFree = FALSE, a->key = 1; a->memorySize = 100;

    struct MEMORY* b = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    b->isFree = FALSE, b->key = 2; b->memorySize = 100;

    struct MEMORY* c = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    c->isFree = FALSE, c->key = 3; c->memorySize = 100;

    struct MEMORY* d = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    d->isFree = FALSE, d->key = 4; d->memorySize = 100;

    struct MEMORY* e = (struct MEMORY*)malloc(sizeof(struct MEMORY));
    e->isFree = FALSE, e->key = 5; e->memorySize = 100;

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