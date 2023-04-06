project(${CMAKE_PROJECT_NAME}Test)

# TODO: split project into tests for engine and game tests

add_executable(${PROJECT_NAME} tester.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
)

add_test(DummyTests
    ${PROJECT_NAME}
    WORKING_DIRECTORY ${OUTPUT_DIR}
)