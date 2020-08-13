#include "Answer.h"

#include <stdlib.h>
#include <string.h>

#include <stdint.h>

#include "ItemTypes.hpp"

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
*
*      Ответ сервера на запрос REQUEST и функции для работы с ним.
*
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

void cl_answer_free( Answer * aAnswer )
{
	// Проверка аргументов.
	if( !aAnswer )
		return;

	// Освобождение памяти, занимаемой аргументами элементов ответа.
	if( aAnswer->vItems )
	{
		unsigned int i;
		for( i = 0; i < aAnswer->vItemsNumber; i++ )
		{
			switch( aAnswer->vItems[i].vType )
			{
			case AnswerItemTypeTextString:
				if( aAnswer->vItems[i].vValue.vString.vText )
					free( aAnswer->vItems[i].vValue.vString.vText );
				break;

			case AnswerItemTypeTagInf:
				if( aAnswer->vItems[i].vValue.vInf.vValue )
					free( aAnswer->vItems[i].vValue.vInf.vValue );
				if( aAnswer->vItems[i].vValue.vInf.vRequest )
					free( aAnswer->vItems[i].vValue.vInf.vRequest );
				break;

			case AnswerItemTypeTagBr:
			case AnswerItemTypeStartOList:
			case AnswerItemTypeStartUList:
			case AnswerItemTypeListItem:
			case AnswerItemTypeEndList:
				break;

			case AnswerItemTypeTagHref:
				if( aAnswer->vItems[i].vValue.vHref.vURL )
					free( aAnswer->vItems[i].vValue.vHref.vURL );
				if( aAnswer->vItems[i].vValue.vHref.vLink )
					free( aAnswer->vItems[i].vValue.vHref.vLink );
				if( aAnswer->vItems[i].vValue.vHref.vTarget )
					free( aAnswer->vItems[i].vValue.vHref.vTarget );
				break;

			case AnswerItemTypeInstruct:
				if( aAnswer->vItems[i].vValue.vInstruct.vVarName )
					free( aAnswer->vItems[i].vValue.vInstruct.vVarName );
				if( aAnswer->vItems[i].vValue.vInstruct.vValue )
					free( aAnswer->vItems[i].vValue.vInstruct.vValue );
				break;

			case AnswerItemTypeTagOpenWindow:
				if( aAnswer->vItems[i].vValue.vOpenWindow.vURL )
					free( aAnswer->vItems[i].vValue.vOpenWindow.vURL );
				break;

			case AnswerItemTypeTagRSS:
				if( aAnswer->vItems[i].vValue.vRSS.vURL )
					free( aAnswer->vItems[i].vValue.vRSS.vURL );
				if( aAnswer->vItems[i].vValue.vRSS.vAlt )
					free( aAnswer->vItems[i].vValue.vRSS.vAlt );
				break;
			}
		}
	}

	// Освобождение памяти, занимаемой массивом элементов ответа.
	if( aAnswer->vItems )
		free( aAnswer->vItems );

	// Освобождение памяти, занимаемой ответом.
	free( aAnswer );
}

unsigned int cl_answer_get_size( const Answer * aAnswer )
{
	// Проверка аргументов.
	if( !aAnswer )
		return 0;
	else
		return aAnswer->vItemsNumber;
}

ClientLibReturnCode cl_answer_get_type( const Answer * aAnswer, unsigned int aItemInd, AnswerItemType * aType )
{
	// Проверка аргументов.
	if( !aAnswer || !aType )
		return clrcErrorInvArgs;

	// Проверка корректности запроса.
	if( aAnswer->vItemsNumber <= aItemInd )
		return clrcErrorInvArgs;

	// Подготовка результата.
	*aType = aAnswer->vItems[aItemInd].vType;

	return clrcSuccess;
}

ClientLibReturnCode cl_answer_get_text( const Answer * aAnswer, unsigned int aItemInd, const char ** aTextString, unsigned int * aTextStringLength )
{
	// Проверка аргументов.
	if( !aAnswer || !aTextString || !aTextStringLength )
		return clrcErrorInvArgs;

	// Проверка количества элементов.
	if( aItemInd >= aAnswer->vItemsNumber )
		return clrcErrorInvArgs;

	// Проверка типа элемента.
	if( aAnswer->vItems[aItemInd].vType != AnswerItemTypeTextString )
		return clrcErrorInvArgs;

	// Получение данных.
	*aTextString = aAnswer->vItems[aItemInd].vValue.vString.vText;
	*aTextStringLength = aAnswer->vItems[aItemInd].vValue.vString.vTextLength;

	return clrcSuccess;
}

ClientLibReturnCode cl_answer_get_tag_inf( const Answer * aAnswer, unsigned int aItemInd, const char ** aValue, unsigned int * aValueLength, const char ** aRequest, unsigned int * aRequestLength )
{
	// Проверка аргументов.
	if( !aAnswer || !aValue || !aValueLength || !aRequest || !aRequestLength )
		return clrcErrorInvArgs;

	// Проверка количества элементов.
	if( aItemInd >= aAnswer->vItemsNumber )
		return clrcErrorInvArgs;

	// Проверка типа элемента.
	if( aAnswer->vItems[aItemInd].vType != AnswerItemTypeTagInf )
		return clrcErrorInvArgs;

	// Получение данных.
	*aValue = aAnswer->vItems[aItemInd].vValue.vInf.vValue;
	*aValueLength = aAnswer->vItems[aItemInd].vValue.vInf.vValueLength;
	*aRequest = aAnswer->vItems[aItemInd].vValue.vInf.vRequest;
	*aRequestLength = aAnswer->vItems[aItemInd].vValue.vInf.vRequestLength;

	return clrcSuccess;
}

