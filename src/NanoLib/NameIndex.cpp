#include "NameIndex.hpp"

#include <cmath>

/**
 *  Минимальный размер сохраненного индекса.
 *
 * [ uint32_t ] Общий размер сохрененного индекса.
 * [ 64 + 6*sizeof(UINT4) ] MD5 состояние индекса.
 * [ uint32_t ] Количество элементов в индексе.
 * [ uint32_t ] Первый свободный идентификатор в индексе.
 * [ uint32_t ] Ссылка на обратный индекс.
 * [ uint32_t ] Контрольный элемент, стоящий в конце сохраненного блока и дублирующий общий размер сохраненного индекса.
 */
#define NanoLibNameIndexMinMemory 5*sizeof( uint32_t ) + sizeof( UINT4 )*6 + 64

using namespace NanoLib;

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *
 *      class NameIndex
 *
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

/**
 *  Описание сохранения индекса.
 * [ uint32_t ] Общий размер сохрененного индекса.
 * [ 64 + 6*sizeof(UINT4) ] MD5 состояние индекса.
 * [ uint32_t ] Количество элементов в индексе.
 * [ uint32_t ] Первый свободный идентификатор в индексе.
 * [ uint32_t ] Смещение от начала буфера до обратного индекса.
 * [ * ] Прямой поисковый индекс.
 * [ * ] Обратный поисковый индекс.
 * [ * ] Имена, добавленные в индекс и их идентификаторы.
 * [ uint32_t ] Контрольный элемент, стоящий в конце сохраненного блока и дублирующий общий размер сохраненного индекса.
 */

NameIndex::NameIndex( unsigned int vAverageNameLength, unsigned int vBaseNamesNumber, unsigned int vStepNamesNumber ):
	vAllocator( (vAverageNameLength+1+sizeof(niName))*vBaseNamesNumber, (vAverageNameLength+1+sizeof(niName))*vStepNamesNumber ),
	vRWIndex( vStepNamesNumber ),
	vRWReverseIndex( vStepNamesNumber )
{
	MD5Init( &vMD5State );
}

const char * NameIndex::ReturnCodeToString( ReturnCode aReturnCode ) const
{
	switch( aReturnCode )
	{
		case rcSuccess:
			return "Success";
		case rcElementExists:
			return "This element exists already";
		case rcErrorInvArgs:
			return "Invalid arguments";
		case rcErrorNoFreeMemory:
			return "Can't allocate memory";
		case rcErrorBufferTooSmall:
			return "There is no memory for saving index";
		case rcErrorFault:
			return "Unreconized internal error";
		case rcErrorInvState:
			return "Invalid index status";
		case rcErrorLimitIsReached:
			return "Index elements limit is reached";
		case rcErrorInvData:
			return "Invalid data";
		default:
			return nullptr;
	}
}

void NameIndex::Create()
{
	// Очистка от старых данных.
	Reset();

	// Вычисление размера памяти, необходимой для сохранения пустого индекса.
	vNeedMemorySize = NanoLibNameIndexMinMemory;

	// Простановка статуса.
	vState = stReadWrite;

	// Инициализация счетчика индекса.
	vNextId = 0;
}

NameIndex::ReturnCode NameIndex::Open( const void * aBuffer, unsigned int aBufferSize )
{
	// Проверка аргументов.
	if( !aBuffer )
		return rcErrorInvArgs;

	// Проверка минимального размера памяти.
	if( aBufferSize < NanoLibNameIndexMinMemory )
		return rcErrorInvData;

	const char * ptr = static_cast<const char *>(aBuffer);

	// Получение размера сохраненного индекса.
	unsigned int memory = *reinterpret_cast<const uint32_t*>(ptr);
	if( memory > aBufferSize || memory < NanoLibNameIndexMinMemory )
		return rcErrorInvData;
	ptr += sizeof(uint32_t);

	// Очистка от старых данных.
	Reset();

	// Получение MD5 состояния.
	memcpy( vMD5State.buffer, ptr, 64 );
	ptr += 64;
	memcpy( vMD5State.count, ptr, 2*sizeof(UINT4) );
	ptr += 2*sizeof(UINT4);
	memcpy( vMD5State.state, ptr, 4*sizeof(UINT4) );
	ptr += 4*sizeof(UINT4);
	MD5_CTX MD5Tmp = vMD5State;
	MD5Final( vMD5Digest, &MD5Tmp );

	// Загрузка индекса.
	vNeedMemorySize = memory;

	vBuffer = static_cast<const char *>(aBuffer);

	vNextId = ROGetNextID();

	vState = stReadOnly;

	// Проверка контрольной суммы.
	if( *(reinterpret_cast<const uint32_t*>(vBuffer+vNeedMemorySize)-1) != vNeedMemorySize )
	{
		Reset();
		return rcErrorInvData;
	}

	return rcSuccess;
}

