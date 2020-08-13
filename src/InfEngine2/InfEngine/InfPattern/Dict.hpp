#ifndef __Dict_hpp__
#define __Dict_hpp__

#include <InfEngine2/_Include/Errors.h>

/**
 *  Представление словаря.
 */
class InfPatternDict
{
    private:
        InfPatternDict();


    public:
        // Вычисление объема памяти, необходимого для сохранения словаря.
        unsigned int GetNeedMemorySize();

        // Сохранение словаря.
        InfEngineErrors Save( char* aBuffer, unsigned int aBufferSize, unsigned int& aResultSize );


    private:
        // Идентификатор словаря.
        unsigned int vID;

        // Имя словаря.
        const char * vName;

        /**
         *  Флаг, показывающий есть ли в словаре пустая строка. Такая строка не
         * отображена в списке строк словаря.
         */
        bool vHasEmptyString;

        // Флаг, показывающий является ли данный словарь inline-словарем.
        bool vInlineDict;

        // Количество строк в словаре.
        unsigned int vStringsNumber;

        // Размер памяти, необходимой для сохранения словаря.
        unsigned int vNeedMemorySize;
};

#endif  /** __Dict_hpp__ */
