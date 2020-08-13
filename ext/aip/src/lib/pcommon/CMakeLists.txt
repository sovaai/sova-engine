cmake_minimum_required( VERSION 3.7 )

project( pcommon )
add_library( ${PROJECT_NAME} STATIC
    convert.cpp
    convert2UTF.cpp
    Dos2Uni.cpp
    Iso2Uni.cpp
    Koi2Uni.cpp
    Lat12Uni.cpp
    Lat12Uni_cyr.cpp
    Mac2Uni.cpp
    Win2Uni.cpp
    Viet2Uni.cpp
    EucJp2Uni.cpp
    Greek2Uni.cpp
    Arab2Uni.cpp
    koi8_ctype.c
    strptime.c
    string.c )
target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../.. ${CMAKE_CURRENT_LIST_DIR}/../../_include )
