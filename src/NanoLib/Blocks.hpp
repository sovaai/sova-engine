#ifndef __Blocks_hpp__
#define __Blocks_hpp__

#include <lib/aptl/avector.h>
#include "nMemoryAllocator.hpp"
#include <cstdio>
#include <NanoLib/LogSystem.hpp>

#include <unistd.h>


/**
 *  Контейнер для хранения блоков данных с возможность их выгрузки в файл.
 */
class Blocks
{
public:
	/**
	 *  Конструктор класса.
	 * @param aFileName - имя файла для хранения данных.
	 * @param aThreshold - максимальный размер памяти в байтах, которую контейнер может выделить для хранения блоков.
	 */
	Blocks( const char * aFileName, unsigned int aThreshold ):
		vFileName( aFileName ), vThresHold( aThreshold ) { Clear(); }

	/** Деструктор. */
	~Blocks()
	{
		Clear();

		// Удаление временного файла.
		if( vFileName && vThresHold )
			unlink( vFileName );
	}

	/**
	 *  Добавление блока данных в контейнер.
	 * @param aData - указатель на данные.
	 * @param aDataSize - размер данных в байтах.
	 */
	bool Add( const char * aData, unsigned int aDataSize );

	/**
	 *  Добавить блок данных в контейнер, ссылка на блок действительна до следующего вызова какого-либо метода этого класса.
	 * @param aData - указатель на данные.
	 * @param aDataSize - размер данных в байтах.
	 */
	char * Add( unsigned int aDataSize );

	/** Возвращает количество сохранённых в контейнере блоков. */
	unsigned int Count() const;

	/** Возвращает суммарный объём данных. */
	unsigned int Size() const;

	/**
	 *  Возвращает n-ый блок данных.
	 * @param aBlockNum - номер блока.
	 */
	const char * Data( unsigned int aBlockNum ) const;

	/**
	 *  Возвращает размер n-ого блока данных.
	 * @param aBlockNum - номер блока.
	 */
	unsigned int DataSize( unsigned int aBlockNum ) const;

	/** Очищает контейнер. */
	void Clear();

private:
	/** Сбросить все данные в файл если лимит использования памяти превышен. */
	bool Dump() const;

	/**
	 *  Загрузить из файла в память заданный блок, если он ещё не загружен.
	 * @param aBlockNumber - номер блока данных.
	 */
	bool Load( unsigned int aBlockNum ) const;

private:
	/** Представление блока данных. */
	struct Block;

	/** Менеджер памяти. */
	mutable nMemoryAllocator vMemoryAllocator;

	/** Имя файла для сохранения данных. */
	const char * const vFileName = nullptr;

	/** Максимальный размер в байтах выделяемой для размещения данных памяти. */
	const unsigned int vThresHold = 0;

	/** Блоки данных, находящиеся в памяти. */
	mutable avector<Block> vBlocks;

	/** Список сдвигов начал блоков данных. */
	avector<unsigned int> vIndex;

	/** Текущий режим работы контейнера. */
	mutable enum Mode { READ_MODE, WRITE_MODE } vMode;

	/** Количество памяти, выделенной под данные в текущий момент. */
	mutable unsigned int vUsedMemory;

	/** Номер первого находящегося в памяти блока. */
	mutable unsigned int vFirstBlockNum;

	/** Файл для хранения данных. */
	mutable FILE * vFd = nullptr;

	/** Флаг добавления данных с момента последнего чтения. */
	mutable bool vNewData;

	/** Суммарный объём данных. */
	mutable unsigned int vSize;
};

/**
 *  Представление блока данных.
 */
struct Blocks::Block
{
	Block( const char * aData = nullptr, unsigned int aSize = 0 )
	{
		vData = aData;
		vSize = aSize;
	}

	const char * vData = nullptr;
	unsigned int vSize = 0;
};

#endif /** __Blocks_hpp__ */
