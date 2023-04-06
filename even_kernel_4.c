#include "vercors.h"

// Address spaces for _kernel_out_s0_x_bx___block_id_x
#define __address_space__a __global
#define __address_space__b __global
#define __address_space__out __global
/*@
 context_everywhere _a != NULL;
 context_everywhere _b != NULL;
 context_everywhere _out != NULL;
 requires shared_mem_size_1 == 34;

 context get_local_size(0) == 32;
 context get_local_size(1) == 1;
 context get_local_size(2) == 1;
 context get_num_groups(0) == 4;
 context get_num_groups(1) == 1;
 context get_num_groups(2) == 1;

 context \pointer_length(_a) == 126;
 context \pointer_length(_b) == 126;
 context \pointer_length(_out) == 128;

 requires ((_c_s0_x_bx*32) + get_local_id(0)) <= 33 ==> Perm(_c_0[(_c_s0_x_bx*32) + get_local_id(0)], 1\1);
 context ((_c_s0_x_bx*32) + get_local_id(0)) <= 33 ==> Perm(_a[((_c_s0_x_bx + get_group_id(0))*32) + get_local_id(0)], read);
 context ((_c_s0_x_bx*32) + get_local_id(0)) <= 33 ==> Perm(_b[((_c_s0_x_bx + get_group_id(0))*32) + get_local_id(0)], read);
 context Perm(_out[((get_group_id(0)*32) + get_local_id(0)) + 1], 1\1);
 ensures Perm(_c_0[get_local_id(0)], read);
 ensures Perm(_c_0[get_local_id(0) + 1], read);
 ensures Perm(_c_0[get_local_id(0) + 2], read);
 ensures _out[((get_group_id(0)*32) + get_local_id(0)) + 1] == (_c_0[get_local_id(0) + 1] + (_c_0[get_local_id(0) + 2] + _c_0[get_local_id(0)]));
@*/
__kernel void _kernel_out_s0_x_bx___block_id_x(
 __address_space__a const int *restrict _a,
 __address_space__b const int *restrict _b,
 __address_space__out int *restrict _out,
 __local int* _c_0)
{
 int _out_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 // produce c
 int _0 = 65 - ___thread_id_x;
 int _1 = _0 >> 5;
 /*@
  loop_invariant (0 <= _c_s0_x_bx) && (_c_s0_x_bx <= 2);
  loop_invariant (\forall* int _c_s0_x_tx_forall int _c_s0_x_bx_forall; ((0 <= _c_s0_x_bx_forall) && (_c_s0_x_bx_forall < 2)) && (((0 <= _c_s0_x_tx_forall) && (_c_s0_x_tx_forall < 32)) && (((_c_s0_x_bx_forall*32) + _c_s0_x_tx_forall) <= 33)); Perm(_c_0[(_c_s0_x_bx_forall*32) + _c_s0_x_tx_forall], 1\1));
  loop_invariant (\forall* int _c_s0_x_tx_forall int _c_s0_x_bx_forall; ((0 <= _c_s0_x_bx_forall) && (_c_s0_x_bx_forall < 2)) && (((0 <= _c_s0_x_tx_forall) && (_c_s0_x_tx_forall < 32)) && (((_c_s0_x_bx_forall*32) + _c_s0_x_tx_forall) <= 33)); Perm(_a[((_c_s0_x_bx_forall + get_group_id(0))*32) + _c_s0_x_tx_forall], read));
  loop_invariant (\forall* int _c_s0_x_tx_forall int _c_s0_x_bx_forall; ((0 <= _c_s0_x_bx_forall) && (_c_s0_x_bx_forall < 2)) && (((0 <= _c_s0_x_tx_forall) && (_c_s0_x_tx_forall < 32)) && (((_c_s0_x_bx_forall*32) + _c_s0_x_tx_forall) <= 33)); Perm(_b[((_c_s0_x_bx_forall + get_group_id(0))*32) + _c_s0_x_tx_forall], read));
  loop_invariant (\forall* int _c_s0_x_bx_forall int _c_s0_x_tx_forall; (((0 <= _c_s0_x_bx_forall) && (_c_s0_x_bx_forall < 2)) && (_c_s0_x_bx_forall < _c_s0_x_bx)) && ((0 <= _c_s0_x_tx_forall) && (_c_s0_x_tx_forall < 32)); (((_c_s0_x_bx_forall*32) + _c_s0_x_tx_forall) <= 33) ==> (_c_0[(_c_s0_x_bx_forall*32) + _c_s0_x_tx_forall] == (_a[((_c_s0_x_bx_forall + get_group_id(0))*32) + _c_s0_x_tx_forall] + _b[((_c_s0_x_bx_forall + get_group_id(0))*32) + _c_s0_x_tx_forall])));
 @*/
 for (int _c_s0_x_bx = 0; _c_s0_x_bx < 0 + _1; _c_s0_x_bx++)
 {
  int _2 = _c_s0_x_bx + _out_s0_x_bx___block_id_x;
  int _3 = _2 * 32;
  int _4 = _3 + ___thread_id_x;
  int _5 = _a[_4];
  int _6 = _b[_4];
  int _7 = _5 + _6;
  int _8 = _c_s0_x_bx * 32;
  int _9 = _8 + ___thread_id_x;
  _c_0[_9] = _7;
 } // for _c_s0_x_bx
 /*@
  requires ((_c_s0_x_bx*32) + get_local_id(0)) <= 33 ==> Perm(_c_0[(_c_s0_x_bx*32) + get_local_id(0)], 1\1);
  requires (((_c_s0_x_bx*32) + get_local_id(0)) <= 33) ==> (_c_0[(_c_s0_x_bx*32) + get_local_id(0)] == (_a[((_c_s0_x_bx + get_group_id(0))*32) + get_local_id(0)] + _b[((_c_s0_x_bx + get_group_id(0))*32) + get_local_id(0)]));
  ensures Perm(_c_0[get_local_id(0)], read);
  ensures Perm(_c_0[get_local_id(0) + 1], read);
  ensures Perm(_c_0[get_local_id(0) + 2], read);
  ensures _c_0[get_local_id(0)] == (_a[(get_group_id(0)*32) + get_local_id(0)] + _b[(get_group_id(0)*32) + get_local_id(0)]);
  ensures _c_0[get_local_id(0) + 1] == (_a[((get_group_id(0)*32) + get_local_id(0)) + 1] + _b[((get_group_id(0)*32) + get_local_id(0)) + 1]);
  ensures _c_0[get_local_id(0) + 2] == (_a[((get_group_id(0)*32) + get_local_id(0)) + 2] + _b[((get_group_id(0)*32) + get_local_id(0)) + 2]);
 @*/
 barrier(0 | CLK_LOCAL_MEM_FENCE);
 int _10 = 0;
 // consume c
 int _11 = ___thread_id_x + 1;
 int _12 = _c_0[_11];
 int _13 = ___thread_id_x + 2;
 int _14 = _c_0[_13];
 int _15 = _c_0[___thread_id_x];
 int _16 = _14 + _15;
 int _17 = _12 + _16;
 int _18 = _out_s0_x_bx___block_id_x * 32;
 int _19 = _18 + ___thread_id_x;
 int _20 = _19 + 1;
 _out[_20] = _17;
} // kernel _kernel_out_s0_x_bx___block_id_x
#undef __address_space__a
#undef __address_space__b
#undef __address_space__out