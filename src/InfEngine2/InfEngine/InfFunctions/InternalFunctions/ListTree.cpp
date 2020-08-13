#include "InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"
#include "ListTree.hpp"
#include <algorithm>

using namespace InternalFunctions;

const DLFucntionArgInfo ListTree::ARGS[] = {
    { DLFAT_TEXT, 0},
};

const DLFunctionInfo ListTree::DLF_INFO = {
    "ListTree",
    "",
    "Print given tree leaves as list.",
    DLFRT_TEXT,
    1,
    ARGS,
    { 3, 0, 0 },
    { 3, 0, 0 },
    DLFO_CACHE,
    DLFunctionInterfaceVersion,
    5,
    InfDataProtocolVersion,
    nullptr
};

ExtICoreRO * ListTree::vBase = nullptr;

InfEngineErrors ListTree::AddLeafToAnswer( const  Vars::Vector::VarStruct & aLeaf, iAnswer & aAnswer )
{   
    // Получение полей.
    Vars::Vector::VarValue state_val( nullptr, nullptr );
    Vars::Vector::VarValue name_val( nullptr, nullptr );
    Vars::Vector::VarValue userlink_val( nullptr, nullptr );
    Vars::Vector::VarValue url_val( nullptr, nullptr );
    Vars::Vector::VarValue new_window_val( nullptr, nullptr );    
    InfEngineErrors iee = aLeaf.GetFieldValue( "state", 5, state_val );
    if( INF_ENGINE_SUCCESS != iee ||
            INF_ENGINE_SUCCESS != ( iee = aLeaf.GetFieldValue( "name", 4, name_val ) ) ||
            INF_ENGINE_SUCCESS != ( iee = aLeaf.GetFieldValue( "userlink", 8, userlink_val ) ) ||
            INF_ENGINE_SUCCESS != ( iee = aLeaf.GetFieldValue( "url", 3, url_val ) ) ||
            INF_ENGINE_SUCCESS != ( iee = aLeaf.GetFieldValue( "new_window", 10, new_window_val ) ) )
    {
        if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
    }
    
    // Если поле name пустое или отсутствует.
    if( !name_val )
        return INF_ENGINE_SUCCESS;
    
    // Если флаг state отсутствует или установлен в ноль.
    if( !state_val.AsText().GetValue() || strcmp( state_val.AsText().GetValue(), "1" ) )
        return INF_ENGINE_SUCCESS;
    
    // Ответ в виде гипер-сслыки.
    if( url_val.AsText().GetValue() )
    {   
        unsigned int name_len = 0;
        const char * name = name_val.AsText().GetValue( name_len );        
        unsigned int url_len = 0;
        const char * url = url_val.AsText().GetValue( url_len );
        unsigned int target_len = 0;
        const char * target = new_window_val.AsText().GetValue( target_len );
        if( INF_ENGINE_SUCCESS != ( iee = aAnswer.AddHref( name, name_len, target, target_len, url, url_len , true ) ) )
			ReturnWithError( iee, "Failed to represent leaf as a string" );
    }
    // Ответ в виде ссылки.
    else if( userlink_val.AsText().GetValue() && !strcmp( userlink_val.AsText().GetValue(), "1" ) )
    {
        unsigned int name_len = 0;
        const char * name = name_val.AsText().GetValue( name_len );                        
		if( INF_ENGINE_SUCCESS != ( iee = aAnswer.AddInf( name, name_len, nullptr, 0, true ) ) )
			ReturnWithError( iee, "Failed to represent leaf as a string" );
    }
    // Ответ в виде текста.
    else
    {
        unsigned int name_len = 0;
        const char * name = name_val.AsText().GetValue( name_len );                        
        if( INF_ENGINE_SUCCESS != ( iee = aAnswer.AddText( name, name_len, true ) ) )
			ReturnWithError( iee, "Failed to represent leaf as a string" );
    }    
    
    return INF_ENGINE_SUCCESS;
}

struct FieldInfo
{
    FieldInfo( unsigned int aFieldN = 0, unsigned int aPos = 0, bool aIsSubtree = false )
            : vFieldN( aFieldN )
            , vPos( aPos )
            , vIsSubtree( aIsSubtree )
    {}

    bool operator ()( const FieldInfo & a, const FieldInfo & b )
            { return a.vPos < b.vPos; }

    unsigned int vFieldN;        
    unsigned int vPos;
    bool vIsSubtree;
};

