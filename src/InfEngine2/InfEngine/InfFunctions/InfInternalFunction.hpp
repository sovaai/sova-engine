#ifndef INF_INTERNAL_FUNCTION_HPP
#define INF_INTERNAL_FUNCTION_HPP

#include "InfFunction.hpp"

/**
 *  Класс для работы с внутренними функциями языка DL.
 */
class InternalInfFunction : public InfFunction
{
public:
	/** Конструктор класса InfFunction. */
	InternalInfFunction() : InfFunction( DLFT_INTERNAL ) {}

	/** Конструктор копирования. */
	InternalInfFunction( const InfFunction & aFunction ) = delete;

public:
	/** Копирование. */
	InternalInfFunction& operator=( const InfFunction & aFunction ) = delete;

public:
	/**
	 *  Установка данных.
	 * @param aDLFunctionInfo
	 */
	InfEngineErrors SetDLFunctionInfo( const DLFunctionInfo * aDLFunctionInfo )
	{
		vDLFunctionInfo = aDLFunctionInfo;

		vNameLength = std::strlen( vDLFunctionInfo->name );

		return INF_ENGINE_SUCCESS;
	}
};


/** Класс для манипуляции образом внутренней функции в памяти. */
class InternalInfFunctionManipulator : public InfFunctionManipulator
{
public:
	InternalInfFunctionManipulator() {}
	InternalInfFunctionManipulator( const char * aBuffer ) { SetBuffer( aBuffer ); }
	InternalInfFunctionManipulator( const InfFunctionManipulator & aFunction ) = delete;

public:
	InternalInfFunctionManipulator& operator=( const InfFunctionManipulator & aFunction ) = delete;

public:
	/** Загружает из памяти информацию о функции. */
	virtual InfEngineErrors SetBuffer( const char * aBuffer )
	{
		// Очистка манипулятора.
		Reset();

		InfFunctionManipulator::SetBuffer( aBuffer );

		return Load();
	}

	/** Загружает реализацию функции. */
	InfEngineErrors Load();
};

#endif // INF_INTERNAL_FUNCTION_HPP
