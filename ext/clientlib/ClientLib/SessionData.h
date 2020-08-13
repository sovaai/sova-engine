#ifndef __SessionData_h__
#define __SessionData_h__

#ifdef __cplusplus
extern "C"
{
#endif

/** Структура описывающая переменную. */
typedef struct
{
	/** Имя переменной. */
	const char * vName;

	/** Значение переменной. */
	const char * vValue;
} __Var;

/** Структура описывающая сессию. */
struct __SessionData
{
	/** Идентификатор инфа. */
	unsigned int vInfId;

	/** Идентификатор сессии. */
	unsigned int vSessionId;

	/** Количество переменных в сессии. */
	unsigned int vVarsNumber;

	/** Количество выделенной под переменные памяти. */
	unsigned int vVarsAvalible;

	/** Список переменных. */
	__Var * vVars;
};

#ifdef  __cplusplus
}
#endif

#endif /** __SessionData_h__ */
