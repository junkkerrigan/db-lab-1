cmake_minimum_required(VERSION 3.15)
project(db_lb_1 C)

set(CMAKE_C_STANDARD 99)

add_executable(db_lb_1 main.c outputUtils.c outputUtils.h types.h constants.h constants.c)