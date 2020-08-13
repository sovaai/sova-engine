#include "NDocImage.hpp"
#include "InfEngine2/_Include/Errors.h"

InfEngineErrors NDocImage::Init( const TermsDocImageIterator & aIterator )
{
    TermsDocImageIterator head( aIterator );
    TermsDocImageIterator end( aIterator );
    unsigned int size = 0;
    
    while( !head.IsHead() )
    {        
        --head;
        ++size;
    }    
    while( !end.IsEnd() )
    {        
        ++end;
        ++size;
    }    
    ++size;
    
    vImage.resize( size );
    if( vImage.no_memory() )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;
    
    for( unsigned int pos = 0; pos < vImage.size(); ++pos )
    {
        vImage[pos] = Iterator( head, pos );
        ++head;
    }
    
    return INF_ENGINE_SUCCESS;
}