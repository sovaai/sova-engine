#ifndef _LEXID32_H_
#define _LEXID32_H_

/* Check the lingproc compilation mode for errors */
#ifdef FORCE_LEXID64
# error You should not include LexID32.h when FORCE_LEXID64 is defined
#endif

/* Force define */
#ifndef FORCE_LEXID32
# define FORCE_LEXID32
#endif

#ifdef _MSC_VER
# pragma message("LEXID32 used")
#endif

#include <sys/types.h>
#include <_include/_inttype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma warning(disable:4514) /* unreferenced inline/local function has been removed */
#endif

    typedef uint32_t LEXID;

#define LEXNONE    0
#define LEXINVALID 0xFFFFFFFF

    /* LEXID logical data structure:
     *
     * morphology number:    4  bits (max 15 morphs + hier)
     * capitalization sheme: 2  bits (lower, upper, title and 1 reserved)
     * form number:          8  bits (max 256 forms)
     * lexeme id:            18 bits (max 262.143 lexemes)
     */

    /* Hier LEXID logical data structure:
     *
     * morphology number:    4  bits (max 15 morphs + hier)
     * capitalization sheme: 2  bits (lower, upper, title and 1 reserved)
     * lexeme id:            26 bits (max 67.108.863 lexemes, actually half)
     */

#define LEX_CTRL_MASK    0xFF000000
#define LEX_CTRL_HOMO    0xF0000000
#define LEX_CTRL_MARKUP  0xF1000000

/*! \note The structure of LEXID */
#define LEX_MORPH     0xF0000000
#define LEX_CAPS      0x0C000000
#define LEX_IDNORM    0x0003FFFF
#define LEX_FORM      (~(LEX_MORPH|LEX_CAPS|LEX_IDNORM))
#define LEX_IDHIER    (LEX_FORM|LEX_IDNORM)
#define LEX_UNKNOWN   LEX_IDHIER
#define LEX_NOHOMO    0x000003FF

#define CAP_NOCASE    0x00000000
#define CAP_LOWERCASE 0x04000000
#define CAP_UPPERCASE 0x08000000
#define CAP_TITLECASE (CAP_LOWERCASE|CAP_UPPERCASE)

#define PUNCTSPACE_LEFT  CAP_UPPERCASE
#define PUNCTSPACE_RIGHT CAP_LOWERCASE
#define PUNCTSPACE_BOTH  CAP_TITLECASE
#define PUNCTSPACE_NONE  CAP_NOCASE

/*! \note Reserved Morphology numbers */
#define MORPHNO_HIER  0x0
#define MORPHNO_PUNCT 0x1
#define MORPHNO_NUM   0x2
#define MORPHNO_CTRL  0xF

/*************** FUNCTION PROTOTYPES ***************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#define tmp_LexNoMrph(lex)           ((unsigned short)(((lex)&LEX_MORPH)>>28))
#define tmp_LexIdNorm(lex)           ((lex)&LEX_IDNORM)
#define tmp_LexIdForm(lex)           ((unsigned char)(LexNoMrph(lex) > 2 ? (unsigned char)(((lex)&LEX_FORM)>>18) : 0))
#define tmp_LexIdHier(lex)           ((lex)&LEX_IDHIER)
#define tmp_LexId(lex)               ((LexNoMrph(lex)>2) ? ((lex)&LEX_IDNORM) : ((lex)&LEX_IDHIER))

#define tmp_LexIsNum(lex)            (((lex)&LEX_MORPH)==(MORPHNO_NUM<<28))
#define tmp_LexIsPunct(lex)          (((lex)&LEX_MORPH)==(MORPHNO_PUNCT<<28))
#define tmp_LexIsHier(lex)           (((lex)&LEX_MORPH)==(MORPHNO_HIER<<28))
#define tmp_LexNone(lex)             (LexIsNum(lex) ? 0 : LexIdHier(lex) == LEXNONE)

#define tmp_LexIsUpper(lex)          (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_UPPERCASE)
#define tmp_LexIsLower(lex)          (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_LOWERCASE)
#define tmp_LexIsTitle(lex)          (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_TITLECASE)
#define tmp_LexIsNoCase(lex)         (LexIsPunct(lex) ? 1 : ((lex)&LEX_CAPS)==CAP_NOCASE)

#define tmp_LexIsCtrl(lex)           ((lex) != LEXINVALID && LEXID((lex)&LEX_MORPH)==LEXID(MORPHNO_CTRL<<28))
#define tmp_LexIsCtrlHomo(lex)       (((lex)&LEX_CTRL_MASK)==LEX_CTRL_HOMO)
#define tmp_LexIsCtrlMarkup(lex)     (((lex)&LEX_CTRL_MASK)==LEX_CTRL_MARKUP)

#define tmp_LexNoHomo(lex)           (LexIsCtrlHomo(lex) ? ((unsigned short)((lex)&LEX_NOHOMO)) : 0)

#define tmp_LexMarkupTag(lex)        (LexIsCtrlMarkup(lex) ? (((unsigned short)lex)&0x7FFF) : 0)
#define tmp_LexIsMarkupOpening(lex)  (LexIsCtrlMarkup(lex) ? ((((unsigned short)(lex))&0x8000)==0) : 0)
#define tmp_LexIsMarkupClosing(lex)  (LexIsCtrlMarkup(lex) ? ((((unsigned short)(lex))&0x8000)!=0) : 0)

#define tmp_LexPunctSpace(lex)       (LexIsPunct(lex) ? ((unsigned char)(((lex)&LEX_CAPS)>>26)): 0)
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
inline unsigned char LexIdForm( LEXID lex )  { return tmp_LexIdForm(lex); }
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

inline unsigned int LexMarkupTag( LEXID lex ) { return tmp_LexMarkupTag(lex); }
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
    return lex | (LEXID(no&0xF)<<28);
}

inline LEXID LexSetForm( LEXID lex, unsigned int f )
{
    if (LexNoMrph(lex)<=2)
        return lex;
    lex &= (~LEX_FORM);
    return lex | (LEXID(f)<<18);
}

inline void  _LexSetNoMrph(LEXID* lex, unsigned int no) { *lex |= (((LEXID)no)<<28); }
inline void  _LexSetForm(LEXID* lex, unsigned int f) { *lex |= (((LEXID)f)<<18); }
inline LEXID _LexSetDictForm( LEXID lex ) { return tmp_LexSetDictForm(lex); } /* Low level routine */

inline LEXID    LexNormalForm( LEXID lex )      { return tmp_LexNormalForm(lex); }
inline LEXID    LexNormalize( LEXID lex )       { return tmp_LexNormalize(lex); }

inline int32_t  LexNumGetNumber(LEXID lex) { return LexIsNum(lex) ? (lex&0x0fffffff) : 0; }
inline int      LexPunctGetPunct(LEXID lex) { return LexIsPunct(lex) ? (lex&0xff) : 0; }

inline LEXID LexSetOrthVar( LEXID lex, uint8_t /* var */ )
{
	return lex;
}

inline LEXID LexSetFormVariant( LEXID lex, uint8_t /* formVariant */ )
{
	return lex;
}

inline uint8_t LexOrthVar( LEXID /* lex */ )
{
	return 0;
}

inline uint8_t LexFormVariant( LEXID /* lex */ )
{
        return 0;
}

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

#endif /* _LEXID32_H_ */

