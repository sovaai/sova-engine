/* Copyright (C) 2001-2002 Alexey P. Ivanov, MSU, Dept. of Phys., Moscow
 *     All rights reserved.
 *
 * PROJECT: LINGUISTIC PROCESSOR
 *//*!
  \file  MarkupTypes.h
  \brief Markup tags definition
 *//*
 * COMPILER: MS VC 6.0, GCC                     TARGET: WIN32,UNIX
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 15.10.01: v 1.00 created by AlexIV
 *==========================================================================
 */
#ifndef __MarkupTypes_H
#define __MarkupTypes_H

#ifdef _MSC_VER
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*************** TYPES DEFINITION ******************************************/

/*! \enum MARKUP_TAGS
    \brief Markup tags values definition
*/
enum MARKUP_TAGS { 

    MARKUP_INVALID  = 0xFFFF,
    MARKUP_CLOSING  = 0x8000, // No more than 32767 different markups
    MARKUP_NONE     = 0x0,

    MARKUP_BODY     = 0x1, 
    MARKUP_SUBJECT  = 0x2,

    MARKUP_TITLE    = 0x3,
    MARKUP_SUBTITLE = 0x4,

    MARKUP_ABSTRACT = 0x5,  /*!< Meta-description */
    MARKUP_KEYWORDS = 0x6,  /*!< Meta-keywords */
    MARKUP_AUTHOR   = 0x7,  /*!< Meta-author */
    MARKUP_COMMENT  = 0x8,  /*!< Hidden   */
    MARKUP_LINK     = 0x9,  /*!< Pure address of the link (HREF) */
    MARKUP_ALT      = 0xA,  /*!< Alt Text */

    MARKUP_PARA     = 0xB,  /*!< Paragraph boundaries */

    MARKUP_EMPHASIS = 0xC,  /*!< Generic */
    MARKUP_BOLD     = 0xD,  /*!< Emphasis: bold */
    MARKUP_ITALIC   = 0xE,  /*!< Emphasis: italic */
    MARKUP_UNDERLINE= 0xF,  /*!< Emphasis: underline */
    MARKUP_MONOSPACED = 0x10,  /*!< Emphasis: monospaced */
    
    MARKUP_P	    = 0x11, /*!< Tag P for HTML markup */

    MARKUP_STRIKETHROUGH = 0x15, /*!<  <del>...</del>, <s>...</s>*/

    MARKUP_TABLE    = 0x20, 
    MARKUP_TR       = 0x21, /*!< Table Row  */
    MARKUP_TD       = 0x22, /*!< Table Data */

    MARKUP_LIST     = 0x30,
    MARKUP_LI       = 0x31, /*!< List Item: M_Aux may contain order number */

    MARKUP_META     = 0x40, /*!< Generic Metainformation */

    MARKUP_INPUT    = 0x50, /*!< Generic input form field */
    MARKUP_FILTERIN = 0x51, /*!< <INDEX>..</INDEX> tag */
    MARKUP_FILTEROUT= 0x52, /*!< <NOINDEX>..</NOINDEX> tag */

    MARKUP_DIV      = 0x60,
    
    MARKUP_BASE_LINK = 0x70, /*!< <base href="">  */

    MARKUP_HEADING1 = 0x101,
    MARKUP_HEADING2 = 0x102,
    MARKUP_HEADING3 = 0x103,
    MARKUP_HEADING4 = 0x104,
    MARKUP_HEADING5 = 0x105,
    MARKUP_HEADING6 = 0x106,
    MARKUP_HEADING7 = 0x107,
    MARKUP_HEADING8 = 0x108,
    MARKUP_HEADING9 = 0x109,

