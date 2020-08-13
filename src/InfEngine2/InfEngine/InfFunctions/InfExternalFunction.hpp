#ifndef INF_EXTERNAL_FUNCTION_HPP
#define INF_EXTERNAL_FUNCTION_HPP

#include "InfFunction.hpp"

/**
 *  Класс для работы с внешними функциями языка DL.
 */
class ExternalInfFunction : public InfFunction
{
public:
	/** Конструктор класса ExternalInfFunction. */
	ExternalInfFunction() : InfFunction( DLFT_EXTERNAL ) {}

	ExternalInfFunction( const InfFunction & aFunction ) = delete;

	/** Деструктор класса ExternalInfFunction. */
	virtual ~ExternalInfFunction() { Reset(); }

public:
	ExternalInfFunction& operator=( const InfFunction & aFunction ) = delete;

public:
	/**
	 *  Загрузка информации о функции из динамической библиотеки.
	 * @param aFilePath - адрес .so файла.
	 */
	InfEngineErrors Load( const char * aFilePath, nMemoryAllocator & aMemoryAllocator );

	/** Получение необходимого размера памяти для сохранения объекта. **/
	virtual unsigned int GetNeedMemorySize() const
	{
		return InfFunction::GetNeedMemorySize() + sizeof( uint32_t ) + vLibPathLength + 1 + 2 * sizeof( DLFunctionVersion );
	}

	/** Сохранение данных в буффер. **/
	virtual InfEngineErrors Save( char * aBuffer, unsigned int aBufferSize, unsigned int & aResultSize ) const;

private:
	/** Инициализация полей объекта. */
	void Init();

	/** Освобождение выделенных ресурсов. */
	void Reset();

private:
	/** Адрес динамической библиотеки, содержащей функцию. */
	char * vLibPath { nullptr };

	/** Дилина адреса библиотеки. */
	unsigned int vLibPathLength { 0 };

	/** Хэндлер динамической библиотеки, содержащий функцию. */
	void * vDLHandler { nullptr };
};


/** Класс для манипуляции образом внешней функции в памяти. */
class ExternalInfFunctionManipulator : public InfFunctionManipulator
{
public:
	ExternalInfFunctionManipulator() { Init(); }

	ExternalInfFunctionManipulator( const char * aBuffer ) { Init(); SetBuffer( aBuffer ); }

	ExternalInfFunctionManipulator( const InfFunctionManipulator & aFunction ) = delete;

	~ExternalInfFunctionManipulator() { Reset(); }

public:
	ExternalInfFunctionManipulator& operator=( const InfFunctionManipulator & aFunction ) = delete;

public:
	/** Инициализация переменных. */
	void Init() { InfFunctionManipulator::Reset(); vFunction = nullptr; }

	/** Освобождает ресурсы. */
	virtual void Reset();

	/** Загружает из памяти информацию о функции. */
	virtual InfEngineErrors SetBuffer( const char * aBuffer );

	/** Загружает реализацию функции в память. */
	InfEngineErrors Load( const char * aFunctionPath );

	/** Получение размера памяти, занятой объектом. **/
	virtual unsigned int GetMemorySize() const
	{
		return vBuffer
			   ? InfFunctionManipulator::GetMemorySize() + sizeof( uint32_t ) + GetPathLength() + 1 + 2 * sizeof( DLFunctionVersion )
			   : 0;
	}

	/** Возвращает длину пути к динамической библиотеке. **/
	unsigned int GetPathLength() const
	{
		return vBuffer ? *reinterpret_cast<const uint32_t*>( vBuffer + InfFunctionManipulator::GetMemorySize() ) : 0;
	}

	/** Возвращает путь к динамической библиотеке. **/
	const char * GetLibPath() const
	{
		return vBuffer ? vBuffer +  sizeof( uint32_t ) + InfFunctionManipulator::GetMemorySize() : nullptr;
	}

	/** Возвращает версию функции. **/
	const DLFunctionVersion * GetCurVersion() const
	{
		return vBuffer
			   ? reinterpret_cast<const DLFunctionVersion*>( vBuffer +
															 sizeof( uint32_t ) +
															 GetPathLength() +
															 1 +
															 InfFunctionManipulator::GetMemorySize() )
			   : nullptr;
	}

	/** Возвращает минимальную версию функции, совместимой с функцией текущей версии. **/
	const DLFunctionVersion * GetMinVersion() const
	{
		return vBuffer
			   ? reinterpret_cast<const DLFunctionVersion*>( vBuffer +
															 sizeof( uint32_t ) +
															 GetPathLength() +
															 1 +
															 sizeof( DLFunctionVersion ) +
															 InfFunctionManipulator::GetMemorySize() )
			   : nullptr;
	}

private:
	void * vDLHandler { nullptr };

};

#endif // INF_EXTERNAL_FUNCTION_HPP
