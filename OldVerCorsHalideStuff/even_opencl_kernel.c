#include "vercors.h"
// Address spaces for _kernel_in_s0_x_bx___block_id_x
#define __address_space__in __global
/*@
 requires _in != NULL;
 requires 0 <= ((get_group_id(0)*get_local_size(0)) + get_local_id(0));
 context get_group_id(0) < 4 ==> Perm(_in[(get_group_id(0)*get_local_size(0)) + get_local_id(0)], 1\1);
 context (get_group_id(0) >= 4 && get_local_id(0) <= 0) ==> Perm(_in[(get_group_id(0)*get_local_size(0)) + get_local_id(0)], 1\1);
 
 ensures get_group_id(0) < 4 ==> (_in[(get_group_id(0)*get_local_size(0)) + get_local_id(0)] % 2) == 0;
 ensures (get_group_id(0) >= 4 && get_local_id(0) <= 0) ==> (_in[(get_group_id(0)*get_local_size(0)) + get_local_id(0)] % 2) == 0;
 requires get_local_size(0) == 32;
 requires  get_num_groups(0) == 5;
@*/
__kernel void _kernel_in_s0_x_bx___block_id_x(
 __address_space__in int *restrict _in,
 __local int16* __shared)
{
 int _in_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 bool _0 = _in_s0_x_bx___block_id_x < 4;
 if (_0)
 {
  int _1 = _in_s0_x_bx___block_id_x * 32;
  int _2 = _1 + ___thread_id_x;
  int _3 = _2 * _2;
  int _4 = _3 * 2;
  _in[_2] = _4;
 } // if _0
 else
 {
  bool _5 = ___thread_id_x <= 0;
  if (_5)
  {
   _in[128] = 32768;
  } // if _5
 } // if _0 else
} // kernel _kernel_in_s0_x_bx___block_id_x
#undef __address_space__in

// Address spaces for _kernel_even_s0_x_bx___block_id_x
#define __address_space__even __global
#define __address_space__in __global
/*@
 requires _even != NULL;
 requires _in != NULL;
 context ((get_group_id(0)*get_local_size(0)) + get_local_id(0)) <= 126 ==> Perm(_even[(get_group_id(0)*get_local_size(0)) + get_local_id(0)], 1\1);
 context ((get_group_id(0)*get_local_size(0)) + get_local_id(0)) <= 126 ==> Perm(_even[(get_group_id(0)*get_local_size(0)) + get_local_id(0)], read);
 context ((get_group_id(0)*get_local_size(0)) + get_local_id(0)) <= 126 ==> Perm(_even[((get_group_id(0)*get_local_size(0)) + get_local_id(0)) + 1], read);
 ensures (126 < ((get_group_id(0)*get_local_size(0)) + get_local_id(0))) || ((_even[(get_group_id(0)*get_local_size(0)) + get_local_id(0)] % 2) == 1);
 requires (126 < ((get_group_id(0)*get_local_size(0)) + get_local_id(0))) || ((_in[(get_group_id(0)*get_local_size(0)) + get_local_id(0)] % 2) == 0);
 requires (126 < ((get_group_id(0)*get_local_size(0)) + get_local_id(0))) || ((_in[((get_group_id(0)*get_local_size(0)) + get_local_id(0)) + 1] % 2) == 0);
 requires get_local_size(0) == 32;
 requires  get_num_groups(0) == 4;
@*/
__kernel void _kernel_even_s0_x_bx___block_id_x(
 __address_space__even int *restrict _even,
 __address_space__in const int *restrict _in,
 __local int16* __shared)
{
 int _even_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 bool _11 = _even_s0_x_bx___block_id_x < 3;
 if (_11)
 {
  int _12 = _even_s0_x_bx___block_id_x * 32;
  int _13 = _12 + ___thread_id_x;
  int _14 = _in[_13];
  int _15 = _13 + 1;
  int _16 = _in[_15];
  int _17 = _14 + _16;
  _even[_13] = _17;
 } // if _11
 else
 {
  bool _18 = ___thread_id_x <= 30;
  if (_18)
  {
   int _19 = ___thread_id_x + 96;
   int _20 = _in[_19];
   int _21 = ___thread_id_x + 97;
   int _22 = _in[_21];
   int _23 = _20 + _22;
   _even[_19] = _23;
  } // if _18
 } // if _11 else
} // kernel _kernel_even_s0_x_bx___block_id_x
#undef __address_space__even
#undef __address_space__in