#ifndef __InfPatternItemSwitch_hpp__
#define __InfPatternItemSwitch_hpp__

#include "BaseCondition.hpp"
#include "Array.hpp"
#include "TagFunction.hpp"

namespace InfPatternItems
{
	/**
     *  Представление тэга оператора выбора.
	 */
    class TagSwitch: public Base
	{
	public:
        TagSwitch()
            { vType = itSwitch; }

	public:
		/** Установка данных. */
        void Set( const TagFunction::Argument * aSwitchArg,
                  const TagFunction::Argument ** aCaseArg,
                  const TagFunction::Argument ** aCaseBody,
                  unsigned int aCaseNumber,
                  const TagFunction::Argument * aDefaultBody )
		{
            vSwitchArg = aSwitchArg;
            vCaseArg = aCaseArg;
            vCaseBody = aCaseBody;
            vCaseNumber = aCaseNumber;
            vDefaultBody = aDefaultBody;
		}

	public:
		unsigned int GetNeedMemorySize() const;

		unsigned int Save( void * aBuffer ) const;

	public:
		unsigned int GetCasesNumber() const { return vCaseNumber; }

		const TagFunction::Argument * GetCaseBody( unsigned int i ) const { return vCaseBody[i]; }

		const TagFunction::Argument * GetDefault() const { return vDefaultBody; }

	private:
        /** Аргумент ключевого слова switch. */
        const TagFunction::Argument * vSwitchArg = nullptr;

        /** Аргументы ключевых слов case. */
        const TagFunction::Argument ** vCaseArg = nullptr;
        /** Тела блоков case. */
        const TagFunction::Argument ** vCaseBody = nullptr;
        /** Количество блоков case. */
        unsigned int vCaseNumber = 0;

        /** Тело блока default. */
        const TagFunction::Argument * vDefaultBody = nullptr;
	};

	/**
     *  Манипулятор тэга оператора выбора.
	 */
    class TagSwitchManipulator
	{
	public:
        TagSwitchManipulator( const void * aBuffer = nullptr ):
			vBuffer( static_cast<const char*>( aBuffer ) ) {}

	public:
		/** Инициализация. */
		void Init( const void * aBuffer )
			{ vBuffer = static_cast<const char*>( aBuffer ); }

	public:
        /** Получение количества блоков case. */
        inline unsigned int GetCaseNumber()
            { return vBuffer ? reinterpret_cast<const uint32_t*>( vBuffer )[1]: 0;}

        /** Получение аргумента switch. */
        const char * GetSwitchArg() const
            { return vBuffer ? vBuffer + reinterpret_cast<const uint32_t*>(vBuffer)[2] : nullptr; }

        /** Получение тела блока default. */
        const char * GetDefaultBody() const
            { return vBuffer ? vBuffer + reinterpret_cast<const uint32_t*>(vBuffer)[3] : nullptr; }

        /** Получение аргумента case. */
        const char * GetCaseArg( unsigned int aCaseN ) const
            { return vBuffer ? vBuffer + reinterpret_cast<const uint32_t*>(vBuffer)[4 + 2*aCaseN] : nullptr; }

        /** Получение тела блока case. */
        const char * GetCaseBody( unsigned int aCaseN ) const
            { return vBuffer ? vBuffer + reinterpret_cast<const uint32_t*>(vBuffer)[5 + 2*aCaseN] : nullptr; }

	private:
		/** Данные элемента. */
		const char * vBuffer = nullptr;
	};
}

#endif /** __InfPatternItemSwitch_hpp__ */
