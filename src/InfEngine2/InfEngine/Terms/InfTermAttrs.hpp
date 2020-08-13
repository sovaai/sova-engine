#ifndef INFENGINE_TERM_ATTRS_HPP
#define INFENGINE_TERM_ATTRS_HPP

#include <cstring>

#include <Filtration3/terms/term_attrs.h>

/**
 * Управление упаковкой данных в поисковые термины.
 *
 * RubricId uint32_t
 *   - FFFFFFFF - первичный идентификатор.

 * SecondaryId uint32_t
 *   - FFFFFF00 - Номер поискового шаблона в общем шаблоне. ( 2^8 )
 *   - 000000FF - Позиция термина в поисковом шаблоне. ( 2^8 )
 *
 * InfTermFlags uint32_t
 *   - 00000001 - Флаг последнего термина в поисковом шаблоне.
 *   - 00000002 - Флаг первого не виртуального термина.
 *   - 00000004 - Флаг звездочки перед термином.
 *   - 00000008 - Флаг суперзвездочки перед термином.
 *   - 00000010 - Флаг звездочки после термина.
 *   - 00000020 - Флаг суперзвездочки после термина.
 *   - 00000040 - Флаг словаря после термина.
 *   - 00000080 - Флаг словаря перед термином.
 *   - 00000100 - Флаг переменной после термина.
 *   - 00000200 - Флаг переменной перед термином.
 **/

class InfTermAttrs
{
public:
	/** Конструктор. */
	InfTermAttrs() { clear(); }
	/** Конструктор копирования. */
	InfTermAttrs( const InfTermAttrs & aTermAttrs ) { assign( aTermAttrs.raw() ); }
	/** Конструктор копирования бинарного представления атрибутов. */
	explicit InfTermAttrs( const cf_term_attrs & aTermAttrs ) { assign( aTermAttrs ); }

public:
	/** Копирование. */
	InfTermAttrs & operator= ( const InfTermAttrs & aTermAttrs ) { assign( aTermAttrs.raw() ); return *this; }
	/** Копирование бинарного представления атрибутов.. */
	InfTermAttrs & operator= ( const cf_term_attrs & aTermAttrs ) { assign( aTermAttrs ); return *this; }
	/** Копирование бинарного представления атрибутов. */
	void assign( const cf_term_attrs & aAttrs ) { std::memcpy( vAttrs, aAttrs, sizeof( cf_term_attrs ) ); }

public:
	/** Очистка данных. */
	void clear()
	{
		std::memset( &vAttrs, 0, sizeof( cf_term_attrs ) );
		set_term_morph_any( &vAttrs );
		set_term_lang( &vAttrs, LNG_RUSSIAN );
	}

public:
	/** Получение бинарого представления атрибутов. */
	const cf_term_attrs & raw() const { return vAttrs; }

public:
	using id_type = uint32_t;

	void SetId( id_type aId ) { set_term_rubric_id( &vAttrs, aId ); }
	id_type GetId() { return term_rubric_id( &vAttrs ); }

public:
	using subid_type = uint32_t;

	void SetSubId( subid_type aSubId )
	{
		set_term_ext_id( &vAttrs, ( ( 0xFFFFFF00 & ( aSubId << 8 ) ) | ( 0x000000FF & ( term_ext_id( &vAttrs ) ) ) ) );
	}
	subid_type GetSubId() { return static_cast<subid_type>( ( term_ext_id( &vAttrs ) & 0xFFFFFF00 ) >> 8 ); }

public:
	using pos_type = uint8_t;

	void SetPos( pos_type aPos )
	{
		set_term_ext_id( &vAttrs, ( ( 0x000000FF & aPos ) | ( 0xFFFFFF00 & ( term_ext_id( &vAttrs ) ) ) ) );
	}
	pos_type GetPos() { return static_cast<pos_type>( term_ext_id( &vAttrs ) & 0x000000FF ); }

public:
	void SetFlagLastTerm() { set_term_ifl( &vAttrs, ( term_ifl( &vAttrs ) | 0x00000001 ) ); }
	bool GetFlagLastTerm() { return ( term_ifl( &vAttrs ) & 0x00000001 ) ? true : false; }

public:
	void SetFlagFirstRealTerm() { set_term_ifl( &vAttrs, ( term_ifl( &vAttrs ) | 0x00000002 ) ); }
	bool GetFlagFirstRealTerm() { return ( term_ifl( &vAttrs ) & 0x00000002 ) ? true : false; }

public:
	void SetFlagStarBefore() { set_term_ifl( &vAttrs, ( term_ifl( &vAttrs ) | 0x00000004 ) ); }
	bool GetFlagStarBefore() { return ( term_ifl( &vAttrs ) & 0x00000004 ) ? true : false; }

public:
	void SetFlagStarAfter() { set_term_ifl( &vAttrs, ( term_ifl( &vAttrs ) | 0x00000010 ) ); }
	bool GetFlagStarAfter() { return ( term_ifl( &vAttrs ) & 0x00000010 ) ? true : false; }

public:
	void SetFlagSuperStarBefore() { set_term_ifl( &vAttrs, ( term_ifl( &vAttrs ) | 0x00000008 ) ); }
	bool GetFlagSuperStarBefore() { return ( term_ifl( &vAttrs ) & 0x00000008 ) ? true : false; }

public:
	void SetFlagSuperStarAfter() { set_term_ifl( &vAttrs, ( term_ifl( &vAttrs ) | 0x00000020 ) ); }
	bool GetFlagSuperStarAfter() { return ( term_ifl( &vAttrs ) & 0x00000020 ) ? true : false; }

private:
	/** Бинарное представление атрибутов. */
	cf_term_attrs vAttrs;
};

#endif /** INFENGINE_TERM_ATTRS_HPP */
