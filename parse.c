#include "parse.h"

int parse_input(char *input, char **cmdArgs){

  int index = 0;
  char *found;
  int prevIndex;

  // get the first word
  found = strtok(input, " ");

  // get the rest of the words
  while (NULL != found){
    if (strlen(found) >= 3 && found[0] == '*' && found[1] == '.'){
      prevIndex = index;
      index = globEC(found+1, cmdArgs, index);
      if (prevIndex == index){
        cmdArgs[index] = found;
        index++;
      }
    }
    else{
      cmdArgs[index] = found;
      index++;
    }
    found = strtok(NULL, " ");
  }

  // store an empty string at the end
  cmdArgs[index] = NULL;

  return index; // return num of cmdArgs
}

int globEC(char *extension, char **cmdArgs, int currIndex){
  // open dirName directory
  struct dirent *entry = NULL;
  DIR *dr = opendir(".");
  if (dr == NULL){
    perror("opendir");
    exit(EXIT_FAILURE);
  }

  // go through dirName directory and add extension files
  while ((entry = readdir(dr)) != NULL){
    if (strstr(entry->d_name, extension) != NULL){
      cmdArgs[currIndex] = entry->d_name;
      currIndex++;
    }
  }

  closedir(dr);
  
  return currIndex;
}
