#include "Vector.hpp"

#include <cmath>

#include <NanoLib/TextFuncs.hpp>
#include <NanoLib/LogSystem.hpp>

using Vars::Vector;

bool Vars::Vector::SwitchRegistry( const Vars::Registry & aVarsRegistry ) noexcept
{
	// Сравнение основных контрольных сумм реестров.
	unsigned char newmd5[16];
	aVarsRegistry.GetMainCheckSum( newmd5 );
	unsigned char oldmd5[16];
	vVarsRegistry->GetMainCheckSum( oldmd5 );
	if( !std::memcmp( newmd5, oldmd5, sizeof( newmd5 ) ) )
	{
		vVarsRegistry = &aVarsRegistry;
		return true;
	}
	else
	{
		for( unsigned int i = 0; i < 16; i++ )
			if( oldmd5[i] )
				return false;
		vVarsRegistry = &aVarsRegistry;
		return true;
	}
}

aTextString Vector::VarStruct::vTmpString;


const char* Vector::VarText::GetValue( unsigned int & aValueLength ) const
{
	if( !vBuffer )
	{
		aValueLength = 0;
		return nullptr;
	}

	if( GetProperties() > Options::Extended )
	{
		aValueLength = 0;
		return nullptr;
	}
	else
	{
		aValueLength = reinterpret_cast<const uint32_t*>( vBuffer )[3];
		if( aValueLength != 0 )
			return reinterpret_cast<const char*>( vBuffer ) + 4 * sizeof( uint32_t );
		else
			return nullptr;
	}
}

unsigned int Vector::VarText::GetNumberOfExtendedValues() const
{
	if( !vBuffer )
		return 0;

	if( GetProperties() > Options::Extended )
		return reinterpret_cast<const uint32_t*>( vBuffer )[3];
	else
		return 0;
}

const char* Vector::VarText::GetExtendedValue( unsigned int aValNum, unsigned int & aValueLength ) const
{
	if( !vBuffer )
	{
		aValueLength = 0;
		return nullptr;
	}

	if( GetProperties() > Options::Extended )
	{
		if( aValNum >= GetNumberOfExtendedValues() )
		{
			aValueLength = 0;
			return nullptr;
		}
		else
		{
			const char * Value = static_cast<const char*>( vBuffer ) + reinterpret_cast<const uint32_t*>( vBuffer )[4 + aValNum];

			aValueLength = reinterpret_cast<const uint32_t*>( Value )[3];
			if( aValueLength != 0 )
				return Value + reinterpret_cast<const uint32_t*>( Value )[2];
			else
				return nullptr;
		}
	}
	else
	{
		aValueLength = 0;
		return nullptr;
	}
}

const char* Vector::VarText::GetExtendedCondition( unsigned int aValNum, unsigned int & aValueLength ) const
{
	if( !vBuffer )
	{
		aValueLength = 0;
		return nullptr;
	}

	if( GetProperties() > Options::Extended )
	{
		if( aValNum >= GetNumberOfExtendedValues() )
		{
			aValueLength = 0;
			return nullptr;
		}
		else
		{
			const char * Value = static_cast<const char*>( vBuffer ) + reinterpret_cast<const uint32_t*>( vBuffer )[4 + aValNum];

			aValueLength = reinterpret_cast<const uint32_t*>( Value )[1];
			if( aValueLength )
				return Value + reinterpret_cast<const uint32_t*>( Value )[0];
			else
				return nullptr;
		}
	}
	else
	{
		aValueLength = 0;
		return nullptr;
	}
}

const char* Vector::VarText::GetExtendedInstruct( unsigned int aValNum, unsigned int & aValueLength ) const
{
	if( !vBuffer )
	{
		aValueLength = 0;
		return nullptr;
	}

	if( GetProperties() > Options::Extended )
	{
		if( aValNum >= GetNumberOfExtendedValues() )
		{
			aValueLength = 0;
			return nullptr;
		}
		else
		{
			const char * Value = static_cast<const char*>( vBuffer ) + reinterpret_cast<const uint32_t*>( vBuffer )[4 + aValNum];

			aValueLength = reinterpret_cast<const uint32_t*>( Value )[5];
			if( aValueLength != 0 )
				return Value + reinterpret_cast<const uint32_t*>( Value )[4];
			else
				return nullptr;
		}
	}
	else
	{
		aValueLength = 0;
		return nullptr;
	}
}

