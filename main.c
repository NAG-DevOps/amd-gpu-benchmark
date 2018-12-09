#include "stdio.h"
#include "CL/cl.h"

#define BUFFER_SIZE 1024
#define MAX_PLATFORMS_ENTRIES 4
#define MAX_DEVICES_ENTRIES 4

int display_platforms_ids() {
  cl_uint platforms_cnt;
  cl_platform_id platforms_ids[MAX_PLATFORMS_ENTRIES];

  cl_int res = clGetPlatformIDs(MAX_PLATFORMS_ENTRIES, platforms_ids, &platforms_cnt);
  if (res != CL_SUCCESS) return res;

  printf("Platforms count: %d\n", platforms_cnt);
  for (int i=0; i < platforms_cnt; ++i) {
    printf("* Id: %p\n", platforms_ids[i]);
  }
  return CL_SUCCESS;
}


int display_devices_ids(cl_platform_id platform) {
  cl_uint devices_cnt;
  cl_device_id devices_ids[MAX_DEVICES_ENTRIES];

  cl_int res = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, MAX_DEVICES_ENTRIES,
			      devices_ids, &devices_cnt);
  if (res != CL_SUCCESS) return res;

  printf("Devices count: %d\n", devices_cnt);
  for (int i=0; i < devices_cnt; ++i) {
    printf("* Id: %p\n", devices_ids[i]);
  }
  return CL_SUCCESS;
}


int display_platform_infos(cl_platform_id platform_id) {
  cl_int res;
  char buffer[BUFFER_SIZE]; // Buffer used to retrieve info strings

  int get_info(cl_platform_info info) {
    return clGetPlatformInfo(platform_id, info, BUFFER_SIZE, buffer, NULL);
  }

  printf("Platform ID: %p\n", platform_id);

  if ((res = get_info(CL_PLATFORM_PROFILE)) != CL_SUCCESS) return res;
  printf("* Profile: %s\n", buffer);
  if ((res = get_info(CL_PLATFORM_VERSION)) != CL_SUCCESS) return res;
  printf("* Version: %s\n", buffer);
  if ((res = get_info(CL_PLATFORM_NAME)) != CL_SUCCESS) return res;
  printf("* Name: %s\n", buffer);
  if ((res = get_info(CL_PLATFORM_VENDOR)) != CL_SUCCESS) return res;
  printf("* Vendor: %s\n", buffer);
  if ((res = get_info(CL_PLATFORM_EXTENSIONS)) != CL_SUCCESS) return res;
  printf("* Extensions: %s\n", buffer);

  return CL_SUCCESS;
}


