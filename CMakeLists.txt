cmake_minimum_required(VERSION 3.0.0)
project(DramSimulator)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_library(inih INTERFACE)
target_include_directories(inih INTERFACE ext/headers)

add_library(format INTERFACE)
target_include_directories(format INTERFACE ext/fmt/include)
target_compile_definitions(format INTERFACE FMT_HEADER_ONLY=1)

# argparsing library, only used in main program not the library
add_library(args INTERFACE)
target_include_directories(args INTERFACE ext/headers)

add_library(json INTERFACE)
target_include_directories(json INTERFACE ext/headers)

# scala jni
option(BUILD_JNI "Whether or not to build interface for scala" OFF)
if (${BUILD_JNI})
    find_package(JNI REQUIRED)
    include_directories(${JAVA_INCLUDE_PATH})
    include_directories(${JAVA_INCLUDE_PATH2})
endif()

#if(DEFINED ENV{JAVA_HOME})
#    message("JAVA_HOME: $ENV{JAVA_HOME}")
#else()
#    message("NOT DEFINED JAVA_HOME VARIABLES")
#endif()
#include_directories($ENV{JAVA_HOME}/include $ENV{JAVA_HOME}/include/linux src)


set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -fPIC")

# Main DramSimulator Lib
set (DramSimulatorMainLib src/bankstate.cc
        src/channel_state.cc
        src/command_queue.cc
        src/common.cc
        src/configuration.cc
        src/controller.cc
        src/dram_system.cc
        src/hmc.cc
        src/refresh.cc
        src/simple_stats.cc
        src/timing.cc
        src/memory_system.cc
        src/cosimulation.cc)



if (${BUILD_JNI})
    add_library(DramSimulator SHARED
            ${DramSimulatorMainLib}
            src/DramSimulator_DramSimulator.cpp
            )
else()
    add_library(DramSimulator SHARED
            ${DramSimulatorMainLib}
            )
endif()



target_sources(DramSimulator
        PRIVATE src/cosimulation.cc)


if (CMD_TRACE)
    target_compile_options(DramSimulator PRIVATE -DCMD_TRACE)
endif (CMD_TRACE)

if (ADDR_TRACE)
    target_compile_options(DramSimulator PRIVATE -DADDR_TRACE)
endif (ADDR_TRACE)


target_include_directories(DramSimulator INTERFACE src)
target_compile_options(DramSimulator PRIVATE -Wall)
target_link_libraries(DramSimulator PRIVATE inih format)
set_target_properties(DramSimulator PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ..
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
