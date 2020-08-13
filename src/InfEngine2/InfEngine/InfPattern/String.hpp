#ifndef __InfPattern_String_hpp__
#define __InfPattern_String_hpp__

#include "Items/Array.hpp"
#include "ConditionsRegistry.hpp"

namespace InfPatternItems
{
	/**
	 *  Базовая шаблон-строка, на основе которой стоятся все шаблон-строки.
	 */
	class StringRW: public Array
	{
	public:
		/**
		 *  Конструктор базовой шаблон строки на основе массива DL элементов.
		 * @param aItems - массив DL элементов.
		 * @param aItemsNum - количество элементов в массиве.
		 */
		StringRW( Base ** aItems = nullptr, unsigned int aItemsNum = 0 ): Array( aItems, aItemsNum ) {}

	public:
		/** Получение объема памяти, необходимого для сохранения атрибутов шаблон-строки. */
		virtual unsigned int GetNeedMemorySizeForAttrs() const = 0;

		/**
		 *  Сохранение атрибутов шаблон-строки в буфер.
		 * @param aBuffer - буфер для сохранения шаблон-строки.
		 * @return размер памяти, занятый шаблон-строкой в буфере.
		 */
		virtual unsigned int SaveAttrs( void * aBuffer ) const = 0;
	};

	/**
	 *  Базовый класс доступа к сохраненной шаблон-строке.
	 */
	class StringRO: public ArrayManipulator
	{
	public:
		/** Конструктор по умолчанию. */
		StringRO() {}

		/**
		 *  Конструктор шаблон-строки на основе массива DL элементов.
		 * @param aItems - массив DL элементов.
		 */
		StringRO( const ArrayManipulator aItems ):
			ArrayManipulator( aItems ) {}
	};



	/**
	 *  Шаблон-условие.
	 */
	class ConditionRW
	{
	public:
		ConditionRW( BaseCondition * aCondition, unsigned int aWeight )
			{ vCondition = aCondition; vWeight = aWeight; }

	public:
		/** Получение размера памяти, необходимого для сохранения шаблон-условия. */
		unsigned int GetNeedMemorySize() const
			{ return vCondition->GetNeedMemorySize(); }

		/**
		 *  Сохранение шаблон-условия в буфер.
		 * @param aBuffer - буфер для сохранения шаблон-условия.
		 * @return размер памяти, занимаемый шаблон-условием в буфере.
		 */
		unsigned int Save( char * aBuffer ) const
			{ return vCondition->Save( aBuffer ); }

	public:
		/** Получение веса шаблон-условия. */
		unsigned int GetWeight() const
			{ return vWeight; }

	public:
		/** Идентификатор условия. */
		BaseCondition * vCondition = nullptr;

		/** Вес шаблон-условия. */
		unsigned int vWeight = 0;
	};

	/**
	 *  Класс доступа к сохраненному шаблон-условию.
	 */
	class ConditionRO: public BaseConditionManipulator
	{
	public:
		/**
		 *  Конструктор.
		 * @param aBuffer - буфер с сохраненным шаблон-условием.
		 */
		ConditionRO( const void * aBuffer = nullptr ):
			BaseConditionManipulator( aBuffer ) {}

	public:
		/**
		 *  Проверка условия.
		 * @param aConditionsRegistry - реестр условий.
		 * @param aVarsValues - вектор значений переменных.
		 */
		bool Check( const InfConditionsRegistry & aConditionsRegistry, const Vars::Vector & aVarsValues ) const
			{ return aConditionsRegistry.Check( BaseConditionManipulator::GetConditionId(), aVarsValues ); }
	};



	/** Тип данных для работы с ссылками в шаблон-вопросах. */
	typedef std::pair<unsigned int,unsigned int> Segment;