InfEngineErrors ListTree::AddTreeToAnswer( const  Vars::Vector::VarStruct & aRoot, iAnswer & aAnswer )
{
    InfEngineErrors iee = INF_ENGINE_SUCCESS;        
    
    avector<FieldInfo> fields_info;
    
    for( unsigned int field_n = 1; field_n <= aRoot.GetFieldsCount(); ++field_n )
    {
        // Обрабатываем очередное поле типа структура.
        Vars::Vector::VarValue field_val = aRoot.GetFieldValue( field_n );
		if( field_val && Vars::Type::Struct == field_val.GetType() )
        {
            // Получение полей структуры.
            Vars::Vector::VarStruct field = field_val.AsStruct();
            Vars::Vector::VarValue state_val( nullptr, nullptr );
            Vars::Vector::VarValue name_val( nullptr, nullptr );
            Vars::Vector::VarValue pos_val( nullptr, nullptr );            
            if( INF_ENGINE_SUCCESS != ( iee = field.GetFieldValue( "state", 5, state_val ) ) ||
                    INF_ENGINE_SUCCESS != ( iee = field.GetFieldValue( "name", 4, name_val ) ) ||
                    INF_ENGINE_SUCCESS != ( iee = field.GetFieldValue( "pos", 3, pos_val ) ) )
            {
                if( INF_ENGINE_ERROR_NOFREE_MEMORY == iee )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
            }
            
            // Если флаг state отсутствует или установлен в ноль, то игнорируем поле.
            if( !state_val.AsText().GetValue() || strcmp( state_val.AsText().GetValue(), "1" ) )
                continue;
            
            // Определение позиции поля.
            unsigned int pos = static_cast<unsigned int>(-1);
            if( pos_val && pos_val.AsText() )
                pos = TextFuncs::ToInteger( pos_val.AsText().GetValue() );
                        
            // Добавляем поле в список на обработку.
            fields_info.push_back( FieldInfo( field_n, pos, !name_val ) );
            if( fields_info.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
        }
    }
    
    // Сортировка полей по значению поля pos.
    std::sort( fields_info.get_buffer(), fields_info.get_buffer() + fields_info.size(), FieldInfo() );
    
    // Создание списка из полей.
    for( unsigned int field_n = 0; field_n < fields_info.size(); ++field_n )
    {
        // Добавляем в список разделитель.
        if( field_n )
        {
            if( INF_ENGINE_SUCCESS != ( iee = aAnswer.AddBr() ) )
				ReturnWithError( iee, "Failed to create answer" );
        }
        
        // Обрабатываем ветвь дерева.
        if( fields_info[field_n].vIsSubtree )
        {
            if( INF_ENGINE_SUCCESS != ( iee = AddTreeToAnswer( aRoot.GetFieldValue( fields_info[field_n].vFieldN ).AsStruct(), aAnswer ) ) )
				ReturnWithTrace( iee );
        }
        // Обрабатываем лист.
        else
        {
            if( INF_ENGINE_SUCCESS != ( iee = AddLeafToAnswer( aRoot.GetFieldValue( fields_info[field_n].vFieldN ).AsStruct(), aAnswer ) ) )
				ReturnWithTrace( iee );
        }
    }
    
    return INF_ENGINE_SUCCESS;
}


InfEngineErrors ListTree::Apply(const char ** aArgs,
        const char *& aResult,
        unsigned int & aLength,
        nMemoryAllocator * aMemoryAllocator, 
        FunctionContext * aFunctionContext )
{
    // Проверка аргументов.
    if( !aArgs[0] || aArgs[1] )
        return INF_ENGINE_ERROR_ARGC;
    
    // Инициализация результата.
    aResult = nullptr;
    aLength = 0;
    
    if( aFunctionContext->vSession )
    {    
        // Поиск переменной.
        const Vars::Vector::VarValue var_val = aFunctionContext->vSession->GetValue( aArgs[0], strlen( aArgs[0] ) );
		if( var_val && Vars::Type::Struct == var_val.GetType() )
        {
            unsigned int answer_size = aFunctionContext->vAnswer->Size();
            
            // Создание списка.
            InfEngineErrors iee = AddTreeToAnswer( var_val.AsStruct(), *(aFunctionContext->vAnswer) );
            if( INF_ENGINE_SUCCESS != iee )
            {
                aFunctionContext->vAnswer->PopBack( aFunctionContext->vAnswer->Size() - answer_size );
				ReturnWithTrace( iee );
            }
        }
    }
    
    return INF_ENGINE_SUCCESS;
}
