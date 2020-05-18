#include "smash.h"

void sigHandler(int signo){
  int fgPid = getfgPid();
  if (fgPid != 0){
    if (signo == SIGINT)
      kill(fgPid, SIGINT);
    else if (signo == SIGTSTP)
      kill(fgPid, SIGTSTP);
  }
}

// main method
// runs the shell program
int main(int argc, char **argv, char **envp){

  // setup signal handler
  struct sigaction act;
  act.sa_handler = &sigHandler;
  sigfillset(&act.sa_mask); // block all signals while sighandler working
  act.sa_flags = SA_RESTART; // restart fxns interrupted by sighandler
  if (sigaction(SIGINT, &act, NULL) == -1){
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGTSTP, &act, NULL) == -1){
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  // initialize mask for blocking all signals
  sigset_t mask;
  if (sigfillset(&mask) < 0){
    perror("sigfillset");
    exit(EXIT_FAILURE);
  }
 
  // determine whether shell will be non-interactive
  int interactive = 1; // 1 = interactive, 2 = non-interactive
  FILE *fp;
  struct stat fileStat;
  int index = 0;
  if (argc > 1){ // non-interactive
    if (stat(argv[1], &fileStat) == 0) // if script specified is 1st arg
      index = 1;
    else if (argc > 2 && stat(argv[2], &fileStat) == 0) // if script specified is 2nd arg
      index = 2;
    if (index != 0){
      fp = fopen(argv[index], "r");
      if (fp == NULL){
        perror("fopen error opening file");
        return -1;
      }
      interactive = 0;
    }
  }

  // check if debugging has been specified
  if (argc > 1) {
    if (strcmp(argv[1], "-d") == 0)
      setFlag();
    else if (argc > 2 && strcmp(argv[2], "-d") == 0)
      setFlag();
  }

  int pageSize = getpagesize();
  char input[pageSize];
  int runSmash = 1;
  // loop to execute the shell
  while (runSmash){
   
    // clear input
    input[0] = '\0';

    // print prompt and get the input from the user
    if (interactive == 1){
      printf("smash> ");
      if (NULL == fgets(input, sizeof(input), stdin)){
        runSmash = 0;
        break;
      }
    }
    else{
      if (NULL == fgets(input, sizeof(input), fp)){
        runSmash = 0;
        break;
      }
    }

    // get rid of newline at the end of input
    input[strlen(input)-1] = '\0';

    // block all signals here
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0){
      perror("sigprocmask block");
      exit(EXIT_FAILURE);
    }

    // code that checks if background jobs finished
    checkBackgroundJobs();

    // unblock signals here
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0){
      perror("sigprocmask unblock");
      exit(EXIT_FAILURE);
    }

    // execute the input provided by the user
    if (exec(input, envp) == 1)
      runSmash = 0;
  }

  freeAllJobs(); // free all jobs that are left over

  return 0;
}
