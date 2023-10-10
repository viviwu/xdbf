cmake_minimum_required(VERSION 3.17)
project(dbf_tool CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

set(LIBRARY_OUTPUT_PATH     ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)

include_directories(../../common/base ./)

add_executable(dbf_tool
        main.cpp
        dbf_doc.cpp
        )