#ifndef _UCHARSET_H_
#define _UCHARSET_H_

#include <stdlib.h>

#include <_include/_inttype.h>
#include <_include/language.h>
#include <_include/cc_compat.h>

#include <unicode/uchar.h>
#include <unicode/utext.h>
#include <unicode/utf16.h>

#include "../IndirectTable.h"

class UCharSet
{
protected:
    // copy prevention
    UCharSet( const UCharSet & ) { abort(); }
    UCharSet &operator=( const UCharSet & ) { abort(); return *this; }

public:
    // constants
    enum tune_modes
    {
        tmApostrophOnlyPunct = 0x00000001u,
        tmRQuotMarkPunct     = 0x00000002u,
        tmApostrophPunct     = tmApostrophOnlyPunct | tmRQuotMarkPunct,
        tmHyphenWordPart     = 0x00000004u,
        tmTildeWordPart      = 0x00000008u,
        tmWordPartAsNative   = 0x00000010u,
        tmSpaceNative        = 0x00000020u,
        tmWordPartWoEdge     = 0x00000040u,
        tmNumberSignWordPart = 0x00000080u,
        tmAtSignWordPart     = 0x00000100u,
        tmUnderlineWordPart  = 0x00000200u
    };

    // table flags and constants
    enum flags
    {
        // Standard char flags
        upper     = 0x00000001,  //  Upper case letter
        lower     = 0x00000002,  //  Lower case letter
        digit     = 0x00000004,  //  Digit [0-9]
        space     = 0x00000008,  //  HT, LF, CR, VT, FF
        punct     = 0x00000010,  //  Punctuation character
        cntrl     = 0x00000020,  //  Control character
        blank     = 0x00000040,  //  Space char
        xdigit    = 0x00000080,  //  Hexadecimal digit

        // Multibyte char flags
        leadbyte  = 0x00008000,  //  Multibyte leadbyte
        alphaspec = 0x00000100,  //  Alphabetic character (special)

        // Specific flags
        accent    = 0x00010000,  //  Accented character
        ligature  = 0x00020000,  //  Ligature character: oe
        wordpart  = 0x00040000,  //  Character can be part of a word
        symbol    = 0x00080000,  //  Symbol
        hyphen    = 0x00100000,  //  Hyphen
        ignore    = 0x00200000,  //  Ignore as part of word: soft hyphen

        // native character for current charset language
        native    = 0x00400000,

        internal_wordpart = 0x00800000,

        // Complex flags
        alpha   = upper|lower|alphaspec,      //  Any alphabetic char
        alnum   = upper|lower|alphaspec|digit,//  Any alpanumeric char
        graph   = upper|lower|alphaspec|digit|punct|symbol,
                                  //  Any graphics
        print   = upper|lower|alphaspec|digit|punct|symbol|blank,
                                  //  Is printable character
        white   = space|blank|cntrl           //  Any space or blank char
    };

    // construction and initialization
    UCharSet() :
        lang( LNG_INVALID ),
        tuneMode( 0 ),
        charProps(),
        charLower(),
        utxtA( 0 ),
        utxtB( 0 ) { }

    UCharSet( LangCodes _lang ) :
        lang( LNG_INVALID ),
        tuneMode( 0 ),
        charProps(),
        charLower(),
        utxtA( 0 ),
        utxtB( 0 ) { Init( _lang ); }

    virtual ~UCharSet()
    {
        if ( likely(utxtA != 0) )
            utext_close( utxtA );
        if ( likely(utxtB != 0) )
            utext_close( utxtB );
    }

    bool IsValid() const { return lang != LNG_INVALID; }
    int Init( LangCodes lang ); /* returns 0 on success, -1 on error */
    int Tune( unsigned int tuneMode ); // returns 0 on success, -1 on error

    // charset properties
    LangCodes GetLang() const { return lang; }

    bool IsUpper( uint32_t c )  const { return ((charProps.getValue( c ) & upper)  != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_UPPERCASE );    }
    bool IsLower( uint32_t c )  const { return ((charProps.getValue( c ) & lower)  != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_LOWERCASE );    }
    bool IsXdigit( uint32_t c ) const { return ((charProps.getValue( c ) & xdigit) != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_POSIX_XDIGIT ); }
    bool IsSpace( uint32_t c )  const { return ((charProps.getValue( c ) & space)  != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_WHITE_SPACE );  }
    bool IsGraph( uint32_t c )  const { return ((charProps.getValue( c ) & graph)  != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_POSIX_GRAPH );  }
    bool IsPrint( uint32_t c )  const { return ((charProps.getValue( c ) & print)  != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_POSIX_PRINT );  }
    bool IsAlpha( uint32_t c )  const { return ((charProps.getValue( c ) & alpha)  != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_ALPHABETIC );   }
    bool IsAlnum( uint32_t c )  const { return ((charProps.getValue( c ) & alnum)  != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_POSIX_ALNUM );  }
    bool IsHyphen( uint32_t c ) const { return ((charProps.getValue( c ) & hyphen) != 0) || u_hasBinaryProperty( static_cast<UChar32>(c), UCHAR_HYPHEN );       }

