
#include <stdio.h>
#include <stdlib.h>

#include <_include/_string.h>

#include <lib/aptl/avector.h>
#include <lib/aptl/OpenAddressingDict.h>
#include <ZTools/ZUtils/ScanPath.h>
#include <LingProc4/StringsReceiver.h>
#include <LingProc4/UCharSet/UCharSet.h>
#include <LingProc4/UTFCanonizer.h>

#include <unicode/utf8.h>
#include <unicode/ucnv.h>
#include <unicode/utext.h>

#ifndef _WIN32
#include <libgen.h>
#include <unistd.h>
#endif

#include "../common/file_raii.h"

enum MakeMode
{
    MODE_STOP,
    MODE_SUFF
};

class StringsContainer : public MultipleStringsReceiver
{
private:    
    struct Key { const avector<char> *chunk; size_t offset; Key() : chunk( 0 ), offset( 0 ) { } };

    struct Hasher : public OpenAddressingHasher<const char *>
    {
        uint32_t getHash( Key key ) const
        {
            return OpenAddressingHasher<const char *>::getHash( key.chunk->get_buffer() + key.offset );
        }
    };

    struct Comparator : public KeyEqualComparator<const char *>
    {
	bool areKeysEqual( Key firstKey, Key secondKey ) const
	{
            return KeyEqualComparator<const char *>::areKeysEqual(
                firstKey.chunk->get_buffer() + firstKey.offset,
                secondKey.chunk->get_buffer() + secondKey.offset );
	}
    };
    
    typedef OpenAddressingDict< Key, size_t, 1024, 1024, maxSizeTBits, Comparator, Hasher > StringsIndex;
    
public:
    StringsContainer() : MultipleStringsReceiver(), index() { }

    void commitString( size_t actualLength )
    {
        Key k;
        k.chunk  = &buffer;
        k.offset = threshold;
        
        MultipleStringsReceiver::commitString( actualLength );
        if ( hasErrorOccurred() )
            return;
        
        size_t *dataOffset = 0;
        
        StringsIndex::ResultStatus status = index.get( k, &dataOffset );
        if ( status == StringsIndex::RESULT_STATUS_NOT_FOUND )
        {
            status = index.access( k, &dataOffset );
            if ( status != StringsIndex::RESULT_STATUS_SUCCESS )
            {
                errorOccurred = true;
                return;
            }
            
            *dataOffset  = 1;
        }
        else if ( status == StringsIndex::RESULT_STATUS_SUCCESS )
        {
            *dataOffset += 1;
            MultipleStringsReceiver::pop();
        }
        else
            errorOccurred = true;
    }

    void getSorted( avector<const char *> &result ) const
    {
        for ( StringsIndex::Iterator it = index.first(); !index.isEnd( it ); it = index.next( it ) )
        {
            const Key *k = index.getKey( it );
            
            result.push_back( k->chunk->get_buffer() + k->offset );
            if ( result.no_memory() )
                return;
        }

        qsort( result.get_buffer(), result.size(), sizeof( const char * ), StringsContainer::sortProc );
    }

    static int sortProc( const void *s1, const void *s2 )
    {
        return strcmp( *(char **)s1, *(char **)s2 );
    }
    
private:
    StringsIndex index;
};

class Processor
{
public:
    Processor( bool _allowDigit,
               bool _allowHyphen,
               LangCodes _sourceLang,
               avector<UCharSet *> &_charsets,
               MakeMode _mode,
               StringsContainer &_strings ) :
        allowDigit( _allowDigit ),
        allowHyphen( _allowHyphen ),
        sourceLang( _sourceLang ),
        mode( _mode ),
        charsets( _charsets ),
        strings( _strings ),
        currentCharset( 0 ),
        utxt( 0 )
    {
        if ( charsets.size() > 0 )
            currentCharset = charsets[0];
    }

    virtual ~Processor()
    {
        if ( utxt != 0 )
            utext_close( utxt );
    }

    static int trigger( const char *pathname, const char * /* filename */, const char * /* relpath */, void *context )
    {
        Processor *proc = reinterpret_cast<Processor *>( context );
        return proc->process( pathname );
    }

