#include "InternalFunction.hpp"
#include "BelongsTo.hpp"
#include "InfEngine2/_Include/Errors.h"

using namespace InternalFunctions;

const DLFucntionArgInfo BelongsTo::ARGS[] = {
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
	{ DLFAT_EXTENDED, nullptr, DLFAA_NONE },
};

const DLFucntionArgInfo BelongsTo::VAR_ARGS = { DLFAT_EXTENDED, nullptr, DLFAA_NONE };

const DLFunctionInfo BelongsTo::DLF_INFO = {
	"BelongsTo",
	"",
	"Check if word or phrase obtained by first argument interpretation contained in specified dictionary or dictionaries.",
	DLFRT_BOOL,
	2,
	BelongsTo::ARGS,
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	DLFO_CACHE,
	DLFunctionInterfaceVersion,
	5,
	InfDataProtocolVersion,
	&VAR_ARGS,
};

ExtICoreRO * BelongsTo::vBase = nullptr;

DictMatcher BelongsTo::vDictMatcher;
DocImage BelongsTo::vDocImage;
CDocMessage BelongsTo::vDocMessage;
DictTermsFilter BelongsTo::vDictFilter;

static const char CTRUE[] = "True";
static const unsigned int CTRUE_LEN = 4;

static const char CFALSE[] = "";
static const unsigned int CFALSE_LEN = 0;


InfEngineErrors BelongsTo::_apply( const char * aWord,
								   const char ** aDicts,
								   nMemoryAllocator * aMemoryAllocator,
								   Session * aSession,
								   DictMatcher & aDictMatcher,
								   DocImage & aDocImage,
								   CDocMessage & aDocMessage,
								   DictTermsFilter & aDictFilter )
{
	if( !aWord || !aDicts[0] )
		return INF_ENGINE_ERROR_ARGC;

	aDocImage.Reset();

	// Подготовка текста для поиска.
	int ret = vDocMessage.SetMessage( nullptr, 0, aWord, strlen( aWord ), true );
	if( ret != 0 )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create DocMessage. Return Code: %d", ret );
	aDocMessage.PresetLangs( LNG_RUSSIAN, LNG_UNKNOWN, false );

	// Лемматизация текста.
	InfEngineErrors iee = vBase->GetIndexBase().GetIndexDBase().LemmatizeDoc( aDocMessage, aDocImage );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Поиск совпадений первого аргумента функции со словарями, перечисленными в остальных аргументах.
	aDictMatcher.Reset();

	// Установка фильтра для поиска в указанных словарях.
	aDictFilter.Reset();
	aTextString name;
	for( const char ** dict_name = aDicts; *dict_name; ++dict_name )
	{
		nlReturnCode nlrc = name.assign( *dict_name );
		if( nlrc == nlrcErrorNoFreeMemory )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		if( nlrc != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, INF_ENGINE_STRING_ERROR_INTERNAL );

		nlrc = SetLower( name );
		if( nlrc == nlrcErrorNoFreeMemory )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		if( nlrc != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, INF_ENGINE_STRING_ERROR_INTERNAL );

		const unsigned int * dict_id = vBase->GetDLData().GetDictIdByName( name.ToConstChar(), name.size() );
		if( dict_id )
		{
			if( INF_ENGINE_SUCCESS != ( iee = aDictFilter.AddAcceptableDictId( *dict_id ) ) )
			{
				if( iee == INF_ENGINE_ERROR_NOFREE_MEMORY )
					ReturnWithError( iee, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				else
					ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}
		else
			LogWarn( "Unknown dictionary \"%s\" in function BelongsTo", *dict_name );
	}
	aDictMatcher.SetFilter( &aDictFilter, DictMatcher::FT_DICT_FILTER );

	if( vBase->GetIndexBase().GetIndexDBase().IsReady() )
	{
		InfEngineErrors iee = vBase->GetIndexBase().GetIndexDBase().Check( aDocImage, aDictMatcher );
		if( iee != INF_ENGINE_SUCCESS )
			return iee;
	}
	else
		return INF_ENGINE_ERROR_INTERNAL;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BelongsTo::Apply( const char ** aArgs, const char* & aResult, unsigned int & aLength, nMemoryAllocator * aMemoryAllocator,
								  FunctionContext * aFunctionContext )
{
	if( !vBase )
		return INF_ENGINE_ERROR_FUNC_NOT_INITIALIZED;

	if( !aArgs[0] || !aArgs[1] )
		return INF_ENGINE_ERROR_ARGC;

	bool result = false;
	InfEngineErrors iee = _apply( aArgs[0], aArgs + 1, aMemoryAllocator, aFunctionContext->vSession, vDictMatcher, vDocImage, vDocMessage, vDictFilter );
	if( INF_ENGINE_SUCCESS != iee )
		return iee;

	result = vDictMatcher.GetTermsNum();
	unsigned int doc_len = vDocImage.Size();

	// Поиск результата с учётом фильтров.
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

	// Подготовка результата.
	if( success )
	{
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