	/**
	 *  Шаблон-вопрос.
	 */
	class QuestionWR: public StringRW
	{
	public:
		/**
		 *  Конструктор шаблон-вопроса.
		 * @param aItems - элементы шаблон-вопроса.
		 * @param aItemsNumber - число элементов шаблон-вопроса.
		 * @param aWeight - вес шаблон-вопроса.
		 */
		QuestionWR( Base ** aItems, unsigned int aItemsNumber, unsigned int aWeight = 0 ):
			StringRW( aItems, aItemsNumber )
		{
			vWeight = aWeight;
			memset( vRefereces, 0, 2*vMaxReferencesNumber*sizeof( unsigned int ) );
		}

	public:
		/**
		 *  Установка референций.
		 * @param aReferences - референции.
		 * @param aReferencesNumber - число референций.
		 */
		void SetReferences( Segment * aReferences, unsigned int aReferencesNumber )
		{
			memset( vRefereces, 0, 2*vMaxReferencesNumber*sizeof( unsigned int ) );
			for( unsigned int i = 0; i < vMaxReferencesNumber && i < aReferencesNumber; i++ )
			{
				vRefereces[2*i] = aReferences[i].first;
				vRefereces[2*i+1] = aReferences[i].second;
			}
		}

		/** Установка веса терминов в шаблон-вопросе. */
		void SetTermsWeight( unsigned int aTermsWeight )
			{ vTermsWeight = aTermsWeight; }

	public:
		/**
		 *  Получение типа i-того элемента шаблон-вопроса.
		 * @param aNum - номер элемента.
		 */
		ItemType GetItemType( unsigned int aNum ) const
			{ return vItems[aNum]->GetType(); }

	public:
		/** Вычисление объема памяти, необходимого для сохранения аттрибутов шаблон-вопроса. */
		unsigned int GetNeedMemorySizeForAttrs() const
			{ return 2*sizeof( uint32_t ) + 20; }

		/**
		 *  Сохранение аттрибутов шаблон-вопроса.
		 * @param aBuffer - буфер для сохранения.
		 * @return - размер захраненного шаблон-вопроса.
		 */
		unsigned int SaveAttrs( void * aBuffer ) const
		{
			void * ptr = aBuffer;
			binary_data_save<uint32_t>( ptr, vWeight );
			binary_data_save<uint32_t>( ptr, vTermsWeight );
			for( unsigned int i = 0; i < 20; i++ )
				binary_data_save<uint8_t>( ptr, vRefereces[i] );
			return static_cast<char*>( ptr ) - static_cast<char*>( aBuffer );
		}

	protected:
		/** Максимальное допустимое число ссылок в шаблон-вопросе. */
		static const unsigned int vMaxReferencesNumber = 10;

	protected:
		/** Частичный вес шаблона, определяемый на этапе компиляции. */
		unsigned int vWeight = 0;

		/** Вес только терминов, включая inline-словари. */
		unsigned int vTermsWeight = 0;

		/** Массив индексов ссылок. */
		unsigned int vRefereces[20];
	};

	/**
	 *  Класс доступа к сохраненному шаблон-вопросу.
	 */
	class QuestionRO: public StringRO
	{
	public:
		/** Конструктор манипулятора сохраненного шаблон-вопроса. */
		QuestionRO() {}

		/**
		 *  Конструктор доступа к сохраненному шаблон-вопросу.
		 * @param aQuestionId - идентификатор шаблон-вопроса.
		 * @param aItems - массив DL элементов.
		 * @param aAttrs - буфер с сохраненными атрибутами шаблон-вопроса.
		 */
		QuestionRO( unsigned int aQuestionId, const ArrayManipulator & aItems, const void * aAttrs ):
			StringRO( aItems )
		{
			vQuestionId = aQuestionId;
			vAttrs = aAttrs;
		}

	public:
		/**
		 *  Установка данных для доступа к шаблон-вопросу.
		 * @param aQuestionId - идентификатор шаблон-вопроса.
		 * @param aItems - массив DL элементов.
		 * @param aAttrs - буфер с сохраненными атрибутами шаблон-вопроса.
		 */
		void Set( unsigned int aQuestionId, const ArrayManipulator & aItems, const void * aAttrs )
		{
			vQuestionId = aQuestionId;
			vAttrs = aAttrs;
			StringRO::Set( aItems );
		}

