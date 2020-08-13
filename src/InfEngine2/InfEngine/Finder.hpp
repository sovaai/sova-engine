#ifndef __Finder_hpp__
#define __Finder_hpp__

#include <algorithm>
#include <list>

#include "Terms/Matcher.hpp"
#include "IndexBase.hpp"
#include "DLData.hpp"

class BFinder
{
public:
	/** Конструктор базового поисковика. */
	BFinder() : vResults( vRuntimeAllocator ) {}

public:
	/**
	 *  Класс, описывающий кандидата на совпадение.
	 */
	class Candidat
	{
	public:
		/**
		 *  Элемент кандидата.
		 */
		class Item
		{
		public:
			/**
			 *  Типа элемента кандидата.
			 */
			typedef enum
			{
				Text       = 0 /** текстовый элемент */,
				InlineDict = 1 /** inline словарь. */,
				Dict       = 2 /** словарь. */,
				LCDict     = 3 /** словарь с пониженным весом. */,
				Star       = 4 /** звездочка. */,
				SuperStar  = 5 /** супер звездочка. */
			} Type;

		public:
			void Reset()
			{
				vStars = false;
				vStarStart = false;
				vStarFinish = false;
				vStarAfter = false;
				vWaitingForEnd = nullptr;
				vWeight = 0;
			}

		public:
			/** Тип элемента. */
			Type vType { Text };

			/** Итератор, указывающий на начало элемента. */
			NDocImage::Iterator vBeginPos;
			/** Итератор, указывающий на конец элемента. */
			NDocImage::Iterator vEndPos;
			
			/** Итератор, указывающий на самую левую возможную позицию следующего элемента. */
			NDocImage::Iterator vMostLeftNextBegin;

			/** Идентификатор совпавшего словаря (если элемент совпал со словарём). */
			unsigned int vDictId { 0 };
			/** Номер совпавшей строки словаря (если элемент совпал со словарём). */
			unsigned int vLineId { 0 };
			/** Вес кандидата  (если элемент совпал со словарём). */
			unsigned int vWeight { 0 };
			/** Наличие звёздочек (если элемент совпал со словарём). */
			bool vStars { false };
			/** Наличие (супер)звёздочки в начале строки (если элемент совпал со словарём). */
			bool vStarStart { false };
			/** Наличие (супер)звёздочки в конце строки (если элемент совпал со словарём). */
			bool vStarFinish { false };

			/** Кончается звёздочкой. */
			bool vStarAfter { false };

			/** Служебное поле. Указывает на предыдущий элемент, кончающийся звёздочкой. */
			Item * vWaitingForEnd { nullptr };
		};

	public:
		/** Идентификатор шаблона. */
		unsigned int vPatternId { 0 };
		/** Идентификатор шаблон-вопроса. */
		unsigned int vQuestionId { 0 };
		/** Массив элементов кандидата. */
		Item * vItems { nullptr };
		/** Размер массива элементов кандидата. */
		unsigned int vItemsNumber { 0 };
		/** Вес кандидата. */
		unsigned int vWeight { 0 };
		/** Вес кандидата, расчитанный только по его элементам (без веса condition). */
		unsigned int vSelfWeight { 0 };
	};

	/**
	 *  Структура, агрегирующая информацию о термине и его вхождении в запрос.
	 */
	struct MatchedTerm
	{
		MatchedTerm( const TermsStorage::TermAttrsRO * aAttrs = nullptr, const CommonMatcher::FoundTerm * aEntry = nullptr ) :
			vAttrs( aAttrs ), vEntry( aEntry ) {}

		const TermsStorage::TermAttrsRO * vAttrs { nullptr };
		const CommonMatcher::FoundTerm * vEntry { nullptr };
	};

	/**
	 *  Класс, реализующий механизм кэширования результатов сравнения шаблоново.
	 */
	class CachedMatchingResults
	{
	public:
		/**
		 *  Результат проверки строки словаря/шаблон-вопроса на совпадение.
		 */
		enum MatchingResult
		{
			MR_NOT_MATCHED = 0 /** Ранее проверка не осуществлялась. */,
			MR_SUCCESS = 1 /**  Строка подходит. */,
			MR_FAIL = 2 /** Строка не подходит. */
		};

