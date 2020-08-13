cmake_minimum_required( VERSION 3.7 )

if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
  cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
cmake_policy( SET CMP0054 NEW )
find_package( ICU COMPONENTS UC DATA REQUIRED )

project( UMakeCArray )
add_executable( ${PROJECT_NAME} UMakeCArray.cpp )

target_link_libraries( ${PROJECT_NAME} ${CMAKE_DL_LIBS} ${ICU_UC_LIBRARIES} ${ICU_DATA_LIBRARIES} zutils lpucharset utf_canonizer )
target_include_directories( ${PROJECT_NAME} PUBLIC ${ICU_INCLUDE_DIRS} )
