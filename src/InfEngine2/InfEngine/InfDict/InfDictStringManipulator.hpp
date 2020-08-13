#ifndef INF_DICT_STRING_MANIPULATOR_HPP
#define INF_DICT_STRING_MANIPULATOR_HPP

#include "../InfPattern/Items.hpp"
#include "../PatternsStorage.hpp"

class DLDataRO;

/**
 *  Класс для доступа к шаблон-словарю.
 */
class InfDictStringManipulator
{
public:
	/** Конструктор. */
	InfDictStringManipulator() { vBuffer = nullptr; }


public:
	/** Инициализация массива. **/
	InfEngineErrors Attach( const char * aBuffer ) { vBuffer = aBuffer; return vBuffer ? INF_ENGINE_SUCCESS : INF_ENGINE_ERROR_INV_ARGS; }

	/** Привязка к базе, содержащей хранилище с данной строкой. **/
	void SetDLData( const DLDataRO * aDLDataRO ) { vDLDataRO = aDLDataRO; }
	
public:
	/** Получение общего объема памяти, занимаемой шаблоном. **/
	unsigned int GetMemorySize() const { return vBuffer ? ( (uint32_t*)vBuffer )[0] : 0; }

	/** Получение идентификатора, выданного хранилищем шаблонов. **/
	unsigned int GetId() const { return vBuffer ? ( (uint32_t*)vBuffer )[1] : static_cast<unsigned int>( -1 ); }

	/** Получение длины оригинальной строки шаблон-словаря. **/
	unsigned int GetOriginalLength() const
	{
		return vBuffer ? ( (uint16_t*)( vBuffer + 2 * sizeof( uint32_t ) ) )[0] : static_cast<unsigned int>( -1 );
	}				

	/** Получение ссылки на оригинальную строку шаблон-словаря. **/
	const char * GetOriginal() const { return vBuffer ? vBuffer + 2 * sizeof( uint32_t ) + sizeof( uint16_t ) : nullptr; }
	
	
	/** Получение строки словаря из хранилища. **/
	const PatternsStorage::Pattern Get() const;

private:
	const char * vBuffer { nullptr };
	
	const DLDataRO * vDLDataRO { nullptr };
};

#endif /** INF_DICT_STRING_MANIPULATOR_HPP */
