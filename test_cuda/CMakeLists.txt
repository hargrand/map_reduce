# CMakeList.txt : CMake project for CUDA tests, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.24)

# Define a sub-project and enable C++ and CUDA language support.
project(test_cuda_project LANGUAGES CXX CUDA)

# Set CUDA architectures. 'ARCH_BIN' compiles for the detected GPU architecture.
# This resolves the CMP0104 warning by providing a non-empty value.
set(CMAKE_CUDA_ARCHITECTURES native)

# Enable testing for this directory and below.
# This should ideally be in the root CMakeLists.txt, but is safe here.
enable_testing()

# Add source to this project's executable.
add_executable (
  test_cuda
  test_cuda.cu
  ../spt/assert.cpp
  ../spt/test_common.cpp)

set_property(TARGET test_cuda PROPERTY CXX_STANDARD 17)

# Enable extended lambda support for CUDA
target_compile_options(test_cuda PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--extended-lambda>)

# Place the executable in the <build_root>/bin directory
set_target_properties(test_cuda PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
 
# Add the executable as a test to be run by CTest.
add_test(NAME test_cuda COMMAND test_cuda)
