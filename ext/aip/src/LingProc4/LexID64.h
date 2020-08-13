#ifndef _LEXID64_H_
#define _LEXID64_H_

/* Check the lingproc compilation mode for errors */
#ifdef FORCE_LEXID32
# error You should not include LexID64.h when FORCE_LEXID32 is defined
#endif

/* Force define */
#ifndef FORCE_LEXID64
# define FORCE_LEXID64
#endif

#ifdef _MSC_VER
# pragma message("LEXID64 used")
#endif

#include <sys/types.h>
#include <_include/_inttype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
# pragma warning(disable:4514) /* unreferenced inline/local function has been removed */
#endif

    typedef uint64_t LEXID;

#define LEXNONE 0
#define LEXINVALID  CONSTANTUI64( 0xFFFFFFFFFFFFFFFF )

    /* LEXID and Hier LEXID logical data structure:
     *
     * morphology number:             8  bits (max 255 morphs + hier)
     * scheme:
     *     - punct morph -- spaces:   2  bits (space before and space after)
     *     - other -- capitalization: 2  bits (lower, upper, title and 1 reserved)
     * complex word part:             2  bits (currently unused)
     * form number:                   10 bits (max 1024 forms)
     * orth variant:                  4  bits (max 16 orth variants)
     * form variant:                  5  bits (max 32 orth variants)
     * reserved:                      1  bit
     * lexeme id:                     32 bits (max 4.294.967.295 lexemes)
     */

#define LEX_CTRL_MASK     CONSTANTUI64( 0xFFF0000000000000 )
#define LEX_CTRL_HOMO     CONSTANTUI64( 0xFF00000000000000 )
#define LEX_CTRL_MARKUP   CONSTANTUI64( 0xFF10000000000000 )

/*! \note The structure of LEXID */
#define LEX_MORPH         CONSTANTUI64( 0xFF00000000000000 )
#define LEX_CAPS          CONSTANTUI64( 0x00C0000000000000 )
#define LEX_SPACES       (LEX_CAPS)
#define LEX_IDNORM        CONSTANTUI64( 0x00000000FFFFFFFF )
#define LEX_FORM          CONSTANTUI64( 0x000FFC0000000000 )
#define LEX_WORDPART      CONSTANTUI64( 0x0030000000000000 )
#define LEX_IDHIER       (LEX_IDNORM)
#define LEX_UNKNOWN      (LEX_IDHIER)
#define LEX_NOHOMO        CONSTANTUI64( 0x00000000000003FF )

#define CAP_NOCASE        CONSTANTUI64( 0x0000000000000000 )
#define CAP_LOWERCASE     CONSTANTUI64( 0x0040000000000000 )
#define CAP_UPPERCASE     CONSTANTUI64( 0x0080000000000000 )
#define CAP_TITLECASE    (CAP_LOWERCASE|CAP_UPPERCASE)

#define PUNCTSPACE_LEFT  CAP_UPPERCASE
#define PUNCTSPACE_RIGHT CAP_LOWERCASE
#define PUNCTSPACE_BOTH  CAP_TITLECASE
#define PUNCTSPACE_NONE  CAP_NOCASE

/*! \note Reserved Morphology numbers */
#define MORPHNO_HIER     CONSTANTUI64( 0x0 )
#define MORPHNO_PUNCT    CONSTANTUI64( 0x1 )
#define MORPHNO_NUM      CONSTANTUI64( 0x2 )
#define MORPHNO_CTRL     CONSTANTUI64( 0xFF )


