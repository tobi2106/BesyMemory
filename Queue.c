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

struct queue* headQ = NULL;

Boolean isQempty()
{
    return headQ == NULL ? TRUE : FALSE;
}

void displayQ()
{
    if (headQ == NULL)
    {
        printf("[display] headQ ist NULL\n");
    }
    else
    {
        struct queue* ptr = headQ;
        printf("\n[");
        while (ptr != NULL)
        {
            printf("(%d)", ptr->data->pid);
            ptr = ptr->next;
        }
        printf("]\n");
    }
}

void enqueue(PCB_t* value)
{
    queue* q = malloc(sizeof(struct queue));
    queue* temp = headQ;
    q->data = value;

    if (isQempty()) headQ = q;
    else
    {
        while (temp->next != NULL) temp = temp->next;
        temp->next = q;
    } 
    q->next = NULL;
}

unsigned dequeue()
{
    queue* temp = headQ;
    PCB_t* data = malloc(sizeof(struct PCB_t));

    if (isQempty())
    {
        printf("[Dequeue] Die Q ist leer");
        return 0;
    }
    else
    {
        data = headQ->data;
        //Nur ein Eintrag
        if (headQ->next == NULL)
        {
            headQ = NULL;
        }
        else
        {
            headQ = headQ->next;
        }
        return data->pid;
    }
}

Boolean doseNextQFit()
{
    if (isQempty()) return FALSE;
    return (MEMORY_SIZE - usedMemory) >= headQ->data->size ? TRUE : FALSE;
}

int mainQueue()
{
    /*
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

    displayQ();

    enqueue(p1);
    displayQ();

    enqueue(p2);
    displayQ();

    enqueue(p3);
    displayQ();

    enqueue(p4);
    displayQ();

    enqueue(p5);
    displayQ();

    dequeue();
    displayQ();

    enqueue(p6);
    displayQ();

    dequeue();
    displayQ();

    enqueue(p7);
    displayQ();

    dequeue();
    displayQ();

    enqueue(p8);
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    enqueue(p9);
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    enqueue(p1);
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();

    enqueue(p2);
    displayQ();

    dequeue();
    displayQ();

    dequeue();
    displayQ();
    */
    return 1;
}