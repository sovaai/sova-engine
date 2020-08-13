/**
 * @file   LPMorphInterface.h
 * @author swizard <me@swizard.info>
 * @date   Mon Apr  7 16:24:39 2008
 * 
 * @brief  A generic interface for LingProc morphology.
 * 
 * 
 */
#ifndef _LPMORPHINTERFACE_H_
#define _LPMORPHINTERFACE_H_

#include <lib/fstorage/fstorage.h>

#include "../UCharSet/UCharSet.h"
#include "../LemmatizeResultFunctor.h"
#include "../StemmatizeResultFunctor.h"
#include "../NormalizeResultFunctor.h"
#include "../LingProcErrors.h"
#include "../LanguageInfo.h"

#include "LPMorphCommon.h"
#include "MorphEnumerator.h"

struct StrictDictInfo;
struct BinaryData;
namespace LPStrict
{
    class Info;
    class Resolver;
}

/* The morphology interface */
class LPMorphInterface
{
public:
    LPMorphInterface() { }
    virtual ~LPMorphInterface() { }

    /* The interface: obligatory part */
public:	
    /** 
     * Creates and initializes the new morphology.
     * 
     * @param charset A charset to use
     * @param params The creation parameters
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors create( const UCharSet *charset, const LPMorphCreateParams *params ) = 0;
    /** 
     * Flushes the morphology (force it to lose all accumulated data).
     * 
     */
    virtual LingProcErrors flush() = 0;
    /** 
     * Closes the morphology.
     * 
     */
    virtual LingProcErrors close() = 0;
    /** 
     * Performs the word lemmatization.
     * 
     * @param functor A functor object which will receive each lemmatization result
     * @param word The word to lemmatize
     * @param wordLength The length of the word
     * @param flags Specific lemmatization flags
     * 
     * @return 0 if a morphology cannot handle the word, homonyms count if success or an error otherwise
     */
    virtual int lemmatize( LemmatizeResultFunctor &functor,
                           const char             *word,
                           size_t                  wordLength = static_cast<size_t>( -1 ),
                           unsigned int            flags = 0 ) = 0;
    /** 
     * Performs the word stemmatization. The stem is terminated with '\0'.
     * 
     * @param functor A functor object which will receive each stemmatization result
     * @param word The word to stemmatize
     * @param wordLength The length of the word
     * @param flags Specific stemmatization flags
     * 
     * @return 0 if a morphology cannot handle the word, homonyms count if success or an error otherwise
     */
    virtual int stemmatize( StemmatizeResultFunctor &functor,
                            const char              *word,
                            size_t                   wordLength = static_cast<size_t>( -1 ),
                            unsigned int             flags = 0 ) = 0;
    /** 
     * Performs the word normalization.
     * 
     * @param functor A functor object which will receive each normalization result
     * @param word The word to normalize
     * @param wordLength The length of the word
     * @param flags Specific normalization flags
     * 
     * @return 0 if a morphology cannot handle the word, homonyms count if success or an error otherwise
     */
    virtual int normalize( NormalizeResultFunctor &/* functor */,
                           const char*             /* word */,
                           size_t                  /* wordLength = static_cast<size_t>( -1 ) */,
                           unsigned int            /* flags = 0 */ )
    {
        return 0;
    }
    /** 
     * Requests the text back for a LEXID data given.
     *  Returns:
     *   - LingProcErrors (<0) if an error occurred, out buffer is unchanged
     *   - 0 if the given lexeme does not belong to the morphology, out buffer is unchanged
     *   - non-zero lexeme length exluding '\0' char on success and:
     *     * if the lexeme length is smaller than bufferSize, the buffer is filled and trailing '\0' is added
     *     * if the lexeme length is equal to bufferSize, the buffer is filled and trailing '\0' is omitted
     *     * if the lexeme length is greater than bufferSize, the buffer content is untouched
     * 
     * @param lex A LEXID for which to request a text
     * @param buffer A buffer to put the text into
     * @param bufferSize A size of the buffer given
     * 
     * @return The size required to fit the whole text (excluding '\0') or LingProcErrors error on failure
     */
    virtual int getLexText( LEXID lex, char *buffer, size_t bufferSize, const BinaryData *additionalData = 0 ) = 0;
    /** 
     * Requests the text back for a partial LEXID data and a stem given (obtained from stemmatization).
     *  Returns:
     *   - LingProcErrors (<0) if an error occurred, out buffer is unchanged
     *   - 0 if the given lexeme does not belong to the morphology, out buffer is unchanged
     *   - non-zero lexeme length exluding '\0' char on success and:
     *     * if the lexeme length is smaller than bufferSize, the buffer is filled and trailing '\0' is added
     *     * if the lexeme length is equal to bufferSize, the buffer is filled and trailing '\0' is omitted
     *     * if the lexeme length is greater than bufferSize, the buffer content is untouched
     * 
     * @param partialLex A partial LEXID for which to request a text
     * @param stem A stem that should be repaired
     * @param buffer A buffer to put the text into
     * @param bufferSize A size of the buffer given
     * 
     * @return The size required to fit the whole text (excluding '\0') or LingProcErrors error on failure
     */
    virtual int getLexText( LEXID partialLex, const char *stem, char *buffer, size_t bufferSize, const BinaryData *additionalData = 0 ) = 0;
    /** 
     * Iterates all lexids hold within the morphology with enumerator object given.
     * 
     * @param enumerator An enumerator functor object
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors enumerate( MorphEnumerator &enumerator ) = 0;
    /** 
     * Returns the size of the morphology.
     * 
     * 
     * @return The size requested
     */
    virtual size_t size() = 0;
                
public:
    /** 
     * Returns the mophology dictionary data.
     * 
     * 
     * @return The data requested
     */
    const LPMorphDictInfo *info()
    {
        return &_info;
    }
	
