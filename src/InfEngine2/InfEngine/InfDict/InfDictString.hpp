#ifndef INF_DICT_STRING_HPP
#define INF_DICT_STRING_HPP

#include "../InfPattern/Items.hpp"

/**
 *  Класс для создания представление строки словаря.
 */
class InfDictString
{
public:
	/** Конструктор. */
	InfDictString() : vIsMain( true ) {}

public:

	/** Получение размера памяти, необходимого для сохранения. **/
	unsigned int GetNeedMemorySize() const;

	/** Сохранение шаблон-словаря в буффер. **/
	InfEngineErrors Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aResultMemory ) const;


public:
	/** Возвращает значение флага, определяющего тип строки - главная или подчинённая. */
	bool IsMain() const { return vIsMain; }


public:
	/**
	 *  Установить элементы разобрнной строки словаря.
	 * @param aItems - массив элементов строки словаря.
	 * @param aItemsCount - количество элементов в массиве.
	 */
	void SetItems( InfPatternItems::Base ** aItems, unsigned int aItemsCount )
	{
		vItems = aItems;
		vItemsCount = aItemsCount;
	}

	/**
	 *  Уствновить уазатель на оригинальный текст словаря.
	 * @param aOriginal - указатель на оригинальный текст словаря.
	 * @param aOriginalLength - длина оригинального текста.
	 */
	void SetOriginal( const char * aOriginal, unsigned int aOriginalLength )
	{
		vOriginal = aOriginal;
		vOriginalLength = aOriginalLength;
	}

	/**
	 *  Определение типа строки - главная или подчинённая.
	 * @param aIsMain - истина, если строка главная, иначе - ложь.
	 */
	void SetMainFlag( bool aIsMain ) { vIsMain = aIsMain; }

	/**
	 *  Установка идентификатора, выданного хранилищем шаблонов.
	 * @param aId - идентификатор.
	 */
	void SetId( unsigned int aId ) { vId = aId; }

	/** Получение идентификатора, выдданого хранилищем шаблонов. */
	unsigned int GetId() const { return vId; }

	/** Возвращает количество элементов в строке словаря. */
	unsigned int GetItemsCount() const { return vItemsCount; }

	/**
	 *  Получение типа aNum'го элемента словаря.
	 * @param aNum - порядковый номер элемента в строке.
	 */
	InfPatternItems::ItemType GetItemType( unsigned int aNum ) const { return vItems[aNum]->GetType(); }

	/**
	 *  Возвращает заданный элемент строки словаря.
	 * @param aNum - порядковый номер элемента в строке.
	 */
	const InfPatternItems::Base * GetItem( unsigned int aNum ) const { return vItems[aNum]; }
	InfPatternItems::Base * GetItem( unsigned int aNum ) { return vItems[aNum]; }

	/** Возвращает список элементов. */
	InfPatternItems::Base ** GetItems() const { return vItems; }

public:
	/**
	 *  Замена в строке словаря синонимы на канонический вид.
	 * @param aSynonymsBase - база синонимов.
	 * @param aAllocator - аллокатор памяти.
	 */
    InfEngineErrors ApplySynonyms( SymbolymBaseRO & aSymbolymsBase, SynonymBaseRO & aSynonymsBase, nMemoryAllocator & aAllocator );

private:
	/** Идентификатор, присваиваемый при добавлении в хранилище шаблонов. **/
	unsigned int vId { 0 };

	/** Массив элементов разобранной строки словаря. **/
	InfPatternItems::Base ** vItems { nullptr };

	/** Количество элементов в разобранной строке словаря. **/
	unsigned int vItemsCount { 0 };

	/** Строка словаря в оригинальном виде. **/
	const char * vOriginal { nullptr };

	/** Длина строки словаря в оригинальном виде. **/
	unsigned int vOriginalLength { 0 };

	/** Флаг главной строки. */
	bool vIsMain { true };
};

#endif /** INF_DICT_STRING_HPP */
