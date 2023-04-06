#define __kernel __vercors_kernel__

#define CLK_GLOBAL_MEM_FENCE __vercors_global_barrier__
#define CLK_LOCAL_MEM_FENCE __vercors_local_barrier__

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
#define __global
#define __local
#define restrict
#define const
#define int16 int
// Address spaces for _kernel_in_s0_x_bx___block_id_x
#define __address_space__in __global
/*@
 requires _in != NULL;
 context ((get_group_id(0)*get_local_size(0)) + get_local_id(0)) <= 63 ==>Perm(_in[(((get_group_id(0)*get_local_size(0)) + get_local_id(0))*2) + 1], 1\1);
 context ((get_group_id(0)*get_local_size(0)) + get_local_id(0)) <= 63 ==> Perm(_in[((get_group_id(0)*get_local_size(0)) + get_local_id(0))*2], 1\1);
 ensures ((get_group_id(0)*get_local_size(0)) + get_local_id(0)) <= 63 ==>((_in[(((get_group_id(0)*get_local_size(0)) + get_local_id(0))*2) + 1] % 2) == 0);
 ensures ((get_group_id(0)*get_local_size(0)) + get_local_id(0)) <= 63 ==>((_in[((get_group_id(0)*get_local_size(0)) + get_local_id(0))*2] % 2) == 0);
 requires get_local_size(0) == 32;
 requires  get_num_groups(0) == 2;
 context 0 <= get_group_id(0) && get_group_id(0) < get_num_groups(0);
@*/
__kernel void _kernel_in_s0_x_bx___block_id_x(
 __address_space__in int *restrict _in,
 __local int16* __shared)
{
 int _in_s0_x_bx___block_id_x = get_group_id(0);
 int ___thread_id_x = get_local_id(0);
 int _0 = _in_s0_x_bx___block_id_x * 32;
 int _1 = _0 + ___thread_id_x;
 int _2 = _1 * _1;
 int _3 = _2 * 8;
 int _4 = _1 * 2;
 _in[_4] = _3;
 int _5 = _in_s0_x_bx___block_id_x * 32;
 int _6 = _5 + ___thread_id_x;
 int _7 = _6 * 2;
 int _8 = _7 + 1;
 int _9 = _8 * _8;
 int _10 = _9 * 2;
 _in[_8] = _10;
} // kernel _kernel_in_s0_x_bx___block_id_x
#undef __address_space__in