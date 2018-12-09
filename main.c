#include "stdio.h"
#include "CL/cl.h"

#define DEVICE_BUFFER_SIZE 1024
#define HOST_BUFFER_SIZE 1024

void check_success(cl_int rv, char* msg) {
  if (rv != CL_SUCCESS) {
    fprintf(stderr, "%s call failed with return code: %d\n", msg, rv);
    exit(EXIT_FAILURE);
  }
}

int main() {
  cl_int rv; // OpenCL calls return value
  const char message[] = "Hello World !";
  char host_buffer[HOST_BUFFER_SIZE];

  // Assuming only 1 platform and 1 device
  cl_platform_id platform_id;
  check_success(clGetPlatformIDs(1, &platform_id, NULL), "clGetPlatformIDs");
  cl_device_id device_id;
  check_success(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, NULL), "clGetDeviceIDs");

  // Create the OpenCL context
  const cl_context_properties context_properties[] = {
    CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0
  };
  cl_context context = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &rv);
  check_success(rv, "clCreateContext");

  // Create a command queue (default properties - i.e in-order host command queue)
  // It looks like the RX480 doesn't allow device size queues (cf cl-info)
  cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &rv);
  check_success(rv, "clCreateCommandQueueWithProperties");

  // Create a on-device memory buffer with default properties (i.e read-write no host-mapped)
  cl_mem dev_buffer = clCreateBuffer(context, 0, DEVICE_BUFFER_SIZE, NULL, &rv);
  check_success(rv, "clCreateBuffer");

  // Write to, then Read from on-device memory (buffer)
  check_success(clEnqueueWriteBuffer(command_queue, dev_buffer, CL_TRUE, 0, sizeof(message), message, 0, NULL, NULL),
		"clEnqueueWriteBuffer");
  check_success(clEnqueueReadBuffer(command_queue, dev_buffer, CL_TRUE, 0, sizeof(message), host_buffer, 0, NULL, NULL),
		"clEnqueueReadBuffer");
  printf("Message read: %s\n", host_buffer);

  // Free resources
  check_success(clReleaseMemObject(dev_buffer), "clReleaseMemObject");
  check_success(clReleaseCommandQueue(command_queue), "clReleaseCommandQueue");
  check_success(clReleaseContext(context), "clReleaseContext");
}
