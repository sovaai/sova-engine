#include "SynonymIndex.hpp"

SynonymIndexWR::SynonymIndexWR( InfLingProc *& aLp, fstorage_section_id aSectionId ):
	vLp( aLp ),
	vTBase( aSectionId )
{
    vSynonymAttrs.SetBuffer(&vAttrs);
}

InfEngineErrors SynonymIndexWR::Create()
{
    // Уничтожение базы синонимов, если она была создана ранее.
    Destroy();

    // Создание базы синонимов.
    enum FltTermsErrors ftres = vTBase.Create(vLp, TERMS_BASE_FLAGS_MAP_ERANGE_WARNING);
    if (ftres != TERMS_OK)
    {
        Destroy();

		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't create FltTermsBase. Return code: %d", ftres);
    }

    return INF_ENGINE_SUCCESS;
}

void SynonymIndexWR::Destroy()
{
    // Очистка базы синонимов.
    
    vTBase.Close();
}

void SynonymIndexWR::Reset()
{    
    Destroy();
}

InfEngineErrors SynonymIndexWR::Save(fstorage* aFStorage)
{
    // Проверка аргументов.
    if (!aFStorage)
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS);

    // Сохранение базы синонимов.
    struct cf_terms_save_params params;
    memset(&params, 0, sizeof ( cf_terms_save_params));
    enum FltTermsErrors ftres = vTBase.Save(&params, aFStorage);
    if (ftres != TERMS_OK)
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't save FltTermsBase. Return code: %d", ftres);

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymIndexWR::AddSynonym(const SynonymParser::Synonym * aSynonym, unsigned int aMainID,
        unsigned short aGroupID, unsigned short aDictId)
{
    // Проверка аргументов.
    if (!aSynonym)
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS);

    vSynonymAttrs.SetValues(aMainID, aGroupID, aDictId);

    enum FltTermsErrors ftres = vTBase.Add(vSynonymAttrs.GetAttrs(), aSynonym->Text);    
    if (ftres != TERMS_OK)
    {
        if (ftres == TERMS_WARN_EXIST)
            return INF_ENGINE_WARN_SYNONYM_DUPLICATE;

		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't add pattern term. Return Code: %d", ftres);
    }
    return INF_ENGINE_SUCCESS;
}

SynonymIndexRO::SynonymIndexRO( InfLingProc *& aLp, fstorage_section_id aSectionId ):
	vLp( aLp ),
	vTBase( aSectionId )
{
    vState = stNone;
}

InfEngineErrors SynonymIndexRO::Open(fstorage* aFStorage)
{
    // Проверка агрументов.
    if (!aFStorage)
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS);

    // Очистка данных.
    Close();

    // Загрузка базы терминов.
    enum FltTermsErrors ftres = vTBase.Open(vLp, aFStorage, TERM_BASE_OPEN_RO_SHARED);
    if (ftres != TERMS_OK)
    {
        Close();

		ReturnWithError( INF_ENGINE_ERROR_CANT_OPEN, "Can't open FltTermsBase. Return Code: %d", ftres);
    }

    vState = stReady;

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymIndexRO::Check(const DocImage& aRequest, FltTermsMatcher& aMatcher)
{
    // Проверка готовности базы для поиска.
    if (!IsReady())
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE);

    // Поиск терминов в индексной базе.
    FltTermsErrors fte = vTBase.Check(&aMatcher, &aRequest);
    if (fte != TERMS_OK)
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't check terms. Return Code: %d", fte);

    return INF_ENGINE_SUCCESS;
}

InfEngineErrors SynonymIndexRO::LemmatizeDoc(DocText& aDocText, DocImage& aDocImage)
{
    // Лемматизация.
    LingProcErrors lpe = vLp->LemmatizeDoc(aDocText, aDocImage);
    if (lpe != LP_OK)
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't lemmatize DocImage. Return Code: %d", lpe);

    return INF_ENGINE_SUCCESS;
}

void SynonymIndexRO::Close()
{
    vTBase.Close();

    vState = stNone;
}
