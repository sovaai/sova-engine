cmake_minimum_required( VERSION 3.7 )

add_subdirectory( data )
add_subdirectory( tools )
add_subdirectory( UCharSet )
add_subdirectory( StrictMorph )
add_subdirectory( ArabicMorph/libbuckwalter )

if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
  cmake_policy( SET CMP0074 NEW )
endif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
cmake_policy( SET CMP0054 NEW )
find_package( ICU COMPONENTS uc data REQUIRED )

# UTF_CANONIZER

project( utf_canonizer )
add_library( ${PROJECT_NAME} STATIC UTFCanonizer.cpp )
target_include_directories( ${PROJECT_NAME} PUBLIC ${ICU_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/.. )

# LANGUAGE_INFO

project( language_info )
add_library( ${PROJECT_NAME} STATIC LanguageInfo.cpp )
target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/.. )

# WORD_NORMALIZER

project( word_normalizer )
add_library( ${PROJECT_NAME} STATIC
    WordNormalizer.cpp
    WordNormalizer/WordNormalizerGerman.cpp
    WordNormalizer/WordNormalizerArabic.cpp )
target_include_directories( ${PROJECT_NAME} PUBLIC ${ICU_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/.. )

# LINGPROC4

project( lingproc4 )
add_library( ${PROJECT_NAME} STATIC
    LingProcErrors.cpp
    DetectWords.cpp
    LingProcDict.cpp
    LingProc.cpp
    LingProcData.cpp
    LPStrict.cpp
    DocText/DocText.cpp
    DocImage/DocImage.cpp
    RusLatConv/RusLatConv.cpp
    RusLatConv/RusLatConvFS.cpp
    PhraseBreak/PhraseBreakerDict.cpp
    PhraseBreak/LPPhraseBreak.cpp
    Process/Process.cpp
    Process/WordBreaker.cpp
    Process/ParagraphBreaker.cpp
    Process/LPLangDetector.cpp
    Process/LangDetector/LangDetector.cpp
    SimpleMorph/LPMorphPunct.cpp
    SimpleMorph/LPMorphNumber.cpp
    SimpleMorph/LPMorphFuzzy.cpp
    SimpleMorph/LPMorphStrict.cpp
    SimpleMorph/LPMorphTilde.cpp )
target_link_libraries( ${PROJECT_NAME} PUBLIC ${ICU_UC_LIBRARIES} ${ICU_DATA_LIBRARIES}
    detect_tables
    morph_tables
    phrases_tables
    strict_morph
    utf_canonizer
    pts
    language_info
    fstorage
    pcommon
    buckwalter
    )
target_compile_options( ${PROJECT_NAME} PRIVATE -Wno-attributes )
