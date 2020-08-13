#include <nMemoryAllocator/nMemoryAllocator.hpp>

#include "csDataPack.h"

/** Подключаем внешние библиотеки. **/
#include <math.h>
#include <aptl/avector.h>

/** Версия протокола упаковки данных. */

// Версия протокола упаковки данных.
const char * gcsDataPackVersion = "1.0";

// Получение версии протокола упаковки данных.
const char * csDataPackVersion()
{
	return gcsDataPackVersion;
}


/**
 *  Функции настройки размера передаваемых пакетов.
 */
// Максимальный размер чанка.
size_t MaxChunkSize = 1 << 12;

// Получение максимального размера чанка.
size_t GetMaxChunkSize()
{
	return MaxChunkSize;
}

// Установка максимального размера чанка.
void SetMaxChunkSize( size_t aNewMaxChunkSize )
{
	if( aNewMaxChunkSize > 1 << 16 )
		MaxChunkSize = 1 << 16;
	else if( aNewMaxChunkSize < 1 << 4 )
		MaxChunkSize = 1 << 4;
	else
		MaxChunkSize = aNewMaxChunkSize;
}

// Вычисление объема памяти, необходимого для сохранения аттрибута.
inline size_t CalculateBlockSize( csdata_attr_t & aElement )
{
	size_t result = 0;

	if( aElement.attr_type & CSDATA_STRING )
		result = sizeof( uint16_t ) + aElement.attr_value_size + 1 + aElement.attr_name_size + 1;
	else
		result = sizeof( uint32_t ) + aElement.attr_name_size + 1;
	result += result % 2;

	return result;
}


/**
 *  Реализация упаковки. C вариант.
 */

// Статический аллокатор памяти.
nMemoryAllocator gAllocator;

// Упаковка данных для отправки.
extern "C" csdata_data_t * csDataPack( csdata_data_t * aSource, int aDataPackFlags )
{
	return csDataPack( aSource, gAllocator, aDataPackFlags );
}

// Распаковка полученных данных.
extern "C" csdata_data_t * csDataUnPack( csdata_data_t * aSource, int aFullAllocate )
{
	return csDataUnPack( aSource, gAllocator, aFullAllocate != 0 );
}

// Разрешение аллокатору использовать ранее выделенную память.
extern "C" void csAllocatorReset()
{
	gAllocator.Reset();
}

// Освобождение памяти, выделенной аллокатором.
extern "C" void csAllocatorFree()
{
	gAllocator.Free();
}

// Настройка шага выделения памяти для аллокатора.
extern "C" void csAllocatorSetMemoryStep( unsigned int aNewMemoryStep )
{
	gAllocator.SetMemoryStep( aNewMemoryStep );
}

// Получение общего объема памяти, выделенной аллокатором.
extern "C" size_t csAllocatorAllocatedMemorySize()
{
	return gAllocator.GetAllocatedMemorySize();
}


/**
 *  Реализация упаковки. С++ вариант.
 */

// Проверка структуры на корректность и вычисление длин имен и значений, там где они не проставлены.
bool csVerifyStructure( csdata_data_t * aSource )
{
	// Проверка аргументов.
	if( !aSource )
	{
		errno = EINVAL;
		return false;
	}

	// Верификация и просчет длин там, где это необходимо.
	for( unsigned int i = 0; i < aSource->attr_count; i++ )
	{
		csdata_attr_t & attr = aSource->attr_list[i];

		if( attr.attr_type & CSDATA_STRING )
		{
			// Элемент является строкой.
			if( !attr.attr_value.attr_string && attr.attr_value_size != 0 )
				attr.attr_value_size = 0;
			else if( attr.attr_value.attr_string && attr.attr_value_size == 0 )
				attr.attr_value_size = strlen( attr.attr_value.attr_string );
		}
		else
		{
			// Элемент является числом.
			if( attr.attr_value_size != sizeof( uint32_t ) )
				attr.attr_value_size = sizeof( uint32_t );
		}

		if( !attr.attr_name && attr.attr_name_size != 0 )
			attr.attr_name_size  = 0;
		else if( attr.attr_name && attr.attr_name_size == 0 )
			attr.attr_name_size = strlen( attr.attr_name );
	}

	return true;
}

// Перечисление типов чанков и необходимых для их использования масок.
namespace csChunkType
{
	// Маски типов чанков.
	int Multiple   = 0x8000;
	int Single     = 0x6000;
	int FirstPart  = 0x4000;
	int MiddlePart = 0x0000;
	int LastPart   = 0x2000;

	// Максимальное для количества элементов в чанке.
	int ElementsNumberMask = 0x0FFF;

	// Маска типа элемента.
	int ElementTypeMask = 0x1000;
};

