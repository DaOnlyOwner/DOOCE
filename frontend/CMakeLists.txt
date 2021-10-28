cmake_minimum_required(VERSION 3.9.4)
project(dooce_frontend LANGUAGES CXX C)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT dooce_frontend)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

file(GLOB_RECURSE FILES "../src/*.cpp" "../include/*.h" "extern/dearimgui/src/*.cpp")

option(SET_RELATIVE_PATH "Set a relative path for the resources for installing" OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw)

add_executable(dooce_frontend ${FILES} src/frontend.cpp)
target_include_directories(dooce_frontend PRIVATE extern/glfw/include)
target_include_directories(dooce_frontend PRIVATE extern/dearimgui/include)
target_include_directories(dooce_frontend PRIVATE "../include")
target_include_directories(dooce_frontend PRIVATE "include")
target_include_directories(dooce_frontend PRIVATE "extern/stbimage")
target_link_libraries(dooce_frontend glfw)
target_link_libraries (dooce_frontend ${OPENGL_LIBRARIES})
set_target_properties(dooce_frontend PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
	    CXX_STANDARD_REQUIRED ON
	     )

if(SET_RELATIVE_PATH)
target_compile_definitions(dooce_frontend PUBLIC RESOURCE_DIR=\"pieces\") 
else()
target_compile_definitions(dooce_frontend PUBLIC RESOURCE_DIR=\"${CMAKE_SOURCE_DIR}/frontend/pieces\") 
endif()

install(TARGETS dooce_frontend DESTINATION dooce_application)
install(DIRECTORY pieces/ DESTINATION dooce_application/pieces)