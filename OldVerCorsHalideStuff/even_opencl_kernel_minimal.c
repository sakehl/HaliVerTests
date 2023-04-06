#include <opencl.h>
/*@
 context opencl_gcount == 1;
 requires _in != NULL;
 context Perm(_in[\ltid], read);
@*/
__kernel void simple(int *_in)
{
 int t = _in[get_local_id(0)];
}