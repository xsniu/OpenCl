const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void ImageConvert(__read_only image2d_t srcImg, __write_only image2d_t destImg, \ 
                            int width, int height)
{
    int2 outImageCoord = (int2)(get_global_id(0), get_global_id(1));
    if (outImageCoord.x < width && outImageCoord.y < height)
    {
        float4 pixelIn = read_imagef(srcImg, sampler, outImageCoord);
        float4 pixelOut = (float4)(pixelIn.w, pixelIn.z, pixelIn.y, pixelIn.x);
        write_imagef(destImg, outImageCoord, pixelOut);
    }
}   