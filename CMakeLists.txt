
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXX_FLAGS}  -std=c++14 -O0  -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXX_FLAGS}  -std=c++14 -O3  -Wall")

add_compile_options(-std=c++14)

project(Opencl)


find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
message("${OpenCV_LIBRARY_DIR}")
endif()

include_directories(${OPENCV_INCLUDE_DIR})
link_directories(${OPENCV_LIBRARY_DIR})
link_libraries(${OpenCV_LIBRARIES})
# include_directories(${PROJECT_SOURCE_DIR}/include)



if(WIN32)
    message("WIN32")
elseif(APPLE)
    message("APPLE")
    find_library(OPENCL_LIBRARIES OpenCl)    
    find_path(OPENCL_INCLUDE_DIR OpenCL/cl.h)
elseif(UNIX)
    message("UNIX")
    find_package(OpenCL REQUIRED)
endif()


include_directories(${OPENCL_INCLUDE_DIR})
set(SRC_HEAD OpenClManager.h)
set(SRC OpenClManager.cpp main.cpp)

add_executable(${PROJECT_NAME} ${SRC_HEAD} ${SRC})

if(WIN32)
    message("WIN32")
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} ${OPENCL_LIBRARIES}) #${OpenCV_LIBS})
elseif(UNIX)
    message("UNIX")
    find_package(OpenCL REQUIRED)
    link_directories(${OpenCL_LIBRARY_DIR})
    target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES}) 

endif()

