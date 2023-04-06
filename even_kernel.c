#include "vercors.h"

// Address spaces for _kernel_c_s0_x_bx___block_id_x
#define __address_space__a __global
#define __address_space__b __global
#define __address_space__c __global
/*@
  context blockDim.x == 32;
  context blockDim.y == 1;
  context blockDim.z == 1;
  context gridDim.x == 4;
  context gridDim.y == 1;
  context gridDim.z == 1;

 context _a != NULL;
 context _b != NULL;
 context _c != NULL;
 context \pointer_length(_a) == 128;
 context \pointer_length(_b) == 128;
 context \pointer_length(_c) == 128;

 //requires shared_mem_size_1 == 0;

 context (((blockIdx.x*32) + threadIdx.x) <= 126) && (blockIdx.x < 3) ==> Perm(_c[(blockIdx.x*32) + threadIdx.x], 1\1);
 context (((blockIdx.x*32) + threadIdx.x) <= 126) && (blockIdx.x < 3) ==> Perm(_a[(blockIdx.x*32) + threadIdx.x], read);
 context (((blockIdx.x*32) + threadIdx.x) <= 126) && (blockIdx.x < 3) ==> Perm(_b[(blockIdx.x*32) + threadIdx.x], read);
 context (3 <= blockIdx.x) && (((blockIdx.x*32) + threadIdx.x) <= 126) ==> Perm(_c[(blockIdx.x*32) + threadIdx.x], 1\1);
 context (3 <= blockIdx.x) && (((blockIdx.x*32) + threadIdx.x) <= 126) ==> Perm(_a[(blockIdx.x*32) + threadIdx.x], read);
 context (3 <= blockIdx.x) && (((blockIdx.x*32) + threadIdx.x) <= 126) ==> Perm(_b[(blockIdx.x*32) + threadIdx.x], read);
@*/
__kernel void _kernel_c_s0_x_bx___block_id_x(
 __address_space__a const int *restrict _a,
 __address_space__b const int *restrict _b,
 __address_space__c int *restrict _c,
 __local int* __shared)
{
 int _c_s0_x_bx___block_id_x = blockIdx.x;
 int ___thread_id_x = threadIdx.x;
 bool _0 = _c_s0_x_bx___block_id_x < 3;
 if (_0)
 {
  int _1 = _c_s0_x_bx___block_id_x * 32;
  int _2 = _1 + ___thread_id_x;
  int _3 = _a[_2];
  int _4 = _b[_2];
  int _5 = _3 + _4;
  _c[_2] = _5;
 } // if _0
 else
 {
  bool _6 = ___thread_id_x <= 30;
  if (_6)
  {
   int _7 = ___thread_id_x + 96;
   int _8 = _a[_7];
   int _9 = _b[_7];
   int _10 = _8 + _9;
   _c[_7] = _10;
  } // if _6
 } // if _0 else
} // kernel _kernel_c_s0_x_bx___block_id_x
#undef __address_space__a
#undef __address_space__b
#undef __address_space__c