const char* Vector::VarText::GetExtendedEmotion( unsigned int aValNum, unsigned int & aValueLength ) const
{
	if( !vBuffer )
	{
		aValueLength = 0;
		return nullptr;
	}

	if( GetProperties() > Options::Extended )
	{
		if( aValNum >= GetNumberOfExtendedValues() )
		{
			aValueLength = 0;
			return nullptr;
		}
		else
		{
			const char * Value = static_cast<const char*>( vBuffer ) + reinterpret_cast<const uint32_t*>( vBuffer )[4 + aValNum];

			aValueLength = reinterpret_cast<const uint32_t*>( Value )[7];
			if( aValueLength != 0 )
				return Value + reinterpret_cast<const uint32_t*>( Value )[6];
			else
				return nullptr;
		}
	}
	else
	{
		aValueLength = 0;
		return nullptr;
	}
}

InfEngineErrors Vector::SetTextValue( const char * aVarName, unsigned int aVarNameLength, const char * aVarValue, unsigned int aVarValueLength )
{
	// Поиск идентификатора переменной.
	Id var_id = vVarsRegistry->Search( aVarName, aVarNameLength );
	if( !var_id )
		return INF_ENGINE_WARN_UNSUCCESS;

	// Установка значения.
	return SetTextValue( var_id, aVarValue, aVarValueLength );
}