/*************** FUNCTION PROTOTYPES ***************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#define tmp_LexNoMrph(lex)           ((unsigned short)(((lex)&LEX_MORPH)>>56))
#define tmp_LexIdNorm(lex)           ((lex)&LEX_IDNORM)
#define tmp_LexIdForm(lex)           ((unsigned short)(LexNoMrph(lex) > 2 ? (unsigned short)(((lex)&LEX_FORM)>>42) : 0))
#define tmp_LexIdHier(lex)           ((lex)&LEX_IDHIER)
#define tmp_LexId(lex)               ((LexNoMrph(lex)>2) ? ((lex)&LEX_IDNORM) : ((lex)&LEX_IDHIER))

#define tmp_LexIsNum(lex)            (((lex)&LEX_MORPH)==(MORPHNO_NUM<<56))
#define tmp_LexIsPunct(lex)          (((lex)&LEX_MORPH)==(MORPHNO_PUNCT<<56))
#define tmp_LexIsHier(lex)           (((lex)&LEX_MORPH)==(MORPHNO_HIER<<56))
#define tmp_LexNone(lex)             (LexIsNum(lex) ? 0 : LexIdHier(lex) == LEXNONE)

#define tmp_LexIsUpper(lex)          (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_UPPERCASE)
#define tmp_LexIsLower(lex)          (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_LOWERCASE)
#define tmp_LexIsTitle(lex)          (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_TITLECASE)
#define tmp_LexIsNoCase(lex)         (LexIsPunct(lex) ? 1 : ((lex)&LEX_CAPS)==CAP_NOCASE)

#define tmp_LexIsCtrl(lex)           ((lex) != LEXINVALID && LEXID((lex)&LEX_MORPH)==LEXID(MORPHNO_CTRL<<56))
#define tmp_LexIsCtrlHomo(lex)       (((lex)&LEX_CTRL_MASK)==LEX_CTRL_HOMO)
#define tmp_LexIsCtrlMarkup(lex)     (((lex)&LEX_CTRL_MASK)==LEX_CTRL_MARKUP)

#define tmp_LexNoHomo(lex)           (LexIsCtrlHomo(lex) ? ((unsigned short)((lex)&LEX_NOHOMO)) : 0)

#define tmp_LexMarkupTag(lex)        (LexIsCtrlMarkup(lex) ? (((unsigned short)lex) & CONSTANTUI64( 0x7FFF ) ) : 0)
#define tmp_LexIsMarkupOpening(lex)  (LexIsCtrlMarkup(lex) ? ((((unsigned short)(lex)) & CONSTANTUI64( 0x8000 ) )==0) : 0)
#define tmp_LexIsMarkupClosing(lex)  (LexIsCtrlMarkup(lex) ? ((((unsigned short)(lex)) & CONSTANTUI64( 0x8000 ) )!=0) : 0)

#define tmp_LexPunctSpace(lex)       (LexIsPunct(lex) ? ((unsigned char)(((lex)&LEX_CAPS)>>54)): 0)
#define tmp_LexPunctSpaceRight(lex)  (LexIsPunct(lex) ? (((lex)&PUNCTSPACE_RIGHT)!=0) : 0)
#define tmp_LexPunctSpaceLeft(lex)   (LexIsPunct(lex) ? (((lex)&PUNCTSPACE_LEFT)!=0)  : 0)
#define tmp_LexPunctSpaceBoth(lex)   (LexIsPunct(lex) ? (((lex)&PUNCTSPACE_BOTH)==PUNCTSPACE_BOTH) : 0)

#define tmp_LexSetUpper(lex)         ((LexIsNum(lex)||LexIsPunct(lex)) ? (lex) : ((lex)&(~LEX_CAPS))|CAP_UPPERCASE)
#define tmp_LexSetLower(lex)         ((LexIsNum(lex)||LexIsPunct(lex)) ? (lex) : ((lex)&(~LEX_CAPS))|CAP_LOWERCASE)
#define tmp_LexSetTitle(lex)         ((LexIsNum(lex)||LexIsPunct(lex)) ? (lex) : ((lex)&(~LEX_CAPS))|CAP_TITLECASE)
#define tmp_LexSetNoCase(lex)        ((lex)&(~LEX_CAPS))

#define tmp_LexNormalForm(lex)       (LexNoMrph(lex) > 2 ? ((lex)&(~LEX_FORM)) : (lex))
#define tmp_LexNormalize(lex)        (LexSetNoCase(LexNormalForm(lex)))
#define tmp_LexSetDictForm(lex)      (LexNoMrph(lex) > 2 ? ((lex)|(LEX_FORM)) : (lex))


#ifdef __cplusplus

/*! \note Lexid helpers */
inline unsigned short LexNoMrph( LEXID lex ) { return tmp_LexNoMrph(lex); }
inline LEXID LexIdNorm( LEXID lex )          { return tmp_LexIdNorm(lex); }
inline unsigned short LexIdForm( LEXID lex ) { return tmp_LexIdForm(lex); }
inline LEXID LexIdHier( LEXID lex )          { return tmp_LexIdHier(lex); }
inline LEXID LexId( LEXID lex )              { return tmp_LexId(lex); }

