// Custom VerCors header for OpenCL
#ifndef OPENCL_H
#define OPENCL_H

// Keywords for OpenCL
#define __kernel __vercors_kernel__
#define __global __vercors_global_memory__
#define global __vercors_global_memory__
#define __local __vercors_local_memory__
#define local __vercors_local_memory__
// Barriers
#define CLK_IMAGE_MEM_FENCE 4
#define CLK_GLOBAL_MEM_FENCE 2
#define CLK_LOCAL_MEM_FENCE 1
#define barrier(locality) __vercors_barrier__(locality)

extern /*@ pure @*/ int get_work_dim(); // Number of dimensions in use

extern /*@ pure @*/ int get_global_size(int dimindx); // Number of global work-items  

extern /*@ pure @*/ int get_global_id (int dimindx); // Global work-item ID value

extern /*@ pure @*/ int get_local_size (int dimindx); //

extern /*@ pure @*/ int get_enqueued_local_size (int dimindx); // Number of local work-items 

extern /*@ pure @*/ int get_local_id (int dimindx); // Local work-item ID 

extern /*@ pure @*/ int get_num_groups (int dimindx); // Number of work-groups

extern /*@ pure @*/ int get_group_id (int dimindx); // Work-group ID 

extern /*@ pure @*/ int get_global_offset (int dimindx); // Global offset

extern /*@ pure @*/ int get_global_linear_id (); // Work-items, 1-dimensional global ID

extern /*@ pure @*/ int get_local_linear_id (); // Work-items, 1-dimensional local ID

extern /*@ pure @*/ int get_sub_group_size (); // Number of work-items in the subgroup

extern /*@ pure @*/ int get_max_sub_group_size (); // Maximum size of a subgroup

extern /*@ pure @*/ int get_num_sub_groups (); // Number of subgroups

extern /*@ pure @*/ int get_enqueued_num_sub_groups (); //

extern /*@ pure @*/ int get_sub_group_id (); // Sub-group ID

extern /*@ pure @*/ int get_sub_group_local_id (); // Unique work-item ID

/*@
ensures a >= b ==> \result == a;
ensures !(a >= b) ==> \result == b;
@*/
/*@ pure @*/ int max(int a, int b);

/*@
ensures a > b ==> \result == b;
ensures !(a > b) ==> \result == a;
@*/
/*@ pure @*/ int min(int a, int b);


#endif

// Halide defines
#define restrict
#define const
//--silicon ../halide-dev/verification-tests/even_opencl_kernel_gen_0.c --encoded temp.sil

