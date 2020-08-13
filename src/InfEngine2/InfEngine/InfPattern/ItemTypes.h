#ifndef __InfPatternItemTypes_h__
#define __InfPatternItemTypes_h__

#include <cstdint>

#include <stdio.h>

#include <InfEngine2/_Include/Errors.h>

#ifdef __cplusplus
namespace InfPatternItems
{
#endif

/** Типы элементов. **/
typedef enum
{
	itInstruct          = 1, /** Тэг изменения значения переменной. */
	itText              = 2, /** Текстовая строка. */
	itInf               = 3, /** Тэг ответа инфа. */
	itBr                = 4, /** Тег переноса строки. */
	itHref              = 5, /** Тэг ссылки. */
	itOpenWindow        = 8, /** Тэг открытия ссылки в новом окне. */
	itReference         = 9, /** Ссылка на фрагмент поискового шаблона. */
	itGetAnswer         = 10, /** Тэг обращения к шаблон-ответу другого шаблона. */
	itDict              = 11, /** Тэг обращения к словарю. */
	itIf                = 12, /** Тэг выбора. */
	itVar               = 13, /** Тэг обращения к переменной. */
	itStar              = 14, /** Тэг обращения к звездочке. */
	itDictInline        = 15, /** Тэг словаря-inline. */
	itCondition         = 16, /** Условие. */
	itEliStar           = 18, /** Тэг звездочки, используемый для шаблонов эллипсисов. */
	itEliReference      = 19, /** Ссылка на фрагмент поискового шаблона эллипсисов. */
	itFunction          = 20, /** Тэг функции. */
	itFunctionArg       = 21, /** Аргумент функции. */
	itTagRSS            = 22, /** Тэг запроса RSS. */
	itSpace             = 23, /** Тэг пробельного символа. */
	itKeywordArgument   = 24, /** Именованный аргумент. */
	itBreak             = 26, /** Тег отбрасывания ответа. */
	itExtendAnswer      = 27, /** Тег добавления к текущему шаблон-ответу шаблон-ответа из другого шаблона. */
	itDictRef           = 28, /** Ссылка на словарь в шаблон-вопросе по его порядковому номеру. */
	itPre               = 29, /** Неинтерпретируемый текст. */
	itSwitch            = 30, /** Тэг выбора. */
	itTLCS              = 31, /** Условный оператор уровня шаблона. */
	itNothing           = 32, /** Тэг nothing. */
	itDisableautovars   = 33, /** Тэг отключения автоматического изменения переменных. */
	itStartOList        = 34, /** Тэг начала упорядоченного списка. */
	itStartUList        = 35, /** Тэг начала неупорядоченного списка. */
	itListItem          = 36, /** Тэг начала элемента списка. */
	itEndList           = 37, /** Тэг завершения списка */
} ItemType;
#ifdef __cplusplus
}
#endif

#endif /** __InfPatternItemTypes_h__ */
