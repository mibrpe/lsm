#include <assert.h>
#include <errno.h>
// #include <error.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 4096

#include "compute.h"

int main(int argc, char *argv[]) {
  const char *app_reader = "/tmp/app_reader";
  const char *app_writer = "/tmp/app_writer";

  int read_fd, write_fd;
  char buf[BUFSIZE] = {'\n'};
  int count = 0;
  int j;

  if (access(app_writer, F_OK)) {
    mkfifo(app_writer, 0666);
  }

  if (access(app_reader, F_OK)) {
    mkfifo(app_reader, 0666);
  }

  read_fd = open(app_writer, O_RDONLY);
  if (read_fd < 0) {
    printf("%s:Could not open %s for reading %s\n", argv[0], app_writer,
           strerror(errno));
    exit(1);
  }

  printf("opened read_fd\n");
  // write computation output
  write_fd = open(app_reader, O_WRONLY);
  if (write_fd < 0) {
    printf("%s:Could not open %s for writing %s\n", argv[0], app_reader,
           strerror(errno));
    exit(1);
  }

  printf("opened write_fd\n");

  // read application data
  while (1) {
    printf("Start Read\n");

    int numInputs = 0;
    check( read(read_fd, &numInputs, sizeof(int)) );
  
    printf("read bytes %d\n", numInputs);
    // printf("All Reads Complete\n");
    j = compute(read_fd, numInputs);

    printf("computation complete, writing output \n");
    count = j;
    int result = write(write_fd, &count, sizeof(int));
    if (result < 0) {
      printf("%s:Could not write %s\n", argv[0], strerror(errno));
      exit(1);
    }
  }

  close(read_fd);
  close(write_fd);

  printf("done\n");
}
