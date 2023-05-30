cmake_minimum_required(VERSION 3.25)
project(BasicSnake C)

set(CMAKE_C_STANDARD 11)
set( LIBS XInput )

add_executable(BasicSnake main.c WindowRenderer.c PCG_Input.c)
TARGET_LINK_LIBRARIES(BasicSnake ${LIBS} )
