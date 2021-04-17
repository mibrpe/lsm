#ifndef __COMPUTE_H__
#define __COMPUTE_H__

#include "../diskSectors.h"
#include "lsm.h"
#define MAX_FILES 1000

static void check(int val) {
  if (val <= 0) {
    perror("read:");
    exit(1);
  }
}

void exitMap(KVPair_t *map, FileInfo info);
KVPair_t *init_map(FileInfo info);

int compute(int read_fd, int numItems);

int addRunsCompute(int numInputs, 
                   FileInfo inputFileInfo[MAX_FILES], 
                   FileInfo outputFileInfo, 
                   bool lastLevel);


#endif // __COMPUTE_H__
