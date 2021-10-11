cmake_minimum_required(VERSION 3.9.4)
project(move_gen_test LANGUAGES CXX C)

file(GLOB_RECURSE FILES_ "../include/*.h" "../src/*.cpp")
list(FILTER FILES_ EXCLUDE REGEX "main.cpp")

add_executable(move_gen_test ${FILES_} move_gen_test.cpp)
set_target_properties(move_gen_test
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
target_include_directories(move_gen_test PRIVATE "../include")
