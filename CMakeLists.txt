cmake_minimum_required(VERSION 3.26)

project(TS VERSION 0.0.1 LANGUAGES CXX)

if(NOT WIN32 AND NOT CMAKE_COMPILER_IS_MSVC)
    message(WARNING "Config was tested only on a environment with Windows system and MVSC compiler!")
    if(CMAKE_CXX_COMPILER_VERSION NOT VERSION_EQUAL 19.35.32216.1)
        message(WARNING "Config was tested only on the MSVC compiler version 19.35.32216.1.")
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# TODO: consider add doxygen to the project
set(EXTERNAL_DIR external)
get_filename_component(EXTERNAL_DIR ${EXTERNAL_DIR} ABSOLUTE)
set(ASSETS_DIR assets)
get_filename_component(ASSETS_PATH ${ASSETS_DIR} ABSOLUTE)

add_subdirectory(${EXTERNAL_DIR}/vulkan)

add_subdirectory(game)
add_subdirectory(engine)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME}Game)

set_property(TARGET gmock PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET gmock_main PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET gtest PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET gtest_main PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET gtest_main PROPERTY FOLDER "ThirdPartyLibraries")