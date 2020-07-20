cmake_minimum_required(VERSION 3.0.0)
project(skbitmap-to-png VERSION 0.0.1)

# make force to find version of static library '^'b
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# xcode and their sdk is sucks, therefore send tribute for brew '^'b
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

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/skcms)
    message (
        FATAL_ERROR
        "it seems submodule \"skcms\" ins't initialized. type following command and initialize them.\n"
        "\"git submodule update\""
    )
endif()

add_custom_target(skcms-object
    COMMAND ninja -C skcms
    COMMAND ${CMAKE_COMMAND} -E copy skcms/out/skcms.o ${CMAKE_BINARY_DIR}
    COMMENT build skcms

    BYPRODUCTS ${CMAKE_BINARY_DIR}/skcms.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(skcms STATIC IMPORTED GLOBAL)
add_dependencies(skcms skcms-object)
set_target_properties(skcms PROPERTIES
    IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/skcms.o
)

set (
    INCLUDE_DIR_SUMMARY

    ${PNG_INCLUDE_DIRS}
    
    src
    skcms
)

set (
    SOURCES

    src/skbitmap-to-png.cc
)

set (
    LIBRARIES

    PNG::PNG
    skcms
)

include_directories(${INCLUDE_DIR_SUMMARY})

add_library(skbitmap-to-png SHARED ${SOURCES})
add_dependencies(skbitmap-to-png skcms)

target_link_libraries(skbitmap-to-png PRIVATE ${LIBRARIES})
target_compile_options(skbitmap-to-png PRIVATE
    -std=c++11
)