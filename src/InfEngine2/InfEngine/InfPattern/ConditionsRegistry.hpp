#ifndef __ConditionsRegistry_hpp__
#define __ConditionsRegistry_hpp__

#include "../InfPattern/Items/BaseCondition.hpp"
#include "../Vars/Vector.hpp"

/**
 *  Реестр условий языка DL.
 */
class InfConditionsRegistry
{
public:
	/** Конструктор условий. */
	InfConditionsRegistry() {}


public:
	/** Создание рееста в ReadWrite режиме. */
	void Create();

	/**
	 *  Сохранение реестра в fstorage.
	 * @param aFStorage - указатель на открытый fstorage.
	 */
	InfEngineErrors Save( fstorage * aFStorage ) const;

	/**
	 *  Открытие реестра из fstorage в ReadOnly режиме.
	 * @param aFStorage - указатель на открытый fstorage.
	 */
	InfEngineErrors Open( fstorage * aFStorage );

	/** Закрытие реестра. */
	void Close();


public:
	/**
	 *  Регистрация условия в индексе.
	 * @param aConditionType - тип условия.
	 * @param aVarId - идентификатор переменной.
	 * @param aVarValue - значение для сравнения.
	 * @param aConditionId - идентификатор условия.
	 */
	InfEngineErrors Registrate( InfBaseConditionType aConditionType, Vars::Id aVarId, const char * aVarValue, unsigned int & aConditionId );


public:
	bool Check( unsigned int aConditionId, const Vars::Vector & aVarsValues ) const;


private:
	/** Структура, описывающая условие. */
	typedef struct
	{
		/** Тип условия. */
		InfBaseConditionType vType;
		/** Идентификатор переменной. */
		Vars::Id vVarId;
		/** Идентификатор значения переменной. */
		unsigned int vValueId;
	} Condition;


private:
	/**
	 *  Сравнение условия с условием из индекса.
	 * @param aCondition - условие для сравнение.
	 * @param aPosition - позиция условия в индексе.
	 */
	int Compare( const Condition & aCondition, unsigned int aPosition ) const;

	/**
	 *  Внуренняя функция для поиска позиции в индексе, соответствующей указанному условию.
	 * @param aCondition - условие для поиска.
	 * @param aPosition - позиция условия в индексе, в случае, если условие найдено; позиция, на которую нужно
	 *                   поместить условие, если оно не было найдено.
	 */
	bool SearchInt( Condition aCondition, unsigned int & aPosition ) const;


private:
	/** Реестр значений. */
	NanoLib::NameIndex vValues;

	/** Поисковый индекс в ReadWrite режиме. */
	avector<unsigned int> vIndexRW;
	/** Поисковый индекс в ReadOnly режиме. */
	const unsigned int * vIndexRO = nullptr;

	/** Данные условий в ReadWrite режиме. */
	avector<Condition> vConditionsRW;
	/** Данные условий в ReadOnly режиме. */
	const Condition * vConditionsRO = nullptr;

	/** Количество условий в реестре. */
	unsigned int vConditionsNumber = 0;
	/** Размер данных условий. */
	unsigned int vConditionsSize = 0;
};

#endif  /** __ConditionsRegistry_hpp__ */