	public:
		/** Получение частичного веса шаблон-вопроса. */
		unsigned int GetWeight() const
			{ return ( vAttrs ? *static_cast<const uint32_t*>( vAttrs ) : 0 ); }

		/** Получение веса только терминов, включая inline-словари. */
		unsigned int GetTermsWeight() const
			{ return ( vAttrs ? static_cast<const uint32_t*>( vAttrs )[1] : 0 ); }


		/** Получение идентификатора шаблон-вопроса. */
		unsigned int GetId() const
			{ return vQuestionId; }

	public:
		/**
		 *  Получение i-ой референции.
		 * @param aNum - номер требуемой референции.
		 * @param aRefBegin - позиция перед референции.
		 * @param aRefEnd - последняя позиция референции.
		 */
		void GetReferece( unsigned int aNum, unsigned int & aRefBegin, unsigned int & aRefEnd ) const
		{
			// Если запрашивается некорректный референс, то возвращаем нули.
			if( aNum >= 10 || !vAttrs )
				aRefBegin = aRefEnd = 0;
			else
			{
				auto ptr = binary_data_get<uint8_t>( vAttrs, 2*sizeof( uint32_t ) + sizeof( uint8_t )*2*aNum );
				aRefBegin = *ptr;
				aRefEnd = *(ptr+1);
			}
		}

	private:
		/** Идентификатор шаблон-вопроса. */
		unsigned int vQuestionId = -1;

		/** Буфер с атрибутами шаблон-вопроса. */
		const void * vAttrs = nullptr;
	};



	/**
	 *  That-шаблон.
	 */
	typedef QuestionWR ThatWR;

	/**
	 *  Класс доступа к сохраненным that-шаблоном.
	 */
	typedef QuestionRO ThatRO;



	/**
	 *  Шаблон-ответ.
	 */
	class AnswerRW: public StringRW
	{
	public:
		/**
		 *  Конструктор.
		 * @param aItems - список DL элементов.
		 * @param aItemsNum - число DL элементов.
		 */
		AnswerRW( Base ** aItems = nullptr, unsigned int aItemsNum = 0 ):
			StringRW( aItems, aItemsNum ) {}

		/**
		 *  Конструктор.
		 * @param aAnswer - шаблон-ответ.
		 */
		AnswerRW( const AnswerRW & aAnswer ): AnswerRW( aAnswer.vItems, aAnswer.GetItemsNumber() )
		{
			vModifiers = aAnswer.vModifiers;
			vTLCS = aAnswer.vTLCS;
		}

	public:
		/** Модификаторы шаблон-ответа */
		enum class Modifiers
		{
			Last		= 1,	// Шаблон-ответ с пониженным приоритетом.
			Uniq		= 2,	// Шаблон-ответ срабатывающий только один раз.
			LightUniq	= 4,	// Шаблон-ответ срабатывающий повторно только после срабатывания остальных mLUniq шаблон-ответов.
			Weight		= 8,	// Модификатор частичного веса шаблон-ответа.
			First		= 16	// Модификатор для событий, говорящий, что шаблон должен срабатывать только один раз.
		};

	public:
		/**
		 *  Установка модификатора шаблон-ответа.
		 * @param aModifier - модификатор шаблон-ответа.
		 */
		void SetModifier( Modifiers aModifier )
			{ vModifiers |= (unsigned int)aModifier; }

		/**
		 *  Установка частичного веса шаблон-ответа.
		 * @param aValue
		 */
		void SetWeightModifier( int aWeight )
		{
			SetModifier( Modifiers::Weight );
			if( aWeight > 511 )
				aWeight = 511;
			if( aWeight < -512 )
				aWeight = -512;

			vModifiers |= static_cast<char>(aWeight) << static_cast<int>(Modifiers::Weight);
		}

		/**
		 *  Удаление модификатора шаблон-ответа.
		 * @param aModifier - модификатор шаблон-ответа.
		 */
		void UnsetModifier( Modifiers aModifier )
			{ vModifiers &= !(unsigned int)aModifier; }

		/** Проверка наличия модификаторов. */
		bool IsModified() const
			{ return vModifiers > 0; }

