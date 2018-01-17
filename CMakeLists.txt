
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}  -std=c++11 -O0  -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}  -std=c++11 -O3  -Wall")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -gdb -std=c++11 -o0")
project(Opencl)


find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
message("${OpenCV_LIBRARY_DIR}")
endif()

include_directories(${OPENCV_INCLUDE_DIR})
link_directories(${OPENCV_LIBRARY_DIR})
link_libraries(${OpenCV_LIBRARIES})
# include_directories(${PROJECT_SOURCE_DIR}/include)

# find_package(OpenCL REQUIRED)
find_library(OPENCL_LIBRARIES OpenCl)
find_path(OPENCL_INCLUDE_DIR OpenCL/cl.h)

include_directories(${OPENCL_INCLUDE_DIR})
set(SRC_HEAD OpenClManager.h)
set(SRC OpenClManager.cpp main.cpp)

add_executable(${PROJECT_NAME} ${SRC_HEAD} ${SRC})

target_link_libraries(${PROJECT_NAME} ${OPENCL_LIBRARIES}) #${OpenCV_LIBS})
