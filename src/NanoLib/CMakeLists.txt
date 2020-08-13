cmake_minimum_required( VERSION 3.7 )

project( NanoLib )

#add_definitions( -DUSE_INF_TERMS -DCSDATA_EXTENDED )

#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

find_package( GMP REQUIRED )
find_package( MEMCACHED REQUIRED )
if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
	cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
cmake_policy( SET CMP0054 NEW )
find_package( ICU COMPONENTS uc REQUIRED )

file( GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
file( GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" )

add_library( ${PROJECT_NAME} STATIC
	${SOURCES} ${HEADERS} )

target_include_directories( ${PROJECT_NAME} PRIVATE
	${MEMCACHED_INCLUDE_DIRS}
	${CMAKE_CURRENT_LIST_DIR}/../.. # @todo Надо сделать включение общим.
	)

target_include_directories( ${PROJECT_NAME} PUBLIC
	${GMP_INCLUDE_DIRS}
	${CMAKE_CURRENT_LIST_DIR}/..
	${ICU_INCLUDE_DIRS}
	)

target_link_libraries( ${PROJECT_NAME} PRIVATE md5a )

target_link_libraries( ${PROJECT_NAME} PUBLIC ${GMP_LIBRARIES} ${MEMCACHED_LIBRARIES} aipheaders )

# Интерфейсная библиотека для использования класса LogSystem без всей библиотеки NanoLib.
add_library( "${PROJECT_NAME}-LogSystem" INTERFACE )
target_sources( "${PROJECT_NAME}-LogSystem" INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/LogSystem.cpp
	${CMAKE_CURRENT_LIST_DIR}/LogSystem.hpp )

target_include_directories( "${PROJECT_NAME}-LogSystem" INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/.. )

# Интерфейсная библиотека для использования класса nMemoryAllocator без всей библиотеки NanoLib.
add_library( "${PROJECT_NAME}-nMemoryAllocator" INTERFACE )
target_sources( "${PROJECT_NAME}-nMemoryAllocator" INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/nMemoryAllocator.cpp
	${CMAKE_CURRENT_LIST_DIR}/nMemoryAllocator.hpp
	${CMAKE_CURRENT_LIST_DIR}/nMemoryAllocator.h )
target_include_directories( "${PROJECT_NAME}-nMemoryAllocator" INTERFACE ${CMAKE_CURRENT_LIST_DIR}/.. )
target_link_libraries( ${PROJECT_NAME}-nMemoryAllocator INTERFACE aipheaders )

# Интерфейсная библиотека для использования класса LongFloat без всей библиотеки NanoLib.
add_library( "${PROJECT_NAME}-LongFloat" INTERFACE )
target_sources( "${PROJECT_NAME}-LongFloat" INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/LongFloat.cpp
	${CMAKE_CURRENT_LIST_DIR}/LongFloat.hpp )
target_include_directories( "${PROJECT_NAME}-LongFloat" INTERFACE ${CMAKE_CURRENT_LIST_DIR}/.. ${GMP_INCLUDE_DIRS} )
target_link_libraries( "${PROJECT_NAME}-LongFloat" INTERFACE ${GMP_LIBRARIES} )

# Интерфейсная библиотека для использования класса NumberConvertor без всей библиотеки NanoLib.
add_library( "${PROJECT_NAME}-NumberConvertor" INTERFACE )
target_sources( "${PROJECT_NAME}-NumberConvertor" INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/NumberConvertor.cpp
	${CMAKE_CURRENT_LIST_DIR}/NumberConvertor.hpp )
target_link_libraries( "${PROJECT_NAME}-NumberConvertor" INTERFACE ${PROJECT_NAME}-LongFloat )
