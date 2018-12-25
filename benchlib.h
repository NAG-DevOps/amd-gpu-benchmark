#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 * Some functions common to main and main-cpu
 */
void check_fd_open(FILE* fd);
void check_fd_close(int rv);
void init_host_buffer(char* buffer, int length);
