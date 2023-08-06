project(${PROJECT_NAME}_tests)

add_executable(${PROJECT_NAME} tester.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
    tsengine
)


#  First Set source and destination directories then add custom target to copy directory then add to dependecies
# set(source_dir ${CMAKE_SOURCE_DIR}/path/to/source_directory)
# set(destination_dir ${CMAKE_BINARY_DIR}/path/to/destination_directory)

add_custom_target(copy_assets_to_test
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSETS_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

add_dependencies(${PROJECT_NAME} copy_assets_to_test)

add_test(TestAll ${PROJECT_NAME})