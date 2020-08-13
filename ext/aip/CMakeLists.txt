cmake_minimum_required( VERSION 3.7 )
if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
	cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
find_package( ICU COMPONENTS uc data REQUIRED )

add_definitions(
	"-DNO_PRINT_VERSION"
	"-DPTS2_CROSS_HASHTABLE_SIZE=64"
	"-DPTS2_CROSS_HASHTABLE_INC=32"

	"-DUSE_INF_TERMS"
	"-DLOG_DATA_FLOW_ENABLED"
	"-DINF_ENGINE_STATISTICS"
	"-DCSDATA_EXTENDED"

	"-D_STL=std"
)

add_subdirectory( src )
add_subdirectory( headers )
