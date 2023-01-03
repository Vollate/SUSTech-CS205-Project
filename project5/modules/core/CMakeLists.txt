aux_source_directory(src SOURCES)
add_library(RM_core OBJECT ${SOURCES})
target_include_directories(RM_core PRIVATE include)
