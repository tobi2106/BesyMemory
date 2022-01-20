#pragma once

#include "bs_types.h"


// Checks if the list is empty
Boolean isEmpty();

// returns the length of the list
int length();

// displays the memory
void displayMemory();

// allocates an element into the memory via firstFit
// adapts the pointers of the elements affected by the allocation
Boolean firstFit(PCB_t* process);

// Initializes the memory and adds a free memory element to it
void initMemory();

// Deletes an entry from the list
void delete(MEMORY* current);

// merges free memory with the current free memory element
void merge(MEMORY* current);

// Sets a process as finished and free memory in the list
void setFinish(unsigned pid);

// Compacts the whole list and merges all free memory elements to one
void compacting();