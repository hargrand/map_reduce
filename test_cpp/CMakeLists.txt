# CMakeList.txt : CMake project for cpp, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.24)

# Define a sub-project and enable C++ and CUDA language support.
project(test_cpp_project LANGUAGES CXX)

# Enable testing for this directory and below.
# This should ideally be in the root CMakeLists.txt, but is safe here.
enable_testing()

# Add source to this project's executable.
add_executable (
  test_cpp
  test_cpp.cpp
  ../spt/assert.cpp
  ../test_common/common.cpp)

set_property(TARGET test_cpp PROPERTY CXX_STANDARD 17)

# Place the executable in the <build_root>/bin directory
set_target_properties(test_cpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
 
# Add the executable as a test to be run by CTest.
add_test(NAME cpp_collection_test COMMAND test_cpp)
