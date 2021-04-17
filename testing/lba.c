#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
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
  char command[256];
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
  int blockSize = 256;
  ioctl(fd, BLKSSZGET, &blockSize);
  lseek(fd, blockSize * info.startSector + info.offset, SEEK_SET);
  return fd;
}


int main(int argc, char **argv) {
  char *fname = "file.txt";
  if (argc > 1) fname = argv[1];

  char buf[256];
  FileInfo f = getFileInfo(fname, 0);
  printf("%lu %lu %lu\n", f.startSector, f.offset, f.size);
  int fd;  

  printf("---------------- BEFORE: READING FILE WITH LBA ---------------\n");

  fd = openFileFromInfo(f, O_RDONLY);
  read(fd, buf, 256);
  printf("%s\n\n", buf);
  close(fd);

  printf("-------------------- WRITING TO FILE -------------------------\n");

  f = getFileInfo("file.txt", 0);
  fd = openFileFromInfo(f, O_RDWR);
  write(fd, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n", 50);
  fsync(fd);
  close(fd);

  printf(" Done writing.\n\n");
  printf("---------- READING FILE WITH FOPEN / FILENAME----------------\n");

  FILE *fp = fopen("file.txt", "r");
  fread(buf, 256, 1, fp);
  printf("%s\n\n", buf);
  close(fd);

  printf("---------------- READING FILE WITH LBA ---------------------\n");

  fd = openFileFromInfo(f, O_RDONLY);
  read(fd, buf, 256);
  printf("%s\n\n", buf);
  close(fd);

  return 0;
}