    int process( const char *pathname )
    {
        if ( forceSkip( pathname ) )
            return 0;

        avector<char> contents;
        
        // load input file
        FILE_RAII f( fopen( pathname, "rb" ) );
        if ( f.f == 0 )
        {
            fprintf( stderr, "Cannot open list file: %s!\n", pathname );
            return 1;
        }

        if ( f.loadContents( contents, pathname ) != 0 )
            return 1;

        UErrorCode ustatus = U_ZERO_ERROR;

        int32_t signatureLength = 0;

        ucnv_detectUnicodeSignature( contents.get_buffer(), contents.size(), &signatureLength, &ustatus );

        const char *text     = contents.get_buffer() + signatureLength;
        size_t      textSize = contents.size() - signatureLength;
        
        UTFCanonizer         canonizer;
        UTFCanonizer::Status cstatus = canonizer.create();
        if ( likely(cstatus == UTFCanonizer::STATUS_OK) )
            cstatus = canonizer.canonize( text, textSize );
        if ( unlikely(cstatus != UTFCanonizer::STATUS_OK) )
        {
            fprintf( stderr, "Failed to perform utf canonization from '%s', error: %d\n", pathname, (int)cstatus );
            return 1;
        }

        text     = canonizer.getResultText();
        textSize = canonizer.getResultTextSize();
                        
        utxt = utext_openUTF8( utxt, text, textSize, &ustatus );
        if ( !U_SUCCESS(ustatus) )
        {
            fprintf( stderr, "Failed to create UTEXT from '%s', error: %d\n", pathname, (int)ustatus );
            return 1;
        }

        enum ParsingState
        {
            READING_WORD_START,
            READING_WORD_REST,
            READING_COMMENT,
            SKIPPING_REST,
            PROCESSING_WORD
        } state;

        state              = READING_WORD_START;
        int64_t  wordStart = 0;
        int64_t  wordEnd   = 0;
        int64_t  current   = 0;
        int64_t  lineIdx   = 0;
        size_t   i;
        char    *p;
        
        /* Parse the text */
        for ( UChar32 ch = utext_next32From( utxt, 0 ); ch >= 0; ch = utext_next32( utxt ) )
        {            
            for ( ;; )
            {
                switch ( state )
                {
                case READING_WORD_START:
                    if ( u_isspace( ch ) )
                        break;

                    if ( ch == static_cast<UChar32>('\n') )
                    {
                        lineIdx++;
                        break;
                    }
                    
                    wordStart = utext_getPreviousNativeIndex( utxt );
                    wordEnd   = utext_getNativeIndex( utxt );
                    
                    if ( ch == static_cast<UChar32>('/') )
                        state = READING_COMMENT;
                    else
                        state = READING_WORD_REST;
                    break;

                case READING_WORD_REST:
                    if ( ch == static_cast<UChar32>('\n') )
                    {
                        lineIdx++;
                        state = PROCESSING_WORD;
                        continue;
                    }

                    if ( ch == static_cast<UChar32>('/') )
                        state = READING_COMMENT;
                    else if ( !u_isspace( ch ) )
                        wordEnd = utext_getNativeIndex( utxt );
                    break;

                case READING_COMMENT:
                    if ( ch == static_cast<UChar32>('/') )
                        state = SKIPPING_REST;
                    else
                    {
                        state = READING_WORD_REST;
                        continue;
                    }
                    break;
                    
                case SKIPPING_REST:
                    if ( ch == static_cast<UChar32>('\n') )
                    {
                        lineIdx++;
                        state = PROCESSING_WORD;
                        continue;
                    }
                    break;

                case PROCESSING_WORD:
                    if ( wordStart < wordEnd )
                    {
                        current = utext_getPreviousNativeIndex( utxt );

                        ch = utext_next32From( utxt, wordStart );
                        if ( ch == static_cast<UChar32>('/') && utext_getNativeIndex( utxt ) == wordEnd )
                        {
                            utext_next32From( utxt, current );
                            state = READING_WORD_START;
                            break;
                        }
                        
                        /* Determine which charset to use */
                        if ( sourceLang == LNG_ALL )
                        {
                            for ( ch = utext_next32From( utxt, wordStart );
                                  utext_getNativeIndex( utxt ) <= wordEnd;
                                  ch = utext_next32( utxt ) )
                            {
                                currentCharset = 0;
                                
                                for ( i = 0; i < charsets.size(); i++ )
                                    if ( charsets[i]->IsLangChar( ch ) )
                                    {
                                        if ( currentCharset != 0 )
                                        {
                                            currentCharset = 0;
                                            break;
                                        }
                                        else
                                            currentCharset = charsets[i];
                                    }

                                if ( currentCharset != 0 )
                                    break;
                            }

                            if ( currentCharset == 0 )
                                currentCharset = charsets[ charsets.size() - 1 ];
                        }
                        else
                            currentCharset = charsets[0];

                        /* Allocate string */
                        i = 0;
                        p = strings.requestBuffer( (size_t)(wordEnd - wordStart) * 4 );
                        if ( strings.hasErrorOccurred() )
                        {
                            fprintf( stderr, "Failed to allocate string [%.*s], probably out of memory!\n",
                                     (int)(wordEnd - wordStart + 1), text + wordStart );
                            return 1;
                        }
                        
                        /* Check and construct word */
                        for ( ch = utext_next32From( utxt, wordStart );
                              utext_getNativeIndex( utxt ) <= wordEnd;
                              ch = utext_next32( utxt ) )
                        {
                            ch = currentCharset->ToLower( ch );

                            if ( ch == 0x00002019 ) // right single quotation mark
                                ch = 0x00000027;

                            if ( !currentCharset->IsLangChar( ch ) ) // non-language character
                            {
                                if ( allowDigit && currentCharset->IsDigit( ch ) )
                                    ; // OK
                                else if ( allowHyphen && ch == 0x0000002D )
                                    ; // OK
                                else
                                {
                                    fprintf( stderr, "Invalid language entry: [%.*s] in '%s' (char: 0x%08X, lang: 0x%X) at line %d\n",
                                             (int)(wordEnd - wordStart + 1),
                                             text + wordStart,
                                             pathname,
                                             ch,
                                             currentCharset->GetLang(),
                                             (int)(lineIdx - 1) );
                                    return 1;
                                }
                            }
                            else if ( ch == 0x00000027 &&
                                      mode == MODE_SUFF &&
                                      ( sourceLang == LNG_RUSSIAN || sourceLang == LNG_UKRAINIAN ) )
                            {
                                /* Apostroph in suffix */
                                fprintf( stderr, "Invalid apostroph in suffix: [%.*s] at line %d\n",
                                         (int)(wordEnd - wordStart + 1), text + wordStart, (int)(lineIdx - 1) );
                                return 1;
                            }

                            U8_APPEND_UNSAFE( p, i, ch );
                        }

                        strings.commitString( i );
                        if ( strings.hasErrorOccurred() )
                        {
                            fprintf( stderr, "Failed to store string [%s] at line %d, probably out of memory!\n", p, (int)(lineIdx - 1) );
                            return 1;
                        }

                        utext_next32From( utxt, current );
                    }

                    state = READING_WORD_START;
                    break;
                };

                break;
            }
        }
        
        return 0;
    }

