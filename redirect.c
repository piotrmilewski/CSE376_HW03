#include "redirect.h"

static int out = -1;
static int err = -1;
static int in = -1;
static int save_out = -1;
static int save_err = -1;
static int save_in = -1;

// handle redirects
int handleRedirects(char **cmdArgs, int *numArgs){
  char *outfile = NULL;
  char *errfile = NULL;
  char *infile = NULL;
  int delete = 0;
  // determine which file redirects need to be done
  for (int i = 0; i < *numArgs; i++){
    if (cmdArgs[i][0] == '>'){
      outfile = cmdArgs[i]+1;
      delete = 1;
      *numArgs = *numArgs - 1;
    }
    else if (cmdArgs[i][0] == '<'){
      infile = cmdArgs[i]+1;
      delete = 1;
      *numArgs = *numArgs - 1;
    }
    else if (strlen(cmdArgs[i]) >= 2 && cmdArgs[i][0] == '2' && cmdArgs[i][1] == '>'){
      errfile = cmdArgs[i]+2;
      delete = 1;
      *numArgs = *numArgs - 1;
    }
    if (delete == 1){
      int x = i;
      while (cmdArgs[x] != NULL){
        cmdArgs[x] = cmdArgs[x+1];
        x++;
      }
      i--;
      delete = 0;
    }
  }
  
  // handle outfile
  if (outfile != NULL){
    out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (out < 0){
      perror("open outfile");
      return -1;
    }

    save_out = dup(fileno(stdout));

    if (dup2(out, fileno(stdout)) < 0){
      perror("dup2 stdout");
      return -1;
    }
  }

  // handle errfile
  if (errfile != NULL){
    err = open(errfile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (err < 0){
      perror("open errfile");
      return -1;
    }

    save_err = dup(fileno(stderr));

    if (dup2(err, fileno(stderr)) < 0){
      perror("dup2 stderr");
      return -1;
    }
  }

  // handle infile
  if (infile != NULL){
    in = open(infile, O_RDONLY);
    if (in < 0){
      perror("open infile");
      return -1;
    }

    save_in = dup(fileno(stdin));

    if (dup2(in, fileno(stdin)) < 0){
      perror("dup2 stdin");
      return -1;
    }
  }

  return 0;
}

// reset redirects
int resetRedirects(){
  // reset stdout
  if (out != -1){
    fflush(stdout);
    close(out);

    if (dup2(save_out, fileno(stdout)) < 0){
      perror("dup2 reset stdout");
      return -1;
    }

    close(save_out);
    out = -1;
    save_out = -1;
  }

  // reset stderr
  if (err != -1){
    fflush(stderr);
    close(err);

    if (dup2(save_err, fileno(stderr)) < 0){
      perror("dup2 reset stderr");
      return -1;
    }

    close(save_err);
    err = -1;
    save_err = -1;
  }

  // reset stdin
  if (in != -1){
    fflush(stdin);
    close(in);

    if (dup2(save_in, fileno(stdin)) < 0){
      perror("dup2 reset stdin");
      return -1;
    }

    close(save_in);
    in = -1;
    save_in = -1;
  }
  return 0;
}
