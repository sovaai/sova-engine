/**
 *  Статический аллокатор для выделения памяти при передаче данных между клиентской библиотекой и сервером InfServer.
 */

#ifndef __Allocator_h__
#define __Allocator_h__

#include "csDataPack.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 *  Выделение памяти под структуру обмена данных с помощью аллокатора памяти.
 * @param aAttrNumber - количество элементов в структуре.
 */
csdata_data_t * AllocateCSData( unsigned int aAttrNumber );

/** Сброс выделенной аллокатором памяти. */
void ClientLibAllocatorReset();

/** Получение размера выделенной аллокатором памяти. */
unsigned int GetAllocatedMemorySize();

/**
 *  Упаковка данных для передачи серверу.
 * @param aSource - данные для упаковки.
 * @param aDataPackFlags - флаги упаковки.
 */
csdata_data_t * ClientLibPackData( csdata_data_t * aSource, int aDataPackFlags );

/**
 *  Распаковка данных полученных от сервера.
 * @param aSource - данные полученные от сервера.
 */
csdata_data_t * ClientLibUnPackData( csdata_data_t * aSource );

#ifdef  __cplusplus
}
#endif

#endif  /** __Allocator_h__ */
