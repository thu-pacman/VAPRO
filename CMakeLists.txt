cmake_minimum_required(VERSION 3.5)
project(VAPRO)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)


SET(CMAKE_CXX_FLAGS "-Wall -g -O2")
SET(CMAKE_SHARED_LINKER_FLAGS "-g")
ADD_DEFINITIONS(-DDEBUG)
set(CMAKE_CXX_STANDARD 11)

# shared library
set(WRAP ${PROJECT_SOURCE_DIR}/wrap/wrap.py)
include(${PROJECT_SOURCE_DIR}/wrap/WrapConfig.cmake)

add_wrapped_file(papicnt.cpp papicnt.w)
add_library(papicnt SHARED papi_wrap.h papi_wrap.cpp papicnt.cpp clustering.cpp clustering.h)
target_link_libraries(papicnt ${MPI_CXX_LIBRARIES} unwind unwind-x86_64 papi jsoncpp)
