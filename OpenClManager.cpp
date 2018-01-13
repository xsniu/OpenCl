//
// Created by niu19 on 2017/12/29.
//
#include "OpenClManager.h"
#if __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

class OpenClMgr::Imp
{
public:
    cl_context _Context;
    cl_command_queue  _CmdQueue;
    cl_program _Program;
    cl_device_id _Device;
    std::vector<cl_kernel> _Kernel;

public:
    Imp() {};
    ~Imp() {};

    int CreateContext()
    {
        cl_int ciErrNum;
        cl_platform_id platform;
        ciErrNum = clGetPlatformIDs(1, &platform, NULL);
        if (ciErrNum != CL_SUCCESS){
            printf("Find Opencl platform fialed!\n");
            return -1;
        }

        ciErrNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &_Device, NULL);
        if (ciErrNum != CL_SUCCESS){
            printf("function clGetDeviceIDs goes wrong\n");
            return -1;
        }

        cl_context_properties cps[3] = {
                CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };

        //create the context
        _Context = clCreateContext(cps, 1, &_Device, NULL, NULL, &ciErrNum);

        if (ciErrNum != CL_SUCCESS){
            printf("function clCreateContext goes wrong");
            return -1;
        }
        return 0;
    }

    int CreateCommandQueue()
    {
        cl_int errNum;
        size_t deviceSize = 0;
        errNum = clGetContextInfo(_Context, CL_CONTEXT_DEVICES, 0, NULL, &deviceSize);
        if (errNum != CL_SUCCESS)
        {
            printf("Failed call to GetContextInfo\n");
            return -1;
        }

        if (deviceSize <= 0)
        {
            return -1;
        }

        _CmdQueue = clCreateCommandQueue(_Context, _Device, 0, &errNum);
        if (errNum != CL_SUCCESS)
        {
            return -1;
        }

        return 0;

    }

    int CreateProgram(const char*source)
    {
        cl_int errNum;

        _Program = clCreateProgramWithSource(_Context, 1, &source, nullptr, &errNum);

        if (errNum != CL_SUCCESS){
            return -1;
        }

        errNum = clBuildProgram(_Program, 0, nullptr, nullptr, nullptr, nullptr);
        if (errNum != CL_SUCCESS)
        {
            std::string buildInfo;
            size_t infoLen = 0;
            errNum = clGetProgramBuildInfo(_Program, _Device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &infoLen);
            if (errNum != CL_SUCCESS)
            {
                return -1;
            }

            buildInfo.resize(infoLen);
            errNum = clGetProgramBuildInfo(_Program, _Device, CL_PROGRAM_BUILD_LOG, infoLen, \
                                           &buildInfo[0], nullptr);
            if (errNum)
            {
                return -1;
            }

            std::cout << buildInfo << std::endl;
            clReleaseProgram(_Program);
            return -1;
        }

        return 0;
    }


    inline  void CleanUp()
    {
        if (_CmdQueue)
        {
            clReleaseCommandQueue(_CmdQueue);
        }

        for (auto p : _Kernel)
        {
            clReleaseKernel(p);
        }

        if (_Program)
        {
            clReleaseProgram(_Program);
        }

        if (_Context)
        {
            clReleaseContext(_Context);
        }
    }

    int CreateKernel(const std::string& kernelName)
    {
        cl_int errNum;
        auto kernel = clCreateKernel(_Program, kernelName.c_str(), &errNum);
        _Kernel.push_back(kernel);
        if (errNum == CL_SUCCESS){
            return -1;
        }

        if (errNum != CL_SUCCESS){
            return -1;
        }
        return 0;
    }

};


OpenClMgr::OpenClMgr() : _ImpUptr(new Imp)
{

}

OpenClMgr::~OpenClMgr()
{

}

OpenClMgr& OpenClMgr::GetInstance()
{
    static OpenClMgr inst;
    return inst;
}

int OpenClMgr::Init()
{
    auto &imp = *_ImpUptr;

    if (-1 == imp.CreateContext())
    {
        printf("Create context failed!\n");
        return -1;
    }

    if (-1 == imp.CreateCommandQueue())
    {
        printf("Create  Command queue failed!");
        imp.CleanUp();
        return -1;
    }

    return 0;
}


void OpenClMgr::Uint()
{
    auto &imp = *_ImpUptr;
    imp.CleanUp();
}

int OpenClMgr::CreateProgram(const std::string &kernelFile)
{
    std::ifstream ifs(kernelFile, std::ios::in);
    if (ifs.is_open())
    {
        std::cout << "Open kernel file failed!" << std::endl;
        return -1;
    }
    std::string source(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(0));

    auto &imp = *_ImpUptr;
   if (-1 == imp.CreateProgram(source.c_str()))
   {
       printf("Create program failed!\n");
       return -1;
   }
   return 0;
}

int OpenClMgr::CreateKernel(const std::string& kernelName)
{
    auto& imp = *_ImpUptr;
    if (-1 == imp.CreateKernel(kernelName.c_str()))
    {
        std::cout << "Create kernel : %s failed!" << kernelName << std::endl;
        return -1;
    }

    return 0;
}

class InfoDevice::Imp
{
public:
    void Display(cl_device_id device_id, cl_device_info infoName, std::string msg)
    {
        cl_int err;
        std::size_t paraValueSize;

        err = clGetDeviceInfo(device_id, infoName, 0, nullptr, &paraValueSize);
        if (CL_SUCCESS != err)
        {
            std::cout << "Failed to find OpenCL device info length" << msg << std::endl;
            return;
        }

        std::vector<T> info(paraValueSize);
        err = clGetDeviceInfo(device_id, infoName, paraVlaueSize, &info[0], nullptr);
        if (CL_SUCCESS != err)
        {
            std::cout << "Failed to find device info" << msg << std::endl;
            return;
        }

        switch(infoName)
        {
            case CL_DEVICE_TYPE:
            {
                std::string deviceType;
                AppendBitFiled(*(reinterpret_cast<cl_device_type*>(&info[0])), CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU", deviceType);
                
                AppendBitFiled(*(reinterpret_cast<cl_device_type*>(&info[0])), CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU", deviceType);

                AppendBitFiled(*(reinterpret_cast<cl_device_type*>(&info[0])), CL_DEVICE_TYPE_ACCELERATOR, "CL_DEVICE_TYPE_ACCELERATOR", deviceType);                

                AppendBitFiled(*(reinterpret_cast<cl_device_type*>(&info[0])), CL_DEVICE_TYPE_DEFAULT, "CL_DEVICE_TYPE_DEFAULT", deviceType);                                

                std::cout << "\t\t " << msg <<":\t" << deviceType << std::endl;
                break;
            }
            default:
                std::cout << "\t\t" << msg;
                for(auto p : info)
                {
                    std::cout << p;
                }
                std::cout << endl;
        }

    }
}

InfoDevice::InfoDevice() :_ImpUptr(new Imp)
{

}

InfoDevice::~InfoDevice()
{

}

void InfoDevice::Display()
{
    auto& imp = *_ImpUptr;
    

}



