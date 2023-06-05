project(${CMAKE_PROJECT_NAME}Game)

file(GLOB_RECURSE SRC_FILES
    *.cpp
)

file(GLOB_RECURSE H_FILES
    *.h
)

file(GLOB_RECURSE HPP_FILES
    *.hpp
)

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${H_FILES}
    ${HPP_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    TSEngine
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
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
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
    VS_DEBUGGER_WORKING_DIRECTORY ${OUTPUT_DIR}
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC
        $<IF:$<CONFIG:Debug>, /Zi /Od /MDd, /O2 /MD>
        /W4 /WX
    )
endif()