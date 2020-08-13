#ifndef __BelongsTo_hpp__
#define __BelongsTo_hpp__

#include <InfEngine2/Functions/FunctionLib.hpp>
#include <InfEngine2/InfEngine/CoreRO.hpp>
#include <InfEngine2/InfEngine/Terms/Matcher.hpp>
#include <NanoLib/LogSystem.hpp>

namespace InternalFunctions
{
	class BelongsTo
	{
	public:
		static void Init( ExtICoreRO & aBase ) { vBase = &aBase; }

		static InfEngineErrors Apply( const char ** aArgs,
									  const char* & aResult,
									  unsigned int & aLength,
									  nMemoryAllocator * aMemoryAllocator,
									  FunctionContext * aFunctionContext );

	public:
		static const DLFucntionArgInfo ARGS[];
		static const DLFucntionArgInfo VAR_ARGS;
		static const DLFunctionInfo DLF_INFO;

	protected:
		static InfEngineErrors _apply( const char * aWord,
									   const char ** aDicts,
									   nMemoryAllocator * aMemoryAllocator,
									   Session * aSession,
									   DictMatcher & aDictMatcher,
									   DocImage & aDocImage,
									   CDocMessage & aDocMessage,
									   DictTermsFilter & aDictFilter );

	protected:
		static DictMatcher vDictMatcher;
		static DocImage vDocImage;
		static CDocMessage vDocMessage;
		static DictTermsFilter vDictFilter;

	private:
		static ExtICoreRO * vBase;
	};

}

#endif // __BelongsTo_hpp__