		/**
		 *  Проверка, выполнялось ли ранее сопоставление заданной строки словаря / шаблон-впроса.
		 * @param aMainId - идентификатор словаря / шаблона.
		 * @param aSubId - номер строки / вопроса.
		 * @param aResult - результат проверки.
		 */
		InfEngineErrors AlreadyMatched( unsigned int aMainId, unsigned int aSubId, MatchingResult & aResult ) const;

		/**
		 *  Начало первой попытки сопоставления строки/вопроса. Изначально строка/вопрос помечается как не совпавшая.
		 * @param aMainId - идентификатор словаря / шаблона.
		 * @param aSubId - номер строки / вопроса.
		 */
		InfEngineErrors TryMatch( unsigned int aMainId, unsigned int aSubId );

		/**
		 *  Копирование результатов совпадения заданной строки/вопроса в результаты совпадения заданного словаря/шаблона.
		 * @param aMainId - идентификатор словаря.
		 * @param aSubId - номер строки.
		 * @param aCondWeight - вес проверенных условий заданного словаря/шаблона.
		 */
		InfEngineErrors CopyMatch( unsigned int aMainId, unsigned int aSubId, unsigned int & debug_cnt, unsigned int aCondWeight );

		/**
		 *  Добавление новой записи в кэш.
		 * @param aId - идентификатор, выданный хранилищем шаблонов.
		 * @param aPos - позиция первого вхождения в списке результатов.
		 * @param aMainId - идентификатор словаря / шаблона.
		 * @param aSubId - номер строки / вопроса.
		 */
		InfEngineErrors AddToCache( unsigned int aId, unsigned int aPos, unsigned int aMainId, unsigned int aSubId );

		void Reset() { vCache.clear(); }

	protected:
		
		/** Позиции совпадений в списке совпадений vDictMatches. */
		typedef std::list<unsigned int> MatchNList;
		
		/**
		 *  Получение идентификатора строки словаря/шаблон-вопроса, выданного хранилищем шаблонов.
		 * @param aMainId - идентификатор словаря/шаблона.
		 * @param aSubId - номер строки/вопроса.
		 */
		virtual unsigned int GetStringId( unsigned int aMainId, unsigned int aSubId ) const = 0;

		/**
		 *  Копирование результатов совпадения заданной строки/вопроса в результаты совпадения заданного словаря/шаблона.
		 * @param aPos - позиции копируемых вхождений в всписке вхождений.
		 * @param aMainId - идентификатор словаря/шаблона.
		 * @param aSubId - номер строки/вопроса.
		 * @param aCondWeight - вес проверенных условий заданного словаря/шаблона.
		 */
		virtual InfEngineErrors CopyFrom( const MatchNList & aPos, unsigned int aMainId, unsigned int aSubId, unsigned int & debug_cnt, unsigned int aCondWeight ) = 0;

	private:
		/**
		 *  Поля для кэширования результатов поиска.
		 */		
		struct CacheNode
		{
			CacheNode() {}

			CacheNode( MatchingResult aMatchingResult, unsigned int aMatchN, unsigned int aMainId, unsigned int aSubId ) :
				vMatchingResult { aMatchingResult }
			{
				vMatchN.push_back( aMatchN );
				vMainId = aMainId;
				vSubId = aSubId;
			}

			/** Результат сопоставления. */
			MatchingResult vMatchingResult { MR_NOT_MATCHED };

			/** Позиции совпадений в списке совпадений vDictMatches. */
			MatchNList vMatchN;

			/** Идентификатор словаря / шаблона. */
			unsigned int vMainId = -1;

			/** Номер строки в словаре / вопроса в шаблоне. */
			unsigned int vSubId = -1;
		};
		
		/** Ключ кэша - идентификатор строки словаря, выданный хранилищем шаблонов. */
		typedef unsigned int CacheKey;

		/** Кэш найденных вхождений. */
		typedef std::map<CacheKey, CacheNode> Cache;		
		
		Cache vCache;
	};

	/**
	 *  Класс, содержащий результаты сопоставления словарей.
	 */
	class DictsMatchingResults : public CachedMatchingResults
	{
	public:
		/** Информация о совпадении словаря. */
		struct DictMatchInfo;

		/**
		 *  Суммарная информация о всех совпадениях одного словаря.
		 */
		struct DictInfo
		{
			/** Проверка словаря на наличие вхождений. */
			bool IsEmpty() const { return vMinMatchLen == -1; }

