cmake_minimum_required(VERSION 3.16)
set(C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
project(C-Matrix)

include_directories(header)
aux_source_directory(modules MODULES)
add_executable(MatrixDemo ${MODULES} samples/Demo.c)
