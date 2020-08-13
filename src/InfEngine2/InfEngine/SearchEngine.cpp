#include "SearchEngine.hpp"
#include <InfEngine2/InfEngine/InfFunctions/InternalFunctions/InternalFunction.hpp>
#include <InfEngine2/InfEngine/Signature/SignatureUtils.hpp>

#include <ctime>

#include <cstdlib>

using namespace InfPatternItems;

BSearchEngine::BSearchEngine()
{
	// Запуск рандомайзера.
	srand( (unsigned int)time( nullptr ) );
	std::srand( std::time( nullptr ) );
}

BSearchEngine::~BSearchEngine( )
{
	CloseBase( );
}

InfEngineErrors BSearchEngine::OpenBase( const char * aBasePath )
{
	// Проверка агрументов.
	if( !aBasePath )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка состояния.
	if( vFStorage )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "SearchEngine base is opened already" );

	// Создание объекта fstorage.
	vFStorage = fstorage_create( );
	if( !vFStorage )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create fstorage." );

	// Открытие fstorage.
	fstorage_connect_config fscc;
	memset( &fscc, 0, sizeof ( fstorage_connect_config ) );
	fscc.pid = FSTORAGE_PID_INF_ENGINE;
	int ret = fstorage_connect_shared( vFStorage, aBasePath, &fscc );
	if( ret != 0 )
	{
		CloseBase( );

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't open fstorage. Return Code: %d, %s", ret, aBasePath );
	}

    // Загрузка сигнатуры лингвистических данных.
    const char * dl_signature = nullptr;
    unsigned int dl_signature_size = 0;
    InfEngineErrors iee = GetSignature( dl_signature, dl_signature_size );
    if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

    // Создание сигнатуры движка.
    char * bin_signature = nullptr;
    unsigned int bin_signature_size = 0;
	if( INF_ENGINE_SUCCESS != ( iee = BuildBinarySignature( bin_signature, bin_signature_size, vMemoryAllocator ) ) )
		ReturnWithTrace( iee );

    // Проверка совместимости сигнатур.
	if( INF_ENGINE_SUCCESS != ( iee = CompareSignatures( bin_signature, dl_signature, vSignatureCompareInfo ) ) )
    {
        if( INF_ENGINE_WARN_UNSUCCESS == iee )
        {
			LogInfo( "Signatures comparison info: \"%s\"", vSignatureCompareInfo.ToConstChar() );
			ReturnWithError( INF_ENGINE_ERROR_DATA, "DL signature is incompatible to Engine signature." );
        }
		ReturnWithTrace( iee );
    }
    else
		LogInfo( "Signatures comparison info: SUCCESS" );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BSearchEngine::GetSignature( const char * aBasePath, const char *& aSignature, unsigned int & aSignatureSize, bool aEllipsis, fstorage_section_id aSectionID  )
{
	// Проверка агрументов.
	if( !aBasePath )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка состояния.
	if( vFStorage )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "SearchEngine base is opened already" );

	// Создание объекта fstorage.
	vFStorage = fstorage_create( );
	if( !vFStorage )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't create fstorage." );

	// Открытие fstorage.
	fstorage_connect_config fscc;
	memset( &fscc, 0, sizeof ( fstorage_connect_config ) );
	fscc.pid = aEllipsis ? FSTORAGE_PID_ELLIPSIS : FSTORAGE_PID_INF_ENGINE;
	int ret = fstorage_connect_shared( vFStorage, aBasePath, &fscc );
	if( ret != 0 )
	{
		CloseBase( );

		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't open fstorage. Return Code: %d, %s", ret, aBasePath );
	}

	InfEngineErrors iee = GetSignature( aSignature, aSignatureSize, aSectionID );
	if( INF_ENGINE_SUCCESS != iee )
		ReturnWithTrace( iee );

	char * buf = nAllocateObjects( vMemoryAllocator, char, aSignatureSize );
	if( !buf )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( buf, aSignature, aSignatureSize );
	aSignature = buf;

	CloseBase();

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors BSearchEngine::GetSignature( const char *& aSignature, unsigned int & aSignatureSize, fstorage_section_id aSectionID ) const
{
    // Загрузка секции с сигнатурой лингвистических данных.
    fstorage_section* sectdata = fstorage_get_section( vFStorage, aSectionID );
    if( !sectdata )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

    // Получение указателя на сигнатуру.
    aSignature = static_cast<const char *>( fstorage_section_get_all_data( sectdata ) );
    if( !aSignature )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, "Can't get fstorage pointer." );
    
    aSignatureSize = fstorage_section_get_size( sectdata );
    
	return INF_ENGINE_SUCCESS;
}

