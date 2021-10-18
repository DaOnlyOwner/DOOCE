cmake_minimum_required(VERSION 3.11.0)
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

add_subdirectory(extern/pybind11)

# TODO: use this code -> include the pybind11 as pip package instead of a git submodule
# if(SKBUILD)
#   # Scikit-Build does not add your site-packages to the search path automatically,
#   # so we need to add it _or_ the pybind11 specific directory here.
#   execute_process(
#     COMMAND
#       "${PYTHON_EXECUTABLE}" -c
#       "import pybind11; print(pybind11.get_cmake_dir())"
#     OUTPUT_VARIABLE _tmp_dir
#     OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
#   list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
# endif()
# find_package(pybind11 CONFIG REQUIRED)


pybind11_add_module(dooce ${FILES})
target_compile_definitions(dooce PRIVATE VERSION_INFO=${PROJECT_VERSION})
target_include_directories(dooce PRIVATE "include")
set_target_properties(dooce
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
add_subdirectory(tests)
install(TARGETS dooce DESTINATION .)