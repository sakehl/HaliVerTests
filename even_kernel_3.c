#include "vercors.h"

// Address spaces for _kernel_out_s0_x_bx___block_id_x
#define __address_space__c __global
#define __address_space__out __global
/*@
 context_everywhere _c != NULL;
 context_everywhere _out != NULL;
 context \pointer_length(_c) == 130;
 context \pointer_length(_out) == 128;

 context get_local_size(0) == 32;
 context get_local_size(1) == 1;
 context get_local_size(2) == 1;
 context get_num_groups(0) == 4;
 context get_num_groups(1) == 1;
 context get_num_groups(2) == 1;

 context Perm(_out[((get_group_id(0)*32) + get_local_id(0)) + 1], 1\1);
 context Perm(_c[(get_group_id(0)*32) + get_local_id(0)], read);
 context Perm(_c[((get_group_id(0)*32) + get_local_id(0)) + 1], read);
 context Perm(_c[((get_group_id(0)*32) + get_local_id(0)) + 2], read);
 
 ensures _out[((get_group_id(0)*32) + get_local_id(0)) + 1] == (_c[((get_group_id(0)*32) + get_local_id(0)) + 1] + (_c[((get_group_id(0)*32) + get_local_id(0)) + 2] + _c[(get_group_id(0)*32) + get_local_id(0)]));
@*/
__kernel void _kernel_out_s0_x_bx___block_id_x(
 __address_space__c const int *restrict _c,
 __address_space__out int *restrict _out,
 __local int* __shared)
{
 int _out_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 int _11 = _out_s0_x_bx___block_id_x * 32;
 int _12 = _11 + ___thread_id_x;
 int _13 = _12 + 1;
 int _14 = _c[_13];
 int _15 = _12 + 2;
 int _16 = _c[_15];
 int _17 = _c[_12];
 int _18 = _16 + _17;
 int _19 = _14 + _18;
 _out[_13] = _19;
} // kernel _kernel_out_s0_x_bx___block_id_x
#undef __address_space__c
#undef __address_space__out