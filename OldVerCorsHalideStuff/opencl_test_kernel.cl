__kernel void increment(int in, __global int* out, __local int* x) 
{
  __local int y;
  if(get_local_id(0) == 0){
    *x = 1;
    y = in;
  }

  barrier(0 | CLK_LOCAL_MEM_FENCE);
  if(get_local_id(0) == 1){
    out[0] = *x + y;
  }
}