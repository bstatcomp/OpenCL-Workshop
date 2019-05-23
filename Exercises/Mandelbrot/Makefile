CC=gcc
CCFLAGS=-std=c99 -DCL_USE_DEPRECATED_OPENCL_1_2_APIS -O3
LIBS= -lOpenCL -L../Helpers/FreeImage/x64 -lfreeimage -lm
HELPERS= ../Helpers

MandelbrotCPU: MandelbrotCPU.c
	$(CC) $^ $(CCFLAGS) $(LIBS) -I $(HELPERS) -o $@

MandelbrotCL: MandelbrotCL.c
	$(CC) $^ $(CCFLAGS) $(LIBS) -I $(HELPERS) -o $@

clean:
	rm -f MandelbrotCL
	rm -f MandelbrotCPU
