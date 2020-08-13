/**
 * @file   LPMorphStrictReal.h
 * @author swizard <me@swizard.info>
 * @date   Sat Jul 19 20:42:25 2008
 * 
 * @brief  Strict morphology for LingProc.
 * 
 * 
 */
#ifndef _LPMORPHSTRICT_H_
#define _LPMORPHSTRICT_H_

#include <lib/aptl/Functor.h>
#include <lib/aptl/avector.h>

#include <LingProc4/StrictMorph/API/StrictMorphFS.h>

#include "LPMorphInterface.h"
#include "../BinaryData.h"
#include "../LPStrict.h"

/* The strict morphology */
class LPMorphStrict : public LPMorphInterface
{
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    LPMorphStrict();
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~LPMorphStrict();

    /* The interface implementation */
public:	
    /** 
     * Creates and initializes the new morphology.
     * 
     * @param charset A charset to use
     * @param params The creation parameters
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors create(const UCharSet *charset, const LPMorphCreateParams *params);
    /** 
     * Flushes the morphology (force it to lose all accumulated data).
     * 
     */
    LingProcErrors flush()
    {
        return LP_ERROR_ENOTIMPL;
    }
    /** 
     * Closes the morphology.
     * 
     */
    LingProcErrors close();
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
    int lemmatize(LemmatizeResultFunctor &functor,
                  const char             *word,
                  size_t                  wordLength = (size_t)(-1),
                  unsigned int            flags = 0);
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
    int stemmatize(StemmatizeResultFunctor &functor,
                   const char*              word,
                   size_t                   wordLength = (size_t)(-1),
                   unsigned int             flags = 0);
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
    int getLexText(LEXID lex, char *buffer, size_t bufferSize, const BinaryData *additionalData = 0);
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
    int getLexText(LEXID partialLex, const char *stem, char *buffer, size_t bufferSize, const BinaryData *additionalData = 0);
    /** 
     * Iterates all lexids hold within the morphology with enumerator object given.
     * 
     * @param enumerator An enumerator functor object
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors enumerate(MorphEnumerator &enumerator);
    /** 
     * Returns the size of the morphology.
     * 
     * 
     * @return The size requested
     */
    size_t size()
    {
        return static_cast<size_t>(0);
    }

public:
    /** 
     * Attaches the fstorage with strict morph data. Used by strict morphology. 
     * 
     * @param fs A fstorage with strict morph data
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors FSAttachStrictMorph(fstorage *fs);
    /** 
     * Creates the morphology from the previously initialized template.
     * 
     * @param charset A charset to use
     * @param templateFs The fstorage handler containing the template
     * @param sectionId The section identificator within the fstorage
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors FSCreateFromTemplate(const UCharSet      *charset,
                                        fstorage            *templateFs,
                                        fstorage_section_id  sectionId);
    /** 
     * Opens the morphology from a fstorage handler with the read/write access.
     * 
     * @param charset A charset to use
     * @param templateFs The fstorage handler containing the morphology
     * @param sectionId The section identificator within the fstorage
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors FSOpenRW(const UCharSet      *charset,
                            fstorage            *fs,
                            fstorage_section_id  sectionId);
    /** 
     * Opens the morphology from a fstorage handler with the read only access.
     * 
     * @param charset A charset to use
     * @param templateFs The fstorage handler containing the morphology
     * @param sectionId The section identificator within the fstorage
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors FSOpenRO(const UCharSet      *charset,
                            fstorage            *fs,
                            fstorage_section_id  sectionId);
    /** 
     * Opens the morphology from a fstorage handler with the limited (very basic) access.
     * 
     * @param charset A charset to use
     * @param templateFs The fstorage handler containing the morphology
     * @param sectionId The section identificator within the fstorage
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors FSOpenLimited(const UCharSet      *charset,
                                 fstorage            *fs,
                                 fstorage_section_id  sectionId);
    /** 
     * Store the morphology to the given fstorage in the given format.
     * 
     * @param templateFs The fstorage handler to where to store the morphology
     * @param sectionId The section identificator within the fstorage
     * @param transport True if the transport format should be used, false if work one
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors FSSave(fstorage            *fs,
                          fstorage_section_id  sectionId,
                          bool                 transport);
    /** 
     * Creates LPStrict::Info wrapper around LEXID given.
     * 
     * @param lex LEXID to wrap Info around
     * @param resolver Results receiver
     * 
     * @return LP_OK if successfull or an error otherwise
     */
    LingProcErrors resolveLPStrict( LEXID lex, LPStrict::Resolver &resolver ) const;
    /** 
     * Returns strict morphology dictionary information (if available).
     * 
     * 
     * @return Dictionaty information struct or NULL if no information available
     */
    const StrictDictInfo *getStrictDictInfo() const;
        
        
private:
    void initData();
    LingProcErrors deserializeHeader(fstorage *fs, fstorage_section_id sectionId);
    LingProcErrors serializeHeader(fstorage *fs, fstorage_section_id sectionId);
    LingProcErrors initCharSet(const UCharSet *charset);
    LingProcErrors openCommon(const UCharSet *charset, fstorage *fs, fstorage_section_id sectionId);
    LingProcErrors initializeStrictMorph();
        
public:
    struct CompoundLex
    {
        CompoundLex() :
            lex( 0 ),
            bound( 0 ),
            nextHomoIdx( -1 ),
            formsSetsOffset( 0 ),
            formsSetsCount( 0 ),
            additionalData() { }
                
