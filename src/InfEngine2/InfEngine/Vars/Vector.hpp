#ifndef __VarsVector_hpp__
#define __VarsVector_hpp__

#include <NanoLib/aTextString.hpp>
#include "Registry.hpp"

namespace Vars
{
	/**
	 *  Вектор значений переменных.
	 */
	class Vector
	{
	public:
		/**
		 *  Конструктор.
		 * @param aVarsRegistry - реестр переменных.
		 */
		Vector( const Registry & aVarsRegistry ) : vFakeValue( {}, nullptr, this ), vVarsRegistry( &aVarsRegistry ) {}

	public:
		/**
		 *  Переключение реестра с проверкой совместимости.
		 * @param aVarsRegistry - новый реестр.
		 */
		bool SwitchRegistry( const Vars::Registry & aVarsRegistry ) noexcept;

	public:
		/** Представление значения текстовой переменной. */
		class VarText
		{
		public:
			/**
			 *  Конструктор.
			 * @param aBuffer - буффер со значением.
			 */
			VarText( const void * aBuffer ) : vBuffer( aBuffer ) {}

		public:
			operator bool() const { return vBuffer; }
			operator unsigned int() const = delete;
			operator int() const = delete;

			/** Получение идентификатора переменной. */
			Id GetId() const { return ( vBuffer ? Id( { reinterpret_cast<const uint32_t*>( vBuffer )[1], false } ) : Id {} ); }

			/** Получение свойств переменной. */
			Options GetProperties() const
			{
				if( !vBuffer )
					return Options::ReadOnly;

				return static_cast<Options>( static_cast<const char*>( vBuffer )[2 * sizeof( uint32_t )] );
			}

		public:
			/**
			 *  Получение значения переменной без свойства Extended.
			 * @param aValueLength - длина переменной.
			 */
			const char * GetValue( unsigned int & aValueLength ) const;

			/** Получение значения переменной без свойства Extended. */
			const char * GetValue() const
			{
				unsigned int ValueLength;
				return GetValue( ValueLength );
			}


		public:
			/** Получение количества вариантов значения для переменной со свойством Extended. */
			unsigned int GetNumberOfExtendedValues() const;

			/**
			 *  Получение значения со свойством Extended.
			 * @param aValNum - порядковый номер значения.
			 * @param aValueLength - длина значения.
			 */
			const char * GetExtendedValue( unsigned int aValNum, unsigned int & aValueLength ) const;

			/**
			 *  Получение условия на that_anchor для значения переменной со свойством Extended.
			 * @param aValNum - порядковый номер значения.
			 * @param aValueLength - длина значения.
			 */
			const char * GetExtendedCondition( unsigned int aValNum, unsigned int & aValueLength ) const;

			/**
			 *  Получение that_anchor инструкции для значения переменной со свойством Extended.
			 * @param aValNum - порядковый номер значения.
			 * @param aValueLength - длина значения.
			 */
			const char * GetExtendedInstruct( unsigned int aValNum, unsigned int & aValueLength ) const;

			/**
			 *  Получение инструкции на отображение эмоции.
			 * @param aValNum - порядковый номер значения.
			 * @param aValueLength - длина значения.
			 */
			const char * GetExtendedEmotion( unsigned int aValNum, unsigned int & aValueLength ) const;

		public:
			/** Получение размера памяти, необходимого для сохранения значения переменной. */
			unsigned int GetNeedMemorySize() const { return ( vBuffer ? reinterpret_cast<const uint32_t*>( vBuffer )[0] : 0 ); }

			/**
			 *  Сохранение значения в буффер.
			 * @param aBuffer - буффер.
			 */
			unsigned int Save( char * aBuffer ) const
			{
				unsigned int memory = GetNeedMemorySize();
				if( vBuffer )
				{
					memcpy( aBuffer, vBuffer, memory );
					return memory;
				}
				else
					return 0;
			}

