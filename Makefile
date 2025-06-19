#CFLAGS=-I ../OpenCL-Headers/ -DCL_TARGET_OPENCL_VERSION=200
#CFLAGS=-DCL_TARGET_OPENCL_VERSION=200
CFLAGS=-I /opt/amdgpu-pro/include -DCL_TARGET_OPENCL_VERSION=200
LDFLAGS=-L /opt/amdgpu-pro/lib64 -lOpenCL

all: main main-cpu cl-info

main: main.o benchlib.o
	gcc -o $@ $^ ${LDFLAGS}

main-cpu: main-cpu.o benchlib.o
	gcc -o $@ $^ ${LDFLAGS}

cl-info: cl-info.o
	gcc -o $@ $^ ${LDFLAGS}

%.o: %.c
	gcc ${CFLAGS} -c $^

clean:
	rm -f main main-cpu cl-info fit.log *.o