// Создание чанка типа Single.
csdata_attr_t * CreateChunkSingle( csdata_attr_t & aElement, nMemoryAllocator & aAllocator, csdata_attr_t & aChunk )
{
	unsigned int tmp;

	// Вычисление необходимого размера памяти для сохранения элемента.
	tmp = CalculateBlockSize( aElement );

	// Выделение памяти.
	char * data = (char*)aAllocator.Allocate( sizeof( uint16_t ) + tmp );
	if( !data )
	{
		errno = ENOMEM;
		return NULL;
	}

	// Заполнение данных.
	aChunk.attr_name = (char*)"c";
	aChunk.attr_name_size = 1;

	aChunk.attr_type = (CSDATA_DATA_TYPES)( CSDATA_STRING | CSDATA_KEEP_ANAME | CSDATA_KEEP_AVALUE );

	aChunk.attr_value.attr_string = data;
	aChunk.attr_value_size = sizeof( uint16_t ) + tmp;

	// Сохранение элемента в чанк.
	if( aElement.attr_type & CSDATA_STRING )
	{
		// Элемент является строкой.

		*( (uint16_t*)data ) = csChunkType::Single | csChunkType::ElementTypeMask;
		data += sizeof( uint16_t );

		// Ссылка на имя.
		*( (uint16_t*)data ) = sizeof( uint16_t ) + aElement.attr_value_size + 1;
		data += sizeof( uint16_t );

		// Сохраняем значение.
		if( aElement.attr_value_size )
		{
			memcpy( data, aElement.attr_value.attr_string, aElement.attr_value_size );
			data += aElement.attr_value_size;
		}
		*data = '\0';
		data++;

		// Сохраняем имя.
		if( aElement.attr_name_size )
		{
			memcpy( data, aElement.attr_name, aElement.attr_name_size );
			data += aElement.attr_name_size;
		}
		*data = '\0';
		data++;
	}
	else
	{
		// Элемент является числом.
		*( (uint16_t*)data ) = csChunkType::Single & ~csChunkType::ElementTypeMask;
		data += sizeof( uint16_t );

		// Сохранение значения.
		*( (uint32_t*)data ) = aElement.attr_value.attr_int;
		data += sizeof( uint32_t );

		// Сохранение имени.
		if( aElement.attr_name_size )
		{
			memcpy( data, aElement.attr_name, aElement.attr_name_size );
			data += aElement.attr_name_size;
		}
		*data = '\0';
		data++;
	}

	return &aChunk;
}

// Создание чанка типа Multiple.
csdata_attr_t * CreateChunkMultiple( csdata_data_t * aSource, unsigned int aChunkSize,
									 unsigned int aBeginInd, unsigned int aEndInd,
									 nMemoryAllocator & aAllocator, csdata_attr_t & aChunk )
{
	// Проверка аргументов.
	if( !aSource )
	{
		errno = EINVAL;
		return NULL;
	}

	// Выделение памяти.
	char * data = (char*)aAllocator.Allocate( aChunkSize );
	if( !data )
	{
		errno = ENOMEM;
		return NULL;
	}

	// Заполнение данных.
	aChunk.attr_name = (char*)"c";
	aChunk.attr_name_size = 1;

	aChunk.attr_type = (CSDATA_DATA_TYPES)( CSDATA_STRING | CSDATA_KEEP_ANAME | CSDATA_KEEP_AVALUE );

	aChunk.attr_value.attr_string = data;
	aChunk.attr_value_size = aChunkSize;

	// Простановка типа чанка и количества элементов.
	*( (uint16_t*)data ) = csChunkType::Multiple | ( csChunkType::ElementsNumberMask & ( aEndInd - aBeginInd ) );
	data += sizeof( uint16_t );

	uint16_t * types = (uint16_t*)data;
	data += (int)ceil( ( (double)( aEndInd - aBeginInd ) ) / 16 ) * sizeof( uint16_t );

	uint16_t * shifts = (uint16_t*)data;
	data += ( aEndInd - aBeginInd ) * sizeof( uint16_t );

	// Сохраняем элементы.
	for( size_t i = 0; i < aEndInd - aBeginInd; i++ )
	{
		csdata_attr_t & attr = aSource->attr_list[aBeginInd + i];

		// Сохранение типа элемента.
		if( attr.attr_type & CSDATA_STRING )
			*( types + (int)floor( (double)i / 16 ) ) |= 1 << ( i % 16 );
		else
			*( types + (int)floor( (double)i / 16 ) ) &= ~( 1 << ( i % 16 ) );

		// Сохранение ссылки на элемент.
		*shifts = data - aChunk.attr_value.attr_string;
		shifts++;

		// Сохранение элемента.
		if( attr.attr_type & CSDATA_STRING )
		{
			// Сохранение ссылки на имя.
			*( (uint16_t*)data ) = sizeof( uint16_t ) + attr.attr_value_size + 1;
			data += sizeof( uint16_t );

			// Сохранение значения.
			if( attr.attr_value_size )
			{
				memcpy( data, attr.attr_value.attr_string, attr.attr_value_size );
				data += attr.attr_value_size;
			}
			*data = '\0';
			data++;
		}
		else
		{
			// Сохранение значения.
			*( (uint32_t*)data ) = attr.attr_value.attr_int;
			data += sizeof( uint32_t );
		}

		// Сохранение имени.
		if( attr.attr_name_size )
		{
			memcpy( data, attr.attr_name, attr.attr_name_size );
			data += attr.attr_name_size;
		}
		*data = '\0';
		data++;

		// Сдвиг.
		data += ( data - aChunk.attr_value.attr_string ) % 2;
	}

	return &aChunk;
}

