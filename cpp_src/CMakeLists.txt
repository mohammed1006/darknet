cmake_minimum_required(VERSION 2.6)

project("Testing")
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} .)
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests test/tests.cpp 
                        layer/layer.cpp
                        layer/convolutional.cpp
                        layer/activation.cpp
              )
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)