ClientLibReturnCode cl_answer_get_tag_href( const Answer * aAnswer, unsigned int aItemInd, const char ** aURL, unsigned int * aURLLength,
											const char ** aTarget, unsigned int * aTargetLength, const char ** aLink, unsigned int * aLinkLength )
{
	// Проверка аргументов.
	if( !aAnswer || !aURL || !aURLLength || !aTarget || !aTargetLength || !aLink || !aLinkLength )
		return clrcErrorInvArgs;

	// Проверка количества элементов.
	if( aItemInd >= aAnswer->vItemsNumber )
		return clrcErrorInvArgs;

	// Проверка типа элемента.
	if( aAnswer->vItems[aItemInd].vType != AnswerItemTypeTagHref )
		return clrcErrorInvArgs;

	// Получение данных.
	*aURL = aAnswer->vItems[aItemInd].vValue.vHref.vURL;
	*aURLLength = aAnswer->vItems[aItemInd].vValue.vHref.vURLLength;

	*aLink = aAnswer->vItems[aItemInd].vValue.vHref.vLink;
	*aLinkLength = aAnswer->vItems[aItemInd].vValue.vHref.vLinkLength;

	*aTarget = aAnswer->vItems[aItemInd].vValue.vHref.vTarget;
	*aTargetLength = aAnswer->vItems[aItemInd].vValue.vHref.vTargetLength;

	return clrcSuccess;
}

ClientLibReturnCode cl_answer_get_instruct( const Answer * aAnswer, unsigned int aItemInd, const char ** aVarName, unsigned int * aVarNameLength,
											const char ** aValue, unsigned int * aValueLength )
{
	// Проверка аргументов.
	if( !aAnswer || !aVarName || !aVarNameLength || !aValue || !aValueLength )
		return clrcErrorInvArgs;

	// Проверка количества элементов.
	if( aItemInd >= aAnswer->vItemsNumber )
		return clrcErrorInvArgs;

	// Проверка типа элемента.
	if( aAnswer->vItems[aItemInd].vType != AnswerItemTypeInstruct )
		return clrcErrorInvArgs;

	// Получение данных.
	*aVarName = aAnswer->vItems[aItemInd].vValue.vInstruct.vVarName;
	*aVarNameLength = aAnswer->vItems[aItemInd].vValue.vInstruct.vVarNameLength;

	*aValue = aAnswer->vItems[aItemInd].vValue.vInstruct.vValue;
	*aValueLength = aAnswer->vItems[aItemInd].vValue.vInstruct.vValueLength;

	return clrcSuccess;
}

ClientLibReturnCode cl_answer_get_tag_open_window( const Answer * aAnswer, unsigned int aItemInd, const char ** aURL, unsigned int * aURLLength, unsigned int * aTarget )
{
	// Проверка аргументов.
	if( !aAnswer || !aURL || !aURLLength || !aTarget )
		return clrcErrorInvArgs;

	// Проверка количества элементов.
	if( aItemInd >= aAnswer->vItemsNumber )
		return clrcErrorInvArgs;

	// Проверка типа элемента.
	if( aAnswer->vItems[aItemInd].vType != AnswerItemTypeTagOpenWindow )
		return clrcErrorInvArgs;

	// Получение данных.
	*aURL = aAnswer->vItems[aItemInd].vValue.vOpenWindow.vURL;
	*aURLLength = aAnswer->vItems[aItemInd].vValue.vOpenWindow.vURLLength;

	*aTarget = aAnswer->vItems[aItemInd].vValue.vOpenWindow.vParentWindow;

	return clrcSuccess;
}

ClientLibReturnCode cl_answer_get_tag_rss( const Answer * aAnswer, unsigned int aItemInd, const char ** aURL, unsigned int * aURLLength,
										   const char ** aAlt, unsigned int * aAltLength, unsigned int * aOffset, unsigned int * aShowTitle,
										   unsigned int * aShowLink, unsigned int * aUpdatePeriod )
{
	// Проверка аргументов.
	if( !aAnswer || !aURL || !aURLLength || !aAlt || !aAltLength || !aOffset || !aShowTitle || !aShowLink || !aUpdatePeriod )
		return clrcErrorInvArgs;

	// Проверка количества элементов.
	if( aItemInd >= aAnswer->vItemsNumber )
		return clrcErrorInvArgs;

	// Проверка типа аргумента.
	if( aAnswer->vItems[aItemInd].vType != AnswerItemTypeTagRSS )
		return clrcErrorInvArgs;

	// Получение данных.
	*aURL = aAnswer->vItems[aItemInd].vValue.vRSS.vURL;
	*aURLLength = aAnswer->vItems[aItemInd].vValue.vRSS.vURLLength;

	*aAlt = aAnswer->vItems[aItemInd].vValue.vRSS.vAlt;
	*aAltLength = aAnswer->vItems[aItemInd].vValue.vRSS.vAltLength;

	*aOffset = aAnswer->vItems[aItemInd].vValue.vRSS.vOffset;

	*aShowTitle = aAnswer->vItems[aItemInd].vValue.vRSS.vShowTitle;

	*aShowLink = aAnswer->vItems[aItemInd].vValue.vRSS.vShowLink;

	*aUpdatePeriod = aAnswer->vItems[aItemInd].vValue.vRSS.vUpdatePeriod;

	return clrcSuccess;
}
