#pragma once

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

Boolean isEmpty();

int length();

void displayMemory();

//Boolean firstFit(MEMORY* link);

//void insertLast(Boolean isFree, int key, unsigned memorySize, PCB_t* prozess);

Boolean insertLast(PCB_t* prozess);

void setHead();

unsigned check();

void delete(MEMORY* current);

void merge(MEMORY* current);

void setFinish(unsigned pid);

void defragmentierung();

void kompaktierung();

int mainMemory();