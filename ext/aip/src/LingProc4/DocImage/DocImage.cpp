
#include "DocImage.h"
#include "DocImageIterator.h"

void DocImage::Reset()
{
    image.clear();
    words.clear();
    compoundLexPool.clear();
    compoundChainsPool.clear();
    markup.clear();

    primLang = suppLang = LNG_UNKNOWN;
    docText = 0;
}

LEXID DocImage::GetLex( size_t wordNo, size_t homoNo ) const
{
    if ( wordNo < words.size() )
        return image[ words[ wordNo ].imgIndex + homoNo ];
    else
        return LEXINVALID;
}

size_t DocImage::GetHomoNumber( size_t wordNo ) const
{
    if ( wordNo < words.size() )
        return words[ wordNo ].imgLength;
    else
        return static_cast<size_t>(-1);
}

LEXID DocImage::GetCompoundLex( size_t wordNo, size_t chainNo, size_t positionNo, size_t homoNo ) const
{
    return *( GetCompoundWordImage( wordNo, chainNo, positionNo ) + homoNo );
}

const LEXID *DocImage::GetCompoundWordImage( size_t wordNo, size_t chainNo, size_t positionNo ) const
{
    size_t lexPoolOffset = compoundChainsPool[ words[ wordNo ].chainsIndex + chainNo ].chainOffset;

    /* Move to the required position */
    for ( ; positionNo > 0; positionNo-- )
        lexPoolOffset += static_cast<size_t>( compoundLexPool[ lexPoolOffset ] ) + 1;

    return compoundLexPool.get_buffer() + lexPoolOffset + 1;
}

size_t DocImage::GetCompoundHomoNumber( size_t wordNo, size_t chainNo, size_t positionNo ) const
{
    size_t lexPoolOffset = compoundChainsPool[ words[ wordNo ].chainsIndex + chainNo ].chainOffset;
        
    /* Move to the required position */
    for ( ; positionNo > 0; positionNo-- )
        lexPoolOffset += static_cast<size_t>( compoundLexPool[ lexPoolOffset ] ) + 1;

    return static_cast<size_t>( compoundLexPool[ lexPoolOffset ] );
}

LEXID DocImage::GetLex( const DocImageIterator &it, size_t homoNo ) const
{
    const LEXID *lexPtr = GetLexPtr( it, homoNo );
    if ( lexPtr == 0 )
        return LEXINVALID;

    return *lexPtr;
}

size_t DocImage::GetHomoNumber( const DocImageIterator &it ) const
{
    size_t homoNum = GetCompoundHomoNumber( it );
    if ( !it.IsHead() && !it.IsEnd() && !it.IsMiddleCompoundPos() )
        homoNum += GetHomoNumber( it.GetWordPos() );
    return homoNum;
}

size_t DocImage::GetCompoundHomoNumber( const DocImageIterator &it ) const
{
    if( it.IsEnd() || it.IsHead() )
        return 0;

    size_t homoNum = 0;
    size_t wordPos = it.GetWordPos();
    if( it.IsCompoundPos() )
    {
        size_t compPos = it.GetCompoundPos();
        if( compPos < GetCompoundChainLength( wordPos, 0 ) )
            homoNum = GetCompoundHomoNumber( wordPos, 0, compPos );
    }
    return homoNum;
}

const LEXID *DocImage::GetLexPtr( const DocImageIterator &it, size_t homoNo ) const
{
    if ( it.IsEnd() || it.IsHead() )
        return 0;
    
    const LEXID *lexPtr = 0;
    size_t wordPos = it.GetWordPos();
    if ( it.IsCompoundPos() )
    {
        size_t compPos = it.GetCompoundPos();
        if( compPos < GetCompoundChainLength( wordPos, 0 ) )
        {
            size_t compHomoNum = GetCompoundHomoNumber( wordPos, 0, compPos );
            if( homoNo < compHomoNum )
                lexPtr = GetCompoundWordImage( wordPos, 0, compPos ) + homoNo;
            else if ( it.IsLastCompoundPos() && homoNo - compHomoNum < GetHomoNumber( wordPos ) )
                lexPtr = GetWordImage( wordPos ) + ( homoNo - compHomoNum );
        }
    }
    else
    {
        if ( homoNo < GetHomoNumber( wordPos ) )
            lexPtr = GetWordImage( wordPos ) + homoNo;
    }
    return lexPtr;
}

const StrictWordFormsSet *DocImage::GetCompoundWordForms( size_t wordNo, size_t chainNo, size_t positionNo ) const
{
    if ( compoundForms.size() == 0 )
        return 0;
    
    size_t lexPoolOffset = compoundChainsPool[ words[ wordNo ].chainsIndex + chainNo ].chainOffset;

    /* Move to the required position */
    for ( ; positionNo > 0; positionNo-- )
        lexPoolOffset += static_cast<size_t>( compoundLexPool[ lexPoolOffset ]) + 1;

    return compoundForms.get_buffer() + lexPoolOffset + 1;
}

const StrictWordFormsSet *DocImage::GetWordForms( const DocImageIterator &it, size_t homoNo ) const
{
    if ( it.IsEnd() || it.IsHead() )
        return NULL;
    
    if ( imageForms.size() == 0 )
        return NULL;
    
    const StrictWordFormsSet *formPtr = NULL;
    
    size_t wordPos = it.GetWordPos();
    if ( it.IsCompoundPos() )
    {
        size_t compPos = it.GetCompoundPos();
        if ( compPos < GetCompoundChainLength( wordPos, 0 ) )
        {
            size_t compHomoNum = GetCompoundHomoNumber( wordPos, 0, compPos );
            if ( homoNo < compHomoNum )
                formPtr = GetCompoundWordForms( wordPos, 0, compPos ) + homoNo;
            else if ( it.IsLastCompoundPos() && homoNo - compHomoNum < GetHomoNumber( wordPos ))
                formPtr = GetWordForms( wordPos ) + ( homoNo - compHomoNum );
        }
    }
    else
    {
        if ( homoNo < GetHomoNumber( wordPos ) )
            formPtr = GetWordForms( wordPos ) + homoNo;
    }

    if ( formPtr == 0 )
        return 0;
    
    return formPtr;
}

