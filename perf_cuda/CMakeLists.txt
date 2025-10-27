# CMakeList.txt : CMake project for CUDA, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.24)

# Enable CUDA language support for this sub-project.
project(cuda_map_reduce LANGUAGES CXX CUDA)

# Set CUDA architectures. 'ARCH_BIN' compiles for the detected GPU architecture.
# This resolves the CMP0104 warning by providing a non-empty value.
set(CMAKE_CUDA_ARCHITECTURES native)

# Add source to this project's executable.
add_executable (
  perf_cuda
  perf.cu
  ../spt/assert.cpp
  ../spt/support.cpp)

set_property(TARGET perf_cuda PROPERTY CXX_STANDARD 17)

# Enable extended lambda support for CUDA
target_compile_options(perf_cuda PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--extended-lambda>)

# Place the executable in the <build_root>/bin directory
set_target_properties(perf_cuda PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# TODO: Add tests and install targets if needed.
