cmake_minimum_required(VERSION 3.13)
project(HuffmanCoding)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR})

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif ()

add_library(huffman-lib STATIC
        buffered_io/buffered_reader.h
        buffered_io/buffered_writer.h
        buffered_io/buffered_reader.cpp
        buffered_io/buffered_writer.cpp
        constants.h
        huffman.h
        huffman.cpp)

add_executable(compression-testing tests.cpp
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc)

target_link_libraries(compression-testing huffman-lib -lpthread)

add_executable(compression-utility utility.cpp)
target_link_libraries(compression-utility huffman-lib)
