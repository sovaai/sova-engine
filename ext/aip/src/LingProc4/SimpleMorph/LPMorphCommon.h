/**
 * @file   LPMorphCommon.h
 * @author swizard <me@swizard.info>
 * @date   Fri Apr 11 18:54:21 2008
 * 
 * @brief  Common defines for LingProc morphologies interface.
 * 
 * 
 */
#ifndef _LPMORPHCOMMON_H_
#define _LPMORPHCOMMON_H_

#include <string.h>
#include <sys/types.h>
#include <_include/_inttype.h>
#include <_include/language.h>

/* creation flags */
#define MORPH_FLAG_CASE_INSENSITIVE    0x00000001U
#define MORPH_FLAG_LWR_ACCENT          0x00000002U
#define MORPH_FLAG_LWR_PUNCT           0x00000004U
#define MORPH_FLAG_REM_IGNORE          0x00000008U
#define MORPH_FLAG_NATIVE_ONLY         0x00000010U

#define MORPH_FLAG_PROCESS_COMPOUNDS   0x00000020U
#define MORPH_FLAG_EXPAND_LIGATURES    0x00000040U

#define MORPH_FLAG_HIER_NATIVE_ONLY    0x00000080U

#define MORPH_FLAG_ARABIC_SEQ_COLLAPSE 0x00000100U

#define MORPH_FLAG_INVALID             0x40000000U
#define MORPH_FLAG_USE_DEFAULT         0x80000000U

/* runtime flags */
#define MORPH_FLAG_NO_APPEND           0x01000000U
#define MORPH_FLAG_PROCESS_TILDE       0x02000000U
#define MORPH_FLAG_FORCE_UNKNOWN       0x04000000U
#define MORPH_FLAG_NO_DICT_INFO        0x08000000U

/* Morhology types */
enum MorphTypes
{
    MORPH_TYPE_INVALID = 0,
    MORPH_TYPE_HIER,
    MORPH_TYPE_FUZZY,
    MORPH_TYPE_STRICT,
    MORPH_TYPE_PUNCT,
    MORPH_TYPE_NUM,

    MORPH_TYPE_ALL, /**< That is: both MORPH_TYPE_FUZZY and MORPH_TYPE_STRICT */

    MORPH_TYPE_TILDE,
    MORPH_TYPE_TILDE_REV,
    MORPH_TYPE_TILDE_BOTH,
        
    MORPH_TYPE_DEFAULT = 0 /**< Used in LP create and lemmatization params */
};

class MorphTypeInfo
{
public:
    static const char *ShortName( MorphTypes type )
    {
        switch ( type )
        {
        case MORPH_TYPE_HIER:
            return "h";
        case MORPH_TYPE_FUZZY:
            return "f";
        case MORPH_TYPE_TILDE:
            return "t";
        case MORPH_TYPE_TILDE_REV:
            return "r";
        case MORPH_TYPE_STRICT:
            return "s";
        case MORPH_TYPE_PUNCT:
            return "p";
        case MORPH_TYPE_NUM:
            return "n";
        default:
            return "x";
        }
    }

    static const char *FullName( MorphTypes type )
    {
        switch ( type )
        {
        case MORPH_TYPE_HIER:
            return "hier";
        case MORPH_TYPE_FUZZY:
            return "fuzzy";
        case MORPH_TYPE_TILDE:
            return "tilde";
        case MORPH_TYPE_TILDE_REV:
            return "tilde_rev";
        case MORPH_TYPE_STRICT:
            return "strict";
        case MORPH_TYPE_PUNCT:
            return "punct";
        case MORPH_TYPE_NUM:
            return "number";
        default:
            return "unknown";
        }
    }
};

/* A morphology dictionary information */
struct LPMorphDictInfo
{
    MorphTypes   type;           /**< morphology type */
    unsigned int version;        /**< dictionary version */
    unsigned int variant;        /**< dictionary variant */

    LangCodes    lang;           /**< language */
    unsigned int globalId;       /**< dictionary ID (globally unique) */

    unsigned int internalNumber; /**< internal number, assigned by LP */
    size_t       minStemLength;  /**< minimum stem length */
    size_t       minCompoundStemLength; /**< minimum stem length for a compound part */
    unsigned int creationFlags;  /**< creation flags */

    LPMorphDictInfo() { memset( this, 0, sizeof(LPMorphDictInfo) ); }
};

/* A morphology creation parameters */
struct LPMorphCreateParams
{
    MorphTypes   type;            /**< morph type */
    LangCodes    lang;            /**< language */

    unsigned int internalNumber;  /**< internal number, assigned by LP */
    size_t       maxWordLength;   /**< maximum length of a word that can be lemmatized;
                                     may be adjusted by morph engine; 0 - use default */
    unsigned int flags;           /**< creation flags; may be adjusted by morph engine;
                                     MORPH_FLAG_USE_DEFAULT - use default */
    unsigned int minStemLength; /**< use this value for minimum stem length or leave zero for default */
    unsigned int minCompoundStemLength; /**< use this value for minimum compound part length or leave zero for default */
};

#endif /* _LPMORPHCOMMON_H_ */

