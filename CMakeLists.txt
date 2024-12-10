cmake_minimum_required(VERSION 3.22)
project(dasex_pipeline_dispatcher)

set(CMAKE_CXX_STANDARD 17)

add_executable(dasex_pipeline_dispatcher main.cpp)
add_executable(TestWorker tests/TestWorker.cc)