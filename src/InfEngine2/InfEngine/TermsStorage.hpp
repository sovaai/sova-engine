#ifndef TERMS_STORAGE_HPP
#define TERMS_STORAGE_HPP

#include <NanoLib/NameIndex.hpp>
#include "NDocImage.hpp"

namespace TermsStorage
{
	/** Количество памяти в байтах, занимаемой одним набором атрибутов. */
	static const unsigned int ATTRS_SIZE = 22;

	/** Номер байта, с которого начинаются флаги. */
	static const unsigned int FLAGS_BASE = 20;


	/** Атрибуты термина. */
	struct TermAttrs
	{
	public:
		TermAttrs() { Reset(); }

		TermAttrs( const TermsStorage::TermAttrs & aTermAttrs ) { this->operator=( aTermAttrs ); }

	public:
		TermAttrs& operator=( const TermsStorage::TermAttrs & aTermAttrs )
		{
			memcpy( vBuffer, aTermAttrs.vBuffer, ATTRS_SIZE ); return *this;
		}

	public:
		/** Очистить все атрибуты. */
		void Reset() { memset( vBuffer, 0, ATTRS_SIZE ); }

		/** Идентификатор шаблона. */
		void SetId( unsigned int aId ) { *reinterpret_cast<uint32_t*>( vBuffer ) = aId; }
		/** Идентификатор шаблона-вопроса в шаблоне. */
		void SetSubId( unsigned int aSubId ) { *reinterpret_cast<uint16_t*>( vBuffer + 4 ) = aSubId; }
		/** Порядковый номер термина в шаблон-вопросе. */
		void SetPos( unsigned int aPos ) { *reinterpret_cast<uint16_t*>( vBuffer + 6 ) = aPos; }
		/** PersonId, использованный в условии шаблона. */
		void SetInfPersonId( unsigned int aPersonId ) { *reinterpret_cast<uint32_t*>( vBuffer + 8 ) = aPersonId; }
		/** Сдвиг суперзвездочек перед термином. */
		void SetStarShiftBefore( unsigned int aShift ) { vBuffer[12] = aShift; }
		/** Сдвиг суперзвездочек после термина. */
		void SetStarShiftAfter( unsigned int aShift ) { vBuffer[13] = aShift; }
		/** Позиция следующего термина. */
		void SetNextPos( unsigned int aPos ) { *reinterpret_cast<uint16_t*>( vBuffer + 14 ) = aPos; }
		/** Порядковый номер термина в шаблон-вопросе, считая только термины и inline-словари без пустых вариантов. */
		void SetRealTermPos( unsigned int aPos ) { *reinterpret_cast<uint16_t*>( vBuffer + 16 ) = aPos; }
		/** Минимальное количество слов после данного элемента. */
		void SetRealTermsAfter( unsigned int aMinAfter ) { *reinterpret_cast<uint16_t*>( vBuffer + 18 ) = aMinAfter; }


		/** Флаг последнего термина в поисковом шаблоне. */
		void SetFlagLastTerm() { vBuffer[FLAGS_BASE] |= 0x01; }
		/** Флаг первого не виртуального термина. */
		void SetFlagFirstTerm() { vBuffer[FLAGS_BASE] |= 0x02; }
		/** Флаг звездочки перед термином. */
		void SetFlagStarBefore() { vBuffer[FLAGS_BASE] |= 0x04; }
		/** Флаг суперзвездочки перед термином. */
		void SetFlagSuperStarBefore() { vBuffer[FLAGS_BASE] |= 0x08; }
		/** Флаг звездочки после термина. */
		void SetFlagStarAfter() { vBuffer[FLAGS_BASE] |= 0x10; }
		/** Флаг суперзвездочки после термина. */
		void SetFlagSuperStarAfter() { vBuffer[FLAGS_BASE] |= 0x20; }
		/** Флаг текстового термина. */
		void SetFlagRealTerm() { vBuffer[FLAGS_BASE] |= 0x40; }
		/** Флаг процессирования после термина. */
		void SetFlagProcessAfter() { vBuffer[FLAGS_BASE] |= 0x80; }
		/** Флаг процессирования перед термином. */
		void SetFlagProcessBefore() { vBuffer[FLAGS_BASE + 1] |= 0x01; }
		/** Флаг словаря после термина. */
		void SetFlagDictAfter() { vBuffer[FLAGS_BASE + 1] |= 0x02; }
		/** Флаг словаря перед термином. */
		void SetFlagDictBefore() { vBuffer[FLAGS_BASE + 1] |= 0x04; }
		/** Флаг пустого словаря перед термином. */
		void SetFlagEmptyDictBefore() { vBuffer[FLAGS_BASE + 1] |= 0x08; }
		/** Флаг пустого словаря после термина. */
		void SetFlagEmptyDictAfter() { vBuffer[FLAGS_BASE + 1] |= 0x10; }


