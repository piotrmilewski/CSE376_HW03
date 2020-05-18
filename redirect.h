#ifndef REDIRECT
#define REDIRECT

#include "baseHeaders.h"

// handle redirects
int handleRedirects(char **cmdArgs, int *numArgs);
// reset redirects
int resetRedirects();

#endif
