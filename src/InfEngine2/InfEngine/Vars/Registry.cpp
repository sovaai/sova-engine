#include "Registry.hpp"

#include <cstring>
#include <cassert>

#include <InfEngine2/_Include/BinaryData.hpp>

#include <NanoLib/LogSystem.hpp>

using NameIndex = NanoLib::NameIndex;

namespace Vars
{
	InfEngineErrors RegistryRO::Open( fstorage * aFStorage, const RegistryRO * aBaseRegistry ) noexcept
	{
		// Проверка аргументов.
		if( !aFStorage )
			ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

		// Сброс данных реестра.
		Close();

		// Получение секции fstorage.
		auto section = fstorage_get_section( aFStorage, FSTORAGE_SECTION_VARS_REGISTRY );
		if( !section )
			ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

		// Получение размера секции.
		auto size = fstorage_section_get_size( section );
		if( size <= 6 * sizeof( uint32_t ) )
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

		// Получение указателя на память.
		const char * buffer = static_cast<const char*>( fstorage_section_get_all_data( section ) );
		if( !buffer )
			ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

		// Получение сдвига сохранения индекса.
		auto shift = reinterpret_cast<const uint32_t*>( buffer );

		// Проверка размера данных.
		if( shift[5] != fstorage_section_get_size( section ) )
			ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

		// Проверка чексуммы базового реестра.
		if( ( shift[1] && !aBaseRegistry ) || ( !shift[1] && aBaseRegistry ) )
			ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, INF_ENGINE_STRING_ERROR_INTERNAL );
		else if( shift[1] && aBaseRegistry )
		{
			unsigned char md5[16];
			aBaseRegistry->GetFullCheckSum( md5 );

			if( std::memcmp( buffer + shift[1], md5, 16 ) )
				ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, INF_ENGINE_STRING_ERROR_INTERNAL );
		}

		// Открытие индекса имен.
		NameIndex::ReturnCode nirc = vVarNameIndex.Open( buffer + shift[2], size - shift[2] );
		if( nirc != NameIndex::rcSuccess )
		{
			switch( nirc )
			{
			case NameIndex::rcErrorInvData:
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
			default:
				ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}

		vBuffer = buffer;
		vNodes = reinterpret_cast<const uint32_t*>( buffer + shift[3] );
		vMap = reinterpret_cast<const uint32_t*>( buffer + shift[4] );
		vBaseRegistry = aBaseRegistry;

		return INF_ENGINE_SUCCESS;
	}

	void RegistryRO::Close() noexcept
	{
		vVarNameIndex.Reset();
		vBuffer = nullptr;
		vNodes = nullptr;
		vMap = nullptr;
		vBaseRegistry = nullptr;
	}

	Id RegistryRO::Search( const char * aVarName, size_t aVarNameLength ) const noexcept
	{
		if( ( !aVarName && aVarNameLength != 0 ) || vVarNameIndex.GetState() != NameIndex::stReadOnly )
			return {};

		auto id = vVarNameIndex.Search( aVarName, aVarNameLength );
		if( id )
			return { static_cast<unsigned int>(( vBaseRegistry ? vBaseRegistry->GetVariablesNumber() : 0 ) + *id), false };
		else if( vBaseRegistry )
			return vBaseRegistry->Search( aVarName, aVarNameLength );
		else
			return {};
	}

	const char* RegistryRO::GetVarNameById( Id aVarId, unsigned int & aVarNameLength ) const noexcept
	{
		if( !ValidateId( aVarId ) )
			return aVarNameLength = 0, "";

		if( aVarId.is_tmp() )
			return aVarNameLength = 0, nullptr;

		if( vBaseRegistry )
		{
			if( aVarId.get() < vBaseRegistry->GetVariablesNumber() )
				return vBaseRegistry->GetVarNameById( aVarId, aVarNameLength );
			else
				aVarId -= vBaseRegistry->GetVariablesNumber();
		}

		return vVarNameIndex.GetName( aVarId.get(), aVarNameLength );
	}

	Type RegistryRO::GetVarType( Id aVarId ) const noexcept
	{
		if( !ValidateId( aVarId ) )
			return Type::Unknown;

		if( vBaseRegistry )
		{
			if( aVarId.get() < vBaseRegistry->GetVariablesNumber() )
			{
				if( IsMapped( aVarId ) )
				{
					return GetNodeType( vMap, aVarId );
				}
				else
				{
					return vBaseRegistry->GetVarType( aVarId );
				}
			}
			else
			{
				aVarId -= vBaseRegistry->GetVariablesNumber();
			}
		}
		
		return GetNodeType( vNodes, aVarId );
	}

	size_t RegistryRO::GetStructFieldsCount( Id aVarId ) const noexcept
	{
		if( !ValidateId( aVarId ) )
			return 0;

		if( aVarId.is_tmp() )
			return 0;

		if( vBaseRegistry )
		{
			if( aVarId.get() < vBaseRegistry->GetVariablesNumber() )
			{
				if( IsMapped( aVarId ) )
					return GetNodeChildsNumber( vMap, aVarId );
				else
					return vBaseRegistry->GetStructFieldsCount( aVarId );
			}
			else
				aVarId -= vBaseRegistry->GetVariablesNumber();
		}

		return GetNodeChildsNumber( vNodes, aVarId );
	}

	Id RegistryRO::GetStructFieldId( Id aVarId, unsigned int aFieldN ) const noexcept
	{
		if( !ValidateId( aVarId ) )
			return {};

		if( aVarId.is_tmp() )
			return {};

		// Нулевое поле. соответствует самой переменной.
		if( aFieldN == 0 )
			return aVarId;

		if( vBaseRegistry )
		{
			if( aVarId.get() < vBaseRegistry->GetVariablesNumber() )
			{
				if( IsMapped( aVarId ) )
				{
					auto oldchilds = vBaseRegistry->GetStructFieldsCount( aVarId );
					auto newchilds = GetNodeChildsNumber( vMap, aVarId ) - oldchilds;
					if( aFieldN <= newchilds )
						return GetNodeChildId( vMap, aVarId, aFieldN - 1 );
					else if( aFieldN <= oldchilds + newchilds )
						return vBaseRegistry->GetStructFieldId( aVarId, aFieldN - newchilds );
					else
						return {};
				}
				else
					return vBaseRegistry->GetStructFieldId( aVarId, aFieldN );
			}
			else
				aVarId -= vBaseRegistry->GetVariablesNumber();
		}

		if( aFieldN <= GetNodeChildsNumber( vNodes, aVarId ) )
			return GetNodeChildId( vNodes, aVarId, aFieldN - 1 );
		else
			return {};
	}

	void RegistryRO::GetFullCheckSum( unsigned char aCheckSum[16] ) const noexcept
	{
		if( vBuffer )
			std::memcpy( aCheckSum, vBuffer + reinterpret_cast<const uint32_t*>( vBuffer )[0], 16 );
		else
			std::memset( aCheckSum, 0, 16 );
	}

	void RegistryRO::GetMainCheckSum( unsigned char aCheckSum[16] ) const noexcept
	{
		if( vBaseRegistry )
			vBaseRegistry->GetMainCheckSum( aCheckSum );
		else
			GetFullCheckSum( aCheckSum );
	}

	void RegistryRO::GetBaseCheckSum( unsigned char aCheckSum[16] ) const noexcept
	{
		if( vBuffer )
			std::memcpy( aCheckSum, vBuffer + reinterpret_cast<const uint32_t*>( vBuffer )[1], 16 );
		else
			std::memset( aCheckSum, 0, 16 );
	}

	Options RegistryRO::GetProperties( Id aVarId ) const
	{
		if( !ValidateId( aVarId ) )
			return Options::None;

		if( aVarId.is_tmp() )
			return Options::None;

		if( vBaseRegistry )
		{
			if( aVarId.get() < vBaseRegistry->GetVariablesNumber() )
			{
				if( aVarId.get() < vMap[0] )
				{
					auto options = GetNodeOptions( vMap, aVarId );
					if( options > Options::Updated )
						return options;
					else
						return vBaseRegistry->GetProperties( aVarId );
				}
				else
					return vBaseRegistry->GetProperties( aVarId );
			}
			else
				aVarId -= vBaseRegistry->GetVariablesNumber();
		}

		return GetNodeOptions( vNodes, aVarId );
	}

	InfEngineErrors RegistryRW::Create( const RegistryRO * aBaseRegistry ) noexcept
	{
		// Проверка состояния индекса имен.
		if( vVarNameIndex.GetState() != NameIndex::stClosed )
			ReturnWithError( INF_ENGINE_ERROR_STATE, "VarRegistry has been already created or opened." );

		// Создание индекса.
		vVarNameIndex.Create();

		// Сохранение ссылки на базовый реестр переменных.
		vBaseRegistry = aBaseRegistry;

		// Регистрация автоматических переменных.
		InfEngineErrors iee;

		Id id;
		if( ( iee = Registrate( "inf_person",               strlen( "inf_person" ),                 id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarInfPerson ||
			( iee = Registrate( "reply-",                   strlen( "reply-" ),                     id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarInfPrevResponse ||
			( iee = Registrate( "reply--",                  strlen( "reply--" ),                    id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarInfPrevPrevResponse ||
			( iee = Registrate( "query-",                   strlen( "query-" ),                     id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarUserPrevRequest ||
			( iee = Registrate( "query--",                  strlen( "query--" ),                    id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarUserPrevPrevRequest ||
			( iee = Registrate( "that_anchor",              strlen( "that_anchor" ),                id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarThatAnchor ||
			( iee = Registrate( "last_mark",                strlen( "last_mark" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastMark ||
			( iee = Registrate( "auto_reply_id",            strlen( "auto_reply_id" ),              id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoReplyId ||
			( iee = Registrate( "auto_date",                strlen( "auto_date" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoDate ||
			( iee = Registrate( "auto_time",                strlen( "auto_time" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoTime ||
			( iee = Registrate( "inf_ellipsismode",         strlen( "inf_ellipsismode" ),           id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarInfEllipsisMode ||
			( iee = Registrate( "auto_ellipsis_disclosed",  strlen( "auto_ellipsis_disclosed" ),    id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoEllipsisDisclosed ||
			( iee = Registrate( "auto_response_type",       strlen( "auto_response_type" ),         id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoResponseType ||
			( iee = Registrate( "auto_ellipsis_id",         strlen( "auto_ellipsis_id" ),           id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoEllipsisId ||
			( iee = Registrate( "auto_ellipsis_detected",   strlen( "auto_ellipsis_detected" ),     id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoEllipsisDetected ||
			( iee = Registrate( "animation",                strlen( "animation" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAnimation ||
			( iee = Registrate( "auto_response_counter",    strlen( "auto_response_counter" ),      id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoResponseCounter ||
			( iee = Registrate( "last_mark2",               strlen( "last_mark2" ),                 id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastMark2 ||
			( iee = Registrate( "acv_info",                 strlen( "acv_info" ),                   id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAcvInfo ||
			( iee = Registrate( "auto_request_length",      strlen( "auto_request_length" ),        id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarRequestLength ||
			( iee = Registrate( "inf_uuid",                 strlen( "inf_uuid" ),                   id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarInfUUID ||
			( iee = Registrate( "inf_cuid",                 strlen( "inf_cuid" ),                   id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarInfCUID ||
			( iee = Registrate( "user_timezone",            strlen( "user_timezone" ),              id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarUserTimeZone ||
			( iee = Registrate( "last_service",             strlen( "last_service" ),               id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastService ||
			( iee = Registrate( "last_set1",                strlen( "last_set1" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastSet1 ||
			( iee = Registrate( "last_set2",                strlen( "last_set2" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastSet2 ||
			( iee = Registrate( "last_set3",                strlen( "last_set3" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastSet3 ||
			( iee = Registrate( "last_set4",                strlen( "last_set4" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastSet4 ||
			( iee = Registrate( "last_set5",                strlen( "last_set5" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastSet5 ||
			( iee = Registrate( "last_set6",                strlen( "last_set6" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLastSet6 ||
			( iee = Registrate( "match_weight",             strlen( "match_weight" ),               id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarMatchWeight ||
			( iee = Registrate( "langdetectedkazakh",       strlen( "langdetectedkazakh" ),         id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarLangDetectedKazakh ||
			( iee = Registrate( "inputtype",                strlen( "inputtype" ),                  id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarInputType ||
			( iee = Registrate( "skipunknown",              strlen( "skipunknown" ),                id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineSkipUnknown ||
			( iee = Registrate( "dictmergemethod",          strlen( "dictmergemethod" ),            id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineDictMergeMethod ||
			( iee = Registrate( "dictmergeeffect",          strlen( "dictmergeeffect" ),            id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineDictMergeEffect ||
			( iee = Registrate( "auto_reply_label",         strlen( "auto_reply_label" ),           id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoReplyLabel ||
			( iee = Registrate( "auto_request_counter",     strlen( "auto_request_counter" ),       id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoRequestCounter ||
			( iee = Registrate( "auto_event_ids",           strlen( "auto_event_ids" ),             id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarAutoEventIds ||
			( iee = Registrate( "disable_cache",            strlen( "disable_cache" ),              id ) ) != INF_ENGINE_SUCCESS
			|| id != InfEngineVarDisableCache)
		{
			Close();
			if( iee == INF_ENGINE_SUCCESS )
				ReturnWithError( INF_ENGINE_ERROR_FAULT, "Internal error: variable collision!" );
			else
				return iee;
		}

		// Простановка свойств переменных.
		SetProperties( InfEngineVarInfPerson, Options::ReadOnly );
		SetProperties( InfEngineVarInfPrevResponse, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarInfPrevPrevResponse, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarUserPrevRequest, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarUserPrevPrevRequest, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarThatAnchor, Options::Auto );
		SetProperties( InfEngineVarLastMark, Options::Auto );
		SetProperties( InfEngineVarAutoReplyId, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarAutoDate, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarAutoTime, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarInfEllipsisMode, Options::ReadOnly );
		SetProperties( InfEngineVarAutoEllipsisDisclosed, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarAutoResponseType, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarAutoEllipsisId, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarAutoEllipsisDetected, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarAnimation, Options::None );
		SetProperties( InfEngineVarAutoResponseCounter, Options::Auto /*| Options::ReadOnly*/ );
		SetProperties( InfEngineVarLastMark2, Options::Auto );
		SetProperties( InfEngineVarAcvInfo, Options::Auto );
		SetProperties( InfEngineVarRequestLength, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarInfUUID, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarInfCUID, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarUserTimeZone, Options::Auto );
		SetProperties( InfEngineVarLastService, Options::Auto );
		SetProperties( InfEngineVarLastSet1, Options::Auto );
		SetProperties( InfEngineVarLastSet2, Options::Auto );
		SetProperties( InfEngineVarLastSet3, Options::Auto );
		SetProperties( InfEngineVarLastSet4, Options::Auto );
		SetProperties( InfEngineVarLastSet5, Options::Auto );
		SetProperties( InfEngineVarLastSet6, Options::Auto );
		SetProperties( InfEngineVarMatchWeight, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarLangDetectedKazakh, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarInputType, Options::Auto /*| Options::ReadOnly*/ );
		SetProperties( InfEngineVarAutoReplyLabel, Options::Auto | Options::ReadOnly );
		SetProperties( InfEngineVarAutoEventIds, Options::Auto | Options::ReadOnly );

		return INF_ENGINE_SUCCESS;
	}

	InfEngineErrors RegistryRW::Save( fstorage * aFStorage ) const noexcept
	{
		// Проверка аргументов.
		if( !aFStorage )
			ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

		// Проверка состояния реестра.
		if( vVarNameIndex.GetState() == NameIndex::stClosed )
			ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

		// Получение указателя на секцию.
		auto section = fstorage_get_section( aFStorage, FSTORAGE_SECTION_VARS_REGISTRY );
		if( !section )
			ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

		// Вычисление необходимой памяти.
		size_t memory = 6 * sizeof( uint32_t );
		{
			// Чексумма текущего реестра.
			memory += 16;
			binary_data_alignment( memory );

			// Чексумма базового реестра.
			memory += vBaseRegistry ? 16 : 0;
			binary_data_alignment( memory );

			// Реестр имен переменных.
			memory += vVarNameIndex.GetNeedMemorySize();
			binary_data_alignment( memory );

			// Данные новых переменных.
			memory += ( 1 + vNodes.size() ) * sizeof( uint32_t );
			for( auto & node : vNodes )
				memory += ( 2 + node.vChildsNumber ) * sizeof( uint32_t );
			binary_data_alignment( memory );

			// Данные измененных переменных.
			memory += ( 1 + vMap.size() ) * sizeof( uint32_t );
			for( auto & node : vMap )
				memory += ( 2 + node.vChildsNumber - vBaseRegistry->GetStructFieldsCount( node.vId ) ) * sizeof( uint32_t );
			binary_data_alignment( memory );
		}
		int ret = fstorage_section_realloc( section, memory );
		if( ret != FSTORAGE_OK )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Получение указателя на память.
		void * buffer = fstorage_section_get_all_data( section );
		if( !buffer )
			ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

		void * ptr = buffer;

		// Сдвиги.
		auto shift = binary_data_skip<uint32_t>( ptr, 5 );

		// Сохранение размера сохраненного реестра.
		binary_data_save<uint32_t>( ptr, memory );

		// Сохранение чексуммы текущего реестра.
		*shift++ = binary_data_alignment( ptr, buffer );
		unsigned char md5[16];
		GetFullCheckSum( md5 );
		binary_data_save<unsigned char>( ptr, md5, 16 );

		// Сохранение чексуммы базового реестра.
		if( vBaseRegistry )
		{
			*shift++ = binary_data_alignment( ptr, buffer );
			GetBaseCheckSum( md5 );
			binary_data_save<unsigned char>( ptr, md5, 16 );
		}
		else
			*shift++ = 0;

		// Сохранение индекса.
		*shift++ = binary_data_alignment( ptr, buffer );
		binary_data_save_object( ptr, vVarNameIndex );

		// Сохранение массива новых переменных.
		*shift++ = binary_data_alignment( ptr, buffer );
		binary_data_save<uint32_t>( ptr, vNodes.size() );
		auto nshift = binary_data_skip<uint32_t>( ptr, vNodes.size() );
		for( auto & node : vNodes )
		{
			*nshift++ = binary_data_alignment( ptr, buffer );

			// Сохранение типа переменной.
			binary_data_save<uint16_t>( ptr, static_cast<unsigned int>( node.vType ) );

			// Сохранение свойств переменной.
			binary_data_save<uint16_t>( ptr, static_cast<unsigned int>( node.vProperties ) );

			// Количество дочерних узлов.
			binary_data_save<uint32_t>( ptr, node.vChildsNumber );

			// Идентификаторы дочерних узлов.
			auto childsptr = ptr;
			for( auto child = node.vChild; child; child = GetNode( child )->vSibling )
				binary_data_save<uint32_t>( ptr, child.get() );
			assert( binary_data_alignment( ptr, childsptr ) == node.vChildsNumber * sizeof( uint32_t ) );
		}

		// Сохранение измененных переменных.
		*shift++ = binary_data_alignment( ptr, buffer );
		binary_data_save<uint32_t>( ptr, vMap.size() );
		auto mshift = binary_data_skip<uint32_t>( ptr, vMap.size() );
		for( auto & node : vMap )
		{
			*mshift++ = binary_data_alignment( ptr, buffer );

			// Сохранение типа переменной.
			binary_data_save<uint16_t>( ptr, static_cast<unsigned int>( node.vType ) );

			// Сохранение свойств переменной.
			binary_data_save<uint16_t>( ptr, static_cast<unsigned int>( node.vProperties ) );

			// Количество дочерних узлов.
			binary_data_save<uint32_t>( ptr, node.vChildsNumber );

			// Идентификаторы дочерних узлов.
			auto childsptr = ptr;
			for( auto child = node.vChild; child; child = GetNode( child )->vSibling )
				binary_data_save<uint32_t>( ptr, child.get() );
			assert( binary_data_alignment( ptr, childsptr ) ==
			        (node.vChildsNumber - vBaseRegistry->GetStructFieldsCount( node.vId )) * sizeof( uint32_t ) );
		}

		assert( binary_data_alignment( ptr, buffer ) == memory );

		return INF_ENGINE_SUCCESS;
	}

	void RegistryRW::Close() noexcept
	{
		vVarNameIndex.Reset();
		vNodes.clear();
		vMap.clear();
		vBaseRegistry = nullptr;
	}

	InfEngineErrors RegistryRW::Registrate( const char * aVarName, unsigned int aVarNameLength, Id & aVarId, bool aUniq ) noexcept
	try
	{
		// Проверка состония..
		if( vVarNameIndex.GetState() != NameIndex::stReadWrite )
			ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

		// Проверка аргументов.
		if( !ValidateName( aVarName, aVarNameLength ) )
			return INF_ENGINE_ERROR_INV_ARGS;

		// Нормализация имени переменной.
		static std::string varname;
		varname.assign( aVarName, aVarNameLength );
		NormalizeVarName( varname );

		// Поиск имени переменной в реестре.
		aVarId = SearchNormalized( varname.c_str(), varname.size() );
		if( aVarId )
		{
			if( aUniq )
			{
				if( GetProperties( aVarId ) > Options::Explicit )
					return INF_ENGINE_WARN_ELEMENT_EXISTS;
				else
				{
					if( vBaseRegistry && aVarId.get() < vBaseRegistry->GetVariablesNumber() )
					{
						// Отображение переменной базового реестра на текущий для исправления ее свойств.
						auto iee = MapNode( aVarId );
						if( iee != INF_ENGINE_SUCCESS )
							return iee;
					}
					AddProperties( aVarId, Options::Explicit );
					return INF_ENGINE_SUCCESS;
				}
			}
			else
				return INF_ENGINE_SUCCESS;
		}

		// Добавление полного имени переменной в реестр.
		auto iee = InsertNode( varname.c_str(), varname.size(), aVarId );
		if( iee != INF_ENGINE_SUCCESS )
			return iee;
		// Простановка автоматических свойств переменных, основанных на их именах.
		InitProperties( aVarId, varname.c_str(), varname.size() );
		AddProperties( aVarId, Options::Explicit );

		// Разбор структуры имени.
		Id parent;
		for( size_t pos { 0 }; ( pos = varname.find( ".", pos ) ) != varname.npos; pos++ )
		{
			auto id = SearchNormalized( varname.c_str(), pos );
			if( id )
			{
				parent = id;
				continue;
			}
			else
			{
				auto iee = InsertNode( varname.c_str(), pos, id );
				if( iee != INF_ENGINE_SUCCESS )
					return iee;
				InitProperties( id, varname.c_str(), pos );

				if( parent )
				{
					if( vBaseRegistry && parent.get() < vBaseRegistry->GetVariablesNumber() )
					{
						auto iee = MapNode( parent );
						if( iee != INF_ENGINE_SUCCESS )
							return iee;
					}

					auto pnode = GetNode( parent );
					GetNode( id )->vSibling = pnode->vChild;
					pnode->vChild = id;
					pnode->vType = Type::Struct;
					pnode->vChildsNumber++;
				}

				parent = id;
			}
		}

		if( parent )
		{
			if( vBaseRegistry && parent.get() < vBaseRegistry->GetVariablesNumber() )
			{
				auto iee = MapNode( parent );
				if( iee != INF_ENGINE_SUCCESS )
					return iee;
			}

			auto pnode = GetNode( parent );
			GetNode( aVarId )->vSibling = pnode->vChild;
			pnode->vChild = aVarId;
			pnode->vType = Type::Struct;
			pnode->vChildsNumber++;
		}

		return INF_ENGINE_SUCCESS;
	}
	catch( std::bad_alloc )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	Id RegistryRW::Search( const char * aVarName, size_t aVarNameLength ) const noexcept
	try
	{
		// Проверка состояния.
		if( vVarNameIndex.GetState() != NameIndex::stReadWrite )
			return {};

		// Проверка аргументов.
		if( !ValidateName( aVarName, aVarNameLength ) )
			return {};

		// Нормализация имени переменной.
		static std::string varname;
		varname.assign( aVarName, aVarNameLength );
		NormalizeVarName( varname );

		return SearchNormalized( varname.c_str(), varname.size() );
	}
	catch( std::bad_alloc )
	{
		return {};
	}

	const char* RegistryRW::GetVarNameById( Id aVarId, unsigned int & aVarNameLength ) const noexcept
	{
		if( aVarId.is_tmp() )
			return aVarNameLength = 0, "";
		else if( vBaseRegistry )
		{
			if( aVarId.get() < vBaseRegistry->GetVariablesNumber() )
				return vBaseRegistry->GetVarNameById( aVarId, aVarNameLength );
			else
				aVarId -= vBaseRegistry->GetVariablesNumber();
		}

		return vVarNameIndex.GetName( aVarId.get(), aVarNameLength );
	}

	void RegistryRW::GetFullCheckSum( unsigned char aCheckSum[16] ) const noexcept
	{
		if( vVarNameIndex.GetState() != NanoLib::NameIndex::stReadWrite )
			std::memset( aCheckSum, 0, 16 );

		MD5_CTX context;
		MD5Init( &context );

		// Получение контрольной суммы базового реестра.
		GetBaseCheckSum( aCheckSum );
		MD5Update( &context, aCheckSum, 16 );

		// Получение чексуммы реестра имен переменных.
		vVarNameIndex.GetMD5Digest( aCheckSum );
		MD5Update( &context, aCheckSum, 16 );

		// Получение контрольной суммы всех измененных узлов.
		auto size = vMap.size();
		MD5Update( &context, reinterpret_cast<const unsigned char*>( &size ), sizeof( size ) );
		for( auto & node : vMap )
		{
			node.GetCheckSum( aCheckSum );
			MD5Update( &context, aCheckSum, 16 );
		}

		// Получение контрольной суммы всех новых узлов.
		size = vNodes.size();
		MD5Update( &context, reinterpret_cast<const unsigned char*>( &size ), sizeof( size ) );
		for( auto & node : vNodes )
		{
			node.GetCheckSum( aCheckSum );
			MD5Update( &context, aCheckSum, 16 );
		}

		MD5Final( aCheckSum, &context );
	}

	void RegistryRW::GetMainCheckSum( unsigned char aCheckSum[16] ) const noexcept
	{
		if( vBaseRegistry )
			vBaseRegistry->GetMainCheckSum( aCheckSum );
		else
			GetFullCheckSum( aCheckSum );
	}

	void RegistryRW::GetBaseCheckSum( unsigned char aCheckSum[16] ) const noexcept
	{
		if( vBaseRegistry )
			vBaseRegistry->GetFullCheckSum( aCheckSum );
		else
			std::memset( aCheckSum, 0, 16 );
	}

	Options RegistryRW::GetProperties( Id aVarId ) const noexcept
	{
		if( aVarId.is_tmp() )
			return Options::None;
		else
		{
			auto node = GetNode( aVarId );
			if( node )
				return node->vProperties;
			else if( vBaseRegistry && aVarId.get() < vBaseRegistry->GetVariablesNumber() )
				return vBaseRegistry->GetProperties( aVarId );
			else
				return Options::None;
		}
	}

	Type RegistryRW::GetVarType( Id aVarId ) const noexcept
	{
		if( aVarId.is_tmp() )
			return Type::Unknown;
		else
		{
			auto node = GetNode( aVarId );
			if( node )
				return node->vType;
			else if( vBaseRegistry && aVarId.get() < vBaseRegistry->GetVariablesNumber() )
				return vBaseRegistry->GetVarType( aVarId );
			else
				return Type::Unknown;
		}
	}

	size_t RegistryRW::GetStructFieldsCount( Id aVarId ) const noexcept
	{
		if( aVarId.is_tmp() )
			return 0;
		else
		{
			auto node = GetNode( aVarId );
			if( node )
				return node->vChildsNumber;
			else if( vBaseRegistry && aVarId.get() < vBaseRegistry->GetVariablesNumber() )
				return vBaseRegistry->GetStructFieldsCount( aVarId );
			else
				return 0;
		}
	}

	Id RegistryRW::GetStructFieldId( Id aVarId, unsigned int aFieldN ) const noexcept
	{
		if( aVarId.is_tmp() || aVarId.get() > GetVariablesNumber() )
			return {};

		auto node = GetNode( aVarId );
		if( node )
		{
			if( aFieldN == 0 )
				return aVarId;

			auto childs = GetStructFieldsCount( aVarId ) - ( vBaseRegistry ? vBaseRegistry->GetStructFieldsCount( aVarId ) : 0 );
			if( aFieldN <= childs )
			{
				auto child = GetNode( node->vChild );
				for( aFieldN--; aFieldN; aFieldN-- )
					child = GetNode( child->vSibling );
				return child->vId;
			}
			else if( aFieldN <= GetStructFieldsCount( aVarId ) )
				return vBaseRegistry->GetStructFieldId( aVarId, aFieldN - childs );
			else
				return {};
		}
		else
			return vBaseRegistry->GetStructFieldId( aVarId, aFieldN );
	}

	void RegistryRW::SetProperties( Id aVarId, Options aProperties ) noexcept
	{
		auto node = GetNode( aVarId );
		if( node )
			node->vProperties = aProperties;
	}

	void RegistryRW::AddProperties( Id aVarId, Options aProperties ) noexcept
	{
		auto node = GetNode( aVarId );
		if( node )
			node->vProperties |= aProperties;
	}

	void RegistryRW::InitProperties( Id aVarId, const char * aVarName, size_t aVarNameLength ) noexcept
	{
		// Простановка автоматических свойств переменных, основанных на их именах.
		Options properties { Options::None };
		/*if( aVarNameLength > 4 && !strncasecmp( aVarName, "inf_", 4 ) )
			properties = Options::ReadOnly;
		else */if( aVarNameLength > 6 && ( !strncasecmp( aVarName, "quest_", 6 ) ||
										 !strncasecmp( aVarName, "business_", 9 ) ||
										 !strncasecmp( aVarName, "yota_", 5 ) ) )
			properties = Options::ReadOnly | Options::Extended;

		if( vBaseRegistry )
			properties |= Options::ReadOnly;

		SetProperties( aVarId, properties );
	}

	const RegistryRW::Node* RegistryRW::GetNode( Id aVarId ) const noexcept
	{
		if( vBaseRegistry )
		{
			if( aVarId.get() < vBaseRegistry->GetVariablesNumber() )
				if( aVarId.get() < vMap.size() && vMap[aVarId.get()] )
					return &vMap[aVarId.get()];
				else
					return nullptr;
			else
				aVarId -= vBaseRegistry->GetVariablesNumber();
		}
		return &vNodes[aVarId.get()];
	}

	RegistryRW::Node* RegistryRW::GetNode( Id aVarId ) noexcept
	{
		if( vBaseRegistry )
		{
			if( aVarId.get() < vBaseRegistry->GetVariablesNumber() )
				if( aVarId.get() < vMap.size() && vMap[aVarId.get()] )
					return &vMap[aVarId.get()];
				else
					return nullptr;
			else
				aVarId -= vBaseRegistry->GetVariablesNumber();
		}
		return &vNodes[aVarId.get()];
	}

	Id RegistryRW::SearchNormalized( const char * aVarName, size_t aVarNameLength ) const noexcept
	{
		// Поиск в текущем реестре.
		auto id = vVarNameIndex.Search( aVarName, aVarNameLength );
		if( id )
			return { static_cast<unsigned int >(( vBaseRegistry ? vBaseRegistry->GetVariablesNumber() : 0 ) + *id), false };

		// Поиск в базовом реестре.
		if( vBaseRegistry )
			return vBaseRegistry->Search( aVarName, aVarNameLength );
		else
			return {};
	}

	std::string& RegistryRW::NormalizeVarName( std::string & aVarName ) const noexcept
	{
		AllTrim( aVarName );
		SetLower( aVarName );
		return aVarName;
	}

	bool RegistryRW::ValidateName( const char * aVarName, unsigned int aVarNameLength ) const noexcept
	{
		if( !aVarName || aVarNameLength == 0 )
			return false;

		for( unsigned int i = 0; i < aVarNameLength; i++ )
		{
			char sym = aVarName[i];
			if( !TextFuncs::IsAlNum( sym ) && !std::strchr( "_-.", sym ) )
				return false;
		}

		if( aVarName[0] == '.' || aVarName[aVarNameLength - 1] == '.' )
			return false;

		return true;
	}

	InfEngineErrors RegistryRW::InsertNode( const char * aVarName, unsigned int aVarNameLength, Id & aVarId ) noexcept
	try
	{
		// Проверка возможности добавления переменной в реестр.
		if( IsFull() )
			return INF_ENGINE_ERROR_LIMIT;

		// Предварительное выделение памяти.
		vNodes.resize( vVarNameIndex.GetNamesNumber() + 1 );

		unsigned int id;
		switch( vVarNameIndex.AddName( aVarName, aVarNameLength, id ) )
		{
		case NameIndex::rcSuccess:
		case NameIndex::rcElementExists:
			aVarId = { id, false };
			if( vBaseRegistry )
				aVarId += vBaseRegistry->GetVariablesNumber();
			vNodes[id] = { aVarId };
			return INF_ENGINE_SUCCESS;
		case NameIndex::rcErrorLimitIsReached:
			return INF_ENGINE_ERROR_LIMIT;
		case NameIndex::rcErrorInvState:
			ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );
		default:
			ReturnWithError( INF_ENGINE_ERROR_NANOLIB, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}
	catch( std::bad_alloc )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	InfEngineErrors RegistryRW::MapNode( const Id & aVarId ) noexcept
	try
	{
		if( !vBaseRegistry || aVarId.get() >= vBaseRegistry->GetVariablesNumber() )
			ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INTERNAL );

		if( vMap.size() <= aVarId.get() )
			vMap.resize( aVarId.get() + 1 );
		else if( vMap[aVarId.get()] )
			return INF_ENGINE_SUCCESS;

		vMap[aVarId.get()].vId = aVarId;
		vMap[aVarId.get()].vSibling = {};
		vMap[aVarId.get()].vChild = {};
		vMap[aVarId.get()].vType = vBaseRegistry->GetVarType( aVarId );
		vMap[aVarId.get()].vProperties = vBaseRegistry->GetProperties( aVarId ) | Options::Updated;
		vMap[aVarId.get()].vChildsNumber = vBaseRegistry->GetStructFieldsCount( aVarId );

		return INF_ENGINE_SUCCESS;
	}
	catch( std::bad_alloc )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	void RegistryRW::Node::GetCheckSum( unsigned char aCheckSum[16] ) const noexcept
	{
		MD5_CTX context;
		MD5Init( &context );

		auto id = vId.get();
		MD5Update( &context, reinterpret_cast<const unsigned char*>( &id ), sizeof( unsigned int ) );
		id = vSibling.get();
		MD5Update( &context, reinterpret_cast<const unsigned char*>( &id ), sizeof( unsigned int ) );
		id = vChild.get();
		MD5Update( &context, reinterpret_cast<const unsigned char*>( &id ), sizeof( unsigned int ) );
		MD5Update( &context, reinterpret_cast<const unsigned char*>( &vProperties ), sizeof( vProperties ) );
		MD5Update( &context, reinterpret_cast<const unsigned char*>( &vType ), sizeof( vType ) );
		MD5Update( &context, reinterpret_cast<const unsigned char*>( &vChildsNumber ), sizeof( vChildsNumber ) );

		MD5Final( aCheckSum, &context );
	}
}