		/** Проверка значений модификатора. */
		bool IsLUniq() const
			{ return vModifiers & static_cast<unsigned int>(Modifiers::LightUniq); }

		bool IsLast() const
			{ return vModifiers & static_cast<unsigned int>(Modifiers::Last); }

		/** Получение частичного веса шаблон-ответа. */
		int GetWeightModifier() const
			{ return vModifiers & static_cast<unsigned int>(Modifiers::Weight) ? static_cast<char>( vModifiers >> static_cast<unsigned int>(Modifiers::Weight) & 0xff ) : 0; }


	public:
		/**
		 *  Установка номера условного оператора уровня шаблона.
		 * @param aTLCS - номер условного оператора уровня шаблона, в зоне действия которого находится шаблон-вопрос.
		 */
		void SetTLCS( unsigned int aTLCS )
			{ vTLCS = aTLCS; }

	public:
		/** Вычисление объема памяти, необходимого для сохранения. */
		unsigned int GetNeedMemorySizeForAttrs() const
			{ return 2*sizeof( uint32_t ); }

		/**
		 *  Сохранение.
		 * @param aBuffer - буффер для сохранения.
		 * @param aBufferSize - размер буффера.
		 * @param aUsedMemory - реальный размер сохраненного объекта.
		 */
		unsigned int SaveAttrs( void * aBuffer ) const
			{ return  binary_data_save_rs<uint32_t>( aBuffer, vModifiers ) + binary_data_save_rs<uint32_t>( aBuffer, vTLCS ); }

	protected:
		/** Маска модификаторов. */
		unsigned int vModifiers = 0;

		/** Номер условного оператора уровня шаблона, в зоне действия которого находится шаблон-ответ. */
		unsigned int vTLCS = static_cast<unsigned int>(-1);
	};

	/**
	 *  Класс доступа к сохраненному шаблон-ответу.
	 */
	class AnswerRO: public StringRO
	{
	public:
		/** Конструктор пустого шаблон-ответа. */
		AnswerRO() {}

		/**
		 *  Конструктор.
		 * @param aItems - строка элементов шаблон-ответа.
		 * @param aAttrs - сохраненные атрибуты шаблон-ответа.
		 */
		AnswerRO( const ArrayManipulator & aItems, const void * aAttrs = nullptr ):
			StringRO( aItems )
		{
			vModifiers = static_cast<const uint32_t*>( aAttrs )[0];
			vTLCS = static_cast<const uint32_t*>( aAttrs )[1];
		}

	public:
		using Modifiers = AnswerRW::Modifiers;

	public:
		/**
		 *  Проверка модификатора.
		 * @param aModifier - модификатор.
		 */
		bool Check( Modifiers aModifier ) const
			{ return vModifiers & (int)aModifier; }

		/** Получение номера условного оператору уровня шаблона, в зоне действия которого находится шаблон-ответ. */
		unsigned int GetTLCS() const
			{ return vTLCS; }

		/** Проверка наличия условного оператору уровня шаблона. */
		bool HasTLCS() const
			{ return GetTLCS() != static_cast<unsigned int>(-1); }

		/** Проверка модификаторов. */
		bool IsLast() const
			{ return Check( Modifiers::Last ); }
		bool IsUniq() const
			{ return Check( Modifiers::Uniq ); }
		bool IsLUniq() const
			{ return Check( Modifiers::LightUniq ); }
		bool IsFirst() const
			{ return Check( Modifiers::First ); }

		/** Получение частичного веса шаблон-ответа. */
		int GetWeightModifier() const
			{ return Check( Modifiers::Weight ) ? static_cast<char>( (vModifiers >> static_cast<int>(Modifiers::Weight)) & 0xff ) : 0; }

	private:
		/** Маска модификаторов шаблон-ответа. */
		unsigned int vModifiers = 0;

		/** Номер условного оператора уровня шаблона. */
		unsigned int vTLCS = static_cast<unsigned int>(-1);
	};
}

#endif /** __InfPattern_String_hpp__ */
