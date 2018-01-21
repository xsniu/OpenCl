#include "OpenClManager.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat srcImg = cv::imread("1.jpeg", -1);
    //cv::resize(img, srcImg, cv::Size(1280, 720));
    
    auto& ocl = OpenClMgr::GetInstance();
    if (ocl.Init())
    {
        std::cout << "OCL init failed!" << std::endl;
        return 0;
    }

    auto context = ocl.GetContext();
    if (!context)
    {
        return 0;
    }
    auto commandQueue = ocl.GetCommandQueue();
    if (!commandQueue)
    {
        return 0;
    }

    auto program = ocl.CreateProgram("ImageConvert.cl");
    if (!program)
    {
        return 0;
    }

    auto kernel = ocl.CreateKernel(program, "ImageConvert");
    if (!kernel)
    {
        return 0;
    }
    
    cv::Mat destImg(srcImg.size(), srcImg.type());

    cl_image_format clImageFormat;
    clImageFormat.image_channel_data_type = CL_UNORM_INT8;
    clImageFormat.image_channel_order = CL_RGB  ;

    cl_int err;
    auto srcImage = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, \
                         &clImageFormat, srcImg.cols, srcImg.rows, 0, srcImg.data, &err);
    CheckCLError(err);

    auto destImage = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &clImageFormat, \
                     destImg.cols, destImg.rows, 0, nullptr, &err);
    CheckCLError(err);

    auto width = srcImg.cols;
    auto height = srcImg.rows;
    auto channel = srcImg.channels();

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &srcImage);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &destImage);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_int), &width);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_int), &height);

    CheckCLError(err);


    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = {static_cast<size_t>(width), static_cast<size_t>(height), 1 };
    err = clEnqueueWriteImage(commandQueue, srcImage, CL_TRUE, origin, region, \
                                 0, 0, srcImg.data, 0, nullptr, nullptr);
        CheckCLError(err);
    int m = (static_cast<size_t>(width) / 16 + 1) * 16;
    int n = (static_cast<size_t>(height) / 16 + 1) * 16;


    size_t globalThread[] = {(static_cast<size_t>(width) / 16 + 1) * 16, \
                            (static_cast<size_t>(height) / 16 + 1) * 16};
    // size_t localThread[] = {16, 16};
    err = clEnqueueNDRangeKernel(commandQueue, kernel, 2, nullptr, globalThread, nullptr/* localThread */, 0, nullptr, nullptr);
    CheckCLError(err);
    

    err = clEnqueueReadImage(commandQueue, destImage, CL_TRUE, origin, region, 0, 0, \ 
                (void*)destImg.data, 0, nullptr, nullptr);
    CheckCLError(err);

    cv::imshow("src", srcImg);

    cv::imshow("test", destImg);
    cl::Platform::

    // cv::imshow("test", srcImg);
    cv::waitKey(0);
    return 0;
}