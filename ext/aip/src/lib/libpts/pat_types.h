/*******************************************************************************
 * Proj: Content Filtration Library                                            *
 * --------------------------------------------------------------------------- *
 * File: pat_types.h                                                           *
 * Created: Wed Dec 10 17:21:02 2003                                           *
 * Desc: Common types for pat_tree and pat_tree_storage.                       *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  pat_types.h
 * \brief Common types for pat_tree and pat_tree_storage.
 */

#ifndef __pat_types_h__
#define __pat_types_h__

#ifdef __cplusplus
extern "C" 
{
#endif

    /* All char sequence have to be ended via PAT_CHAR_FINISH */
#define PAT_CHAR_FINISH 0x00
    typedef unsigned char pat_char;

#define PAT_ID_NONE 0x00000000U
    typedef unsigned int  pat_id;

#define PAT_LOOKUP_EXACT_MATCH 0
#define PAT_LOOKUP_BEST_MATCH  1

#define PTS_SAVE_DATA_FORMAT      0x00000001U
#define PTS_SAVE_TRANSPORT_FORMAT 0x00000002U


#ifdef __cplusplus
}
#endif

#endif /* __pat_types_h__ */

/*
 * <eof pat_types.h>
 */