NameIndex::ReturnCode NameIndex::Load( const void * aBuffer, unsigned int aBufferSize )
{
	// Проверка аргументов.
	if( !aBuffer )
		return rcErrorInvArgs;

	// Проверка минимального размера памяти.
	if( aBufferSize < NanoLibNameIndexMinMemory )
		return rcErrorInvData;

	const char * ptr = static_cast<const char *>(aBuffer);

	// Получение размера сохраненного индекса.
	unsigned int memory = *reinterpret_cast<const uint32_t*>(ptr);
	if( memory > aBufferSize || memory < NanoLibNameIndexMinMemory )
		return rcErrorInvData;
	ptr += sizeof(uint32_t);

	// Очистка от старых данных.
	Reset();

	// Получение MD5 состояния.
	memcpy( vMD5State.buffer, ptr, 64 );
	ptr += 64;
	memcpy( vMD5State.count, ptr, 2*sizeof(UINT4) );
	ptr += 2*sizeof(UINT4);
	memcpy( vMD5State.state, ptr, 4*sizeof(UINT4) );
	ptr += 4*sizeof(UINT4);
	MD5_CTX MD5Tmp = vMD5State;
	MD5Final( vMD5Digest, &MD5Tmp );

	// Загрузка индекса.
	vNeedMemorySize = memory;

	vBuffer = static_cast<const char *>(aBuffer);

	char * buffer = nullptr;
	if( ROIndexSize() )
	{
		buffer = static_cast<char*>(vAllocator.Allocate( ROIndexSize()*sizeof(niName) ));
		if( !buffer )
			return rcErrorNoFreeMemory;
	}

	vRWIndex.resize( ROIndexSize() );
	if( vRWIndex.no_memory() )
	{
		Reset();

		return rcErrorNoFreeMemory;
	}
	vRWReverseIndex.resize( ROIndexSize() );
	if( vRWReverseIndex.no_memory() )
	{
		Reset();

		return rcErrorNoFreeMemory;
	}

	for( unsigned int i = 0; i < ROIndexSize(); i++ )
	{
		niName* name = reinterpret_cast<niName*>(buffer);
		buffer += sizeof(niName);
		new ((nMemoryAllocatorElementPtr*)name) niName;

		name->vHash = ROHashByPos( i );
		name->vId = *ROIDByPos( i );
		name->vName = RONameByPos( i );
		name->vNameLength = *RONameLengthByPos( i );

		vRWIndex[i] = name;
		vRWReverseIndex[name->vId] = name;
	}

	vNextId = ROGetNextID();

	vState = stReadWrite;

	// Проверка контрольной суммы.
	if( *(reinterpret_cast<const uint32_t*>(vBuffer+vNeedMemorySize)-1) != vNeedMemorySize )
	{
		Reset();
		return rcErrorInvData;
	}

	return rcSuccess;
}

