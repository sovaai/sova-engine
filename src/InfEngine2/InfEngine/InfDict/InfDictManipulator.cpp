#include "InfDictManipulator.hpp"
#include <InfEngine2/InfEngine/DLData.hpp>

#include <cstring>

InfEngineErrors InfDictManipulator::Save( char* aBuffer, unsigned int aBufferSize, unsigned int& aResultSize ) const
{
    if( !aBuffer )
        return INF_ENGINE_ERROR_INV_ARGS;

    if( aBufferSize < GetSize() )
        return INF_ENGINE_WARN_UNSUCCESS;

    aResultSize = GetSize();
    memcpy( aBuffer, (char*)vBuffer, GetSize() );

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfDictManipulator::GetString( unsigned int aNum, InfDictStringManipulator& aDictString, const DLDataRO * aDLData, bool aGetMainString )
{       
    // Если запрашиваемая строка находится в одном из подсловарей.
    if( aNum >= GetSelfLinesNum() )
    {
        if( !aDLData )
            return INF_ENGINE_ERROR_INV_ARGS;
        
        unsigned int line_shift = GetSelfLinesNum();
        for( unsigned int subdict_n = 0; subdict_n < GetSubdictsCount(); ++subdict_n )
        {
            // Загрузка подсловаря.
            unsigned int subdict_id = GetSubDictID( subdict_n );
            
            InfDictWrapManipulator subdict_manip;
            InfEngineErrors iee = aDLData->GetDictById( subdict_id, subdict_manip );
            if( INF_ENGINE_SUCCESS != iee )
                return iee;

            // Поиск запрашиваемой строки в очередном подсловаре.
            if( aNum < line_shift + subdict_manip.GetSelfLinesNum() )
                return subdict_manip.GetString( aNum - line_shift, aDictString, aGetMainString );

            // Коректировка номеров строк.
            line_shift += subdict_manip.GetSelfLinesNum();
        }

        // Некорректный номер строки.
        return INF_ENGINE_ERROR_INV_ARGS;
    }
    else
    {
        if( aGetMainString )
        {
            // Подмена номера подчинённой строки на номер управляющей.
            aNum = ((uint32_t*)vBuffer)[11+GetSelfLinesNum()+aNum];
        }

        return aNum < GetNum() ? aDictString.Attach( vBuffer+((uint32_t*)vBuffer)[11+aNum] ) : INF_ENGINE_ERROR_INV_ARGS; 
    }
}