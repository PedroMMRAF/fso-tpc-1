/* ---------------------------------------------- 
   This program should be exec'ed by the Scheduler
   messages must be:
    - received on stdin
    - sent on stdout
 ------------------------------------------------ */

/* ------------------------------------------------------------- */
/* Messages between Scheduler and CPU are always 3 integers long */
/* ------------------------------------------------------------- */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>

#ifndef GLOBALS_H
#include "globals.h"
#endif

int myCPUid; // my CPU id, derived from argv[1]

/*
fromSCH: receive a message (from the Scheduler process)
return:
- the cpu number (for verification by the CPU receiving function)
- the job ID
- the job duration (expected = real in this simulation)
*/
void fromSCH(int *cpuID, int *jID, int *jDuration)
{
    int msg[3];

    /*
    read the message from stdin
    verify (assert) that the size of the message is correct
    unpack the 3 ints - cpuID, jID, jDuration
    verify (assert) that the cpuID matches myCPUid
    */
    int size = read(0, msg, sizeof(msg));

    assert(size == sizeof(msg));

    *cpuID = msg[0];
    *jID = msg[1];
    *jDuration = msg[2];

    assert(myCPUid == *cpuID);
}

/*
toSCH: send a message (to the Scheduler process)
parameters:
- the cpu number (i.e., myCPUid or another variable with the same value)
- the EOSIM tag
- the ammount of work this CPU has done, i.e., SUM of all durations
*/
void toSCH(int cpu, int jID, int jDuration)
{
    int msg[3];

    /*
    pack the 3 ints - cpuID, jID, jDuration into msg
    write the message on stdout
    verify (assert) that the the write was performed correctly
    */
    msg[0] = cpu;
    msg[1] = jID;
    msg[2] = jDuration;

    int size = write(1, msg, sizeof(msg));

    assert(size == sizeof(msg));
}

/* ---------------------------------------------- 
   This program should be exec'ed by the Scheduler
   messages must be:
    - received on stdin
    - sent on stdout
 ------------------------------------------------ */
int main(int argc, char *argv[])
{
    int cpuID, jID, jDuration;
    unsigned int CPUconsumed = 0;

    myCPUid = atoi(argv[1]);

    for (;;)
    {
        fromSCH(&cpuID, &jID, &jDuration);

        if (jID == EOSIM)
            break;

        //sleep(jDuration); // Usar nos testes "em casa"
        usleep(1000 * jDuration); //Usar na submissão ao Mooshak

        CPUconsumed += jDuration;
    }

    toSCH(cpuID, EOSIM, CPUconsumed);
}