		private:
			/** Буффер со значением переменной. */
			const void * vBuffer { nullptr };
		};

		class VarStruct;
		class VarArray;

		class VarValue
		{
			friend class VarStruct;
		public:
			VarValue( Id aId, const void * aBuffer, const Vector * aVector ) : vId( aId ), vBuffer( aBuffer ), vVector( aVector ) {}

			VarValue( const void * aBuffer, const Vector * aVector ) : vBuffer( aBuffer ), vVector( aVector )
			{
				if( aBuffer )
					vId.load( reinterpret_cast<const uint32_t*>( aBuffer )[1] );
			}

		public:
			Id GetId() const { return vId; }
            
			const char * GetName() const { return vVector->vVarsRegistry->GetVarNameById( GetId() ); }

			Type GetType() const { return vVector ? vVector->vVarsRegistry->GetVarType( GetId() ) : Type::Unknown; }

			bool IsNull() const { return !this->operator bool(); }

			operator bool() const { return vId.valid(); }

		public:
			VarText AsText() const
			{
				if( IsNull() )
					return VarText( nullptr );

				switch( GetType() )
				{
				case Type::Text:
					return VarText( vBuffer );
				case Type::Struct:
					{
						const VarValue dflt_value = vVector->GetStructValueAsText( GetId() );
						return dflt_value ? VarText( dflt_value.vBuffer ) : VarText( nullptr );
					}
					default:
						return VarText( nullptr );
				}
			}

			VarStruct AsStruct() const { return VarStruct( vId, vBuffer, vVector ); }

			VarArray AsArray() const { return VarArray( vId, vBuffer, vVector ); }

			/** Получение размера памяти, необходимого для сохранения значения переменной. */
			unsigned int GetNeedMemorySize() const
			{
				if( IsNull() )
					return 0;

				switch( GetType() )
				{
				case Type::Text:
					{
						VarText text_val( vBuffer );
						return text_val.GetNeedMemorySize();
					}
				case Type::Struct:
					{
						VarStruct struct_val( vId, vBuffer, vVector );
						return struct_val.GetNeedMemorySize();
					}
					default:
						return 0;
				}
			}

			/**
			 *  Сохранение значения в буффер.
			 * @param aBuffer - буффер.
			 */
			unsigned int Save( char * aBuffer ) const
			{
				if( IsNull() )
					return 0;

				switch( GetType() )
				{
				case Type::Text:
					{
						VarText text_val( vBuffer );
						return text_val.Save( aBuffer );
					}
				case Type::Struct:
					{
						VarStruct struct_val( vId, vBuffer, vVector );
						return struct_val.Save( aBuffer );
					}
					default:
						return 0;
				}
			}

		protected:
			Id vId;

			const void * vBuffer { nullptr };

			const Vector * vVector { nullptr };
		};

		/** Представление значения переменной типа структура. */
		class VarStruct
		{
		public:
			/**
			 *  Конструктор.
			 * @param aBuffer - буффер со значением.
			 * @param aVarsRegistry - реестр переменных.
			 */
			VarStruct( Id aId, const void * aBuffer, const Vector * aVector ) : vId( aId ), vBuffer( aBuffer ), vVector( aVector ) {}

			/** Получение размера памяти, необходимого для сохранения значения переменной. */
			unsigned int GetNeedMemorySize() const
			{
				const VarValue dflt_value = vVector->GetStructValueAsText( GetId() );
				if( dflt_value )
					return VarText( dflt_value.vBuffer ).GetNeedMemorySize();
				else
					return 0;
			}

			/**
			 *  Сохранение значения в буффер.
			 * @param aBuffer - буффер.
			 */
			unsigned int Save( char * aBuffer ) const
			{
				const VarValue dflt_value = vVector->GetStructValueAsText( GetId() );
				if( dflt_value )
					return VarText( dflt_value.vBuffer ).Save( aBuffer );
				else
					return 0;
			}

