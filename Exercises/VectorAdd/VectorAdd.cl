__kernel void vector_add(__global const int *A, __global const int *B, __global int *C, int size)
{
  // naive implementation of a vector add kernel
  int i = get_global_id(0);
  if (i < size) {
    C[i] = A[i] + B[i];
  }
}

