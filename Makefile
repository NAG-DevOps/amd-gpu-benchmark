CFLAGS=-I ~/src/OpenCL-Headers/ -DCL_TARGET_OPENCL_VERSION=200
LDFLAGS=-L /opt/amdgpu-pro/lib/x86_64-linux-gnu/ -l OpenCL

main: main.o
	gcc -o $@ $^ ${LDFLAGS}

%.o: %.c
	gcc ${CFLAGS} -c $^

clean:
	rm main *.o
