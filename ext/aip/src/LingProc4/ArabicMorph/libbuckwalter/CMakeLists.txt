cmake_minimum_required( VERSION 3.7 )

if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
  cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
cmake_policy( SET CMP0054 NEW )
find_package( ICU COMPONENTS UC DATA REQUIRED )

project( buckwalter )
add_library( ${PROJECT_NAME} STATIC
    transformation.cpp
    morphology.cpp
    utils.cpp
    grammar.cpp
    grammar_file_parser.cpp
    grammar_storage.cpp
    compatibility_storage.cpp
    compatibility_file_parser.cpp
    lexemes.cpp
    forms.cpp
    pascalstrings.cpp
    LPArabicMorph.cpp
    fstorage_utils.cpp )
target_link_libraries( ${PROJECT_NAME} PUBLIC ${ICU_UC_LIBRARIES} ${ICU_DATA_LIBRARIES} pts2 language_info fstorage )
target_include_directories( ${PROJECT_NAME} PUBLIC ${ICU_INCLUDE_DIRS} )
