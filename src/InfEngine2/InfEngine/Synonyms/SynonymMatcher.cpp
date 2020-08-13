#include "SynonymMatcher.hpp"
#include "InfEngine2/_Include/Errors.h"

enum FltTermsErrors SynonymMatcher::AddFound(const TERM* aTerm, const TermsDocImageIterator& aLastPos,
        TDATA_FLAGS_TYPE /*aWhere*/, bool /*aIsHomo*/)
{
    // Получение аттрибутов.    
    const cf_term_attrs * attrs = term_attrs(aTerm);
    if (!attrs)
		ReturnWithError( TERMS_ERROR_EFAULT, "Can't get synonym's attrs");

    static TermsDocImageIterator PrevPos;
    
    if( vLastAddedTerm )
    {
        if( term_id( vLastAddedTerm ) == term_id( aTerm ) && PrevPos == aLastPos )
            return TERMS_OK;
    }
    PrevPos = aLastPos;
    vLastAddedTerm = aTerm;
    
    
    // Добавление атрибутов в список найденных.
    SynonymAttrsRO * synonym_attrs = vSynonyms.grow();
    if (!synonym_attrs)
        return TERMS_ERROR_ENOMEM;

    synonym_attrs->SetBuffer(attrs);

    // Добавление позиций в список найденных.
    vPositions.push_back(aLastPos.GetWordPos());
    if (vPositions.no_memory())
        return TERMS_ERROR_ENOMEM;

    return TERMS_OK;
}
