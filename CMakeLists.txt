cmake_minimum_required(VERSION 3.10)
project(Vapro)

option(BUILD_EXAMPLES "Example programs for test" ON)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

include_directories(include)
include_directories(3rd-party/dbg-macro)
# Required packages: libunwind, papi, jsoncpp

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)
set(CMAKE_CXX_STANDARD 17)


SET(CMAKE_CXX_FLAGS "-Wall -g")
SET(CMAKE_SHARED_LINKER_FLAGS "-g")

# # shared library
# set(WRAP ${PROJECT_SOURCE_DIR}/wrap/wrap.py)
# include(${PROJECT_SOURCE_DIR}/wrap/WrapConfig.cmake)
# link_directories(${CMAKE_SOURCE_DIR}/jevents)

# add_wrapped_file(papicnt.cpp papicnt.w)
file(GLOB_RECURSE SRC_Vapro src/*.cc)

add_library(vapro SHARED ${SRC_Vapro})
target_link_libraries(vapro ${MPI_CXX_LIBRARIES} papi)
# target_link_libraries(vapro ${MPI_CXX_LIBRARIES} unwind unwind-x86_64 papi jsoncpp jevents)

add_executable(test_vapro test/test_collector.cc)
target_link_libraries(test_vapro vapro)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
