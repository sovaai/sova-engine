#ifndef _STRICTMORPHPARAMS_H_
#define _STRICTMORPHPARAMS_H_

#include <sys/types.h>
#include <_include/language.h>

class StrictMorphParams
{
public:
        enum Flags
        {
                FlagNone              = 0x00000000U,
                FlagCaseInsensitive   = 0x00000001U,
                FlagLwrAccent         = 0x00000002U,
                FlagLwrPunct          = 0x00000004U,
                FlagRemIgnore         = 0x00000008U,
                FlagNativeOnly        = 0x00000010U,
                FlagProcessCompounds  = 0x00000020U,
                FlagArabicSeqCollapse = 0x00000040U,
                FlagUseDefault        = 0x80000000U,
                FlagInvalid           = 0xFFFFFFFFU
        };
	
public:
	StrictMorphParams() :
		language( LNG_RUSSIAN ),
		internalNumber( 0 ),
		maxWordLength( 0 ),
                minStemLength( 0 ),
                minCompoundStemLength( 0 ),
		flags( (unsigned int)FlagUseDefault )
	{
	}
	~StrictMorphParams() { }

public: /* Data */
	LangCodes    language;
	unsigned int internalNumber;
	size_t       maxWordLength;
        size_t       minStemLength;
        size_t       minCompoundStemLength;
	unsigned int flags;
};

#endif /* _STRICTMORPHPARAMS_H_ */

