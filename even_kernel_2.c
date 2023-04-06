#include "vercors.h"

// Address spaces for _kernel_c_s0_x_bx___block_id_x
#define __address_space__a __global
#define __address_space__b __global
#define __address_space__c __global
/*@
 context get_local_size(0) == 32;
 context get_local_size(1) == 1;
 context get_local_size(2) == 1;
 context get_num_groups(0) == 3;
 context get_num_groups(1) == 1;
 context get_num_groups(2) == 1;

 context _a != NULL;
 context _b != NULL;
 context _c != NULL;
 context \pointer_length(_a) == 130;
 context \pointer_length(_b) == 130;
 context \pointer_length(_c) == 130;

 context (\forall* int i; (((0 <= i) && (i < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + i)) <= 129)) && (get_group_id(0) < 2); Perm(_c[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)], 1\1));
 context (\forall* int i; (((0 <= i) && (i < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + i)) <= 129)) && (get_group_id(0) < 2); Perm(_a[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)], read));
 context (\forall* int i; (((0 <= i) && (i < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + i)) <= 129)) && (get_group_id(0) < 2); Perm(_b[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)], read));
 ensures (get_group_id(0) < 2) ==> (\forall* int i; (0 <= i) && (i < 2); (((get_group_id(0)*64) + ((get_local_id(0)*2) + i)) <= 129) ==> (_c[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)] == (_a[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)] + _b[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)])));
 context (\forall* int i; (((0 <= i) && (i < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + i)) <= 129)) && (2 <= get_group_id(0)); Perm(_c[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)], 1\1));
 context (\forall* int i; (((0 <= i) && (i < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + i)) <= 129)) && (2 <= get_group_id(0)); Perm(_a[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)], read));
 context (\forall* int i; (((0 <= i) && (i < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + i)) <= 129)) && (2 <= get_group_id(0)); Perm(_b[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)], read));
 ensures (2 <= get_group_id(0)) ==> (\forall* int i; (0 <= i) && (i < 2); (((get_group_id(0)*64) + ((get_local_id(0)*2) + i)) <= 129) ==> (_c[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)] == (_a[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)] + _b[(get_group_id(0)*64) + ((get_local_id(0)*2) + i)])));
@*/
__kernel void _kernel_c_s0_x_bx___block_id_x(
 __address_space__a const int *restrict _a,
 __address_space__b const int *restrict _b,
 __address_space__c int *restrict _c,
 __local int* __shared)
{
 int _c_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 bool _0 = _c_s0_x_bx___block_id_x < 2;
 if (_0)
 {
  int _1 = _c_s0_x_bx___block_id_x * 32;
  int _2 = _1 + ___thread_id_x;
  int _3 = _2 * 2;
  int _4 = _a[_3];
  int _5 = _b[_3];
  int _6 = _4 + _5;
  _c[_3] = _6;
  int _7 = _c_s0_x_bx___block_id_x * 32;
  int _8 = _7 + ___thread_id_x;
  int _9 = _8 * 2;
  int _10 = _9 + 1;
  int _11 = _a[_10];
  int _12 = _b[_10];
  int _13 = _11 + _12;
  _c[_10] = _13;
 } // if _0
 else
 {
  bool _14 = ___thread_id_x <= 0;
  if (_14)
  {
   int _15 = _a[128];
   int _16 = _b[128];
   int _17 = _15 + _16;
   _c[128] = _17;
   int _18 = _a[129];
   int _19 = _b[129];
   int _20 = _18 + _19;
   _c[129] = _20;
  } // if _14
 } // if _0 else
} // kernel _kernel_c_s0_x_bx___block_id_x
#undef __address_space__a
#undef __address_space__b
#undef __address_space__c