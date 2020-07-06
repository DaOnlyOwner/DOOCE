cmake_minimum_required(VERSION 3.8)
project(dooce LANGUAGES CXX C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT dooce)
file(GLOB_RECURSE FILES "src/*.cpp" "src/*.h")

add_executable(dooce ${FILES})
set_target_properties(dooce PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(dooce PUBLIC cxx_std_17)