// Address spaces for _kernel_in_s0_x_bx___block_id_x
#define __address_space__f __global
#define __address_space__in __global
#define __address_space__k __global
/*@
 context_everywhere _f != NULL;
 context_everywhere _in != NULL;
 context_everywhere _k != NULL;
 requires shared_mem_size_1 == 64;
 context_everywhere get_local_size(0) == 64;
 context_everywhere  get_num_groups(0) == 2;
 requires Perm(_h_0[get_local_id(0)], 1\1);
 context Perm(_k[(((get_group_id(0)*64) + get_local_id(0))*129) + 1024], read);
 requires (_k[(((get_group_id(0)*64) + get_local_id(0))*129) + 1024] == 0);
 context (\forall* int _h_s1_r4__x_forall; (0 <= _h_s1_r4__x_forall) && (_h_s1_r4__x_forall < 8); Perm(_f[(((((get_group_id(0)*64) + get_local_id(0))*8) + _h_s1_r4__x_forall)*1029) + 5144], read));
 requires (\forall* int _h_s1_r4__x_forall; (0 <= _h_s1_r4__x_forall) && (_h_s1_r4__x_forall < 8); (_f[(((((get_group_id(0)*64) + get_local_id(0))*8) + _h_s1_r4__x_forall)*1029) + 5144] % 2) == 0);
 context (\forall* int _in_s0_x_tx_ix_forall; (((0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)) <= 127)) && (get_local_id(0) < 32); Perm(_in[(get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)], 1\1));
 context (\forall* int _in_s0_x_tx_ix_forall; (((0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)) <= 127)) && (get_local_id(0) < 32); Perm(_f[((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall))*1029], read));
 ensures (\forall* int _in_s0_x_tx_ix_forall; (((0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)) <= 127)) && (get_local_id(0) < 32); Perm(_h_0[(get_local_id(0)*2) + _in_s0_x_tx_ix_forall], read));
 requires (get_local_id(0) < 32) ==> (\forall* int _in_s0_x_tx_ix_forall; (0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2); (((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)) <= 127) ==> ((_f[((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall))*1029] % 2) == 1));
 ensures (get_local_id(0) < 32) ==> (\forall* int _in_s0_x_tx_ix_forall; (0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2); (((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)) <= 127) ==> ((_in[(get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)] % 2) == 1));
@*/
__kernel void _kernel_in_s0_x_bx___block_id_x(
 __address_space__f const int *restrict _f,
 __address_space__in int *restrict _in,
 __address_space__k const int *restrict _k,
 __local int* _h_0)
{
 int _in_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 // produce h
 int _37 = _in_s0_x_bx___block_id_x * 64;
 int _38 = _37 + ___thread_id_x;
 int _39 = _38 * 129;
 int _40 = _39 + 1024;
 int _41 = _k[_40];
 _h_0[___thread_id_x] = _41;
 /*@
  requires Perm(_h_0[get_local_id(0)], 1\1);
  requires _h_0[get_local_id(0)] == 0;
  ensures Perm(_h_0[get_local_id(0)], 1\1);
  ensures (_h_0[get_local_id(0)] == 0);
 @*/
 barrier(0 | CLK_LOCAL_MEM_FENCE);
 int _42 = 0;
 int _43 = _in_s0_x_bx___block_id_x * 64;
 int _44 = _43 + ___thread_id_x;
 int _45 = _44 * 8;
 /*@
  loop_invariant (0 <= _h_s1_r4__x) && (_h_s1_r4__x <= 8);
  loop_invariant Perm(_h_0[get_local_id(0)], 1\1);
  loop_invariant (\forall* int _h_s1_r4__x_forall; (0 <= _h_s1_r4__x_forall) && (_h_s1_r4__x_forall < 8); Perm(_f[(((((get_group_id(0)*64) + get_local_id(0))*8) + _h_s1_r4__x_forall)*1029) + 5144], read));
  loop_invariant (\forall* int _h_s1_r4__x_forall; (0 <= _h_s1_r4__x_forall) && (_h_s1_r4__x_forall < 8); (_h_s1_r4__x == 0) ==> (_h_0[get_local_id(0)] == 0));
  loop_invariant (\forall* int _h_s1_r4__x_forall; (0 <= _h_s1_r4__x_forall) && (_h_s1_r4__x_forall < 8); (_f[(((((get_group_id(0)*64) + get_local_id(0))*8) + _h_s1_r4__x_forall)*1029) + 5144] % 2) == 0);
  loop_invariant (\forall* int _h_s1_r4__x_forall; ((0 <= _h_s1_r4__x_forall) && (_h_s1_r4__x_forall < 8)) && (_h_s1_r4__x_forall < _h_s1_r4__x); (_h_s1_r4__x_forall == 8) ==> ((_h_0[get_local_id(0)] % 2) == 0));
  loop_invariant (_h_0[get_local_id(0)] % 2) == (_h_s1_r4__x % 2);
 @*/
 for (int _h_s1_r4__x = 0; _h_s1_r4__x < 0 + 8; _h_s1_r4__x++)
 {
  int _46 = _h_0[___thread_id_x];
  int _47 = _h_s1_r4__x + _45;
  int _48 = _47 * 1029;
  int _49 = _48 + 5144;
  int _50 = _f[_49];
  int _51 = _46 + _50;
  int _52 = _51 + 1;
  _h_0[___thread_id_x] = _52;
 } // for _h_s1_r4__x
 /*@
  requires Perm(_h_0[get_local_id(0)], 1\1);
  requires (\forall* int _h_s1_r4__x_forall; (0 <= _h_s1_r4__x_forall) && (_h_s1_r4__x_forall < 8); (_h_s1_r4__x_forall == 8) ==> ((_h_0[get_local_id(0)] % 2) == 0));
  ensures (\forall* int _in_s0_x_tx_ix_forall; (((0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2)) && (((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)) <= 127)) && (get_local_id(0) < 32); Perm(_h_0[(get_local_id(0)*2) + _in_s0_x_tx_ix_forall], read));
  ensures (get_local_id(0) < 32) ==> (\forall* int _in_s0_x_tx_ix_forall; (0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2); (((get_group_id(0)*64) + ((get_local_id(0)*2) + _in_s0_x_tx_ix_forall)) <= 127) ==> ((_h_0[(get_local_id(0)*2) + _in_s0_x_tx_ix_forall] % 2) == 0));
 @*/
 barrier(0 | CLK_LOCAL_MEM_FENCE);
 int _53 = 0;
 // consume h
 bool _54 = ___thread_id_x < 32;
 if (_54)
 {
  int _55 = _in_s0_x_bx___block_id_x * 32;
  int _56 = _55 + ___thread_id_x;
  int _57 = _56 * 2;
  int _58 = ___thread_id_x * 2;
  /*@
   loop_invariant (0 <= _in_s0_x_tx_ix) && (_in_s0_x_tx_ix <= 2);
   loop_invariant (\forall* int _in_s0_x_tx_ix_forall; ((0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2)) && (((get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall)) <= 127); Perm(_in[(get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall)], 1\1));
   loop_invariant (\forall* int _in_s0_x_tx_ix_forall; ((0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2)) && (((get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall)) <= 127); Perm(_f[((get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall))*1029], read));
   loop_invariant (\forall* int _in_s0_x_tx_ix_forall; ((0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2)) && (((get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall)) <= 127); Perm(_h_0[(get_local_id(0)*2) + _in_s0_x_tx_ix_forall], read));
   loop_invariant (\forall* int _in_s0_x_tx_ix_forall; (0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2); (((get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall)) <= 127) ==> ((_f[((get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall))*1029] % 2) == 1));
   loop_invariant (\forall* int _in_s0_x_tx_ix_forall; (0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2); (((get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall)) <= 127) ==> ((_h_0[(get_local_id(0)*2) + _in_s0_x_tx_ix_forall] % 2) == 0));
   loop_invariant (\forall* int _in_s0_x_tx_ix_forall; ((0 <= _in_s0_x_tx_ix_forall) && (_in_s0_x_tx_ix_forall < 2)) && (_in_s0_x_tx_ix_forall < _in_s0_x_tx_ix); (((get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall)) <= 127) ==> ((_in[(get_local_id(0)*2) + ((get_group_id(0)*64) + _in_s0_x_tx_ix_forall)] % 2) == 1));
  @*/
  for (int _in_s0_x_tx_ix = 0; _in_s0_x_tx_ix < 0 + 2; _in_s0_x_tx_ix++)
  {
   int _59 = _in_s0_x_tx_ix + _57;
   int _60 = _59 * 1029;
   int _61 = _f[_60];
   int _62 = _in_s0_x_tx_ix + _58;
   int _63 = _h_0[_62];
   int _64 = _61 + _63;
   _in[_59] = _64;
  } // for _in_s0_x_tx_ix
 } // if _54
} // kernel _kernel_in_s0_x_bx___block_id_x
#undef __address_space__f
#undef __address_space__in
#undef __address_space__k