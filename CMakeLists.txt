cmake_minimum_required(VERSION 4.3)
project(di)

set(CMAKE_CXX_STANDARD 26)


add_executable(di src/main.cpp src/registration.cpp src/calculator.cpp)
target_compile_options(di PRIVATE -freflection)
target_include_directories(di PUBLIC include)