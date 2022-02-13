cmake_minimum_required(VERSION 3.5)

project(serial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(serial
    source/serial_port.cpp
    source/line_reader.cpp
    source/string_splitter.cpp
    source/serial_port_impl_win32.cpp
    source/serial_port_impl_linux.cpp
    )

target_include_directories(serial PUBLIC include)
target_include_directories(serial PRIVATE source)
