#ifndef EXEC
#define EXEC

#include "baseHeaders.h"
#include "parse.h"
#include "jobs.h"
#include "redirect.h"
#include "debug.h"

void run_execvp(char **cmdArgs, int numArgs);

// void return # if builtin is run such as exit
// retVal 0 = no special case
// retVal 1 = exit
// retVal 2 = special case
int run_builtIn(char **cmdArgs, int numArgs, char **envp);

// return 0 on default
// return 1 if user specified to exit
int exec(char *input, char **envp);

#endif
