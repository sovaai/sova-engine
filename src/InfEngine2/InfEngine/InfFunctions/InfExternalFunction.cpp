#include "InfExternalFunction.hpp"
#include <dlfcn.h>
#include <cstring>

#include <InfEngine2/_Include/Errors.h>
#include "InternalFunctions/InternalFunction.hpp"

InfEngineErrors ExternalInfFunction::Load( const char * aFilePath, nMemoryAllocator & aMemoryAllocator )
{
	Reset();

	// Копирвание адреса динамической библиотеки.
	vLibPathLength = strlen( aFilePath );
	vLibPath = nAllocateObjects( aMemoryAllocator, char, vLibPathLength + 1 );
	if( !vLibPath )
	{
		Reset();
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}
	memcpy( vLibPath, aFilePath, vLibPathLength );
	vLibPath[vLibPathLength] = '\0';

	// Преобразование пути к библиотеке в глобальный. Это необходимо из-за особенностей функции dlopen.
	char LibraryPathBuffer[4096];
	const char * LibraryPath = aFilePath;
	if( ( LibraryPath = realpath( LibraryPath, LibraryPathBuffer ) ) == nullptr )
		LibraryPath = aFilePath;

	// Загрузка динамической библиотеки.
	vDLHandler = dlopen( LibraryPath, RTLD_NOW );
	if( !vDLHandler )
	{
		Reset();
		ReturnWithError( INF_ENGINE_ERROR_FILE, "%s: %s", INF_ENGINE_STRING_ERROR_FAILED_LOAD_DL, dlerror() );
	}

	// Полуение адреса функции, возвращающей информацию о библиотеке.
	FGetDLFucntionInfo GetDLFucntionInfo = reinterpret_cast<FGetDLFucntionInfo>( dlsym( vDLHandler, DLF_ENTRY_POINT ) );
	if( !GetDLFucntionInfo )
	{
		Reset();
		ReturnWithError( INF_ENGINE_ERROR_ENTRY_POINT, INF_ENGINE_STRING_ERROR_FAILED_FOUND_ENTRY_POINT );
	}

	// Получение информации о библиотеке.
	vDLFunctionInfo = GetDLFucntionInfo();
	if( !vDLFunctionInfo )
	{
		Reset();
		ReturnWithError( INF_ENGINE_ERROR_DLF, INF_ENGINE_STRING_ERROR_DLF );
	}

	// Проверка наличия в библиотеке нужной функции.
	if( !dlsym( vDLHandler, vDLFunctionInfo->name ) )
	{
		Reset();
		ReturnWithError( INF_ENGINE_ERROR_DLF, INF_ENGINE_STRING_ERROR_DLF );
	}

	// Сохранние информации о функции.
	vNameLength = strlen( vDLFunctionInfo->name );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExternalInfFunction::Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aResultSize ) const
{
	// Проверяем, был ли инициализирова объект.
	if( !vDLFunctionInfo )
		ReturnWithError( INF_ENGINE_ERROR_DLF, INF_ENGINE_STRING_ERROR_DLF );

	unsigned int memory = ExternalInfFunction::GetNeedMemorySize();

	if( aBufferSize < memory )
		ReturnWithError( INF_ENGINE_ERROR_DLF, INF_ENGINE_STRING_ERROR_DLF );

	char * ptr = aBuffer;

	// Сохранить базовую информацию о функции.
	unsigned int res = 0;
	InfEngineErrors iee = InfFunction::Save( ptr, aBufferSize, res );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );
	ptr += res;

	// Сохранить дину адреса библиотеки.
	memcpy( ptr, &vLibPathLength, sizeof( uint32_t ) );
	ptr += sizeof( uint32_t );

	// Сохранить адрес динамической библиотеки завершающим нулём.
	memcpy( ptr, vLibPath, vLibPathLength );
	ptr += vLibPathLength;
	*ptr = 0;
	++ptr;

	// Сохранить версию функции.
	memcpy( ptr, &vDLFunctionInfo->version, sizeof( DLFunctionVersion ) );
	ptr += sizeof( DLFunctionVersion );

	// Сохранить минимальную совместимую версию функции.
	memcpy( ptr, &vDLFunctionInfo->min_version, sizeof( DLFunctionVersion ) );
	ptr += sizeof( DLFunctionVersion );

	aResultSize = ptr - aBuffer;
	if( memory != aResultSize )
		ReturnWithError( INF_ENGINE_ERROR_CHECKSUMM, INF_ENGINE_STRING_ERROR_INCORRECT_SIZE_OF_SAVING_RESULT );

	return INF_ENGINE_SUCCESS;
}

