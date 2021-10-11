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

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT dooce)
file(GLOB_RECURSE FILES "src/*.cpp" "include/*.h")

add_library(dooce ${FILES})
target_include_directories(dooce PRIVATE "include")
set_target_properties(dooce
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

add_subdirectory(tests)
