project(${PROJECT_NAME}_tests)

add_executable(${PROJECT_NAME} tester.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
    tsengine
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../src
)

add_custom_target(copy_assets_tests
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSETS_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# TODO: tests for every build type
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_dependencies(${PROJECT_NAME} copy_assets_tests)

add_test(DummyTests ${PROJECT_NAME} --gtest_filter=DummyTests.*)
add_test(MathTests ${PROJECT_NAME} --gtest_filter=MathTests.*)

option(CI_RUNNING "" OFF)

if(NOT CI_RUNNING)
    add_test(GameTests ${PROJECT_NAME} --gtest_filter=GameTests.*)
endif()