#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#include "readKernel.h"

#define SIZE      (1024)
#define WORKGROUP_SIZE  (64)

int main(void) 
{
  int i;
  cl_int ret;

  int vector_size = SIZE;

  char *source_str = NULL;
  
  read_kernel(&source_str, "VectorAdd.cl");
  
  // allocate vector memory
  int *A = (int*)malloc(vector_size*sizeof(int));
  int *B = (int*)malloc(vector_size*sizeof(int));
  int *C = (int*)malloc(vector_size*sizeof(int));
  int *C_cpu = (int*)malloc(vector_size*sizeof(int));

  // fill the vectors with some data
  for(i = 0; i < vector_size; i++) 
  {
    A[i] = i;
    B[i] = vector_size - i;
  }

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
  
  size_t local_item_size = WORKGROUP_SIZE;
  size_t global_item_size = vector_size + (vector_size % local_item_size);   

  // create the OpenCL device buffers
  cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                  vector_size*sizeof(int), NULL, &ret);
  
  cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                  vector_size*sizeof(int), NULL, &ret);
  
  cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
                  vector_size*sizeof(int), NULL, &ret);
  
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
  cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);
 
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
  ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
  ret |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
  ret |= clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&vector_size);
  
  // send data to the OpenCL device
  ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, sizeof(int) * vector_size, A, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, sizeof(int) * vector_size, B, 0, NULL, NULL);
  
  // run the kernel
  ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL); 
  
  // read back the result
  ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, vector_size*sizeof(int), C, 0, NULL, NULL);        

  // do the CPU vector add
  for(int i = 0;i < vector_size; i++)
    C_cpu[i] = A[i] + B[i];
  
  // check results
  int result_ok = 1;
  for(i = 0; i < vector_size; i++){
      if(C_cpu[i] != C[i]) {
        result_ok = 0;
        break;
      }    
  }
  if(result_ok)
    printf("RESULT OK\n");
  else
    printf("ERROR\n");

  // free all the allocated memory
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(a_mem_obj);
  ret = clReleaseMemObject(b_mem_obj);
  ret = clReleaseMemObject(c_mem_obj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);

  free(A);
  free(B);
  free(C);
  free(C_cpu);
  free(source_str);

  return 0;
}
