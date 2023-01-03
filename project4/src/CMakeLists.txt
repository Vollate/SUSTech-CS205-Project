
aux_source_directory(./ SOURCE)
add_library(c_matrix ${SOURCE})
if(OpenMP_FOUND)
    target_link_libraries(c_matrix PUBLIC OpenMP::OpenMP_C)
endif()
target_include_directories(c_matrix PUBLIC "${CMAKE_SOURCE_DIR}/include")