void ExternalInfFunction::Init()
{
	InfFunction::Reset();

	vLibPath = nullptr;
	vLibPathLength = 0;
	vDLHandler = nullptr;
}

void ExternalInfFunction::Reset()
{
	InfFunction::Reset();

	vLibPath = nullptr;
	vLibPathLength = 0;

	if( vDLHandler )
	{
		dlclose( vDLHandler );
		vDLHandler = nullptr;
	}
}

void ExternalInfFunctionManipulator::Reset()
{
	// Освобождение динамической библиотеки.
	if( vDLHandler )
	{
		dlclose( vDLHandler );
		vDLHandler = nullptr;
	}

	InfFunctionManipulator::Reset();
}

InfEngineErrors ExternalInfFunctionManipulator::SetBuffer( const char * aBuffer )
{
	// Очстка манипулятора.
	Reset();

	InfFunctionManipulator::SetBuffer( aBuffer );

	// Преобразование пути к библиотеке в глобальный. Это необходимо из-за особенностей функции dlopen.
	char LibraryPathBuffer[4096];
	const char * LibraryPath = GetLibPath();
	if( ( LibraryPath = realpath( LibraryPath, LibraryPathBuffer ) ) == nullptr )
		LibraryPath = GetLibPath();

	vDLHandler = dlopen( LibraryPath, RTLD_NOW );
	if( !vDLHandler )
		return INF_ENGINE_SUCCESS;

	FGetDLFucntionInfo GetDLFucntionInfo = reinterpret_cast<FGetDLFucntionInfo>( dlsym( vDLHandler, DLF_ENTRY_POINT ) );
	if( !GetDLFucntionInfo )
	{
		Reset();
		return INF_ENGINE_ERROR_ENTRY_POINT;
	}

	vDLFunctionInfo = GetDLFucntionInfo();

	if( GetInfo()->version < *GetMinVersion() )
	{
		Reset();
		return INF_ENGINE_ERROR_FUNC_VERSION;
	}

	if( GetInfo()->DLFunctionInterfaceMinVer > DLFunctionInterfaceVersion )
	{
		Reset();
		return INF_ENGINE_ERROR_FUNC_API_VERSION;
	}

	if( GetInfo()->DLFunctionInterfaceVer < DLFunctionInterfaceVersion )
	{
		Reset();
		return INF_ENGINE_ERROR_FUNC_API_VERSION;
	}

	vFunction = reinterpret_cast<FDLFucntion>( dlsym( vDLHandler, GetName() ) );
	if( !vFunction )
	{
		Reset();
		return INF_ENGINE_ERROR_ENTRY_POINT;
	}

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ExternalInfFunctionManipulator::Load( const char * aFunctionPath )
{
	if( vDLHandler )
		dlclose( vDLHandler );

	// Преобразование пути к библиотеке в глобальный. Это необходимо из-за особенностей функции dlopen.
	char LibraryPathBuffer[4096];
	const char * LibraryPath = aFunctionPath;
	if( ( LibraryPath = realpath( LibraryPath, LibraryPathBuffer ) ) == nullptr )
		LibraryPath = aFunctionPath;

	vDLHandler = dlopen( LibraryPath, RTLD_NOW );
	if( !vDLHandler )
	{
		Reset();
		return INF_ENGINE_ERROR_FILE;
	}

	FGetDLFucntionInfo GetDLFucntionInfo = reinterpret_cast<FGetDLFucntionInfo>( dlsym( vDLHandler, DLF_ENTRY_POINT ) );
	if( !GetDLFucntionInfo )
	{
		Reset();
		return INF_ENGINE_ERROR_ENTRY_POINT;
	}

	vDLFunctionInfo = GetDLFucntionInfo();

	if( GetInfo()->version < *GetMinVersion() )
	{
		Reset();
		return INF_ENGINE_ERROR_FUNC_VERSION;
	}

	if( GetInfo()->DLFunctionInterfaceMinVer > DLFunctionInterfaceVersion )
	{
		Reset();
		return INF_ENGINE_ERROR_FUNC_API_VERSION;
	}

	if( GetInfo()->DLFunctionInterfaceVer < DLFunctionInterfaceVersion )
	{
		Reset();
		return INF_ENGINE_ERROR_FUNC_API_VERSION;
	}

	vFunction = reinterpret_cast<FDLFucntion>( dlsym( vDLHandler, GetName() ) );
	if( !vFunction )
	{
		Reset();
		return INF_ENGINE_ERROR_ENTRY_POINT;
	}

	return INF_ENGINE_SUCCESS;
}
