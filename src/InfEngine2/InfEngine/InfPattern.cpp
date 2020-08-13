#include "InfPattern.hpp"

InfEngineErrors InfPattern::ApplySynonyms( SymbolymBaseRO & aSymbolymsBase, SynonymBaseRO & aSynonymsBase, nMemoryAllocator & aAllocator )
{
	InfEngineErrors iee = INF_ENGINE_SUCCESS;

	for( unsigned int i = 0; i < vQuestionsNum; ++i )
	{
		auto qst = vQuestions[i];

		for( unsigned int j = 0; j < qst->GetItemsNumber(); ++j )
		{
			auto type = qst->GetItemType( j );
			if( InfPatternItems::itText == type )
			{
                iee = static_cast<InfPatternItems::Text*>( qst->GetItem( j ) )->ApplySynonyms( aSymbolymsBase, aSynonymsBase );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

			}
			else if( InfPatternItems::itDictInline == type )
			{
                iee = static_cast<InfPatternItems::TagDictInline*>( qst->GetItem( j ) )->ApplySynonyms( aSymbolymsBase, aSynonymsBase, aAllocator );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );
			}
		}
	}

	if( vThat )
	{
		for( unsigned int i = 0; i < vThat->GetItemsNumber(); ++i )
		{
			auto type = vThat->GetItemType( i );
			if( InfPatternItems::itText == type )
			{
                iee = static_cast<InfPatternItems::Text*>( vThat->GetItem( i ) )->ApplySynonyms( aSymbolymsBase, aSynonymsBase );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );

			}
			else if( InfPatternItems::itDictInline == type )
			{
                iee = static_cast<InfPatternItems::TagDictInline*>( vThat->GetItem( i ) )->ApplySynonyms( aSymbolymsBase, aSynonymsBase, aAllocator );
				if( iee != INF_ENGINE_SUCCESS )
					ReturnWithTrace( iee );
			}
		}
	}

	return INF_ENGINE_SUCCESS;
}
