cmake_minimum_required(VERSION 3.16)

project(airport_avl C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

add_library(avl_tree STATIC avl_tree.c)
target_include_directories(avl_tree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(airport_database STATIC database.c)
target_link_libraries(airport_database PUBLIC avl_tree)
target_include_directories(airport_database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(airport_cli main.c)
target_link_libraries(airport_cli PRIVATE airport_database)

if(BUILD_TESTING)
    add_executable(test_avl tests/test_avl.c)
    target_link_libraries(test_avl PRIVATE avl_tree)
    add_test(NAME avl_rotations COMMAND test_avl rotations)
    add_test(NAME avl_insert_find COMMAND test_avl insert_find)
    add_test(NAME avl_delete COMMAND test_avl delete)

    add_executable(test_database tests/test_database.c)
    target_link_libraries(test_database PRIVATE airport_database)
    target_compile_definitions(test_database PRIVATE
        TEST_AIRPORTS_FILE="${CMAKE_CURRENT_SOURCE_DIR}/tests/data/airports_sample.txt"
    )
    add_test(NAME database_load COMMAND test_database load)
    add_test(NAME database_load_failure COMMAND test_database failure)
endif()