		/** Идентификатор шаблона. */
		unsigned int GetId() { return *reinterpret_cast<uint32_t*>( vBuffer ); }
		/** Идентификатор шаблона-вопроса в шаблоне. */
		unsigned int GetSubId() { return *reinterpret_cast<uint16_t*>( vBuffer + 4 ); }
		/** Порядковый номер термина в шаблон-вопросе. */
		unsigned int GetPos() { return *reinterpret_cast<uint16_t*>( vBuffer + 6 ); }
		/** PersonId, использованный в условии шаблона. */
		unsigned int GetInfPersonId() { return *reinterpret_cast<uint32_t*>( vBuffer + 8 ); }
		/** Сдвиг суперзвездочек перед термином. */
		unsigned int GetStarShiftBefore() { return vBuffer[12]; }
		/** Сдвиг суперзвездочек после термина. */
		unsigned int GetStarShiftAfter() { return vBuffer[13]; }
		/** Позиция следующего термина. */
		unsigned int GetNextPos() { return *reinterpret_cast<uint16_t*>( vBuffer + 14 ); }
		/** Порядковый номер термина в шаблон-вопросе, считая только термины и inline-словари без пустых вариантов. */
		unsigned int GetRealTermPos() { return *reinterpret_cast<uint16_t*>( vBuffer + 16 ); }
		/** Количество обязательных терминов после данного элемента. */
		unsigned int GetRealTermsAfter() { return *reinterpret_cast<uint16_t*>( vBuffer + 18 ); }


		/** Флаг последнего термина в поисковом шаблоне. */
		bool GetFlagLastTerm() { return vBuffer[FLAGS_BASE] & 0x01; }
		/** Флаг первого не виртуального термина. */
		bool GetFlagFirstTerm() { return vBuffer[FLAGS_BASE] & 0x02; }
		/** Флаг звездочки перед термином. */
		bool GetFlagStarBefore() { return vBuffer[FLAGS_BASE] & 0x04; }
		/** Флаг суперзвездочки перед термином. */
		bool GetFlagSuperStarBefore() { return vBuffer[FLAGS_BASE] & 0x08; }
		/** Флаг звездочки после термина. */
		bool GetFlagStarAfter() { return vBuffer[FLAGS_BASE] & 0x10; }
		/** Флаг суперзвездочки после термина. */
		bool GetFlagSuperStarAfter() { return vBuffer[FLAGS_BASE] & 0x20; }
		/** Флаг текстового термина. */
		bool GetFlagRealTerm() { return vBuffer[FLAGS_BASE] & 0x40; }
		/** Флаг процессирования после термина. */
		bool GetFlagProcessAfter() { return vBuffer[FLAGS_BASE] & 0x80; }
		/** Флаг процессирования перед термином. */
		bool GetFlagProcessBefore() { return vBuffer[FLAGS_BASE + 1] & 0x01; }
		/** Флаг словаря после термина. */
		bool GetFlagDictAfter() { return vBuffer[FLAGS_BASE + 1] & 0x02; }
		/** Флаг словаря перед термином. */
		bool GetFlagDictBefore() { return vBuffer[FLAGS_BASE + 1] & 0x04; }
		/** Флаг пустого словаря перед термином. */
		bool GetFlagEmptyDictBefore() { return vBuffer[FLAGS_BASE + 1] & 0x08; }
		/** Флаг пустого словаря после термина. */
		bool GetFlagEmptyDictAfter() { return vBuffer[FLAGS_BASE + 1] & 0x10; }

		char vBuffer[ATTRS_SIZE];
	};


