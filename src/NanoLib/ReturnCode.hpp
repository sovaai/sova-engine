#ifndef __NanoLibReturnCode_hpp__
#define __NanoLibReturnCode_hpp__

/**
 *  Коды возврата ошибок функций библиотеки NanoLib.
 */
typedef enum nlReturnCode
{
    /* Конец файла. */
    nlrcEOF               =  100,

    /** Ошибки нет, но операция не успешна. */
    nlrcUnsuccess         =  1,

    /** Успех. */
    nlrcSuccess = 0,

    /**  Функции были переданы некорректные аргументы. */
    nlrcErrorInvArgs      = -1,

    /**  Объект не инициализирован или инициализированн некорректно. */
    nlrcErrorStatus       = -2,
    
    /** Ошибка работы с памятью - не могу выделить нужный объем памяти. */
    nlrcErrorNoFreeMemory = -3,

    /** Ошибка при попытке открыть файл. */
    nlrcErrorFileCantOpen = -4,

    /** Обшика разбора ini-файла. */
    nlrcErrorParse        = -5,

    /** Ошибка операции чтения. */
    nlrcErrorRead         = -6,
    
    /** Размер буфера недостаточен. */
    nlrcErrorBufferTooSmall = -7,
            
    /** Непредвиденная ошибка. */
    nlrcErrorInternal = -7,
} nlReturnCode;

inline const char * nlReturnCodeToString( nlReturnCode aReturnCode )
{
    switch( aReturnCode )
    {
        case nlrcEOF:
            return "End of file";
        case nlrcUnsuccess:
            return "Unsuccess";
        case nlrcSuccess:
            return "Success";
        case nlrcErrorInvArgs:
            return "Invalid arguments";
        case nlrcErrorStatus:
            return "Invalid status";
        case nlrcErrorNoFreeMemory:
            return "Can't allocate memory";
        case nlrcErrorFileCantOpen:
            return "Can't open file";
        case nlrcErrorParse:
            return "Parse error";
        case nlrcErrorRead:
            return "Can't read";
        case nlrcErrorBufferTooSmall:
            return "Buffer too small";
    	default:
    		return "Unknown fault";
    }
}

#endif /** __NanoLibReturnCode_hpp__*/