			/** Минимальная длина вхождения в словах. */
			unsigned int vMinMatchLen = -1;
			/** Максимальная длина вхождения в словах. */
			unsigned int vMaxMatchLen { 0 };

			/** Самая левая позиция начала вхождения. */
			unsigned int vMostLeftBeginPos = -1;
			/** Самая правая позиция начала вхождения */
			unsigned int vMostRightBeginPos { 0 };

			/** Самая левая позиция конца вхождения (номер последнего слова вхождения, а не следующего за ним). */
			unsigned int vMostLeftEndPos = -1;
			/** Самая правая позиция конца вхождения (номер последнего слова вхождения, а не следующего за ним). */
			unsigned int vMostRightEndPos { 0 };

			/** Максимальный вес вхождения словаря. */
			unsigned int vMaxWeight { 0 };

			/** Флаг, показывающий, были ли учтены подсловари данного словаря. */
			bool vDisclosed { false };
		};

		/** Класс, содержащий список найденных вхождений для словаря. */
		class DictMatch;

	public:
		void SetDLData( const DLDataRO * aDLData ) { vDLData = aDLData; }

		/**
		 *  Индексация найденных результатов.
		 * @param vMemoryAllocator - менеджер памяти.
		 */
		InfEngineErrors BuildIndex( nMemoryAllocator & vMemoryAllocator );

		/**
		 *  Слияние нескольких наборов резльтатов поиска по словарям из разных баз в один набор.
		 * @param aResults - список комплектов результатов поиска по словарям из разных баз.
		 * @param aDLDataRO - список баз, по которым проводился поиск.
		 * @param aCount - количество результатов.
		 * @param aJoiningMethod - метод объединения словарей.
		 * @param aMemoryAllocator - менеджер памяти для создания объекта.
		 */
		InfEngineErrors CreateByJoining( const DictsMatchingResults ** aResults, const DLDataRO ** aDLDataRO, unsigned int aCount,
										 InfDictWrapManipulator::JoiningMethod aJoiningMethod, nMemoryAllocator & aMemoryAllocator );

		/**
		 *  Добавление информации о новом найденном вхождении.
		 * @param aDictId - идентификатор словаря.
		 * @param aLineId - номер строки.
		 * @param aBeginPos - итератор, указывающий на начало элемента.
		 * @param aEndPos - итератор, указывающий на конец элемента.
		 * @param aStarBefore - наличие звёздочек или суперзвёздочек в начале строки.
		 * @param aStarAfter - наличие звёздочек или суперзвёздочек в конце строки.
		 * @param aStars - наличие звёздочек или суперзвёздочек.
		 * @param aWeight - вес совпадения.
		 */
		InfEngineErrors AddMatch( unsigned int aDictId, unsigned int aLineId, NDocImage::Iterator aBeginPos, NDocImage::Iterator aEndPos,
								  unsigned int aStarBefore, unsigned int aStarAfter, bool aStars, unsigned int aWeight );

	public:
		/** Очистка выделенной памяти. */
		void Reset();

		/**
		 *  Поиск списка вхождений словаря.
		 * @param aDictId - идентификатор словаря.
		 * @param aDictMatch - результат.
		 */
		InfEngineErrors GetMatch( unsigned int aDictId, DictMatch & aDictMatch ) const;

		/**
		 *  Поиск суммарной информации о вхождениях заданного словаря.
		 * @param aDictId - идентификатор словаря.
		 */
		InfEngineErrors GetDictInfo( unsigned int aDictId, const DictInfo* & aDictInfo ) const;

		/**
		 *  Поиск максимального веса для вхождения словаря.
		 * @param aDictId - идентификатор словаря.
		 */
		unsigned int GetMaxWeight( unsigned int aDictId ) const { return aDictId < vDictInfo.size() ? vDictInfo[aDictId].vMaxWeight : 0; }

		/** Возвращает количество найденных вхождений. */
		unsigned int GetmatchesNumber() const { return vDictMatches.size(); }

	protected:
		/**
		 *  Получение идентификатора строки словаря, выданного хранилищем шаблонов.
		 * @param aDictId - идентификатор словаря.
		 * @param aLineId - номер строки.
		 */
		unsigned int GetStringId( unsigned int aDictId, unsigned int aLineId ) const;

