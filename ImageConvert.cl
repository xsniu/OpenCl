const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__global void ImageConvert(__read_only image2d_t srcImg, __write_only image2d_t destImg, \ 
                            int width, int height)
{
    int2 outImageCoord = (int2)(get_global_id(0), get_global_id(1));
    if (outImageCoord.x < width && outImageCoord.y < height)
    {
        uint4 pixelIn = read_imageui(srcImg, sampler, outImageCoord);
        uint4 pixelOut = (uint4)(pixelIn.w, pixelIn.z, pixelIn.y, pixelIn.x);
        write_imageui(destImg, outImageCoord, pixelOut);
    }
}   