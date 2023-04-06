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
//--silicon ../halide-dev/verification-tests/even_opencl_kernel_simple.c --encoded temp.sil

#include <opencl.h>
/*@
 //context_everywhere _f != NULL;
 context_everywhere _in != NULL;
 context_everywhere get_local_size(0) == 32;
 context_everywhere  get_num_groups(0) == 2;
 context Perm(_in[(get_group_id(0)*get_local_size(0)*2) + ((get_local_id(0)*2) + 0)], 1\1);
 context Perm(_in[(get_group_id(0)*get_local_size(0)*2) + ((get_local_id(0)*2) + 1)], 1\1);
 ensures _in[(get_group_id(0)*get_local_size(0)*2) + ((get_local_id(0)*2) + 0)] == get_group_id(0)*get_local_size(0) + get_local_id(0);
 ensures _in[(get_group_id(0)*get_local_size(0)*2) + ((get_local_id(0)*2) + 1)] == get_group_id(0)*get_local_size(0) + get_local_id(0);
@*/
__kernel void simple(global int *_in)
{
 int gid = get_global_id(0);

 /*@
 loop_invariant i>=0 && i<=2;
 loop_invariant (\forall* int j; 0<=j && j<2; Perm(_in[(get_group_id(0)*get_local_size(0)*2) + ((get_local_id(0)*2) + j)], 1\1));

 loop_invariant (\forall int j; 0<=j && j<i; _in[(get_group_id(0)*get_local_size(0)*2) + ((get_local_id(0)*2) + j)] == get_group_id(0)*get_local_size(0) + get_local_id(0) );
 
 //loop_invariant Perm(_in[(get_group_id(0)*get_local_size(0)*2) + ((get_local_id(0)*2) + 0)], 1\1);
 //loop_invariant Perm(_in[(get_group_id(0)*get_local_size(0)*2) + ((get_local_id(0)*2) + 1)], 1\1);
 @*/
 for (int i = 0; i < 0+2; i++)
 {
    _in[2*gid + i] = gid;
 }

}