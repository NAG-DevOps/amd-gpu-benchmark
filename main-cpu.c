#include "stdio.h"

#define N_STREAM_PROCESSORS 9216
#define BUFFER_SIZE (N_STREAM_PROCESSORS + 1)

void kerntest(char* data, int id) {
  int tmp = data[id] - 32;
  for (int i=0; i<1000000; i++) {
    tmp = (2*tmp + id) % 95;
  }
  data[id] = (char)(tmp + 32);
}

int main() {
  const char message[] = "Hello World!";
  char host_buffer[BUFFER_SIZE];
  int data_len = BUFFER_SIZE-1;

  // Filling host buffer with our message
  for (int i=0; i < data_len; i++) {
    host_buffer[i] = message[i % (sizeof(message) - 1)];
  }
  host_buffer[data_len] = '\0';

  for (int i=0; i<N_STREAM_PROCESSORS; i++) {
    kerntest(host_buffer, i);
  }

  printf("Message read: %s\n", host_buffer);
}
