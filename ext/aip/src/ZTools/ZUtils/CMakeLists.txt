cmake_minimum_required( VERSION 3.7 )

project( zutils )
add_library( ${PROJECT_NAME} STATIC
    ScanPath.cpp
    Alpha.cpp
    CSParams.cpp
    EString.cpp
    StrUtils.cpp )
target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../.. )
