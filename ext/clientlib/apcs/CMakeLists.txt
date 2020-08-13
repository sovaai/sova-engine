project( apcs )

cmake_minimum_required( VERSION 2.6 )

add_definitions( -DCSDATA_EXTENDED )

set( CMAKE_C_FLAGS "-fPIC" )
set( CMAKE_CXX_FLAGS "-fPIC" )

file( GLOB SOURCES *.h *.c )

add_library( ${PROJECT_NAME}obj OBJECT ${SOURCES} )
add_library( ${PROJECT_NAME} STATIC ${SOURCES} )
target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/.. )
target_link_libraries( ${PROJECT_NAME} m )