    //Document properties
        MARKUP_DOCUMENTPROPERTIES = 0x200,
    //Summary information tags
	MARKUP_SUMMARYINFORMATION = 0x201,
/* Duplicated in general markup tags
 	MARKUP_TITLE			= 0x202,
	MARKUP_SUBJECT			= 0x203,
	MARKUP_AUTHOR			= 0x204,
	MARKUP_KEYWORDS			= 0x205, 
*/
	MARKUP_COMMENTS			= 0x206,
	MARKUP_TEMPLATE			= 0x207,
	MARKUP_LASTSAVEDBY		= 0x208,
	MARKUP_REVISIONNUMBER	= 0x209,
	MARKUP_TOTALEDITINGTIME	= 0x20A,
	MARKUP_LASTPRINTED		= 0x20B,
	MARKUP_CREATETIMEDATE	= 0x20C,
	MARKUP_LASTSAVEDTIMEDATE= 0x20D,
	MARKUP_NUMBEROFPAGES	= 0x20E,
	MARKUP_NUMBEROFWORDS	= 0x20F,
	MARKUP_NUMBEROFCHARS	= 0x210,
	MARKUP_THUMBNAIL		= 0x211,
	MARKUP_NAMEOFCREATINGAPPLICATION = 0x212,
	MARKUP_SECURITY			= 0x213,

//Document summary information tags
	MARKUP_DOCUMENTSUMMARYINFO = 0x301,
	MARKUP_CATEGORY			= 0x302,
	MARKUP_PRESENTATIONTARGET  = 0x303,
	MARKUP_BYTES			= 0x304,
	MARKUP_LINES			= 0x305,
	MARKUP_PARAGRAPH		= 0x306,
	MARKUP_SLIDES			= 0x307,
	MARKUP_NOTES			= 0x308,
	MARKUP_HIDDENSLIDES		= 0x309,
	MARKUP_MMCLIPS			= 0x30A,
	MARKUP_SCALECROP		= 0x30B,
	MARKUP_HEADINGPAIRS		= 0x30C,
	MARKUP_TITLESOFPARTS	= 0x30D,
	MARKUP_MANAGER			= 0x30E,
	MARKUP_COMPANY			= 0x30F,
	MARKUP_LINKSUPTODATE	= 0x310, 

//OOXML-specific
        MARKUP_FOOTNOTE			= 0x321, // single item within MARKUP_NOTES
        MARKUP_FOOTNOTEREF			= 0x322, // reference to a footnote within document
        MARKUP_ENDNOTE			= 0x323, // single item within MARKUP_NOTES
        MARKUP_ENDNOTEREF			= 0x324, // reference to a endnote within document
        MARKUP_HEADER			= 0x325,
        MARKUP_FOOTER			= 0x326,

//Facebook-specific
	MARKUP_FB_ID		= 0x341,
	MARKUP_FB_USERID	= 0x342,
	MARKUP_FB_USERNAME	= 0x343,
	MARKUP_FB_TAG		= 0x344,

        MARKUP_LINKED = 0x401, /* HTML <a> ... </a> (anchor) content */

    MARKUP_LINK_IMG   = 0x450, /*!< links to images from IMG tag */


        MARKUP_STYLE                    = 0x461,
        MARKUP_SCRIPT                   = 0x462,
        
        /* new set of tags with no particular properties */
        // /*
        MARKUP_FRAMESET = 0x463,
        MARKUP_NOFRAMES = 0x464,
        MARKUP_CAPTION = 0x465,
        MARKUP_NOSCRIPT = 0x466,
        MARKUP_HR = 0x467,
        MARKUP_ACRONYM = 0x468,
        MARKUP_OBJECT = 0x469,
        MARKUP_APPLET = 0x46A,
        MARKUP_CENTER = 0x46B,
        MARKUP_BR = 0x46C,
        MARKUP_BLOCKQUOTE = 0x46D,
        MARKUP_FIELDSET = 0x46E,
        MARKUP_LABEL = 0x46F,
        MARKUP_SPAN = 0x470,
        MARKUP_SUB = 0x471,
        MARKUP_SUP = 0x472,
        MARKUP_NOBR = 0x473,
        MARKUP_BIG = 0x474,
        MARKUP_SMALL = 0x475,
        MARKUP_CITE = 0x476,
        MARKUP_Q = 0x477,
        MARKUP_CODE = 0x478,
        MARKUP_DFN = 0x479,
        MARKUP_KBD = 0x47A,
        MARKUP_SAMP = 0x47B,
        MARKUP_VAR = 0x47C,
        MARKUP_BLINK = 0x47D,
        MARKUP_MAP = 0x47E,
        MARKUP_NOEMBED = 0x47F,
        MARKUP_NOLAYER = 0x480,
        MARKUP_ADDRESS = 0x481,
        MARKUP_DIR = 0x482,
        MARKUP_MENU = 0x483,
        MARKUP_BDO = 0x484,
        MARKUP_BGSOUND = 0x485,
        MARKUP_SOUND = 0x486,
        MARKUP_ILAYER = 0x487,
        MARKUP_LAYER = 0x488,
        MARKUP_MARQUEE = 0x489,
        MARKUP_SPACER = 0x48A,
        MARKUP_MULTICOL = 0x48B,
        MARKUP_ABBR = 0x48C,
        MARKUP_COL = 0x48D,
        MARKUP_COLGROUP = 0x48E,
        MARKUP_EMBED = 0x48F,
        MARKUP_INS = 0x490,
        MARKUP_LEGEND = 0x491,
        MARKUP_OPTGROUP = 0x492,
        MARKUP_XMP = 0x493,
        
