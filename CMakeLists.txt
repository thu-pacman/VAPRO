cmake_minimum_required(VERSION 3.5)
project(Vapro)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# Required packages: libunwind, papi, jsoncpp

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)


SET(CMAKE_CXX_FLAGS "-Wall -g -O2 -Wno-deprecated-declarations -Wno-sign-compare")
SET(CMAKE_SHARED_LINKER_FLAGS "-g")
ADD_DEFINITIONS(-DDEBUG)
set(CMAKE_CXX_STANDARD 11)

# shared library
set(WRAP ${PROJECT_SOURCE_DIR}/wrap/wrap.py)
include(${PROJECT_SOURCE_DIR}/wrap/WrapConfig.cmake)
link_directories(${CMAKE_SOURCE_DIR}/jevents)

add_wrapped_file(papicnt.cpp papicnt.w)
add_library(papicnt SHARED papi_wrap.h papi_wrap.cpp papicnt.cpp clustering.cpp clustering.h online_analyse.cpp online_analyse.h utility.cpp utility.h SocketClient.cpp SocketClient.h pmu.cpp pmu.h wrapper.cpp pmc_wrapper.h pmc_jevents.cpp)
target_link_libraries(papicnt ${MPI_CXX_LIBRARIES} unwind unwind-x86_64 papi jsoncpp jevents)

# Microbenchmarks

add_executable(example_mpi_program example_mpi_program.cpp)
target_link_libraries(example_mpi_program)

# add_executable(microbenchmark_loop microbenchmark_loop.cpp)
# target_link_libraries(microbenchmark_loop)

# add_executable(microbenchmark_pair_send microbenchmark_pair_send.cpp)
# target_link_libraries(microbenchmark_pair_send)

# share library for hooking all functions
# add_library(hook SHARED papi_wrap.h papi_wrap.cpp hook.cpp)
# target_link_libraries(hook papi dl jsoncpp unwind unwind-x86_64)
