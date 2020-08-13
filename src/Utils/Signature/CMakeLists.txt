cmake_minimum_required( VERSION 3.0 )

project( CheckSignature )

add_executable( ${PROJECT_NAME} CheckSignature.cpp )

target_link_libraries( ${PROJECT_NAME} PRIVATE InfEngine terms ${CMAKE_DL_LIBS} )

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	-DPTS2_CROSS_HASHTABLE_SIZE=64
	-DPTS2_CROSS_HASHTABLE_INC=32
	-DUSE_INF_TERMS
	-DCSDATA_EXTENDED
	-D_STL=std
)
