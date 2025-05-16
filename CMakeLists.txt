cmake_minimum_required(VERSION 3.17)
project(atgo_dbf C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

#set(LIBRARY_OUTPUT_PATH     ${CMAKE_CURRENT_SOURCE_DIR}/lib)
#set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(common/3rd/libdbf/include common/dbf common/utils .)

# aux_source_directory(common/3rd/libdbf/src LIB_DBF_SRC) # collect *.c/cpp, but *.h files will be ignored!!!
FILE(GLOB_RECURSE LIB_DBF_SRC common/3rd/libdbf/src/*.h common/3rd/libdbf/src/*.c)
message("LIB_DBF_SRC: ${LIB_DBF_SRC}")
add_library(libdbf ${LIB_DBF_SRC})

add_executable(libdbf_tester libdbf_tester.c atgo/atgo_struct.h utils.h)
target_link_libraries(libdbf_tester libdbf)

add_executable(libdbf_reader libdbf_reader.c utils.h)
target_link_libraries(libdbf_reader libdbf)

add_executable(dbf_doc_teser
        dbf_doc_teser.cpp
        common/dbf/dbf_def.h
        common/dbf/dbf_doc.h
        common/dbf/dbf_doc.cpp
        common/utils/tool.cpp
        common/dbf/dbf_doc.cpp
        utils.h
)



