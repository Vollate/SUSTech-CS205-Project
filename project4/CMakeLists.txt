cmake_minimum_required(VERSION 3.16)

set(C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(version 1.0.1)
set(project_name "C-Matrix")
project(${project_name} VERSION ${version})
add_link_options(-lm)
include(./cmake/preconfig.cmake)
add_subdirectory(src)
enable_testing()
add_subdirectory(test)

set(header_dir "include/${project_name}-${version}/${CMAKE_BUILD_TYPE}")
set(headers "${CMAKE_SOURCE_DIR}/include/Mat.h")
set(lib_root_dir "lib/${project_name}-${version}")
set(lib_dir "${lib_root_dir}/${CMAKE_BUILD_TYPE}")

install(
  TARGETS c_matrix
  EXPORT CMatrix
  DESTINATION ${lib_dir})
install(FILES ${headers} DESTINATION ${header_dir})
install(FILES "${CMAKE_SOURCE_DIR}/cmake/${project_name}Config.cmake"
        DESTINATION ${lib_root_dir})
