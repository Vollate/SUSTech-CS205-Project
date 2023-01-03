cmake_minimum_required(VERSION 3.16)

project(CXX-RawMatrixLib)
set(CMAKE_CXX_STANDARD 17)    

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_compile_options("/bigobj")
endif ()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#    add_compile_options("-stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=ld.lld")
    set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=ld.lld")
    set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=ld.lld")
endif ()


add_subdirectory(modules)

enable_testing()
add_subdirectory(test)

