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

Boolean firstFit(PCB_t* prozess);

void initMemory();

void delete(MEMORY* current);

void merge(MEMORY* current);

void setFinish(unsigned pid);

void kompaktierung();