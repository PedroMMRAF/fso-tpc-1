/* ------------------------------------------------------------- */
/* Messages between Scheduler and CPU are always 3 integers long */
/* ------------------------------------------------------------- */

#include <unistd.h>
#include <assert.h>

#ifndef COMMUNICATIONS_H
#include "communications.h"
#endif

/*
toCPU: we send:
- the cpu number (for verification by the CPU receiving function)
- the jID job ID or the EOSIM tag
- the job duration (expected = real in this simulation). N.A on EOSIM
*/
void toCPU(int cpu, int jID, int jDuration)
{
    int msg[3];

    /*
    pack the 3 ints - cpuID, jID, jDuration into msg
    write the message to the pipe that links to the correct CPU
    verify (assert) that the the write was performed correctly
    */

    msg[0] = cpu;
    msg[1] = jID;
    msg[2] = jDuration;

    int size = write(CPUqueueList[cpu].rdQ[1], msg, sizeof(msg));

    assert(size == sizeof(msg));
}

/*
fromCPU: receive an EOSIM reply message (from the CPU process)
parameter:
- the cpu number (we are receiving from)
return:
- the cpu number (we have received) FAILS if they do not match!
- the jID value must be EOSIM
- the duration - ammount of work performed by the CPU process
*/
void fromCPU(int cpu, int *cpuID, int *jID, int *jDuration)
{
    int msg[3];

    /*
    read the message from the pipe that links to the correct CPU
    verify (assert) that the size of the message is correct
    unpack the 3 ints - cpuID, jID, jDuration
    verify (assert) that the cpuID matches the cpu
    */

    int size = read(CPUqueueList[cpu].wrQ[0], msg, sizeof(msg));

    assert(size == sizeof(msg));

    *cpuID = msg[0];
    *jID = msg[1];
    *jDuration = msg[2];

    assert(cpu == *cpuID);
}
