cmake_minimum_required(VERSION 3.26)

project(ts VERSION 0.0.1 LANGUAGES CXX)

set(GAME_NAME "Awesome Game" CACHE STRING "Game name")
message(STATUS "Game name: ${GAME_NAME}")

if(NOT WIN32 AND NOT CMAKE_COMPILER_IS_MSVC)
    message(WARNING "Config was tested only on a environment with Windows system and MVSC compiler!")
endif()

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.35.32216.1")
    message(WARNING
        "Config was tested on the MSVC compiler ver 19.35.32216.1\n"
        "Your MSVC compiler ver:  ${CMAKE_CXX_COMPILER_VERSION}"
    )
endif()

if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    message(FATAL_ERROR "Config was tested only on AMD64 arch")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo;Release" CACHE STRING "Available configs" FORCE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ENABLE_TESTS "Test the engine basic operations" ON)

set(EXTERNAL_DIR external)
get_filename_component(EXTERNAL_DIR ${EXTERNAL_DIR} ABSOLUTE)

set(ASSETS_DIR assets)
get_filename_component(ASSETS_DIR ${ASSETS_DIR} ABSOLUTE)

set(CYBSDK_DIR CACHE STRING "Specify costum CybSDK path")

add_subdirectory(${EXTERNAL_DIR})

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(game)
add_subdirectory(engine)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME}game)

set_property(TARGET gmock PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET gmock_main PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET gtest PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET gtest_main PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET gtest_main PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET tinyobjloader PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET Continuous PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET Experimental PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET Nightly PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET NightlyMemoryCheck PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET generate_openxr_header PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET xr_global_generated_files PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET GenericCodeGen PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET glslang PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET glslang-default-resource-limits PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET MachineIndependent PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET OGLCompiler PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET OSDependent PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET SPIRV PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET SPVRemapper PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET uninstall PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET HLSL PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET openxr_loader PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET glslangValidator PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET spirv-remap PROPERTY FOLDER "ThirdPartyLibraries")

set_property(TARGET copy_assets PROPERTY FOLDER "tools")
set_property(TARGET copy_assets_tests PROPERTY FOLDER "tools")