		public:
			/** Получение идентификатора переменной. */
			Id GetId() const { return vId; }

			/** Получение имени переменной. */
			const char * GetName() const { return vVector->vVarsRegistry->GetVarNameById( GetId() ); }

			/**
			 *  Получение имени переменной.
			 * @param aLength - длина имени.
			 */
			const char * GetName( unsigned int & aLength ) const { return vVector->vVarsRegistry->GetVarNameById( GetId(), aLength ); }

			/** Получение количества полей у структуры. */
			unsigned int GetFieldsCount() const { return vVector->vVarsRegistry->GetStructFieldsCount( GetId() ); }

			/**  Получение иденитфикатора переменной, являющейся значением по умолчанию структуры. */
			Id GetDefaultId() const { return vVector->vVarsRegistry->GetStructDefaultId( GetId() ); }

			/**
			 *  Получение иденитфикатора переменной, являющейся полем структуры.
			 * @param aFieldN - порядковый номер поля структуры (поле default всегда стоит под номером 0).
			 */
			Id GetFieldId( unsigned int aFieldN ) const { return vVector->vVarsRegistry->GetStructFieldId( GetId(), aFieldN ); }

			/**
			 *  Получение имени переменной, являющейся полем структуры.
			 * @param aFieldN - порядковый номер поля структуры (поле default всегда стоит под номером 0).
			 */
			const char * GetFieldName( unsigned int aFieldN ) const
			{
				return vVector->vVarsRegistry->GetVarNameById( GetFieldId( aFieldN ) );
			}

			/**
			 *  Получение имени переменной, являющейся полем структуры.
			 * @param aFieldN - порядковый номер поля структуры (поле default всегда стоит под номером 0).
			 * @param aNameLength - длина имени.
			 */
			const char * GetFieldName( unsigned int aFieldN, unsigned int & aNameLength ) const
			{
				return vVector->vVarsRegistry->GetVarNameById( GetFieldId( aFieldN ), aNameLength );
			}

			/**
			 *  Получение переменной, являющейся полем структуры.
			 * @param aFieldN - порядковый номер поля структуры (поле default всегда стоит под номером 0).
			 */
			VarValue GetFieldValue( unsigned int aFieldN ) const { return vVector->GetValue( GetFieldId( aFieldN ) ); }

			/**
			 *  Получение переменной, являющейся полем структуры.
			 * @param aFieldName - название поля.
			 * @param aNameLength - длина названия поля.
			 * @param aVarValue - результат.
			 */
			InfEngineErrors GetFieldValue( const char * aFieldName, unsigned int aNameLength, VarValue & aVarValue ) const
			{
				aVarValue = VarValue( nullptr, nullptr );

				// Получение имени структуры.
				unsigned int length;
				const char * name = GetName( length );
				if( !name )
					return INF_ENGINE_WARN_UNSUCCESS;
				// Построение полного имени поля.
				nlReturnCode nlrc = vTmpString.assign( name, length );
				if( nlrcSuccess != nlrc || nlrcSuccess != ( nlrc = vTmpString.append( ".", 1 ) ) || nlrcSuccess != ( nlrc = vTmpString.append( aFieldName, aNameLength ) ) )
					return INF_ENGINE_ERROR_NOFREE_MEMORY;

				// Получение поля.
				aVarValue = vVector->GetValue( vTmpString.ToConstChar(), vTmpString.size() );

				return INF_ENGINE_SUCCESS;
			}

		private:
			/** Идентификатор переменной. */
			Id vId;

			/** Буффер со значением переменной. */
			const void * vBuffer { nullptr };

			const Vector * vVector { nullptr };

		private:
			/** Поле для оптимизации внутренних вычислений. */
			static aTextString vTmpString;
		};

