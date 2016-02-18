cmake_minimum_required(VERSION 3.1)
project( Mandelbrot )

set(EXECUTABLE "mandelbrot")
set(VERSION_MAJOR 0 CACHE STRING "Project major version number.")
set(VERSION_MINOR 1 CACHE STRING "Project minor version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR)

## Change default build type to 'Debug'
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)

## Load packages fot cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)
include(Resources)

## Create some variables
file(GLOB SOURCE_FILES
	"src/*.cpp")
file(GLOB HEADER_FILES
	"include/*.h" "include/*.hpp")
file(GLOB SHADER_FILES
	"shader/*.glsl" "shader/*.vert" "shader/*.frag" "shader/*.geom")

## Show shaders in IDE
add_custom_target(Shader ALL SOURCES ${SHADER_FILES})

## Create source groups (for Visual Studio)
source_group("Headers" FILES ${HEADER_FILES})
source_group("Sources" FILES ${SOURCE_FILES})
source_group("Shaders" FILES ${SHADER_FILES})

## Add executable
add_executable("${EXECUTABLE}" ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories("${EXECUTABLE}" PUBLIC "include/")

## Add resources
add_resources("shaders" "shaders.h" ${SHADER_FILES})
target_link_libraries("${EXECUTABLE}" "shaders")

## Add include directories of libraries
target_include_directories("${EXECUTABLE}" PRIVATE ${OpenGL_INCLUDE_DIRS})
target_include_directories("${EXECUTABLE}" PRIVATE ${GLEW_INCLUDE_DIRS})
target_include_directories("${EXECUTABLE}" PRIVATE ${GLFW_INCLUDE_DIRS})
target_include_directories("${EXECUTABLE}" PRIVATE ${GLM_INCLUDE_DIRS})

## Link with libraries
target_link_libraries("${EXECUTABLE}" ${OPENGL_LIBRARIES})
target_link_libraries("${EXECUTABLE}" ${GLEW_LIBRARIES})
target_link_libraries("${EXECUTABLE}" ${GLFW_LIBRARIES})

## Use C++11
set_target_properties("${EXECUTABLE}" PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties("${EXECUTABLE}" PROPERTIES CXX_STANDARD 11)
