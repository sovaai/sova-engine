#include "InternalFunction.hpp"
#include "InfEngine2/_Include/Errors.h"
#include "DumpVar.hpp"

using namespace InternalFunctions;

const DLFucntionArgInfo DumpVar::ARGS[] = {
	{ DLFAT_TEXT, 0},
};

const DLFunctionInfo DumpVar::DLF_INFO = {
	"DumpVar",
	"",
	"Print dump of given variable.",
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

ExtICoreRO * DumpVar::vBase = nullptr;

InfEngineErrors DumpVar::Apply( const char ** aArgs, const char *& aResult, unsigned int & aLength, nMemoryAllocator * aMemoryAllocator,
								FunctionContext * aFunctionContext )
{
	// Проверка аргументов.
	if( !aArgs[0] || aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;

	// Инициализация результата.
	aResult = nullptr;
	aLength = 0;

	// Буфер для формирования результата.
	aTextString result;
	// Имя переменной.
	nlReturnCode nlrc = result.assign( aArgs[0] );
	if( nlrcSuccess != nlrc )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	if( aFunctionContext->vSession )
	{
		// Поиск переменной.
		const Vars::Vector::VarValue var_val = aFunctionContext->vSession->GetValue( aArgs[0], strlen( aArgs[0] ) );
		if( var_val )
		{
			// Тип переменной.
			switch( var_val.GetType() )
			{
				case Vars::Type::Text:
					// Описание типа переменной.
					if( nlrcSuccess != ( nlrc = result.append( ": " ) ) || nlrcSuccess != ( nlrc = result.append( "TEXT " ) ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

					// Описание значения переменной.
					if( nlrcSuccess != ( nlrc = result.append( "\"" ) ) ||
							nlrcSuccess != ( nlrc = result.append( var_val.AsText().GetValue() ) ) ||
							nlrcSuccess != ( nlrc = result.append( "\"" ) ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					break;
				case Vars::Type::Struct:
				{
					// Описание типа переменной.
					if( nlrcSuccess != ( nlrc = result.append( ": " ) ) || nlrcSuccess != ( nlrc = result.append( "STRUCTURE" ) ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

					// Начало описания структуры.
					if( nlrcSuccess != ( nlrc = result.append( " { " ) ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

					// Описание значения переменной типа структура.
					Vars::Vector::VarStruct var_struct = var_val.AsStruct();
					for( unsigned int field_n = 0; field_n < var_struct.GetFieldsCount(); ++field_n )
					{
						// Подготовка аргументов для рекурсивного запроса.
						const char * args[] = { var_struct.GetFieldName( field_n + 1 ), nullptr };
						const char * field = nullptr;
						unsigned int field_len = 0;

						InfEngineErrors iee = Apply( args, field, field_len, aMemoryAllocator, aFunctionContext );
						// Если произошла ошибка.
						if( INF_ENGINE_SUCCESS != iee )
						{
							if( iee == INF_ENGINE_WARN_UNSUCCESS || iee == INF_ENGINE_ERROR_DLF_SKIP_ANSWER )
								return iee;
						ReturnWithTrace( iee );
						}

						// Добавляем описание поля.
						if( field_n )
							if( nlrcSuccess != ( nlrc = result.append( ", ", 2 ) ) )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
						if( nlrcSuccess != ( nlrc = result.append( field, field_len ) ) )
							ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					}

					// Конец описания структуры.
					if( nlrcSuccess != ( nlrc = result.append( " }" ) ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
					break;
				}
				default:
					// Описание типа переменной.
					if( nlrcSuccess != ( nlrc = result.append( ": " ) ) || nlrcSuccess != ( nlrc = result.append( "UNKNOWN" ) ) )
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			}
		}
		else
		{
			// Переменная не найдена.
			if( nlrcSuccess != ( nlrc = result.append( " [no value]" ) ) )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
	}

	// Выделяем память под результат.
	char* Buffer = static_cast<char*>( aMemoryAllocator->Allocate( result.size() + 1 ) );
	if( !Buffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Копируем результат.
	memcpy( Buffer, result.ToConstChar(), result.size() );
	Buffer[result.size()] = '\0';
	aResult = Buffer;
	aLength = result.size();

	return INF_ENGINE_SUCCESS;
}
