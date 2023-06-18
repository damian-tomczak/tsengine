project(${CMAKE_PROJECT_NAME}test)

add_executable(${PROJECT_NAME} tester.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
)

add_test(TestAll ${PROJECT_NAME})