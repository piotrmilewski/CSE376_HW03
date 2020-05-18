#include "jobs.h"

static job *root = NULL;
static job *tail = NULL;
static int currJobNumber = 0;
static pid_t fgpid = 0;
static int last_status = 0;

// background jobs fxns
// inserts a new bg job into the array
void jobInsert(char *name, pid_t pid, int status){

  // create new job
  job *new = (job *)malloc(sizeof(job));
  // add name through array
  strcpy(new->name, name);
  new->pid = pid;
  new->jobNumber = currJobNumber++;
  new->status = status;
  new->exitStatus = -1;
  new->nextJob = NULL;
  // insert new job
  if (root == NULL){
    root = new;
    tail = new;
  }
  else{
    tail->nextJob = new;
    tail = new;
  }
}

// makes jobNumber job go to the foreground (use waitpid here)
void fg(int jobNumber){
  int child_status;
  job *node = root;
  job *prevNode = NULL;
  while (node != NULL){
    // find corresponding node and check if it's still running
    if (node->jobNumber == jobNumber && node->exitStatus == -1){
      // remove node from joblist
      if (prevNode == NULL)
        root = node->nextJob;
      else
        prevNode->nextJob = node->nextJob;
      // initiate foreground for bg
      setfgPid(node->pid);
      kill(node->pid, SIGCONT);
      pid_t tpid = waitpid(node->pid, &child_status, WUNTRACED);
      if (tpid < 0){
        perror("waitpid foreground");
        exit(EXIT_FAILURE);
      }
      clearfgPid();
      if (WIFEXITED(child_status)){
        setStatus(WEXITSTATUS(child_status));
        endPrompt(node->name, getStatus());
      }
      if (WIFSIGNALED(child_status)){
        printf(" Program aborted\n");
      }
      else if (WIFSTOPPED(child_status)){
        printf(" Program suspended\n");
        jobInsert(node->name, node->pid, 0);
      }
      free(node);
      node = NULL;
    }
    else
      node = node->nextJob;
  }
}

// makes suspended job continue execution (send sigcont)
void bg(int jobNumber){
  job *node = root;
  while (node != NULL){
    // find the corresponding jobNumber and make sure it's suspended
    if (node->jobNumber == jobNumber && node->status == 0){
      kill(node->pid, SIGCONT);
      node->status = 1;
      node = NULL;
    }
    else{
      node = node->nextJob;
    }
  }
}

// send signal N to job jobNumber
void killJob(int signal, int jobNumber){
  job *node = root;
  while (node != NULL){
    if (node->jobNumber == jobNumber){
      kill(node->pid, signal);
      if (signal == SIGCONT){
        node->status = 1;
      }
      if (signal == SIGTSTP)
        node->status = 0;
      node = NULL;
    }
    else{
      node = node->nextJob;
    }
  }
}

// prints out all the background running jobs
void printJobs(){
  printf("job number, name, pid, status\n");
  job *node = root;
  job *prevNode = NULL;
  job *nextNode = NULL;
  job *finishedR = NULL;
  job *finishedT = NULL;
  while (node != NULL){
    if (node->exitStatus != -1){ // finished process
      nextNode = node->nextJob;
      // remove finished process
      if (prevNode == NULL)
        root = node->nextJob;
      else
        prevNode->nextJob = node->nextJob;
      // add it to list of finished nodes
      if (finishedR == NULL){
        finishedR = node;
        finishedT = node;
      }
      else{
        finishedT->nextJob = node;
        finishedT = node;
      }
      node->nextJob = NULL;
      node = nextNode;
    }
    else{
      printf("%d, %s, %d, %s\n", node->jobNumber, node->name, node->pid,
              node->status == 1 ? "running" : "suspended");
      prevNode = node;
      node = node->nextJob;
    }
  }
  job *toFree = NULL;
  while (finishedR != NULL){
    printf("job '%s' finished with exit status: %d\n", finishedR->name, finishedR->exitStatus);
    toFree = finishedR;
    finishedR = finishedR->nextJob;
    free(toFree);
  }
}

// goes through all the children and calls waitpid on them, updating them if they finished
void checkBackgroundJobs(){
  job *node = root;
  int status;
  int bgpid;
  while (node != NULL){
    if (node->exitStatus == -1){
      bgpid = waitpid(node->pid, &status, WNOHANG);
      if (bgpid < 0)
        perror("waitpid background");
      if (node->pid == bgpid){
        if (WIFEXITED(status)){
          node->exitStatus = WEXITSTATUS(status);
          endPrompt(node->name, node->exitStatus);
        }
        if (WIFSIGNALED(status)){
          printf("Background program aborted\n");
          node->exitStatus = WTERMSIG(status);
        }
        else if (WIFSTOPPED(status)){
          printf("Background Program suspended\n");
        }
        else if (WIFCONTINUED(status)){
          printf("Background Program continued\n");
        }
      }
    }
    node = node->nextJob;
  }
}


// frees all the jobs at the end of the program (when exit is called)
void freeAllJobs(){
  job *node;
  while (root != NULL){
    node = root; // store node to free
    root = root->nextJob; // move to the next node
    free(node); // free the stored node
  }
}

// foreground job fxns
void setfgPid(pid_t pid){
  fgpid = pid;
}

pid_t getfgPid(){
  return fgpid;
}

void clearfgPid(){
  fgpid = 0;
}

// set and get job status
void setStatus(int stat){
  last_status = stat;
}

int getStatus(){
  return last_status;
}