		/** Представление значения переменной типа массив (пока не реализовано). */
		class VarArray
		{
		public:
			/**
			 *  Конструктор.
			 * @param aBuffer - буффер со значением.
			 * @param aVarsRegistry - реестр переменных.
			 */
			VarArray( Id aId, const void * aBuffer, const Vector * aVector ) : vId( aId ), vBuffer( aBuffer ), vVector( aVector ) {}

		private:
			/** Идентификатор переменной. */
			Id vId;

			/** Буффер со значением переменной. */
			const void * vBuffer { nullptr };

			const Vector * vVector { nullptr };
		};

	public:
		/**
		 *  Установка значения переменной.
		 * @param aVarName - имя переменной.
		 * @param aVarNameLength - длина имени переменной.
		 * @param aVarValue - значение переменной.
		 * @param aVarValueLength - длина значения переменной.
		 */
		InfEngineErrors SetTextValue( const char * aVarName, unsigned int aVarNameLength,
									  const char * aVarValue, unsigned int aVarValueLength );

		/**
		 *  Установка значения переменной.
		 * @param aVarId - идентификатор переменной.
		 * @param aVarValue - значение переменной.
		 * @param aVarValueLength - длина значения переменной.
		 */
		InfEngineErrors SetTextValue( Id aVarId, const char * aVarValue, unsigned int aVarValueLength );

	private:
		VarValue * SaveTextValue( nMemoryAllocator & aAllocator, Id aVarId,
								  const char * aVarValue, unsigned int aVarValueLength,
								  Options aVarProperties ) const;

	public:
		/**
		 *  Установка значения временной переменной.
		 * @param aVarId - идентификатор переменной.
		 * @param aValue - значение.
		 * @param aValueLength - длина значения.
		 */
		InfEngineErrors SetTmpValue( Id aVarId, const char * aValue, unsigned int aValueLength ) const;

	public:
		/**
		 *  Получение значения переменной. При этом при неудаче производится поиск по значениям по умолчанию, до уровня
		 * вложенности не больше 5.
		 * @param aVarName - имя переменной.
		 * @param aVarNameLength - длина имени переменной.
		 */
		const VarValue GetValue( const char * aVarName, unsigned int aVarNameLength ) const;
        
        /**
		 *  Получение значения переменной. При этом при неудаче производится поиск по значениям по умолчанию, до уровня
		 * вложенности не больше 5.
		 * @param aVarName - имя переменной.
		 */
        const VarValue GetValue( const std::string & aVarName ) const { return GetValue( aVarName.c_str(), aVarName.length() ); }

		/**
		 *  Получение значения переменной. При этом при неудаче производится поиск по значениям по умолчанию, до уровня
		 * вложенности не больше 5.
		 * @param aVarId - идентификатор переменной.
		 */
		const VarValue GetValue( Id aVarId ) const;
		
		/**
		 *	Проверка существования переменной в сессии.
		 * @param aVarId - идентификатор переменной.
		 */
		bool VarExists( Id aVarId ) const { return vVarsRegistry->VarExists( aVarId ); }

		/**
		 *  Получение текстового значения переменной типа структура.
		 * @param aVarId - идентификатор переменной.
		 */
		const VarValue GetStructValueAsText( Id aVarId ) const;

		const VarValue GetTmpValue( Id aVarId ) const;

		/**
		 *  Получение переменной по её порядковому номеру в массиве.
		 */
		const VarValue  GetValueByN( unsigned int aNum ) const { return ( vValues.size() > aNum ? *( vValues[aNum] ) : vFakeValue ); }

		/**
		 *  Получение идентфикатора переменной. При этом при неудаче производится поиск по значениям по умолчанию, до уровня
		 * вложенности не больше 5. Результат -1 означает, что переменная не найдена.
		 * @param aVarName - имя переменной.
		 * @param aVarNameLength - длина имени переменной.
		 */
		Id GetValueId( const char * aVarName, unsigned int aVarNameLength ) const
		{
			return vVarsRegistry->Search( aVarName, aVarNameLength );
		}

		/** Получение количества переменных. */
		unsigned int GetValuesNumber() const { return vValues.size(); }