        MARKUP_THEAD = 0x494,
        MARKUP_TBODY = 0x495,
        MARKUP_TFOOT = 0x496,
        MARKUP_FONT = 0x497,
        MARKUP_BASEFONT = 0x498,
        // */

    /* Markup for zones with different relevance 
     * Continous block of markup identifiers, see 
     * MARKUP_RELEVANCE_BEGIN and MARKUP_RELEVANCE_END.
     */

    MARKUP_RELEVANCE_1=0x500,
    MARKUP_RELEVANCE_2=0x501,
    MARKUP_RELEVANCE_3=0x502,
    MARKUP_RELEVANCE_4=0x503,
    MARKUP_RELEVANCE_5=0x504,
    MARKUP_RELEVANCE_6=0x505,
    MARKUP_RELEVANCE_7=0x506,
    MARKUP_RELEVANCE_8=0x507,
    MARKUP_RELEVANCE_9=0x508,
    MARKUP_RELEVANCE_10=0x509,
    MARKUP_RELEVANCE_11=0x50A,
    MARKUP_RELEVANCE_12=0x50B,
    MARKUP_RELEVANCE_13=0x50C,
    MARKUP_RELEVANCE_14=0x50D,
    MARKUP_RELEVANCE_15=0x50E,
    MARKUP_RELEVANCE_16=0x50F,
    MARKUP_RELEVANCE_17=0x510,
    MARKUP_RELEVANCE_18=0x511,
    MARKUP_RELEVANCE_19=0x512,
    MARKUP_RELEVANCE_20=0x513,
    MARKUP_RELEVANCE_21=0x514,
    MARKUP_RELEVANCE_22=0x515,
    MARKUP_RELEVANCE_23=0x516,
    MARKUP_RELEVANCE_24=0x517,
    MARKUP_RELEVANCE_25=0x518,
    MARKUP_RELEVANCE_26=0x519,
    MARKUP_RELEVANCE_27=0x51A,
    MARKUP_RELEVANCE_28=0x51B,
    MARKUP_RELEVANCE_29=0x51C,
    MARKUP_RELEVANCE_30=0x51D,
    MARKUP_RELEVANCE_31=0x51E,
    MARKUP_RELEVANCE_32=0x51F,
    MARKUP_RELEVANCE_33=0x520,
    MARKUP_RELEVANCE_34=0x521,
    MARKUP_RELEVANCE_35=0x522,
    MARKUP_RELEVANCE_36=0x523,
    MARKUP_RELEVANCE_37=0x524,
    MARKUP_RELEVANCE_38=0x525,
    MARKUP_RELEVANCE_39=0x526,
    MARKUP_RELEVANCE_40=0x527,
    MARKUP_RELEVANCE_41=0x528,
    MARKUP_RELEVANCE_42=0x529,
    MARKUP_RELEVANCE_43=0x52A,
    MARKUP_RELEVANCE_44=0x52B,
    MARKUP_RELEVANCE_45=0x52C,
    MARKUP_RELEVANCE_46=0x52D,
    MARKUP_RELEVANCE_47=0x52E,
    MARKUP_RELEVANCE_48=0x52F,
    MARKUP_RELEVANCE_49=0x530,
    MARKUP_RELEVANCE_50=0x531,
    MARKUP_RELEVANCE_51=0x532,
    MARKUP_RELEVANCE_52=0x533,
    MARKUP_RELEVANCE_53=0x534,
    MARKUP_RELEVANCE_54=0x535,
    MARKUP_RELEVANCE_55=0x536,
    MARKUP_RELEVANCE_56=0x537,
    MARKUP_RELEVANCE_57=0x538,
    MARKUP_RELEVANCE_58=0x539,
    MARKUP_RELEVANCE_59=0x53A,
    MARKUP_RELEVANCE_60=0x53B,
    MARKUP_RELEVANCE_61=0x53C,
    MARKUP_RELEVANCE_62=0x53D,
    MARKUP_RELEVANCE_63=0x53E,
    MARKUP_RELEVANCE_64=0x53F,
    MARKUP_RELEVANCE_65=0x540,
    MARKUP_RELEVANCE_66=0x541,
    MARKUP_RELEVANCE_67=0x542,
    MARKUP_RELEVANCE_68=0x543,
    MARKUP_RELEVANCE_69=0x544,
    MARKUP_RELEVANCE_70=0x545,
    MARKUP_RELEVANCE_71=0x546,
    MARKUP_RELEVANCE_72=0x547,
    MARKUP_RELEVANCE_73=0x548,
    MARKUP_RELEVANCE_74=0x549,
    MARKUP_RELEVANCE_75=0x54A,
    MARKUP_RELEVANCE_76=0x54B,
    MARKUP_RELEVANCE_77=0x54C,
    MARKUP_RELEVANCE_78=0x54D,
    MARKUP_RELEVANCE_79=0x54E,
    MARKUP_RELEVANCE_80=0x54F,
    MARKUP_RELEVANCE_81=0x550,
    MARKUP_RELEVANCE_82=0x551,
    MARKUP_RELEVANCE_83=0x552,
    MARKUP_RELEVANCE_84=0x553,
    MARKUP_RELEVANCE_85=0x554,
    MARKUP_RELEVANCE_86=0x555,
    MARKUP_RELEVANCE_87=0x556,
    MARKUP_RELEVANCE_88=0x557,
    MARKUP_RELEVANCE_89=0x558,
    MARKUP_RELEVANCE_90=0x559,
    MARKUP_RELEVANCE_91=0x55A,
    MARKUP_RELEVANCE_92=0x55B,
    MARKUP_RELEVANCE_93=0x55C,
    MARKUP_RELEVANCE_94=0x55D,
    MARKUP_RELEVANCE_95=0x55E,
    MARKUP_RELEVANCE_96=0x55F,
    MARKUP_RELEVANCE_97=0x560,
    MARKUP_RELEVANCE_98=0x561,
    MARKUP_RELEVANCE_99=0x562,
    MARKUP_RELEVANCE_100=0x563,
    MARKUP_RELEVANCE_101=0x564,
    MARKUP_RELEVANCE_102=0x565,
    MARKUP_RELEVANCE_103=0x566,
    MARKUP_RELEVANCE_104=0x567,
    MARKUP_RELEVANCE_105=0x568,
    MARKUP_RELEVANCE_106=0x569,
    MARKUP_RELEVANCE_107=0x56A,
    MARKUP_RELEVANCE_108=0x56B,
    MARKUP_RELEVANCE_109=0x56C,
    MARKUP_RELEVANCE_110=0x56D,
    MARKUP_RELEVANCE_111=0x56E,
    MARKUP_RELEVANCE_112=0x56F,
    MARKUP_RELEVANCE_113=0x570,
    MARKUP_RELEVANCE_114=0x571,
    MARKUP_RELEVANCE_115=0x572,
    MARKUP_RELEVANCE_116=0x573,
    MARKUP_RELEVANCE_117=0x574,
    MARKUP_RELEVANCE_118=0x575,
    MARKUP_RELEVANCE_119=0x576,
    MARKUP_RELEVANCE_120=0x577,
    MARKUP_RELEVANCE_121=0x578,
    MARKUP_RELEVANCE_122=0x579,
    MARKUP_RELEVANCE_123=0x57A,
    MARKUP_RELEVANCE_124=0x57B,
    MARKUP_RELEVANCE_125=0x57C,
    MARKUP_RELEVANCE_126=0x57D,
    MARKUP_RELEVANCE_127=0x57E,
    MARKUP_RELEVANCE_128=0x57F,
    
