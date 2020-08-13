cmake_minimum_required( VERSION 3.0 )

# Общее название всего проекта.
project( InfEngine )

# Включение системы тестирования.
enable_testing()

# Установка сборки по умолчанию в RelWithDebInfo.
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE RelWithDebInfo )
endif()

# Исключение вывода отладочной информации и отключение assert'ов для всех типов сборок, кроме Debug.
if( NOT CMAKE_BUILD_TYPE STREQUAL Debug )
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG" )
endif()

if( NOT CMAKE_BUILD_TYPE STREQUAL Release )
	# @todo нужно перенести в конкретные проекты.
	add_definitions( -DLOG_DATA_FLOW_ENABLED -DINF_ENGINE_STATISTICS )
endif()

# Отключение большого числа предупреждений. @todo: Необходимо избавиться от этого.
add_compile_options(
	-Wno-logical-not-parentheses
)
if( CMAKE_CXX_COMPILER_ID STREQUAL AppleClang )
	add_compile_options(
		-Wno-return-type-c-linkage
		-Wno-dynamic-class-memaccess
		-Wno-tautological-constant-compare
	)
endif()

# Поиск библиотек ICU для MacOs установленных через homebrew.
if( ${APPLE} )
	list( APPEND ICU_ROOT "/usr/local/opt/icu4c/" )
endif( ${APPLE} )

# @todo Включение версии стандарта для конкретных проектов.
set( USER_CXX_FLAGS "-std=c++11" )
set( CMAKE_CXX_STANDARD 11 )

# Добавление пути к дополнительным модулям CMake.
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeCustom" )

# Установка директории для сборки утилит.
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
# Установка директории для сборки библиотек.
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

add_subdirectory( ext )
add_subdirectory( src )