	/** Атрибуты термина. */
	struct TermAttrsRO
	{
	public:
		TermAttrsRO( const char * aBuffer = nullptr ) { vBuffer = aBuffer; }

		void Init( const char * aBuffer ) { vBuffer = aBuffer; }

		/** Идентификатор шаблона. */
		unsigned int GetId() const { return *reinterpret_cast<const uint32_t*>( vBuffer ); }
		/** Идентификатор шаблона-вопроса в шаблоне. */
		unsigned int GetSubId() const { return *reinterpret_cast<const uint16_t*>( vBuffer + 4 ); }
		/** Порядковый номер термина в шаблон-вопросе. */
		unsigned int GetPos() const { return *reinterpret_cast<const uint16_t*>( vBuffer + 6 ); }
		/** PersonId, использованный в условии шаблона. */
		unsigned int GetInfPersonId() const { return *reinterpret_cast<const uint32_t*>( vBuffer + 8 ); }
		/** Сдвиг суперзвездочек перед термином. */
		unsigned int GetStarShiftBefore() const { return vBuffer[12]; }
		/** Сдвиг суперзвездочек после термина. */
		unsigned int GetStarShiftAfter() const { return vBuffer[13]; }
		/** Позиция следующего термина. */
		unsigned int GetNextPos() const { return *reinterpret_cast<const uint16_t*>( vBuffer + 14 ); }
		/** Порядковый номер термина в шаблон-вопросе, считая только термины и inline-словари без пустых вариантов. */
		unsigned int GetRealTermPos() const { return *reinterpret_cast<const uint16_t*>( vBuffer + 16 ); }
		/** Количество обязательных терминов после данного элемента. */
		unsigned int GetRealTermsAfter() const { return *reinterpret_cast<const uint16_t*>( vBuffer + 18 ); }


		/** Флаг последнего термина в поисковом шаблоне. */
		bool GetFlagLastTerm() const { return vBuffer[FLAGS_BASE] & 0x01; }
		/** Флаг первого не виртуального термина. */
		bool GetFlagFirstTerm() const { return vBuffer[FLAGS_BASE] & 0x02; }
		/** Флаг звездочки перед термином. */
		bool GetFlagStarBefore() const { return vBuffer[FLAGS_BASE] & 0x04; }
		/** Флаг суперзвездочки перед термином. */
		bool GetFlagSuperStarBefore() const { return vBuffer[FLAGS_BASE] & 0x08; }
		/** Флаг звездочки после термина. */
		bool GetFlagStarAfter() const { return vBuffer[FLAGS_BASE] & 0x10; }
		/** Флаг суперзвездочки после термина. */
		bool GetFlagSuperStarAfter() const { return vBuffer[FLAGS_BASE] & 0x20; }
		/** Флаг текстового термина. */
		bool GetFlagRealTerm() const { return vBuffer[FLAGS_BASE] & 0x40; }
		/** Флаг процессирования после термина. */
		bool GetFlagProcessAfter() const { return vBuffer[FLAGS_BASE] & 0x80; }
		/** Флаг процессирования перед термином. */
		bool GetFlagProcessBefore() const { return vBuffer[FLAGS_BASE + 1] & 0x01; }
		/** Флаг словаря после термина. */
		bool GetFlagDictAfter() const { return vBuffer[FLAGS_BASE + 1] & 0x02; }
		/** Флаг словаря перед термином. */
		bool GetFlagDictBefore() const { return vBuffer[FLAGS_BASE + 1] & 0x04; }
		/** Флаг пустого словаря перед термином. */
		bool GetFlagEmptyDictBefore() const { return vBuffer[FLAGS_BASE + 1] & 0x08; }
		/** Флаг пустого словаря после термина. */
		bool GetFlagEmptyDictAfter() const { return vBuffer[FLAGS_BASE + 1] & 0x10; }


	protected:
		const char * vBuffer { nullptr };
	};
};