void BSearchEngine::CloseBase( )
{
	if( vFStorage )
	{
		fstorage_close( vFStorage );
		fstorage_destroy( vFStorage );
		vFStorage = nullptr;
	}
}

InfEngineErrors BSearchEngine::DetectLangs( const char * aUserRequest, unsigned int aUserRequestLength, Session &aSession, iAnswer &aAnswer ) const
{
	// Проверка аргументоа.
	if( !aUserRequest )
		aUserRequestLength = 0;

	// Флаги языков.
	bool langKazakh = false;

	// Проход по пользовательскому запросу.
	int32_t pos = 0;
	while( pos < aUserRequestLength )
	{
		// Выделение символа UTF.
		UChar32 ch;
		U8_NEXT( aUserRequest, pos, (int32_t)aUserRequestLength, ch );
		if( ch == U_SENTINEL )
		{
			// Некорректный UTF.
			break;
		}

		// Поиск казахских букв.
		if( ch == 0x04D8 || ch == 0x04D9 || ch == 0x0492 || ch == 0x0493 || ch == 0x049A || ch == 0x049B || ch == 0x04A2 || ch == 0x04A3 ||
				ch == 0x04E8 || ch == 0x04E9 || ch == 0x04B0 || ch == 0x04B1 || ch == 0x04EE || ch == 0x04EF || ch == 0x04AE || ch == 0x04AF ||
				ch == 0x04BA || ch == 0x04BB || ch == 0x0406 )
		{
			langKazakh = true;
		}
	}

	// Обновление сессии и ответа инфа.
	if( langKazakh )
	{
		InfEngineErrors iee = aSession.SetTextValue( InfEngineVarLangDetectedKazakh, "TRUE", 4 );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		aAnswer.AddInstruct( InfEngineVarLangDetectedKazakh, "TRUE", 4, false );
	}
	else
	{
		InfEngineErrors iee = aSession.SetTextValue( InfEngineVarLangDetectedKazakh, "", 0 );
		if( iee != INF_ENGINE_SUCCESS )
			ReturnWithTrace( iee );

		aAnswer.AddInstruct( InfEngineVarLangDetectedKazakh, "", 0, false );
	}

	return INF_ENGINE_SUCCESS;
}



