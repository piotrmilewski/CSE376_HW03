#ifndef JOBS
#define JOBS

#include "baseHeaders.h"
#include "debug.h"

typedef struct job {
  char name[1024]; // regular terminal doesn't accept lines of more than 1024 characters
  pid_t pid;
  int jobNumber;
  int status; // 0 = suspended, 1 = running
  int exitStatus;
  struct job *nextJob;
} job;

// background jobs fxns
// inserts a new bg job into the array
void jobInsert(char *name, pid_t pid, int status);
// makes jobNumber job go to the foreground (use waitpid here)
void fg(int jobNumber);
// makes suspended job continue execution (send sigcont)
void bg(int jobNumber);
// send signal N to job jobNumber
void killJob(int signal, int jobNumber);
// prints out all the background running jobs
void printJobs();
// goes through all the children and calls waitpid on them, updating them if they finished
void checkBackgroundJobs();
// frees all the jobs at the end of the program (when exit is called)
void freeAllJobs();

// foreground job fxns
void setfgPid(pid_t pid);
pid_t getfgPid();
void clearfgPid();

// set and get job status
void setStatus(int stat);
int getStatus();

#endif
