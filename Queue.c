#include <stdio.h>
#include <stdlib.h>
#include "bs_types.h"
#include "globals.h"
#include "log.h"

struct queue* headQ = NULL;


// Checks if the queue is empty
Boolean isQempty()
{
    return headQ == NULL ? TRUE : FALSE;
}

// Prints out the Queue
void displayQ()
{
    if (headQ == NULL) logGeneric("(displayQ) Queue is empty!");        // Checks if the queue is empty
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

// adds an entry to the queue
void enqueue(PCB_t* value)
{
    queue* q = malloc(sizeof(struct queue));
    queue* temp = headQ;
    q->data = value;

    if (isQempty()) headQ = q;      // Sets the new entry as the head if the queue is empty
    else
    {
        while (temp->next != NULL) temp = temp->next;   // Iterates through the list and adds the new element at the end of the queue
        temp->next = q;
    } 
    q->next = NULL;
}

// removes the head from the queue, sets the next entry as head and gets the pid
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
        if (headQ->next == NULL) headQ = NULL;
        else headQ = headQ->next;
        return data->pid;
    }
}

// Checks if the head of the queue fits into the free memory
Boolean doseNextQFit()
{
    if (isQempty()) return FALSE;
    return (MEMORY_SIZE - usedMemory) >= headQ->data->size ? TRUE : FALSE;
}