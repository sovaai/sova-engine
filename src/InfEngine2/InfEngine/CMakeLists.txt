cmake_minimum_required( VERSION 3.7 )

project( InfEngine )

find_package( FLEX REQUIRED )
if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
	cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
cmake_policy( SET CMP0054 NEW )
find_package( ICU COMPONENTS UC REQUIRED )

file( GLOB_RECURSE GLOB_SOURCES *.hpp *.h *.cpp *.c )
aux_source_directory( . SOURCES )

add_library( ${PROJECT_NAME} STATIC ${GLOB_SOURCES} ${SOURCES} ${PROTO_SRCS} )

target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )

# target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../.. )

target_link_libraries( ${PROJECT_NAME} PUBLIC NanoLib ClientLib )

set( ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/InfFunctions/InternalFunctions/FindContacts )

set( FLEX ${ROOT_DIR}/contacts_finder.flex )
set( FLEX_SOURCE ${CMAKE_BINARY_DIR}/contacts_finder.cpp )
add_custom_command(
	OUTPUT ${FLEX_SOURCE}
	COMMAND ${FLEX_EXECUTABLE} --outfile=${FLEX_SOURCE} ${FLEX}
	DEPENDS ${FLEX}
	COMMENT "Generating ${FLEX_SOURCE}"
)

file( GLOB SOURCES ${ROOT_DIR}/*.cpp ${ROOT_DIR}/*.hpp )

target_include_directories( ${PROJECT_NAME} PRIVATE ${ROOT_DIR} ${ICU_INCLUDE_DIRS} )

target_compile_definitions( ${PROJECT_NAME} PUBLIC -DUSE_INF_TERMS -DCSDATA_EXTENDED )

target_sources( ${PROJECT_NAME} PRIVATE ${FLEX_SOURCE} ${SOURCES} ${FLEX} )
target_compile_options( ${PROJECT_NAME} PRIVATE -Wno-deprecated-register )

# Интерфейсная библиотека для использования класса iAnswer без всей библиотеки InfEngine.
add_library( "${PROJECT_NAME}-iAnswer" INTERFACE )
target_sources( "${PROJECT_NAME}-iAnswer" INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/iAnswer.cpp
	${CMAKE_CURRENT_LIST_DIR}/iAnswer.hpp )
target_link_libraries( "${PROJECT_NAME}-iAnswer" INTERFACE NanoLib-nMemoryAllocator )
target_include_directories( "${PROJECT_NAME}-iAnswer" INTERFACE ${ICU_INCLUDE_DIRS} )

# Интерфейсная библиотека для использования класса Vars::Vector без всей библиотеки InfEngine.
add_library( "${PROJECT_NAME}-Vars" INTERFACE )
target_sources( "${PROJECT_NAME}-Vars" INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/Vars/Vector.cpp
	${CMAKE_CURRENT_LIST_DIR}/Vars/Vector.hpp )
target_link_libraries( ${PROJECT_NAME}-Vars INTERFACE )
