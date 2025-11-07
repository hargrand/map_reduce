# CMakeList.txt : CMake project for cpp, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.24)

# Define a sub-project. This is good practice for modularity.
project(cpp_map_reduce LANGUAGES CXX)

# Add source to this project's executable.
add_executable (
  perf_cpp
  perf.cpp
  ../spt/assert.cpp
  ../spt/perf_common.cpp
  ../spt/timer.cpp)

set_property(TARGET perf_cpp PROPERTY CXX_STANDARD 17)

# Place the executable in the <build_root>/bin directory
set_target_properties(perf_cpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_compile_options(perf_cpp PRIVATE
    # Flags for GCC and Clang (for C++)
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror -O3>
    # Flags for MSVC (Visual Studio) (for C++)
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /O2>
)
