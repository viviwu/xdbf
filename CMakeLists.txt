cmake_minimum_required(VERSION 3.17)
project(atgo_dbf)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

include_directories(
        ./../common/dbf
        ./atgo
)

add_executable(atgo_dbf
        main.cpp
        ../common/dbf/def_doc.cpp
        ./atgo/atgo_dbf_doc.cpp
        )

# Include directories
target_include_directories(atgo_dbf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})