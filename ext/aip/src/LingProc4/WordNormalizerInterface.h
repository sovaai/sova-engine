#ifndef _WORDNORMALIZERINTERFACE_H_
#define _WORDNORMALIZERINTERFACE_H_

#include <LingProc4/UCharSet/UCharSet.h>
#include <_include/cc_compat.h>
#include <_include/_inttype.h>

#include "SimpleMorph/LPMorphCommon.h"

/* Normalization result, can be one of those values: */
enum WordNormalizerResult
{
    NORMALIZE_SUCCESS,          /* Ok */
    NORMALIZE_ERROR_PARAMETER,  /* An invalid parameter provided (i.e. null word) */
    NORMALIZE_ERROR_FLAGS,      /* Invalid flags combination is passed */
    NORMALIZE_ERROR_BUFFER,     /* getResultBuffer method failed */
    NORMALIZE_ERROR_NOT_NATIVE, /* Symbol is not native for the language */
    NORMALIZE_ERROR_FUNCTOR,    /* An error occurred in functor invokation */
    NORMALIZE_ERROR_INTERNAL    /* Probably bug in the implementation */
};

/* Word capitalization detected during normalization */
enum WordNormalizerCaps
{
    NORMALIZE_CAPS_NO_CASE,
    NORMALIZE_CAPS_LOWER_CASE,
    NORMALIZE_CAPS_TITLE_CASE,
    NORMALIZE_CAPS_UPPER_CASE
};

class UCharSet;

/* A functor for results receiving */
class WordNormalizerFunctor
{
public:
    WordNormalizerFunctor() { }
    virtual ~WordNormalizerFunctor() { }

public:
    virtual char                 *getResultBuffer( size_t requestSize ) = 0;
    virtual WordNormalizerResult  nextResult( const char         *result,
                                              size_t              resultSize,
                                              WordNormalizerCaps  caps ) = 0;
};

/* The class interface */
class WordNormalizer
{
protected:
    WordNormalizer() : charset( 0 ), defaultFlags( MORPH_FLAG_INVALID ) { }

public:
    static WordNormalizer *factory( LangCodes lang, MorphTypes morph );

    virtual ~WordNormalizer() { }

public:
    virtual WordNormalizerResult create( const UCharSet *charset, uint32_t defaultFlags = MORPH_FLAG_INVALID )
    {
        /* Check if some parameters are invalid */
        if ( unlikely(charset == 0) )
            return NORMALIZE_ERROR_PARAMETER;

        this->charset             = charset;
        this->defaultFlags        = defaultFlags;

        return NORMALIZE_SUCCESS;
    }

    /**
     * Word normalization with default flags given at construction time.
     *
     * @param functor A functor to retrieve normalization results
     * @param word A word to normalize
     * @param wordSize A word length, can be static_cast<size_t>(-1) (default) if zero-terminated string
     *
     * @return Normalization result
     * @see WordNormalizerResult
     */
    WordNormalizerResult normalizeWord( WordNormalizerFunctor  &functor,
                                        const char             *word,
                                        size_t                  wordSize = static_cast<size_t>(-1) )
    {
        return normalizeWord( functor, defaultFlags, word, wordSize );
    }

    /**
     * Generic method for word normalization.
     *
     * @param functor A functor to retrieve normalization results
     * @param flags A flags according to those normalization is performed
     * @param word A word to normalize
     * @param wordSize A word length, can be static_cast<size_t>(-1) (default) if zero-terminated string
     *
     * @return Normalization result
     * @see WordNormalizerResult
     */
    virtual WordNormalizerResult normalizeWord( WordNormalizerFunctor &functor,
                                                uint32_t               flags,
                                                const char            *word,
                                                size_t                 wordSize = static_cast<size_t>(-1) ) = 0;

    uint32_t getDefaultFlags() const { return defaultFlags; }

protected:
    const UCharSet *charset;
    uint32_t        defaultFlags;
};

/* WordNormalizerFunctor for one-shot results into the target buffer */
class WordNormalizerTarget : public WordNormalizerFunctor
{
public:
    WordNormalizerTarget( char *_target, size_t _targetSize ) :
        WordNormalizerFunctor(),
        target( _target ),
        targetSize( _targetSize ),
        resultSize( 0 ),
        caps( NORMALIZE_CAPS_NO_CASE ) { }
    ~WordNormalizerTarget() { }

public:
    char *getResultBuffer( size_t requestSize )
    {
        if ( requestSize <= targetSize )
            return target;
        return 0;
    }

    WordNormalizerResult nextResult( const char*         /* result */,
                                     size_t              resultSize,
                                     WordNormalizerCaps  caps )
    {
        this->resultSize = resultSize;
        this->caps       = caps;
        return NORMALIZE_SUCCESS;
    }

private:
    char   *target;
    size_t  targetSize;

public:
    size_t             resultSize;
    WordNormalizerCaps caps;
};

#endif /* _WORDNORMALIZERINTERFACE_H_ */

