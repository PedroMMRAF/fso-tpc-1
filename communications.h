#define COMMUNICATIONS_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

/* Use this struct as a template for 2 pipes for 1 CPU */
struct CPUqueuePair
{
    int rdQ[2];
    int wrQ[2];
};

/*
- If you are aiming to support only 1 CPU,
  define a variable based on struct CPUqueuePair
- If you are aiming to support only multiple CPUs,
  define a more complex structure based on struct CPUqueuePair
*/
extern struct CPUqueuePair CPUqueueList[MAX_CPUs];

/*
Prototypes of communications functions used by the Scheduler program
*/
void toCPU(int cpu, int jID, int jDuration);

void fromCPU(int cpu, int *cpuID, int *jID, int *jDuration);
