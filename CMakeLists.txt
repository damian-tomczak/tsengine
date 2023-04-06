cmake_minimum_required(VERSION 3.10)

project(TSEngine VERSION 0.0.1 LANGUAGES CXX)

if(NOT WIN32 AND NOT CMAKE_COMPILER_IS_MSVC)
    message(WARNING "Config was tested only on a environment with Windows system and MVSC compiler!")
endif()

if(NOT DEFINED ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(EXTERNAL_PATH "external")
get_filename_component(ABSOLUTE_EXTERNAL_PATH ${EXTERNAL_PATH} ABSOLUTE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)

option(UNIT_TEST "Test the basic operation" ON)
if(UNIT_TEST)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    enable_testing()
    add_subdirectory(${ABSOLUTE_EXTERNAL_PATH}/googletest)
    add_subdirectory(tests)
endif()