add_library(writeTable cs.c functions.h)

add_executable(hw1 main.c)

target_include_directories(writeTable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(hw1 writeTable)

