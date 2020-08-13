#include "ConditionsRegistry.hpp"
#include "lib/aptl/avector.h"

#include <NanoLib/LogSystem.hpp>

#include <cmath>

void InfConditionsRegistry::Create()
{
	vValues.Create();

	vIndexRW.clear();

	vConditionsRW.clear();
}

InfEngineErrors InfConditionsRegistry::Save( fstorage * aFStorage ) const
{
	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка состояния.
	if( vValues.GetState() == NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Получение указателя на секцию.
	fstorage_section* SectData = fstorage_get_section( aFStorage, FSTORAGE_SECTION_CONDITIONS_REGISTRY );
	if( !SectData )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Выделение памяти.
	unsigned int MemorySize = sizeof(uint32_t)*5 + vConditionsNumber*sizeof(uint32_t) + vConditionsSize;
	MemorySize += MemorySize%sizeof(uint32_t) ? sizeof(uint32_t)-MemorySize%sizeof(uint32_t) : 0;
	MemorySize += vValues.GetNeedMemorySize();
	MemorySize += MemorySize%sizeof(uint32_t) ? sizeof(uint32_t)-MemorySize%sizeof(uint32_t) : 0;

	int ret = fstorage_section_realloc( SectData, MemorySize );
	if( ret != FSTORAGE_OK )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	// Получение указателя на память.
	char* Buffer = static_cast<char*>(fstorage_section_get_all_data( SectData ));
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage pointer." );

	char* Ptr = Buffer;

	// Сохранение общего размера данных.
	*((uint32_t*)Ptr) = MemorySize;
	Ptr += sizeof(uint32_t);

	// Сохранение числа условий в реестре.
	*((uint32_t*)Ptr) = vConditionsNumber;
	Ptr += sizeof(uint32_t);

	// Сохранение сдвига к индексу значений.
	uint32_t* Shift = ((uint32_t*)Ptr);
	Ptr += sizeof(uint32_t);

	// Сохранение числа условий в реестре.
	*((uint32_t*)Ptr) = vConditionsSize;
	Ptr += sizeof(uint32_t);

	// Сохранение поискового индекса.
	memcpy( Ptr, vIndexRO, sizeof(uint32_t)*vConditionsNumber );
	Ptr += sizeof(uint32_t)*vConditionsNumber;

	memcpy( Ptr, vConditionsRO, vConditionsSize );
	Ptr += vConditionsSize;

	Ptr += (Ptr-Buffer)%sizeof(uint32_t) ? sizeof(uint32_t)-(Ptr-Buffer)%sizeof(uint32_t) : 0;

	// Установка сдвига к данным условий.
	*Shift = Ptr-Buffer;

	// Сохранение индекса значений.
	Ptr += vValues.Save( Ptr );

	Ptr += (Ptr-Buffer)%sizeof(uint32_t) ? sizeof(uint32_t)-(Ptr-Buffer)%sizeof(uint32_t) : 0;

	// Сохранение контрольного размера данных.
	*((uint32_t*)Ptr) = MemorySize;
	Ptr += sizeof(uint32_t);

	// Контрольная проверка сохранения.
	if( (unsigned int)(Ptr-Buffer) != MemorySize )
		ReturnWithError( INF_ENGINE_ERROR_FAULT, "Can't save InfConditionsRegistry" );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors InfConditionsRegistry::Open( fstorage * aFStorage )
{
	// Проверка аргументов.
	if( !aFStorage )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка состояния.
	if( vValues.GetState() != NanoLib::NameIndex::stClosed )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	// Получение секции с сохраненным реестром.
	fstorage_section* FStorageSection = fstorage_get_section( aFStorage, FSTORAGE_SECTION_CONDITIONS_REGISTRY );
	if( !FStorageSection )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	// Получение указателя на выделенную память.
	char* Buffer = static_cast<char*>(fstorage_section_get_all_data( FStorageSection ));
	if( !Buffer )
		ReturnWithError( INF_ENGINE_ERROR_FSTORAGE, "Can't get fstorage section." );

	char* Ptr = Buffer;

	// Проверка размера секции.
	unsigned int SectionSize = fstorage_section_get_size( FStorageSection );
	if( SectionSize < 3*sizeof(uint32_t) || SectionSize < *reinterpret_cast<uint32_t*>(Ptr) )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Общий размер сохранения.
	unsigned int Size = *reinterpret_cast<uint32_t*>(Ptr);
	Ptr += sizeof(uint32_t);

	// Проверка контрольного числа.
	if( Size < 3*sizeof(uint32_t) || *reinterpret_cast<uint32_t*>(Buffer+Size-sizeof(uint32_t)) != Size )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	vConditionsNumber = *reinterpret_cast<uint32_t*>(Ptr);
	Ptr += sizeof(uint32_t);

	uint32_t* Shift = reinterpret_cast<uint32_t*>(Ptr);
	Ptr += sizeof(uint32_t);

	vConditionsSize = *reinterpret_cast<uint32_t*>(Ptr);
	Ptr += sizeof(uint32_t);

	vIndexRO = reinterpret_cast<unsigned int*>(Ptr);
	Ptr += vConditionsNumber*sizeof(uint32_t);

	vConditionsRO = reinterpret_cast<Condition*>(Ptr);

	Ptr = Buffer + *Shift;

	if( Size < (unsigned int)(Ptr - Buffer) )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	NanoLib::NameIndex::ReturnCode nlnirc = vValues.Open( Ptr, Size - (Ptr - Buffer) );
	if( nlnirc != NanoLib::NameIndex::rcSuccess )
	{
		LogError( "Can't open InfConditionsRegistry: %s", vValues.ReturnCodeToString( nlnirc ) );

		Close();

		switch( nlnirc )
		{
			case NanoLib::NameIndex::rcErrorNoFreeMemory:
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
			default:
				return INF_ENGINE_ERROR_FAULT;
		}
	}

	return INF_ENGINE_SUCCESS;
}

void InfConditionsRegistry::Close()
{
	vValues.Destroy();

	vIndexRW.clear();
	vIndexRO = nullptr;

	vConditionsRW.clear();
	vConditionsRO = nullptr;

	vConditionsNumber = 0;
    vConditionsSize = 0;
}

InfEngineErrors InfConditionsRegistry::Registrate( InfBaseConditionType aConditionType, Vars::Id aVarId, const char * aVarValue, unsigned int & aConditionId )
{
	// Проверка состояния.
	if( vValues.GetState() != NanoLib::NameIndex::stReadWrite )
		ReturnWithError( INF_ENGINE_ERROR_STATE, INF_ENGINE_STRING_ERROR_STATE );

	unsigned int ValueId = 0;
	if( aConditionType == InfConditionEqual || aConditionType == InfConditionNotEqual )
	{
		// Определение идентификатора значения.
		if( !aVarValue )
			aVarValue = "";

		NanoLib::NameIndex::ReturnCode nlnirc = vValues.AddName( aVarValue, ValueId );
		if( nlnirc != NanoLib::NameIndex::rcSuccess && nlnirc != NanoLib::NameIndex::rcElementExists )
		{
			switch( nlnirc )
			{
				// Достигнуто максимальное число зарегистрированных имен.
				case NanoLib::NameIndex::rcErrorLimitIsReached:
					ReturnWithError( INF_ENGINE_ERROR_FAULT,
									 "Can't registrate condition: %s", vValues.ReturnCodeToString( nlnirc ) );
					// Ошибка выделения памяти.
				case NanoLib::NameIndex::rcErrorNoFreeMemory:
					ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
									 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
				default:
					ReturnWithError( INF_ENGINE_ERROR_FAULT, INF_ENGINE_STRING_ERROR_INTERNAL );
			}
		}
	}

	// Поиск в ранее сохраненных условиях.
	unsigned int Position;
	if( SearchInt( (Condition){ aConditionType, aVarId, ValueId }, Position ) )
	{
		aConditionId = vIndexRW[Position];

		return INF_ENGINE_SUCCESS;
	}

	// Добавление условия.
	vIndexRW.grow();
	if( vIndexRW.no_memory() )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	vIndexRO = vIndexRW.get_buffer();

	vConditionsRW.grow();
	if( vConditionsRW.no_memory() )
	{
		vIndexRW.pop_back();
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	}
	vConditionsRO = vConditionsRW.get_buffer();

	if( Position != vIndexRW.size()-1 )
		memmove( vIndexRW.get_buffer()+Position+1, vIndexRW.get_buffer()+Position,
				 sizeof(unsigned int)*(vIndexRW.size()-Position-1) );
	vIndexRW[Position] = vConditionsRW.size()-1;

	vConditionsRW.back().vType = aConditionType;
	vConditionsRW.back().vVarId = aVarId;
	vConditionsRW.back().vValueId = ValueId;

	aConditionId = vConditionsRW.size()-1;

	vConditionsNumber++;

	vConditionsSize = vConditionsRW.size_bytes();

	return INF_ENGINE_SUCCESS;
}

bool InfConditionsRegistry::Check( unsigned int aConditionId, const Vars::Vector & aVarsValues ) const
{
	// Проверка состояния.
	if( vValues.GetState() == NanoLib::NameIndex::stClosed )
		return false;

	if( aConditionId >= vConditionsNumber )
		return false;

	// Проверка условия.
	if( vConditionsRO[aConditionId].vType == InfConditionExist ||
			vConditionsRO[aConditionId].vType == InfConditionDontExist )
	{
		// Получение значения переменной.
		const Vars::Vector::VarValue val = aVarsValues.GetValue( vConditionsRO[aConditionId].vVarId );
		if( !val )
			return ( vConditionsRO[aConditionId].vType == InfConditionDontExist );
		const Vars::Vector::VarText Value = val.AsText();
		if( !Value)
			return ( vConditionsRO[aConditionId].vType == InfConditionDontExist );

		if( Value.GetProperties() > Vars::Options::Extended )
		{
			// Получение значения переменной that_anchor.
			unsigned int ThatAnchorLength;
			const char * ThatAnchor = aVarsValues.GetValue( InfEngineVarThatAnchor ).AsText().GetValue( ThatAnchorLength );

			// Поиск непустого варианта с выполненым условием.
			for( unsigned int i = 0; i < Value.GetNumberOfExtendedValues(); i++ )
			{
				// Получение варианта.
				unsigned int VarValueLength;
				const char * VarValue = Value.GetExtendedValue( i, VarValueLength );

				if( !VarValue || VarValueLength == 0 )
					continue;

				// Проверка условия варианта.
				unsigned int ConditionLength;
				const char * Condition = Value.GetExtendedCondition( i, ConditionLength );
				if( ( Condition && ConditionLength != 0 ) && ( !ThatAnchor || ThatAnchorLength == 0 ) )
					continue;
				else if( ( Condition && ConditionLength != 0 ) && ( ThatAnchor && ThatAnchorLength != 0 ) &&
						 ( ConditionLength != ThatAnchorLength || strncasecmp( Condition, ThatAnchor, ConditionLength ) ) )
					continue;

				return ( vConditionsRO[aConditionId].vType == InfConditionExist );
			}

			return ( vConditionsRO[aConditionId].vType == InfConditionDontExist );
		}
		else
		{
			// Получение варианта.
			unsigned int VarValueLength;
			const char * VarValue = Value.GetValue( VarValueLength );
			if( !VarValue || VarValueLength == 0 )
				return ( vConditionsRO[aConditionId].vType == InfConditionDontExist );
			else
				return ( vConditionsRO[aConditionId].vType == InfConditionExist );
		}
	}
	else if( vConditionsRO[aConditionId].vType == InfConditionEqual )
	{
		// Получение значения переменной.
		const Vars::Vector::VarText Value = aVarsValues.GetValue( vConditionsRO[aConditionId].vVarId ).AsText();
		if( Value.GetProperties() > Vars::Options::Extended )
			return false;

		// Получение варианта.
		unsigned int VarValueLength;
		const char * VarValue = Value.GetValue( VarValueLength );
		if( !VarValue || VarValueLength == 0 )
			return !strcmp( "", vValues.GetName( vConditionsRO[aConditionId].vValueId ) );
		else
			return !strcmp( VarValue, vValues.GetName( vConditionsRO[aConditionId].vValueId ) );
	}
	else if( vConditionsRO[aConditionId].vType == InfConditionNotEqual )
	{
		// Получение значения переменной.
		const Vars::Vector::VarValue val = aVarsValues.GetValue( vConditionsRO[aConditionId].vVarId );
		if( !val )
			return false;
		const Vars::Vector::VarText Value = val.AsText();
		if( !Value || Value.GetProperties() > Vars::Options::Extended )
			return false;

		// Получение варианта.
		unsigned int VarValueLength;
		const char * VarValue = Value.GetValue( VarValueLength );
		if( !VarValue || VarValueLength == 0 )
			return strcmp( "", vValues.GetName( vConditionsRO[aConditionId].vValueId ) );
		else
			return strcmp( VarValue, vValues.GetName( vConditionsRO[aConditionId].vValueId ) );
	}
	else if( vConditionsRO[aConditionId].vType == InfConditionAnyValue )
	{
		return true;
	}

	return false;
}

int InfConditionsRegistry::Compare( const Condition & aCondition, unsigned int aPosition ) const
{
	if( aCondition.vType < vConditionsRO[vIndexRO[aPosition]].vType )
		return -1;
	else if( aCondition.vType > vConditionsRO[vIndexRO[aPosition]].vType )
		return 1;
	else if( aCondition.vVarId < vConditionsRO[vIndexRO[aPosition]].vVarId )
		return -1;
	else if( aCondition.vVarId > vConditionsRO[vIndexRO[aPosition]].vVarId )
		return 1;

	if( aCondition.vType == InfConditionEqual || aCondition.vType == InfConditionNotEqual )
	{
		if( aCondition.vValueId < vConditionsRO[vIndexRO[aPosition]].vValueId )
			return -1;
		else if( aCondition.vValueId > vConditionsRO[vIndexRO[aPosition]].vValueId )
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

bool InfConditionsRegistry::SearchInt( Condition aCondition, unsigned int & aPosition ) const
{
	// Проверка на наличие хотя бы одного элемента в индексе.
	if( vConditionsNumber == 0 )
	{
		aPosition = 0;
		return false;
	}

	unsigned int first = 0;
	unsigned int last = vConditionsNumber;

	if( Compare( aCondition, first ) == 0 )
	{
		aPosition = first;

		return true;
	}
	else if( Compare( aCondition, last-1 ) == 0 )
	{
		aPosition = last-1;

		return true;
	}
	else if( Compare( aCondition, last-1 ) > 0 )
	{
		aPosition = last;

		return false;
	}
	else if( Compare( aCondition, first ) < 0 )
	{
		aPosition = first;

		return false;
	}
	else
	{
		while( last > first+1 )
		{
			unsigned int center = (unsigned int)floor((double)(first+last)/2);

			if( Compare( aCondition, center ) == 0 )
			{
				aPosition = center;

				return true;
			}
			else if( Compare( aCondition, center ) < 0 )
				last = center;
			else
				first = center;
		}

		aPosition = last;

		return false;
	}
}