    MARKUP_RELEVANCE_BEGIN = MARKUP_RELEVANCE_1,
    MARKUP_RELEVANCE_END   = MARKUP_RELEVANCE_128
};

typedef struct __MARKUPTAG
{
  unsigned short m_Type, m_Aux;  /* M_Aux: mask for last tag */
  unsigned int   m_nBeg;
} MarkupTag;

typedef struct __MarkupItem
{
  unsigned short m_Type, m_Aux;  /* M_Aux: mask for last tag */
  unsigned int   m_nBeg, m_nLen;
} MarkupItem;

/*! \struct MarkupAttr
    \brief single html tag attribute
*/
typedef struct __MarkupAttr
{
    char *name;  // both <= 255 bytes
    char *value; //
} MarkupAttr;

/*! \struct MarkupAttrArray
    \brief attributes of a tag
*/
typedef struct __MarkupAttrArray
{
    unsigned short size;
    MarkupAttr     *attr;
} MarkupAttrArray;

#define MARKUP_LAST 0x8000 /* m_Aux mask for linked lists */

#define IsMarkupLast(item) ((((item).m_Aux)&MARKUP_LAST)!=0)
#define IsOpeningTag(tag)  ((((tag).m_Type)&MARKUP_CLOSING)==0)
#define IsClosingTag(tag)  ((((tag).m_Type)&MARKUP_CLOSING)!=0)
#define IsEqualTag(tag1,tag2)  (((tag1).m_Type&~MARKUP_CLOSING)==((tag2).m_Type&~MARKUP_CLOSING))

#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

#ifdef _MSC_VER
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif /*__MarkupTypes_H*/