		/**
		 *  Копирование результатов совпадения заданной строки в результаты совпадения заданного словаря.
		 * @param aPos - позиции копируемых вхождений в всписке вхождений.
		 * @param aDictId - идентификатор словаря.
		 * @param aLineId - номер строки.
		 * @param aCondWeight - аргумент не используется, объясвлен для совместимости.
		 */
		InfEngineErrors CopyFrom( const MatchNList & aPos, unsigned int aDictId, unsigned int aLineId, unsigned int & debug_cnt, unsigned int aCondWeight );				

	private:
		
	public:
		// Список совпадений в словарях, отсортированный по номерам словарей, строк и весам.
		avector<DictMatchInfo> vDictMatches;

		// Массив, в котором на i-ой позиции находится список найденных вхождений словаря с идентификатором i.
		avector<DictMatchInfo**> vIndex;
		// Массив, в котором на i-ой позиции находится длина списка найденных вхождений словаря с идентификатором i.
		avector<unsigned int> vIndexCeilSize;
		// Массив, в котором на i-ой позиции находится суммарная информация о вхождениях словаря с идентификатором i.
		avector<DictInfo> vDictInfo;

		// Контейнер для хранения результатов выполнения запросов к методу GetMatch с учётом подсловарей.
		mutable avector<DictMatchInfo> vDisclosedMatches;
		// Массив, в котором на i-ой позиции находится номер позиции в массиве vDisclosedMatches - начало списка найденных вхождений словаря с идентификатором i, включае его подсловарии.
		mutable avector<unsigned int> vDisclosedIndex;
		// Массив, в котором на i-ой позиции находится длина списка в массиве vDisclosedMatches - списка найденных вхождений словаря с идентификатором i, включае его подсловарии.
		mutable avector<unsigned int> vDisclosedIndexCeilSize;
		// Массив, в котором на i-ой позиции находится суммарная информация о вхождениях словаря с идентификатором i, включая его подсловари.
		mutable avector<DictInfo> vDisclosedDictInfo;

		// Скомпилированная база.
		const DLDataRO * vDLData { nullptr };
	};

	class ThatMatchingResults
	{
	private:
		class Item
		{
		public:
			Item( unsigned int aPatternId = 0, bool aValid = false ) { vPatternId = aPatternId; IsValid = aValid; }
		public:
			bool operator<( const Item & aItem ) const { return vPatternId < aItem.vPatternId; }

		public:
			unsigned int vPatternId { 0 };
			bool IsValid { false };
		};

	public:
		void Reset() { vPatternsId.clear(); }

		InfEngineErrors SetValid( unsigned int aPatternId )
		{
			vPatternsId.push_back( { aPatternId, true } );
			if( vPatternsId.no_memory() )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
			return INF_ENGINE_SUCCESS;
		}

		InfEngineErrors SetInvalid( unsigned int aPatternId )
		{
			vPatternsId.push_back( { aPatternId, false } );
			if( vPatternsId.no_memory() )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
			return INF_ENGINE_SUCCESS;
		}

		void BuildIndex() { std::sort( vPatternsId.get_buffer(), vPatternsId.get_buffer() + vPatternsId.size() ); }

		bool IsValidPattern(  unsigned int aPatternId  ) const
		{
			const Item * item = std::lower_bound( vPatternsId.get_buffer(), vPatternsId.get_buffer() + vPatternsId.size(), Item { aPatternId, false } );
			if( item >= vPatternsId.get_buffer() && item < vPatternsId.get_buffer() + vPatternsId.size() )
				return item->vPatternId == aPatternId && item->IsValid;
			return false;
		}

	private:
		avector<Item> vPatternsId;
	};


	class QuestionMatchingResults : public CachedMatchingResults
	{
	public:
		QuestionMatchingResults( nMemoryAllocator & aAllocator ) : vAllocator( aAllocator ) {}

	public:
		void SetDLData( const DLDataRO * aDLData ) { vDLData = aDLData; }

		/**
		 *  Добавление найденного кандидата в контейнер с результатами поиска по шаблон-вопросам.
		 * @param aCandidat
		 */
		InfEngineErrors AddCandidat( Candidat * aCandidat );

		/** Завершение подготовки кандидатов. */
		InfEngineErrors BuildIndex();