    virtual bool forceSkip( const char */* pathname */ )
    {
        return false;
    }
    
protected:
    bool                 allowDigit;
    bool                 allowHyphen;
    LangCodes            sourceLang;
    MakeMode             mode;
    avector<UCharSet *> &charsets;
    StringsContainer    &strings;
    UCharSet            *currentCharset;
    UText               *utxt;
};

class ProcessorMorphList : public Processor
{
public:
    ProcessorMorphList( bool _allowDigit,
                        bool _allowHyphen,
                        LangCodes _sourceLang,
                        avector<UCharSet *> &_charsets,
                        MakeMode _mode,
                        StringsContainer &_strings ) :
        Processor( _allowDigit,
                   _allowHyphen,
                   _sourceLang,
                   _charsets,
                   _mode,
                   _strings )
    {
    }

    bool forceSkip( const char *pathname )
    {
        const char *file;
        
#ifdef _WIN32
        char buffer[_MAX_FNAME];
        file = buffer;
        buffer[0] = 0;
        _splitpath( pathname, 0, 0, buffer, 0 );
#else
        file = basename( const_cast< char * >(pathname) );
#endif

        if ( currentCharset == 0 )
            return true;
        
        switch ( mode )
        {
        case MODE_STOP:
            return ( currentCharset->findSubstring( file, static_cast<size_t>(-1), "stop", 4, false ) == 0 );

        case MODE_SUFF:
            return ( currentCharset->findSubstring( file, static_cast<size_t>(-1), "suff", 4, false ) == 0 );
        }

        return true;
    }
};

