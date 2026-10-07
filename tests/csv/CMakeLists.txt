add_executable(tests_hw_1 tests.c)
target_link_libraries(tests_hw_1 writeTable)

add_test(NAME tests_hw_1 COMMAND tests_hw_1)
set_tests_properties(tests_hw_1 PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

