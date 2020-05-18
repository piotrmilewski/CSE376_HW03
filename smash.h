#ifndef SMASH
#define SMASH

#include "baseHeaders.h"
#include "parse.h"
#include "exec.h"
#include "debug.h"

void sigHandler(int signo);
int main(int argc, char **argv, char **envp);

#endif
