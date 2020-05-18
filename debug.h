#ifndef DEBUG
#define DEBUG

#include "baseHeaders.h"

#define runPrompt(cmd) \
  if (getFlag() == 1) \
    fprintf(stderr, "RUNNING: %s\n", cmd); \

#define endPrompt(cmd, exitStatus) \
  if (getFlag() == 1) \
    fprintf(stderr, "ENDED: %s (ret=%d)\n", cmd, exitStatus) \
    

void setFlag();
int getFlag();

#endif
