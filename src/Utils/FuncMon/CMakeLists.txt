cmake_minimum_required( VERSION 3.7 )

if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
	cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
find_package( ICU COMPONENTS UC DATA REQUIRED )

project( FuncMon )

add_executable( ${PROJECT_NAME} FuncMon.cpp )

target_link_libraries( ${PROJECT_NAME} PRIVATE InfEngine ${ICU_UC_LIBRARIES} ${CMAKE_DL_LIBS} )