int main( int argc, char **argv )
{
    const char *usage =
        "Usage:\n  UMakeCArray [-d] [-y] template [-i:|-l:]list outfile en|ru|ua|de|fr|es|vi|er stop|suff\n"
        "  -d - allow digits inside words\n"
        "  -y - allow hyphens inside words\n"
        "  -i:list - process entries from morphlist-file (stop and suff combined)\n"
        "  -l:list - process entries from list-file, single entry otherwise\n"
        "  Wildcards are OK both at commandline and in the list-file\n"
        "  en|ru|ua|de|fr|es|vi|er - Eng, Rus, Ukr, Ger, French, Spanish, Vietnamese, Eng+Rus source\n";

    if (argc < 6)
    {
        fprintf( stderr, "%s", usage );
        return 1;
    }

    bool                allowDigit  = false;
    bool                allowHyphen = false;
    LangCodes           sourceLang  = LNG_UNKNOWN;
    MakeMode            mode;

    struct CharSetsRAII : public avector<UCharSet *>
    {
        ~CharSetsRAII()
        {
            for ( size_t i = 0; i < size(); i++ )
                delete avector<UCharSet *>::operator[]( i );
        }
    } charsets;
    
    // optional keys
    int argshift = 0;
    for ( int i = 0; i < 3; i++ )
    {
        if ( strcmp( argv[i + 1], "-d" ) == 0 )
        {
            allowDigit = true;
            argshift++;
        }
        else if ( strcmp( argv[i + 1], "-y" ) == 0 )
        {
            allowHyphen = true;
            argshift++;
        }
    }

    argv += argshift;
    argc -= argshift;
    
    const char *lang = argv[4];
    if ( strcmp( lang, "en" ) == 0 )
        sourceLang = LNG_ENGLISH;
    else if ( strcmp( lang, "ru" ) == 0 )
        sourceLang = LNG_RUSSIAN;
    else if ( strcmp( lang, "ua" ) == 0 )
        sourceLang = LNG_UKRAINIAN;
    else if ( strcmp( lang, "de" ) == 0 )
        sourceLang = LNG_GERMAN;
    else if ( strcmp( lang, "fr" ) == 0 )
        sourceLang = LNG_FRENCH;
    else if ( strcmp( lang, "es" ) == 0 )
        sourceLang = LNG_SPANISH;
    else if ( strcmp( lang, "vi" ) == 0 )
        sourceLang = LNG_VIETNAMESE;
    else if ( strcmp( lang, "eu" ) == 0 )
        sourceLang = LNG_EURO;
    else if ( strcmp( lang, "cy" ) == 0 )
        sourceLang = LNG_CYRILLIC;
    else if ( strcmp( lang, "er" ) == 0 )
        sourceLang = LNG_ALL;
    else
    {
        fprintf( stderr, "%s", usage );
        return 1;
    }

    if ( strcmp( argv[5], "stop" ) == 0 )
        mode = MODE_STOP;
    else if ( strcmp( argv[5], "suff" ) == 0 )
        mode = MODE_SUFF;
    else
    {
        fprintf( stderr, "%s", usage );
        return 1;
    }

    if ( sourceLang == LNG_ALL )
    {
        charsets.push_back( new UCharSet( LNG_RUSSIAN ) );
        charsets.push_back( new UCharSet( LNG_ENGLISH ) );
    }
    else
        charsets.push_back( new UCharSet( sourceLang ) );

    struct ProcessorRAII
    {
        Processor *proc;

        ProcessorRAII() : proc( 0 ) { }
        ~ProcessorRAII() { if ( proc != 0 ) delete proc; }
    } proc;

    StringsContainer strings;
    
    if ( !strncmp( argv[2], "-l:", 3 ) )
    {
        proc.proc = new Processor( allowDigit, allowHyphen, sourceLang, charsets, mode, strings );

        if ( ScanFileList( argv[2] + 3, Processor::trigger, proc.proc ) != 0 )
            return 1;
    }
    else if ( !strncmp( argv[2], "-i:", 3 ) )
    {
        proc.proc = new ProcessorMorphList( allowDigit, allowHyphen, sourceLang, charsets, mode, strings );

        if ( ScanFileList( argv[2] + 3, Processor::trigger, proc.proc ) != 0 )
            return 1;
    }
    else
    {
        proc.proc = new Processor( allowDigit, allowHyphen, sourceLang, charsets, mode, strings );
        
        if ( ScanPath( argv[2], false, Processor::trigger, NULL, proc.proc ) != 0 )
            return 1;
    }

    avector<const char *> processedStrings;

    strings.getSorted( processedStrings );
    if ( processedStrings.no_memory() )
    {
        fprintf( stderr, "Failed to allocate results buffer, probably out of memory!\n" );
        return 1;
    }

    if ( processedStrings.size() == 0 )
    {
        fprintf( stderr, "Empty list!\n" );
        return 1;
    }
    
    if ( mode == MODE_SUFF && processedStrings.size() > 254)
    {
        fprintf( stderr, "Too many suffixes: %d, maximum 254!\n", (int)processedStrings.size() );
        return 1;
    }

    /* Load template */

    FILE_RAII fTemplate( fopen( argv[1], "rb" ) );
    if ( fTemplate.f == 0 )
    {
        fprintf( stderr, "Cannot open template file '%s'!\n", argv[1] );
        return 1;
    }

    avector<char> templateContents;
    
    if ( fTemplate.loadContents( templateContents, argv[1] ) != 0 )
            return 1;

    int32_t    signatureLength = 0;
    UErrorCode ustatus         = U_ZERO_ERROR;
    ucnv_detectUnicodeSignature( templateContents.get_buffer(), templateContents.size(), &signatureLength, &ustatus );
    
    FILE_RAII fOut( fopen( argv[3], "wb" ) );
    if ( fOut.f == 0 )
    {
        fprintf( stderr, "Cannot create output file '%s'!\n", argv[3] );
        return 1;
    }
    
    /* Output */
    const char *pattern      = "/* TO_INSERT */";
    const char *patternStart = charsets[0]->findSubstring(
        templateContents.get_buffer() + signatureLength,
        templateContents.size() - signatureLength,
        pattern,
        strlen(pattern) );

    if ( patternStart == 0 )
    {
        fprintf( stderr, "Invalid template (no replace pattern found)!\n" );
        return 1;
    }

    size_t bytes   = patternStart - (templateContents.get_buffer() + signatureLength);
    size_t written = fwrite( templateContents.get_buffer() + signatureLength,
                             1,
                             bytes,
                             fOut.f );
    if ( written != bytes )
    {
        fprintf( stderr, "Failed to write out file!" );
        return 1;
    }

    for ( size_t i = 0; i < processedStrings.size(); i++ )
    {
        fputc( '"', fOut.f );
        for ( const char *p = processedStrings[i]; *p != '\0'; p++ )
        {
            if ( *p >= 0 )
                fputc( *p, fOut.f );
            else
                fprintf( fOut.f, "\\x%02X\"\"", static_cast<unsigned char>(*p) );
        }
        fprintf( fOut.f, "\" /* %s */", processedStrings[i] );
        if ( i < processedStrings.size() - 1 )
            fprintf( fOut.f, ",\n" );
    }

    bytes = templateContents.size() - signatureLength - bytes - strlen( pattern );
    written = fwrite( templateContents.get_buffer() + signatureLength + written + strlen( pattern ),
                      1,
                      bytes,
                      fOut.f );
    if ( written != bytes )
    {
        fprintf( stderr, "Failed to write out file!" );
        return 1;
    }
    
    fprintf( stderr, "%s - %u entries.\n", argv[3], (unsigned)processedStrings.size() );
    
    return 0;
}

