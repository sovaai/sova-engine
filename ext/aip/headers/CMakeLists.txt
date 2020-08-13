cmake_minimum_required( VERSION 3.7 )

# Фиктивная библиотека для установки путей к заголовочным файлам A&P библиотеки.
project( aipheaders )

file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.c "int c;" )
add_library( ${PROJECT_NAME} STATIC ${CMAKE_CURRENT_BINARY_DIR}/dummy.c )

target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../src/ )

target_compile_definitions( ${PROJECT_NAME} PUBLIC
	-DPTS2_CROSS_HASHTABLE_SIZE=64 -DPTS2_CROSS_HASHTABLE_INC=32 -D_STL=std -DUSE_INF_TERMS )
