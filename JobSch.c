#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Global definitions
#ifndef GLOBALS_H
#include "globals.h"
#endif

// Definitions for scheduler/cpu communications
#ifndef COMMUNICATIONS_H
#include "communications.h"
#endif

// Definitions for scheduler data structures
#ifndef SCHEDULER_H
#include "scheduler.h"
#endif

struct CPUqueuePair CPUqueueList[];

/* -------------------------------------------- */
void createCPUs(int nCPUs)
{
    char cpuID[2]; // will contain the CPU number when exec'ing the
                   // CPU program. In the exec(), that number is a char

    for (int cpu = 0; cpu < nCPUs; cpu++)
    {
        /*
        create the pipes
        fork()
        in the parent, close unnecessary pipe descriptors
        in the son:
        - close unnecessary pipe descriptors
        - prepare the son to communicate with the pipe using stdin and stdout
        - exec the CPU program with the cpu number (as char) in cpuID as argument
        - verify that the exec has not failed!
        */
        struct CPUqueuePair *pair = CPUqueueList + cpu;

        pipe(pair->rdQ);
        pipe(pair->wrQ);

        if (fork())
        {
            close(pair->rdQ[0]);
            close(pair->wrQ[1]);
        }
        else
        {
            close(pair->rdQ[1]);
            close(pair->wrQ[0]);
            close(0);
            dup(pair->rdQ[0]);
            close(1);
            dup(pair->wrQ[1]);

            sprintf(cpuID, "%d", cpu);

            execl("./CPU", "./CPU", cpuID, (char *)NULL);
            exit(1);
        }
    }
}

#define MAX(x, y) (x > y ? x : y)

void report(int nCPUs, struct cpuInfo cpuData[])
{
    int cpuID, jID, jDuration, totConsumed = 0, totalSimulationTime = 0;

    for (int cpu = 0; cpu < nCPUs; cpu++)
    {

        fromCPU(cpu, &cpuID, &jID, &jDuration);

        cpuData[cpu].usage += jDuration;
        totConsumed += jDuration;
        totalSimulationTime = MAX(totalSimulationTime, cpuData[cpu].usage);

        printf("CPU[%d] usage= %d\n", cpu, cpuData[cpu].usage);
    }

    printf("Total Simulation Time=%d\n", totalSimulationTime);
    printf("Average CPU consumed for %d CPUs= %.1f%%\n",
           nCPUs, 100.0 * totConsumed / totalSimulationTime);
}

int main(int argc, char *argv[])
{
    char policy = argv[1][0];  // F or S
    int nCPUs = atoi(argv[2]); // 1 or > 1, up to 4
    char *filename = argv[3];  // name of the file with Job data
                               // or the character "-" if reading from stdin

    int l, cpu;

    struct cpuInfo cpuData[nCPUs];
    memset(cpuData, 0, sizeof(struct cpuInfo) * nCPUs);

    struct jobQ jobQueue[MAX_JQ_SIZE];
    memset(jobQueue, 0, sizeof(struct jobQ) * MAX_JQ_SIZE);

    createCPUs(nCPUs);

    int lines = readJobSubmission(filename, jobQueue);

    if (policy == 'S')
    {
        int k;

        for (k = lines - 1; k > 1; k--)
        {
            for (l = 0; l < k; l++)
            {
                if (jobQueue[l].duration > jobQueue[l + 1].duration)
                {
                    struct jobQ temp = jobQueue[l];
                    jobQueue[l] = jobQueue[l + 1];
                    jobQueue[l + 1] = temp;
                }
            }
        }
    }

    // Schedule the jobs (FIFO version, assumes job runtime == duration)
    l = 0;
    cpu = 0;
    while (l < lines)
    {
        printf("jQ[%d] jID %d run %d\n", l, jobQueue[l].jID,
               jobQueue[l].duration);
        dispatch(l, jobQueue, cpu, cpuData);
        l++;
        cpu = (cpu + 1) % nCPUs;
    }

    // send SIMulation END messages (EOSIM) to CPUs
    for (int cpu = 0; cpu < nCPUs; cpu++)
        toCPU(cpu, EOSIM, EOSIM);

    // Get the replies from each CPU, compute and print results
    report(nCPUs, cpuData);

    /* Wait for forked processes */
    wait(NULL);

    return 0;
}
