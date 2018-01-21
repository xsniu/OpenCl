//
// Created by niu19 on 2017/12/29.
//
#include "OpenClManager.h"



#include <string>
#include <iostream>
#include <vector>
#include <fstream>

class OpenClMgr::Imp
{
public:
    cl_context _Context;
    cl_command_queue  _CmdQueue;
    std::vector<cl_program> _Program;
    std::vector<cl_device_id> _Device;
    std::vector<cl_kernel> _Kernel;

public:
    Imp() {};
    ~Imp() {};

    void CL_CALLBACK EventCallBack(cl_event env, cl_int event_status)
    {
        cl_int err = 0;
        cl_ulong event_start_time = 0;
        cl_ulong event_end_time = 0;
        size_t result_len = 0;
        double runTime = 0;
        err = clGetEventProfilingInfo(env, CL_PROFILING_COMMAND_QUEUED,  \
                                      sizeof(cl_ulong), &event_start_time, &result_len);
        err = clGetEventProfilingInfo(env, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), \
                                     &event_end_time, &result_len);
        runTime = event_end_time - event_start_time;
        std::cout << "Kernel runtime %f secs" << runTime * 1.0e-9 << std::endl;
    }

    int CreateContext()
    {
        cl_int ciErrNum;
        cl_platform_id platform;
        ciErrNum = clGetPlatformIDs(1, &platform, NULL);
        if (ciErrNum != CL_SUCCESS){
            printf("Find Opencl platform fialed!\n");
            return -1;
        }

        cl_uint deviceNum;
        ciErrNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceNum);
        CheckCLError(ciErrNum);
        if (deviceNum <= 0)
        {
            return -1;
        }
        _Device.resize(deviceNum);

        ciErrNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, deviceNum, &_Device[0], NULL);
        if (ciErrNum != CL_SUCCESS){
            printf("function clGetDeviceIDs goes wrong\n");
            return -1;
        }

        cl_context_properties cps[3] = {
                CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };

        //create the context
        _Context = clCreateContext(cps, 1, &_Device[0], NULL, NULL, &ciErrNum);

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

        // Enable performance envaluation information
        // _CmdQueue = clCreateCommandQueue(_Context, _Device[0], CL_QUEUE_PROFILING_ENABLE, &errNum);
        _CmdQueue = clCreateCommandQueue(_Context, _Device[0], 0, &errNum);
        if (errNum != CL_SUCCESS)
        {
            return -1;
        }

        return 0;

    }

    cl_program CreateProgram(const char*source)
    {
        cl_int errNum;

        auto program = clCreateProgramWithSource(_Context, 1, &source, nullptr, &errNum);

        if (errNum != CL_SUCCESS){
            return nullptr;
        }

        errNum = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
        if (errNum != CL_SUCCESS)
        {
            std::string buildInfo;
            size_t infoLen = 0;
            errNum = clGetProgramBuildInfo(program, _Device[0], CL_PROGRAM_BUILD_LOG, 0, nullptr, &infoLen);
            if (errNum != CL_SUCCESS)
            {
                return nullptr;
            }

            buildInfo.resize(infoLen);
            errNum = clGetProgramBuildInfo(program, _Device[0], CL_PROGRAM_BUILD_LOG, infoLen, \
                                           &buildInfo[0], nullptr);
            if (errNum)
            {
                return nullptr;
            }

            std::cout << buildInfo << std::endl;
            clReleaseProgram(program);
            return nullptr;
        }
        _Program.push_back(program);

        return program;
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

        for (auto p : _Program)
        {
            clReleaseProgram(p);
        }

        if (_Context)
        {
            clReleaseContext(_Context);
        }
    }

    cl_kernel CreateKernel(const cl_program program,  const std::string& kernelName)
    {
        cl_int errNum;
        auto kernel = clCreateKernel(program, kernelName.c_str(), &errNum);
        _Kernel.push_back(kernel);

        if (errNum != CL_SUCCESS){
            return nullptr;
        }
        return kernel;
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

cl_program OpenClMgr::CreateProgram(const std::string &kernelFile)
{
    std::ifstream ifs(kernelFile, std::ios::in);
    if (!ifs.is_open())
    {
        std::cout << "Open kernel file failed!" << std::endl;
        return nullptr;
    }
    std::string source(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(0));

    auto &imp = *_ImpUptr;
    auto program = imp.CreateProgram(source.c_str()); 
   if (nullptr == program)
   {
       printf("Create program failed!\n");
       return nullptr;
   }
   return program;
}

cl_kernel OpenClMgr::CreateKernel(const cl_program program, const std::string& kernelName)
{
    auto& imp = *_ImpUptr;
    auto kernel = imp.CreateKernel(program, kernelName.c_str());
    if (nullptr == kernel)
    {
        std::cout << "Create kernel : %s failed!" << kernelName << std::endl;
        return nullptr;
    }

    return kernel;
}

cl_context OpenClMgr::GetContext() const
{
    auto &imp = *_ImpUptr;
    return imp._Context;
}

cl_command_queue OpenClMgr::GetCommandQueue() const 
{
    auto &imp = *_ImpUptr;
    return imp._CmdQueue;
}

// class InfoDevice::Imp
// {
// public:
//     void Display(cl_device_id device_id, cl_device_info infoName, std::string msg)
//     {
//         cl_int err;
//         std::size_t paraValueSize;

//         err = clGetDeviceInfo(device_id, infoName, 0, nullptr, &paraValueSize);
//         if (CL_SUCCESS != err)
//         {
//             std::cout << "Failed to find OpenCL device info length" << msg << std::endl;
//             return;
//         }

//         std::vector<T> info(paraValueSize);
//         err = clGetDeviceInfo(device_id, infoName, paraVlaueSize, &info[0], nullptr);
//         if (CL_SUCCESS != err)
//         {
//             std::cout << "Failed to find device info" << msg << std::endl;
//             return;
//         }

//         switch(infoName)
//         {
//             case CL_DEVICE_TYPE:
//             {
//                 std::string deviceType;
//                 AppendBitFiled(*(reinterpret_cast<cl_device_type*>(&info[0])), CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU", deviceType);
                
//                 AppendBitFiled(*(reinterpret_cast<cl_device_type*>(&info[0])), CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU", deviceType);

//                 AppendBitFiled(*(reinterpret_cast<cl_device_type*>(&info[0])), CL_DEVICE_TYPE_ACCELERATOR, "CL_DEVICE_TYPE_ACCELERATOR", deviceType);                

//                 AppendBitFiled(*(reinterpret_cast<cl_device_type*>(&info[0])), CL_DEVICE_TYPE_DEFAULT, "CL_DEVICE_TYPE_DEFAULT", deviceType);                                

//                 std::cout << "\t\t " << msg <<":\t" << deviceType << std::endl;
//                 break;
//             }
//             default:
//                 std::cout << "\t\t" << msg;
//                 for(auto p : info)
//                 {
//                     std::cout << p;
//                 }
//                 std::cout << std::endl;
//         }

//     }
// };

// InfoDevice::InfoDevice() :_ImpUptr(new Imp)
// {

// }

// InfoDevice::~InfoDevice()
// {

// }

// void InfoDevice::Display()
// {
//     auto& imp = *_ImpUptr;
    

// }



