#include <stdio.h>
#include <stdlib.h>
#include "FreeImage.h"
#include <math.h>
#include <CL/cl.h>

#include "readKernel.h"

#define WORKGROUP_SIZE 64

void mandelbrotCPU(unsigned char *image, int height, int width) {
  int i;
  cl_int ret;

  char *source_str = NULL;
  
  read_kernel(&source_str, "Mandelbrot.cl");
  
  // find the platform and device
  cl_platform_id  platform_id[10];
  cl_uint     ret_num_platforms;
  cl_device_id  device_id[10];
  cl_uint     ret_num_devices;
  char      *buf;
  size_t     buf_len;
  
  ret = clGetPlatformIDs(10, platform_id, &ret_num_platforms);
  
  ret = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU, 10, device_id, &ret_num_devices);        
  
  // create the context and command queue on the platform 0 - device 0
  cl_context context = clCreateContext(NULL, 1, &device_id[0], NULL, NULL, &ret);
  
  cl_command_queue command_queue = clCreateCommandQueue(context, device_id[0], 0, &ret);
  
  // set the workgroup size and the global size
  size_t local_item_size[2] = {WORKGROUP_SIZE, WORKGROUP_SIZE};
  size_t global_item_size[2] = {height + (height % WORKGROUP_SIZE), width + (width % WORKGROUP_SIZE)};   

  // create the OpenCL device buffers
  cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                  height * width * 4, NULL, &ret);

  // build the OpenCL source
  cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str,  
                           NULL, &ret);

  ret = clBuildProgram(program, 1, &device_id[0], NULL, NULL, NULL);

  if (ret != 0) {
    size_t build_log_len;
    char *build_log;
    ret = clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, 
                  0, NULL, &build_log_len);
    
    build_log =(char *)malloc(sizeof(char)*(build_log_len+1));
    ret = clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, 
                    build_log_len, build_log, NULL);
    printf("%s\n", build_log);
    free(build_log);
  }

  // retrieve the kernel and set the parameters
  cl_kernel kernel = clCreateKernel(program, "mandelbrot", &ret);
 
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
  ret |= clSetKernelArg(kernel, 1, sizeof(cl_int), (void *)&height);
  ret |= clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&width);
  
  // run the kernel
  ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_item_size, local_item_size, 0, NULL, NULL); 
  
  // read back the result
  ret = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0, height * width * 4, image, 0, NULL, NULL);        

  // free all the allocated memory
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(a_mem_obj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);

  free(source_str);
}


int main(void)
{
	int height = 1024;
	int width = 1024;
	int pitch = ((32 * width + 31) / 32) * 4;

	//allocate the memory for a RGBA image
	unsigned char *image = (unsigned char *)malloc(height * width * sizeof(unsigned char) * 4);

	mandelbrotCPU(image, height, width);

	// save the image
	FIBITMAP *dst = FreeImage_ConvertFromRawBits(image, width, height, pitch,
		32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	FreeImage_Save(FIF_PNG, dst, "mandelbrot.png", 0);
	return 0;
}
