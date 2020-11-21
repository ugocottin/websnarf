cmake_minimum_required(VERSION 3.17)
project(websnarf C)

set(CMAKE_C_STANDARD 99)

add_executable(websnarf main.c websnarf.c websnarf.h main.h server.c server.h socket.c socket.h)