    bool IsDigit( uint32_t c )  const { return ((charProps.getValue( c ) & digit) != 0)  || u_isdigit( static_cast<UChar32>(c) ); }
    bool IsPunct( uint32_t c )  const { return ((charProps.getValue( c ) & punct) != 0)  || u_ispunct( static_cast<UChar32>(c) ); }
    bool IsCntrl( uint32_t c )  const { return ((charProps.getValue( c ) & cntrl) != 0)  || u_iscntrl( static_cast<UChar32>(c) ); }
    bool IsInternalOnly( uint32_t c )  const { return ((charProps.getValue( c ) & internal_wordpart) != 0); }

    bool IsLigature( uint32_t c )   const { return (charProps.getValue( c ) & ligature) != 0; }
    bool IsBlank( uint32_t c )      const { return (charProps.getValue( c ) & blank)    != 0; }
    bool IsAccented( uint32_t c )   const { return (charProps.getValue( c ) & accent)   != 0; }
    bool IsWordIgnore( uint32_t c ) const { return (charProps.getValue( c ) & ignore)   != 0; }
    bool IsWord( uint32_t c )       const { return (charProps.getValue( c ) & wordpart) != 0; }
    bool IsLangChar( uint32_t c )   const { return (charProps.getValue( c ) & native)   != 0; }

    uint32_t ToUpper( uint32_t c )   const { return static_cast<uint32_t>( u_toupper( static_cast<UChar32>(c) ) ); }
    uint32_t ToLower( uint32_t c )   const { return static_cast<uint32_t>( u_tolower( static_cast<UChar32>(c) ) ); }

    uint32_t ToLoPunct( uint32_t c ) const
    {
        if ( !IsPunct( c ) )
            return c;

        uint32_t m = charLower.getValue( c );
        return m == 0 ? c : m;
    }

    uint32_t ToLoAccent( uint32_t c) const
    {
        if ( !IsAccented( c ) )
            return c;

        uint32_t m = charLower.getValue( c );
        return m == 0 ? c : m;
    }

    int enumerateKnownSymbols( IndirectTable::Enumerator &enumerator ) const
    {
        return charProps.enumerate( enumerator );
    }

    void tunePropsAdd( uint32_t ch, flags flag )
    {
        charProps.setValue( ch, ( charProps.getValue( ch ) | flag ) );
    }

    void tunePropsClear( uint32_t ch, flags flag )
    {
        charProps.setValue( ch, ( charProps.getValue( ch ) & ( ~flag ) ) );
    }
    
    void strlwr( const char *word, size_t wordSize, char *outBuffer, size_t &outBufferOffset, size_t outBufferSize ) const
    {
        for ( size_t wordOffset = 0; wordOffset < wordSize && outBufferOffset < outBufferSize; )
        {
            uint32_t ch;
            U8_NEXT_UNSAFE( word, wordOffset, ch );
            ch = ToLower( ch );
            U8_APPEND_UNSAFE( outBuffer, outBufferOffset, ch );
        }
    }

    void strupr( const char *word, size_t wordSize, char *outBuffer, size_t &outBufferOffset, size_t outBufferSize ) const
    {
        for ( size_t wordOffset = 0; wordOffset < wordSize && outBufferOffset < outBufferSize; )
        {
            uint32_t ch;
            U8_NEXT_UNSAFE( word, wordOffset, ch );
            ch = ToUpper( ch );
            U8_APPEND_UNSAFE( outBuffer, outBufferOffset, ch );
        }
    }

    const char *findSubstring( const char *string,
                               size_t stringSize,
                               const char *pattern,
                               size_t patternSize,
                               bool caseSensitive = true );

protected:
    LangCodes     lang;
    unsigned int  tuneMode;

    IndirectTable charProps;
    IndirectTable charLower;

private:
    UText              *utxtA;
    UText              *utxtB;
};


#endif /* _UCHARSET_H_ */

