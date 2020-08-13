#ifndef _STRICTMORPH_H_
#define _STRICTMORPH_H_

#include <sys/types.h>

#include <lib/aptl/Functor.h>
#include <lib/aptl/FunctorBuffer.h>

#include <LingProc4/StrictMorphGram/StrictLex.h>
#include <LingProc4/StrictMorphGram/StrictDictInfo.h>
#include <LingProc4/StrictMorphGram/StrictMorphErrors.h>

#include "StrictMorphParams.h"
#include "StrictMorphMemoryHandler.h"

/* A lexeme internal number: use the special helpers on it to obtain lexeme information */
typedef unsigned int LexNumber;

/* The strict morphology handler */

class UCharSet;
class StrictMorphShadow;
class WordNormalizerFunctor;
struct BinaryData;

class StrictMorph
{
public:
        /* Functors for unpacking */
        typedef Functor<const StrictLex &, StrictMorphErrors> UnpackFunctor;

public:
        StrictMorph();
        ~StrictMorph();

public:
        /** 
         * Initializes the StrictMorph instance based on data in handler provided.
         * 
         * @param charset A charset to use for the current morphology
         * @param params  A structure with creation parameters filled
         * @param handler A handler with morphology data
         * @param underlyingError The error lifted if failure
         * 
         * @return Creation result as StrictMorphErrors
         */
        StrictMorphErrors create( const UCharSet                 *charset,
                                  const StrictMorphParams        &params,
                                  const StrictMorphMemoryHandler &handler,
                                  int                            &underlyingError );

public:
        /** 
         * For each lexeme within a dictionary creates its paradigm and invokes the
         * given enumerator for each LEXID from paradigm.
         * 
         * @param enumerator A functor to invoke with the LEXID constructed
         * 
         * @return Enumeration result as StrictMorphErrors
         */
        StrictMorphErrors enumerate( StrictLex::LemmatizeFunctor &enumerator ) const;

        /** 
         * Performs the word lemmatization.
         * 
         * @param functor A functor object which will receive each lemmatization result
         * @param word The word to lemmatize
         * @param wordLength The length of the word
         * @param flags StrictMorphParams::FlagUseDefault or special flags.
         * 
         * @return Lemmatization result as StrictMorphErrors
         */
        StrictMorphErrors lemmatize( StrictLex::LemmatizeFunctor &functor,
                                     const char                  *word,
                                     size_t                       wordLength = static_cast<size_t>(-1),
                                     unsigned int                 flags = StrictMorphParams::FlagUseDefault ) const;
        
        /** 
         * Uses the builtin morphology normalizer to normalize a given word.
         * 
         * @param functor A functor for results receiving
         * @param word A word to normalize
         * @param wordLength The length of the word
         * @param flags StrictMorphParams::FlagUseDefault or special flags.
         * 
         * @return Normalization status as StrictMorphErrors
         */
        StrictMorphErrors normalizeWord( WordNormalizerFunctor    &functor,
                                         const char               *word,
                                         size_t                    wordLength = static_cast<size_t>(-1),
                                         unsigned int              flags = StrictMorphParams::FlagUseDefault ) const;

        /** 
         * Requests dictionary file information.
         * 
         * 
         * @return NULL if failure or dictionary info structure
         */
        const StrictDictInfo *getDictInfo() const;
        
        /** 
         * Requests morphology default flags, given on creation.
         * 
         * 
         * @return Flags value or StrictMorphParams::FlagInvalid on error
         */
        unsigned int getFlags() const;
        
public:
        /* Status description static helper */
        static const char   *getStatusDescription( StrictMorphErrors status );
        
        /* Misc: internal morphology number accessor */
        unsigned int getMorphNumber() const;
        
public:
        /* Serialization / deserialization */
        template<typename PackType> StrictMorphErrors pack( const StrictLex &lex, PackType &result, BinaryData *additionalData = 0 ) const;
        template<typename PackType> StrictMorphErrors callWithUnpack( PackType packedLex, UnpackFunctor &functor, const BinaryData *additionalData = 0 ) const;

public:
        unsigned long getDataSerial() const;
        unsigned long getDataMajor() const;
        unsigned long getDataMinor() const;
        
public:
        bool isCreated() { return shadow != 0; }
        
public:
        StrictMorphShadow       *getShadow() { return shadow; }
        const StrictMorphShadow *getShadow() const { return shadow; }
        
protected:
        StrictMorphShadow *shadow;
};

#endif /* _STRICTMORPH_H_ */