InfEngineErrors Vector::SetTextValue( Id aVarId, const char * aVarValue, unsigned int aVarValueLength )
{
	if( aVarId.is_tmp() )
		return SetTmpValue( aVarId, aVarValue, aVarValueLength );

	// Сброс ранее подсчитаного размера контейнера.
	vNeedMemorySize = static_cast<unsigned int>( -1 );

	// Проверка аргументов.
	if( !aVarValue && aVarValueLength )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Установка временной переменной.
	if( aVarId.is_tmp() )
		ReturnWithTraceExt( SetTmpValue( aVarId, aVarValue, aVarValueLength ), INF_ENGINE_SUCCESS );

	// Проверка идентификатора переменной.
	if( aVarId.get() >= vVarsRegistry->GetVariablesNumber() )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка типа переменной.
	switch( vVarsRegistry->GetVarType( aVarId ) )
	{
	case Type::Text:
		// Do nothing.
		break;

	case Type::Struct:
		{
			// Подмена идентификатора структуры на иденитификатор её поля "default".
			aVarId = vVarsRegistry->GetStructDefaultId( aVarId );
			break;
		}

	default:
		ReturnWithError( INF_ENGINE_ERROR_INTERNAL, "Unsupported variable type: %i", vVarsRegistry->GetVarType( aVarId ) );

	}

	// Разбор значения переменной.
	const VarValue * NewValue;
	InfEngineErrors iee = ParseValue( aVarId, aVarValue, aVarValueLength, NewValue );
	if( iee != INF_ENGINE_SUCCESS )
		ReturnWithTrace( iee );

	// Поиск места переменной в массиве указателей.
	unsigned int IndPos;
	bool success = SearchInd( aVarId, IndPos );
	if( success )
	{}
	else
	{
		// Подготовка места в массиве указателей для указателя на новое значение.
		vValues.grow();
		if( vValues.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
							 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		if( IndPos != vValues.size() - 1 )
			memmove( vValues.get_buffer() + IndPos + 1, vValues.get_buffer() + IndPos,
					 ( vValues.size() - IndPos - 1 ) * sizeof( VarText* ) );

	}

	// Добавление указателя на новое значение.
	vValues[IndPos] = NewValue;

	return INF_ENGINE_SUCCESS;
}

Vector::VarValue* Vector::SaveTextValue( nMemoryAllocator & aAllocator, Id aVarId, const char * aVarValue, unsigned int aVarValueLength, Options aVarProperties ) const
{
	// Общий объем памяти, нужный для сохранения значения переменной.
	unsigned int MemorySize = 4 * sizeof( uint32_t ) + aVarValueLength + 1;
	// Выравнивание памяти.
	MemorySize += MemorySize % sizeof( uint32_t ) ? sizeof( uint32_t ) - MemorySize % sizeof( uint32_t ) : 0;

	// Выделение памяти.
	char * buffer = static_cast<char*>( aAllocator.Allocate( MemorySize ) );
	if( !buffer )
		return nullptr;

	// Сохранение полного размера разобранного значения.
	reinterpret_cast<uint32_t*>( buffer )[0] = MemorySize;

	// Сохранение идентификатора переменной.
	reinterpret_cast<uint32_t*>( buffer )[1] = aVarId.get();

	// Сохранение свойств переменной.
	reinterpret_cast<uint32_t*>( buffer )[2] = 0;
	reinterpret_cast<char*>( buffer )[2 * sizeof( uint32_t )] = static_cast<unsigned int>( aVarProperties );

	// Сохранение длины значения.
	reinterpret_cast<uint32_t*>( buffer )[3] = aVarValueLength;

	// Сохранение значения.
	if( aVarValueLength )
		memcpy( buffer + 4 * sizeof( uint32_t ), aVarValue, aVarValueLength );
	buffer[4 * sizeof( uint32_t ) + aVarValueLength] = '\0';

	// Выделение памяти.
	VarValue * Value = static_cast<VarValue*>( aAllocator.Allocate( sizeof( VarValue ) ) );
	if( !Value )
		return nullptr;
	new ( (nMemoryAllocatorElementPtr*)Value )VarValue( buffer, this );

	return Value;
}

InfEngineErrors Vector::SetTmpValue( Id aVarId, const char * aValue, unsigned int aValueLength ) const
{
	// Проверка аргументов.
	if( !aValue && aValueLength )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	if( vDefaultValues )
		ReturnWithTraceExt( vDefaultValues->SetTmpValue( aVarId, aValue, aValueLength ), INF_ENGINE_SUCCESS );

	// Выделение памяти.
	if( vTmpValues.size() <= aVarId.get() )
	{
		unsigned int size = vTmpValues.size();
		vTmpValues.resize( aVarId.get() + 1 );
		if( vTmpValues.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		bzero( vTmpValues.get_buffer() + size, sizeof( VarValue* ) * ( vTmpValues.size() - size ) );
	}

	vTmpValues[aVarId.get()] = SaveTextValue( vTmpAllocator, aVarId, aValue, aValueLength, Options::None );
	if( !vTmpValues[aVarId.get()] )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

const Vector::VarValue Vector::GetValue( const char * aVarName, unsigned int aVarNameLength ) const
{
	// Проверка аргументов.
	if( !aVarName || aVarNameLength == 0 )
		ReturnWithError( vFakeValue, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Поиск идентификатора переменной.
	Id var_id = vVarsRegistry->Search( aVarName, aVarNameLength );
	if( !var_id )
		return vFakeValue;

	return GetValue( var_id );
}

const Vector::VarValue Vector::GetValue( Id aVarId ) const
{
	// Получение значения временной переменной.
	if( aVarId.is_tmp() )
		return GetTmpValue( aVarId );

	// Возвращение переменной типа структура.
	if( Type::Struct == vVarsRegistry->GetVarType( aVarId ) )
		return VarValue( aVarId, nullptr, this );

	// Поиск значения.
	unsigned int Ind;
	const Vector * CurrentVector = this;
	unsigned int Counter = 0;
	while( !CurrentVector->SearchInd( aVarId, Ind ) )
	{
		if( CurrentVector->vDefaultValues )
		{
			// Если найти значение не удалось, то переходим к значениям по умолчанию.
			CurrentVector = CurrentVector->vDefaultValues;

			if( ++Counter >= 10 )
				// Ограничение рекурсии.
				return vFakeValue;
		}
		else
			return vFakeValue;
	}

	// Значение найдено.
	return *( CurrentVector->vValues[Ind] );
}

const Vector::VarValue Vector::GetStructValueAsText( Id aVarId ) const
{
	// Получение значения временной переменной.
	if( aVarId.is_tmp() )
		ReturnWithTraceExt( GetTmpValue( aVarId ), INF_ENGINE_SUCCESS );

	// Проверка типа переменной.
	if( Type::Struct != vVarsRegistry->GetVarType( aVarId ) )
		return vFakeValue;

	// Поиск значения.
	unsigned int Ind;
	const Vector * CurrentVector = this;
	unsigned int Counter = 0;

	while( !CurrentVector->SearchInd( aVarId, Ind ) )
	{
		if( CurrentVector->vDefaultValues )
		{
			// Если найти значение не удалось, то переходим к значениям по умолчанию.
			CurrentVector = CurrentVector->vDefaultValues;

			if( ++Counter >= 10 )
			{
				// Ограничение рекурсии.
				return vFakeValue;
			}
		}
		else
		{
			return vFakeValue;
		}
	}

	// Значение найдено.
	return *( CurrentVector->vValues[Ind] );
}

const Vector::VarValue Vector::GetTmpValue( Id aVarId ) const
{
	if( vDefaultValues )
		return vDefaultValues->GetTmpValue( aVarId );

	if( aVarId.get() >= vTmpValues.size() || !vTmpValues[aVarId.get()] )
		return vFakeValue;
	else
		return *vTmpValues[aVarId.get()];
}

unsigned int Vector::Save( void * aBuffer ) const
{
	// Сохранение общего размера сохраненного вектора.
	static_cast<uint32_t*>( aBuffer )[0] = GetNeedMemorySize();

	// Сохранение числа значений в векторе.
	static_cast<uint32_t*>( aBuffer )[1] = vValues.size();

	// Сдвиги к местам сохранения значений переменных.
	uint32_t * Shifts = static_cast<uint32_t*>( aBuffer ) + 2;
	// Место для сохранения значений.
	char * buffer = reinterpret_cast<char*>( Shifts ) + vValues.size() * sizeof( uint32_t );

	// Сохранение значений переменных.
	for( unsigned int i = 0; i < vValues.size(); i++ )
	{
		Shifts[i] = buffer - static_cast<char*>( aBuffer );
		vValues[i]->Save( buffer );
		buffer += vValues[i]->GetNeedMemorySize();
	}

	reinterpret_cast<uint32_t*>( buffer )[0] = GetNeedMemorySize();
	buffer += sizeof( uint32_t );

	return buffer - static_cast<char*>( aBuffer );
}

InfEngineErrors Vector::Load( const char * aBuffer, unsigned int aBufferSize, unsigned int & aResultSize )
{
	// Проверка аргументов.
	if( !aBuffer )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка размера памяти.
	if( aBufferSize < sizeof( uint32_t ) )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Очистка вектора.
	Reset();

	aResultSize = *reinterpret_cast<const uint32_t*>( aBuffer );

	// Проверка размера памяти.
	if( aBufferSize < aResultSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Проверка контрольного числа.
	if( *reinterpret_cast<const uint32_t*>( aBuffer + ( aResultSize - sizeof( uint32_t ) ) ) != aResultSize )
		ReturnWithError( INF_ENGINE_ERROR_INV_DATA, INF_ENGINE_STRING_ERROR_INVALID_DATA );

	// Получение числа элементов в векторе.
	unsigned int NumberOfValues = reinterpret_cast<const uint32_t*>( aBuffer )[1];

	if( NumberOfValues != 0 )
	{
		vValues.resize( NumberOfValues );
		if( vValues.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		const uint32_t * Shifts = reinterpret_cast<const uint32_t*>( aBuffer ) + 2;

		// Выделение памяти.
		void * Memory = vAllocator.Allocate( NumberOfValues * sizeof( VarValue ) +
											 aResultSize - ( NumberOfValues + 3 ) * sizeof( uint32_t ) );
		if( !Memory )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Копирование данных.
		char * Data = static_cast<char*>( Memory );
		memcpy( Data, aBuffer + ( NumberOfValues + 2 ) * sizeof( uint32_t ),
				aResultSize - ( NumberOfValues + 3 ) * sizeof( uint32_t ) );

		// Создание индекса.
		VarValue * Values = reinterpret_cast<VarValue*>( Data + aResultSize - ( NumberOfValues + 3 ) * sizeof( uint32_t ) );
		for( unsigned int i = 0; i < NumberOfValues; i++ )
		{
			new ( (nMemoryAllocatorElementPtr*)( Values + i ) )VarValue( Data + Shifts[i] -
																		 ( NumberOfValues + 2 ) * sizeof( uint32_t ), this );

			vValues[i] = Values + i;
		}
	}
	vNeedMemorySize = aResultSize;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors Vector::SetDefaultValues( const Vector & aDefaultValues )
{
	/**
	 *  Получение MD5 дайджестов реестров переменных для текущего вектора значений и предлагаемого в качестве вектора
	 * значений по умолчанию.
	 */
	unsigned char MD5Digest[16];
	vVarsRegistry->GetMainCheckSum( MD5Digest );

	unsigned char CandidatMD5Digest[16];
	aDefaultValues.vVarsRegistry->GetMainCheckSum( CandidatMD5Digest );

	if( !memcmp( MD5Digest, CandidatMD5Digest, 16 ) )
	{
		vDefaultValues = &aDefaultValues;
		return INF_ENGINE_SUCCESS;
	}
	else
		return INF_ENGINE_ERROR_CHECKSUMM;
}

void Vector::Reset()
{
	vAllocator.Reset();

	vValues.clear();

	ResetTmp();

	vNeedMemorySize = static_cast<unsigned int>( -1 );
}

bool Vector::SearchInd( Id aVarId, unsigned int & aIndexPos ) const
{
	// Пустой вектор значений.
	if( vValues.empty() )
	{
		aIndexPos = 0;
		return false;
	}

	// Бинарный поиск по идентификатору переменной.
	unsigned int LeftBorder = 0;
	unsigned int RightBorder = vValues.size() - 1;

	if( vValues[LeftBorder]->GetId() > aVarId )
	{
		aIndexPos = LeftBorder;

		return false;
	}
	else if( vValues[RightBorder]->GetId() < aVarId )
	{
		aIndexPos = RightBorder + 1;

		return false;
	}

	do
	{
		unsigned int Center = (unsigned int)std::floor( (double)( LeftBorder + RightBorder ) / 2 );

		if( vValues[Center]->GetId() == aVarId )
		{
			aIndexPos = Center;

			return true;
		}
		else if( vValues[Center]->GetId() < aVarId )
			LeftBorder = Center;
		else
			RightBorder = Center;
	}
	while( RightBorder - LeftBorder > 1 );

	if( vValues[RightBorder]->GetId() == aVarId )
	{
		aIndexPos = RightBorder;

		return true;
	}
	else if( vValues[LeftBorder]->GetId() == aVarId )
	{
		aIndexPos = LeftBorder;

		return true;
	}

	aIndexPos = RightBorder;

	return false;
}

InfEngineErrors Vector::ParseValue( Id aVarId, const char * aVarValue, unsigned int aVarValueLength, const VarValue* & aParsedValue )
{
	// Проверка аргументов.
	if( !aVarValue )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, INF_ENGINE_STRING_ERROR_INVALID_ARGUMENTS );

	// Проверка идентификатора переменной.
	if( aVarId.get() >= vVarsRegistry->GetVariablesNumber() )
		ReturnWithError( INF_ENGINE_ERROR_INV_ARGS, "aVarId(%i) >= vVarsRegistry.GetVariablesNumber(%i)", aVarId.get(), vVarsRegistry->GetVariablesNumber() );

	// Получение свойств переменной.
	auto VarProperties = vVarsRegistry->GetProperties( aVarId );

	// Исходя из свойств переменной производится разбор ее значения.
	if( !( VarProperties > Options::Extended ) )
	{
		// Разбор простого значения.
		if( ( aParsedValue = SaveTextValue( vAllocator, aVarId, aVarValue, aVarValueLength, VarProperties ) ) == nullptr )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		return INF_ENGINE_SUCCESS;
	}
	else
	{
		// Разбор множественного значения.
		vTmpBuffer.clear();

		// Один вариант значения есть всегда.
		vTmpBuffer.push_back( 0 );
		if( vTmpBuffer.no_memory() )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Поиск остальных вариантов и подсчет их количества.
		const char * CurrentPtr = aVarValue;
		while( ( CurrentPtr = static_cast<const char*>( memchr( CurrentPtr, '\n',
																aVarValueLength - ( CurrentPtr - aVarValue ) ) ) ) )
		{
			// Сохранение позиции начала новой строки.
			vTmpBuffer.push_back( ( CurrentPtr - aVarValue ) + 1 );
			if( vTmpBuffer.no_memory() )
				ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY,
								 INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

			// Переход к поиску следующей строки.
			CurrentPtr++;
		}

		/**
		 *  Выделение памяти для сохранения значения переменной. Производится выделение чуть большего объема памяти,
		 * чем необходимо, но при этом можно спокойно использовать указатели, что удобно.
		 *  4*sizeof( uint32_t ) - размер, идентификатор переменной, свойства переменной, количество вариантов значений.
		 *  vTmpBuffer.size()*sizeof(uint32_t) - сдвиги к значениям.
		 *  8*vTmpBuffer.size()*sizeof(uint32_t) - управляющие части значений.
		 *  aVarValueLength+1 - память для сохранения условий, инструкций и значений.
		 *  vTmpBuffer.size()*(sizeof(uint32_t)-1) - запас на выравнивание данных по 32 бита.
		 */
		void * Memory = vAllocator.Allocate( ( 4 + 9 * vTmpBuffer.size() ) * sizeof( uint32_t ) + aVarValueLength + 1 +
											 vTmpBuffer.size() * ( sizeof( uint32_t ) - 1 ) );
		if( !Memory )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

		// Указатель на место для размера сохраненного значения.
		uint32_t * Size = static_cast<uint32_t*>( Memory );

		// Идентификатор переменной.
		static_cast<uint32_t*>( Memory )[1] = aVarId.get();

		// Свойства переменной.
		static_cast<uint32_t*>( Memory )[2] = 0;
		reinterpret_cast<char*>( Memory )[2 * sizeof( uint32_t )] = static_cast<unsigned int>( VarProperties );

		// Количество вариантов значения.
		static_cast<uint32_t*>( Memory )[3] = vTmpBuffer.size();

		// Сдвиги к вариантам значений.
		uint32_t * Shifts = static_cast<uint32_t*>( Memory ) + 4;

		// Память для сохранения значений.
		Memory = Shifts + vTmpBuffer.size();

		// Разбор значений.
		for( unsigned int i = 0; i < vTmpBuffer.size(); i++ )
		{
			// Получение варианта.
			const char * SubValue = aVarValue + vTmpBuffer[i];
			unsigned int SubValueLength = ( i == vTmpBuffer.size() - 1 ) ?
										  aVarValueLength - vTmpBuffer[i] :
										  vTmpBuffer[i + 1] - vTmpBuffer[i] - 1;

			// Указатель на начало памяти, где будет сохранен вариант значения.
			char * ElementBase = static_cast<char*>( Memory );

			// Сдвиг к текущему варианту значения.
			Shifts[i] = ElementBase - reinterpret_cast<char*>( Size );

			// Управляющие элементы значения.
			uint32_t * ConditionShift = reinterpret_cast<uint32_t*>( ElementBase );
			uint32_t * ConditionLength = reinterpret_cast<uint32_t*>( ElementBase ) + 1;
			uint32_t * ValueShift = reinterpret_cast<uint32_t*>( ElementBase ) + 2;
			uint32_t * ValueLength = reinterpret_cast<uint32_t*>( ElementBase ) + 3;
			uint32_t * InstructShift = reinterpret_cast<uint32_t*>( ElementBase ) + 4;
			uint32_t * InstructLength = reinterpret_cast<uint32_t*>( ElementBase ) + 5;
			uint32_t * EmotionShift = reinterpret_cast<uint32_t*>( ElementBase ) + 6;
			uint32_t * EmotionLength = reinterpret_cast<uint32_t*>( ElementBase ) + 7;
			Memory = ConditionShift + 8;

			unsigned int StartPos = 0;

			// Выделение условия на якорь.
			if( StartPos < SubValueLength && SubValue[StartPos] == '[' )
			{
				StartPos++;

				// Пропускаем все пробельные символы.
				for(; StartPos < SubValueLength && SubValue[StartPos] == ' '; StartPos++ ) ;

				// Выделение якоря.
				unsigned int AnchorStartPos = StartPos;
				for(; StartPos < SubValueLength &&
					( TextFuncs::IsAlNum( SubValue[StartPos] ) || SubValue[StartPos] == '_' ); StartPos++ ) ;
				unsigned int AnchorEndPos = StartPos;

				// Пропускаем все пробельные символы.
				for(; StartPos < SubValueLength && SubValue[StartPos] == ' '; StartPos++ ) ;

				// Проверка корректности описания условия на якорь.
				if( StartPos >= SubValueLength || SubValue[StartPos] != ']' )
				{
					// Некорректное описание условия на якорь.
					*ConditionShift = 0;
					*ConditionLength = 0;

					StartPos = 0;
				}
				else if( AnchorEndPos == AnchorStartPos )
				{
					// Пустое условия на якорь.
					*ConditionShift = 0;
					*ConditionLength = 0;

					StartPos++;
				}
				else
				{
					*ConditionShift = static_cast<char*>( Memory ) - ElementBase;
					memcpy( Memory, SubValue + AnchorStartPos, AnchorEndPos - AnchorStartPos );
					static_cast<char*>( Memory )[AnchorEndPos - AnchorStartPos] = '\0';
					Memory = static_cast<char*>( Memory ) + ( AnchorEndPos - AnchorStartPos ) + 1;

					*ConditionLength = AnchorEndPos - AnchorStartPos;

					StartPos++;
				}
			}
			else
			{
				*ConditionShift = 0;
				*ConditionLength = 0;
			}

			unsigned int EndPos = SubValueLength;

			// Выделение инструкции на якорь.
			if( EndPos > StartPos && SubValue[EndPos - 1] == ']' )
			{
				EndPos--;
				// Пропускаем все пробельные символы.
				for(; EndPos > StartPos && SubValue[EndPos - 1] == ' '; EndPos-- ) ;

				// Выделение якоря.
				unsigned int AnchorEndPos = EndPos;
				for(; EndPos > StartPos &&
					( TextFuncs::IsAlNum( SubValue[EndPos - 1] ) || SubValue[EndPos - 1] == '_' ); EndPos-- ) ;
				unsigned int AnchorStartPos = EndPos;

				// Пропускаем все пробельные символы.
				for(; EndPos > StartPos && SubValue[EndPos - 1] == ' '; EndPos-- ) ;

				// Проверка корректности описания условия на якорь.
				if( EndPos <= StartPos || SubValue[EndPos - 1] != '[' )
				{
					// Некорректное описание инструкции присвоения якоря.
					*InstructShift = 0;
					*InstructLength = 0;
					*EmotionShift = 0;
					*EmotionLength = 0;

					EndPos = SubValueLength;
				}
				else
				{
					if( AnchorEndPos == AnchorStartPos )
					{
						// Пустая инструкция на присвоение якоря.
						*InstructShift = 0;
						*InstructLength = 0;
					}
					else
					{
						*InstructShift = static_cast<char*>( Memory ) - ElementBase;
						memcpy( Memory, SubValue + AnchorStartPos, AnchorEndPos - AnchorStartPos );
						static_cast<char*>( Memory )[AnchorEndPos - AnchorStartPos] = '\0';
						Memory = static_cast<char*>( Memory ) + ( AnchorEndPos - AnchorStartPos ) + 1;

						*InstructLength = AnchorEndPos - AnchorStartPos;
					}

					EndPos--;

					/**
					 *  Выделение инструкции на изменение эмоции. Код эмоции должен быть записан в круглых скобках
					 * и состоять только из цифр.
					 */
					if( EndPos && SubValue[EndPos - 1] == ')' )
					{
						unsigned int EmotionEndPos = EndPos - 1;

						// Выделение кода эмоции.
						unsigned int EmotionStartPos = EndPos - 1;
						for(; EmotionStartPos > StartPos && '0' <= SubValue[EmotionStartPos - 1] && SubValue[EmotionStartPos] <= '9'; EmotionStartPos-- ) ;

						// Проверка корректности описания условия на якорь.
						if( EmotionStartPos <= StartPos || SubValue[EmotionStartPos - 1] != '(' )
						{
							// Некорректное описание инструкции присвоения эмоции.
							*EmotionShift = 0;
							*EmotionLength = 0;
						}
						else if( EmotionEndPos == EmotionStartPos )
						{
							// Пустая инструкция на присвоение якоря.
							*EmotionShift = 0;
							*EmotionLength = 0;

							EndPos = EmotionStartPos - 1;
						}
						else
						{
							// Сохранение инструкции по изменению эмоции.
							*EmotionShift = static_cast<char*>( Memory ) - ElementBase;
							memcpy( Memory, SubValue + EmotionStartPos, EmotionEndPos - EmotionStartPos );
							static_cast<char*>( Memory )[EmotionEndPos - EmotionStartPos] = '\0';
							Memory = static_cast<char*>( Memory ) + ( EmotionEndPos - EmotionStartPos ) + 1;

							*EmotionLength = EmotionEndPos - EmotionStartPos;

							EndPos = EmotionStartPos - 1;
						}
					}
					else
					{
						*EmotionShift = 0;
						*EmotionLength = 0;
					}
				}
			}
			else
			{
				*InstructShift = 0;
				*InstructLength = 0;
				*EmotionShift = 0;
				*EmotionLength = 0;
			}

			// Удаление пробельных символов.
			for(; StartPos < EndPos && SubValue[StartPos] == ' '; StartPos++ ) ;
			for(; StartPos < EndPos && SubValue[EndPos - 1] == ' '; EndPos-- ) ;

			if( EndPos > StartPos )
			{
				*ValueShift = static_cast<char*>( Memory ) - ElementBase;
				memcpy( Memory, SubValue + StartPos, EndPos - StartPos );
				static_cast<char*>( Memory )[EndPos - StartPos] = '\0';
				Memory = static_cast<char*>( Memory ) + ( EndPos - StartPos ) + 1;

				*ValueLength = EndPos - StartPos;
			}
			else
			{
				*ValueShift = 0;
				*ValueLength = 0;
			}
		}

		// Установка размера сохраненного значения.
		*Size = static_cast<char*>( Memory ) - reinterpret_cast<char*>( Size );
		if( ( *Size ) % sizeof( uint32_t ) )
		{
			// Выравнивание данных.
			Memory = static_cast<char*>( Memory ) + ( sizeof( uint32_t ) - ( *Size ) % sizeof( uint32_t ) );
			*Size += sizeof( uint32_t ) - ( *Size ) % sizeof( uint32_t );
		}

		// Выделение памяти.
		VarValue * Result = static_cast<VarValue*>( vAllocator.Allocate( sizeof( VarValue ) ) );
		if( !Result )
			ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
		new ( (nMemoryAllocatorElementPtr*)Result )VarValue( Size, this );

		aParsedValue = Result;

		return INF_ENGINE_SUCCESS;
	}
}
