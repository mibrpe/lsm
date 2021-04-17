#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __DISKSECTORS_H__
#define __DISKSECTORS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
// #include <linux/fs.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FSMOUNTPOINT "/dev/sdb"

typedef struct {
  size_t offset;
  size_t startSector;
  size_t endSector;
  size_t size;
} FileInfo;

static FileInfo getFileInfo(const char *filename, size_t size) {
  char command[1024];
  sprintf(command, "hdparm --fibmap '%s' | tail -n 1", filename);
  FILE *f = popen(command, "r");
  if (f == NULL) {
    perror("open:");
    exit(1);
  }
  FileInfo info;
  fscanf(f, " %lu %lu %lu", &info.offset, &info.startSector, &info.endSector);
  fclose(f);
  info.size = size;
  return info;
}

/***
 * Given a struct with the start LBA and byte offset of a file, return a file
 * descriptor which has been seeked to the correct starting position of the 
 * file on disk.
 */
static int openFileFromInfo(FileInfo info, int mode) {
  int fd = open(FSMOUNTPOINT, mode);
  int blockSize = 512;
  // ioctl(fd, BLKSSZGET, &blockSize);
  // printf("blocksize: %d\n\n", blockSize);
  lseek(fd, blockSize * info.startSector + info.offset, SEEK_SET);
  return fd;
}

#endif // __DISKSECTORS_H__

#ifdef __cplusplus
}
#endif