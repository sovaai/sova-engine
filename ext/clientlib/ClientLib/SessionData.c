#include "SessionData.h"

#include <stdlib.h>
#include <string.h>

#include "ClientLib.h"

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
*
*      Данные сессии и функции для работы с ней.
*
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

ClientLibReturnCode cl_session_init( SessionData * aSessionData )
{
	// Проверка аргументов.
	if( !aSessionData )
		return clrcErrorInvArgs;

	// Инициализация структуры.
	aSessionData->vInfId = 0;
	aSessionData->vSessionId = -1;
	aSessionData->vVarsNumber = 0;
	aSessionData->vVarsAvalible = 0;
	aSessionData->vVars = NULL;

	return clrcSuccess;
}

SessionData * cl_session_create()
{
	// Выделение памяти под данные сессии.
	SessionData * NewSessionData = (SessionData*)malloc( sizeof( SessionData ) );
	if( !NewSessionData )
		return NULL;

	// Инициализация данных сессии.
	cl_session_init( NewSessionData );

	return NewSessionData;
}

ClientLibReturnCode cl_session_resize( SessionData * aSessionData, unsigned int aVarsNumber )
{
	// Проверка аргументов.
	if( !aSessionData )
		return clrcErrorInvArgs;

	// Выделение памяти.
	if( aSessionData->vVarsAvalible < aVarsNumber )
	{
		if( aSessionData->vVars )
			aSessionData->vVars = realloc( aSessionData->vVars, aVarsNumber * sizeof( __Var ) );
		else
			aSessionData->vVars = malloc( aVarsNumber * sizeof( __Var ) );

		if( !aSessionData->vVars )
		{
			cl_session_init( aSessionData );

			return clrcErrorNoFreeMemory;
		}

		aSessionData->vVarsAvalible = aVarsNumber;
	}

	// Очистка переменных.
	if( aVarsNumber )
		bzero( aSessionData->vVars, sizeof( __Var ) * aVarsNumber );

	aSessionData->vVarsNumber = aVarsNumber;

	return clrcSuccess;
}

void cl_session_set_var( SessionData * aSessionData, unsigned int aVarInd, const char * aVarName, const char * aVarValue )
{
	// Проверка аргументов.
	if( !aSessionData || !aVarName )
		return;

	if( aVarInd >= aSessionData->vVarsNumber )
		return;

	aSessionData->vVars[aVarInd].vName = aVarName;
	aSessionData->vVars[aVarInd].vValue = aVarValue;
}

void cl_session_set_id( SessionData * aSessionData, unsigned int aSessionId )
{
	if( aSessionData )
		aSessionData->vSessionId = aSessionId;
}

void cl_session_set_inf_id( SessionData * aSessionData, unsigned int aInfId )
{
	if( aSessionData )
		aSessionData->vInfId = aInfId;
}

unsigned int cl_session_get_size( const SessionData * aSessionData )
{
	if( !aSessionData )
		return 0;
	else
		return aSessionData->vVarsNumber;
}

ClientLibReturnCode cl_session_get( const SessionData * aSessionData, unsigned int aVarInd, const char ** aVarName, const char ** aVarValue )
{
	// Проверка аргументов.
	if( !aSessionData || !aVarName || !aVarValue )
		return clrcErrorInvArgs;

	if( aVarInd >= aSessionData->vVarsNumber )
		return clrcErrorInvArgs;

	// Присовение значений.
	*aVarName = aSessionData->vVars[aVarInd].vName;
	*aVarValue = aSessionData->vVars[aVarInd].vValue;

	return clrcSuccess;
}

void cl_session_free( SessionData * aSessionData )
{
	// Проверка аргументов.
	if( !aSessionData )
		return;

	// Освобождение памяти, занятой переменными.
	if( aSessionData->vVarsAvalible )
		free( aSessionData->vVars );

	// Освобождение памяти, занятой объектом сессии.
	free( aSessionData );
}
