cmake_minimum_required(VERSION 3.8)
project(dooce LANGUAGES CXX C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT dooce)
file(GLOB_RECURSE FILES "src/*.cpp" "include/*.h")

add_executable(dooce ${FILES})
target_include_directories(dooce PRIVATE "include")
set_target_properties(dooce
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

enable_testing()
add_subdirectory(tests)
