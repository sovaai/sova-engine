#ifndef __SignatureUtils_hpp__
#define __SignatureUtils_hpp__

#include <NanoLib/LogSystem.hpp>
#include <InfEngine2/_Include/Errors.h>
#include <NanoLib/nMemoryAllocator.hpp>
#include <cstdio>
#include "Signature.hpp"
#include <InfEngine2/InfEngine/DLData.hpp>


/**
 *  Сериализация сигнатуры. Результата - строка, содержащая только печатные символы.
 * @param aSignature - буфер с сигнатурой.
 * @param aSignatureSize - размер буфера с сигнатурой.
 * @param aStr - полученная в результате строка.
 * @param aLen - длинна строки с результатом в байтах.
 * @param aAllocator - менеджер памяти, использующийся для создания строки с результатом.
 */
InfEngineErrors SignatureToString( const char * aSignature, unsigned int aSignatureSize, char *& aStr, unsigned int & aLen, nMemoryAllocator & aAllocator );

/**
 *  Десериализация сигнатуры.
 * @param aStr - строка, полученная с помощью функции SignatureToString.
 * @param aLen - длина строки.
 * @param aSignature - буфер с полученной сигнатурой.
 * @param aSignatureSize - размер буфера с сигнатурой в байтах.
 * @param aAllocator - менеджер памяти, использующийся для создания сигнтуры.
 */
InfEngineErrors StringToSignature( const char * aStr, unsigned int aLen, const char *& aSignature, unsigned int & aSignatureSize, nMemoryAllocator & aAllocator );

/**
 * Создание сигнатуры для исполняемых фалов InfEngine.
 * @param aBuffer - буфер с результатом.
 * @param aSize - размер буфера в байтах.
 * @param aAllocator - менеджер памяти, с помощью которого будет размещён в памяти результат.
 */ 
InfEngineErrors BuildBinarySignature( char *& aBuffer, unsigned int & aSize, nMemoryAllocator & aAllocator );

/**
 * Вывод на экран сигнатуры для исполняемых файлов.
 * @param aBuffer - буфер с сигнатурой.
 * @param out - дескриптор для вывода.
 */
InfEngineErrors PrintBinarySignature( const char * aBuffer, FILE * out = stdout );

/**
 * Создание сигнатуры для лингвистических данных.
 * @param aSignature - построенная сигнатура.
 * @param aDLDataWR - скомпилированные лингвистические данные.
 * @param aAllocator - менеджер памяти, с помощью которого будет размещён в памяти результат.
 */
InfEngineErrors BuildDLDataSignature( Signature *& aSignature, const DLDataWR & aDLDataWR, nMemoryAllocator & aAllocator );

/**
 * Вывод на экран сигнатуры для лингвистических данных.
 * @param aBuffer - буфер с сигнатурой.
 * @param out - дескриптор для вывода.
 */
InfEngineErrors PrintDLDataSignature( const char * aBuffer, FILE * out = stdout );

/**
 *  Проверка совместимости сигнатуры для исполняемых файлов и сигнатуры для лингвистических данных.
 * В случае неуспеха возвращает INF_ENGINE_WARN_UNSUCCESS и сообщение, описывающее результат сравнения сигнатур.
 * @param aBinarySignatureBuffer - буфер с сигнатурой для исполняемых файлов.
 * @param aDLSignatureBuffer - буфер с сигнатурой для лингвистических данных.
 * @param aResultInfo - сообщение, описывающее результат сравнения сигнатур.
 */
InfEngineErrors CompareSignatures( const char * aBinarySignatureBuffer, const char * aDLSignatureBuffer, aTextString & aResultInfo );

/**
 *  Получение сигнатуры у InfServer.
 * @param aInfServerPath - путь к исполняемому файлу InfServer.
 * @param aSignature - буфер для записи сигнатуры.
 * @param aSignatureSize - получившийся размер буфера.
 * @param aAllocator - менеджер памяти, с помощью которого будет выделена память под буфер для сигнатуры.
 */
InfEngineErrors GetInfServerSignature( const char * aInfServerPath, const char *& aSignature, unsigned int & aSignatureSize, nMemoryAllocator & aAllocator );


/**
 *  Проверка сигнатуры лингвистических данных на предмет наличия необходимых DL-функций.
 * В случае неуспеха возвращает INF_ENGINE_WARN_UNSUCCESS и сообщение, описывающее результат проверки.
 * @param aBinarySignatureBuffer - буфер с сигнатурой для исполняемых файлов.
 * @param aDLSignatureBuffer - буфер с сигнатурой лингвистических данных.
 * @param aFuncRegistry - реестр функций.
 * @param aResultInfo - сообщение, описывающее результат сравнения сигнатур.
 */
InfEngineErrors CheckFunctions( const char * aBinarySignatureBuffer, const char * aDLSignatureBuffer, const FunctionsRegistryWR * aFuncRegistry, aTextString & aResultInfo );
#endif // __SignatureUtils_hpp__
