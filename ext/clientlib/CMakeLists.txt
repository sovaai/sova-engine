project( ClientLib )
cmake_minimum_required( VERSION 2.6 )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

add_subdirectory( nMemoryAllocator )
add_subdirectory( apcs )
add_subdirectory( ClientLib )
