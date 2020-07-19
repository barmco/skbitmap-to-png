cmake_minimum_required(VERSION 3.0.0)
project(skbitmap-to-png VERSION 0.0.1)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

set(PNG_LIBRARIES /usr/local/opt/libpng/lib)
set(PNG_INCLUDE_DIR /usr/local/opt/libpng/include)
set(ZLIB_ROOT /usr/local/opt/zlib/)

find_package(PNG REQUIRED)

if(NOT PNG_FOUND OR NOT ZLIB_FOUND)
    message (
        FATAL_ERROR 
        "one or more dependency was not found: png, zlib\n"
        "if you using mac osx platform, you can get the brew and install these dependencies\n"
        "good luck '^'b"
    )
endif()

set (
    INCLUDE_DIR_SUMMARY
    
    ${PNG_INCLUDE_DIRS}
)

set (
    SOURCES

    src/skbitmap-to-png.cpp
)

set (
    LIBRARIES

    PNG::PNG
)

include_directories(${INCLUDE_DIR_SUMMARY})

add_library(skbitmap-to-png SHARED ${SOURCES})

target_link_libraries(skbitmap-to-png PUBLIC ${LIBRARIES})