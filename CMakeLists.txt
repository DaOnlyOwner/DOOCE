cmake_minimum_required(VERSION 3.9.4)
project(dooce LANGUAGES CXX C)

include(CheckIPOSupported)
check_ipo_supported(RESULT sup OUTPUT err)

if(sup)
   message(STATUS "LTO supported")
   set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
   message(STATUS "Warning: LTO not supported: ${error}")
endif() 

option(BUILD_TESTS "Build the move generation tests" ON)
option(BUILD_FRONTEND "Build the frontend" ON)

if(BUILD_FRONTEND)
	add_subdirectory(frontend)
endif()
if(BUILD_TESTS)
	add_subdirectory(tests)
endif()
