project( nMemoryAllocator )
cmake_minimum_required( VERSION 2.6 )

set( CMAKE_C_FLAGS "-fPIC" )
set( CMAKE_CXX_FLAGS "-fPIC" )

file( GLOB_RECURSE SOURCES nMemoryAllocator.h nMemoryAllocator.hpp nMemoryAllocator.cpp )

add_library( ${PROJECT_NAME} OBJECT ${SOURCES} )
target_compile_options( ${PROJECT_NAME} PRIVATE "--std=c++11" )

