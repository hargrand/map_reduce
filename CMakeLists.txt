# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.24)

project (vs_map_reduce)

# Add the spt directory to the include path for all sub-projects.
include_directories(spt)

# Add the flag to allow unsupported MSVC compilers for CUDA.
# This is necessary for preview versions of Visual Studio.
if(MSVC)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -allow-unsupported-compiler")
endif()

# Include sub-projects.
add_subdirectory (mandelbrot_cpp)
add_subdirectory (mandelbrot_cuda)
add_subdirectory (perf_cpp)
add_subdirectory (test_cpp)
add_subdirectory (perf_cuda)
add_subdirectory (test_cuda)