int display_device_infos(cl_device_id device_id) {
  cl_int rv;

  cl_device_info ulong_infos[] = {
    CL_DEVICE_VENDOR_ID,
    CL_DEVICE_MAX_COMPUTE_UNITS,
    CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
    CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,
    CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,
    CL_DEVICE_MAX_CLOCK_FREQUENCY,
    CL_DEVICE_ADDRESS_BITS,
    CL_DEVICE_MAX_MEM_ALLOC_SIZE,
    CL_DEVICE_MEM_BASE_ADDR_ALIGN,
    CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
    CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
    CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
    CL_DEVICE_GLOBAL_MEM_SIZE,
    CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,
    CL_DEVICE_MAX_CONSTANT_ARGS,
    CL_DEVICE_LOCAL_MEM_TYPE,
    CL_DEVICE_LOCAL_MEM_SIZE,
    CL_DEVICE_ERROR_CORRECTION_SUPPORT,
    CL_DEVICE_ENDIAN_LITTLE,
    CL_DEVICE_AVAILABLE,
    CL_DEVICE_COMPILER_AVAILABLE,
    CL_DEVICE_LINKER_AVAILABLE,
    CL_DEVICE_EXECUTION_CAPABILITIES,
    CL_DEVICE_QUEUE_ON_HOST_PROPERTIES,
    CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES,
    CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE,
    CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE,
    CL_DEVICE_MAX_ON_DEVICE_QUEUES,
    CL_DEVICE_PRINTF_BUFFER_SIZE,
  };
  char* ulong_info_strings[] = {
    "Vendor ID",
    "Max compute units",
    "Max work item dimensions",
    "Preferred vector width int",
    "Preferred vector width short",
    "Max clock frequency (MHz)",
    "Address bits",
    "Max memory allocation size (Bytes)",
    "Memory base address alignment",
    "Global memory cache type",
    "Global memory cacheline size (Bytes)",
    "Global memory cache size (Bytes)",
    "Global memory size (Bytes)",
    "Max constant buffer size (Bytes)",
    "Max constant arguments",
    "Local memory type",
    "Local memory size (Bytes)",
    "Error correction support",
    "Little endian device",
    "Device Available",
    "Compiler available",
    "Linker available",
    "Execution capabilities",
    "Queue on host properties",
    "Queue on device properties",
    "Queue on device preferred size (Bytes)",
    "Queue on device max size (Bytes)",
    "Max queues per context",
    "Printf buffer size (Bytes)",
  };

  printf("Device ID: %p\n", device_id);

  // uint, ulong, bool, bitfields infos
  for (int i=0; i < (sizeof(ulong_infos) / sizeof(cl_device_info)); ++i) {
    cl_ulong ulong_val = 0;
    rv = clGetDeviceInfo(device_id, ulong_infos[i], sizeof(ulong_val), &ulong_val, NULL);
    if (rv != CL_SUCCESS) return rv;
    printf("* %s: %lu\n", ulong_info_strings[i], ulong_val);
  }

  cl_device_info string_infos[] = {
    CL_DEVICE_BUILT_IN_KERNELS,
    CL_DEVICE_NAME,
    CL_DEVICE_VENDOR,
    CL_DRIVER_VERSION,
    CL_DEVICE_PROFILE,
    CL_DEVICE_VERSION,
    CL_DEVICE_OPENCL_C_VERSION,
    CL_DEVICE_EXTENSIONS,
  };
  char* string_info_strings[] = {
    "Built in kernels",
    "Name",
    "Vendor",
    "Driver version",
    "Profile",
    "Device version",
    "OpenCL C version",
    "Extensions",
  };

  // uint, ulong, bool, bitfields infos
  for (int i=0; i < (sizeof(string_infos) / sizeof(cl_device_info)); ++i) {
    char buffer[BUFFER_SIZE];
    rv = clGetDeviceInfo(device_id, string_infos[i], BUFFER_SIZE, buffer, NULL);
    if (rv != CL_SUCCESS) return rv;
    printf("* %s: %s\n", string_info_strings[i], buffer);
  }

  return CL_SUCCESS;
}


int main() {
  cl_int res; // cl API calls return value
  cl_platform_id platform_id;
  cl_device_id device_id;

  // Displaying platforms IDs
  if ( (res = display_platforms_ids()) != CL_SUCCESS) {
    fprintf(stderr, "display_platforms_ids call failed with return code: %d\n", res);
    exit(EXIT_FAILURE);
  }
  printf("\n");

  // Retrieve first platform_id
  if ((res = clGetPlatformIDs(1, &platform_id, NULL)) != CL_SUCCESS) {
    fprintf(stderr, "getPlatformIDs call failed with return code: %d\n", res);
    exit(EXIT_FAILURE);
  }

  // Display platform infos
  if ( (res = display_platform_infos(platform_id)) != CL_SUCCESS) {
    fprintf(stderr, "display_platform_infos call failed with return code: %d\n", res);
    exit(EXIT_FAILURE);
  }
  printf("\n");

  // Display devices IDs
  if ( (res = display_devices_ids(platform_id)) != CL_SUCCESS) {
    fprintf(stderr, "display_devices_ids call failed with return code: %d\n", res);
    exit(EXIT_FAILURE);
  }
  printf("\n");

  // Retrieve first device ID
  if ((res = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, NULL)) != CL_SUCCESS) {
    fprintf(stderr, "getDeviceIDs call failed with return code: %d\n", res);
    exit(EXIT_FAILURE);
  }

  // Display device infos
  if ( (res = display_device_infos(device_id)) != CL_SUCCESS) {
    fprintf(stderr, "display_device_infos call failed with return code: %d\n", res);
    exit(EXIT_FAILURE);
  }
  printf("\n");
}
