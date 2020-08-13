#ifndef SYMBOLYM_TABLES_HPP
#define SYMBOLYM_TABLES_HPP

#include "../Synonyms/SynonymParser.hpp"

class SymbolymTables {
	public:
		/** Возвращает количество прекомпилированных таблиц замены символов. */
		unsigned int GetTablesCount();

		/** Возвращает количество парвил замены в заданной таблице. */
		unsigned int GetTableSize( unsigned int aTableN );

		/**
		 *  Возвращает заданное правило замены из заданной таблицы.
		 * @param aTableN - номер таблицы.
		 * @param aGroupN - номер правила замены.
		 * @param aGroup - в этом аргументе возвращается результат.
		 */
		InfEngineErrors GetSymbolymGroup( unsigned int aTableN, unsigned int aGroupN, SynonymParser::SynonymGroup & aGroup );

	protected:
		// Менеджер памяти.
		nMemoryAllocator vMemoryAllocator;
};

#endif //SYMBOLYM_TABLES_HPP