    /* The interface: optional part (not implemented by default) */
public:	
    /** 
     * Attaches the fstorage with strict morph data. Used by strict morphology. 
     * 
     * @param fs A fstorage with strict morph data
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors FSAttachStrictMorph( fstorage* /*fs*/ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Creates the morphology from the previously initialized template.
     * 
     * @param charset A charset to use
     * @param templateFs The fstorage handler containing the template
     * @param sectionId The section identificator within the fstorage
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors FSCreateFromTemplate( const UCharSet      * /* charset */,
                                                 fstorage            * /* templateFs */,
                                                 fstorage_section_id   /* sectionId */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Opens the morphology from a fstorage handler with the read/write access.
     * 
     * @param charset A charset to use
     * @param templateFs The fstorage handler containing the morphology
     * @param sectionId The section identificator within the fstorage
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors FSOpenRW( const UCharSet      * /* charset */,
                                     fstorage            * /* fs */,
                                     fstorage_section_id   /* sectionId */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Opens the morphology from a fstorage handler with the read only access.
     * 
     * @param charset A charset to use
     * @param templateFs The fstorage handler containing the morphology
     * @param sectionId The section identificator within the fstorage
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors FSOpenRO( const UCharSet      * /* charset */,
                                     fstorage            * /* fs */,
                                     fstorage_section_id   /* sectionId */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Opens the morphology from a fstorage handler with the limited (very basic) access.
     * 
     * @param charset A charset to use
     * @param templateFs The fstorage handler containing the morphology
     * @param sectionId The section identificator within the fstorage
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors FSOpenLimited( const UCharSet      * /* charset */,
                                          fstorage            * /* fs */,
                                          fstorage_section_id   /* sectionId */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Store the morphology to the given fstorage in the given format.
     * 
     * @param templateFs The fstorage handler to where to store the morphology
     * @param sectionId The section identificator within the fstorage
     * @param transport True if the transport format should be used, false if work one
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors FSSave( fstorage            * /* fs */,
                                   fstorage_section_id   /* sectionId */,
                                   bool                  /* transport */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Creates the morphology from the previously initialized template.
     * 
     * @param charset A charset to use
     * @param templateFile The file containing the template
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors createFromTemplate( const UCharSet * /* charset */,
                                               const char * /* templateFile */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Opens the morphology from a file with the read/write access.
     * 
     * @param charset A charset to use
     * @param filename The file with a morphology
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors openRW( const UCharSet * /* charset */, const char * /* filename */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Opens the morphology from a file with the read only access.
     * 
     * @param charset A charset to use
     * @param filename The file with a morphology
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors openRO( const UCharSet * /* charset */, const char * /* filename */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Opens the morphology from a file with the limited (very basic) access.
     * 
     * @param charset A charset to use
     * @param filename The file with a morphology
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors openLimited( const UCharSet * /* charset */, const char * /* filename */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Stores the morphology to a given file.
     * 
     * @param filename The file to where to save the morphology
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors save( const char * /* filename */, bool /* transport */ )
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Creates LPStrict::Info wrapper around LEXID given.
     * 
     * @param lex LEXID to wrap Info around
     * @param resolver Results receiver
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    virtual LingProcErrors resolveLPStrict( LEXID /* lex */, LPStrict::Resolver &/* resolver */ ) const
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Returns strict morphology dictionary information (if available).
     * 
     * 
     * @return Dictionaty information struct or NULL if no information available
     */
    virtual const StrictDictInfo *getStrictDictInfo() const
    {
        return 0;
    }


public:
    /* Morphology properties */
    bool isValid()
    {
        return _charset != 0;
    }
    LangCodes getLang()
    {
        return _info.lang;
    }
    unsigned int internalNumber()
    {
        return _info.internalNumber;
    }
    MorphTypes type()
    {
        return _info.type;
    }
	
protected:
    LPMorphDictInfo  _info;
    const UCharSet  *_charset;
	
};

static char digits[] = "0123456789ABCDEF";

inline char *dumpHex( uint64_t value, int fourBitDigits, char *buffer )
{
        char *p = buffer;
        
        for ( int i = fourBitDigits - 1; i >= 0; i-- )
        {
                char digit = digits[ ((value >> (i << 2)) & 0xF) ];
                if ( digit != '0' || p != buffer )
                        *p++ = digit;
        }

        return p;
}

template< typename LexidType > inline uint64_t realMorphNumber( const LPMorphDictInfo &_info );

template< > inline uint64_t realMorphNumber< uint32_t >( const LPMorphDictInfo &_info )
{
    const LanguageInfoDb< uint64_t > *langInfo =
        LanguageInfoDb< uint64_t >::getLanguageInfo( _info.lang );

    switch ( _info.type )
    {
    case MORPH_TYPE_HIER:
    case MORPH_TYPE_FUZZY:
        return langInfo->fuzzyNumber;

    case MORPH_TYPE_STRICT:
        return langInfo->strictNumber;

    case MORPH_TYPE_TILDE:
        return langInfo->tildeNumber;

    case MORPH_TYPE_TILDE_REV:
        return langInfo->tildeRevNumber;

    default:
        return 0;
    };
}

template< > inline uint64_t realMorphNumber< uint64_t >( const LPMorphDictInfo &_info )
{
    return _info.internalNumber;
}

#endif /* _LPMORPHINTERFACE_H_ */

