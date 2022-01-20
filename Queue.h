#pragma once

#include "bs_types.h"

// Returns if the queue is empty
int isQempty();

// Displays the queue
void displayQ();

// Adds and element to the queue
void enqueue(PCB_t* value);

// Removes an element from the queue and returns the pid
unsigned dequeue();

// checks if the head fits into the free memory
Boolean doseNextQFit();