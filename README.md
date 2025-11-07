# Map-Reduce comparisons C++ vs CUDA

## Windows / MSVC

### Prerequisites
Install Visual Studio 2022 Community Edition (This appears to be the latest version the CUDA toolkit currently supports)
Install NVIDIA CUDA Toolkit version 13.0 (This needs to be done after installing MSVC so it can register itself with the compiler tools)

### Download, build and install zlib
```batch
git clone git@github.com:madler/zlib.git
cd zlib
md build
cmake -S . -B build  -DCMAKE_INSTALL_PREFIX="c:\local\zlib"
cmake --build build --config Release
cmake --install build --config Release
```

### Download, build and install libpng
```batch
cd ..
git clone git@github.com:pnggroup/libpng.git
cd libpng
md build
cmake -S . -B build  -DCMAKE_INSTALL_PREFIX="c:\local\libpng" -DZLIB_LIBRARY="c:\local\zlib\lib\z.lib" -DZLIB_INCLUDE_DIR="c:\local\zlib\include"
cmake --build build --config Release
cmake --install build --config Release
```

### Build the Map/Reduce Samples
```batch
cd ..
cd map_reduce
md build
cmake -S . -B build  -DZLIB_LIBRARY="c:\local\zlib\lib\zs.lib" -DZLIB_INCLUDE_DIR="c:\local\zlib\include" -DPNG_LIBRARY="c:\local\libpng\lib\libpng18_static.lib" -DPNG_PNG_INCLUDE_DIR="c:\local\libpng\include"
cmake --build build --config Release```
```

### Run
#### Compare Mandelbrot image generation times:
```batch
.\build\bin\Release\mandel_cpp.exe mandel_cpp_4k.png 3840 2160 8192 -3.555556 -2.0 4.0
.\build\bin\Release\mandel_cuda.exe mandel_cuda_4k.png 3840 2160 8192 -3.555556 -2.0 4.0```
```

#### Compare dot product performance times:
```batch
.\build\bin\Release\perf_cpp.exe cpp.csv 10 100 1000 10000 100000 1000000 10000000 100000000
.\build\bin\Release\perf_cuda.exe cuda.csv 10 10 100 1000 10000 100000 1000000 10000000 100000000
```

#### Run the unit tests suite
```batch
.\build\bin\Release\test_cpp.exe
.\build\bin\Release\test_cuda.exe
```

## Linux

### Prerequisites
You can install the developer versions of zlib and libpng using the software package manager native to your Linux distribution (i.e. `sudo apt-get install zlib1g-dev libpng-dev` for Debian/Ubuntu or `sudo yum install zlib-devel libpng-devel` for CentOS/Fedora).

You will also need to make sure you have gcc, cmake and the cuda toolkit installed on your host.

### Build the Map/Reduce Samples
```bash
cd ..
cd map_reduce
mkdir build
cmake -S . -B build
cmake --build build
```

### Run
#### Compare Mandelbrot image generation times:
```bash
./build/bin/mandel_cpp mandel_cpp_4k.png 3840 2160 8192 -3.555556 -2.0 4.0
./build/bin/mandel_cuda mandel_cuda_4k.png 3840 2160 8192 -3.555556 -2.0 4.0
```

#### Compare dot product performance times:
```bash
./build/bin/perf_cpp cpp.csv 10 100 1000 10000 100000 1000000 10000000 100000000
./build/bin/perf_cuda cuda.csv 10 10 100 1000 10000 100000 1000000 10000000 100000000
```

#### Run the unit tests suite
```bash
./build/bin/test_cpp
./build/bin/test_cuda
```
