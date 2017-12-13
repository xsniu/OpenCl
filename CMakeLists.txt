
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -std=c++11 -o0 -Wno-c++11-extensions")
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
set(SRC ${PROJECT_SOURCE_DIR}/OpenCl.cpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} ${OPENCL_LIBRARIES}) #${OpenCV_LIBS})
