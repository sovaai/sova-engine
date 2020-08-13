#ifndef __InfPatternItemsTagExtendAnswer_hpp__
#define __InfPatternItemsTagExtendAnswer_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	/**
	 *  Представление элемента продолжения ответа ExtendAnswer.
	 */
	class TagExtendAnswer: public Base
	{
	public:
		/** Конструктор класса. */
		TagExtendAnswer()
			{ vType = itExtendAnswer; }

		/**
		 *  Задать список идентификаторов шаблонов.
		 * @param aPatternId - список идентификаторов шаблонов.
		 * @param aId - список номеров шаблонов.
		 * @param aCount - длина списка.
		 */
		void SetPatternsId( const char ** aPatternsId, const uint32_t * aId, unsigned int aCount )
		{
			vPatternsId = aPatternsId;
			vId = aId;
			vCount = aCount;
		}

		/** Возвращает количество сохранённых в тэге идентификаторов шаблонов. */
		unsigned int GetCount() const
			{ return vCount; }

		/**
		 *  Возвращает идентификатор шаблона.
		 * @param aIdNum - номер идентификатора в списке.
		 */
		const char * GetPatternId( unsigned int aIdNum ) const
			{ return vPatternsId && aIdNum < vCount ? vPatternsId[aIdNum] : nullptr; }

	public:
		unsigned int GetNeedMemorySize() const;

		unsigned int Save( void * aBuffer ) const;

	private:
		/** Список идентификаторов шаблонов. */
		const char ** vPatternsId = nullptr;

		/** Список номеров шаблонов (номера шаблонов, назначаемые им при компиляции). */
		const uint32_t * vId = nullptr;

		/** Длина списка. */
		unsigned int vCount = 0;
	};

	/**
	 * Манипулятор элемента продолжения ответа ExtendAnswer.
	 */
	class TagExtendAnswerManipulator
	{
	public:
		/** Конструктор класса. */
		TagExtendAnswerManipulator()
			{ vBuffer = nullptr; }

		/**
		 * Инициализировать манипулятор.
		 * @param aBuffer - буфер.
		 */
		void SetBuffer( const void * aBuffer )
			{ vBuffer = reinterpret_cast<const char *>(aBuffer); }

		/** Возвращает количество сохранённых в тэге идентификаторов шаблонов. */
		unsigned int GetCount() const
			{ return vBuffer ? *reinterpret_cast<const uint32_t*>(vBuffer) : 0; }

		/**
		 *  Возвращает идентификатор шаблона.
		 * @param aIdNum - номер идентификатора в списке.
		 */
		const char * GetPatternId( unsigned int aIdNum ) const
			{ return vBuffer ? vBuffer + reinterpret_cast<const uint32_t*>(vBuffer)[1 + GetCount() + aIdNum] : nullptr; }

		/**
		 *  Возвращает номер шаблона.
		 * @param aIdNum - номер в списке.
		 */
		const unsigned int GetId( unsigned int aIdNum ) const
			{ return vBuffer ? reinterpret_cast<const uint32_t*>(vBuffer)[1+aIdNum] : 0; }

	private:
		const char * vBuffer = nullptr;
	};
}

#endif /** __InfPatternItemsTagExtendAnswer_hpp__ */
