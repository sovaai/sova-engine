#ifndef __NumberConvertor_hpp__
#define	__NumberConvertor_hpp__

#include "aTextString.hpp"
#include "LongFloat.hpp"

class NumberConvertor
{
  public:
    /** Конструктор. */
    NumberConvertor( );


  public:

    typedef enum
    {
        lRU = 0,
        lEN = 1
    } LANGUAGE;


  private:
    bool Compare( const char * aWord, unsigned int aInd ) const;

    nlReturnCode Update( long int& aResult, int aUpdate ) const
    {
        return nlrcSuccess;
    }

    /**
     *  Установка языка.
     * @param aLang - новый язык.
     */
    nlReturnCode SetLang( LANGUAGE aLang ) const;


  private:
    /**
     *  Конвертация целого числа в прописную запись.
     * @param aNumber - исходное число.
     * @param aResult - контейнер для результата.
     */
    nlReturnCode int_i2s( long int aNumber, aTextString& aResult ) const;

    /**
     *  Конвертация дробного числа в прописную запись. При ковертации производится округление дробной части до сотых.
     * @param aNumber - исходное число.
     * @param aResult - контейнер для результата.
     */
    nlReturnCode int_d2s( LongFloat aNumber, aTextString& aResult ) const;


  public:
    /**
     *  Конвертация целого числа в прописную запись.
     * @param aNumber - исходное число.
     * @param aResult - контейнер для результата. Перед конвертацией очищается!
     * @param aLang - язык прописной записи.
     */
    nlReturnCode i2s( long int aNumber, aTextString& aResult, LANGUAGE aLang = lRU ) const;

    /**
     *  Конвертация дробного числа в прописную запись. При ковертации производится округление дробной части до сотых.
     * @param aNumber - исходное число.
     * @param aResult - контейнер для результата. Перед конвертацией очищается!
     * @param aLang - язык прописной записи.
     */
    nlReturnCode d2s( LongFloat aNumber, aTextString& aResult, LANGUAGE aLang = lRU ) const;

    /**
     *  Конвертация прописной записи целого числа в цифровую.
     * @param aNumber - прописное представление числа.
     * @param aResult - контейнер для результата.
     */
    nlReturnCode s2i( const char * aNumber, long int& aResult ) const;

    /**
     *  Конвертация прописной записи дробного числа в цифровую.
     * @param aNumber - прописное представление числа.
     * @param aResult - контейнер для результата.
     * @param aStringRep - флаг, показывающий, что исходное число было преставлено в прописной записи.
     */
    nlReturnCode s2lf( const char * aNumber, LongFloat& aResult, bool* aStringRep = nullptr ) const;
    

  private:
    const char * const * const vRU;
    const char * const * const vEN;
    mutable const char * const * vLang;


    mutable char vTmpBuffer[15];
};

#endif	/** __NumberConvertor_hpp__ */
