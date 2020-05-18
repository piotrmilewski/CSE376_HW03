#include "debug.h"

static int dbg_flag = 0;

void setFlag(){
  dbg_flag = 1;
}

int getFlag(){
  return dbg_flag;
}
