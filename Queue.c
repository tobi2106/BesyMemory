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

#define FULL 10

struct queue* tailQ = NULL;
struct queue* headQ = NULL;

//Eigentlich nicht nötig, let's see
void initialize(queue* q)
{
    q->data = NULL;
    q->next = NULL;
}

int isempty()
{
    return tailQ == NULL;
}

void enqueue(MEMORY* value)
{
    queue* q = (struct queue*)malloc(sizeof(struct queue));

    q->data = value;

    if (isempty())
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

    if (isempty())
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

void display()
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
        printf("]");
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

    display();

    enqueue(a);
    display();

    enqueue(b);
    display();

    enqueue(c);
    display();

    enqueue(d);
    display();

    enqueue(e);
    display();

    dequeue();
    display();

    enqueue(a);
    display();

    dequeue();
    display();

    enqueue(a);
    display();

    dequeue();
    display();

    enqueue(a);
    display();

    dequeue();
    display();

    dequeue();
    display();

    dequeue();
    display();

    enqueue(a);
    display();

    dequeue();
    display();

    dequeue();
    display();

    enqueue(a);
    display();

    dequeue();
    display();

    dequeue();
    display();

    enqueue(a);
    display();

    dequeue();
    display();

    dequeue();
    display();

    return 0;
}