#ifndef __NanoRandomBox_hpp__
#define __NanoRandomBox_hpp__

#include <lib/aptl/avector.h>

class NanoRandomBox
{
public:
	enum ReturnCode { RT_OK, RT_NO_MEMORY, RT_INVALID_DATA, RT_NOT_ENOUGH_MEMORY, RT_WRONG_CHECKSUM };

	/**
	 *  Конструктор класса.
	 * @param aCount - размер диапазона, из которого генератор выдаёт числа..
	 */
	NanoRandomBox( unsigned int aCount );

	/**
	 *  Подготавливает генератор к выдаче чисел из диапазона от 0 до aCount-1 в случайном порядке.
	 * @param aCount - размер диапазона, из которого генератор выдаёт числа.
	 */
	bool Init( unsigned int aCount );

	/** Подготавливает очередь со случайными числами. */
	void Refresh();

	/** Подготавливает генератор к выдаче чисел заново. */
	void Reset();

	/** Возвращает размер диапазона, из которого генератор выдаёт числа. */
	unsigned int GetCount() const;

	/** Возвращает истину, если в генераторе не осталось чисел, иначе - ложь. */
	bool IsEmpty() const;

	/** Возвращает количество оставшихся в генераторе чисел. */
	unsigned int GetRemainingAmount() const;

	/**
	 *  Возвращает истину, если число aNum ещё не было выдано генератором.
	 * @param aNum - число из диапазона от 0 до GetCount()-1.
	 */
	bool IsInBox( unsigned int aNum ) const;

	/**
	 *  Удаляет число aNum из диапазона. Оно никогда не будет выдано.
	 * @param aNum - число из диапазона от 0 до GetCount()-1.
	 */
	bool TakeFromBox( unsigned int aNum );

	/**
	 *  Помещает число aNum на место последнего взятого.
	 * @param aNum - призвольное число.
	 */
	void RetriveToBox( unsigned int aNum );

	/** Возвращает случайное не возвращавшееся ранее число из диапазона от 0 до GetCount()-1. */
	unsigned int TakeFromBox();

public:
	/** Возвращает размер непрерывного блока памяти в байтах, необходимого для сохранения объекта. */
	unsigned int GetNeedMemorySize() const;

	/**
	 *  Сохраняет объект в памяти.
	 * @param aBuffer - блок памяти для сохранения объекта.
	 */
	unsigned int Save( void * aBuffer ) const;

	/**
	 *  Загружает объект из памяти.
	 * @param aBuffer - блок памяти, содержащий созранённый ранее объект.
	 * @param aBufferSize  - размер блока памяти.
	 */
    ReturnCode Load( const void * aBuffer, unsigned int aBufferSize );

protected:
	avector<unsigned char> vMask;

	unsigned int vMaskSize;

	avector<unsigned int> vQueue;

	unsigned int vLeftInBox;
};

typedef NanoRandomBox RandomBox;

#endif /** __NanoRandomBox_hpp__ */
