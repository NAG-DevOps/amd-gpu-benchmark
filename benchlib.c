#include "benchlib.h"

void check_fd_open(FILE* fd) {
  if (!fd) {
    perror(NULL);
    exit(EXIT_FAILURE);
  }
}

void check_fd_close(int rv) {
  if (rv) {
    perror(NULL);
    exit(EXIT_FAILURE);
  }
}

void init_host_buffer(char* buffer, int length) {
  const char message[] = "Hello World!";
  for (int i=0; i < length; i++) {
    buffer[i] = message[i % (sizeof(message) - 1)];
  }
  buffer[length-1] = '\0';
}
