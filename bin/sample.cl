__kernel void gpuMain(__global float *in0, __global float *in1, __global float *out) {
    
    // Get the index of the current element
    int i = get_global_id(0);

    // Do the operation
    out[i] = in0[i] + in1[i];
}