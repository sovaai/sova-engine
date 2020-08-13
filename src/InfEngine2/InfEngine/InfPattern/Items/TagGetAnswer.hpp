#ifndef __TagGetAnswer_hpp__
#define	__TagGetAnswer_hpp__

#include "Base.hpp"

namespace InfPatternItems
{
	class TagGetAnswer: public Base
	{
	public:
		/** Constructor. */
		TagGetAnswer()
			{ vType = itGetAnswer; }

	public:
		/**
		 *  Set target pattern's identificator.
		 * @param aTargetPatternId - target pattern's identificator.
		 */
		void Set( unsigned int aTargetPatternId )
			{ vTargetPatternId = aTargetPatternId; }

	public:
		unsigned int GetNeedMemorySize() const
			{ return sizeof(uint32_t); }

		unsigned int Save( void * aBuffer ) const
		{
			*static_cast<uint32_t*>(aBuffer) = vTargetPatternId;
			return sizeof( uint32_t );
		}

	private:
		/** Target's pattern identificator. */
		unsigned int vTargetPatternId = 0;
	};

	/**
	 *  Manipulator for GetAnswer tag.
	 */
	class TagGetAnswerManipulator
	{
	public:
		/**
		 *  Constructor.
		 * @param aBuffer - buffer contains tag GetAnswer.
		 */
		TagGetAnswerManipulator( const void * aBuffer = nullptr ):
			vBuffer( aBuffer ) {}

	public:
		/**
		 *  Initialization.
		 * @param aBuffer - buffer contains tag GetAnswer.
		 */
		void Init( const void * aBuffer )
			{ vBuffer = aBuffer; }

	public:
		/** Get target pattern identificator. */
		unsigned int GetTargetId() const
			{ return vBuffer ? *static_cast<const uint32_t*>(vBuffer) : 0; }

	private:
		/** Buffer contains tag GetAnswer. */
		const void * vBuffer = nullptr;
	};
}

#endif	/** __TagGetAnswer_hpp__ */
