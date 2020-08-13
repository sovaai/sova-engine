cmake_minimum_required( VERSION 3.7 )

project( InfServer )

add_executable( ${PROJECT_NAME}
	InfEngineServer.hpp
	InfEngineServer.cpp
	MainServer.cpp )

target_link_libraries( ${PROJECT_NAME} PRIVATE InfEngine terms ClientLib ${CMAKE_DL_LIBS} )