class TermsStorageWR
{
public:
	TermsStorageWR( nMemoryAllocator & aMemoryAllocator ) : vMemoryAllocator( aMemoryAllocator ) { vTermIndex.Create(); }

public:
	/**
	 *  Добавить в базу атрибут термина.
	 * @param aTerm - термин.
	 * @param aTermLength - длина термина.
	 * @param aTermAttrs - атрибуты термина.
	 * @param aTermId - в этом аргументе функция возвращает идентификатор, по которому можно все наборы атрибутов для данного термина.
	 * @param aTermIsNew - в этом аргументе функция возвращает признак того, что такой термин встречен впервый.
	 */
	InfEngineErrors AddTermAttrs( const char * aTerm, unsigned int aTermLength,
								  TermsStorage::TermAttrs & aTermAttrs, unsigned int & aTermId, bool & aTermIsNew );


	/** Получение размера памяти, необходимого для сохранения. **/
	unsigned int GetNeedMemorySize() const;

	/**
	 *  Сохранить базу атрибутов в буфер.
	 * @param aBuffer - буфер.
	 * @param aBufferSize - размер буфера в байтах.
	 * @param ausedMemory - размер использованной для сохранения части буфера в байтах.
	 */
	InfEngineErrors Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aUsedMemory ) const;

	/** Сброс базы атрибутов терминов. */
	void Reset();


private:
	typedef avector<TermsStorage::TermAttrs*> TermAttrsList;


private:
	/** Словарь терминов. */
	NanoLib::NameIndex vTermIndex;

	/** Список атрибутов терминов. На i-ой позиции находится список терминов атрибута с TermId = i. */
	avector<TermAttrsList*> vAttrs;

	/** Менеджер памяти. */
	nMemoryAllocator & vMemoryAllocator;

	/** Переменные, используемые для оптимизации. */
	mutable TermAttrsList vTermAttrsList;
};


class TermsStorageRO
{
public:
	struct Iterator;
	struct CommonIterator;
	struct PersonIdIterator;

public:
	/**
	 *  Открыть базу с атрибутами терминов.
	 * @param aBuffer - буфер.
	 */
	void Open( const char * aBuffer ) { vBuffer = aBuffer; }

	InfEngineErrors SearchAttrs( unsigned int aTermId, CommonIterator & aIterator ) const;

	InfEngineErrors SearchAttrs( unsigned int aTermId, unsigned int aPersonId, PersonIdIterator & aIterator ) const;

	unsigned int GetTermsCount() const { return *reinterpret_cast<const uint32_t*>( vBuffer ); }

private:
	const char * vBuffer { nullptr };
};

struct TermsStorageRO::Iterator
{
public:
	/**
	 *  Фильтр для найденных терминов.
	 */
	class Filter
	{
	public:
		/**
		 * Возвращает true, если термин следует включить в результаты поиска, иначе - false.
		 * @param aTermAttrsRO - атрибуты термин.
		 * @param aCoords - позиция термина во входной фразе.
		 */
		virtual bool IsAccepted( const TermsStorage::TermAttrsRO * aTermAttrsRO, const NDocImage::TermCoords & aCoords ) const = 0;
	};

public:
	virtual const TermsStorage::TermAttrsRO * Next() = 0;

	virtual const bool IsEnd() const = 0;

	virtual const TermsStorage::TermAttrsRO * Item() const = 0;

	virtual void SetEnd() = 0;

	void InitFilters( const Filter* const * aFilters, unsigned int aFiltersCount, const NDocImage::TermCoords * aCoords )
	{
		vFilters = aFilters;
		vFiltersCount = aFiltersCount;
		vCoords = aCoords;
	}

	bool TermIsValid( const TermsStorage::TermAttrsRO * aTermAttrsRO ) const
	{
		if( aTermAttrsRO && vFilters && vFiltersCount && vCoords )
		{
			for( unsigned int filter_n = 0; filter_n < vFiltersCount; ++filter_n )
				if( !vFilters[filter_n]->IsAccepted( aTermAttrsRO, *vCoords ) )
					return false;
		}

		return true;
	}


protected:
	/** Список внешних фильтров для поиска. */
	const Filter* const * vFilters { nullptr };

	/** Количество внешних фильтров для поиска. */
	unsigned int vFiltersCount { 0 };

	/** Координаты термина, найденного во входной фразе. */
	const NDocImage::TermCoords * vCoords { nullptr };
};


struct TermsStorageRO::CommonIterator : public TermsStorageRO::Iterator
{
public:
	const TermsStorage::TermAttrsRO * Next();

	const bool IsEnd() const { return !vItem; }

