cmake_minimum_required(VERSION 3.10)

# Set the policy for CUDA architectures to suppress the CMP0104 warning.
# This ensures that CMAKE_CUDA_ARCHITECTURES is used.
cmake_policy(SET CMP0104 NEW)


project(mandel_cuda CXX CUDA)

# Find the libpng library. The 'REQUIRED' keyword will cause CMake to
# fail with an error if the library is not found.
find_package(PNG REQUIRED)

# Find the Zlib library, which is a dependency of libpng.
find_package(ZLIB REQUIRED)

# Define the executable target from the source file.
add_executable(
    mandel_cuda
    main.cu
    mandel.cu
    ../spt/image.cpp
    ../spt/mandel_common.cpp
    ../spt/timer.cpp)

# Set CUDA architectures. 'ARCH_BIN' compiles for the detected GPU architecture.
# This resolves the CMP0104 warning by providing a non-empty value.
set(CMAKE_CUDA_ARCHITECTURES native)

# Set the C++ standard to C++17 for the mandel_cuda target.
# This is the modern, target-centric way to set language standards.
set_property(TARGET mandel_cuda PROPERTY CXX_STANDARD 17)
set_property(TARGET mandel_cuda PROPERTY CXX_STANDARD_REQUIRED ON)

# Set the CUDA standard to C++17 for the mandel_cuda target.
set_property(TARGET mandel_cuda PROPERTY CUDA_STANDARD 17)
set_property(TARGET mandel_cuda PROPERTY CUDA_STANDARD_REQUIRED ON)

# Place the executable in the <build_root>/bin directory
set_target_properties(mandel_cuda PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Link the executable against the libraries found by find_package.
# Using the modern target-based approach (PNG::PNG) is recommended.
target_link_libraries(mandel_cuda PRIVATE PNG::PNG ZLIB::ZLIB)

set_property(TARGET mandel_cuda PROPERTY CXX_STANDARD 17)

# Enable extended lambda support for CUDA
target_compile_options(mandel_cuda PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--extended-lambda>)

# Place the executable in the <build_root>/bin directory
set_target_properties(mandel_cuda PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

