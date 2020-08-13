#include "InfData.h"

#include <stdlib.h>
#include <string.h>

#include "ClientLib.h"

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
*
*      Данные инфа и функции для работы с ними.
*
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

ClientLibReturnCode cl_inf_init( InfData * aInfData )
{
	// Проверка аргументов.
	if( !aInfData )
		return clrcErrorInvArgs;

	// Инициализция профиля инфа.
	aInfData->vInfId = 0;
	aInfData->vTemplates = NULL;
	aInfData->vVarsNumber = 0;
	aInfData->vVars = NULL;
	aInfData->vDictsNumber = 0;
	aInfData->vDicts = NULL;
	aInfData->vMemory = NULL;
	aInfData->vMemAvalible = 0;

	return clrcSuccess;
}

InfData * cl_inf_create()
{
	// Выделение памяти под профиль данных инфа.
	InfData * NewInfData = (InfData*)malloc( sizeof( InfData ) );
	if( !NewInfData )
		return NULL;

	// Инициализация профиля данных инфа.
	cl_inf_init( NewInfData );

	return NewInfData;
}

ClientLibReturnCode cl_inf_resize( InfData * aInfData, unsigned int aVarsNumber, unsigned int aDictsNumber )
{
	// Проверка аргументов.
	if( !aInfData )
		return clrcErrorInvArgs;

	// Выделение памяти.
	if( aInfData->vMemAvalible < aVarsNumber * sizeof( __Var ) + aDictsNumber * sizeof(  __Dict ) )
	{
		// Выделение памяти.
		if( aInfData->vMemory )
			aInfData->vMemory = realloc( aInfData->vMemory, aVarsNumber * sizeof( __Var ) + aDictsNumber * sizeof( __Dict ) );
		else
			aInfData->vMemory = malloc( aVarsNumber * sizeof( __Var ) + aDictsNumber * sizeof( __Dict ) );

		if( !aInfData->vMemory )
		{
			cl_inf_init( aInfData );

			return clrcErrorNoFreeMemory;
		}

		aInfData->vMemAvalible = aVarsNumber * sizeof( __Var ) + aDictsNumber * sizeof( __Dict );
	}

	// Очистка буфера.
	bzero( aInfData->vMemory, aVarsNumber * sizeof( __Var ) + aDictsNumber * sizeof( __Dict ) );
	aInfData->vVars = aInfData->vMemory;
	aInfData->vDicts = aInfData->vMemory + aVarsNumber * sizeof( __Var );

	aInfData->vVarsNumber = aVarsNumber;
	aInfData->vDictsNumber = aDictsNumber;

	return clrcSuccess;
}

void cl_inf_set_var( InfData * aInfData, unsigned int aVarInd, const char * aVarName, const char * aVarValue )
{
	// Проверка аргументов.
	if( !aInfData || !aVarName )
		return;

	if( aVarInd >= aInfData->vVarsNumber )
		return;

	// Установка данных.
	aInfData->vVars[aVarInd].vName = aVarName;
	aInfData->vVars[aVarInd].vValue = aVarValue;
}

void cl_inf_set_dict( InfData * aInfData, unsigned int aDictInd, const char * aDictName, const char * aDict )
{
	// Проверка аргументов.
	if( !aInfData || !aDictName )
		return;

	if( aDictInd >= aInfData->vDictsNumber )
		return;

	// Установка данных.
	aInfData->vDicts[aDictInd].vName = aDictName;
	aInfData->vDicts[aDictInd].vData = aDict;
}

void cl_inf_set_id( InfData * aInfData, unsigned int aInfId )
{
	if( aInfData )
		aInfData->vInfId = aInfId;
}

void cl_inf_set_templates( InfData * aInfData, const char * aTemplates )
{
	if( aInfData )
		aInfData->vTemplates = aTemplates;
}

unsigned int cl_inf_get_vars_cnt( const InfData * aInfData )
{
	if( !aInfData )
		return 0;
	else
		return aInfData->vVarsNumber;
}

unsigned int cl_inf_get_dicts_cnt( const InfData * aInfData )
{
	if( !aInfData )
		return 0;
	else
		return aInfData->vDictsNumber;
}

ClientLibReturnCode cl_inf_get_var( const InfData * aInfData, unsigned int aVarInd, const char ** aVarName, const char ** aVarValue )
{
	// Проверка аргументов.
	if( !aInfData || !aVarName || !aVarValue )
		return clrcErrorInvArgs;

	if( aVarInd >= aInfData->vVarsNumber )
		return clrcErrorInvArgs;

	// Получение данных.
	*aVarName = aInfData->vVars[aVarInd].vName;
	*aVarValue = aInfData->vVars[aVarInd].vValue;

	return clrcSuccess;
}

ClientLibReturnCode cl_inf_get_dict( const InfData * aInfData, unsigned int aDictInd, const char ** aDictName, const char ** aDictText )
{
	// Проверка аргументов.
	if( !aInfData || !aDictName || !aDictText )
		return clrcErrorInvArgs;

	if( aDictInd >= aInfData->vDictsNumber )
		return clrcErrorInvArgs;

	// Получение данных.
	*aDictName = aInfData->vDicts[aDictInd].vName;
	*aDictText = aInfData->vDicts[aDictInd].vData;

	return clrcSuccess;
}

void cl_inf_free( InfData * aInfData )
{
	// Проверка аргументов.
	if( !aInfData )
		return;

	// Освобождение памяти, занимаемой переменными профиля данных инфа.
	if( aInfData->vMemAvalible )
		free( aInfData->vMemory );

	// Освобождение памяти, занимаемой структурой профиля данных инфа.
	free( aInfData );
}
