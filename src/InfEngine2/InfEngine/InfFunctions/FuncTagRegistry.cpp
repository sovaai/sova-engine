#include <InfEngine2/InfEngine/InfFunctions/FuncTagRegistry.hpp>
#include <cstring>

FuncTagRegistry::FuncTagRegistry( bool aROMode ):
	vTags( aROMode ? 1 : 5197111, 0.3)
{
	vROMode = aROMode;
	Create();
}

FuncTagRegistry::~FuncTagRegistry()
{   
}

InfEngineErrors FuncTagRegistry::Create()
{
	vRealSize = 0;
	vFunctionsNumber = 0;
	vMultipleCount = 0;

	InfEngineErrors iee = vFunctionsRegistryWR.Create();
	if( iee != INF_ENGINE_SUCCESS)
		ReturnWithError( iee, "Failed to create FucntionsRegistryWR" );

	iee = vFunctionsRegistryRO.Create();
	if( iee != INF_ENGINE_SUCCESS)
		ReturnWithError( iee, "Failed to create FucntionsRegistryRO" );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors FuncTagRegistry::Save( fstorage* aFStorage ) const

{ 
	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Сохранение реестра функций.
	InfEngineErrors iee = vFunctionsRegistryWR.Save( aFStorage );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Получение указателя на секцию.
	fstorage_section* SectData = fstorage_get_section( aFStorage, FSTORAGE_SECTION_FUNCTIONS_TAG_REGISTRY );
	if( !SectData )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Выделение памяти.
	unsigned int MemorySize = sizeof(uint32_t) +   // Общий размер данных.
							  sizeof(uint32_t) +                      // Количество тэгов-функций.
							  vTags.Count() * sizeof(uint32_t);       // Список размеров тэгов-функций.
	for( unsigned int tag_n = 0; tag_n < vTags.Count(); ++tag_n)
	{
		if ( vTags.GetItemSize(tag_n) )
			MemorySize += *vTags.GetItemSize( tag_n );    // Размер очередного тэга-функции.
		else
			ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Can't get tag-function item size." );
	}

	int ret = fstorage_section_realloc( SectData, MemorySize );
	if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Получение указателя на память.
	char* Buffer = static_cast<char*>(fstorage_section_get_all_data( SectData ));
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

	char* Ptr = Buffer;

	// Сохранение общего размера данных.
	*((uint32_t*)Ptr) = MemorySize;
	Ptr += sizeof(uint32_t);

	// Сохранение числа тэгов-функций в реестре.
	*((uint32_t*)Ptr) = vTags.Count();
	Ptr += sizeof(uint32_t);

	// Сохранение списка размеров тэгов-функций.
	for( unsigned int tag_n = 0; tag_n < vTags.Count(); ++tag_n)
	{
		*((uint32_t*)Ptr) = *vTags.GetItemSize( tag_n );
		Ptr += sizeof(uint32_t);
	}

	// Сохранение тэгов-функций.
	for( unsigned int tag_n = 0; tag_n < vTags.Count(); ++tag_n)
	{
		memcpy( Ptr, vTags.GetItem( tag_n), *vTags.GetItemSize( tag_n ) );
		Ptr += *vTags.GetItemSize( tag_n );
	}

	// Контрольная проверка сохранения.
	if( (unsigned int)(Ptr-Buffer) != MemorySize )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't save FuncTagRegistry" );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors FuncTagRegistry::Open( fstorage* aFStorage, const char * aRootDir, const char * aConfigPath )
{
	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Загрузка реестра функций.
	InfEngineErrors iee = vFunctionsRegistryRO.Open( aFStorage, aRootDir, aConfigPath );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithError( iee, "Failed to load Functions Registry" );

	// Получение секции с сохраненным реестром.
	fstorage_section* FStorageSection = fstorage_get_section( aFStorage, FSTORAGE_SECTION_FUNCTIONS_TAG_REGISTRY );
	if( !FStorageSection )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Получение указателя на выделенную память.
	char* Buffer = static_cast<char*>(fstorage_section_get_all_data( FStorageSection ));
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	char* Ptr = Buffer;

	// Проверка размера секции.
	unsigned int SectionSize = fstorage_section_get_size( FStorageSection );
	if( SectionSize < 2*sizeof(uint32_t) || SectionSize < *reinterpret_cast<uint32_t*>(Ptr) )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Общий размер сохранения.
	unsigned int Size = *reinterpret_cast<uint32_t*>(Ptr);
	Ptr += sizeof(uint32_t);

	// Количество тэгов-функций.
	unsigned int TagFuncCount = *reinterpret_cast<uint32_t*>(Ptr);
	Ptr += sizeof(uint32_t);

	// Размеры тэгов-функций.
	unsigned int CheckSum = 0;
	for( unsigned int tag_n = 0; tag_n < TagFuncCount; ++tag_n)
	{
		CheckSum += *reinterpret_cast<uint32_t*>(Ptr) + sizeof(uint32_t);
		vIndexRO.push_back(Node(nullptr, *reinterpret_cast<uint32_t*>(Ptr)));
		if( vIndexRO.no_memory() )
		{
			vIndexRO.clear();
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, "Can't allocate memory for tag-function index." );
		}

		Ptr += sizeof(uint32_t);
	}

	// Проверка корректности размеров.
	if( CheckSum + 2*sizeof(uint32_t) != Size)
	{
		vIndexRO.clear();
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Wrong tag-function index check summ." );
	}

	// Тэги-функции.
	for( unsigned int tag_n = 0; tag_n < TagFuncCount; ++tag_n)
	{
		vIndexRO[tag_n].vData = Ptr;
		Ptr += vIndexRO[tag_n].vSize;
	}

	return INF_ENGINE_SUCCESS;
}

void FuncTagRegistry::Close()
{    
	vFunctionsRegistryWR.Close();
	vFunctionsRegistryRO.Close();
}

InfEngineErrors FuncTagRegistry::Registrate( const InfPatternItems::TagFunction * aTagFunction, unsigned int & aFuncTagId )
{
	++vFunctionsNumber;

	unsigned int aNeedMemSize = aTagFunction->GetNeedMemorySize();
	unsigned int aResultMemorySize = 0;
	char * aBuffer = nAllocateObjects( vMemoryAllocator, char, aNeedMemSize);

	if( !aBuffer )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	memset(aBuffer, 0 , aNeedMemSize);
	aResultMemorySize += aTagFunction->Save( aBuffer );
	vRealSize += aNeedMemSize;

	unsigned int aPrevSize  = vTags.Count();

	const unsigned int * aId = vTags.Insert( aBuffer, aNeedMemSize, false );

	if ( !aId )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	if( aPrevSize != vTags.Count())
		if( aTagFunction->IsMultiple() )
			++vMultipleCount;

	aFuncTagId = *aId;

	vRealSize += aNeedMemSize;

	return INF_ENGINE_SUCCESS;
}

const char * FuncTagRegistry::GetTagData( unsigned int aFuncTagId ) const
{
	return aFuncTagId < vIndexRO.size() ? vIndexRO[aFuncTagId].vData : nullptr;
}

unsigned int FuncTagRegistry::CountRO() const
{
	return vIndexRO.size();
}

const FunctionsRegistry & FuncTagRegistry::GetFunctionsRegistry() const
{    
	if( vROMode )
		return vFunctionsRegistryRO;
	else
		return vFunctionsRegistryWR;
}

InfEngineErrors FuncTagRegistry::RegistryNewFunction( const char * aFuncPath, unsigned int & aFuncId )
{
	return vFunctionsRegistryWR.RegistryNew( aFuncPath, aFuncId );
}

InfEngineErrors FuncTagRegistry::RegistryNewFunction( const DLFunctionInfo * aFuncInfo, unsigned int & aFuncId )
{
	return vFunctionsRegistryWR.RegistryNew( aFuncInfo, aFuncId );
}

unsigned int FuncTagRegistry::GetRealSize() const
{
	return vRealSize;
}

unsigned int FuncTagRegistry::GetIndexedSize() const
{
	int aIndexSize = 0;
	for (unsigned int  i = 0 ; i < vTags.Count(); ++i)
	{
		if( vTags.GetItemSize(i) )
			aIndexSize += *vTags.GetItemSize(i);
		else
			return 0;
	}

	return aIndexSize;
}

unsigned int FuncTagRegistry::GetTagFunctionsCount() const
{
	return vFunctionsNumber;
}

unsigned int FuncTagRegistry::GetUniqueTagFunctionsCount() const
{
	return vTags.Count();
}

unsigned int FuncTagRegistry::GetMultipleTagFunctionsCount() const
{
	return vMultipleCount;
}

unsigned int FuncTagRegistry::GetHashTableMaxListLen() const
{
	return vTags.GetMaxListLen();
}

double FuncTagRegistry::GetHashTableAvgListLen() const
{
	return vTags.GetAvgListLen();
}
