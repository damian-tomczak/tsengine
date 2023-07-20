project(${PROJECT_NAME}_tests)

add_executable(${PROJECT_NAME} tester.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
    tsengine
)

add_test(TestAll ${PROJECT_NAME})