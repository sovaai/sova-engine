cmake_minimum_required( VERSION 3.7 )

# PHRASES_TABLE

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_deu.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray ARGS "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_abbr_deu.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrDe.utf" tmp/phrases_table_abbr_deu.cpp de stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrDe.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_eng.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray ARGS "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_abbr_eng.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrEn.utf" tmp/phrases_table_abbr_eng.cpp en stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrEn.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_esp.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_abbr_esp.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrEs.utf" tmp/phrases_table_abbr_esp.cpp es stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrEs.utf
  WORKING_DIRECTORY "${BUILD}" )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_vie.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_abbr_vie.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrVi.utf" tmp/phrases_table_abbr_vie.cpp vi stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrVi.utf
  WORKING_DIRECTORY "${BUILD}" )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_fra.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_abbr_fra.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrFr.utf" tmp/phrases_table_abbr_fra.cpp fr stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrFr.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_rus.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_abbr_rus.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrRu.utf" tmp/phrases_table_abbr_rus.cpp ru stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrAbbrRu.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_compend.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray -d "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_compend.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrCompend.utf" tmp/phrases_table_compend.cpp er stop )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_deu.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_stop_deu.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordDe.utf" tmp/phrases_table_stop_deu.cpp de stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrCompend.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_eng.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_stop_eng.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordEn.utf" tmp/phrases_table_stop_eng.cpp en stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordEn.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_esp.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_stop_esp.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordEs.utf" tmp/phrases_table_stop_esp.cpp es stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordEs.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_vie.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_stop_vie.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordVi.utf" tmp/phrases_table_stop_vie.cpp vi stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordVi.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_fra.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_stop_fra.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordFr.utf" tmp/phrases_table_stop_fra.cpp fr stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordFr.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_rus.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_phrases_stop_rus.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordRu.utf" tmp/phrases_table_stop_rus.cpp ru stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/PhraseBreaker/PhraseBrStopwordRu.utf )

project( phrases_tables )
add_library( ${PROJECT_NAME} STATIC
	phrases_tables.c
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_deu.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_eng.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_esp.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_vie.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_fra.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_abbr_rus.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_compend.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_deu.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_eng.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_esp.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_vie.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_fra.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/phrases_table_stop_rus.cpp
	)
target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/tmp )

# MORPH_TABLES

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_cyr.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_suff_cyr.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/CyrList.utf" tmp/table_suff_cyr.cpp cy suff
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/CyrList.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_deu.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_suff_deu.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DeuList.utf" tmp/table_suff_deu.cpp de suff
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DeuList.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_eng.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_suff_eng.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/EngList.utf" tmp/table_suff_eng.cpp en suff
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/EngList.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_esp.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_suff_esp.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/EspList.utf" tmp/table_suff_esp.cpp es suff
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/EspList.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_eur.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_suff_eur.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/EurList.utf" tmp/table_suff_eur.cpp eu suff
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/EurList.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_fra.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_suff_fra.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/FraList.utf" tmp/table_suff_fra.cpp fr suff
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/FraList.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_rus.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_suff_rus.cpp" -i:"${CMAKE_CURRENT_SOURCE_DIR}/source/FuzzyRusNovoteka.ini" tmp/table_suff_rus.cpp ru suff
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/FuzzyRusNovoteka.ini )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_cyr.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_stop_cyr.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/CyrStop.utf" tmp/table_stop_cyr.cpp cy stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/CyrStop.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_deu.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_stop_deu.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DeuStop.utf" tmp/table_stop_deu.cpp de stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DeuStop.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_eng.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_stop_eng.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/EngStop.utf" tmp/table_stop_eng.cpp en stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/EngStop.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_esp.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_stop_esp.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/EspStop.utf" tmp/table_stop_esp.cpp es stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/EspStop.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_eur.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_stop_eur.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/EurStop.utf" tmp/table_stop_eur.cpp eu stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/EurStop.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_fra.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_stop_fra.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/FraStop.utf" tmp/table_stop_fra.cpp fr stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/FraStop.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_rus.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_stop_rus.cpp" -i:"${CMAKE_CURRENT_SOURCE_DIR}/source/FuzzyRusNovoteka.ini" tmp/table_stop_rus.cpp ru stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/FuzzyRusNovoteka.ini )

project( morph_tables )
add_library( ${PROJECT_NAME} STATIC
	morph_tables.c
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_cyr.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_deu.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_eng.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_esp.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_eur.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_fra.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_suff_rus.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_cyr.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_deu.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_eng.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_esp.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_eur.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_fra.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/table_stop_rus.cpp
	)
target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/tmp )

# DETECT_TABLES

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_deu.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_detect_deu.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DetectDe.utf" tmp/detect_table_deu.cpp de stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DetectDe.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_eng.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_detect_eng.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DetectEn.utf" tmp/detect_table_eng.cpp en stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DetectEn.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_esp.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_detect_esp.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DetectEs.utf" tmp/detect_table_esp.cpp es stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DetectEs.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_eur.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_detect_eur.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DetectEu.utf" tmp/detect_table_eur.cpp en stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DetectEu.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_fra.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_detect_fra.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DetectFr.utf" tmp/detect_table_fra.cpp fr stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DetectFr.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_rus.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_detect_rus.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DetectRu.utf" tmp/detect_table_rus.cpp ru stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DetectRu.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_ukr.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_detect_ukr.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DetectUa.utf" tmp/detect_table_ukr.cpp ua stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DetectUa.utf )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_vie.cpp
  COMMAND mkdir -p tmp
  COMMAND ${CMAKE_BINARY_DIR}/bin/UMakeCArray "${CMAKE_CURRENT_SOURCE_DIR}/_tmpl_detect_vie.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/DetectVi.utf" tmp/detect_table_vie.cpp vi stop
  DEPENDS UMakeCArray
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/DetectVi.utf )

project( detect_tables )
add_library( ${PROJECT_NAME} STATIC
	detect_tables.c
	${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_deu.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_eng.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_esp.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_eur.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_fra.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_rus.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_ukr.cpp
	${CMAKE_CURRENT_BINARY_DIR}/tmp/detect_table_vie.cpp
	)
target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/tmp )
