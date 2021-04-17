#include <unistd.h>
// #include <error.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#include "compute.h"

KVPair_t *init_map(FileInfo info) {
  KVPair_t *map = (KVPair_t *)malloc(info.size);
  if (map == NULL) {
    printf("Could not initialize memory ");
    exit(EXIT_FAILURE);
  }
  int fd = openFileFromInfo(info, O_RDONLY);
  printf("init_map reading from %lu\n", info.startSector);
  check( read(fd, map, info.size) );
  close(fd);
  return map;
}

void exitMap(KVPair_t *map, FileInfo info) {
  int fd = openFileFromInfo(info, O_WRONLY);
  int ret = write(fd, map, info.size);
  if (ret != info.size) {
    printf("Write Failed\n");
    exit(1);
  }

  ret = fsync(fd);
  if (ret < 0) {
    printf("Sync failed\n");
    exit(1);
  }

  ret = close(fd);
  if (ret < 0) {
    printf("Close failed\n");
    exit(1);
  }
}

int addRunsCompute(int numInputs, 
                   FileInfo inputFileInfo[MAX_FILES], 
                   FileInfo outputFileInfo, 
                   bool lastLevel) {
  KVPair_t **input_maps = (KVPair_t **)malloc(numInputs * (sizeof(KVPair_t *)));
  KVPair_t *output_map;
  int compute_j;
  int *heads = (int *)malloc(sizeof(int) * numInputs);

  // move this to compute.
  int num_ip_files = numInputs;

  for (int curr = 0; curr < num_ip_files; curr++) {
    input_maps[curr] = init_map(inputFileInfo[curr]);
    heads[curr] = 0;
  }

  output_map = init_map(outputFileInfo);

  // StaticHeap h = StaticHeap((int) k, KVINTPAIRMAX);
  StaticHeap h;
  initStaticHeap(&h, numInputs, KVINTPAIRMAX);

  for (int i = 0; i < numInputs; i++) {
    KVPair_t kvp;
    memcpy(&kvp, input_maps[i], sizeof(KVPair_t));
    push(&h, (KVIntPair_t){kvp, i});
  }

  int j = -1;
  // TODO change int type to new datatype on a different key type
  //        K lastKey = INT_MAX;
  int32_t lastKey = INT_MAX;
  unsigned lastk = INT_MIN;
  while (h.size != 0) {
    KVIntPair_t val_run_pair = pop(&h);
    assert(isNotEqualPair(val_run_pair, KVINTPAIRMAX));  // TODO delete asserts
    if (lastKey == val_run_pair.first.key) {
      if (lastk < val_run_pair.second) {
        memcpy(output_map + j, &val_run_pair.first, sizeof(KVPair_t));
      }
    } else {
      ++j;
      KVPair_t tmp;
      if (j != -1) {
        memcpy(&tmp, output_map + j, sizeof(KVPair_t));
      }
      if (j != -1 && lastLevel && tmp.value == V_TOMBSTONE) {
        --j;
      }
      memcpy(output_map + j, &val_run_pair.first, sizeof(KVPair_t));
      //                output_map[j] = val_run_pair.first;
    }

    lastKey = val_run_pair.first.key;
    lastk = val_run_pair.second;

    unsigned k = val_run_pair.second;
    if (++heads[k] < inputFileInfo[k].size / sizeof(KVPair_t)) {
      //             KVPair_t kvp = input_maps[k][heads[k]];
      KVPair_t kvp;
      memcpy(&kvp, input_maps[k] + heads[k], sizeof(KVPair_t));
      push(&h, (KVIntPair_t){kvp, k});
    }
  }

  KVPair_t tmp;
  memcpy(&tmp, output_map + j, sizeof(KVPair_t));
  if (lastLevel && tmp.value == V_TOMBSTONE) {
    --j;
  }

#if 0
  // move these two functions to lsm.hpp as we want to not port
  // fence pointer logic to compute process at the moment.
  // TODO: revisit this design decision.
  runs[_activeRun]->setCapacity(j + 1);
  runs[_activeRun]->constructIndex();
  
  _prevRun = _activeRun;
  if(j + 1 > 0){
    ++_activeRun;
  }
#endif

  // write output data back to disk
  exitMap(output_map, outputFileInfo);
  // input maps were read only. they are freed by call to distructor
  for (int i = 0; i < numInputs; i++) {
    free(input_maps[i]);
  }
  free(input_maps);
  free(heads);
  free(output_map);
  free(h.arr);
  return j;
}

int compute(int read_fd, int numItems) {

  FileInfo inputFileInfo[MAX_FILES];
  FileInfo outputFileInfo;
  int lastLevel;

  for (int i = 0; i < numItems; i++) {
    check( read(read_fd, inputFileInfo + i, sizeof(FileInfo)) );
  }
  check( read(read_fd, &outputFileInfo, sizeof(FileInfo)) );
  check( read(read_fd, &lastLevel, sizeof(int)) );

  int j = addRunsCompute(numItems, inputFileInfo, outputFileInfo, lastLevel);

  return j;
}