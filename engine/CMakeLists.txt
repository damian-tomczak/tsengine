cmake_minimum_required(VERSION 3.26)

project(${CMAKE_PROJECT_NAME}engine)

function(os_files files_list_name regex)
    set(files_list ${${files_list_name}})
    list(FILTER files_list EXCLUDE REGEX "src/os/*")

    set(OS_FILES)
    if (WIN32)
        file(GLOB_RECURSE OS_FILES
            src/os/win32/${regex}
        )
    else()
        message(FATAL_ERROR "Not implemented")
    endif()

    list(LENGTH OS_FILES OS_FILES_LENGTH)
    if (OS_FILES_LENGTH EQUAL 0)
        message(FATAL_ERROR "Files specific for your system couldn't be found")
    endif()

    list(APPEND files_list ${OS_FILES})
    set(${files_list_name} ${files_list} PARENT_SCOPE)
endfunction()

if (NOT DEFINED VK_USE_PLATFORM)
    if (WIN32)
        set(VK_USE_PLATFORM "WIN32")
    endif()
endif()

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

os_files(SRC_FILES *.cpp)

file(GLOB_RECURSE H_FILES
    src/*.h
    include/*.h
)

os_files(H_FILES *.h)

file(GLOB_RECURSE HPP_FILES
    src/*.hpp
    include/*.hpp
)

set(OTHER_FILES
    src/vulkan/vulkan_functions.inl
)

add_library(${PROJECT_NAME} STATIC
    ${SRC_FILES}
    ${H_FILES}
    ${HPP_FILES}
    ${OTHER_FILES}
)

target_sources(${PROJECT_NAME} PRIVATE
    ${EXTERNAL_DIR}/glslang/glslang/ResourceLimits/ResourceLimits.cpp
    ${EXTERNAL_DIR}/glslang/glslang/ResourceLimits/resource_limits_c.cpp)

target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Vulkan-Headers
    glslang
    SPIRV
    openxr_loader
    headers
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

if (WIN32)
    set(OS_DIR src/os/win32)
else()
    message(FATAL_ERROR "Not implemented")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    ${OS_DIR}
    ${EXTERNAL_DIR}/glslang # glslang doesn't support modern cmake approach
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    VK_USE_PLATFORM_${VK_USE_PLATFORM}_KHR
    VK_NO_PROTOTYPES
    XR_USE_GRAPHICS_API_VULKAN
    TSENGINE_BULDING
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    ENGINE_NAME="${PROJECT_NAME}"
)

source_group(TREE ${CMAKE_SOURCE_DIR}/engine FILES
    ${SRC_FILES}
    ${H_FILES}
    ${HPP_FILES}
    ${OTHER_FILES}
)

string(TOLOWER ${PROJECT_NAME} LOWERCASE_PROJECT_NAME)

set(OUTPUT_DIR_DEBUG ${CMAKE_BINARY_DIR}/output/${LOWERCASE_PROJECT_NAME}/Debug)
set(OUTPUT_DIR_RELEASE ${CMAKE_BINARY_DIR}/output/${LOWERCASE_PROJECT_NAME}/Release)

file(MAKE_DIRECTORY ${OUTPUT_DIR_DEBUG})
file(MAKE_DIRECTORY ${OUTPUT_DIR_RELEASE})

set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR_RELEASE}
    VS_DEBUGGER_WORKING_DIRECTORY $<$<CONFIG:Debug>:${OUTPUT_DIR_DEBUG}>$<$<CONFIG:Release>:${OUTPUT_DIR_RELEASE}>
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC
        $<IF:$<CONFIG:Debug>, /Zi /Od /MDd, /O2 /MD>
        /W3 /WX /wd4710
    )
endif()

option(ENABLE_TESTS "Test the basic operations" ON)
if(ENABLE_TESTS)
    enable_testing()
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(${EXTERNAL_DIR}/googletest ${CMAKE_BINARY_DIR}/external/googletest)
    add_subdirectory(tests)
endif()