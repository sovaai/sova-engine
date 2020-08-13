#include "SymbolymTables.hpp"

#include "tables/ukraine.hpp"
#include "tables/kyrgyz.hpp"
#include "tables/georgian.hpp"

struct SymbolymTable {
	const SymbolymCommon::Symbolym * table;
	unsigned int size;
};

static SymbolymTable Tables[] = {
	SymbolymTable{ SymbolymCommon::Tables::Ukraine::table, SymbolymCommon::Tables::Ukraine::table_size },
	SymbolymTable{ SymbolymCommon::Tables::Kyrgyz::table, SymbolymCommon::Tables::Kyrgyz::table_size },
	SymbolymTable{ SymbolymCommon::Tables::Georgian::table, SymbolymCommon::Tables::Georgian::table_size }
};

unsigned int SymbolymTables::GetTablesCount() {
	return sizeof( Tables ) / sizeof( Tables[0] );
}

unsigned int SymbolymTables::GetTableSize( unsigned int aTableN ) {
	return aTableN < GetTablesCount() ? Tables[aTableN].size : 0;
}

InfEngineErrors SymbolymTables::GetSymbolymGroup( unsigned int aTableN, unsigned int aGroupN, SynonymParser::SynonymGroup & aGroup ) {
	if( aTableN < GetTablesCount() && aGroupN < GetTableSize( aTableN ) ) {
		// Копируем из таблицы заданный набор подставляемых символов.
		const auto & symbolym = Tables[aTableN].table[aGroupN];
		aGroup.Major.Length = strlen( symbolym.vMajor );
		aGroup.Major.Text = nAllocateObjects( vMemoryAllocator, char, aGroup.Major.Length + 1 );
		if( !aGroup.Major.Text )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		strcpy( aGroup.Major.Text, symbolym.vMajor );

		for( unsigned int minor_n = 0; minor_n < symbolym.vMinorsCount; ++minor_n ) {
			SynonymParser::Synonym * s = aGroup.Minors.grow();
			if( aGroup.Minors.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			s->Length = strlen( symbolym.vMinors[minor_n] );
			s->Text = nAllocateObjects( vMemoryAllocator, char, s->Length + 1 );
			if( !s->Text )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
			strcpy( s->Text, symbolym.vMinors[minor_n] );
		}
	}
	else
		ReturnWithError( INF_ENGINE_ERROR_INVALID_ARG, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	return INF_ENGINE_SUCCESS;
}
