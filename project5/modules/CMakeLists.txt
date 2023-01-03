add_subdirectory(core)

add_library(RawMat STATIC $<TARGET_OBJECTS:RM_core>)
target_include_directories(RawMat SYSTEM BEFORE PUBLIC core)