unsigned int NameIndex::Save( void * aBuffer ) const
{
	// Проверка статуса объекта.
	if( vState == stClosed )
		return 0;

	char * ptr = static_cast<char*>(aBuffer);

	if( vState == stReadWrite )
	{
		// Сохранение размера памяти, необходимой для сохранения индекса.
		*reinterpret_cast<uint32_t*>(ptr) = vNeedMemorySize;
		ptr += sizeof(uint32_t);

		// Сохранение MD5 состояния.
		memcpy( ptr, vMD5State.buffer, 64 );
		ptr += 64;
		memcpy( ptr, vMD5State.count, 2*sizeof(UINT4) );
		ptr += 2*sizeof(UINT4);
		memcpy( ptr, vMD5State.state, 4*sizeof(UINT4) );
		ptr += 4*sizeof(UINT4);

		// Сохранение количества имен в индексе.
		*reinterpret_cast<uint32_t*>(ptr) = vRWIndex.size();
		ptr += sizeof(uint32_t);

		// Сохранение следующего доступного идентификатора.
		*reinterpret_cast<uint32_t*>(ptr) = vNextId;
		ptr += sizeof(uint32_t);

		// Ссылка на обратный индекс.
		uint32_t* revshift = reinterpret_cast<uint32_t*>(ptr);
		ptr += sizeof(uint32_t);

		// Выделение места под индекс.
		uint32_t* index = reinterpret_cast<uint32_t*>(ptr);
		ptr += sizeof(uint32_t)*2*vRWIndex.size();

		// Выделение места под обратный индекс.
		uint32_t* reverse = reinterpret_cast<uint32_t*>(ptr);
		ptr += sizeof(uint32_t)*vRWReverseIndex.size();

		*revshift = reinterpret_cast<char*>(reverse)-static_cast<char*>(aBuffer);

		// Сохранение элементов.
		for( unsigned int i = 0; i < vRWIndex.size(); i++ )
		{
			// Сохранение индекса по хэшам.
			*index = vRWIndex[i]->vHash;
			index++;
			*index = ptr-static_cast<char*>(aBuffer);
			index++;

			// Сохранение индекса по идентификаторам.
			reverse[vRWIndex[i]->vId] = ptr-static_cast<char*>(aBuffer);

			// Сохранение идентификатора имени.
			*reinterpret_cast<uint32_t*>(ptr) = vRWIndex[i]->vId;
			ptr += sizeof(uint32_t);

			// Сохранение длины имени.
			*reinterpret_cast<uint32_t*>(ptr) = vRWIndex[i]->vNameLength;
			ptr += sizeof(uint32_t);

			// Сохранение имени.
			memcpy( ptr, vRWIndex[i]->vName, vRWIndex[i]->vNameLength+1 );
			ptr += vRWIndex[i]->vNameLength+1;

			// Сдвиг.
			unsigned int size = ptr-static_cast<char*>(aBuffer);
			ptr += size%sizeof(uint32_t) ? sizeof(uint32_t)-size%sizeof(uint32_t) : 0;
		}

		// Добавление контрольных данных для сохранения.
		*reinterpret_cast<uint32_t*>(ptr) = vNeedMemorySize;
		ptr += sizeof(uint32_t);
	}
	else if( vState == stReadOnly )
	{
		memcpy( aBuffer, vBuffer, vNeedMemorySize );
	}
	else
		return 0;

	return vNeedMemorySize;
}

void NameIndex::Reset()
{
	vState = stClosed;

	vAllocator.Reset();

	vRWIndex.clear();

	vRWReverseIndex.clear();

	vNextId = 0;

	vNeedMemorySize = 0;

	vBuffer = nullptr;

	MD5Init( &vMD5State );
}

void NameIndex::Destroy()
{
	vState = stClosed;

	vAllocator.Free();

	vRWIndex.free_buffer();

	vRWReverseIndex.free_buffer();

	vNextId = 0;

	vNeedMemorySize = 0;

	vBuffer = nullptr;

	MD5Init( &vMD5State );
}

NameIndex::ReturnCode NameIndex::AddName( const char * aName, unsigned int aNameLength, unsigned int & aId )
{
	// Проверка аргументов.
	if( !aName || aNameLength >= (((unsigned int)1<<24)-1) )
		return rcErrorInvArgs;

	// Проверка статуса.
	if( vState != stReadWrite )
		return rcErrorInvState;

	// Проверка количества элементов в индексе.
	if( vNeedMemorySize+sizeof(niName)+5*sizeof(uint32_t)+aNameLength+4 >= 0xFFFFFFFF )
		return rcErrorLimitIsReached;

	// Поиск элемента.
	bool exists;
	unsigned hash;
	unsigned int pos = SearchRW( aName, aNameLength, exists, hash );
	if( exists )
	{
		// Элемент уже существует.
		aId = vRWIndex[pos]->vId;

		return rcElementExists;
	}

	// Выделяем память под имя.
	niName * name = static_cast<niName*>(vAllocator.Allocate( sizeof(niName) ));
	if( !name )
		return rcErrorNoFreeMemory;

	name->vHash = hash;
	name->vId = vNextId++;

	char * string = static_cast<char*>(vAllocator.Allocate( aNameLength+1 ));
	if( !string )
		return rcErrorNoFreeMemory;
	memcpy( string, aName, aNameLength );
	string[aNameLength] = '\0';

	name->vName = string;
	name->vNameLength = aNameLength;

	// Выделение памяти под обратный индекс.
	vRWReverseIndex.grow();
	if( vRWReverseIndex.no_memory() )
		return rcErrorNoFreeMemory;

	// Выделение памяти под прямой индекс.
	vRWIndex.grow();
	if( vRWIndex.no_memory() )
	{
		// Восстановление целосности объекта.
		vRWReverseIndex.pop_back();

		return rcErrorNoFreeMemory;
	}

	// Перемещаем имена с большим показателем сравнения.
	memmove( vRWIndex.get_buffer()+pos+1, vRWIndex.get_buffer()+pos, (vRWIndex.size()-pos-1)*sizeof(niName*) );

	// Установка индекса.
	vRWIndex[pos] = name;

	// Установка обратного индекса.
	vRWReverseIndex[name->vId] = name;

	// Увеличение размера памяти, необходимого для сохранения индекса.
	vNeedMemorySize += 5*sizeof(uint32_t)+aNameLength+1;
	vNeedMemorySize += vNeedMemorySize%sizeof(uint32_t) ? sizeof(uint32_t)-vNeedMemorySize%sizeof(uint32_t) : 0;

	// Возвращаем индекс элемента.
	aId = name->vId;

	// Изменяем md5 состояние.
	MD5Update( &vMD5State, reinterpret_cast<const unsigned char*>(aName), aNameLength );
	MD5Update( &vMD5State, reinterpret_cast<unsigned char*>(&aId), sizeof(unsigned int) );

	// Генерируем md5 чексумму.
	MD5_CTX MD5Tmp = vMD5State;
	MD5Final( vMD5Digest, &MD5Tmp );

	return rcSuccess;
}

