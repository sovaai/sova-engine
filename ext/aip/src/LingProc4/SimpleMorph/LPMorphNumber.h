/**
 * @file   LPMorphNumber.h
 * @author swizard <me@swizard.info>
 * @date   Tue Apr  8 23:18:29 2008
 * 
 * @brief  Numerical morphology for LingProc.
 * 
 * 
 */
#ifndef _LPMORPHNUMBER_H_
#define _LPMORPHNUMBER_H_

#include "LPMorphInterface.h"

/* The numerical morphology */
class LPMorphNumber : public LPMorphInterface
{
public:
    /** 
     * Standard empty constructor with all inheritance propagation.
     * 
     */
    LPMorphNumber() : LPMorphInterface() { }
    /** 
     * Standard empty destructor with all inheritance propagation.
     * 
     */
    ~LPMorphNumber() { }

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
    LingProcErrors close()
    {
        return LP_ERROR_ENOTIMPL;
    }
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
    size_t size();

private:
    size_t processWord( char         *outBuffer,
                        LEXID        &lex,
                        const char   *word,
                        size_t        wordLength = (size_t)(-1) );
};

#endif /* _LPMORPHNUMBER_H_ */