        LEXID      lex;
        size_t     bound;
        int        nextHomoIdx;
        size_t     formsSetsOffset;
        size_t     formsSetsCount;
        BinaryData additionalData;
    };

    typedef avector<CompoundLex>        CompoundLexPool;
    typedef avector<StrictWordFormsSet> FormsSetsPool;

    struct DecompositionChain
    {
        DecompositionChain() : pool( 0 ), chainOffset( 0 ), chainLength( 0 ) { }
        DecompositionChain( const CompoundLexPool *_pool, size_t _chainOffset, size_t _chainLength ) :
            pool( _pool ), chainOffset( _chainOffset ), chainLength( _chainLength ) { }
                
        const CompoundLexPool *pool;
        size_t                 chainOffset;
        size_t                 chainLength;
    };
        
    typedef avector<DecompositionChain> DecompositionChainPool;
        
private:
    class FunctorsMapper : public StrictLex::LemmatizeFunctor
    {
    public:
        FunctorsMapper( const StrictMorph      &_morph,
                        LemmatizeResultFunctor &_functor,
                        CompoundLexPool        &_lexPool,
                        FormsSetsPool          &_formsSetsPool,
                        DecompositionChainPool &_chainsPool,
                        bool                    _forceCompounds,
                        bool                    _forceDictResolve );
        ~FunctorsMapper();

    public:
        int apply(const StrictLex &lex);
        int finish();
                
        bool           hasInvoked()   { return invokeFlag; }
        LingProcErrors getStatus()    { return status; }
        size_t         getHomoCount() { return homoCount; }
                
    private:
        int recurseCompound( const StrictLex *lex, size_t depth, size_t offset );
                
    private:
        const StrictMorph      &morph;
        LemmatizeResultFunctor &functor;
        CompoundLexPool        &lexPool;
        FormsSetsPool          &formsSetsPool;
        DecompositionChainPool &chainsPool;
        bool                    forceCompounds;
        bool                    forceDictResolve;
        bool                    invokeFlag;
        LingProcErrors          status;
        size_t                  homoCount;
    };
        
        
private:
    struct HeaderData
    {
        uint32_t magic;
        uint32_t work_format_version;
        uint32_t work_format_variant;
        uint32_t transport_format_version;
        uint32_t transport_format_variant;
        bool     is_transport;
    } headerData;
        
private:
    fstorage               *strict_morph_data_fs;
    StrictMorphFS           morph;
    CompoundLexPool         lexPool;
    FormsSetsPool           formsSetsPool;
    DecompositionChainPool  chainsPool;
};

#endif /* _LPMORPHSTRICT_H_ */