// Упаковка данных для отправки по сокету.
csdata_data_t * csDataPack( csdata_data_t * aSource, nMemoryAllocator & aAllocator, int aDataPackFlags )
{
	// Проверка аргументов.
	if( !csVerifyStructure( aSource ) )
		return NULL;

	unsigned int tmp;


	// Info: Частичная упаковка не поддержана.
	if( aDataPackFlags & CSDATAPACK_FULL )
		aDataPackFlags |= CSDATAPACK_FULL;

	// Если упаковка не требуется.
	if( ( aDataPackFlags & CSDATAPACK_FULL ) != ( CSDATAPACK_FULL ) )
	{
		// Выделяем память.
		csdata_data_t * res = (csdata_data_t*)aAllocator.Allocate( sizeof( csdata_data_t ) );
		if( !res )
		{
			errno = ENOMEM;
			return NULL;
		}

		// Заполняем данные.
		res->body_ptr = NULL;
		res->body_size = 0;

		res->attr_count = aSource->attr_count + 1;

		// Выделение памяти под атрибуты.
		res->attr_list = (csdata_attr_t*)aAllocator.Allocate( sizeof( csdata_attr_t ) * ( aSource->attr_count + 1 ) );
		if( !res->attr_list )
		{
			errno = ENOMEM;
			return NULL;
		}

		// Тип упаковки.
		res->attr_list[0].attr_name = (char*)"t";
		res->attr_list[0].attr_name_size = 1;

		res->attr_list[0].attr_type = (CSDATA_DATA_TYPES)( CSDATA_INT | CSDATA_KEEP_ANAME );

		res->attr_list[0].attr_value.attr_int = aDataPackFlags;
		res->attr_list[0].attr_value_size = sizeof( uint32_t );

		// Копирование остальных объектов.
		memcpy( res->attr_list + 1, aSource->attr_list, aSource->attr_count * sizeof( csdata_attr_t ) );

		return res;
	}

	// Атрибутов нет.
	if( aSource->attr_count == 0 )
	{
		// Создаем пустой объект.
		csdata_data_t * res = (csdata_data_t*)aAllocator.Allocate( sizeof( csdata_data_t ) );
		if( !res )
		{
			errno = ENOMEM;
			return NULL;
		}

		res->attr_count = 1;
		// Выделение памяти под атрибуты.
		res->attr_list = (csdata_attr_t*)aAllocator.Allocate( sizeof( csdata_attr_t ) * ( 1 ) );
		if( !res->attr_list )
		{
			errno = ENOMEM;
			return NULL;
		}

		res->body_ptr = NULL;
		res->body_size = 0;

		// Заполнение списка аттрибутов.

		// Тип упаковки.
		res->attr_list[0].attr_name = (char*)"t";
		res->attr_list[0].attr_name_size = 1;

		res->attr_list[0].attr_type = (CSDATA_DATA_TYPES)( CSDATA_INT | CSDATA_KEEP_ANAME );

		res->attr_list[0].attr_value.attr_int = 0;
		res->attr_list[0].attr_value_size = sizeof( uint32_t );

		return res;
	}

	// Формирование чанков.
	// Optimization: Memory usage optimization point.
	avector<csdata_attr_t> Chunks;

	// Количество элементов сохраненных в чанки.
	unsigned int SavedElementsNum = 0;

	while( SavedElementsNum < aSource->attr_count )
	{
		// Тип чанка ( 3 бита ) и количество элементов ( 13 бит ).
		unsigned int memory = 2;

		// Вычисляем сколько полных блоков влезет в чанк.
		unsigned int i;
		for( i = SavedElementsNum; i < aSource->attr_count; i++ )
		{
			unsigned int addmem = 0;

			// Каждые 16 элементов требуют 16 бит на типы.
			addmem += ( i - SavedElementsNum ) % 16 == 0 ? sizeof( uint16_t ) : 0;

			// Каждый элемент требует ссылки на него ( 16 бит ).
			addmem += sizeof( uint16_t );

			// Размер блока.
			addmem += CalculateBlockSize( aSource->attr_list[i] );

			// Проверка, влезет ли элемент в текущий чанк.
			if( memory + addmem > MaxChunkSize )
				break;

			memory += addmem;
		}

		// Ни один блок не влезает в чанк.
		if( i == SavedElementsNum )
		{
			// Проверяем можно ли засунуть в чанк один элемент в режиме Single.
			tmp = CalculateBlockSize( aSource->attr_list[SavedElementsNum] );
			if( sizeof( uint16_t ) + tmp <= MaxChunkSize )
			{
				csdata_attr_t * chunk = Chunks.grow();
				if( Chunks.no_memory() )
				{
					errno = ENOMEM;
					return NULL;
				}

				// Формируем чанк типа Single.
				chunk = CreateChunkSingle( aSource->attr_list[i], aAllocator, *chunk );
				if( !chunk )
					return NULL;

				SavedElementsNum++;

				continue;
			}

			// Режем элемент и отправляем в несколько чанков.

			// Вычисление необходимого количества чанков.
			tmp = CalculateBlockSize( aSource->attr_list[i] );
			unsigned int ChunksNum = ceil( (double)tmp / ( MaxChunkSize - sizeof( uint16_t ) ) );

			// Флаг, показывающий, сохранено ли значение элемента.
			bool ValueSaved = false;
			unsigned int shift = 0;

			for( size_t j = 0; j < ChunksNum; j++ )
			{
				csdata_attr_t & attr = aSource->attr_list[i];

				// Выделение памяти.
				csdata_attr_t * chunk = Chunks.grow();
				if( Chunks.no_memory() )
				{
					errno = ENOMEM;
					return NULL;
				}

				unsigned int ChunkLength;
				if( j == ChunksNum - 1 )
					ChunkLength = tmp - j * ( MaxChunkSize - sizeof( uint16_t ) )  + sizeof( uint32_t );
				else
					ChunkLength = MaxChunkSize;
				char * data = (char*)aAllocator.Allocate( ChunkLength );
				if( !data )
				{
					errno = ENOMEM;
					return NULL;
				}

				// Заполненеи данных.
				chunk->attr_name = (char*)"c";
				chunk->attr_name_size = 1;

				chunk->attr_type = (CSDATA_DATA_TYPES)( CSDATA_STRING | CSDATA_KEEP_ANAME | CSDATA_KEEP_AVALUE );

				chunk->attr_value.attr_string = data;
				chunk->attr_value_size = ChunkLength;


				// Простановка типа чанка.
				if( j == 0 )
				{
					// Первый чанк.
					*( (uint16_t*)data ) = csChunkType::FirstPart | ( csChunkType::ElementsNumberMask & j );
					if( attr.attr_type & CSDATA_STRING )
						*( (uint16_t*)data ) |= csChunkType::ElementTypeMask;
					else
						*( (uint16_t*)data ) &= ~csChunkType::ElementTypeMask;
					data += sizeof( uint16_t );

					// Ссылка на имя.
					if( attr.attr_type & CSDATA_STRING )
					{
						*( (uint32_t*)data ) = sizeof( uint32_t ) + attr.attr_value_size + 1;
						data += sizeof( uint32_t );
					}
				}
				else if( j != ChunksNum - 1 )
				{
					// Средний чанк.
					*( (uint16_t*)data ) = csChunkType::MiddlePart | ( csChunkType::ElementsNumberMask & j );
					data += sizeof( uint16_t );
				}
				else
				{
					// Последний чанк.
					*( (uint16_t*)data ) = csChunkType::LastPart | ( csChunkType::ElementsNumberMask & j );
					data += sizeof( uint16_t );
				}

				if( !ValueSaved )
				{
					// Значение еще не сохранено.

					// Сохраняем значение.
					if( attr.attr_value_size + 1 - shift <= ChunkLength - ( data - chunk->attr_value.attr_string ) )
					{
						// Сохраняем значение.
						if( attr.attr_type & CSDATA_STRING )
						{
							if( attr.attr_value_size - shift )
								memcpy( data, attr.attr_value.attr_string + shift, attr.attr_value_size - shift );
						}
						else
						{
							if( attr.attr_value_size - shift )
								memcpy( data, &attr.attr_value.attr_int + shift, sizeof( uint32_t ) - shift );
						}
						data += attr.attr_value_size - shift;
						*data = '\0';
						data++;

						// Простановка флагов.
						ValueSaved = true;
						shift = 0;
					}
					else
					{
						// Сохраняем значение частично.
						unsigned int maxlength = ChunkLength - ( data - chunk->attr_value.attr_string );
						if( attr.attr_type & CSDATA_STRING )
							memcpy( data, attr.attr_value.attr_string + shift, maxlength );
						else
							memcpy( data, &attr.attr_value.attr_int + shift, maxlength );
						data += maxlength;

						// Простановка флагов.
						shift += maxlength;
					}
				}

				if( ValueSaved && ChunkLength - ( data - chunk->attr_value.attr_string ) > 0 )
				{
					// Значение уже сохранено.

					// Сохраняем имя.
					if( attr.attr_name_size + 1 - shift <= ChunkLength - ( data - chunk->attr_value.attr_string ) )
					{
						// Сохраняем имя.
						if( attr.attr_name_size - shift )
							memcpy( data, attr.attr_name + shift, attr.attr_name_size - shift );
						data += attr.attr_name_size - shift;
						*data = '\0';
						data++;

						// Простановка флагов.
						shift = 0;
					}
					else
					{
						// Сохраняем значение частично.
						unsigned int maxlength = ChunkLength - ( data - chunk->attr_value.attr_string );
						memcpy( data, attr.attr_name + shift, maxlength );
						data += maxlength;

						// Простановка флагов.
						shift += maxlength;
					}
				}
			}

			SavedElementsNum++;
		}
		else if( i - SavedElementsNum == 1 )
		{
			csdata_attr_t * chunk = Chunks.grow();
			if( Chunks.no_memory() )
			{
				errno = ENOMEM;
				return NULL;
			}

			// Формируем чанк типа Single.
			chunk = CreateChunkSingle( aSource->attr_list[i - 1], aAllocator, *chunk );
			if( !chunk )
				return NULL;

			SavedElementsNum++;

			continue;
		}
		// В чанк влезает более одного элемента.
		else
		{
			// Ограничение по количеству элементов в чанке.
			if( i - SavedElementsNum > 1 << 12 )
				i = SavedElementsNum + ( 1 << 12 );


			csdata_attr_t * chunk = Chunks.grow();
			if( !chunk )
			{
				errno = ENOMEM;
				return NULL;
			}

			// Формируем чанк типа Multiple.
			chunk = CreateChunkMultiple( aSource, memory, SavedElementsNum, i, aAllocator, *chunk );
			if( !chunk )
				return NULL;

			SavedElementsNum = i;
		}
	}

	// Создание объекта csdata_data_t.
	/*
	   csdata_data_t* res = (csdata_data_t*)aAllocator.Allocate( sizeof( csdata_data_t ) );
	   if( !res )
	   {
		errno = ENOMEM;
		return NULL;
	   }

	   res->body_ptr = NULL;
	   res->body_size = 0;

	   res->attr_count = Chunks.size()+3;
	   res->attr_list = (csdata_attr_t*)aAllocator.Allocate( sizeof( csdata_attr_t )*( Chunks.size()+3 ) );
	   if( !res->attr_list )
	   {
		errno = ENOMEM;
		return NULL;
	   }
	 */
	// Optimization: Strict memory optimization point.

	csdata_data_t * res = csdata_alloc_data( Chunks.size() + 3 );
	if( !res )
	{
		errno = ENOMEM;
		return NULL;
	}

	res->body_ptr = NULL;
	res->body_size = 0;


	// Заполнение списка аттрибутов.

	// Тип упаковки.
	res->attr_list[0].attr_name = (char*)"t";
	res->attr_list[0].attr_name_size = 1;

	res->attr_list[0].attr_type = (CSDATA_DATA_TYPES)( CSDATA_INT | CSDATA_KEEP_ANAME );

	res->attr_list[0].attr_value.attr_int = aDataPackFlags;
	res->attr_list[0].attr_value_size = sizeof( uint32_t );

	// Количество созданных чанков.
	res->attr_list[1].attr_name = (char*)"c";
	res->attr_list[1].attr_name_size = 1;

	res->attr_list[1].attr_type = (CSDATA_DATA_TYPES)( CSDATA_INT | CSDATA_KEEP_ANAME );

	res->attr_list[1].attr_value.attr_int = Chunks.size();
	res->attr_list[1].attr_value_size = sizeof( uint32_t );

	// Количество элементов в исходной структуре.
	res->attr_list[2].attr_name = (char*)"e";
	res->attr_list[2].attr_name_size = 1;

	res->attr_list[2].attr_type = (CSDATA_DATA_TYPES)( CSDATA_INT | CSDATA_KEEP_ANAME );

	res->attr_list[2].attr_value.attr_int = aSource->attr_count;
	res->attr_list[2].attr_value_size = sizeof( uint32_t );

	// Копирование чанков.
	memcpy( res->attr_list + 3, Chunks.get_buffer(), sizeof( csdata_attr_t ) * Chunks.size() );

	return res;
}