InfEngineErrors ISearchEngine::OpenBase( const char * aBasePath )
{
	InfEngineErrors iee;

	// Проверка состояния.
	if( vBase.IsOpened( ) )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "SearchEngine base is opened already" );

	// Открытие базовой части.
	if( ( iee = BSearchEngine::OpenBase( aBasePath ) ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Открытие поисковой базы.
	if( ( iee = vBase.Open( vFStorage, nullptr, nullptr ) ) )
	{
		CloseBase( );

		ReturnWithTrace( iee );
	}

	return INF_ENGINE_SUCCESS;
}

void ISearchEngine::CloseBase( )
{
	vBase.Close( );
	vFinder.Reset( );

	BSearchEngine::CloseBase( );
}

InfEngineErrors ISearchEngine::Answer( Session &aSession, const char * aRequest, unsigned int aRequestLength, iAnswer &aAnswer, unsigned int &aWeight, RequestStat & aRequestStat )
{
	// Освобождение памяти, выделенной ранее при замене синонимов.
	vBase.ResetSynonymsMaps();

	// Сброс старой информации в контейнере для результата.
	aAnswer.Reset( );

	// Детекция языков.
	InfEngineErrors iee = DetectLangs( aRequest, aRequestLength, aSession, aAnswer );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Поиск подходящих шаблонов.

	// Настройка объектов поиска.
    InfBFinder::SearchData MainSearchData( vBase.GetIndexBase( ).GetIndexQBase( ), &vBase.GetIndexBase( ).GetIndexTBase( ), &vBase.GetIndexBase( ).GetIndexDBase( ),
                                           &vBase.GetIndexBase( ).GetSymbolymBase(), &vBase.GetIndexBase( ).GetSynonymBase( ), vBase.GetDLData( ), vBase.GetInfPersonRegistry( ) );
    iee = vFinder.Search( aRequest, aRequestLength, MainSearchData, aSession, aAnswer, aWeight, aRequestStat, true, false );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

Session * ISearchEngine::CreateSession( ) const
{
	Session * session = new Session( vBase.GetDLData( ).GetVarsRegistry( ) );
	if( session )
		session->SetDefaultValues( vBase.GetDLData( ).GetDefaultVars( ) );

	return session;
}

const ICoreRO & ISearchEngine::GetBase( ) const
{
	return vBase;
}


InfEngineErrors ExtISearchEngine::OpenBase( const char * aBasePath, const char * aFunctionsRootDir, const char * aFunctionsConfigPath )
{
	// Проверка состояния.
	if( vBase.IsOpened( ) )
		ReturnWithError( INF_ENGINE_ERROR_STATE, "DL base is opened already." );

	// Открытие основной базы.
	InfEngineErrors iee;
	if( ( iee = BSearchEngine::OpenBase( aBasePath ) ) != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	if( ( iee = vBase.Open( vFStorage, aFunctionsRootDir, aFunctionsConfigPath ) ) )
	{
		CloseBase( );

		ReturnWithTrace( iee );
	}

	// Открытие основания дополнительной базы.
	if( ( iee = vBase.OpenExtBaseFoundation( ) ) )
	{
		CloseBase( );

		ReturnWithTrace( iee );
	}

	// Инициализация внутренних DL-фукций.
	InternalFunctions::InitFunctions( vBase );

	LogInfo( "DL base \"%s\" is successfully opened.", aBasePath );

	return INF_ENGINE_SUCCESS;
}

void ExtISearchEngine::CloseBase( )
{
	vBase.Close( );
	vFinder.Reset( );

	BSearchEngine::CloseBase( );
}

InfEngineErrors ExtISearchEngine::Answer( Session &aSession, const char * aRequest, unsigned int aRequestLength,
										  iAnswer &aAnswer, unsigned int &aWeigth, bool &aAnswerFromUserBase, RequestStat & aRequestStat )
{
	// Освобождение памяти, выделенной ранее при замене синонимов.
	vBase.ResetSynonymsMaps();

	// Поиск подходящих шаблонов.

	// Сброс старой информации в контейнере для результата.
	aAnswer.Reset( );

	// Детекция языков.
	InfEngineErrors iee = DetectLangs( aRequest, aRequestLength, aSession, aAnswer );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Настройка объектов поиска.
    InfBFinder::SearchData MainSearchData( vBase.GetIndexBase( ).GetIndexQBase( ), &vBase.GetIndexBase( ).GetIndexTBase( ), &vBase.GetIndexBase( ).GetIndexDBase( ), &vBase.GetIndexBase( ).GetSymbolymBase(), &vBase.GetIndexBase( ).GetSynonymBase( ), vBase.GetDLData( ), vBase.GetInfPersonRegistry( ) );
    InfBFinder::SearchData ExtSearchData( vBase.GetExtIndexBase( ).GetIndexQBase( ), &vBase.GetExtIndexBase( ).GetIndexTBase( ), &vBase.GetExtIndexBase( ).GetIndexDBase( ), &vBase.GetIndexBase( ).GetSymbolymBase(), &vBase.GetIndexBase( ).GetSynonymBase( ), vBase.GetExtDLData( ), vBase.GetInfPersonRegistry( ) );
	iee = vFinder.Search( aRequest, aRequestLength, MainSearchData, ExtSearchData, aSession, aAnswer, aWeigth, aAnswerFromUserBase, aRequestStat );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	return INF_ENGINE_SUCCESS;
}

/**
 *  Функция для конвертации возвращаемого кода в формате InfEngineErrors в формат EllipsisSearchEngine::ReturnCode.
 */
inline EliSearchEngine::ReturnCode ConvertInfEngineErrorsToReturnCode( InfEngineErrors aReturnCode )
{
	switch( aReturnCode )
	{
	case INF_ENGINE_ERROR_NOFREE_MEMORY:
		return EliSearchEngine::rcErrorNoFreeMemory;
	case INF_ENGINE_ERROR_DATA:
	case INF_ENGINE_ERROR_CHECKSUMM:
	case INF_ENGINE_ERROR_OUT_OF_RANGE:
	case INF_ENGINE_ERROR_INV_DATA:
		return EliSearchEngine::rcErrorInvData;
	case INF_ENGINE_ERROR_FSTORAGE:
		return EliSearchEngine::rcErrorFStorage;
	case INF_ENGINE_SUCCESS:
		return EliSearchEngine::rcSuccess;
	case INF_ENGINE_ERROR_FAULT:
	case INF_ENGINE_ERROR_FILE:
	case INF_ENGINE_ERROR_STATE:
	case INF_ENGINE_ERROR_INV_ARGS:
	case INF_ENGINE_AGAIN:
	case INF_ENGINE_ERROR_INTERNAL:
	case INF_ENGINE_ERROR_CANT_OPEN:
	case INF_ENGINE_ERROR_CANT_PARSE:
	case INF_ENGINE_ERROR_CANT_CREATE:
	case INF_ENGINE_ERROR_CANT_READ:
	case INF_ENGINE_ERROR_CANT_WRITE:
	case INF_ENGINE_ERROR_CANT_CONNECT:
	case INF_ENGINE_ERROR_INV_PROTOCOL:
	case INF_ENGINE_ERROR_FATAL:
	case INF_ENGINE_WARN_UNSUCCESS:
	case INF_ENGINE_ERROR_LIMIT:
	case INF_ENGINE_ERROR_EMPTY:
	case INF_ENGINE_ERROR_INSERT:
	case INF_ENGINE_ERROR_ADD_TERM:
	case INF_ENGINE_ERROR_CACHE:
	case INF_ENGINE_WARN_NANOLIB:
		return EliSearchEngine::rcErrorFault;
	case INF_ENGINE_ERROR_ARGC:
	case INF_ENGINE_ERROR_INVALID_ARG:
	case INF_ENGINE_ERROR_ENTRY_POINT:
	case INF_ENGINE_ERROR_DLF:
	case INF_ENGINE_ERROR_FUNC_VERSION:
		return EliSearchEngine::rcErrorDLF;

	default:
		return EliSearchEngine::rcSuccess;
	}

	// Подавление предупреждения.
	return EliSearchEngine::rcSuccess;
}

EliSearchEngine::EliSearchEngine():
	vSession( vCore.GetDLData( ).GetVarsRegistry( ) ),
	vAnswer( false )
{
	vMatchedPattern.vConditionsRegistry = &vCore.GetDLData( ).GetConditionsRegistry( );
	vMatchedPattern.vDictsData = &vCore.GetDLData( );
	vMatchedPattern.vPatternsData = &vCore.GetDLData( );

	// Запуск рандомайзера.
	srand( (unsigned int)time( nullptr ) );
	std::srand( std::time( nullptr ) );
}

const char * EliSearchEngine::ConvertReturnCodeToString( ReturnCode aReturnCode ) const
{
	switch( aReturnCode )
	{
	case rcSuccess:
		return "Success";
	case rcUnsuccess:
		return "Unsuccess";
	case rcErrorInvArgs:
		return "Invalid arguments";
	case rcErrorFStorage:
		return "FStorage error";
	case rcErrorNoFreeMemory:
		return "Can't allocate memory";
	case rcErrorInvData:
		return "Invalid data";
	case rcErrorFault:
		return "Unknown error";
	case rcErrorBaseIsNotOpen:
		return "Main ellipsis base is not opened";
	case rcErrorBaseIsOpened:
		return "Main ellipsis base is opened already";
	case rcErrorDLF:
		return "DL function internal error";
	}

	// Подавление предупреждения.
	return nullptr;
}

EliSearchEngine::ReturnCode EliSearchEngine::LoadEllisisBase( const char * aEllipsisBasePath )
{
	// Проверка агрументов.
	if( !aEllipsisBasePath )
		ReturnWithError( rcErrorInvArgs, ConvertReturnCodeToString( rcErrorInvArgs ) );

	// Проверка состояния.
	if( vCore.IsOpened( ) )
		ReturnWithError( rcErrorBaseIsOpened, "SearchEngine base is opened already" );

	// Создание fstorage.
	vFStorageMain = fstorage_create( );
	if( !vFStorageMain )
		ReturnWithError( rcErrorFStorage, "Can't create fstorage." );

	// Открытие fstorage.
	fstorage_connect_config fscc;
	memset( &fscc, 0, sizeof ( fstorage_connect_config ) );
	fscc.pid = FSTORAGE_PID_ELLIPSIS;
	int ret = fstorage_connect_shared( vFStorageMain, aEllipsisBasePath, &fscc );
	if( ret != 0 )
	{
		fstorage_destroy( vFStorageMain );
		vFStorageMain = nullptr;

		CloseBase( );

		ReturnWithError( rcErrorFStorage, "Can't open fstorage. Return Code: %d", ret );
	}

	// Открытие ядра.
	InfEngineErrors iee;
	if( ( iee = vCore.Open( vFStorageMain ) ) != INF_ENGINE_SUCCESS )
	{
		CloseBase( );

		switch( iee )
		{
		case INF_ENGINE_ERROR_NOFREE_MEMORY:
			ReturnWithTrace( EliSearchEngine::rcErrorNoFreeMemory );
		case INF_ENGINE_ERROR_INV_DATA:
			ReturnWithTrace( EliSearchEngine::rcErrorInvData );
		default:
			ReturnWithTrace( EliSearchEngine::rcErrorFault );
		}
	}

	// Запись информационного сообщения в лог.
	LogInfo( "Ellipsis base \"%s\" is successfully opened.", aEllipsisBasePath );

	return rcSuccess;
}

void EliSearchEngine::CloseBase( )
{
	vFinderF.Reset( );
	vFinderS.Reset( );
	vFinderT.Reset( );

	vSession.Reset( );

	vCore.Close( );
	if( vFStorageMain )
	{
		fstorage_close( vFStorageMain );
		fstorage_destroy( vFStorageMain );
		vFStorageMain = nullptr;
	}
}

// @todo Нужно избавиться от сортировки.

/**
 *  Функиця сортировки кандидатов.
 */
int BFinderCandidatCmp( const void* a, const void* b )
{
	const BFinder::Candidat ** aa = (const BFinder::Candidat ** )a;
	const BFinder::Candidat ** bb = (const BFinder::Candidat ** )b;
	return ( ( *aa )->vPatternId > ( *bb )->vPatternId ) ? -1 : ( ( ( *aa )->vPatternId < ( *bb )->vPatternId ) ? 1 : 0 );
}

struct EliCandidatInd
{
	unsigned int i;
	unsigned int j;
	unsigned int k;
	unsigned int weight;
};

int SortEliCandidatsInd( const void* a, const void* b )
{
	const EliCandidatInd* elia = static_cast<const EliCandidatInd*>( a );
	const EliCandidatInd* elib = static_cast<const EliCandidatInd*>( b );

	return ( elia->weight > elib->weight ) ? -1 : ( elia->weight < elib->weight ) ? 1 : 0;
}

EliSearchEngine::ReturnCode EliSearchEngine::EllipsisDisclose(
		const char * aUserRequest, unsigned int aUserRequestLength,
		const char * aPreInfResponse, unsigned int aPreInfResponseLength,
		const char * aPreUserRequest, unsigned int aPreUserRequestLength,
		aTextString &aResult, aTextString &aEllipsisId,
		aTextString &aEllipsisPartialIDs )
{
	RequestStat request_stat;

	// Освобождение выделенной при прошлом запросе памяти.
	vTmpAllocator.Reset();

	// Проверка аргументов.
	if( !aUserRequest || !aPreInfResponse || !aPreUserRequest )
		ReturnWithError( rcErrorInvArgs, ConvertReturnCodeToString( rcErrorInvArgs ) );

	// Проверка состояния.
	if( !vFStorageMain )
		ReturnWithError( rcErrorBaseIsNotOpen, ConvertReturnCodeToString( rcErrorBaseIsNotOpen ) );

	aResult.clear( );
	aEllipsisId.clear( );
	aEllipsisPartialIDs.clear( );

	// Подготовка объекта для работы с подобранными шаблонами.
	vMatchedPattern.SetUserRequest( aUserRequest, aUserRequestLength );
	vMatchedPattern.SetInfPreResponse( aPreInfResponse, aPreInfResponseLength );
	vMatchedPattern.SetUserPreRequest( aPreUserRequest, aPreUserRequestLength );

	// Очистка контейнера для результата.
	vAnswer.Reset( );
	aEllipsisId.clear( );

	unsigned int Weight;

	// Настройка объектов поиска.
    InfBFinder::SearchData sdataF( vCore.GetIndexBase( ).GetIndexFBase( ), nullptr, nullptr, nullptr, nullptr, vCore.GetDLData( ), vCore.GetInfPersonRegistry( ) );
    InfEngineErrors iee = vFinderF.Search( aPreUserRequest, aPreUserRequestLength, sdataF, vSession, vAnswer, Weight, request_stat, false, true );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( ConvertInfEngineErrorsToReturnCode( iee ) );

	InfBFinder::SearchData sdataS( vCore.GetIndexBase( ).GetIndexSBase( ),
                                   nullptr, nullptr, nullptr, nullptr,
                                   vCore.GetDLData( ), vCore.GetInfPersonRegistry( ) );
	iee = vFinderS.Search( aPreInfResponse, aPreInfResponseLength, sdataS,
                           vSession, vAnswer, Weight, request_stat, false, true );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( ConvertInfEngineErrorsToReturnCode( iee ) );

	InfBFinder::SearchData sdataT( vCore.GetIndexBase( ).GetIndexTBase( ),
                                   nullptr, nullptr, nullptr, nullptr,
                                   vCore.GetDLData( ), vCore.GetInfPersonRegistry( ) );
	iee = vFinderT.Search( aUserRequest, aUserRequestLength, sdataT,
                           vSession, vAnswer, Weight, request_stat, false, true );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( ConvertInfEngineErrorsToReturnCode( iee ) );

	// Получение списков кандидатов и их сортировка.
	avector<BFinder::Candidat *>& p1 = vFinderF.GetResults();
	p1.qsort( BFinderCandidatCmp );

	avector<BFinder::Candidat *>& p2 = vFinderS.GetResults();
	p2.qsort( BFinderCandidatCmp );

	avector<BFinder::Candidat *>& p3 = vFinderT.GetResults();
	p3.qsort( BFinderCandidatCmp );

	static avector<EliCandidatInd> EliCandidates;
	EliCandidates.clear( );

	// Запись всех частично раскрытых эллипсисов.
	for( unsigned int i = 0; i < p3.size( ) && i < 10; i++ )
	{
		if( aEllipsisPartialIDs.empty( ) )
		{
			if( aEllipsisPartialIDs.assign( p3[i]->vPatternId ) != nlrcSuccess )
				ReturnWithError( rcErrorNoFreeMemory, ConvertReturnCodeToString( rcErrorNoFreeMemory ) );
		}
		else
		{
			if( aEllipsisPartialIDs.append( ", ", 2 ) != nlrcSuccess ||
					aEllipsisPartialIDs.append( p3[i]->vPatternId ) != nlrcSuccess )
				ReturnWithError( rcErrorNoFreeMemory, ConvertReturnCodeToString( rcErrorNoFreeMemory ) );
		}
	}

	// Выделение шаблонов, попавших во все списки.
	unsigned int i = 0, j = 0, k = 0;
	while( i < p1.size( ) && j < p2.size( ) && k < p3.size( ) )
	{
		// Подбираем 2ой шаблон к третьему.
		for(; j < p2.size( ) && p2[j]->vPatternId >= p3[k]->vPatternId; j++ );
		if( j >= p2.size( ) )
			break;

		for(; k < p3.size( ) && p2[j]->vPatternId + 1 < p3[k]->vPatternId; k++ );
		if( k >= p3.size( ) )
			break;

		// Проверка расстояния.
		if( p2[j]->vPatternId + 1 != p3[k]->vPatternId )
			continue;

		// Подбираем первый шаблон ко второму.
		for(; i < p1.size( ) && p1[i]->vPatternId >= p2[j]->vPatternId; i++ );
		if( i >= p1.size( ) )
			break;

		// Проверка расстояния.
		if( p1[i]->vPatternId + 1 != p2[j]->vPatternId )
		{
			k++;
			continue;
		}

		// Вычисление среднего веса.
		unsigned int weight = (unsigned int)( (double)p1[i]->vWeight / 3 +
											  (double)p2[j]->vWeight / 3 +
											  (double)p3[k]->vWeight / 3 );

		// Добавление в список.

		EliCandidates.push_back( (struct EliCandidatInd)
		{
									 i, j, k, weight
								 } );
		if( EliCandidates.no_memory( ) )
			ReturnWithError( rcErrorNoFreeMemory, ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

		// Переход к следующему.
		k++;
	}

	// Сортировка кандидатов.
	EliCandidates.qsort( SortEliCandidatsInd );

	static avector<unsigned int> AnswersInd;
	AnswersInd.clear( );

	for( unsigned int i = 0; i < EliCandidates.size( ); i++ )
	{
		vMatchedPattern.vMatchUserRequestData = p3[EliCandidates[i].k];
		vMatchedPattern.vMatchPreInfMatchData = p2[EliCandidates[i].j];
		vMatchedPattern.vMatchPreUserMatchData = p1[EliCandidates[i].i];

		static InfPatternRO pattern;
		vCore.GetDLData( ).GetPattern( p3[EliCandidates[i].k]->vPatternId, pattern );

		InfPatternItems::AnswerRO items;
		for( unsigned int j = 0; j < pattern.GetAnswersNum( ); j++ )
		{
			// Получение списка шаблон-ответов.
			items = pattern.GetAnswer( j );

			vAnswer.Reset( );

			if( ( iee = vMatchedPattern.CreateAnswer( vSession, items, vAnswer ) ) != INF_ENGINE_SUCCESS )
			{
				if( iee == INF_ENGINE_WARN_UNSUCCESS )
					continue;
				else
					ReturnWithTrace( ConvertInfEngineErrorsToReturnCode( iee ) );
			}

			aResult.clear( );

			if( ( iee = vAnswer.ToString( aResult ) ) != INF_ENGINE_SUCCESS )
				ReturnWithError( ConvertInfEngineErrorsToReturnCode( iee ),
								 "Can't create string by InfResult: %d", iee );

			if( !aResult.empty( ) )
			{
				if( aEllipsisId.assign( pattern.GetId( ) ) != nlrcSuccess )
					ReturnWithError( rcErrorNoFreeMemory, ConvertReturnCodeToString( rcErrorNoFreeMemory ) );

				return rcSuccess;
			}
		}
	}

	return rcUnsuccess;
}
