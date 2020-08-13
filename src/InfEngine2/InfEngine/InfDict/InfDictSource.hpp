#ifndef INF_DICT_SOURCE_HPP
#define INF_DICT_SOURCE_HPP

#include <NanoLib/LogSystem.hpp>
#include <NanoLib/aTextStringFunctions.hpp>

#include <InfEngine2/_Include/Errors.h>

/**
 *  Класс для считывания словаря для дальнейшей компиляции.
 */
class InfDictSource
{
public:
	/** Чтение словаря из файла. **/
	InfEngineErrors ReadDictFromFile( const char * aDictPath );

public:
	/** Получение идентификатора словаря. **/
	unsigned int GetId() { return vId; }

	/** Установка идентификатора словаря. **/
	void SetId( unsigned int aDictId ) { vId = aDictId; }


public:
	/** Получение имени словаря. **/
	const char * GetName() const { return vName.ToConstChar(); }

	/** Установка имени словаря.**/
	InfEngineErrors SetName( const char * aDictName, unsigned int aDictNameLen );


public:
	/** Очистка данных. **/
	void Reset() { vId = 0; vBuffer.clear(); vName.clear(); vTmp.clear(); }


private:
	/** Идентификатор словаря. **/
	unsigned int vId { 0 };

	/** Имя словаря. **/
	aTextString vName;

	/** Буфер для хранения словаря. **/
	aTextString vBuffer;

	/** Вспомогательная строка. **/
	aTextString vTmp;
};

#endif  /** INF_DICT_SOURCE_HPP */