inline bool  LexIsNum( LEXID lex )           { return tmp_LexIsNum(lex); }
inline bool  LexIsPunct( LEXID lex )         { return tmp_LexIsPunct(lex); }
inline bool  LexIsHier( LEXID lex )          { return tmp_LexIsHier(lex); }
inline bool  LexNone( LEXID lex )            { return tmp_LexNone(lex); }

inline bool  LexIsUpper( LEXID lex )         { return tmp_LexIsUpper(lex); }
inline bool  LexIsLower( LEXID lex )         { return tmp_LexIsLower(lex); }
inline bool  LexIsTitle( LEXID lex )         { return tmp_LexIsTitle(lex); }
inline bool  LexIsNoCase( LEXID lex )        { return tmp_LexIsNoCase(lex); }

inline bool  LexIsCtrl( LEXID lex )          { return tmp_LexIsCtrl(lex); }
inline bool  LexIsCtrlHomo( LEXID lex )      { return tmp_LexIsCtrlHomo(lex); }
inline bool  LexIsCtrlMarkup( LEXID lex )    { return tmp_LexIsCtrlMarkup(lex); }

inline size_t LexNoHomo( LEXID lex ) { return tmp_LexNoHomo(lex); }

inline unsigned int LexMarkupTag( LEXID lex ) { return (unsigned int)tmp_LexMarkupTag(lex); }
inline bool  LexIsMarkupOpening( LEXID lex ) { return tmp_LexIsMarkupOpening(lex); }
inline bool  LexIsMarkupClosing( LEXID lex ) { return tmp_LexIsMarkupClosing(lex); }

inline unsigned int LexPunctSpace( LEXID lex ) { return tmp_LexPunctSpace(lex); }

inline LEXID LexSetUpper( LEXID lex )        { return tmp_LexSetUpper(lex); }
inline LEXID LexSetLower( LEXID lex )        { return tmp_LexSetLower(lex); }
inline LEXID LexSetTitle( LEXID lex )        { return tmp_LexSetTitle(lex); }
inline LEXID LexSetNoCase( LEXID lex )       { return tmp_LexSetNoCase(lex); }

inline bool  LexEq( LEXID lex1, LEXID lex2 )
{
    lex1&=(~LEX_CAPS);
    lex2&=(~LEX_CAPS);
    return (LexNoMrph(lex1)>2 && LexNoMrph(lex2)>2) ? (lex1&(~LEX_FORM))==(lex2&(~LEX_FORM)) : (lex1==lex2);
}

inline LEXID LexSetNoMrph( LEXID lex, unsigned int no )
{
    lex &= (~LEX_MORPH);
    return lex | (LEXID(no&0xFF)<<56);
}

inline LEXID LexSetForm( LEXID lex, unsigned int f )
{
    if (LexNoMrph(lex)<=2)
        return lex;
    lex &= (~LEX_FORM);
    return lex | (LEXID(f)<<42);
}

inline void  _LexSetNoMrph(LEXID* lex, unsigned int no) { *lex |= (((LEXID)no)<<56); }
inline void  _LexSetForm(LEXID* lex, unsigned int f) { *lex |= (((LEXID)f)<<42); }
inline LEXID _LexSetDictForm( LEXID lex ) { return tmp_LexSetDictForm(lex); }