		/** Сортировка кандидатов по весу. */
		InfEngineErrors SortByWeight();

		/** Получение количества найденных кандидатов */
		unsigned int GetCandNumber() const { return vCandidats.size(); }

		/**
		 *  Получение заданного кандидата.
		 * @param aNum - номер кандидата.
		 */
		Candidat * GetCandidat( unsigned int aNum ) { return vCandidats[aNum]; }

		/** Очистка контейнера с результатами. */
		void Reset()
		{
			CachedMatchingResults::Reset();
			vCandidats.clear();
		}

		avector<Candidat*> & GetCandidats() { return vCandidats; }

	protected:
		/**
		 *  Получение идентификатора шаблон-вопроса, выданного хранилищем шаблонов.
		 * @param aPatternId - идентификатор шаблона.
		 * @param aQuestionId - номер шаблон-вопроса.
		 */
		unsigned int GetStringId( unsigned int aPatternId, unsigned int aQuestionId ) const
		{
			return vDLData->GetQuestionString( aPatternId, aQuestionId ).GetId();
		}

		/**
		 *  Копирование результатов совпадения заданного шаблон-вопроса в результаты совпадения заданного шаблон-шаблона.
		 * @param aPos - позиции копируемых вхождений в всписке вхождений.
		 * @param aPatternId - идентификатор шаблона.
		 * @param aQuestionId - номер шаблон-вопроса.
		 * @param aCondWeight - вес проверенных условий заданного шаблона.
		 */
		InfEngineErrors CopyFrom( const MatchNList & aPos, unsigned int aPatternId, unsigned int aQuestionId, unsigned int & debug_cnt, unsigned int aCondWeight );		

		/**
		 *  Получение веса шаблон-впроса, вычисленного на этапе компиляции.
		 * @param aPatternId - идентификатор шаблона.
		 * @param aQuestionId - номер шаблон-вопроса.
		 */
		unsigned int GetQuestionWeight( unsigned int aPatternId, unsigned int aQuestionId ) const
		{
			return vDLData->GetQuestionString( aPatternId, aQuestionId ).GetWeight();
		}

	private:
		avector<Candidat*> vCandidats;

		const DLDataRO * vDLData { nullptr };

		nMemoryAllocator & vAllocator;
	};

public:
	/** Данные, необходимые для поиска. */
	class SearchData
	{
	public:
		SearchData( IndexBaseRO & aIndexQBase, IndexBaseRO * aIndexTBase, IndexBaseRO * aIndexDBase,
					SymbolymBaseRO * aSymbolymBase, SynonymBaseRO * aSynonymBase, const DLDataRO & aDLData, 
					const NanoLib::NameIndex & aInfPersonRegistry ) :
			vIndexQBase( aIndexQBase ), vIndexTBase( aIndexTBase ), vIndexDBase( aIndexDBase ),
			vSymbolymBase( aSymbolymBase ), vSynonymBase( aSynonymBase ), vDLData( aDLData ), 
			vInfPersonRegistry( aInfPersonRegistry ) {}

		/** Индексная база шаблон-вопросов. */
		IndexBaseRO & vIndexQBase;
		/** Индексная база that-шаблонов. Может быть nullptr. */
		IndexBaseRO * vIndexTBase { nullptr };
		/** Индексная база словарей. Может быть nullptr. */
        IndexBaseRO * vIndexDBase { nullptr };
        /** База заменяемых символов. */
        SymbolymBaseRO * vSymbolymBase { nullptr };
		/** База синонимов. */
		SynonymBaseRO * vSynonymBase { nullptr };

		/** Данные шаблонов. */
		const DLDataRO & vDLData;
		/** Реестр InfPerson'ов. */
		const NanoLib::NameIndex & vInfPersonRegistry;
	};

	void Reset();

public:
	avector<Candidat*> & GetResults() { return vResults.GetCandidats(); }

protected:
	/**
	 * @brief Сопоставление словарей.
	 * @param aDictMatcher - матчер словарных терминов.
	 * @param aDictsMatchingResults - контейнер для результатов сопоставления словарных терминов.
	 * @param aDLData - база с данными DL.
	 */
	InfEngineErrors ProcessDictsTerms(DictMatcher & aDictMatcher, DictsMatchingResults & aDictsMatchingResults, const DLDataRO & aDLData );

