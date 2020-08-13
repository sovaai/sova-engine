#ifndef __nMemoryAllocator_h__
#define __nMemoryAllocator_h__

#ifdef  __cplusplus
extern "C"
{
#endif

/** Определение структуры, которая будет исполнять роль аллокатора. */
typedef struct __ncMemoryAllocator ncMemoryAllocator;

/** Создание нового аллокатора. */
ncMemoryAllocator* ncMemoryAllocatorCreate();

/**
 *  Удаление существующего аллокатора.
 * @param aMemoryAllocator - существующий аллокатор.
 */
void ncMemoryAllocatorDestroy( ncMemoryAllocator* aMemoryAllocator );

/**
 *  Выделение памяти с помощью аллокатора.
 * @param aMemoryAllocator - существующий аллокатор.
 * @param aTargetMemorySize - необходимый размер памяти.
 */
void* AllocatorAllocate( ncMemoryAllocator* aAllocator, size_t aTargetMemorySize );

/**
 *  Очистка памяти, выделенной аллокатором.
 * @param aMemoryAllocator - существующий аллокатор.
 */
void ncMemoryAllocatorReset( ncMemoryAllocator* aMemoryAllocator );

/**
 *  Освобождение памяти, выделенной аллокатором.
 * @param aMemoryAllocator - существующий аллокатор.
 */
void ncMemoryAllocatorFree( ncMemoryAllocator* aMemoryAllocator );

#ifdef  __cplusplus
}
#endif

#endif  /** __nMemoryAllocator_h__ */
