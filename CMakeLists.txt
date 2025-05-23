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

#add_definitions(-w)

set(X_FLAG OFF)
if (X_FLAG)

    include_directories(common/utils .)

    # aux_source_directory(common/3rd/libdbf/src LIB_DBF_SRC) # collect *.c/cpp, but *.h files will be ignored!!!
    FILE(GLOB_RECURSE LIB_DBF_SRC common/3rd/libdbf/src/*.h common/3rd/libdbf/src/*.c)
    message("LIB_DBF_SRC: ${LIB_DBF_SRC}")
    add_library(libdbf ${LIB_DBF_SRC})

    add_executable(libdbf_tester libdbf_tester.c atx/atx_struct.h common/utils/c_utils.h common/utils/c_utils.c)
    target_link_libraries(libdbf_tester libdbf)
    target_include_directories(libdbf_tester PRIVATE common/3rd/libdbf/include)

    add_executable(libdbf_reader libdbf_reader.c common/utils/c_utils.h common/utils/c_utils.c)
    target_link_libraries(libdbf_reader libdbf)
    target_include_directories(libdbf_reader PRIVATE common/3rd/libdbf/include)

    add_executable(dbf_doc_teser
            common/dbf_doc/dbf_def.h
            common/dbf_doc/dbf_doc.h
            common/dbf_doc/dbf_doc.cpp
            common/utils/c_utils.h
            common/utils/c_utils.c
            atx/atx_struct.h
            atx/at_util.h
            atx/at_util.c
            dbf_doc_teser.cpp
    )
    target_include_directories(dbf_doc_teser PRIVATE common/dbf_doc)
    message("ignore these codes!")
endif ()

add_executable(dbf_tester
        common/dbf/dbf.h
        common/dbf/dbf.cpp
        common/dbf/dbf_tester.cpp
)
target_include_directories(dbf_tester PRIVATE common/dbf)


#if (WIN32)
#    target_compile_definitions(MyExecutable PRIVATE _CRT_SECURE_NO_WARNINGS)
#endif()

