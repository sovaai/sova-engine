#ifndef __TLCS_hpp__
#define __TLCS_hpp__

#include <InfEngine2/InfEngine/InfPattern/Items.hpp>
#include <InfEngine2/InfEngine/InfPattern/String.hpp>

namespace InfPatternItems
{
	class TLCS_WR : public StringRW
	{
	public:
		enum class Type
		{
			If			= 0,
			Elsif		= 1,
			Else		= 2,
			Endif		= 3,
			Switch		= 4,
			Case		= 5,
			Default		= 6,
			Endswitch	= 7,
			Unknown		= 8,
		};

		/**
		 *  Конструктор.
		 * @param aBoolFunction - условный оператор (функция InfPatternItems::TagFunction типа BOOL).
		 * @param aItemsNum - число DL элементов.
		 */
		TLCS_WR( InfPatternItems::Base ** aBoolFunction, Type aTLCSType )
			: StringRW( aBoolFunction ? aBoolFunction : nullptr, aBoolFunction ? 1 : 0 ) , vTLCSType( aTLCSType ) {}

		/** Получение типа условного оператора уровня шаблона. */
		Type GetTLCSType() const { return vTLCSType; }

		/** Вычисление объема памяти, необходимого для сохранения. */
		unsigned int GetNeedMemorySizeForAttrs() const { return sizeof( uint32_t ); }

		/** Сохранение. */
		unsigned int SaveAttrs( void * aBuffer ) const
		{
			return binary_data_save_rs<uint32_t>( aBuffer, static_cast<unsigned int>( vTLCSType ) );
		}

	protected:
		// Тип условного оператора уровня шаблона.
		Type vTLCSType;
	};

	using TLCS_WRs = avector<InfPatternItems::TLCS_WR*>;


	class TLCS_RO : public StringRO
	{
	public:
		using Type = TLCS_WR::Type;

	public:
		/** Конструктор пустого шаблон-ответа. */
		TLCS_RO() {}

		/**
		 *  Конструктор.
		 * @param aItems - строка элементов условного оператора уровня шаблона.
		 * @param aAttrs - сохранённые атрибуты условного оператора уровня шаблона.
		 */
		TLCS_RO( const ArrayManipulator & aItems, const void * aAttrs = nullptr ) :
			StringRO( aItems ) { vType = static_cast<Type>( static_cast<const uint32_t*>( aAttrs )[0] ); }

		/** Возвращает тип условного оператора уровня шаблона. */
		Type GetType() const { return vType; }

	private:
		// Тип условного оператора уровня шаблона.
		Type vType = Type::Unknown;
	};
}

#endif	// __TLCS_hpp__
