aux_source_directory(./ TEST_SOURCE)
add_executable(tester ${TEST_SOURCE})
find_package(OpenBLAS)
target_link_libraries(tester PRIVATE OpenBLAS::OpenBLAS)
target_link_libraries(tester PRIVATE c_matrix)
if ("${CMAKE_BUILD_TYPE} " STREQUAL "Debug ")
    add_definitions(-DTEST_DEBUG)
endif ()
function(test_auto_adder target rows columns )

add_test(
    NAME ${rows}-${columns}
    COMMAND $<TARGET_FILE:${target}> ${rows} ${columns}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

test_auto_adder(tester  16 16)
test_auto_adder(tester  128 128)
test_auto_adder(tester  1000 1000)
test_auto_adder(tester  8000 8000 )
test_auto_adder(tester  64000 64000 )
