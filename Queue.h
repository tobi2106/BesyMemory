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

int isQempty();

void displayQ();

void enqueue(PCB_t* value);

unsigned dequeue();

Boolean doseNextQFit();