/**
 * Функция распаковки полученных данных.
 */
csdata_data_t * csDataUnPack( csdata_data_t * aSource, nMemoryAllocator & aAllocator, bool aFullAllocate )
{
	// Проверка аргументов.
	if( !csVerifyStructure( aSource ) )
		return NULL;

	// Проверка количества чанков.
	if( aSource->attr_count < 1 )
	{
		errno = EINVAL;
		return NULL;
	}

	// Получение типа упаковки.
	if( !( aSource->attr_list[0].attr_type & CSDATA_INT ) ||
		strcmp( aSource->attr_list[0].attr_name, "t" ) )
	{
		// Некорректный протокол. Тип не указан, или аргумент имеет некорректное имя.
		errno = EINVAL;
		return NULL;
	}
	int transport = aSource->attr_list[0].attr_value.attr_int;

	if( !( transport & ( CSDATAPACK_JOIN | CSDATAPACK_SPLIT ) ) )
	{
		//  Данные пришли без упаковки, а значит преобразования не требуют.

		// Выделение памяти.
		csdata_data_t * res = (csdata_data_t*)aAllocator.Allocate( sizeof( csdata_data_t ) );
		if( !res )
		{
			errno = ENOMEM;
			return NULL;
		}

		// Заполнение результирующей структуры.

		// Копирование body.
		res->body_size = aSource->body_size;
		if( aFullAllocate )
		{
			// Выделение памяти.
			res->body_ptr = (char*)aAllocator.Allocate( res->body_size + 1 );
			if( !res->body_ptr )
				return NULL;

			// Копирование.
			memcpy( res->body_ptr, aSource->body_ptr, res->body_size + 1 );
		}
		else
			res->body_ptr = aSource->body_ptr;

		// Копирование аттрибутов.
		res->attr_count = aSource->attr_count - 1;
		if( aFullAllocate )
		{
			// Выделение памяти под список аттрибутов.
			res->attr_list = (csdata_attr_t*)aAllocator.Allocate( sizeof( csdata_attr_t ) * res->attr_count );
			if( !res->attr_list )
				return NULL;

			// Заполнение аттрибутов.
			for( unsigned int i = 0; i < res->attr_count; i++ )
			{
				// Копирование имени атрибута.
				res->attr_list[i].attr_name_size = aSource->attr_list[i + 1].attr_name_size;

				res->attr_list[i].attr_name = (char*)aAllocator.Allocate( res->attr_list[i].attr_name_size + 1 );
				if( !res->attr_list[i].attr_name )
					return NULL;

				memcpy( res->attr_list[i].attr_name, aSource->attr_list[i + 1].attr_name, res->attr_list[i].attr_name_size + 1 );

				// Копирование значения атрибута.
				res->attr_list[i].attr_type = aSource->attr_list[i + 1].attr_type;
				if( res->attr_list[i].attr_type & CSDATA_STRING )
				{
					if( res->attr_list[i].attr_value_size == 0 )
						res->attr_list[i].attr_value_size = strlen( aSource->attr_list[i + 1].attr_value.attr_string );

					res->attr_list[i].attr_value.attr_string = (char*)aAllocator.Allocate( res->attr_list[i].attr_value_size + 1 );
					if( !res->attr_list[i].attr_value.attr_string )
						return NULL;

					memcpy( res->attr_list[i].attr_value.attr_string, aSource->attr_list[i + 1].attr_value.attr_string,
							res->attr_list[i].attr_value_size + 1 );
				}
				else
					res->attr_list[i].attr_value.attr_int = aSource->attr_list[i + 1].attr_value.attr_int;
			}
		}
		else
			res->attr_list = aSource->attr_list + 1;

		return res;

	}
	else if( !( transport & CSDATAPACK_JOIN ) || !( transport & CSDATAPACK_SPLIT ) )
	{
		// Info: Partial data packing is not supported.
	}

	// Проверка количества чанков.
	if( aSource->attr_count < 3 )
	{
		errno = EINVAL;
		return NULL;
	}

	// Проверка типа элемента.
	if( !( aSource->attr_list[1].attr_type & CSDATA_INT ) || strcmp( aSource->attr_list[1].attr_name, "c" ) )
	{
		errno = EINVAL;
		return NULL;
	}
	int ChunksNum = aSource->attr_list[1].attr_value.attr_int;

	// Проверка количества элементов в структуре.
	if( aSource->attr_count != (unsigned int)( 3 + ChunksNum ) )
	{
		errno = EINVAL;
		return NULL;
	}

	// Проверка количества элементов.
	if( !( aSource->attr_list[2].attr_type & CSDATA_INT ) || strcmp( aSource->attr_list[2].attr_name, "e" ) )
	{
		errno = EINVAL;
		return NULL;
	}
	unsigned int ElementsNum = aSource->attr_list[2].attr_value.attr_int;

	// Выделение памяти.
	csdata_attr_t * Elements = (csdata_attr_t*)aAllocator.Allocate( sizeof( csdata_attr_t ) * ElementsNum );
	if( !Elements )
	{
		errno = ENOMEM;
		return NULL;
	}

	unsigned int ind = 0;

	// Распаковка.
	for( unsigned int i = 3; i < aSource->attr_count; i++ )
	{
		csdata_attr_t & chunk = aSource->attr_list[i];

		// Проверка типа чанка.
		if( !( chunk.attr_type & CSDATA_STRING ) || strcmp( chunk.attr_name, "c" ) )
		{
			errno = EINVAL;
			return NULL;
		}

		// Проверка длины чанка.
		if( chunk.attr_value_size < sizeof( uint16_t ) )
		{
			errno = EINVAL;
			return NULL;
		}

		char * data = chunk.attr_value.attr_string;

		if( ( *( (uint16_t*)data ) & csChunkType::Single ) == csChunkType::Single )
		{
			if( ind >= ElementsNum )
			{
				errno = EINVAL;
				return NULL;
			}

			// Определяем тип элемента.
			Elements[ind].attr_type = (CSDATA_DATA_TYPES)( *( (uint16_t*)data ) & csChunkType::ElementTypeMask ?
														   CSDATA_STRING : CSDATA_INT );

			// Установка данных.
			if( Elements[ind].attr_type & CSDATA_STRING )
			{
				char * name = data + sizeof( uint16_t ) + ( (uint16_t*)data )[1];

				// Вычисление длины имени.
				Elements[ind].attr_name_size = strlen( name );

				// Установка имени.
				if( aFullAllocate )
				{
					// Выделение памяти.
					Elements[ind].attr_name = (char*)aAllocator.Allocate( Elements[ind].attr_name_size + 1 );
					if( !Elements[ind].attr_name )
						return NULL;

					// Копирование имени.
					memcpy( Elements[ind].attr_name, name, Elements[ind].attr_name_size + 1 );
				}
				else
					Elements[ind].attr_name = name;

				// Вычисление длины значения.
				Elements[ind].attr_value_size = name - ( data + sizeof( uint16_t ) ) - 1;

				// Установка значения.
				if( aFullAllocate )
				{
					// Выделение памяти.
					Elements[ind].attr_value.attr_string = (char*)aAllocator.Allocate( Elements[ind].attr_value_size + 1 );
					if( !Elements[ind].attr_value.attr_string )
						return NULL;

					// Копирование значения.
					memcpy( Elements[ind].attr_value.attr_string, data + sizeof( uint16_t ) + sizeof( uint16_t ),
							Elements[ind].attr_value_size + 1 );
				}
				else
					Elements[ind].attr_value.attr_string = data + sizeof( uint16_t ) + sizeof( uint16_t );
			}
			else
			{
				char * name = data + sizeof( uint16_t ) + sizeof( uint32_t );

				// Вычисление длины имени.
				Elements[ind].attr_name_size = strlen( name );

				// Установка имени.
				if( aFullAllocate )
				{
					// Выделение памяти.
					Elements[ind].attr_name = (char*)aAllocator.Allocate( Elements[ind].attr_name_size + 1 );
					if( !Elements[ind].attr_name )
						return NULL;

					// Копирование имени.
					memcpy( Elements[ind].attr_name, name, Elements[ind].attr_name_size + 1 );
				}
				else
					Elements[ind].attr_name = name;

				// Установка значения.
				Elements[ind].attr_value.attr_int = *( (uint32_t*)( data + sizeof( uint16_t ) ) );

				// Вычисление длины значения.
				Elements[ind].attr_value_size = sizeof( uint32_t );
			}
			ind++;
		}
		else if( *( (uint16_t*)data ) & csChunkType::Multiple )
		{
			// Определение количества элементов в чанке.
			int number = *( (uint16_t*)data ) & csChunkType::ElementsNumberMask;
			if( ind + number > (unsigned int)ElementsNum )
			{
				errno = EINVAL;
				return NULL;
			}

			uint16_t * shifts = (uint16_t*)( data + sizeof( uint16_t ) + (int)ceil( (double)number / 16 ) * sizeof( uint16_t ) );

			// Проход по всем элементам.
			for( size_t j = 0; j < (unsigned int)number; j++, ind++ )
			{
				// Определяем тип элемента.
				Elements[ind].attr_type = ( ( (uint16_t*)( data + sizeof( uint16_t ) ) + (int)floor( (double)j / 16 ) )[0] &
											( 1 << j % 16 ) ) ? CSDATA_STRING : CSDATA_INT;

				char * ptr = data + shifts[j];

				// Установка данных.
				if( Elements[ind].attr_type & CSDATA_STRING )
				{
					char * name = ptr + ( (uint16_t*)ptr )[0];

					// Вычисление длины имени.
					Elements[ind].attr_name_size = strlen( name );

					// Установка имени.
					if( aFullAllocate )
					{
						// Выделение памяти.
						Elements[ind].attr_name = (char*)aAllocator.Allocate( Elements[ind].attr_name_size + 1 );
						if( !Elements[ind].attr_name )
							return NULL;

						// Копирование имени.
						memcpy( Elements[ind].attr_name, name, Elements[ind].attr_name_size + 1 );
					}
					else
						Elements[ind].attr_name = name;

					// Вычисление длины значения.
					Elements[ind].attr_value_size =  name - ( ptr + sizeof( uint16_t ) ) - 1;

					// Установка значения.
					if( aFullAllocate )
					{
						// Выделение памяти.
						Elements[ind].attr_value.attr_string = (char*)aAllocator.Allocate( Elements[ind].attr_value_size + 1 );
						if( !Elements[ind].attr_value.attr_string )
							return NULL;

						// Копирование значения.
						memcpy( Elements[ind].attr_value.attr_string, ptr + sizeof( uint16_t ), Elements[ind].attr_value_size + 1 );
					}
					else
						Elements[ind].attr_value.attr_string = ptr + sizeof( uint16_t );
				}
				else
				{
					char * name = ptr + sizeof( uint32_t );

					// Вычисление длины имени.
					Elements[ind].attr_name_size = strlen( name );

					// Установка имени.
					if( aFullAllocate )
					{
						// Выделение памяти.
						Elements[ind].attr_name = (char*)aAllocator.Allocate( Elements[ind].attr_name_size + 1 );
						if( !Elements[ind].attr_name )
							return NULL;

						// Копирование имени.
						memcpy( Elements[ind].attr_name, name, Elements[ind].attr_name_size + 1 );
					}
					else
						Elements[ind].attr_name = name;

					// Установка значения.
					Elements[ind].attr_value.attr_int = *( (uint32_t*)ptr );

					// Вычисление длины значения.
					Elements[ind].attr_value_size = sizeof( uint32_t );
				}
			}
		}
		else if( *( (uint16_t*)data ) & csChunkType::FirstPart )
		{
			// Поиск последнего элемента.
			size_t j = i + 1;
			for(; j < aSource->attr_count && !( *( (uint16_t*)( aSource->attr_list[j].attr_value.attr_string ) ) & csChunkType::LastPart ); j++ ) ;
			if( j >= aSource->attr_count )
			{
				errno = EINVAL;
				return NULL;
			}

			// Вычисление объема необходимой памяти.
			unsigned int memory = 0;
			for( size_t k = i; k <= j; k++ )
			{
				// Проверка чанка.
				if( strcmp( aSource->attr_list[k].attr_name, "c" ) )
				{
					errno = EINVAL;
					return NULL;
				}

				memory += aSource->attr_list[k].attr_value_size - sizeof( uint16_t );
			}

			// Выделение памяти.
			char * element = (char*)aAllocator.Allocate( memory );
			if( !element )
			{
				errno = ENOMEM;
				return NULL;
			}

			// Копирование данных.
			char * ptr = element;
			for( size_t k = i; k <= j; k++ )
			{
				memcpy( ptr, aSource->attr_list[k].attr_value.attr_string + sizeof( uint16_t ), aSource->attr_list[k].attr_value_size - sizeof( uint16_t ) );
				ptr += aSource->attr_list[k].attr_value_size - sizeof( uint16_t );
			}

			// Определяем тип элемента.
			Elements[ind].attr_type = (CSDATA_DATA_TYPES)( *( (uint16_t*)aSource->attr_list[i].attr_value.attr_string ) & csChunkType::ElementTypeMask ? CSDATA_STRING : CSDATA_INT );

			// Установка данных.
			if( Elements[ind].attr_type & CSDATA_STRING )
			{
				// Установка имени.
				Elements[ind].attr_name = element + ( (uint32_t*)element )[0];

				// Установка значения.
				Elements[ind].attr_value.attr_string = element + sizeof( uint32_t );

				// Вычисление длины значения.
				Elements[ind].attr_value_size = ( (uint32_t*)element )[0] - sizeof( uint32_t ) - 1;

				// Вычисление длины имени.
				Elements[ind].attr_name_size = strlen( Elements[ind].attr_name );
			}
			else
			{
				// Установка имени.
				Elements[ind].attr_name = element + sizeof( uint32_t );

				// Установка значения.
				Elements[ind].attr_value.attr_int = *( (uint32_t*)element );

				// Вычисление длины значения.
				Elements[ind].attr_value_size = sizeof( uint32_t );

				// Вычисление длины имени.
				Elements[ind].attr_name_size = strlen( Elements[ind].attr_name );
			}
			ind++;
			i = j;
		}
		else
		{
			errno = EINVAL;
			return NULL;
		}
	}

	// Проверка количества элементов.
	if( ind != (unsigned int)ElementsNum )
	{
		errno = EINVAL;
		return NULL;
	}

	// Выделение памяти под основную структуру.
	csdata_data_t * res = (csdata_data_t*)aAllocator.Allocate( sizeof( csdata_data_t ) );
	if( !res )
	{
		errno = ENOMEM;
		return NULL;
	}

	// Заполнение структуры.
	res->attr_count = ElementsNum;
	res->attr_list = Elements;
	res->body_ptr = NULL;
	res->body_size = 0;

	return res;
}
