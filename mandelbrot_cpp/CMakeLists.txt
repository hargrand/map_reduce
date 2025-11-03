cmake_minimum_required(VERSION 3.10)

project(mandel_cpp CXX)

# Find the libpng library. The 'REQUIRED' keyword will cause CMake to
# fail with an error if the library is not found.
find_package(PNG REQUIRED)

# Find the Zlib library, which is a dependency of libpng.
find_package(ZLIB REQUIRED)

# Define the executable target from the source file.
add_executable(mandel_cpp main.cpp mandel.cpp ../spt/image.cpp)

# Set the C++ standard to C++17 for the mandel_cpp target.
# This is the modern, target-centric way to set language standards.
set_property(TARGET mandel_cpp PROPERTY CXX_STANDARD 17)
set_property(TARGET mandel_cpp PROPERTY CXX_STANDARD_REQUIRED ON)

# Place the executable in the <build_root>/bin directory
set_target_properties(mandel_cpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Link the executable against the libraries found by find_package.
# Using the modern target-based approach (PNG::PNG) is recommended.
target_link_libraries(mandel_cpp PRIVATE PNG::PNG ZLIB::ZLIB)

# Add compiler options for strict warnings and treat warnings as errors.
# This uses generator expressions to apply the correct flags based on the compiler.
target_compile_options(mandel_cpp PRIVATE
    # Flags for GCC and Clang (for C++)
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    # Flags for MSVC (Visual Studio) (for C++)
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)
