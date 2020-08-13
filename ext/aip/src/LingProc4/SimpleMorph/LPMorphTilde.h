/**
 * @file   LPMorphTilde.h
 * @author swizard <me@swizard.info>
 * @date   Mon Sep  1 19:00:24 2008
 * 
 * @brief  A fuzzymorph-like morphology with special behaviour.
 * 
 * 
 */
#ifndef _LPMORPHTILDE_H_
#define _LPMORPHTILDE_H_

#include <lib/libpts2/PatriciaTreeFS.h>

#include "LPMorphInterface.h"
#include "../WordNormalizer.h"

class TildeCustomDirect
{
protected:
    /* Tilde morphology constants */
    static const MorphTypes MORPH_TYPE = MORPH_TYPE_TILDE;
        
    static const uint32_t TILDEMORPH_MAX_WORD_LEN                = 4096;
    static const uint32_t TILDEMORPH_MIN_STEM_LEN                = 2;
    static const uint32_t TILDEMORPH_VERSION                     = 1;
    static const uint32_t TILDEMORPH_VARIANT                     = 1;
    static const int      TILDEMORPH_SPEC_SUFFIX                 = 255;

    static const size_t   TILDEMORPH_FS_HEADER_SIZE              = (sizeof(uint32_t) * 11);
    static const uint32_t TILDEMORPH_FS_MAGIC                    = 0x0C1974AAU;
    static const uint32_t TILDEMORPH_FS_WORK_FORMAT_VERSION      = 2;
    static const uint32_t TILDEMORPH_FS_WORK_FORMAT_VARIANT      = 1;

    static const int      TILDEMORPH_FS_SEC_STEMS                = 1;

protected:
    const char *preprocessWord( const char *word, size_t /* wordLength */ ) { return word; }
    void        postprocessWord( char* /* buffer */, size_t /* totalLen */ ) { }
};

/* The tilde morphology */
template< typename TildeCustom >
class LPMorphTildeImpl : public LPMorphInterface, public TildeCustom
{
private:
    enum Constants
    {
        treeFlags =
        PatriciaTree::TREE_STANDARD   |
        PatriciaTree::TREE_CROSSLINKS |
        PatriciaTree::TREE_REVERSE_INDEX
    };
        
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    LPMorphTildeImpl();
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~LPMorphTildeImpl();

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
    LingProcErrors flush();
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
    int stemmatize(StemmatizeResultFunctor& /* functor */,
                   const char*              /* word */,
                   size_t                   /* wordLength = (size_t)(-1) */,
                   unsigned int             /* flags = 0 */)
    {
        return LP_ERROR_ENOTIMPL;
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
    int getLexText(LEXID /* partialLex */, const char* /* stem */, char* /* buffer */, size_t /* bufferSize */, const BinaryData* /* additionalData = 0 */)
    {
        return LP_ERROR_ENOTIMPL;
    }
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
        return (tree != 0 ? tree->getIndexSize() : 0);
    }

public:
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

	
private:
    void initData();
    LingProcErrors deserializeHeader(fstorage *fs, fstorage_section_id sectionId);
    LingProcErrors serializeHeader(fstorage *fs, fstorage_section_id sectionId);
    LingProcErrors initCharSet(const UCharSet *charset);
    LingProcErrors openCommon(const UCharSet *charset, fstorage *fs, fstorage_section_id sectionId);
                
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
        
    WordNormalizer  *normalizer;
    PatriciaTreeFS  *tree;
};

typedef LPMorphTildeImpl< TildeCustomDirect > LPMorphTilde;

#endif /* _LPMORPHTILDE_H_ */

