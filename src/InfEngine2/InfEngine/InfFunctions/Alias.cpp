#include "Alias.hpp"


Alias::Alias()
{
    vArgIndex.Create();
    vTextLength = 0;
    vResultType = UNKNOWN;
    vMinArgsCount = 0;
}

void Alias::SetType( ResultType aResultType )
{
    vResultType = aResultType;
}

InfEngineErrors Alias::SetName( const char * aName, unsigned int aNameLength )
{
    if( !aName )
        return INF_ENGINE_ERROR_INVALID_ARG;

    nlReturnCode rc = vName.assign( aName, aNameLength );
    if( rc == nlrcErrorNoFreeMemory)
        return INF_ENGINE_ERROR_NOFREE_MEMORY;
    else if ( rc != nlrcSuccess )
        return INF_ENGINE_ERROR_INTERNAL;

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors Alias::RegisterArgument( const char * aArgumentName, DLFunctionArgType aArgumentType,
                                        const char ** aValidValues, unsigned int aValidValuesCount,
                                        const char * aDefaultValue, unsigned int aDefaultValueLen )
{
    if( aArgumentType != DLFAT_TEXT && ( aValidValues || aValidValuesCount ) )
        return INF_ENGINE_ERROR_INTERNAL;
    
    if( vMinArgsCount < vArguments.size() && !aDefaultValue )
        return INF_ENGINE_ERROR_INVALID_ARG;
    
    if( vMinArgsCount == vArguments.size() && !aDefaultValue )
        ++vMinArgsCount;

    unsigned int Id;
    NanoLib::NameIndex::ReturnCode rt = vArgIndex.AddName( aArgumentName, Id );
    if( rt != NanoLib::NameIndex::rcSuccess )
        return INF_ENGINE_ERROR_INTERNAL;

    if( Id != vArguments.size() )
        return INF_ENGINE_ERROR_INTERNAL;

    vArguments.push_back( Argument( aArgumentType, aValidValues, aValidValuesCount, aDefaultValue, aDefaultValueLen ) );
    if( vArguments.no_memory() )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;
        
    return INF_ENGINE_SUCCESS;

}

InfEngineErrors Alias::AddTextItem( const char * aText )
{
    return AddTextItem( aText, strlen(aText) );
}


InfEngineErrors Alias::AddTextItem( const char * aText, unsigned int aTextLength )
{
    Item * NewItem = vItems.grow();
    if( !NewItem )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;

    NewItem->vType = Item::TEXT;
    NewItem->vText.vData = aText;
    NewItem->vText.vLength = aTextLength;

    vTextLength += aTextLength;

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors Alias::AddVariableItem( const char * aArgumentName, unsigned int aArgumentNameLength )
{
    Item * NewItem = vItems.grow();
    if( !NewItem )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;

    const unsigned int * Id = vArgIndex.Search( aArgumentName, aArgumentNameLength );
    if( !Id )
        return INF_ENGINE_ERROR_INVALID_ARG;

    NewItem->vType = Item::ARGUMENT;
    NewItem->vNum = *Id;

    return INF_ENGINE_SUCCESS;
}

Alias::ResultType Alias::GetResultType() const
{
    return vResultType;
}

const char * Alias::GetName() const
{
    return vName.ToConstChar();
}

unsigned int Alias::GetArgumentsCount() const
{
    return vArguments.size();
}

unsigned int Alias::GetMinArgumentsCount() const
{
    return vMinArgsCount;
}

const DLFunctionArgType * Alias::GetArgumentType( unsigned int aArgN ) const
{
    return aArgN < vArguments.size() ? &vArguments[aArgN].vType : nullptr;
}

const unsigned int * Alias::GetArgumentN( const char * aArgName) const
{
    return vArgIndex.Search( aArgName );
}

unsigned int Alias::GetArgumentValidValuesCount( unsigned int aArgN ) const
{
    return aArgN < vArguments.size() ? vArguments[aArgN].vValidValuesCount : 0;
}

const char ** Alias::GetArgumentValidValues( unsigned int aArgN ) const
{
    return aArgN < vArguments.size() ? vArguments[aArgN].vValidValues : nullptr;
}

const DLFunctionArgType * Alias::GetArgumentType( const char * aArgName ) const
{
    const unsigned int * Id = vArgIndex.Search( aArgName );
    if( !Id )
        return nullptr;

    return &vArguments[*Id].vType;
}

InfEngineErrors Alias::Apply( const char ** aArgVal, const unsigned int * aArgLen, unsigned int aArgCount,
                        nMemoryAllocator & aMemoryAllocator, char *& aResult, unsigned int & aResultLength ) const
{
    aResultLength = vTextLength;
    
    if( aArgCount < GetMinArgumentsCount() || aArgCount > GetArgumentsCount() )
        return INF_ENGINE_ERROR_INVALID_ARG;

    for( unsigned int arg_n = 0; arg_n < aArgCount; ++arg_n )
        if( !vArguments[arg_n].IsValid( aArgVal[arg_n], aArgLen[arg_n] ) )
            return INF_ENGINE_ERROR_INVALID_ARG;

    for( unsigned int item_n = 0; item_n < vItems.size(); ++item_n )
        if( vItems[item_n].vType == Item::ARGUMENT )
            aResultLength += vItems[item_n].vNum < aArgCount ? aArgLen[vItems[item_n].vNum] : vArguments[vItems[item_n].vNum].vDefaultValueLen;

    aResult = nAllocateObjects( aMemoryAllocator, char, aResultLength + 1 );
    if( !aResult )
        return INF_ENGINE_ERROR_NOFREE_MEMORY;

    char * pos = aResult;
    for( unsigned int item_n = 0; item_n < vItems.size(); ++item_n )
    {
        switch( vItems[item_n].vType )
        {
        case Item::TEXT:
            strncpy( pos, vItems[item_n].vText.vData, vItems[item_n].vText.vLength );
            pos += vItems[item_n].vText.vLength;
            break;

        case Item::ARGUMENT:
            if( vItems[item_n].vNum < aArgCount )
            {
                strncpy( pos, aArgVal[vItems[item_n].vNum], aArgLen[vItems[item_n].vNum] );
                pos += aArgLen[vItems[item_n].vNum];
            }
            else
            {
                strncpy( pos, vArguments[vItems[item_n].vNum].vDefaultValue, vArguments[vItems[item_n].vNum].vDefaultValueLen );
                pos += vArguments[vItems[item_n].vNum].vDefaultValueLen;
            }
            break;
        }
    }
    *pos = '\0';

    return INF_ENGINE_SUCCESS;
}
