# Created By Yun Yang
cmake_minimum_required(VERSION 3.10)

project(glad VERSION 3.3)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)