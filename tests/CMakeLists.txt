
add_executable(TEST_reactive "main.cpp")
target_link_libraries(TEST_reactive reactive)
target_include_directories(TEST_reactive PUBLIC "../include")

add_dependencies(TEST_reactive reactive)

add_test(NAME TEST_reactive COMMAND TEST_reactive)
