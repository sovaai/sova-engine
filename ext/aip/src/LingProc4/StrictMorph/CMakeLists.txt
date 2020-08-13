cmake_minimum_required( VERSION 3.7 )

if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
  cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
cmake_policy( SET CMP0054 NEW )
find_package( ICU COMPONENTS UC REQUIRED )

project( strict_morph )
add_library( ${PROJECT_NAME} STATIC
    LexUnit.cpp
    StrictMorph.cpp
    StrictMorphStatus.cpp
    StrictMorphFS.cpp
    StrictMorphShadow.cpp
    StrictMorphShadowFS.cpp
    StrictMorphShadowFSRW.cpp
    StrictMorphSynthesis.cpp
    PriorityQueue.cpp
    DecompositionProcessor.cpp
    DictionnaryInfo.cpp
    DictionnaryInfoRW.cpp
    DictionnaryInfoFSRW.cpp
    StrictMorphHeader.cpp
    StrictMorphHeaderRW.cpp
    StrictMorphHeaderFSRW.cpp
    Pack32.cpp
    Pack64.cpp
    StrictMorphGram.cpp
    LexGram.cpp
    LexGramRW.cpp
    FormGram.cpp
    FormGramRW.cpp )
target_link_libraries( ${PROJECT_NAME} PUBLIC lpucharset pts2 word_normalizer )
