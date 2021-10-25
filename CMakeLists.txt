cmake_minimum_required(VERSION 3.9.4)
project(dooce LANGUAGES CXX C)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT dooce)

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
file(GLOB_RECURSE FILES "src/*.cpp" "include/*.h" "extern/dearimgui/src/*.cpp")

find_package(OpenGL COMPONENTS OpenGL)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(extern/glfw)
add_subdirectory(tests)

add_executable(dooce ${FILES})
target_include_directories(dooce PRIVATE extern/glfw/include)
target_include_directories(dooce PRIVATE extern/dearimgui/include)
target_include_directories(dooce PRIVATE "include")
target_include_directories(dooce PRIVATE "extern/stbimage")
target_link_libraries(dooce glfw)
target_link_libraries (dooce ${OPENGL_LIBRARIES})
set_target_properties(dooce PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
	    CXX_STANDARD_REQUIRED ON
	     )
target_compile_definitions(dooce PUBLIC RESOURCE_DIR=\"${CMAKE_SOURCE_DIR}/pieces\") 
target_link_libraries(dooce OpenMP::OpenMP_CXX)