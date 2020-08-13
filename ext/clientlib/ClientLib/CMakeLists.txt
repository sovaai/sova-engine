project( ClientLib )
cmake_minimum_required( VERSION 2.6 )

set( CMAKE_C_STANDARD 90 )

file( GLOB SOURCES *.c *.cpp *.h *.hpp )

add_library( ${PROJECT_NAME} STATIC ${SOURCES} $<TARGET_OBJECTS:apcsobj> $<TARGET_OBJECTS:nMemoryAllocator> )

target_compile_options( ${PROJECT_NAME} PRIVATE -fPIC )

target_include_directories( ${PROJECT_NAME} PRIVATE .. )

target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/.. )
