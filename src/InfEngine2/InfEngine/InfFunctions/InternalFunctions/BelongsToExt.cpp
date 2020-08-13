#include "InternalFunction.hpp"
#include "BelongsToExt.hpp"
#include "InfEngine2/_Include/Errors.h"

using namespace InternalFunctions;

const DLFucntionArgInfo BelongsToExt::ARGS[] = {
    { DLFAT_EXTENDED, 0 },
    { DLFAT_TEXT, 0 },
    { DLFAT_EXTENDED, 0 },
};
    
const DLFucntionArgInfo BelongsToExt::VAR_ARGS = { DLFAT_EXTENDED, 0 };
    
const DLFunctionInfo BelongsToExt::DLF_INFO = {
    "BelongsToExt",
    "",
    "Check if word or phrase obtained by first argument interpretation contained in specified dictionary or dictionaries. Return first line of matched dictionary in given variable",
    DLFRT_BOOL,
    3,
    BelongsToExt::ARGS,
    { 0, 0, 0 },
    { 0, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    &VAR_ARGS,
};

ExtICoreRO * BelongsToExt::vBase = nullptr;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;


InfEngineErrors BelongsToExt::Apply( const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * aMemoryAllocator, 
        FunctionContext * aFunctionContext)
{    
	if( !vBase )
        return INF_ENGINE_ERROR_FUNC_NOT_INITIALIZED;
    
    if( !aArgs[0] || !aArgs[1] || !aArgs[2] )
        return INF_ENGINE_ERROR_ARGC;
    
    bool result = false;
    InfEngineErrors iee = BelongsTo::_apply( aArgs[0], aArgs + 2, aMemoryAllocator, aFunctionContext->vSession, vDictMatcher, vDocImage, vDocMessage, vDictFilter );
    if( INF_ENGINE_SUCCESS != iee )
        return iee;
    
    result = vDictMatcher.GetTermsNum();
    unsigned int doc_len = vDocImage.Size();
    
    // Подготовка результата.
    TermsStorageRO::CommonIterator iter; 
    bool success = false;
    if( result )
    {                
        // Ищем подошедший словарь.
        for( unsigned int term_n = 0; !success && term_n < vDictMatcher.GetTermsNum(); ++term_n )
        {
            // Получение вхождений найденного термина.
            vDictMatcher.SetTermsStorage( &vBase->GetIndexBase().GetIndexDBase().GetTermsStorage() );
            if( INF_ENGINE_SUCCESS != ( iee = vDictMatcher.SearchAttrsByN( term_n, iter ) ) )
                return iee;
            
            // Получение найденного термина.
            CommonMatcher::FoundTerm * term = vDictMatcher.GetFoundTerm( term_n );
            // Длина найденного термина.
            unsigned int term_len = term->vCoords.vLastPos.GetPos() - term->vCoords.vFirstPos.GetPos() + 1;
            
            // Поиск полного совпадения аргумента функции со строкой словаря.
            while( iter.Item() )
            {
                // Строка словаря должна состоять из единственного термина.
                if( iter.Item()->GetFlagFirstTerm() && iter.Item()->GetFlagLastTerm() )
                {
                    // Аргумент функции должен совпадать со строкой словаря целиком.                    
                    if( term_len == doc_len )
                    {
                        // Найдено полное совпадение.
                        success = true;
                        break;
                    }
                }
                // Переход к следующему вхождению термина.
                iter.Next();
            }
        }
    }
    
    if( success )
    {
        // Загружаем подошедший словарь.
        unsigned int dict_id = iter.Item()->GetId();
        
        InfDictWrapManipulator dict_manip;
        if( INF_ENGINE_SUCCESS != ( iee = vBase->GetDLData().GetDictById( dict_id, dict_manip ) ) )
            return iee;
        
        // Выбираем из подошедшего словаря первую строку.
        if( dict_manip.GetNum() )
        {
            InfDictStringManipulator dict_string;
            if( INF_ENGINE_SUCCESS != ( iee = dict_manip.GetString( 0, dict_string ) ) )
                return iee;
            
            unsigned int var_name_len = strlen( aArgs[1] );
            char* var_name = static_cast<char *>( aMemoryAllocator->Allocate( var_name_len + 1 ) );
            if( !var_name )
                return INF_ENGINE_ERROR_NOFREE_MEMORY;
            strcpy( var_name, aArgs[1] );
            
            if( nlrcSuccess != TextFuncs::ToLower( var_name, var_name_len ) )
                return INF_ENGINE_ERROR_NANOLIB;
            
			Vars::Id var_value_id = aFunctionContext->vSession->GetValueId( var_name, var_name_len );
			if( !var_value_id )
                return INF_ENGINE_WARN_UNSUCCESS;
            
            // Записываем первую строку из подошедшего словаря в заданную переменную.
            unsigned int orign_len = strlen(dict_string.GetOriginal() );
            if( INF_ENGINE_SUCCESS != ( iee = aFunctionContext->vSession->SetTextValue(  var_value_id, dict_string.GetOriginal(), orign_len ) ) )
                return iee;                
            
            // Записываем в ответ информацию о изменении переменной в сессии.
            if( INF_ENGINE_SUCCESS != ( iee = aFunctionContext->vAnswer->AddInstruct( var_value_id, dict_string.GetOriginal(), orign_len, true ) ) )
            {
                switch( iee )
                {
                    case INF_ENGINE_ERROR_INV_ARGS:
						ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get dict's string." );
                    case INF_ENGINE_ERROR_NOFREE_MEMORY:
						ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
                    default:
						ReturnWithError( INF_ENGINE_ERROR_FAULT, "%s: %d", INF_ENGINE_STRING_ERROR_INTERNAL, iee );
                }
            }
        }        
        
        aResult = CTRUE;
        aLength = CTRUE_LEN;
    }
    else
    {
        aResult = CFALSE;
        aLength = CFALSE_LEN;
    }
    
    return INF_ENGINE_SUCCESS;
}
