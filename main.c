#include <stdio.h>
#include "CL/cl.h"

#define BUFFER_SIZE (50000 + 1)

#define DEFAULT_N_WORKERS 1024
#define DEFAULT_N_LOOPS 100000

#define NWORKERS_START 1000
#define NWORKERS_STEP 1000
#define NWORKERS_END 50000

#define LOOPCNT_START 10000
#define LOOPCNT_STEP 10000
#define LOOPCNT_END 500000

#define SOURCE_SIZE 1024

/*
 * be_* for bench specific structures.
 */

typedef struct _be_device {
  cl_platform_id platform_id;
  cl_device_id device_id;
  cl_context context;
  cl_command_queue command_queue;
} be_device;

typedef struct _be_kernel {
  cl_mem dev_buffer;
  cl_program program;
  cl_kernel kernel;
} be_kernel;

void check_success(cl_int rv, char* msg) {
  if (rv != CL_SUCCESS) {
    fprintf(stderr, "%s call failed with return code: %d\n", msg, rv);
    exit(EXIT_FAILURE);
  }
}

/*
 * Initialize an OpenCL device for our benchmarks.
 * Assuming only 1 platform and 1 device.
 */
cl_int be_device_init(be_device* dev) {
  cl_int rv;
  rv = clGetPlatformIDs(1, &(dev->platform_id), NULL);
  if (rv) return rv;
  rv = clGetDeviceIDs(dev->platform_id, CL_DEVICE_TYPE_ALL, 1, &(dev->device_id), NULL);
  if (rv) return rv;

  // Create the OpenCL context
  const cl_context_properties context_properties[] = {
    CL_CONTEXT_PLATFORM, (cl_context_properties)dev->platform_id, 0
  };
  dev->context = clCreateContext(context_properties, 1, &(dev->device_id), NULL, NULL, &rv);
  if (rv) return rv;

  // Create a command queue
  // RX480 doesn't allow device side queues (cf cl-info)
  const cl_command_queue_properties q_props[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
  dev->command_queue = clCreateCommandQueueWithProperties(dev->context, dev->device_id, q_props, &rv);
  if (rv) return rv;

  return 0;
}

cl_int be_device_release(be_device *dev) {
  cl_int rv;
  rv = clReleaseCommandQueue(dev->command_queue);
  if (rv) return rv;
  rv = clReleaseContext(dev->context);
  if (rv) return rv;
  return 0;
}

cl_int get_kernel(cl_context context, cl_device_id device_id, cl_mem dev_buffer, int loop_cnt, cl_kernel *kernel) {
  const char *opencl_source =
    "__kernel void kerntest(__global char* data, int loops_cnt) {"
    "  size_t id = get_global_id(0);"
    "  int tmp = data[id] - 32;"
    "  for (int i=0; i<loops_cnt; i++) {"
    "    tmp = (2*tmp + id) % 95;"
    "  }"
    "  data[id] = (char)(tmp + 32);"
    "}";

  // Create a program and kernel from the OpenCL source code
  cl_int rv;
  cl_program program = clCreateProgramWithSource(context, 1, &opencl_source, NULL, &rv);
  if (rv) return rv;
  rv = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  if (rv) return rv;

  *kernel = clCreateKernel(program, "kerntest", &rv);
  if (rv) return rv;
  rv = clSetKernelArg(*kernel, 0, sizeof(dev_buffer), &dev_buffer);
  if (rv) return rv;
  rv = clSetKernelArg(*kernel, 1, sizeof(loop_cnt), &loop_cnt);
  return rv;
}

cl_int execute_kernel(cl_command_queue queue, cl_kernel kernel, size_t n_workers, size_t* duration) {
  cl_event exec_ev;
  cl_int rv = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &n_workers, NULL, 0, NULL, &exec_ev);
  if (rv) return rv;
  clWaitForEvents(1, &exec_ev);

  // Profiling
  cl_ulong exec_start;
  cl_ulong exec_end;
  rv = clGetEventProfilingInfo(exec_ev, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &exec_start, NULL);
  if (rv) return rv;
  rv = clGetEventProfilingInfo(exec_ev, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &exec_end, NULL);
  if (rv) return rv;

  *duration = exec_end - exec_start;
  return 0;
}