	const TermsStorage::TermAttrsRO * Item() const { return IsEnd() ? nullptr : &vCurrent; }

	void SetEnd() { vItem = nullptr; }


public:
	unsigned int GetNumber() { return !vItem ? static_cast<unsigned int>( -1 ) : ( vItem - GetFirst() ) / TermsStorage::ATTRS_SIZE; }


public:
	void Set( const char * aBuffer )
	{
		vBuffer = aBuffer;
		vItem = GetFirst();
		if( vItem )
		{
			vCurrent.Init( vItem );
			if( !TermIsValid( &vCurrent ) )
				Next();
		}
	}


public:
	unsigned int GetAttrsNumber() const { return *reinterpret_cast<const uint32_t*>( vBuffer ); }

	unsigned int GetPersonIdNumber() const { return *reinterpret_cast<const uint32_t*>( vBuffer + sizeof( uint32_t ) ); }

	const char * GetFirst() const { return vBuffer + 2 * sizeof( uint32_t ) + GetPersonIdNumber() * 3 * sizeof( uint32_t ); }


protected:
	/** Информация о термние. */
	const char * vBuffer { nullptr };

public:
	const char * vItem { nullptr };

	/** Текущий набор атрибутов термина. */
	TermsStorage::TermAttrsRO vCurrent;
};


struct TermsStorageRO::PersonIdIterator : public TermsStorageRO::Iterator
{
public:
	const TermsStorage::TermAttrsRO * Next();

	const TermsStorage::TermAttrsRO * BinarySearch( unsigned int aId, unsigned int aSubId );

	const bool IsEnd() const { return !vItem; }

	const TermsStorage::TermAttrsRO * Item() const { return IsEnd() ? nullptr : &vCurrent; }

	void SetEnd() { vItem = nullptr; }


public:
	void Set( const char * aBuffer, unsigned int aPersonId );

	unsigned int GetPersonIdNumber() const { return *reinterpret_cast<const uint32_t*>( vBuffer + sizeof( uint32_t ) ); }

	const char * GetFirst();


protected:
	unsigned int GetAttrsNumber() const
	{
		return vPersonIdNum == static_cast<unsigned int>( -1 )
			   ? 0
			   : reinterpret_cast<const uint32_t*>( vBuffer + 2 * sizeof( uint32_t ) + vPersonIdNum * 3 * sizeof( uint32_t ) )[2];
	}


protected:
	/** Информация о термние. */
	const char * vBuffer { nullptr };

	/** Условие на PersonId. */
	unsigned int vPersonId { static_cast<unsigned int>( -1 ) };

	/** Порядковый номер в индексе для заданного PersonId. */
	unsigned int vPersonIdNum { static_cast<unsigned int>( -1 ) };

	/** Следующий набор атрибутов термна. */
	const char * vItem { nullptr };

	/** Превый набор атрибутов термна. */
	const char * vFirst { nullptr };

	/** Текущий набор атрибутов термна. */
	TermsStorage::TermAttrsRO vCurrent;

};

/**
 *  Фильтр для отсеивания найденных в словарях терминов по признаку принадлежности к заданным словарям.
 */
class DictTermsFilter : public TermsStorageRO::Iterator::Filter
{
public:
	/**
	 * Возвращает true, если термин следует включить в результаты поиска, иначе - false.
	 * @param aTermAttrsRO - атрибуты термин.
	 * @param aCoords - позиция термина во входной фразе.
	 */
	virtual bool IsAccepted( const TermsStorage::TermAttrsRO * aTermAttrsRO, const NDocImage::TermCoords & aCoords ) const;

	/**
	 *  Добавить идентификатор словаря к списку допустимых.
	 * @param aDictId - идентификатор словаря.
	 */
	InfEngineErrors AddAcceptableDictId( unsigned int aDictId );

	/**
	 *  Очистить список допустимых словарей.
	 */
	void Reset();

private:
	/** Список идентификаторов словарей, термины из которых фильтр допускает. */
	avector<unsigned int> vDictIDToAccept;

	/** Таблица для быстрой проверки принадлежности идентификатора словаря списку допустимых vDictIDToAccept. */
	avector<bool> vFilterMap;
};

#endif //TERMS_STORAGE_HPP
