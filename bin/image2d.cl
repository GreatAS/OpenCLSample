const sampler_t sampler =
    CLK_NORMALIZED_COORDS_FALSE |
    CLK_ADDRESS_CLAMP_TO_EDGE   |
    CLK_FILTER_NEAREST;

__kernel void gpuMain(read_only image2d_t a, write_only image2d_t b)
{
    int2 pos;
    
    pos.x = get_global_id(0);
    pos.y = get_global_id(1);
        
    float4 pix = read_imagef(a, sampler, pos);
    
    write_imagef(b, pos, pix);
}