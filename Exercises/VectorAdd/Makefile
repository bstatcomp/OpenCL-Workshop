CC=gcc
CCFLAGS=-std=c99 -DCL_USE_DEPRECATED_OPENCL_1_2_APIS -O3
LIBS= -lOpenCL
HELPERS= ../Helpers

VectorAdd: VectorAdd.c
	$(CC) $^ $(CCFLAGS) $(LIBS) -I $(HELPERS) -o $@

clean:
	rm -f VectorAdd
