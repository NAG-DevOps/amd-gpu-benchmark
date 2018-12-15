#include "stdio.h"

#define N_STREAM_PROCESSORS 2304
#define HOST_BUFFER_SIZE 4096

void kerntest(char* data, int id) {
  int tmp = data[id] - 32;
  for (int i=0; i<10000000; i++) {
    tmp = (2*tmp + id) % 95;
  }
  data[id] = (char)(tmp + 32);
}

int main() {
  const char message[] = "Hello World!";
  int msg_len = sizeof(message) - 1;
  char host_buffer[HOST_BUFFER_SIZE];

  // Filling host buffer with our message
  for (int i=0; i<N_STREAM_PROCESSORS; i++) {
    host_buffer[i] = message[i % msg_len];
  }
  host_buffer[N_STREAM_PROCESSORS] = '\0';

  for (int i=0; i<N_STREAM_PROCESSORS; i++) {
    kerntest(host_buffer, i);
  }

  printf("Message read: %s\n", host_buffer);
}
