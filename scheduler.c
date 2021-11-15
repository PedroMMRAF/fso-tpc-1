
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#ifndef COMMUNICATIONS_H
#include "communications.h"
#endif

#ifndef SCHEDULER_H
#include "scheduler.h"
#endif

/*
Dispatch a Job waiting at the job queue at index jobQidx
to the CPU identified by cpu
and update the cpu table information with status=RUNNING
*/
void dispatch(int jobQidx, struct jobQ jobQueue[],
              int cpu, struct cpuInfo cpuData[])
{
    // pipes allow us to queue several messages, so we do not need to assert
    // if the CPU is IDLE because if it is the messages will stay on the queue

    /*
    "Just for fun" you can set the status of the CPU to RUNNING
    send a message to the cpu with the information of the job
    the Job information is retrieved from the jobQueue using the jobQidx
    */
    toCPU(cpu, jobQueue[jobQidx].jID, jobQueue[jobQidx].duration);
    cpuData[cpu].status = RUNNING;
}

/*
readJobSubmission: read the input (comming from a regular file or from stdin
                    and store the data on the jobQueue array

return: number of lines read (that is, the number of Jobs on the "queue")
*/
int readJobSubmission(char *filename, struct jobQ jobQueue[])
{
    FILE *file;
    int lines = 0; // number of lines in the file

    // If reading from stdin (filename is null or named "-")
    if (filename == NULL || strcmp(filename, "-") == 0)
        file = stdin;
    else // "regular filename", open it
        file = fopen(filename, "r");
    assert(file != NULL);

    // Get a line from the filestream "file", split it into words,
    // and capture the relevant data
    char line[MAX_LINE_LEN];
    char *word;

    char *l = fgets(line, MAX_LINE_LEN, file);
    while (l != NULL)
    {
        word = strtok(line, " \t\n"); // get the J
        word = strtok(NULL, " \t\n"); // get the jID
        jobQueue[lines].jID = atoi(word);
        word = strtok(NULL, " \t\n"); // get the R
        word = strtok(NULL, " \t\n"); // get the job duration
        jobQueue[lines].duration = atoi(word);
        word = strtok(NULL, " \t\n"); // get the EOL
        lines++;
        l = fgets(line, MAX_LINE_LEN, file);
    }

    return lines;
}
