
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -std=c++11 -o0")
project(Opencl)

# include_directories(${PROJECT_SOURCE_DIR}/include)

# find_package(OpenCL REQUIRED)
find_library(OPENCL_LIBRARIES OpenCl)
find_path(OPENCL_INCLUDE_DIR OpenCL/cl.h)

include_directories(${OPENCL_INCLUDE_DIR})
set(SRC ${PROJECT_SOURCE_DIR}/hello.cpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} ${OPENCL_LIBRARIES})
