/**
 * @file readwrite.h
 * @author Fedor Zorky
 *
 * @brief Функции считывания и записи данных через сокет с обработкой прерываний и таймаута.
 */

#ifndef __ReadWrite_h__
#define __ReadWrite_h__

#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/uio.h>

#ifdef  __cplusplus
extern "C"
{
    #endif

    /**
     *  Считывание из сокета указанного количества байт в буффер.
     * @param aFileDescriptor - дескриптор файла.
     * @param aBuffer - буфер, куда считывать данные.
     * @param aTargetSize - количество байт, которые надо считать.
     * @param aTimeOut - таймаут в микросекундах. Микросекунда = 10^{-6} секунды.
     *
     * @retval - Количество полученных байт; в случае ошибки устанавливает значение errno и возвращает -1; в случае
     *          прерывания по таймауту errno устанавливается в 0.
     */
    ssize_t nbread( int aFileDescriptor, void *aBuffer, size_t aTargetSize, unsigned int aTimeOut );


    /**
     *  Считывание из сокета указанного количества байт в набор буфферов.
     * @param aFileDescriptor - дескриптор файла.
     * @param iov - буфера, куда считывать данные.
     * @param iovcnt - количество буфферов.
     * @param aTimeOut - таймаут в микросекундах.  Микросекунда = 10^{-6} секунды.
     *
     * @retval - Количество полученных байт; в случае ошибки устанавливает значение errno и возвращает -1; в случае
     *          прерывания по таймауту errno устанавливается в 0.
     */
    ssize_t nbreadv( int aFileDescriptor, const struct iovec *iov, int iovcnt, unsigned int aTimeOut );


    /**
     *  Запись в сокет указанного количества байт из буффера.
     * @param aFileDescriptor - дескриптор файла.
     * @param aBuffer - буфер, откуда брать данные.
     * @param aTargetSize - количество байт, которые надо записать.
     * @param aTimeOut - таймаут в микросекундах.  Микросекунда = 10^{-6} секунды.
     *
     * @retval - Количество полученных байт; в случае ошибки устанавливает значение errno и возвращает -1; в случае
     *          прерывания по таймауту errno устанавливается в 0.
     */
    ssize_t nbwrite( int aFileDescriptor, const void *aBuffer, size_t aTargetSize, unsigned int aTimeOut );


    /**
     *  Запись в сокет указанного количества байт из набора буфферов.
     * @param aFileDescriptor - дескриптор файла.
     * @param iov - буфера, откуда брать данные.
     * @param iovcnt - количество буфферов.
     * @param aTimeOut - таймаут в микросекундах.  Микросекунда = 10^{-6} секунды.
     *
     * @retval - Количество полученных байт; в случае ошибки устанавливает значение errno и возвращает -1; в случае
     *          прерывания по таймауту errno устанавливается в 0.
     */
    ssize_t nbwritev( int aFileDescriptor, const struct iovec *iov, int iovcnt, unsigned int aTimeOut );

    #ifdef  __cplusplus
}
#endif

#endif  /** __ReadWrite_h__ */
