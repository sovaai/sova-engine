#include "Blocks.hpp"

#if defined(__FreeBSD__) or defined(__APPLE__)
	#define fopen64 fopen
	#define fseeko64 fseek
#endif /** __FreeBSD__ */

bool Blocks::Add( const char * aData, unsigned int aSize )
{
	// Поместить в контейнер новый блок данных нужного размера.
	char * Buffer = Add( aSize );
	if( !Buffer )
		return false;

	// Записать данные в блок.
	memcpy( Buffer, aData, aSize );

	return true;
}

char * Blocks::Add( unsigned int aSize )
{
	// Если в памяти находятся данные, загруженные для чтения.
	if( !vNewData && vBlocks.size() )
	{
		// Очищаем память.
		vBlocks.clear();
		vMemoryAllocator.Reset();
		vUsedMemory = 0;
		vFirstBlockNum = -1;
	}

	// Выгрузить при необходимости данные из памяти в файл.
	if( vThresHold && vThresHold <= vUsedMemory + aSize && vBlocks.size() )
		if( !Dump() )
			return nullptr;

	// Выделение памяти для копирования в контейнер нового блока данных.
	Block NewBlock( nAllocateObjects( vMemoryAllocator, char, aSize ), aSize );
	if( !NewBlock.vData )
		return nullptr;

	// Добавление блока в контейнер.
	vBlocks.push_back( NewBlock );
	if( vBlocks.no_memory() )
		return nullptr;

	// Добавление смещения к началу блока в индекс.
	vIndex.push_back( vIndex.back() + aSize );
	if( vIndex.no_memory() )
		return nullptr;

	// Увеличить счётчик использованной памяти.
	vUsedMemory += aSize;

	if( vFirstBlockNum == (uint)-1 )
		vFirstBlockNum = Count() - 1;

	// Установить флаг добавления новых данных.
	vNewData = true;

	// Увеличить суммарный объём данных.
	vSize += aSize;

	return const_cast<char*>( NewBlock.vData );
}

unsigned int Blocks::Count() const
{
	return vIndex.size() - 1;
}

unsigned int Blocks::DataSize( unsigned int aBlockNum ) const
{    
	// Проверяем, загружен ли требуемый блок в память.
	if( vThresHold && ( aBlockNum < vFirstBlockNum || aBlockNum >= vFirstBlockNum + vBlocks.size() ) )
	{
		if( !Load( aBlockNum ) )
			return 0;
	}

	return vBlocks[aBlockNum - vFirstBlockNum].vSize;
}

unsigned int Blocks::Size() const
{
	return vSize;
}

const char * Blocks::Data( unsigned int aBlockNum ) const
{
	// Проверяем, загружен ли требуемый блок в память.
	if( vThresHold && ( aBlockNum < vFirstBlockNum || aBlockNum >= vFirstBlockNum + vBlocks.size() ) )
		if( !Load( aBlockNum ) )
			return nullptr;

	return vBlocks[aBlockNum - vFirstBlockNum].vData;
}

void Blocks::Clear()
{
	if( vThresHold )
	{
		if( vFd )
			fclose( vFd );

		if( (vFd = fopen64( vFileName, "w")) )
			fclose( vFd );
		vFd = nullptr;
	}

	vBlocks.clear();
	vMemoryAllocator.Reset();
	vIndex.clear();
	vIndex.push_back( 0 );
	vMode = WRITE_MODE;
	vUsedMemory = 0;
	vFirstBlockNum = -1;
	vNewData = false;
	vSize = 0;
}

bool Blocks::Dump() const
{
	if( !vThresHold )
		return true;

	if( vMode == READ_MODE )
	{
		// Если последней была опперация чтения, то переходим в режим записи.
		vMode = WRITE_MODE;

		// Закрываем открытый на чтение файл.
		if( vFd && fclose( vFd ) )
			return false;
		vFd = nullptr;
	}

	// Открываем файл на запись.
	if( !vFd && !( vFd = fopen64( vFileName, "a" ) ) )
		return false;

	// Зписываем в файл хранящиеся в памяти блоки данных.
	for( unsigned int block_n = 0; block_n < vBlocks.size(); ++block_n )
		if ( fwrite( vBlocks[block_n].vData, vBlocks[block_n].vSize, 1, vFd ) != 1 )
			return false;

	// Очищаем используемую под блоки данных память.
	vBlocks.clear();
	vMemoryAllocator.Reset();
	vUsedMemory = 0;
	vFirstBlockNum = -1;

	return true;
}

bool Blocks::Load( unsigned int aBlockNum ) const
{
	if( !vThresHold )
		return true;

	// Если в контейнере нет ни одного блока, то это ошибка.
	if( vFirstBlockNum == (uint)-1 )
		return false;

	if( vNewData )
	{
		// Если последней была операция записи, то сохраняем данные и переходим в режим чтения.
		if( !Dump() )
			return false;

		vMode = READ_MODE;
		// Закрываем отркрытый на запись файл.
		if( vFd && fclose( vFd ) )
			return false;
		vFd = nullptr;
	}

	// Очищаем выделенную под блоки данных память.
	vBlocks.clear();
	vMemoryAllocator.Reset();
	vUsedMemory = 0;
	vFirstBlockNum  = -1;

	// Открываем фал с даннымим на чтение.
	if( !vFd && !( vFd = fopen64( vFileName, "r" ) ) )
		return false;

	// Вычисляем количество блоков, которые можно разместить в памяти.
	unsigned int LastBlock = aBlockNum + 1;
	while ( LastBlock < Count() && vIndex[LastBlock] - vIndex[aBlockNum] < vThresHold )
		++LastBlock;

	if( vIndex[LastBlock] - vIndex[aBlockNum] > vThresHold && LastBlock - aBlockNum > 1)
		--LastBlock;

	// Позиционируемся в файле на нужном блоке данных.
	if( fseeko64( vFd, vIndex[aBlockNum], SEEK_SET) )
		return false;

	vUsedMemory = vIndex[LastBlock] - vIndex[aBlockNum];

	// Загружаем в память из файла нужный блок и несколько следующих за ним.
	char * Buffer = nAllocateObjects( vMemoryAllocator, char, vUsedMemory );
	if( !Buffer )
		return false;

	if( fread( Buffer, vUsedMemory, 1, vFd) != 1 )
		return false;

	// Размещаем блоки в памяти.
	for( unsigned int block_n = aBlockNum; block_n < LastBlock; ++block_n )
	{
		Block NewBlock( Buffer + ( vIndex[block_n] - vIndex[aBlockNum] ) , vIndex[block_n + 1] - vIndex[block_n] );
		vBlocks.push_back( NewBlock );
		if( vBlocks.no_memory() )
			return false;
	}
	vFirstBlockNum = aBlockNum;
	vNewData = false;

	return true;
}
