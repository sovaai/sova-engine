cmake_minimum_required( VERSION 3.7 )

project( stopdict_ex )
add_library( ${PROJECT_NAME} STATIC StopDictEx.cpp )
target_include_directories( ${PROJECT_NAME} PUBLIC ${ICU_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/../.. )
