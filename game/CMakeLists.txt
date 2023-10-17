project(${CMAKE_PROJECT_NAME}game)

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
    tsengine::tsengine
)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES
    ${SRC_FILES}
    ${H_FILES}
    ${HPP_FILES}
)

set(OUTPUT_DIR_DEBUG ${CMAKE_BINARY_DIR}/output/${PROJECT_NAME}/Debug)
set(OUTPUT_DIR_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/output/${PROJECT_NAME}/RelWithDebInfo)
set(OUTPUT_DIR_RELEASE ${CMAKE_BINARY_DIR}/output/${PROJECT_NAME}/Release)

file(MAKE_DIRECTORY ${OUTPUT_DIR_DEBUG})
file(MAKE_DIRECTORY ${OUTPUT_DIR_RELWITHDEBINFO})
file(MAKE_DIRECTORY ${OUTPUT_DIR_RELEASE})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR_DEBUG}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR_RELEASE}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR_RELWITHDEBINFO}
    VS_DEBUGGER_WORKING_DIRECTORY $<$<CONFIG:Debug>:${OUTPUT_DIR_DEBUG}>$<$<CONFIG:RelWithDebInfo>:${OUTPUT_DIR_RELWITHDEBINFO}>$<$<CONFIG:Release>:${OUTPUT_DIR_RELEASE}>
)

add_custom_command(OUTPUT file.txt
    COMMAND ${CMAKE_COMMAND} -E copy_if_different C:\Users\damian\Desktop\file.txt ${OUTPUT_DIR_DEBUG}
    DEPENDS ${source}
)

# message("WARSZAWAWARSZAWA")
# file(GLOB_RECURSE FILES_TO_COPY ${ASSETS_DIR}/*)

# foreach(file ${FILES_TO_COPY})
#     message(${file})
#     file(RELATIVE_PATH relative_path ${ASSETS_DIR} ${file})
#     message(${relative_path})

#     set(source ${ASSETS_DIR}/${relative_path})
#     set(destination2 ${OUTPUT_DIR_DEBUG}/assets/${relative_path})
#     get_filename_component(destination ${destination2} DIRECTORY)

#     # Ensure the destination directory exists
#     get_filename_component(destination_dir ${destination} DIRECTORY)
#     file(MAKE_DIRECTORY ${destination_dir})
#     message(GDANSK${destination_dir})
#     message(STATUS ${source})
#     message(STATUS ${destination})
#     add_custom_command(OUTPUT ${destination2}
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${source} ${destination}
#         DEPENDS ${source}
#     )
# endforeach()
# message("POZNANPOZNAN")

# add_custom_target(copy-modified-files ALL DEPENDS ${FILES_TO_COPY})

file(COPY ${EXTERNAL_DLL_FILES} DESTINATION ${OUTPUT_DIR_DEBUG})
file(COPY ${EXTERNAL_DLL_FILES} DESTINATION ${OUTPUT_DIR_RELWITHDEBINFO})
file(COPY ${EXTERNAL_DLL_FILES} DESTINATION ${OUTPUT_DIR_RELEASE})