	/**
	 *  Сопоставление словарей.
	 * @param aPattrensData - база данных DL.
	 * @param aConditionsRegistry - реестр условий.
	 * @param aSession - данные сессии.
	 * @param aPatternMatcher - матчер словарных терминов.
	 * @param aDictsMatchingResults - результат сопоставления словарных терминов.
	 * @param aFirstMatchingOnly -
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 * @param aUserBase -
	 */
	InfEngineErrors ProcessPatternTerms( const DLDataRO & aPatternsData, const InfConditionsRegistry & aConditionsRegistry,
										 const Session & aSession, PatternMatcher & aPatternMatcher,
										 const DictsMatchingResults & aDictsMatchingResults, bool aFirstMatchingOnly,
										 RequestStat & aRequestStat, bool aUserBase );

	/**
	 *  Сопоставление шаблон-вопроса на основе уже сопоставленных обязательных терминов.
	 * @param aQuestion - шаблон-вопрос.
	 * @param aPatternMatcher - матчер словарных терминов.
	 * @param aMatchedTerms - ифнормация о сопоставлении обязательных терминов.
	 * @param aTermsNumber - количество сопоставленных обязательных терминов.
	 * @param aDictsMatchingResults - данные о результатах сопоставления словарей.
	 * @param aCandidats - в этом аргументе в случаче успеха возвращаеются подготовленные кандидаты.
	 * @param aCondWeight - поправка веса шаблона, сделанная при проверке условий шаблона.
	 * @param aIsBestMatch - флаг, показывающий, что данное совпадение заведомо имеет наибольший вес среди всех возможных совпадений с данным шаблон-вопросом.
	 */
	InfEngineErrors MatchPattern( InfPatternItems::QuestionRO & aQuestion, const PatternMatcher & aPatternMatcher, const MatchedTerm * aMatchedTerms,
								  unsigned int aTermsNumber, const DictsMatchingResults & aDictsMatchingResults, avector<Candidat*> & aCandidats,
								  unsigned int aCondWeight, bool & aIsBestMatch );

	/**
	 *  Сопоставление шаблон-вопроса, не содержащего обязательных терминов.
	 * @param aRequest - запрос пользователя.
	 * @param aQuestion - шаблон-вопрос.
	 * @param aPatternsData - база данных DL.
	 * @param aDictsMatchingResults - данные о результатах сопоставления словарей.
	 * @param aCandidats - в этом аргументе в случаче успеха возвращается подготовленные кандидаты.
	 * @param aCondWeight - поправка веса шаблона, сделанная при проверке условий шаблона.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 * @param aUserBase - флаг, показывающий, что поиск идёт по пользовательской базе.
	 */
	InfEngineErrors MatchPatternWT( const NDocImage & aRequest, InfPatternItems::QuestionRO & aQuestion, const DLDataRO & aPatternsData,
									const DictsMatchingResults & aDictsMatchingResults, avector<Candidat*> & aCandidats,
									unsigned int aCondWeight, RequestStat & aRequestStat, bool aUserBase );


	/**
	 *  Вычисляет вес кандидата.
	 * @param aCandidat - успешно сопоставленный кандидат.
	 * @param aTermsWeight - суммарный вес терминов, включая inline-словари.
	 * @param aDictsWeight - суммарный вес словарей, не считая inline-словари.
	 */
	unsigned int CalculateCandidatWeight( Candidat* & aCandidat, unsigned int & aTermsWeight, unsigned int & aDictsWeight );

	/**
	 *  Поиск списка шаблонов, удовлетворяющих условиям that-строк.
	 * @param aIndexTBase - индексная база шаблонов-that.
	 * @param aPatternsData - данные шаблонов.
	 * @param aInfPersonRegistry - реестр InfPerson'ов.
	 * @param aConditionsRegistry - реестр условий.
	 * @param aThatString - подготовленная that-строка.
	 * @param aSession - данные сессии.
	 * @param aDictsMatchingResults -
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 * @param aUserBase - флаг, показывающий, что поиск идёт по пользовательской базе.
	 */
	InfEngineErrors SearchThats( IndexBaseRO & aIndexTBase, const DLDataRO & aPatternsData,
								 const NanoLib::NameIndex & aInfPersonRegistry,
								 const InfConditionsRegistry & aConditionsRegistry, DocImage & aThatString,
								 const Session & aSession, DictsMatchingResults & aDictsMatchingResults,
								 RequestStat & aRequestStat, bool aUserBase );

