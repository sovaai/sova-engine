cmake_minimum_required( VERSION 3.7 )

project( fstorage )
add_library( ${PROJECT_NAME} STATIC fstorage.c )
target_link_libraries( ${PROJECT_NAME} PUBLIC mm_file )
target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../.. )
