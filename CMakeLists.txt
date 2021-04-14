cmake_minimum_required(VERSION 3.19)
project(emu C)

set(CMAKE_C_STANDARD 99)

add_executable(emu src/main.c src/cpu.c src/cpu.h src/syscall.c src/syscall.h src/register.h)