	/**
	 *  Поиск подходящих шаблон-вопросов.
	 * @param aIndexQBase - индексная база шаблонов-вопросов.
	 * @param aPatternsData - данные шаблонов.
	 * @param aInfPersonRegistry - реестр InfPerson'ов.
	 * @param aConditionsRegistry - реестр условий.
	 * @param aRequest - подготовленная строка запроса.
	 * @param aSession - данные сессии.
	 * @param aDictsMatchingResults - результаты сопоставления словарей.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 * @param aUserBase - флаг, показывающий, что поиск идёт по пользовательской базе.
	 */
	InfEngineErrors SearchQuestions( IndexBaseRO & aIndexQBase, const DLDataRO & aPatternsData, const NanoLib::NameIndex & aInfPersonRegistry,
									 const InfConditionsRegistry & aConditionsRegistry, DocImage & aRequest, const Session & aSession,
									 DictsMatchingResults & aDictsMatchingResults, RequestStat & aRequestStat, bool aUserBase );

	/**
	 *  Поиск безтермовых шаблонов.
	 * @param aRequest - запрос пользователя.
	 * @param aSession - сессия.
	 * @param aPatternsData - данные шаблонов.
	 * @param aInfPersonRegistry - реестр InfPerson'ов.
	 * @param aPatterns - список безтерминных шаблонов.
	 * @param aDictsData -
	 * @param aConditionsRegistry - реестр условий.
	 * @param aQstFlag - флаг, показывающий ищем мы шаблон-вопросы или проверяем шаблон-that.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 * @param aUserBase - флаг, показывающий, что поиск идёт по пользовательской базе.
	 */
	InfEngineErrors SearchPatternsWithoutTerms( DocImage & aRequest, const Session & aSession, const DLDataRO & aPatternsData, const NanoLib::NameIndex & aInfPersonRegistry,
												const IndexBaseRO & aPatterns, DictsMatchingResults & aDictsData, const InfConditionsRegistry & aConditionsRegistry,
												bool aQstFlag, RequestStat & aRequestStat, bool aUserBase );

	/**
	 *  Поиск подходящих шаблон-вопросов.
	 * @param aIndexQBase - индексная база шаблонов-вопросов.
	 * @param aPatternsData - данные шаблонов.
	 * @param aInfPersonRegistry - реестр InfPerson'ов.
	 * @param aConditionsRegistry - реестр условий.
	 * @param aRequest - запрос пользователя.
	 * @param aSession - данные сессии.
	 * @param aDictsMatchingResults -
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 * @param aUserBase - флаг, показывающий, что поиск идёт по пользовательской базе.
	 */
	InfEngineErrors SearchPatterns( IndexBaseRO & aIndexQBase, const DLDataRO & aPatternsData, const NanoLib::NameIndex & aInfPersonRegistry,
									const InfConditionsRegistry & aConditionsRegistry, DocImage & aRequest, const Session & aSession,
									DictsMatchingResults & aDictsMatchingResults, RequestStat & aRequestStat, bool aUserBase );

	/**
	 *  Подборка подошедших шаблонов-that.
	 * @param aIndexTBase - индексная база шаблонов-that.
	 * @param aPatternsData - данные шаблонов.
	 * @param aConditionsRegistry - реестр условий.
	 * @param aInfPersonRegistry - реестр InfPerson'ов.
	 * @param aRequest - that-строка.
	 * @param aSession - сессия.
	 * @param aDictsData - Данные подобранных словарей.
	 * @param aRequestStat - статистика выполнения процедуры запроса.
	 * @param aUserBase - флаг, показывающий, что поиск идёт по пользовательской базе.
	 */
	InfEngineErrors MatchThatPattern( IndexBaseRO & aIndexTBase, const DLDataRO & aPatternsData, const InfConditionsRegistry & aConditionsRegistry,
									  const NanoLib::NameIndex & aInfPersonRegistry, DocImage & aRequest, const Session & aSession,
									  DictsMatchingResults & aDictsData, RequestStat & aRequestStat, bool aUserBase );

