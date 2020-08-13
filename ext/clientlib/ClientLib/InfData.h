#ifndef __InfData_h__
#define __InfData_h__

#include "SessionData.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** Структура описывающая словарь. */
typedef struct
{
	/** Имя словаря. */
	const char * vName;

	/** Данные словаря. */
	const char * vData;
} __Dict;

/** Структура описывающая данные инфа. */
struct __InfData
{
	/** Идентификатор инфа. */
	unsigned int vInfId;

	/** Шаблоны инфа. */
	const char * vTemplates;

	/** Количество переменных инфа. */
	unsigned int vVarsNumber;

	/** Переменные инфа. */
	__Var * vVars;

	/** Количество словарей инфа. */
	unsigned int vDictsNumber;

	/** Словари инфа. */
	__Dict * vDicts;

	/** Буфер, выделенный под данные инфа. */
	void * vMemory;

	/** Количество памяти, выделенной под данные инфа. Может превышать реальный размер данных. */
	unsigned int vMemAvalible;
};

#ifdef __cplusplus
}
#endif

#endif /** __InfData_h__ */
