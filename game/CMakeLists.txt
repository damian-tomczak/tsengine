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

file(GLOB_RECURSE ASSETS_FILES
    ${ASSETS_DIR}/*
)

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${H_FILES}
    ${HPP_FILES}
    ${ASSETS_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    TSEngine
)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES
    ${SRC_FILES}
    ${H_FILES}
    ${HPP_FILES}
    ${ASSETS_FILES}
)

string(TOLOWER ${PROJECT_NAME} LOWERCASE_PROJECT_NAME)

set(OUTPUT_DIR_DEBUG ${CMAKE_BINARY_DIR}/output/${LOWERCASE_PROJECT_NAME}/Debug)
set(OUTPUT_DIR_RELEASE ${CMAKE_BINARY_DIR}/output/${LOWERCASE_PROJECT_NAME}/Release)

file(MAKE_DIRECTORY ${OUTPUT_DIR_DEBUG})
file(MAKE_DIRECTORY ${OUTPUT_DIR_RELEASE})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR_RELEASE}
    VS_DEBUGGER_WORKING_DIRECTORY $<$<CONFIG:Debug>:${OUTPUT_DIR_DEBUG}>$<$<CONFIG:Release>:${OUTPUT_DIR_RELEASE}>
)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSETS_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

add_dependencies(${PROJECT_NAME} copy_assets)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC
        $<IF:$<CONFIG:Debug>, /Zi /Od /MDd, /O2 /MD>
        /W3 /WX
    )
endif()