	/**
	 *  Проверка условий шаблона.
	 * @param aPatternsData - данные шаблонов.
	 * @param aConditionsRegistry - реестр условий.
	 * @param aInfPatternId - идентификатор шаблона.
	 * @param aSession - сессия.
	 * @param aExtWeight - дополнительный вес шаблона, порождаемый условиями.
	 */
	InfEngineErrors CheckConditions( const DLDataRO & aPatternsData, const InfConditionsRegistry & aConditionsRegistry, unsigned int aInfPatternId,
									 const Session & aSession, unsigned int & aExtWeight );

protected:
	/** Аллокаторы памяти. */

	/** Аллокатор для выделения временных данных. Может быть очищен при каждом внешнем вызове. */
	nMemoryAllocator vRuntimeAllocator;

	/** Матчер для словарей. */
	DictMatcher vDictMatcher;
	/** Матчер для шаблонов. */
	PatternMatcher vPatternMatcher;

	/** Результаты сопоставления словарей. */
	DictsMatchingResults vDictsMatchingResults;

	/** Результаты сопоставления that-строк. */
	ThatMatchingResults vThatMatchingResults;

	class AnswerData
	{
	public:
		AnswerData() {}

		AnswerData( const InfPatternItems::AnswerRO & aAnswer, Candidat * aCandidat = nullptr, const char * aReplyId = nullptr, const char * aInstrLabel = nullptr, unsigned int aAnswerId = 0 )
			: vAnswer( aAnswer ), vCandidat( aCandidat ), vReplyId( aReplyId ), vInstrLabel( aInstrLabel ), vAnswerId( aAnswerId ) {}

	public:
		InfPatternItems::AnswerRO vAnswer;
		Candidat * vCandidat { nullptr };
		const char * vReplyId { nullptr };
		const char * vInstrLabel { nullptr };
		unsigned int vAnswerId { 0 };
	};
	avector<AnswerData> vAnswers;
	avector<AnswerData> vLUniqAnswers;
	avector<AnswerData> vLastAnswers;

	/** Вспомогательные объекты. */
	CDocMessage vCanonizationDocMessage;
	CDocMessage vDocMessage;
	DocImage vDocImage;

	QuestionMatchingResults vResults;

	/** Кэш для проверки условий. */
	avector<char> vConditionsCheckCache;

private:
	/** Служебные объекты. */
	avector<TermsStorageRO::PersonIdIterator> vTSIterators;
};

/**
 *  Информация о совпадении словаря.
 */
struct BFinder::DictsMatchingResults::DictMatchInfo
{
	/** Идентификатор словаря. */
	unsigned int vDictId { 0 };
	/** Номер строки. */
	unsigned int vLineId { 0 };
	/** Идентификатор, выданный хранилищем шаблонов. */
	unsigned int vId { 0 };

	/** Итератор, указывающий на начало элемента. */
	NDocImage::Iterator vBeginPos;
	/** Итератор, указывающий на конец элемента. */
	NDocImage::Iterator vEndPos;

	/** Наличие звёздочек или суперзвёздочек в начале строки. */
	unsigned int vStarBefore { 0 };
	/** Наличие звёздочек или суперзвёздочек в конце строки. */
	unsigned int vStarAfter { 0 };
	/** Наличие звёздочек или суперзвёздочек. */
	bool vStars { false };

	/** Вес совпадения. */
	unsigned int vWeight { 0 };
};

/**
 *  Класс, содержащий список найденных вхождений для словаря.
 */
class BFinder::DictsMatchingResults::DictMatch
{
public:
	DictMatch() {}

	DictMatch( unsigned int aMatchesNumber, const DictsMatchingResults::DictMatchInfo * aMatches )
	{
		vMatchesNumber = aMatchesNumber;
		vMatches = aMatches;
	}

public:
	/** Возвращает количество вхождений словаря. */
	unsigned int GetMatchesNumber() const { return vMatchesNumber; }

	/**
	 *  Возвращает подробную информацию о заданном вхождении словаря.
	 * @param aNum - номер вхождения.
	 */
	const DictsMatchingResults::DictMatchInfo * GetMatch( unsigned int aNum ) const { return &( vMatches[aNum] ); }

private:
	/** Количество вхождений словаря. */
	unsigned int vMatchesNumber { 0 };

	/** Список вхождений словаря. */
	const DictsMatchingResults::DictMatchInfo * vMatches { nullptr };
};

#endif /** __Finder_hpp__ */