const unsigned int* NameIndex::Search( const char * aName, unsigned int aNameLength ) const
{
	// Проверка аргументов.
	if( !aName )
		return nullptr;

	// Проверка статуса.
	if( vState == stClosed )
		return nullptr;

	// Поиск элемента.
	bool exists;
	unsigned int hash;
	if( vState == stReadWrite )
	{
		unsigned int pos = SearchRW( aName, aNameLength, exists, hash );

		// Получение нужного элемента.
		if( !exists )
			return nullptr;
		else
			return &(vRWIndex[pos]->vId);
	}
	else if( vState == stReadOnly )
	{
		// Поиск элемента по хэш сумме.
		unsigned int pos = SearchRO( aName, aNameLength, exists, hash );

		// Получение нужного элемента.
		if( !exists )
			return nullptr;
		else
			return ROIDByPos( pos );
	}
	else
		return nullptr;
}

const char * NameIndex::GetName( unsigned int aId, unsigned int & aNameLength ) const
{
	// Проверка статуса.
	if( vState == stReadWrite )
	{
		if( aId >= vRWReverseIndex.size() )
		{
			aNameLength = 0;
			return nullptr;
		}
		else
		{
			aNameLength = vRWReverseIndex[aId]->vNameLength;
			return vRWReverseIndex[aId]->vName;
		}
	}
	else if( vState == stReadOnly )
	{
		if( aId >= ROIndexSize() )
		{
			aNameLength = 0;
			return nullptr;
		}
		else
		{
			aNameLength = *RONameLengthByID( aId );
			return RONameByID( aId );
		}
	}
	else
	{
		aNameLength = 0;
		return nullptr;
	}
}

unsigned int NameIndex::Hash( const char * aName, unsigned int aNameLength ) const
{
	unsigned int hash = aNameLength;

	// Обрезаем длину до 24х бит.
	hash &= 0x00FFFFFF;
	hash <<= 8;

	// Вычисляем xor всех символов.
	char rxor = 0;
	for( unsigned int i = 0; i < aNameLength; i++ )
		rxor ^= aName[i];

	// Соединяем данные.
	hash |= (0x000000FF & rxor);

	return hash;
}

