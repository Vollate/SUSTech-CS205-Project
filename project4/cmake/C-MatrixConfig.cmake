set(lib_name "C-Matrix")
get_filename_component(current_dir C-MatrixConfig.cmake PATH)
include(${current_dir}/${CMAKE_BUILD_TYPE}/${lib_name}.cmake)
