#ifndef __InfFunction_hpp__
#define __InfFunction_hpp__

#include <InfEngine2/Functions/FunctionLib.hpp>

/**
 *  Базовый класс для работы с функциями языка DL.
 */
class InfFunction
{
public:
	/** Конструктор класса InfFunction. */
	InfFunction( DLFunctionType aDLFunctionType ) : vDLFunctionType( aDLFunctionType ) {}

	/** Конструктор копирования запрещен. */
	InfFunction( const InfFunction & aFunction ) = delete;


public:
	/** Копирование запрещено. */
	InfFunction& operator=( const InfFunction & aFunction ) = delete;


public:
	/** Освобождение выделенных ресурсов. */
	void Reset() { vNameLength = 0; vDLFunctionInfo = nullptr; }


public:
	/** Получение необходимого размера памяти для сохранения объекта. **/
	virtual unsigned int GetNeedMemorySize() const { return sizeof( uint8_t ) + sizeof( uint32_t ) + vNameLength + 1; }

	/** Сохранение данных в буффер. **/
	virtual InfEngineErrors Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aResultSize ) const;


public:
	/** Возвращает тип реализации функции. */
	const DLFunctionType & GetDLFunctionType() const { return vDLFunctionType; }

	/** Возвращает информацию о функции. */
	const DLFunctionInfo * GetInfo() const { return vDLFunctionInfo ? vDLFunctionInfo : nullptr; }

	/**
	 *  Возвращает описание заданного аргумента.
	 * @param arg_n - номер аргумента.
	 */
	const DLFucntionArgInfo * GetArgInfo( unsigned int arg_n ) const
	{
		return vDLFunctionInfo && arg_n < vDLFunctionInfo->argc ? &vDLFunctionInfo->argt[arg_n] : nullptr;
	}


protected:
	/** Тип реализации функции. */
	DLFunctionType vDLFunctionType;

	/** Длина имени функции. */
	unsigned int vNameLength { 0 };

	/** Указатель на структуру с описанием функции. */
	const DLFunctionInfo * vDLFunctionInfo { nullptr };
};


/** Класс для манипуляции образом функции в памяти. */
class InfFunctionManipulator
{
public:
	InfFunctionManipulator() {}

	InfFunctionManipulator( const char * aBuffer ) { SetBuffer( aBuffer ); }

	InfFunctionManipulator( const InfFunctionManipulator & aFunction ) = delete;


public:
	InfFunctionManipulator& operator=( const InfFunctionManipulator & aFunction ) = delete;


public:
	/** Освобождает ресурсы. */
	virtual void Reset() { vBuffer = nullptr; vFunction = nullptr; vDLFunctionInfo = nullptr; }


public:
	/** Загружает из памяти информацию о функции. */
	virtual InfEngineErrors SetBuffer( const char * aBuffer )
	{
		Reset();
		vBuffer = aBuffer;
		return INF_ENGINE_SUCCESS;
	}


public:
	/** Возвращает тип реализации функции. */
	DLFunctionType GetDLFunctionType() const
	{
		return vBuffer
			   ? static_cast<DLFunctionType>( *reinterpret_cast<const uint8_t*>( vBuffer ) )
			   : DLFT_UNKNOWN;
	}

	/** Возвращает длину имени функции. */
	unsigned int GetNameLength() const
	{
		return vBuffer ? *reinterpret_cast<const uint32_t*>( vBuffer + sizeof( uint8_t ) ) : 0;
	}

	/** Получение размера памяти, занятой объектом. **/
	virtual unsigned int GetMemorySize() const
	{
		return vBuffer ? sizeof( uint8_t ) + sizeof( uint32_t ) + GetNameLength() + 1 : 0;
	}

	/** Возвращает имя функции. */
	const char * GetName() const
	{
		return vBuffer ? vBuffer + sizeof( uint32_t ) + sizeof( uint8_t ) : nullptr;
	}

	/** Возвращает адрес реализации функции. **/
	const FDLFucntion GetFunciton() const { return vFunction; }

	/** Возвращает информацию о функции. */
	const DLFunctionInfo * GetInfo() const { return vDLFunctionInfo; }

	/**
	 *  Возвращает описание заданного аргумента.
	 * @param arg_n - номер аргумента.
	 */
	const DLFucntionArgInfo * GetArgInfo( unsigned int arg_n ) const
	{
		return vDLFunctionInfo && arg_n < vDLFunctionInfo->argc ? &vDLFunctionInfo->argt[arg_n] : nullptr;
	}


protected:
	const char * vBuffer { nullptr };

	FDLFucntion vFunction { nullptr };

	const DLFunctionInfo * vDLFunctionInfo { nullptr };
};

#endif
