cmake_minimum_required( VERSION 3.7 )

project( terms )
add_library( ${PROJECT_NAME} STATIC
	FltTermsArray.cpp
	FltTermsBase.cpp
	FltTermsBaseRO.cpp
	FltTermsBaseLP.cpp
	FltTermsBaseWR.cpp
	FltTermsMatcher.cpp
	FltTermsStop.cpp
	FltTermsWFuncs.cpp
	terms.cpp )

target_link_libraries( ${PROJECT_NAME} PUBLIC
	stopdict_ex
	clingproc
	lingproc4 )
target_include_directories( terms PUBLIC ${ICU_INCLUDE_DIRS} )
