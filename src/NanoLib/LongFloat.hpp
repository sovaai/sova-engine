#ifndef __LongFloat_hpp__
#define	__LongFloat_hpp__

#include <cstddef>
#include <gmpxx.h>

#include "ReturnCode.hpp"

/**
 *
 *  Вещественное число заданоой точности.
 *
 */

class LongFloat
{
  public:
    /** Конструктор. */
    LongFloat( );

    /**
     *  Конструктор.
     * @param aNumber - значение числа.
     */
    LongFloat( long int aNumber );

    /**
     *  Конструктор.
     * @param aNumber - значение числа.
     */
    LongFloat( int aNumber );

    /**
     *  Конструктор.
     * @param aNumber - значение числа.
     */
    LongFloat( double aNumber );
    
  private:
      /**
       *  Конструктор.
       * @param aNumber - значение числа.
       */
      LongFloat( const mpf_class & aNumber );
      
      /**
       *  Конструктор.
       * @param aNumber - значение числа.
       */
      LongFloat( const mpz_class & aNumber );

  public:
    /** Обнуление числа. */
    void Zero( );

    /**
     *  Установка значения числа.
     * @param aNumber - число.
     */
    void Set( const LongFloat & aNumber );
    
  private:
    /**
     *  Установка значения числа.
     * @param aNumber - число.
     */
    void SetF( const mpf_class & aNumber );  
    
    /**
     *  Установка значения числа.
     * @param aNumber - число.
     */
    void SetZ( const mpz_class & aNumber );  
    
    
  public:
    /**
     *  Прибавление.
     * @param aSummand - слогаемое.
     */
    LongFloat Add( const LongFloat & aSummand ) const;

    /**
     *  Вычитание.
     * @param aSubtrahend - вычитаемое.
     */
    LongFloat Sub( const LongFloat & aSubtrahend ) const;

    /**
     *  Умножение.
     * @param aFactor - множитель.
     */
    LongFloat Mult( const LongFloat & aFactor ) const;

    /**
     *  Деление.
     * @param aDivider - делитель.
     */
    LongFloat Div( const LongFloat & aDivider ) const;
    
    /**
     *  Остаток от деления после приведения числа к целому.
     * @param aModule - основание.
     */
    LongFloat Mod( const LongFloat & aModule ) const;


  public:
    /**
     *  Сложение.
     * @param aSummand - слогаемое.
     */
    LongFloat operator +( const LongFloat & aSummand ) const;
    LongFloat & operator +=( const LongFloat & aSummand );

    /**
     *  Вычитание.
     * @param aSubtrahend - вычитаемое.
     */
    LongFloat operator -( const LongFloat & aSubtrahend ) const;
    LongFloat & operator -=( const LongFloat & aSubtrahend );

    /**
     *  Умножение.
     * @param aFactor - множитель.
     */
    LongFloat operator *( const LongFloat & aFactor ) const;
    LongFloat operator *( const long int & aFactor ) const;
    LongFloat operator *( const int & aFactor ) const;
    LongFloat operator *( const double & aFactor ) const;
    LongFloat & operator *=( const LongFloat & aFactor );
    LongFloat & operator *=( const long int & aFactor );
    LongFloat & operator *=( const int & aFactor );
    LongFloat & operator *=( const double & aFactor );

    /**
     *  Деление.
     * @param aDivider - делитель.
     */
    LongFloat operator /( const LongFloat & aDivider ) const;
    LongFloat operator /( const long int & aDivider ) const;
    LongFloat operator /( const int & aDivider ) const;
    LongFloat operator /( const double & aDivider ) const;
    LongFloat & operator /=( const LongFloat & aDivider );
    LongFloat & operator /=( const long int & aDivider );
    LongFloat & operator /=( const int & aDivider );
    LongFloat & operator /=( const double & aDivider );
    
    /**
     *  Деление по модулю.
     * @param aDivider - делитель.
     */
    LongFloat operator %( const LongFloat & aDivider ) const;
    LongFloat operator %( const long int & aDivider ) const;
    LongFloat operator %( const int & aDivider ) const;
    LongFloat operator %( const double & aDivider ) const;
    LongFloat & operator %=( const LongFloat & aDivider );
    LongFloat & operator %=( const long int & aDivider );
    LongFloat & operator %=( const int & aDivider );
    LongFloat & operator %=( const double & aDivider );

    /**
     *  Присвоение значения.
     * @param aNew - новое значение.
     */
    LongFloat operator =( const LongFloat & aNew );
    
    
  public:
    /**
     *  Сравнение.
     * @param aNumber - операнд.
     */
    bool operator ==( const LongFloat & aNumber ) const;
    bool operator ==( const long int & aNumber ) const;
    bool operator ==( const int & aNumber ) const;
    bool operator ==( const double & aNumber ) const;
    
    /**
     *  Сравнение.
     * @param aNumber - операнд.
     */
    bool operator <( const LongFloat & aNumber ) const;
    bool operator <( const long int & aNumber ) const;
    bool operator <( const int & aNumber ) const;
    bool operator <( const double & aNumber ) const;
    bool operator <=( const LongFloat & aNumber ) const;
    bool operator <=( const long int & aNumber ) const;
    bool operator <=( const int & aNumber ) const;
    bool operator <=( const double & aNumber ) const;
    
    /**   
     *  Сравнение.
     * @param aNumber - операнд.
     */
    bool operator >( const LongFloat & aNumber ) const;
    bool operator >( const long int & aNumber ) const;
    bool operator >( const int & aNumber ) const;
    bool operator >( const double & aNumber ) const;
    bool operator >=( const LongFloat & aNumber ) const;
    bool operator >=( const long int & aNumber ) const;
    bool operator >=( const int & aNumber ) const;
    bool operator >=( const double & aNumber ) const;
    
    
  public:
      
    /**
     *  Приведение к типу double.
     */
    operator double() const;
    
    /**
     *  Приведение к типу long double.
     */
    operator long double() const;
    
    /**
     *  Приведение к типу long int.
     */
    operator long int() const;
    
    /**
     *  Приведение к типу unsigned int.
     */
    operator unsigned int() const;
    
    /**
     *  Приведение к типу int.
     */
    operator int() const;

  public:
    /**
     *  Печать числа в буфер.
     * @param aBuffer - буфер.
     * @param aBufferSize - размер буффера.
     * @param aMaxFracLength - максимальное число знаков дробной части, которые нужно печатать.
     */
    int snprintf( char* aBuffer, unsigned int aBufferSize, unsigned int aMaxFracLength ) const;
    
    /**
     *  Чтение числа из буфера.
     * @param aBuffer - буфер.
     */
    int sscanf( const char * aBuffer );


  private:
    /** Значение числа. */
    mpf_class vValue;
};

#endif	/** __LongFloat_hpp__ */