cl_int do_bench_loops(cl_context context, cl_device_id device_id, cl_mem dev_buffer,
		      cl_command_queue queue, size_t n_workers,
		      int start, int end, int step) {
  cl_int rv;
  for (int loop_cnt = start; loop_cnt <= end; loop_cnt += step) {
    cl_kernel kernel;
    rv = get_kernel(context, device_id, dev_buffer, loop_cnt, &kernel);
    if (rv) return rv;

    size_t duration;
    rv = execute_kernel(queue, kernel, n_workers, &duration);
    if (rv) return rv;

    printf("%d\t%zd\t%lu\n", loop_cnt, n_workers, duration);
    fflush(stdout);
  }
  return 0;
}

cl_int do_bench_workers(cl_context context, cl_device_id device_id, cl_mem dev_buffer,
			cl_command_queue queue, int n_loops,
			int start, int end, int step) {
  cl_int rv;
  cl_kernel kernel;
  rv = get_kernel(context, device_id, dev_buffer, n_loops, &kernel);
  if (rv) return rv;

  for (size_t n_workers = start; n_workers <= end; n_workers += step) {
    size_t duration;
    rv = execute_kernel(queue, kernel, n_workers, &duration);
    if (rv) return rv;

    printf("%d\t%zd\t%lu\n", n_loops, n_workers, duration);
    fflush(stdout);
  }
  return 0;
}

int main() {
  cl_int rv; // OpenCL calls return value
  const char message[] = "Hello World!";
  char host_buffer[BUFFER_SIZE];
  int data_len = BUFFER_SIZE-1;

  // Filling host buffer with our message
  for (int i=0; i < data_len; i++) {
    host_buffer[i] = message[i % (sizeof(message) - 1)];
  }
  host_buffer[data_len] = '\0';

  be_device bench_dev;
  rv = be_device_init(&bench_dev);
  check_success(rv, "be_device_init");

  // Create a on-device memory buffer with default properties (i.e read-write no host-mapped)
  cl_mem dev_buffer = clCreateBuffer(bench_dev.context, 0, BUFFER_SIZE, NULL, &rv);
  check_success(rv, "clCreateBuffer");

  // Write initial data set into device buffer
  cl_event write_ev;
  rv = clEnqueueWriteBuffer(bench_dev.command_queue, dev_buffer, CL_FALSE, 0, data_len,
			    host_buffer, 0, NULL, &write_ev);
  check_success(rv, "clEnqueueWriteBuffer");
  clWaitForEvents(1, &write_ev);

  //rv = do_bench_loops(context, device_id, dev_buffer,
  //		      command_queue, DEFAULT_N_WORKERS,
  //		      LOOPCNT_START, LOOPCNT_END, LOOPCNT_STEP);
  //check_success(rv, "do_bench_loops");

  rv = do_bench_workers(bench_dev.context, bench_dev.device_id, dev_buffer,
			bench_dev.command_queue, DEFAULT_N_LOOPS,
			NWORKERS_START, NWORKERS_END, NWORKERS_STEP);
  check_success(rv, "do_bench_workers");

  // Enqueue blocking read command
  rv = clEnqueueReadBuffer(bench_dev.command_queue, dev_buffer, CL_TRUE, 0, data_len,
			   host_buffer, 0, NULL, NULL);
  check_success(rv, "clEnqueueReadBuffer");
  // printf("%s\n", host_buffer);

  // Free resources
  check_success(clReleaseMemObject(dev_buffer), "clReleaseMemObject");
  rv = be_device_release(&bench_dev);
  check_success(rv, "be_device_release");
}
