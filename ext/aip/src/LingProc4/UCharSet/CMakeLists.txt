cmake_minimum_required( VERSION 3.7 )

if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
  cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
cmake_policy( SET CMP0054 NEW )
find_package( ICU COMPONENTS UC REQUIRED )

project( lpucharset )
file( GLOB SOURCES *.h )
add_library( ${PROJECT_NAME} ${SOURCES} UCharSet.cpp UCharSetFS.cpp )
target_include_directories( ${PROJECT_NAME} PUBLIC ${ICU_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/../.. )
