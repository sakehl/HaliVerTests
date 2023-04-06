#include <cuda.h>

////////////////////////////////////////////////////////////////////////////////
//Kernel
////////////////////////////////////////////////////////////////////////////////
/*@ 
context_everywhere array != NULL;
context_everywhere output != NULL;
context_everywhere size == 1024;
context_everywhere opencl_gcount == 1;
context_everywhere opencl_gsize == size;
//context \pointer_index(array, \ltid, write); This passes
//context \pointer_index(array, \ltid, 1); This gives an error:  unspecified error z3.interaction.failed java.lang.ArrayIndexOutOfBoundsException: Index 1 out of bounds for length 1
//context 2*\ltid < size ==> \pointer_index(array, 2*\ltid, write); This gives  CallPreCondition:AssertionFalse
//context 2*\ltid+1 < size ==> \pointer_index(array, 2*\ltid+1, write); This gives  CallPreCondition:AssertionFalse
requires \ltid > 0 ==> \pointer_index(array, \ltid - 1, 1\2); 
//requires \pointer_index(output, \ltid, 1\2);
//context \ltid < (size+2-1)/2 ==> \pointer_index(array, 2*\ltid, write); //This gives  CallPreCondition:AssertionFalse
//context \ltid < (size-1+2-1)/2 ==> \pointer_index(array, 2*\ltid+1, write); //This gives  CallPreCondition:AssertionFalse
@*/
__global__ void CUDA_Kernel(int* array, int* output, int size)
{
  
  int tid = threadIdx.x;
  //@ assert tid == \ltid;
  
}

////////////////////////////////////////////////////////////////////////////////
// CUDA Functions
////////////////////////////////////////////////////////////////////////////////
//@ ensures \pointer(\result, N, write);
int *vercorsMallocInt(int N);
void vercorsFreeInt(int *ar);
//@ ensures \pointer(\result, N, write);
int *vercorsCudaMallocInt(int N);
void vercorsCudaFreeInt(int *addr);
//@ context \pointer(src, N, read) ** \pointer(tgt, N, write);
//@ ensures (\forall int i; i >= 0 && i < N; src[i] == tgt[i]);
void vercorsCudaMemcpyInt(int *tgt, int *src, int N, int direction); 

////////////////////////////////////////////////////////////////////////////////
// Main Program
////////////////////////////////////////////////////////////////////////////////

int CUDA_Host_Blelloch( int argc, char** argv){
  
  int size = 1024; // size of the input 
  
  int* host_output = vercorsMallocInt(size); 
  
  int* device_array;
  device_array = vercorsCudaMallocInt(size);
  vercorsCudaMemcpyInt(device_array, host_output, size, cudaMemcpyHostToDevice) ;
  
  int* device_output;
  device_output = vercorsCudaMallocInt(size);
  
  //setup execution parameters
  int num_of_blocks = 1;
  int num_of_threads_per_block = size;
  
  CUDA_Kernel<<< /*grid*/num_of_blocks, /*threads*/num_of_threads_per_block/*, 0*/ >>>(device_array, device_output, size);
  
  //vercorsCudaMemcpyInt(host_output, device_output, size, cudaMemcpyDeviceToHost) ;
  
  // cleanup memory
  vercorsFreeInt(host_output);
  vercorsCudaFreeInt(device_output);
  
}





