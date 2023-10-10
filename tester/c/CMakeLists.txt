cmake_minimum_required(VERSION 3.17)
project(dbf C)

set(CMAKE_C_STANDARD 11)

set(LIBRARY_OUTPUT_PATH     ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)

include_directories(../../common/3rd/libdbf/include ../../common/base)

aux_source_directory(../../common/3rd/libdbf/src LIB_DBF_SRC)

add_library(dbf ${LIB_DBF_SRC})

add_executable(read_dbf read_dbf.c)
target_link_libraries(read_dbf dbf)

add_executable(write_dbf write_dbf.c)
target_link_libraries(write_dbf dbf)