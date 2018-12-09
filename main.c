#include "stdio.h"
#include "CL/cl.h"

#define BUFFER_SIZE 1024
#define MAX_DEVICES_ENTRIES 4

void check_success(cl_int rv, char* msg) {
  if (rv != CL_SUCCESS) {
    fprintf(stderr, "%s call failed with return code: %d)\n", msg, rv);
    exit(EXIT_FAILURE);
  }
}

int main() {
  // Assuming only 1 platform and 1 device
  cl_platform_id platform_id;
  cl_device_id device_id;
  check_success(clGetPlatformIDs(1, &platform_id, NULL), "clGetPlatformIDs");
  check_success(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, NULL), "clGetDeviceIDs");
}
