cmake_minimum_required( VERSION 3.6 )

project( ExternalFunctions )

set( FUNCTIONS_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )

list( APPEND CMAKE_MODULE_PATH "${ROOT}/CMakeCustom" )

if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
	cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
find_package( ICU COMPONENTS UC REQUIRED )

macro( add_external_function FUNCTION_TYPE FUNCTION_NAME _SOURCES EXTERNAL LINK )
	set( SOURCES ${_SOURCES} )
	set( FUNCTION Function${FUNCTION_TYPE}${FUNCTION_NAME} )
	add_library( ${FUNCTION} SHARED ${SOURCES} ${FUNCTIONS_ROOT}/FunctionLib.hpp )
	foreach( FILE ${EXTERNAL} )
		if( ${FILE} STREQUAL "MemoryAllocator" )
			target_link_libraries( ${FUNCTION} PRIVATE NanoLib-nMemoryAllocator )
		elseif( ${FILE} STREQUAL "NumberConvertor" )
			target_link_libraries( ${FUNCTION} PRIVATE NanoLib-NumberConvertor )
		endif()
	endforeach( FILE )

	foreach( FILE ${LINK} )
		if( ${FILE} STREQUAL "Gmp" )
			find_package( GMP REQUIRED )
			target_include_directories( ${FUNCTION} PRIVATE ${GMP_INCLUDE_DIRS} )
			target_link_libraries( ${FUNCTION} PRIVATE ${GMP_LIBRARIES} )
		endif()
	endforeach( FILE )

	target_include_directories( ${FUNCTION} PRIVATE ${ICU_INCLUDE_DIRS} )

	if( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
		target_link_libraries( ${FUNCTION} PRIVATE "-undefined dynamic_lookup -flat_namespace" )
	else()
		target_link_libraries( ${FUNCTION} PRIVATE "-Wl,--no-whole-archive" )
	endif()

	list( APPEND DL_FUNCTIONS ${FUNCTION} )
	set( DL_FUNCTIONS ${DL_FUNCTIONS} PARENT_SCOPE )

	add_test( NAME ${FUNCTION} COMMAND $<TARGET_FILE:FuncMon> --check $<TARGET_FILE:${FUNCTION}> )

endmacro( add_external_function )

add_subdirectory( Common )
add_subdirectory( Dates )
add_subdirectory( Math )

add_custom_target( ${PROJECT_NAME} ALL DEPENDS ${DL_FUNCTIONS} )
