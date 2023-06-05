project(${CMAKE_PROJECT_NAME}Engine)

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

file(GLOB_RECURSE H_FILES
    src/*.h
    include/*.h
)

file(GLOB_RECURSE HPP_FILES
    src/*.hpp
    include/*.hpp
)

add_library(${PROJECT_NAME} STATIC
    ${SRC_FILES}
    ${H_FILES}
    ${HPP_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    include
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS_PATH="${ASSETS_PATH}/")

source_group(TREE ${CMAKE_SOURCE_DIR}/engine FILES
    ${SRC_FILES}
    ${H_FILES}
    ${HPP_FILES}
)

string(TOLOWER ${PROJECT_NAME} LOWERCASE_PROJECT_NAME)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output/${LOWERCASE_PROJECT_NAME}/${CMAKE_BUILD_TYPE})
if(NOT EXISTS ${OUTPUT_DIR})
    file(MAKE_DIRECTORY ${OUTPUT_DIR})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
    VS_DEBUGGER_WORKING_DIRECTORY ${OUTPUT_DIR}
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC
        $<IF:$<CONFIG:Debug>, /Zi /Od /MDd, /O2 /MD>
        /W4 /WX /wd4710
    )
endif()

option(ENABLE_TESTS "Test the basic operations" ON)
if(ENABLE_TESTS)
    enable_testing()
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(${EXTERNAL_PATH}/googletest ${CMAKE_BINARY_DIR}/external/googletest)
    add_subdirectory(tests)
endif()