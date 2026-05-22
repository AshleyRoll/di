cmake_minimum_required(VERSION 4.3)
project(di_test)

set(CMAKE_CXX_STANDARD 26)


add_executable(di_test main.cpp)
target_compile_options(di_test PRIVATE -freflection)
target_include_directories(di_test PUBLIC include)