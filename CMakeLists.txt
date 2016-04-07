cmake_minimum_required(VERSION 3.1)

project(stream-clean)

set(CMAKE_CXX_FLAGS "-std=c++14")

find_package(OpenMP)
if (NOT OPENMP_FOUND)
  message(WARNING "No OpenMP support")
  set(DISABLE_OPENMP ON)
  add_definitions(-DSTREAM_DISABLE_OPENMP)
endif()

set_source_files_properties(stream.c PROPERTIES LANGUAGE CXX )
add_executable(stream stream.c)
