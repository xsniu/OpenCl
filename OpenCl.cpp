#include <iostream>
#include <fstream>
#if __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

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


