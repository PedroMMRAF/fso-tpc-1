#define SCHEDULER_H

#define RUNNING 1

struct cpuInfo
{
    int status;
    int usage;
};

struct jobQ
{
    int jID;
    int duration;
};

int cpuFree(int nCPUs, struct cpuInfo cpuData[]);

void dispatch(int jobQidx, struct jobQ jobQueue[],
              int cpu, struct cpuInfo cpuData[]);

int readJobSubmission(char *filename, struct jobQ jobQueue[]);
