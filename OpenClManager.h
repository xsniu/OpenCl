//
// Created by niu19 on 2017/12/29.
//

#ifndef OPENCLANDROID_OPENCLMANGER_H
#define OPENCLANDROID_OPENCLMANGER_H

#if __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <memory>
#include <string>
#include <iostream>

#define CheckCLError(err) { if(err != CL_SUCCESS) std::cout << "Error"  << __FILE__ << " line " \
                     << __LINE__ << " error " << err << std::endl;}
class OpenClMgr
{
    class Imp;
    std::unique_ptr<Imp>    _ImpUptr;
    OpenClMgr();

public:
    ~OpenClMgr();
    static  OpenClMgr& GetInstance();

    int Init();
    void Uint();
    cl_context GetContext() const;
    cl_command_queue GetCommandQueue() const;
    cl_program CreateProgram(const std::string& kernelFile);
    cl_kernel CreateKernel(const cl_program program, const std::string& kernelName);

};



// template <typename T>
// class InfoDevice
// {

//     class Imp;
//     std::unique_ptr<Imp> _ImpUptr;
// public:
//     InfoDevice();
//     ~InfoDevice();
    
// public:
//     static void Display()
//     {
//         void Display(cl_device_id device_id, cl_device_info infoName, std::string msg)
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
//     }
//     void AppendBitFiled(T info, T value, std::string infoName, std::string &str)
//     {
//         if (info & value)
//         {
//             if (str.length() > 0)
//             {
//                 str.append(" |");
//             }

//             str.append(infoName);
//         }
//     }

// };
#endif //OPENCLANDROID_OPENCLMANGER_H
