cmake_minimum_required(VERSION 3.8)
project(move_gen_test LANGUAGES CXX C)

file(GLOB_RECURSE FILES "../src/*.cpp")

add_executable(move_gen_test ${FILES} move_gen_test.cpp)
target_include_directories(move_gen_test PRIVATE "../include")
add_test(move_gen_test move_gen_test)