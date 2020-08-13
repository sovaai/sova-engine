#include "iAnswer.hpp"

InfEngineErrors iAnswer::AddText( const char * aText, unsigned int aTextLength, bool aForceCopy )
{
	// Проверка аргументов.
	if( !aText && aTextLength != 0 )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Выделение памяти под элемент.
	Item * pItem = vItems.grow();
	if( !pItem || vItems.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Выделение памяти для копирования текста.
	if( vFullCopyMode || aForceCopy )
	{
		char * ptr = static_cast<char *>( Allocate( aTextLength + 1 ) );
		if( !ptr )
		{
			vItems.pop_back();

			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		memcpy( ptr, aText, aTextLength );
		ptr[aTextLength] = '\0';

		aText = ptr;
	}

	// Происвоение значений.
	pItem->vType = Item::Type::Text;
	pItem->vItem.vText.vValue = aText;
	pItem->vItem.vText.vLength = aTextLength;
	pItem->vPriorityFlag = false;


	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::AddSpace( const char * aSpaces, unsigned int aSpacesLength, bool aForceCopy )
{
	// Проверка аргументов.
	if( !aSpaces && aSpacesLength != 0 )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Выделение памяти под элемент.
	Item * pItem = vItems.grow();
	if( !pItem || vItems.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Выделение памяти для копирования текста.
	if( vFullCopyMode || aForceCopy )
	{
		char * ptr = static_cast<char *>( Allocate( aSpacesLength + 1 ) );
		if( !ptr )
		{
			vItems.pop_back();

			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		memcpy( ptr, aSpaces, aSpacesLength );
		ptr[aSpacesLength] = '\0';

		aSpaces = ptr;
	}

	// Происвоение значений.
	pItem->vType = Item::Type::Space;
	pItem->vItem.vText.vValue = aSpaces;
	pItem->vItem.vText.vLength = aSpacesLength;
	pItem->vPriorityFlag = false;


	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::AddHref( const char * aLink, unsigned int aLinkLength,
		const char * aTarget, unsigned int aTargetLength,
		const char * aURL, unsigned int aURLLength, bool aForceCopy )
{
	// Проверка аргументов.
	if( ( !aLink && aLinkLength != 0 ) || ( !aTarget && aTargetLength != 0 ) || ( !aURL && aURLLength != 0 ) )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Выделение памяти под элемент.
	Item * pItem = vItems.grow();
	if( !pItem || vItems.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Выделение памяти для копирования данных.
	if( vFullCopyMode || aForceCopy )
	{
		char * ptr = static_cast<char *>( Allocate( aLinkLength + aTargetLength + aURLLength + 3 ) );
		if( !ptr )
		{
			vItems.pop_back();

			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		memcpy( ptr, aLink, aLinkLength );
		ptr[aLinkLength] = '\0';

		aLink = ptr;
		ptr += aLinkLength + 1;

		memcpy( ptr, aTarget, aTargetLength );
		ptr[aTargetLength] = '\0';

		aTarget = ptr;
		ptr += aTargetLength + 1;

		memcpy( ptr, aURL, aURLLength );
		ptr[aURLLength] = '\0';

		aURL = ptr;
	}

	pItem->vType = Item::Type::Href;
	pItem->vItem.vHref.vURL = aURL;
	pItem->vItem.vHref.vURLLength = aURLLength;
	pItem->vItem.vHref.vTarget = aTarget;
	pItem->vItem.vHref.vTargetLength = aTargetLength;
	pItem->vItem.vHref.vLink = aLink;
	pItem->vItem.vHref.vLinkLength = aLinkLength;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::AddInstruct( Vars::Id aVarId, const char * aValue, unsigned int aValueLength, bool aForceCopy )
{
	// Проверка аргументов.
	if( !aValue && aValueLength != 0 )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Выделение памяти под элемент.
	Item * pItem = vItems.grow();
	if( !pItem || vItems.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Выделение памяти для копирования значения.
	if( vFullCopyMode || aForceCopy )
	{
		char * ptr = static_cast<char *>( Allocate( aValueLength + 1 ) );
		if( !ptr )
		{
			vItems.pop_back();

			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		memcpy( ptr, aValue, aValueLength );
		ptr[aValueLength] = '\0';

		aValue = ptr;
	}

	pItem->vType = Item::Type::Instruct;
	pItem->vItem.vInstruct.vVarId = aVarId;
	pItem->vItem.vInstruct.vValue = aValue ? aValue : "";
	pItem->vItem.vInstruct.vValueLength = aValueLength;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::AddBr()
{
	// Выделение памяти под элемент.
	Item * pItem = vItems.grow();
	if( !pItem || vItems.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	pItem->vType = Item::Type::Br;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::StartList( bool aOrdered )
{
	Item * pItem = vItems.grow( );
	if( !pItem || vItems.no_memory( ) )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	pItem->vType = aOrdered ? Item::Type::StartOList : Item::Type::StartUList;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::AddListItem()
{
	Item * pItem = vItems.grow( );
	if( !pItem || vItems.no_memory( ) )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	pItem->vType = Item::Type::ListItem;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::FinishList()
{
	Item * pItem = vItems.grow( );
	if( !pItem || vItems.no_memory( ) )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	pItem->vType = Item::Type::EndList;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::AddInf( const char * aText, unsigned int aTextLength, const char * aRequest, unsigned int aRequestLength, bool aForceCopy )
{
	// Проверка аргументов.
	if( !aText && aTextLength != 0 )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Выделение памяти под элемент.
	Item * pItem = vItems.grow();
	if( !pItem || vItems.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Выделение памяти для копирования значения.
	if( vFullCopyMode || aForceCopy )
	{
		char * ptr = static_cast<char *>( Allocate( aTextLength + 1 ) );
		if( !ptr )
		{
			vItems.pop_back();

			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		memcpy( ptr, aText, aTextLength );
		ptr[aTextLength] = '\0';

		aText = ptr;

		ptr = static_cast<char *>( Allocate( aRequestLength + 1 ) );
		if( !ptr )
		{
			vItems.pop_back();

			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		memcpy( ptr, aRequest, aRequestLength );
		ptr[aRequestLength] = '\0';

		aRequest = ptr;
	}

	pItem->vType = Item::Type::Inf;
	pItem->vItem.vInf.vValue = aText;
	pItem->vItem.vInf.vValueLength = aTextLength;
	pItem->vItem.vInf.vRequest = aRequest;
	pItem->vItem.vInf.vRequestLength = aRequestLength;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::AddOpenWindow( const char * aURL, unsigned int aURLLength, unsigned int aTarget, bool aForceCopy )
{
	// Проверка аргументов.
	if( !aURL && aURLLength != 0 )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Выделение памяти под элемент.
	Item * pItem = vItems.grow();
	if( !pItem || vItems.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	// Выделение памяти для копирования значения.
	if( vFullCopyMode || aForceCopy )
	{
		char * ptr = static_cast<char *>( Allocate( aURLLength + 1 ) );
		if( !ptr )
		{
			vItems.pop_back();

			return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		memcpy( ptr, aURL, aURLLength );
		ptr[aURLLength] = '\0';

		aURL = ptr;
	}

	pItem->vType = Item::Type::OpenWindow;
	pItem->vItem.vOpenWindow.vURL = aURL;
	pItem->vItem.vOpenWindow.vURLLength = aURLLength;
	pItem->vItem.vOpenWindow.vTarget = aTarget;

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::AddRSS( const char * aURL, unsigned int aURLLength, const char * aAlt, unsigned int aAltLength,
		unsigned int aOffset, bool aShowTitle, bool aShowLink, unsigned int aUpdatePeriod, bool aForceCopy )
{
	// Проверка аргументов.
	if( (!aURL && aURLLength) || (!aAlt && aAltLength) )
		return INF_ENGINE_ERROR_INV_ARGS;

	// Выделение памяти для копирования значения.
	if( vFullCopyMode || aForceCopy )
	{
		char * ptr = static_cast<char *>( Allocate( aURLLength + 1 ) );
		if( !ptr )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;

		if( aURLLength )
			memcpy( ptr, aURL, aURLLength );
		ptr[aURLLength] = '\0';

		aURL = ptr;

		ptr = static_cast<char *>( Allocate( aAltLength + 1 ) );
		if( !ptr )
			return INF_ENGINE_ERROR_NOFREE_MEMORY;

		if( aAltLength )
			memcpy( ptr, aAlt, aAltLength );
		ptr[aAltLength] = '\0';

		aAlt = ptr;
	}

	// Выделение памяти под элемент.
	Item * pItem = vItems.grow();
	if( !pItem || vItems.no_memory() )
		return INF_ENGINE_ERROR_NOFREE_MEMORY;

	pItem->vType = Item::Type::RSS;
	pItem->vItem.vRSS.vURL = aURL;
	pItem->vItem.vRSS.vURLLength = aURLLength;
	pItem->vItem.vRSS.vAlt = aAlt;
	pItem->vItem.vRSS.vAltLength = aAltLength;
	pItem->vItem.vRSS.vOffset = aOffset;
	pItem->vItem.vRSS.vShowTitle = aShowTitle;
	pItem->vItem.vRSS.vShowLink = aShowLink;
	pItem->vItem.vRSS.vUpdatePeriod = aUpdatePeriod;

	return INF_ENGINE_SUCCESS;
}

void iAnswer::SetPriority( unsigned int aItemInd )
{
	if( aItemInd <= vItems.size() )
		vItems[aItemInd].vPriorityFlag = true;
}

InfEngineErrors iAnswer::ProcessPriorityItems()
{
	// Перенос приоритетных инструкций в конец ответа.
	if( vItems.size() > 1 )
	{
		// Последняя неприоритетная позиция в массиве.
		unsigned int LastAvbPos = vItems.size() - 1;
		for( unsigned int i = vItems.size(); i > 0; i-- )
		{
			// Пропускаем все неприоритетные элементы.
			if( !vItems[i - 1].vPriorityFlag )
				continue;

			unsigned int end = i - 1;

			// Выделение отрезка элементов, целеком состоящего из приоритетных элементов.
			unsigned int begin = end;
			for( ; begin > 0 && vItems[begin - 1].vPriorityFlag; begin-- );

			if( end < LastAvbPos )
			{
				// Выделение памяти для реализации переноса элементов.
				unsigned int originalsize = vItems.size();
				unsigned int size = end - begin + 1;
				vItems.resize( originalsize + size );
				if( vItems.no_memory() )
					return INF_ENGINE_ERROR_NOFREE_MEMORY;

				// Перенос элементов.
				memcpy( vItems.get_buffer() + originalsize, vItems.get_buffer() + begin, sizeof (Item ) * size );
				memcpy( vItems.get_buffer() + begin, vItems.get_buffer() + end + 1, sizeof (Item ) * ( LastAvbPos - end ) );
				memcpy( vItems.get_buffer() + LastAvbPos - size + 1, vItems.get_buffer() + originalsize, sizeof (Item ) * size );

				// Возвращение массиву исходной длины.
				vItems.resize( originalsize );

				// Корректировка первой неприоритетной позиции.
				LastAvbPos -= size;
			}

			i = begin + 1;
		}
	}

	return INF_ENGINE_SUCCESS;
}

bool iAnswer::Empty() const
{
	if( vForceNotEmpty )
		return false;
	for( unsigned int i = 0; i < vItems.size(); i++ )
		switch( vItems[i].vType )
		{
		case Item::Type::Text:
			if( vItems[i].vItem.vText.vLength )
				return false;
			break;

		case Item::Type::Href:
		case Item::Type::Inf:
		case Item::Type::OpenWindow:
		case Item::Type::RSS:
			return false;
		default:
			break;
		}

	return true;
}

InfEngineErrors iAnswer::ToString( aTextString & aResult, unsigned int aFrom, unsigned int aTo ) const
{
	// Очистка строки.
	aResult.clear();

	Item::Type first_elem = Item::Type::None;
	Item::Type last_elem = Item::Type::None;

	for( unsigned int i = aFrom; i < aTo; i++ )
	{
		if( i )
		{
			if( aResult.append( " " ) != nlrcSuccess )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
		}

		switch( vItems[i].vType )
		{
		case Item::Type::Text:
			if( first_elem == Item::Type::None )
				first_elem = vItems[i].vType;
			last_elem = vItems[i].vType;

			if( aResult.append( vItems[i].vItem.vText.vValue, vItems[i].vItem.vText.vLength ) != nlrcSuccess )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
			break;

		case Item::Type::Space:
			if( first_elem == Item::Type::None )
				first_elem = vItems[i].vType;
			last_elem = vItems[i].vType;

			if( aResult.append( vItems[i].vItem.vText.vValue, vItems[i].vItem.vText.vLength ) != nlrcSuccess )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
			break;

		case Item::Type::Br:
			break;

		case Item::Type::Href:
			if( first_elem == Item::Type::None )
				first_elem = vItems[i].vType;
			last_elem = vItems[i].vType;

			if( aResult.append( vItems[i].vItem.vHref.vLink, vItems[i].vItem.vHref.vLinkLength ) != nlrcSuccess )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
			break;

		case Item::Type::Inf:
			if( first_elem == Item::Type::None )
				first_elem = vItems[i].vType;
			last_elem = vItems[i].vType;

			if( aResult.append( vItems[i].vItem.vInf.vValue, vItems[i].vItem.vInf.vValueLength ) != nlrcSuccess )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
			break;

		case Item::Type::Instruct:
			break;

		case Item::Type::OpenWindow:
			if( first_elem == Item::Type::None )
				first_elem = vItems[i].vType;
			last_elem = vItems[i].vType;

			if( aResult.append( vItems[i].vItem.vOpenWindow.vURL, vItems[i].vItem.vOpenWindow.vURLLength ) != nlrcSuccess )
				return INF_ENGINE_ERROR_NOFREE_MEMORY;
			break;

		case Item::Type::None:
			break;

		case Item::Type::RSS:
			break;
		
			case Item::Type::StartUList:
			case Item::Type::StartOList:
			case Item::Type::ListItem:
			case Item::Type::EndList:
				break;
		}
	}

	AllTrim( aResult, first_elem != Item::Type::Space, last_elem != Item::Type::Space );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors iAnswer::Assign( const iAnswer & aAnswer )
{
	Reset();

	for( unsigned int i = 0; i < aAnswer.Size(); i++ )
	{
		InfEngineErrors iee;

		switch( aAnswer[i].vType )
		{
		case Item::Type::Href:
			if( ( iee = AddHref( aAnswer[i].vItem.vHref.vLink, aAnswer[i].vItem.vHref.vLinkLength,
							aAnswer[i].vItem.vHref.vTarget, aAnswer[i].vItem.vHref.vTargetLength,
							aAnswer[i].vItem.vHref.vURL, aAnswer[i].vItem.vHref.vURLLength,
							true ) ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::Inf:
			if( ( iee = AddInf( aAnswer[i].vItem.vInf.vValue, aAnswer[i].vItem.vInf.vValueLength,
							aAnswer[i].vItem.vInf.vRequest, aAnswer[i].vItem.vInf.vRequestLength, true ) ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::Instruct:
			if( ( iee = AddInstruct( aAnswer[i].vItem.vInstruct.vVarId,
							aAnswer[i].vItem.vInstruct.vValue,
							aAnswer[i].vItem.vInstruct.vValueLength, true ) ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::None:
			break;
		case Item::Type::OpenWindow:
			if( ( iee = AddOpenWindow( aAnswer[i].vItem.vOpenWindow.vURL,
							aAnswer[i].vItem.vOpenWindow.vURLLength,
							aAnswer[i].vItem.vOpenWindow.vTarget, true ) ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::Br:
			if( ( iee = AddBr() ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::StartUList:
			if( ( iee = StartList( false ) ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::StartOList:
			if( ( iee = StartList( true ) ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::ListItem:
			if( ( iee = AddListItem() ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::EndList:
			if( ( iee = FinishList() ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::Text:
			if( ( iee = AddText( aAnswer[i].vItem.vText.vValue,
							aAnswer[i].vItem.vText.vLength, true ) ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::Space:
			if( ( iee = AddSpace( aAnswer[i].vItem.vText.vValue,
							aAnswer[i].vItem.vText.vLength, true ) ) != INF_ENGINE_SUCCESS )
				return iee;
			break;
		case Item::Type::RSS:
			if( ( iee = AddRSS( aAnswer[i].vItem.vRSS.vURL, aAnswer[i].vItem.vRSS.vURLLength, aAnswer[i].vItem.vRSS.vAlt,
							aAnswer[i].vItem.vRSS.vAltLength, aAnswer[i].vItem.vRSS.vOffset, aAnswer[i].vItem.vRSS.vShowTitle,
							aAnswer[i].vItem.vRSS.vShowLink, aAnswer[i].vItem.vRSS.vUpdatePeriod, true ) ) != INF_ENGINE_SUCCESS )
				return iee;
		}
	}

	vPatternId = aAnswer.vPatternId;
	vAnswerId = aAnswer.vAnswerId;
	vForceNotEmpty = aAnswer.vForceNotEmpty;
	vDisableAutoVars = aAnswer.vDisableAutoVars;

	return INF_ENGINE_SUCCESS;
}
