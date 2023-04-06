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


#endif

// Halide defines
#define restrict
#define const
#define int16 int
#define max(a, b) ((a) > (b) ? (a) : (b))
#define halide_unused(x)

// Address spaces for _kernel_even_s0_x_bx___block_id_x
#define __address_space__even __global
#define __address_space__in __global
/*@
 context_everywhere _even != NULL;
 context_everywhere _in != NULL;
 context_everywhere get_local_size(0) == 33;
 context_everywhere  get_num_groups(0) == 4;
 
@*/
//requires shared_mem_size_1 == 33;
// reads from global
//@ context (((get_group_id(0)*32) + get_local_id(0)) <= 127 && get_local_id(0) < 32) ==> Perm(_in[get_group_id(0) * 32 + get_local_id(0)], read);
//@ context (((get_group_id(0)*32) + get_local_id(0)) <= 127 && get_local_id(0) == 32 && get_group_id(0) == 0) ==> Perm(_in[get_group_id(0) * 33 + get_local_id(0)], read);
//@ context (((get_group_id(0)*32) + get_local_id(0)) <= 127 && get_local_id(0) == 32 && get_group_id(0) == 1) ==> Perm(_in[get_group_id(0) * 33 + get_local_id(0) + -1], read);
//@ context (((get_group_id(0)*32) + get_local_id(0)) <= 127 && get_local_id(0) == 32 && get_group_id(0) == 2) ==> Perm(_in[get_group_id(0) * 33 + get_local_id(0) + -2], read);
// context (((get_group_id(0)*32) + get_local_id(0)) <= 127 && get_local_id(0) == 32 && get_group_id(0) == 3) ==> Perm(_in[get_group_id(0) * 33 + get_local_id(0) + -3], read);
// writes to local
/*@ requires (((get_group_id(0)*32) + get_local_id(0)) <= 127 && get_local_id(0) < 33) ==> Perm(_in_in_even__0_0[get_local_id(0)], write);
    ensures (((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 33 
    && get_local_id(0) == 0)
      ==> Perm(_in_in_even__0_0[get_local_id(0)], 1);
   ensures (((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 33 
    && get_local_id(0) > 0 && get_local_id(0) < 32) 
      ==> Perm(_in_in_even__0_0[get_local_id(0)], 1\2);
   ensures (((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 33 
    && get_local_id(0) == 31)
      ==> Perm(_in_in_even__0_0[get_local_id(0)+1], 1);
   ensures (((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 33 
    && get_local_id(0) < 31) 
      ==> Perm(_in_in_even__0_0[get_local_id(0)+1], 1\2);
@*/

//@ context ((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 32 ==> Perm(_even[(get_group_id(0)*32) + get_local_id(0)], 1\1);
// context ((get_group_id(0)*32) + get_local_id(0)) <= 126 ==> Perm(_in_in_even__0_0[get_local_id(0)], read);
// context ((get_group_id(0)*32) + get_local_id(0)) <= 126 ==> Perm(_in_in_even__0_0[get_local_id(0) + 1], read);
// ensures (126 < ((get_group_id(0)*32) + get_local_id(0))) || ((_even[(get_group_id(0)*32) + get_local_id(0)] % 2) == 0);
__kernel void _kernel_even_s0_x_bx___block_id_x(
 __global int *restrict _even,
 __global const int *restrict _in
 //, __local int16* _in_in_even__0_0
 )
{

 // __local int16* _in_in_even__0_0_1;
 // __local int16[] _in_in_even__0_0_2;
  //__local int16 _in_in_even__0_0_2[16];
 //extern __local int16 _in_in_even__0_0_3[32];
 __local int _in_in_even__0_0[33];


 int _even_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 // produce in_in_even$0
 int _11 = _even_s0_x_bx___block_id_x * 32;
 int _12 = max(_11, 95);
 int _13 = _12 + ___thread_id_x;
 bool _14 = _13 < 128;
 if (_14)
 {
  int _15 = _even_s0_x_bx___block_id_x * 32;
  int _16 = _15 + ___thread_id_x;
  int _17 = _in[_16];
  (_in_in_even__0_0)[___thread_id_x] = _17;
 } // if _14
 
 // We add get_local_id(0) < 33 because of upper bounds detection for rewrite quantifier pass in vercors, but it is `theoretically` 
 // not needed (if VerCors pass was beter).
 /*@
  requires (((get_group_id(0)*32) + get_local_id(0)) <= 127 && get_local_id(0) < 33) 
    ==> Perm(_in_in_even__0_0[get_local_id(0)], write);
  ensures (((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 33 
   && get_local_id(0) == 0)
     ==> Perm(_in_in_even__0_0[get_local_id(0)], 1);
  ensures (((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 33 
   && get_local_id(0) > 0 && get_local_id(0) < 32) 
     ==> Perm(_in_in_even__0_0[get_local_id(0)], 1\2);
  ensures (((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 33 
   && get_local_id(0) == 31)
     ==> Perm(_in_in_even__0_0[get_local_id(0)+1], 1);
  ensures (((get_group_id(0)*32) + get_local_id(0)) <= 126 && get_local_id(0) < 33 
   && get_local_id(0) < 31) 
     ==> Perm(_in_in_even__0_0[get_local_id(0)+1], 1\2);
 @*/
 barrier(CLK_LOCAL_MEM_FENCE);
 int _18 = 0;
 halide_unused(_18);
 // consume in_in_even$0
 int _19 = _even_s0_x_bx___block_id_x * 32;
 int _20 = _19 + ___thread_id_x;
 bool _21 = _20 <= 126;
 bool _22 = ___thread_id_x < 32;
 bool _23 = _21 && _22;
 if (_23)
 {
  int _24 = (_in_in_even__0_0)[ ___thread_id_x];
  int _25 = ___thread_id_x + 1;
  int _26 = (_in_in_even__0_0)[_25];
  int _27 = _24 + _26;
  int _28 = _even_s0_x_bx___block_id_x * 32;
  int _29 = _28 + ___thread_id_x;
  _even[_29] = _27;
 } // if _23
} // kernel _kernel_even_s0_x_bx___block_id_x
#undef __address_space__even
#undef __address_space__in