unsigned int NameIndex::SearchRW( const char * aName, unsigned int aNameLength, bool & aExists, unsigned int & aHash ) const
{
	// Проверка аргументов.
	if( !aName )
		return rcErrorInvArgs;

	// Вычисление хэш кода.
	aHash = Hash( aName, aNameLength );

	// Проверка на наличие хотя бы одного элемента в индексе.
	if( vRWIndex.size() == 0 )
	{
		aExists = false;
		return 0;
	}

	// Поиск совпадения по хэшу.
	unsigned int found;

	unsigned int first = 0;
	unsigned int last = vRWIndex.size();

	if( aHash == vRWIndex[first]->vHash )
	{
		aExists = true;
		found = first;
	}
	else if( aHash == vRWIndex[last-1]->vHash )
	{
		aExists = true;
		found = last-1;
	}
	else if( aHash > vRWIndex[last-1]->vHash )
	{
		aExists = false;
		return last;
	}
	else if( aHash < vRWIndex[first]->vHash )
	{
		aExists = false;
		return first;
	}
	else
	{
		while( last > first+1 )
		{
			unsigned int center = (unsigned int)floor((double)(first+last)/2);

			if( vRWIndex[center]->vHash == aHash )
			{
				aExists = true;
				found = center;
				break;
			}
			else if( vRWIndex[center]->vHash > aHash )
				last = center;
			else
				first = center;
		}

		if( last <= first+1 )
		{
			// Элемент не был найден.
			aExists = false;

			return last;
		}
	}

	// Производим поиск среди элементов с одинаковым хэш кодом.

	// Поиск границ области с одинаковым хэш кодом.
	for( first = found; first > 0 && vRWIndex[first-1]->vHash == aHash; first-- );
	for( last = found+1; last < vRWIndex.size() && vRWIndex[last]->vHash == aHash; last++ );

	if( !strncmp( aName, vRWIndex[first]->vName, aNameLength ) )
	{
		aExists = true;
		return first;
	}
	else if( !strncmp( aName, vRWIndex[last-1]->vName, aNameLength ) )
	{
		aExists = true;
		return last-1;
	}
	else if( strncmp( aName, vRWIndex[last-1]->vName, aNameLength ) > 0 )
	{
		aExists = false;
		return last;
	}
	else if( strncmp( aName, vRWIndex[first]->vName, aNameLength ) < 0 )
	{
		aExists = false;
		return first;
	}
	else
	{
		while( last > first+1 )
		{
			unsigned int center = (unsigned int)floor((double)(first+last)/2);

			if( !strncmp( vRWIndex[center]->vName, aName, aNameLength ) )
			{
				aExists = true;
				return center;
			}
			else if( strncmp( aName, vRWIndex[center]->vName, aNameLength ) < 0 )
				last = center;
			else
				first = center;
		}

		aExists = false;

		return last;
	}
}

unsigned int NameIndex::SearchRO( const char * aName, unsigned int aNameLength, bool & aExists, unsigned int& aHash ) const
{
	// Проверка аргументов.
	if( !aName )
		return rcErrorInvArgs;

	// Вычисление хэш кода.
	aHash = Hash( aName, aNameLength );

	// Проверка на наличие хотя бы одного элемента в индексе.
	if( ROIndexSize() == 0 )
	{
		aExists = false;
		return 0;
	}

	// Поиск по хэшу.

	unsigned int found;

	unsigned int first = 0;
	unsigned int last = ROIndexSize();

	if( aHash == ROHashByPos( first ) )
	{
		aExists = true;
		found = first;
	}
	else if( aHash == ROHashByPos( last-1 ) )
	{
		aExists = true;
		found = last-1;
	}
	else if( aHash > ROHashByPos( last-1 ) )
	{
		aExists = false;
		return last;
	}
	else if( aHash < ROHashByPos( first ) )
	{
		aExists = false;
		return first;
	}
	else
	{
		while( last > first+1 )
		{
			unsigned int center = (unsigned int)floor((double)(first+last)/2);

			if( ROHashByPos( center ) == aHash )
			{
				aExists = true;
				found = center;
				break;
			}
			else if( ROHashByPos( center ) > aHash )
				last = center;
			else
				first = center;
		}

		if( last <= first+1 )
		{
			aExists = false;

			return last;
		}
	}

	// Производим поиск среди элементов с одинаковым хэш кодом.

	// Поиск границ.
	for( first = found; first > 0 && ROHashByPos( first-1 ) == aHash; first-- );
	for( last = found+1; last < ROIndexSize() && ROHashByPos( last ) == aHash; last++ );

	if( !strncmp( aName, RONameByPos( first ), aNameLength ) )
	{
		aExists = true;
		return first;
	}
	else if( !strncmp( aName, RONameByPos( last-1 ), aNameLength ) )
	{
		aExists = true;
		return last-1;
	}
	else if( !strncmp( aName, RONameByPos( last-1 ), aNameLength ) > 0 )
	{
		aExists = false;
		return last;
	}
	else if( !strncmp( aName, RONameByPos( first ), aNameLength ) < 0 )
	{
		aExists = false;
		return first;
	}
	else
	{
		while( last > first+1 )
		{
			unsigned int center = (unsigned int)floor((double)(first+last)/2);

			if( !strncmp( RONameByPos( center ), aName, aNameLength ) )
			{
				aExists = true;
				return center;
			}
			else if( strncmp( aName, RONameByPos( center ), aNameLength ) < 0 )
				last = center;
			else
				first = center;
		}

		aExists = false;

		return last;
	}
}
