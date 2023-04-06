#include "vercors.h"

// Address spaces for _kernel_c_s0_x_bx___block_id_x
#define __address_space__a __global
#define __address_space__b __global
#define __address_space__c __global
/*@
 context get_local_size(0) == 32;
 context get_local_size(1) == 1;
 context get_local_size(2) == 1;
 context get_num_groups(0) == 5;
 context get_num_groups(1) == 1;
 context get_num_groups(2) == 1;

 context _a != NULL;
 context _b != NULL;
 context _c != NULL;
 context \pointer_length(_a) == 130;
 context \pointer_length(_b) == 130;
 context \pointer_length(_c) == 130;


 context (((get_group_id(0)*32) + get_local_id(0)) <= 129) && (get_group_id(0) < 4) ==> Perm(_c[(get_group_id(0)*32) + get_local_id(0)], 1\1);
 context (((get_group_id(0)*32) + get_local_id(0)) <= 129) && (get_group_id(0) < 4) ==> Perm(_a[(get_group_id(0)*32) + get_local_id(0)], read);
 context (((get_group_id(0)*32) + get_local_id(0)) <= 129) && (get_group_id(0) < 4) ==> Perm(_b[(get_group_id(0)*32) + get_local_id(0)], read);
 ensures (get_group_id(0) < 4) ==> ((((get_group_id(0)*32) + get_local_id(0)) <= 129) ==> (_c[(get_group_id(0)*32) + get_local_id(0)] == (_a[(get_group_id(0)*32) + get_local_id(0)] + _b[(get_group_id(0)*32) + get_local_id(0)])));
 context (4 <= get_group_id(0)) && (((get_group_id(0)*32) + get_local_id(0)) <= 129) ==> Perm(_c[(get_group_id(0)*32) + get_local_id(0)], 1\1);
 context (4 <= get_group_id(0)) && (((get_group_id(0)*32) + get_local_id(0)) <= 129) ==> Perm(_a[(get_group_id(0)*32) + get_local_id(0)], read);
 context (4 <= get_group_id(0)) && (((get_group_id(0)*32) + get_local_id(0)) <= 129) ==> Perm(_b[(get_group_id(0)*32) + get_local_id(0)], read);
 ensures (4 <= get_group_id(0)) ==> ((((get_group_id(0)*32) + get_local_id(0)) <= 129) ==> (_c[(get_group_id(0)*32) + get_local_id(0)] == (_a[(get_group_id(0)*32) + get_local_id(0)] + _b[(get_group_id(0)*32) + get_local_id(0)])));
@*/
__kernel void _kernel_c_s0_x_bx___block_id_x(
 __address_space__a const int *restrict _a,
 __address_space__b const int *restrict _b,
 __address_space__c int *restrict _c,
 __local int* __shared)
{
 int _c_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 bool _0 = _c_s0_x_bx___block_id_x < 4;
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
  bool _6 = ___thread_id_x <= 1;
  if (_6)
  {
   int _7 = ___thread_id_x + 128;
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