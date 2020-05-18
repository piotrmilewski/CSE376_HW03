#include "exec.h"

void run_execvp(char **cmdArgs, int numArgs){

  // check if user specified for background process
  int bg = 0;
  if (strcmp(cmdArgs[numArgs-1], "&") == 0){
    bg = 1;
    cmdArgs[numArgs-1] = NULL; // get rid of & in cmd
  }

  int child_status;
  pid_t pid = fork();
  
  // check if fork failed
  if (pid < 0)
    perror("Fork failure, not executing command\n");
  
  // child
  else if (pid == 0) {
    if (execvp(cmdArgs[0], cmdArgs) < 0){
      printf("Invalid command\n");
      exit(0);
    }
  }
  // parent
  else{
    // turn array into name
    char name[1024];
    strcpy(name, cmdArgs[0]);
    int i = 1;
    while (cmdArgs[i] != NULL){
      strcat(name, " ");
      strcat(name, cmdArgs[i]);
      i++;
    }
    runPrompt(name);
    // if program is in foreground
    if (bg == 0){ 
      setfgPid(pid);
      pid_t tpid = waitpid(pid, &child_status, WUNTRACED);
      if (tpid < 0){
        perror("waitpid foreground");
        exit(EXIT_FAILURE);
      }
      clearfgPid();
      if (WIFEXITED(child_status)){
        setStatus(WEXITSTATUS(child_status));
        endPrompt(name, getStatus());
      }
      if (WIFSIGNALED(child_status)){
        printf(" Program aborted\n");
      }
      else if (WIFSTOPPED(child_status)){
        printf(" Program suspended\n");
        jobInsert(name, pid, 0);
      }
    }
    // if program is in background
    else{
      jobInsert(name, pid, 1); 
    }
  }
}

int run_builtIn(char **cmdArgs, int numArgs, char **envp){

  // turn array into name
  char name[1024];
  strcpy(name, cmdArgs[0]);
  int i = 1;
  while (cmdArgs[i] != NULL){
    strcat(name, " ");
    strcat(name, cmdArgs[i]);
    i++;
  }

  int retVal;
  // "exit" command
  if (strcmp("exit", cmdArgs[0]) == 0){
    runPrompt(name);
    endPrompt(name, 0);
    return 1; // return 1 if user specified to exit
  }
  // "cd" command
  else if (strcmp("cd", cmdArgs[0]) == 0){
    runPrompt(name);
    retVal = chdir(cmdArgs[1]);
    if (retVal < 0)
      printf("Invalid Directory\n");
    endPrompt(name, retVal);
    return 2;
  }
  else if (strcmp("pwd", cmdArgs[0]) == 0){
    runPrompt(name);
    char buf[getpagesize()];
    if (getcwd(buf, sizeof(buf)) == NULL){
      printf("Error getting current working directory\n");
      return 2;
    }
    printf("%s\n", buf);
    endPrompt(name, 0);
    return 2;
  }
  else if (strcmp("jobs", cmdArgs[0]) == 0){
    runPrompt(name);
    printJobs();
    endPrompt(name, 0);
    return 2;
  }
  else if (strcmp("fg", cmdArgs[0]) == 0 && numArgs >= 2){
    runPrompt(name);
    fg(atoi(cmdArgs[1]));
    endPrompt(name, 0);
    return 2;
  }
  else if (strcmp("bg", cmdArgs[0]) == 0 && numArgs >= 2){
    runPrompt(name);
    bg(atoi(cmdArgs[1]));
    endPrompt(name, 0);
    return 2;
  }
  else if (strcmp("kill", cmdArgs[0]) == 0 && numArgs >= 3){
    runPrompt(name);
    char *number = cmdArgs[1] + 1;
    killJob(atoi(number), atoi(cmdArgs[2]));
    endPrompt(name, 0);
    return 2;
  }
  else if (strcmp("echo", cmdArgs[0]) == 0 && numArgs >= 2){
    runPrompt(name);

    // convert variables to actual info
    int i = 1;
    while (cmdArgs[i] != NULL){
      if (cmdArgs[i][0] == '$'){
        if (strlen(cmdArgs[i]) >= 2 && cmdArgs[i][1] == '?')
          printf("%d ", getStatus());
        else{
          int lenOfVar = strlen(cmdArgs[i]+1);
          int j = 0;
          while (envp[j] != NULL){
            if (strncmp(cmdArgs[i]+1, envp[j], lenOfVar) == 0)
              printf("%s ", envp[j] + lenOfVar + 1);
            j++;
          }
        }
      }
      else
        printf("%s ", cmdArgs[i]);
      i++;
    }
    printf("\n");
    endPrompt(name, 0);
    return 2;
  }
  else if (cmdArgs[0][0] == '#'){
    return 2;
  }

  return 0;
}

int exec(char *input, char **envp){

  char *cmdArgs[1024]; // regular terminal doesn't accept more than 1024 characters
  // separate words in input by " " into 2D array
  int numArgs = parse_input(input, cmdArgs);

  // check for blank input
  if (numArgs == 0)
    return 0;

  // set up file redirection
  if (handleRedirects(cmdArgs, &numArgs) < 0)
    return -1;

  int retVal = run_builtIn(cmdArgs, numArgs, envp);
  if (0 != retVal) // 0 means no builtin was run by the user
    return retVal; // let shell deal with special case
  
  run_execvp(cmdArgs, numArgs);

  // reset file redirection
  if (resetRedirects() < 0)
    return -1;

  return 0; // default return
}