		/** Получение переменной по её порядковому номеру в массиве. */
		const VarText * GetTextValueByN( unsigned int aNum ) const
		{
			return ( vValues.size() > aNum ? reinterpret_cast<const VarText*>( vValues[aNum] ) : nullptr );
		}

	public:
		/** Получение размера памяти, необходимого для сохранения вектора значений переменных. */
		unsigned int GetNeedMemorySize() const
		{
			if( static_cast<unsigned int>( -1 ) == vNeedMemorySize )
			{
				vNeedMemorySize = 2 * sizeof( uint32_t );
				vNeedMemorySize += vValues.size() * sizeof( uint32_t );
				for( int i = 0; i < vValues.size(); ++i )
					vNeedMemorySize += vValues[i]->GetNeedMemorySize();
				vNeedMemorySize += sizeof( uint32_t );
			}

			return vNeedMemorySize;
		}

		unsigned int Save( void * aBuffer ) const;

		/**
		 *  Загрузка объекта из буффера.
		 * @param aBuffer - буффер.
		 * @param aBufferSize - размер буффера.
		 * @param aResultSize - размер сохраненного вектора значений переменных.
		 */
		InfEngineErrors Load( const char * aBuffer, unsigned int aBufferSize, unsigned int & aResultSize );

	public:
		/**
		 *  Установка вектора значений переменных по умолчанию. То есть вектора, в котором будет производиться поиск
		 * значения переменной, если в текущем векторе его не будет найдено.
		 * @param aDefaultValues - вектор значений переменных по умолчанию.
		 */
		InfEngineErrors SetDefaultValues( const Vector & aDefaultValues );

		/** Очистка вектора. При этом указатель на массив значений по умолчанию не сбрасываются. */
		void Reset();

		void ResetTmp() const
		{
			vTmpAllocator.Reset();
			vTmpValues.clear();
			if( vDefaultValues )
				vDefaultValues->ResetTmp();
		}


	private:
		/**
		 *  Поиск идентификатора переменной по массиву vValues.
		 * @param aVarId - идентификатор переменной.
		 * @param aIndexPos - Позиция в массиве vValues, соответствущая идентификатору переменной, в случае успеха и
		 *                   позиция, на которую нужно поместить значение переменной, если нужно его добавить.
		 * @return true, если переменная найдена, false - иначе.
		 */
		bool SearchInd( Id aVarId, unsigned int & aIndexPos ) const;

		/**
		 *  Разбор значения переменной и сохранение его в памяти.
		 * @param aVarId - идентификатор переменной. Этот идентификатор будет сохранен вместе со значением.
		 * @param aVarValue - значение переменной. Значение разбирается исходя из свойств переменной.
		 * @param aVarValueLength - длина значения переменной.
		 * @param aParsedValue - указатель на результат. В случае неудачи указатель будет равен nullptr.
		 */
		InfEngineErrors ParseValue( Id aVarId, const char * aVarValue, unsigned int aVarValueLength, const VarValue* & aParsedValue );

	private:
		/** Вспомогательный массив. */
		mutable avector<unsigned int> vTmpBuffer;

		/** Пустое значение переменной. */
		const VarValue vFakeValue;

	private:
		/** Аллокатор памяти. */
		nMemoryAllocator vAllocator;

		/** Массив указателей на значения. */
		avector<const VarValue*> vValues;

		/** Размер памяти, необходимый для сохранения вектора значений переменных. */
		mutable unsigned int vNeedMemorySize = -1;

		/** Указатель на вектор значений по умолчанию. */
		const Vector * vDefaultValues { nullptr };

	private:
		mutable nMemoryAllocator vTmpAllocator;
		mutable avector<const VarValue*> vTmpValues;

	public:
		/** Реестр переменных. */
		const Registry * vVarsRegistry { nullptr };
	};
}

#endif  /** __VarsVector_hpp__ */
