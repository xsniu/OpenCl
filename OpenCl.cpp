#include <iostream>
#include <fstream>
#if __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/opencv.hpp"
#include <memory>

#include <vector>

cl_context CreateContext()
{
    cl_int errNum;
    cl_platform_id firstPlatform;
    cl_uint platformNum;
    errNum = clGetPlatformIDs(1, &firstPlatform, &platformNum);
    if (errNum != CL_SUCCESS)
    {
      std::cout << "Find Opencl platform fialed!." << std::endl;  
      return nullptr;
    }

    cl_context_properties contextProperties [] = {CL_CONTEXT_PLATFORM, (cl_context_properties)firstPlatform, 0};

    cl_context context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &errNum);

    if (errNum != CL_SUCCESS)
    {
        std::cout << "Create context failed!" << std::endl;
        return nullptr;
    }
    return context;

}

cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device)
{
    cl_int errNum;
    cl_device_id * devices;
    size_t deviceSize = 0;
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceSize);
    if (errNum != CL_SUCCESS)
    {
        std::cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
        return NULL;
    }

    if (deviceSize <= 0)
    {
        std::cerr << "No devices available.";
        return NULL;
    }

    // Allocate memory for the devices buffer
    devices = new cl_device_id[deviceSize / sizeof(cl_device_id)];
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceSize, devices, NULL);
    if (errNum != CL_SUCCESS)
    {
        delete [] devices;
        std::cerr << "Failed to get device IDs";
        return NULL;
    }

    // In this example, we just choose the first available device.  In a
    // real program, you would likely use all available devices or choose
    // the highest performance device based on OpenCL device queries
    cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
    if (commandQueue == NULL)
    {
        delete [] devices;
        std::cout << "Failed to create commandQueue for device 0";
        return NULL;
    }

    *device = devices[0];
    delete [] devices;
    return commandQueue;
}

cl_program CreateProgram(cl_context context, cl_device_id device, const char *fileName)
{
    cl_int errNum;
    std::ifstream in(fileName);
    if(!in)
    {
        std::cout << "Program source file is not exist!" << std::endl;
        return nullptr;
    }
    std::istreambuf_iterator<char> beg(in), end; 
    std::string soucreProgram(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(0));
    in.close();
    const char* source = soucreProgram.c_str();
    auto program = clCreateProgramWithSource(context, 1, &source, nullptr, &errNum);
    if (!program)
    {
        std::cout << "Create program failed!" << std::endl;
        return nullptr;
    }

    errNum = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
    if (errNum != CL_SUCCESS)
    {
        std::string buildInfo;
        size_t infoLen;
        errNum = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &infoLen);
        if (errNum != CL_SUCCESS)
        {
            std::cout << "Get build info length failed!" << std::endl;
            return nullptr;
        }

        buildInfo.resize(infoLen);
        errNum = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, infoLen, &buildInfo[0], nullptr);
        if (errNum != CL_SUCCESS)
        {
            std::cout << "Get build log failed!" << std::endl;
            return nullptr;
        }

        std::cout << buildInfo << std::endl;
        clReleaseProgram(program);
        return nullptr;
    }

    return program;
}


void Cleanup(cl_context context, cl_command_queue cmdQueue, cl_program program, cl_kernel kernel, \
            std::vector<cl_mem> memObject)
{
    if(!memObject.empty())
    {
        for(auto &mem : memObject)
        {
            clReleaseMemObject(mem);
        }
    }

    if (cmdQueue != 0)
    {
        clReleaseCommandQueue(cmdQueue);
    }

    if (kernel != 0)
    {
        clReleaseKernel(kernel);
    }

    if(program != 0)
    {
        clReleaseProgram(program);
    }

    if (context != 0)
    {
        clReleaseContext(context);
    }

}


cl_mem LoadImage(cl_context context, const std::string& fileName)
{
    cv::Mat srcImg = cv::imread(fileName);
    if (!srcImg.data)
    {
        std::cout << "Image file is not exist!" << std::endl;
        return nullptr; 
    }

    cv::Mat grayImg;
    cv::cvtColor(srcImg, grayImg, CV_BGR2GRAY);

    auto width = grayImg.cols;
    auto height = grayImg.rows; 
    std::shared_ptr<uchar> bufIn = std::make_shared<uchar>();

    
}

int main()
{
    return 0;

}

