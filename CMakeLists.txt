# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.24)

# Set the MSVC runtime library for all targets in the project.
# This ensures consistency between C++ and CUDA targets, resolving LNK4098.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

project (vs_map_reduce LANGUAGES CXX)

# Find the CUDA toolkit, but don't make it required.
find_package(CUDAToolkit)

if(CUDAToolkit_FOUND)
    message(STATUS "CUDA Toolkit found, enabling CUDA targets.")
    # Allow using a newer MSVC compiler than what the CUDA toolkit officially supports.
    # This must be set BEFORE enable_language(CUDA).
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -allow-unsupported-compiler")
    # Enable the CUDA language since we will be building CUDA targets.
    enable_language(CUDA)
    set(CMAKE_CUDA_ARCHITECTURES 75) # Or "native"
else()
    message(WARNING "CUDA Toolkit not found. CUDA-specific targets will be skipped.")
endif()

# Add the spt directory to the include path for all sub-projects.
include_directories(spt)

# Include sub-projects.
add_subdirectory (mandelbrot_cpp)
add_subdirectory (perf_cpp)
add_subdirectory (test_cpp)

# Only add CUDA subdirectories if the toolkit was found
if(CUDAToolkit_FOUND)
    add_subdirectory (mandelbrot_cuda)
    add_subdirectory (perf_cuda)
    add_subdirectory (test_cuda)
endif()
