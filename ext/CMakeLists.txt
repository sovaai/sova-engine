if( ${APPLE} AND ${CMAKE_HOST_APPLE} AND ( ${CMAKE_MAJOR_VERSION} GREATER 2 OR ${CMAKE_MINOR_VERSION} GREATER 7 ) )
	add_compile_options( -w )
endif()

add_subdirectory( clientlib )
add_subdirectory( aip )