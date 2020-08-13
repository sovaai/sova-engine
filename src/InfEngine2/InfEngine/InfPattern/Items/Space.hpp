#ifndef __InfPatternItemsSpace_hpp__
#define __InfPatternItemsSpace_hpp__

#include <InfEngine2/InfEngine/InfPattern/Items/Text.hpp>

namespace InfPatternItems
{
	/**
	 *  Представление пробельного элемента.
	 */
	class Space : public Text
	{
	public:
		/**
		 *  Конструктор.
		 * @param aBuffer - буфер с сохраненными пробеольными символами.
		 */
		Space( const void * aBuffer = nullptr ): Text( aBuffer ) {}
	};
};

#endif /** __InfPatternItemsSpace_hpp__ */
