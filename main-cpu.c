#include <time.h>
#include "benchlib.h"

#define DEFAULT_WORKERS_CNT 1
#define DEFAULT_LOOPS_CNT 1000

#define NWORKERS_START 100
#define NWORKERS_STEP 100
#define NWORKERS_END 10000

#define LOOPCNT_START 100000
#define LOOPCNT_STEP 100000
#define LOOPCNT_END 10000000

#define BUFFER_SIZE 10001

void kerntest(int id, char* data, int loops_cnt) {
  int tmp = data[id] - 32;
  for (int i=0; i<loops_cnt; i++) {
    tmp = (2*tmp + id) % 95;
  }
  data[id] = (char)(tmp + 32);
}

void do_bench_loops(FILE* out, char* buffer, int start, int end, int step) {
  for (int loops_cnt = start; loops_cnt <= end; loops_cnt += step) {
    clock_t t_start = clock();
    kerntest(0, buffer, loops_cnt);
    clock_t t_end = clock();
    unsigned long duration = (t_end - t_start) * 1000000000 / CLOCKS_PER_SEC; // ns
    fprintf(out, "%d\t%lu\n", loops_cnt, duration);
  }
}

void do_bench_workers(FILE* out, char* buffer, int start, int end, int step) {
  for (int n_workers = start; n_workers <= end; n_workers += step) {
    clock_t t_start = clock();
    for (int i=0; i<n_workers; i++) {
      kerntest(i, buffer, DEFAULT_LOOPS_CNT);
    }
    clock_t t_end = clock();
    unsigned long duration = (t_end - t_start) * 1000000000 / CLOCKS_PER_SEC;
    fprintf(out, "%d\t%lu\n", n_workers*DEFAULT_LOOPS_CNT, duration);
  }
}

int main() {
  FILE* fd;
  char host_buffer[BUFFER_SIZE];
  int data_len = sizeof(host_buffer);
  init_host_buffer(host_buffer, data_len);

  printf("Benching CPU loops ...");
  fflush(stdout);
  fd = fopen("bench_cpu_loops.txt", "w");
  check_fd_open(fd);
  do_bench_loops(fd, host_buffer, LOOPCNT_START, LOOPCNT_END, LOOPCNT_STEP);
  check_fd_close(fclose(fd));
  printf(" done\n");

  printf("Benching CPU workers ...");
  fflush(stdout);
  fd = fopen("bench_cpu_workers.txt", "w");
  check_fd_open(fd);
  do_bench_workers(fd, host_buffer, NWORKERS_START, NWORKERS_END, NWORKERS_STEP);
  check_fd_close(fclose(fd));
  printf(" done\n");
}
