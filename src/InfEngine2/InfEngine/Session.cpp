#include "Session.hpp"

InfEngineErrors DictsStates::GetRandomDictLineId( unsigned int aDictId, InfPatternItems::DictType aDictType, unsigned int aDictSize, unsigned int & aLineId )
{
	aLineId = static_cast<unsigned int>( -1 );

	DictState * dict_state = nullptr;
	InfEngineErrors iee = FindDictState( aDictId, dict_state );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
	{
		if( INF_ENGINE_SUCCESS != ( iee = AddDictState( aDictId, aDictSize, dict_state ) ) )
			ReturnWithTrace( iee );
	}
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Сбрасываем состояние словаря при необходимости.
	if( dict_state->IsEmpty() )
	{
		if( aDictType == InfPatternItems::DT_UDICT )
		{
			if( INF_ENGINE_SUCCESS != ( iee = ResetDictState( aDictId ) ) )
				ReturnWithTrace( iee );

			if( dict_state->IsEmpty() )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}

	aLineId = dict_state->TakeFromBox();
	if( aLineId == static_cast<unsigned int>( -1 ) )
		return INF_ENGINE_WARN_UNSUCCESS;
	vLog.push_back( LogNode( aDictId, aLineId ) );
	if( vLog.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Сбрасываем состояние словаря при необходимости.
	if( dict_state->IsEmpty() )
	{
		if( aDictType == InfPatternItems::DT_UDICT )
		{
			if( INF_ENGINE_SUCCESS != ( iee = ResetDictState( aDictId ) ) )
				ReturnWithTrace( iee );

			if( dict_state->IsEmpty() )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DictsStates::RemoveLineIdFromDict( unsigned int aDictId, InfPatternItems::DictType aDictType, unsigned int aDictSize, unsigned int aLineId )
{
	DictState * dict_state = nullptr;
	InfEngineErrors iee = FindDictState( aDictId, dict_state );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
	{
		if( INF_ENGINE_SUCCESS != ( iee = AddDictState( aDictId, aDictSize, dict_state ) ) )
			ReturnWithTrace( iee );
	}
	else if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Сбрасываем состояние словаря при необходимости.
	if( dict_state->IsEmpty() )
	{
		if( aDictType == InfPatternItems::DT_UDICT )
		{
			if( INF_ENGINE_SUCCESS != ( iee = ResetDictState( aDictId ) ) )
				ReturnWithTrace( iee );

			if( dict_state->IsEmpty() )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}

	if( !dict_state->TakeFromBox( aLineId ) )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
	vLog.push_back( LogNode( aDictId, aLineId ) );
	if( vLog.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Сбрасываем состояние словаря при необходимости.
	if( dict_state->IsEmpty() )
	{
		if( aDictType == InfPatternItems::DT_UDICT )
		{
			if( INF_ENGINE_SUCCESS != ( iee = ResetDictState( aDictId ) ) )
				ReturnWithTrace( iee );

			if( dict_state->IsEmpty() )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DictsStates::RollbackLastChanges()
{
	DictState * dict_state = nullptr;
	LogNode * end = vLog.get_buffer() + vLog.size();
	for( LogNode * node = vLog.get_buffer(); node < end; ++node )
	{
		InfEngineErrors iee = FindDictState( node->vDictId, dict_state );
		if( iee == INF_ENGINE_WARN_UNSUCCESS )
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		dict_state->RetriveToBox( node->vLineId );
	}
	vLog.clear();

	return INF_ENGINE_SUCCESS;
}

unsigned int DictsStates::GetRemainingAmount( unsigned int aDictId, unsigned int aDictSize )
{
	DictState * dict_state = nullptr;
	InfEngineErrors iee = FindDictState( aDictId, dict_state );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return aDictSize;

	unsigned int tmp = dict_state->GetRemainingAmount();

	return tmp;
}

InfEngineErrors DictsStates::ResetDictState( unsigned int aDictId )
{
	DictState * dict_state = nullptr;
	InfEngineErrors iee = FindDictState( aDictId, dict_state );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return INF_ENGINE_SUCCESS;

	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	dict_state->Reset();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DictsStates::Drop()
{
	vStates.clear();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DictsStates::AddDictState( unsigned int aDictId, unsigned int aDictSize, DictState *& aDictState )
{
	Node * node = nAllocateObject( vAllocator, Node );
	if( !node )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	new((nMemoryAllocatorElementPtr*)node )Node( aDictId, aDictSize );
	aDictState = &node->vDictState;

	vStates.push_back( node );
	if( vStates.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors DictsStates::FindDictState( unsigned int aDictId, DictState *& aDictState )
{
	Node ** end = vStates.get_buffer() + vStates.size();
	for( Node ** node = vStates.get_buffer(); node < end; ++node )
		if( ( *node )->vDictId == aDictId )
		{
			aDictState = &( *node )->vDictState;
			return INF_ENGINE_SUCCESS;
		}

	return INF_ENGINE_WARN_UNSUCCESS;
}

InfEngineErrors DictsStates::IsUsed( unsigned int aDictId, InfPatternItems::DictType aDictType, unsigned int aLineId, bool & aAlreadyUsed )
{
	aAlreadyUsed = false;
	DictState * dict_state = nullptr;

	// Ищем состояние заданного словаря словарь.
	InfEngineErrors iee = FindDictState( aDictId, dict_state );
	if( iee == INF_ENGINE_WARN_UNSUCCESS )
		return INF_ENGINE_SUCCESS;
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Сбрасываем состояние словаря при необходимости.
	if( dict_state->IsEmpty() )
	{
		if( aDictType == InfPatternItems::DT_UDICT )
		{
			if( INF_ENGINE_SUCCESS != ( iee = ResetDictState( aDictId ) ) )
				ReturnWithTrace( iee );

			if( dict_state->IsEmpty() )
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
	}

	// Проверяем, использовалась ли заданная строка словаря ранее.
	if( dict_state->IsInBox( aLineId ) )
		return INF_ENGINE_SUCCESS;
	aAlreadyUsed = true;

	return INF_ENGINE_SUCCESS;
}

unsigned int DictsStates::GetNeedMemorySize() const
{
	unsigned int memory = sizeof (uint32_t );	// Количество словарей, для которых сохраняются состояния.
	memory += sizeof (uint32_t ) * vStates.size();	// Сдвиги к состояниям словарей.
	memory += sizeof (uint32_t ) * vStates.size();	// Идентификаторы словарей.
	Node ** end = vStates.get_buffer() + vStates.size();
	for( Node ** node = vStates.get_buffer(); node < end; ++node )
		memory += ( *node )->vDictState.GetNeedMemorySize();	// Размер очередного состояния словаря.

	return memory;
}

unsigned int DictsStates::Save( void * aBuffer ) const
{
	char * ptr = static_cast<char*>( aBuffer );

	// Сохраняем количество состояний словарей.
	binary_data_save<uint32_t>( ptr, vStates.size() );

	// Сохраняем состояния словарей.
	if( vStates.size() )
	{
		// Таблица смещений к состояниям словарей.
		auto shift = binary_data_skip<uint32_t>( ptr, vStates.size() );

		Node ** end = vStates.get_buffer() + vStates.size();
		for( Node ** node = vStates.get_buffer(); node < end; ++node )
		{
			// Сохраняем смещение к описанию очередного состояния словаря.
			*shift++ = ptr - static_cast<char*>( aBuffer );

			// Сохраняем идентификатор очередного словаря и его состояние.
			binary_data_save<uint32_t>( ptr, ( *node )->vDictId );

			binary_data_save_object( ptr, ( *node )->vDictState );
		}
	}

	return ptr - static_cast<char*>( aBuffer );
}

InfEngineErrors DictsStates::Load( const void * aBuffer, unsigned int aBufferSize )
{
	// Сбрасываем лог.
	vLog.clear();
	// Очищаем выделеную ранее память.
	vAllocator.Reset();

	// Проверяем размер буфера.
	if( aBufferSize < sizeof (uint32_t ) )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Выделяем память под список указателей на состояния словарей.
	vStates.resize( *reinterpret_cast<const uint32_t*>( aBuffer ) );
	if( vStates.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Проверяем размер буфера.
	if( aBufferSize < sizeof (uint32_t ) + sizeof (uint32_t ) * vStates.size() )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Указатель на таблицу сдвигов к состониям словарей.
	const uint32_t * state_shift = reinterpret_cast<const uint32_t*>( static_cast<const char*>( aBuffer ) + sizeof (uint32_t ) );
	// Загружаем состояния словарей.
	for( unsigned int state_n = 0; state_n < vStates.size(); ++state_n )
	{
		Node * node = nAllocateObject( vAllocator, Node );
		if( !node )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new((nMemoryAllocatorElementPtr*)node )Node( *reinterpret_cast<const uint32_t*>( static_cast<const char*>( aBuffer ) + *state_shift ), 0 );

		DictState::ReturnCode rt = DictState::RT_OK;
		if( DictState::RT_OK != ( rt = node->vDictState.Load( static_cast<const char*>( aBuffer ) + *state_shift + sizeof (uint32_t ),
						aBufferSize - *state_shift - sizeof (uint32_t ) ) ) )
		{
			const void * buf = static_cast<const char*>( aBuffer ) + *state_shift + sizeof (uint32_t );
			if( DictState::RT_INVALID_DATA == rt )
				ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );
			else if( DictState::RT_NO_MEMORY == rt )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			else
				ReturnWithError( INF_ENGINE_ERROR_INTERNAL, INF_ENGINE_STRING_ERROR_INTERNAL );
		}
		++state_shift;
		vStates[state_n] = node;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::Set( unsigned int aInfId, avector<Vars::Raw> & aSourceSession, unsigned int & aUnknownVariablesCounter )
{
	// Очистка памяти.
	Reset();

	aUnknownVariablesCounter = 0;

	aTextString Tmp;

	for( unsigned int i = 0; i < aSourceSession.size(); i++ )
	{
		Vars::Id VarId;

		// Выделение памяти под имя переменной.
		nlReturnCode nle = Tmp.assign( aSourceSession[i].vName );
		if( nle != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Очистка.
		AllTrim( Tmp );

		// Понижение регистра.
		if( SetLower( Tmp ) != nlrcSuccess )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Поиск имени переменной в регистре.
		VarId = vVarsRegistry->Search( Tmp.ToConstChar(), Tmp.size() );
		if( !VarId )
		{
			aUnknownVariablesCounter++;

			LogWarn( "Unknown variable: %s", aSourceSession[i].vName );

			continue;
		}

		// Установка значения переменной.
		InfEngineErrors iee = SetTextValue( VarId, aSourceSession[i].vValue, strlen( aSourceSession[i].vValue ) );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	// Установка идентификатора инфа.
	vInfId = aInfId;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::Update( const iAnswer & aAnswer, bool aInstructionsOnly )
{
	if( aAnswer.IsAutoVarsEnabled() && !aInstructionsOnly )
	{
		// Очистка переменной that_anchor.
		InfEngineErrors iee = SetTextValue( InfEngineVarThatAnchor, "", 0 );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		// Очистка переменной last_mark и переменной last_mark2.
		iee = SetTextValue( InfEngineVarLastMark, "", 0 );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
		iee = SetTextValue( InfEngineVarLastMark2, "", 0 );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );
	}

	// Проход по всем элементам ответа.
	for( unsigned int i = 0; i < aAnswer.Size(); i++ )
	{
		if( aAnswer[i].vType == iAnswer::Item::Type::Instruct && !aAnswer[i].vItem.vInstruct.vVarId.is_tmp() )
		{
			InfEngineErrors iee = SetTextValue( aAnswer[i].vItem.vInstruct.vVarId, aAnswer[i].vItem.vInstruct.vValue, aAnswer[i].vItem.vInstruct.vValue == nullptr ? 0 : strlen( aAnswer[i].vItem.vInstruct.vValue ) );
			if( iee != INF_ENGINE_SUCCESS )
				ReturnWithTrace( iee );
		}
	}

	if( !aInstructionsOnly )
	{
		try
		{
			vAnswersFlags.set( aAnswer.GetPatternId(), aAnswer.GetAnswerId() );
		}
		catch( ... )
		{
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		}
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::SetAnswerFlag( unsigned int aPatternId, unsigned int aAnswerId )
{
	try
	{
		vAnswersFlags.set( aPatternId, aAnswerId );
	}
	catch( ... )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}
	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::UnsetAnswerFlag( unsigned int aPatternId, unsigned int aAnswerId )
{
	try
	{
		vAnswersFlags.unset( aPatternId, aAnswerId );
	}
	catch( ... )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}
	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::Save( char * aBuffer ) const
{
	// Сохранение смещений.
	auto ptr = aBuffer;
	auto shifts = binary_data_skip<uint32_t>( ptr, 4 );

	// Сохранение md5-дайджеста реестра переменных.
	*shifts++ = ptr - aBuffer;
	unsigned char MD5Digest[16];
	vVarsRegistry->GetMainCheckSum( MD5Digest );
	binary_data_save<unsigned char>( ptr, MD5Digest, 16 );
	// Сохранение md5-чексуммы пользовательских словарей.
	binary_data_save<unsigned char>( ptr, vUserDictsCheckSum, 16 );
	// Сохранение идентификатора инфа.
	binary_data_save<uint32_t>( ptr, vInfId );

	// Сохранение переменных.
	*shifts++ = binary_data_alignment( ptr, aBuffer );
	ptr += Vars::Vector::Save( ptr );

	// Список состояний словарей.
	*shifts++ = binary_data_alignment( ptr, aBuffer );
	binary_data_save_object( ptr, vDictsStates );

	// Флаги сгенерированных шаблон-ответов.
	*shifts++ = binary_data_alignment( ptr, aBuffer );
	ptr += vAnswersFlags.save( ptr );

	binary_data_alignment( ptr, aBuffer );

	if( GetNeedMemorySize() != ptr - aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, "%u bytes instead %u", ptr - aBuffer, GetNeedMemorySize() );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::Save( unsigned int aSessionId, NanoLib::Cache & aCache )
{
	TimeStamp session_ts;
	RequestStat request_stat;

	// Создание ключа.
	char key[23];
	bzero( key, 23 );
	memcpy( key, "InfSession_", 11 );
	int ret = sprintf( key + 11, "%u", aSessionId );
	if( ret < 0 )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	unsigned keyLength = 11 + ret;

	// Выделение памяти под сохранение сессии.
	unsigned int Memory = Session::GetNeedMemorySize();
	char* buffer = static_cast<char*>( malloc( Memory ) );
	if( !buffer )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Сохранение в буфер.
	InfEngineErrors iee = Save( buffer );
	if( INF_ENGINE_SUCCESS != iee )
	{
		free( buffer );
		ReturnWithTrace( iee );
	}

	// Сохранение в кэш.
	TimeStamp cache_ts;
	unsigned int cache_tm = 0;
	NanoLib::Cache::ReturnCode nrc = aCache.InsertData( key, keyLength, buffer, Memory );
	// Время записи сессии в кэш.
	cache_tm = cache_ts.Interval();
	if( nrc != NanoLib::Cache::rcSuccess )
		ReturnWithTrace( INF_ENGINE_ERROR_CACHE );

	// Освобождение памяти.
	free( buffer );

	request_stat.ex.timer_cache_session_save = session_ts.Interval();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::Load( const char * aBuffer, unsigned int aBufferSize )
{
	// Проверка чексуммы.
	unsigned char MD5Digest[16];
	vVarsRegistry->GetMainCheckSum( MD5Digest );
	if( memcmp( aBuffer + reinterpret_cast<const uint32_t*>( aBuffer )[0], MD5Digest, 16 ) )
		// Тут не нужно сообщать об ошибке. Это выделеннный случай.
		return INF_ENGINE_ERROR_CHECKSUMM;

	// Загрузка md5-чексуммы пользовательских словарей.
	memcpy( vUserDictsCheckSum, aBuffer + reinterpret_cast<const uint32_t*>( aBuffer )[0] + 16, 16 );

	// Загрузка идентификатора инфа.
	vInfId = *reinterpret_cast<const uint32_t*>( aBuffer + reinterpret_cast<const uint32_t*>( aBuffer )[0] + 32 );

	// Загрузка данных.
	unsigned int memory;
	const char * ptr = aBuffer + reinterpret_cast<const uint32_t*>( aBuffer )[1];
	InfEngineErrors iee = Vars::Vector::Load( ptr, aBufferSize - ( aBuffer - ptr ), memory );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Загрузка состояний словарей.
	ptr = aBuffer + reinterpret_cast<const uint32_t*>( aBuffer )[2];
	if( ( iee = vDictsStates.Load( ptr, aBufferSize - ( ptr - aBuffer ) ) ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Загрузка флагов сгенерированных шаблон-ответов.
	try
	{
		vAnswersFlags.load( aBuffer + reinterpret_cast<const uint32_t*>( aBuffer )[3] );
	}
	catch( ... )
	{
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::Load( unsigned int aSessionId, NanoLib::Cache & aCache, RequestStat & aRequestStat )
{
	// Очистка.
	Reset();

	// Создание ключа.
	char key[23];
	bzero( key, 23 );
	memcpy( key, "InfSession_", 11 );
	int ret = sprintf( key + 11, "%u", aSessionId );
	if( ret < 0 )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	unsigned key_length = 11 + ret;

	// Загрузка данных из кэша.
	char * buffer;
	unsigned int buffer_size;

	TimeStamp session_ts;
	NanoLib::Cache::ReturnCode nce = aCache.GetData( key, key_length, buffer, buffer_size );
	// Время получения из кэша сессии.
	aRequestStat.vCacheLoadSessionTm = session_ts.Interval();
	aRequestStat.ex.timer_cache_session_load = session_ts.Interval();
	if( nce == NanoLib::Cache::rcUnsuccess )
		return INF_ENGINE_WARN_UNSUCCESS;
	else if( nce != NanoLib::Cache::rcSuccess )
		ReturnWithTrace( INF_ENGINE_ERROR_CACHE );

	// Загрузка сессии из буфера.
	InfEngineErrors iee = Load( buffer, buffer_size );
	if( INF_ENGINE_SUCCESS != iee )
	{
		free( buffer );
		if( INF_ENGINE_ERROR_CHECKSUMM == iee )
			return INF_ENGINE_ERROR_CHECKSUMM;
		ReturnWithTrace( iee );
	}

	// Освобождение памяти.
	free( buffer );

	vSessionId = aSessionId;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Session::Delete( unsigned int aSessionId, NanoLib::Cache & aCache )
{
	// Создание ключа.
	char key[23];
	bzero( key, 23 );
	memcpy( key, "InfSession_", 11 );
	int ret = sprintf( key + 11, "%u", aSessionId );
	if( ret < 0 )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	unsigned keyLength = 11 + ret;

	// Удаление из кэша.
	NanoLib::Cache::ReturnCode nce = aCache.DeleteData( key, keyLength );
	if( nce != NanoLib::Cache::rcSuccess )
		ReturnWithTrace( INF_ENGINE_ERROR_CACHE );

	return INF_ENGINE_SUCCESS;
}
