cmake_minimum_required(VERSION 3.16)

project(better-calculator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(CAL_DEBUG)
endif ()

include_directories(BEFORE /usr/include/python3.10)
include_directories(BEFORE /usr/include/python3.8)
include_directories(BEFORE include)
include_directories(BEFORE library)


function(test_auto_adder TEST_NAME)
    add_test(
            NAME ${TEST_NAME}
            COMMAND $<TARGET_FILE:BetterCalculator> ${TEST_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

test_auto_adder(InputTest)
test_auto_adder(PlusTest)
test_auto_adder(MinusTest)
test_auto_adder(MultiplyTest)
test_auto_adder(DivideTest)
test_auto_adder(SqrtTest)
test_auto_adder(PowTest)

add_subdirectory(src)
