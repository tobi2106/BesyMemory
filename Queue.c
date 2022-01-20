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
    if (headQ == NULL) logGeneric("(displayQ) Queue is empty!");
    else
    {
        struct queue* ptr = headQ;
        printf("\n[QUE] \t%u \t: [", systemTime);
        while (ptr != NULL)
        {
            printf("(<%u> %u)"RESET, ptr->data->pid, ptr->data->size);
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
        logGeneric("(dequeue) The queue is empty!");
        return 0;
    }
    else
    {
        data = headQ->data;
        //Nur ein Eintrag
        if (headQ->next == NULL) headQ = NULL;
        else headQ = headQ->next;
        return data->pid;
    }
}

Boolean doseNextQFit()
{
    if (isQempty()) return FALSE;
    return (MEMORY_SIZE - usedMemory) >= headQ->data->size ? TRUE : FALSE;
}