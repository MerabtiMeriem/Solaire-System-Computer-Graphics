
cmake_minimum_required(VERSION 3.16)

project(ComputerGraphics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(ComputerGraphics
    src/main.cpp
    src/camera.cpp
    src/solarSystem.cpp
)

target_include_directories(ComputerGraphics PRIVATE
    include
    ${GLUT_INCLUDE_DIRS}
)
    

target_link_libraries(ComputerGraphics freeglut opengl32 glu32)
