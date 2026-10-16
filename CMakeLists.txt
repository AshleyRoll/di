cmake_minimum_required(VERSION 4.3)
project(di)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 26)


add_executable(di src/main.cpp src/registry.cpp src/calculator.cpp)
target_compile_options(di PRIVATE -freflection -fno-rtti)
target_include_directories(di PUBLIC include)