inline int32_t  LexNumGetNumber(LEXID lex) { return (uint32_t)(LexIsNum(lex) ? (lex & CONSTANTUI64( 0x00000000FFFFFFFF ) ) : 0); }
inline int      LexPunctGetPunct(LEXID lex) { return (int)(LexIsPunct(lex) ? (lex & CONSTANTUI64( 0xFF ) ) : 0); }

inline LEXID LexSetOrthVar( LEXID lex, uint8_t var )
{
	lex     &= ~(CONSTANTUI64( 0x0F ) << 37);
	return lex | (((LEXID)var & CONSTANTUI64( 0x0F )) << 37);
}

inline LEXID LexSetFormVariant( LEXID lex, uint8_t formVariant )
{
	lex     &= ~(CONSTANTUI64( 0x1F ) << 32);
	return lex | (((LEXID)formVariant & CONSTANTUI64( 0x1F )) << 32);
}

inline uint8_t LexOrthVar( LEXID lex )
{
	return (uint8_t)((lex >> 37) & CONSTANTUI64( 0x0F ));
}

inline uint8_t LexFormVariant( LEXID lex )
{
	return (uint8_t)((lex >> 32) & CONSTANTUI64( 0x1F ));
}

inline LEXID LexNormalForm( LEXID lex )      { return LexSetFormVariant( tmp_LexNormalForm(lex), 0 ); }
inline LEXID LexNormalize( LEXID lex )       { return LexSetFormVariant( LexSetOrthVar( tmp_LexNormalize(lex), 0 ), 0 ); }

#else /* __cplusplus */

#define LexNoMrph(lex)          tmp_LexNoMrph(lex)
#define LexIdNorm(lex)          tmp_LexIdNorm(lex)
#define LexIdForm(lex)          tmp_LexIdForm(lex)
#define LexIdHier(lex)          tmp_LexIdHier(lex)
#define LexId(lex)              tmp_LexId(lex)

#define LexIsNum(lex)           tmp_LexIsNum(lex)
#define LexIsPunct(lex)         tmp_LexIsPunct(lex)
#define LexIsHier(lex)          tmp_LexIsHier(lex)
#define LexNone(lex)            tmp_LexNone(lex)

#define LexIsUpper(lex)         tmp_LexIsUpper(lex)
#define LexIsLower(lex)         tmp_LexIsLower(lex)
#define LexIsTitle(lex)         tmp_LexIsTitle(lex)
#define LexIsNoCase(lex)        tmp_LexIsNoCase(lex)

#define LexIsCtrl(lex)          tmp_LexIsCtrl(lex)
#define LexIsCtrlHomo(lex)      tmp_LexIsCtrlHomo(lex)
#define LexIsCtrlMarkup(lex)    tmp_LexIsCtrlMarkup(lex)

#define LexHoHomo(lex)          tmp_LexNoHomo(lex)

#define LexMarkupTag(lex)       tmp_LexMarkupTag(lex)
#define LexIsMarkupOpening(lex) tmp_LexIsMarkupOpening(lex)
#define LexIsMarkupClosing(lex) tmp_LexIsMarkupClosing(lex)

#define LexPunctSpace(lex)      tmp_LexPunctSpace(lex)
#define LexPunctSpaceRight(lex) tmp_LexPunctSpaceRight(lex)
#define LexPunctSpaceLeft(lex)  tmp_LexPunctSpaceLeft(lex)
#define LexPunctSpaceBoth(lex)  tmp_LexPunctSpaceBoth(lex)

#define LexSetUpper(lex)        tmp_LexSetUpper(lex)
#define LexSetLower(lex)        tmp_LexSetLower(lex)
#define LexSetTitle(lex)        tmp_LexSetTitle(lex)
#define LexSetNoCase(lex)       tmp_LexSetNoCase(lex)

#define LexNormalForm(lex)      tmp_LexNormalForm(lex)
#define LexNormalize(lex)       tmp_LexNormalize(lex)

#endif /* __cplusplus */

#endif /* _LEXID64_H_ */

