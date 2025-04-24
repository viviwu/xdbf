cmake_minimum_required(VERSION 3.17)
project(atgo_dbf)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Include directories
include_directories(
#        ${CMAKE_CURRENT_SOURCE_DIR}
#        common/utils
        common/3rd/libdbf/include
)

add_executable(libdbf_tester
        libdbf_tester.c
        common/3rd/libdbf/include/libdbf/libdbf.h
        common/3rd/libdbf/src/dbf.h
        common/3rd/libdbf/src/dbf.c
        common/3rd/libdbf/src/dbf_endian.h
        common/3rd/libdbf/src/dbf_endian.c
        )


#add_executable(dbf_doc_teser
#        dbf_doc_teser.cpp
#        common/utils/tool.cpp
#        common/dbf/dbf_doc.cpp
#        atgo/atgo_dbf_doc.cpp
#        )



