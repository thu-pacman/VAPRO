cmake_minimum_required(VERSION 3.10)
project(Vapro)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

option(BUILD_EXAMPLES "Example programs for test" ON)

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
# add_library(papicnt SHARED papi_wrap.h papi_wrap.cpp papicnt.cpp clustering.cpp clustering.h online_analyse.cpp online_analyse.h utility.cpp utility.h SocketClient.cpp SocketClient.h pmu.cpp pmu.h wrapper.cpp pmc_wrapper.h pmc_jevents.cpp)
# target_link_libraries(papicnt ${MPI_CXX_LIBRARIES} unwind unwind-x86_64 papi jsoncpp jevents)


# share library for hooking all functions
# add_library(hook SHARED papi_wrap.h papi_wrap.cpp hook.cpp)
# target_link_libraries(hook papi dl jsoncpp unwind unwind-x86_64)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
