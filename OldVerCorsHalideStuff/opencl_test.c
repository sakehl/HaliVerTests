#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define CL_TARGET_OPENCL_VERSION 220
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

int main() {
    cl_command_queue command_queue;
    cl_context context;
    cl_device_id device;
    cl_int input = 1;
    cl_int kernel_result = 0;
    cl_kernel kernel;
    cl_mem buffer;
    cl_platform_id platform;
    cl_program program;

    FILE *fp;
    long lSize;
    char *source;

    fp = fopen ( "opencl_test_kernel.cl" , "rb" );
    if( !fp ) perror("opencl_test_kernel.cl"),exit(1);
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    source = calloc( 1, lSize+1 );
    fread( source , lSize, 1 , fp);
    fclose(fp);

    //const char *source = "__kernel void increment(int in, __global int* out) { out[0] = in + 1; }";

    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    command_queue = clCreateCommandQueue(context, device, 0, NULL);
    buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(cl_int), NULL, NULL);
    program = clCreateProgramWithSource(context, 1, (const char **)&source, NULL, NULL);
    clBuildProgram(program, 1, &device, "", NULL, NULL);
    kernel = clCreateKernel(program, "increment", NULL);
    clSetKernelArg(kernel, 0, sizeof(cl_int), &input);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer);
    clSetKernelArg(kernel, 2, sizeof(cl_int), NULL);
    //clEnqueueTask(command_queue, kernel, 0, NULL, NULL);
    //
    size_t global_item_size = 2;
    size_t local_item_size = 2;
    cl_int ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
                                 &global_item_size, &local_item_size, 0, NULL, NULL);
    if (ret != CL_SUCCESS){
        printf("Error executing kernel: error code %d\n", ret),exit(1);
    }
    //
    clFlush(command_queue);
    clFinish(command_queue);
    clEnqueueReadBuffer(command_queue, buffer, CL_TRUE, 0, sizeof (cl_int), &kernel_result, 0, NULL, NULL);

    
    assert(kernel_result == 2);

    printf("Kernel succes!\n");
    return EXIT_SUCCESS;
}
