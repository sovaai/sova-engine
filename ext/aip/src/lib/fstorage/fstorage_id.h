/*******************************************************************************
 * Proj: Content Filtration Library                                            *
 * --------------------------------------------------------------------------- *
 * File: fstorage_id.h                                                         *
 * Created: Fri Feb 13 18:57:04 2004                                           *
 * Desc: Identificators for fstorage                                           *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  fstorage_id.h
 * \brief Identificators for fstorage
 */

#ifndef __fstorage_id_h__
#define __fstorage_id_h__

/* Test application */

#define FSTORAGE_PID_TEST (1U)

#define FSTORAGE_TEST_SECTION_ONE   (1U)
#define FSTORAGE_TEST_SECTION_TWO   (2U)
#define FSTORAGE_TEST_SECTION_THREE (3U)

/* 
 * Content Filtration Library
 */

#define FSTORAGE_PID_CFLIB        (2U)
#define FSTORAGE_PID_STRICT_MORPH (3U)

/* 0x00400000 - 0x0040000F */
#define FSTORAGE_SECTION_SIGDB     (0x00400000U)
/* 0x00400010 - 0x0040001F */
#define FSTORAGE_SECTION_CATALOGUE (0x00400010U)
/* 0x00400020 - 0x0040002F */
#define FSTORAGE_SECTION_TERMS     (0x00400020U)
/* 0x00400030 - 0x0040003F */
#define FSTORAGE_SECTION_FILTERDB  (0x00400030U)

/* 0x00450000 - 0x004FFFFF */
#define FSTORAGE_SECTION_SIGDBPP        (0x00450000U)
#define FSTORAGE_SECTION_SIGDBPP_LAST   (0x004FFFFFU)

/* 0x005A0000 - 0x005AFFFF */
#define FSTORAGE_SECTION_LINGPROC       (0x005A0000U)
#define FSTORAGE_SECTION_LINGPROC_LAST  (0x005AFFFFU)

/* 0x005B0000 - 0x005BFFFF */
#define FSTORAGE_SECTION_GSG  	   (0x005B0000U)
#define FSTORAGE_SECTION_GSG_UPDATE_BLACK (0x005B0001U)
#define FSTORAGE_SECTION_GSG_UPDATE_WHITE (0x005B0002U)

#define FSTORAGE_SECTION_CONTROL   (0x00600000U)

/* 
 * Autoref2
 * 0x00700000 - ... 
 */

#define FSTORAGE_PID_AUTOREF      (4U)

#define FSTORAGE_SECTION_AUTOREF_DICTS (0x00700000U)


#endif /* __fstorage_id_h__ */

/*
 * <eof fstorage_id.h>
 */
