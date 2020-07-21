cmake_minimum_required(VERSION 3.0.0)
project(skbitmap-to-png VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# make force to find version of static library '^'b
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# xcode and their sdk is sucks, therefore send tribute for brew '^'b
set(PNG_LIBRARIES /usr/local/opt/libpng/lib)
set(PNG_INCLUDE /usr/local/opt/libpng/include)
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

aux_source_directory(${CMAKE_SOURCE_DIR}/src SOURCES)

set (
    INCLUDE_DIR_SUMMARY

    ${PNG_INCLUDE}
    
    src
    skcms
)

set (
    SOURCES

    ${SOURCES}
)

set (
    LIBRARIES

    PNG::PNG
    skcms
)

include_directories(${INCLUDE_DIR_SUMMARY})

add_library(skbitmap-to-png SHARED ${SOURCES})
add_dependencies(skbitmap-to-png skcms)
set_target_properties(skbitmap-to-png PROPERTIES 
    PREFIX ""
)

target_link_libraries(skbitmap-to-png PRIVATE ${LIBRARIES})

# test

add_library(skbitmap-to-png-static STATIC ${SOURCES})
add_dependencies(skbitmap-to-png-static skcms)
set_target_properties(skbitmap-to-png-static PROPERTIES 
    PREFIX ""
)

target_link_libraries(skbitmap-to-png-static PRIVATE ${LIBRARIES})

add_executable(transform-to-png-basic test/transform_to_png_basic.cc)
add_dependencies(transform-to-png-basic skbitmap-to-png-static)
target_link_libraries(transform-to-png-basic PRIVATE skbitmap-to-png-static)

add_executable(transform-to-bgra8888-basic test/transform_to_bgra8888_basic.cc)
add_dependencies(transform-to-bgra8888-basic skbitmap-to-png-static)
target_link_libraries(transform-to-bgra8888-basic PRIVATE skbitmap-to-png-static)
