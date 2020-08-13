/**
 * @file   LPMorphFuzzy.h
 * @author swizard <me@swizard.info>
 * @date   Wed Apr  9 15:21:19 2008
 * 
 * @brief  Fuzzy morphology for LingProc.
 * 
 * 
 */
#ifndef _LPMORPHFUZZY_H_
#define _LPMORPHFUZZY_H_

#include <lib/libpts/pts.h>
#include <lib/libpts/pat_tree.h>
#include <lib/aptl/avector.h>
#include <lib/aptl/OpenAddressingDict.h>

#include "LPMorphInterface.h"
#include "../WordNormalizerInterface.h"

/* The fuzzy morphology */
class LPMorphFuzzy : public LPMorphInterface
{
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    LPMorphFuzzy();
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~LPMorphFuzzy();

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
    int stemmatize(StemmatizeResultFunctor &functor,
                   const char              *word,
                   size_t                   wordLength = (size_t)(-1),
                   unsigned int             flags = 0);
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
    int normalize(NormalizeResultFunctor &functor,
                  const char             *word,
                  size_t                  wordLength = (size_t)(-1),
                  unsigned int            flags = 0);
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
        return (stems ? static_cast<size_t>(pts_items_count(stems)) : 0);
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

public:
    bool isHier()
    {
        return type() == MORPH_TYPE_HIER;
    }
	
private:
    int cutSuffix( unsigned char *outBuffer,
                   LEXID         &lexid,
                   const char    *word,
                   size_t         wordLength );
    void initData();
    LingProcErrors deserializeHeader(fstorage *fs, fstorage_section_id sectionId);
    LingProcErrors serializeHeader(fstorage *fs, fstorage_section_id sectionId);
    LingProcErrors initCharSet(const UCharSet *charset);
    LingProcErrors openCommon(const UCharSet *charset, fstorage *fs, fstorage_section_id sectionId);
    LingProcErrors loadSuffixes(fstorage *fs, fstorage_section_id sectionId);

private:
    typedef OpenAddressingDict< const char *, size_t, 1024, 0, 1 > FlexionsHash;
    typedef avector<const char *>                                  FlexionsBackRef;

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

    WordNormalizer         *normalizer;
    FlexionsHash            suffixes;
    FlexionsBackRef         suffixesBackRef;
    pat_tree               *stopWords;
    patricia_tree_storage  *stems;
};

#endif /* _LPMORPHFUZZY_H_ */

