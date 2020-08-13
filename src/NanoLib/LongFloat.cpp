#include "LongFloat.hpp"

const unsigned int PRECISSION = 1024;

LongFloat::LongFloat( long int aNumber )
        :vValue( 0, PRECISSION )
{
    vValue = aNumber;
}

LongFloat::LongFloat( int aNumber )
        :vValue( 0, PRECISSION )
{
    vValue = aNumber;
}

LongFloat::LongFloat( double aNumber )
        :vValue( 0, PRECISSION )
{
    vValue = aNumber;
}

LongFloat::LongFloat( )
        :vValue( 0, PRECISSION )
{
}

LongFloat::LongFloat( const mpf_class & aNumber )
{
    this->SetF( aNumber );
}

LongFloat::LongFloat( const mpz_class & aNumber )
{
    this->SetZ( aNumber );
}

void LongFloat::Zero( )
{
    vValue = 0;
}

void LongFloat::Set( const LongFloat & aNumber )
{
    vValue = aNumber.vValue;
}

void LongFloat::SetF( const mpf_class & aNumber )
{
    vValue = aNumber;
}

void LongFloat::SetZ( const mpz_class & aNumber )
{
    vValue = aNumber;
}

LongFloat LongFloat::Add( const LongFloat & aSummand ) const
{
    LongFloat tmp;
    tmp.SetF( vValue + aSummand.vValue );
    return tmp;
}

LongFloat LongFloat::Sub( const LongFloat & aSubtrahend ) const
{
    LongFloat tmp;
    tmp.SetF( vValue - aSubtrahend.vValue );
    return tmp;
}

LongFloat LongFloat::Mult( const LongFloat & aFactor ) const
{
    LongFloat tmp;
    tmp.SetF( vValue * aFactor.vValue );
    return tmp;
}

LongFloat LongFloat::Div( const LongFloat & aFactor ) const
{
    LongFloat tmp;
	if( aFactor.vValue != 0 )
		tmp.SetF( vValue / aFactor.vValue );
	else
		tmp.SetF( 0 );
    return tmp;
}

LongFloat LongFloat::Mod( const LongFloat & aModule ) const
{
    mpz_class res( vValue );
    mpz_class tmp_mod( aModule.vValue );
    res %= tmp_mod;
    
    LongFloat rem;
    rem.SetZ( res );
    return rem;
}

LongFloat LongFloat::operator +( const LongFloat & aSummand ) const
{
    return Add( aSummand );
}

LongFloat & LongFloat::operator +=( const LongFloat & aSummand )
{
    vValue += aSummand.vValue;
    return *this;
}

LongFloat LongFloat::operator -( const LongFloat & aSubtrahend ) const
{
    return Sub( aSubtrahend );
}

LongFloat & LongFloat::operator -=( const LongFloat & aSubtrahend )
{
    vValue -= aSubtrahend.vValue;
    return *this;
}

LongFloat LongFloat::operator *( const LongFloat & aFactor ) const
{
    return Mult( aFactor );
}

LongFloat LongFloat::operator *( const long int & aFactor ) const
{
    return Mult( aFactor );
}

LongFloat LongFloat::operator *( const int & aFactor ) const
{
    return Mult( aFactor );
}

LongFloat LongFloat::operator *( const double & aFactor ) const
{
    return Mult( aFactor );
}

LongFloat & LongFloat::operator *=( const LongFloat & aFactor )
{
    vValue *= aFactor.vValue;
    return *this;
}

LongFloat & LongFloat::operator *=( const long int & aFactor )
{
    vValue *= aFactor;
    return *this;
}

LongFloat & LongFloat::operator *=( const int & aFactor )
{
    vValue *= aFactor;
    return *this;
}

LongFloat & LongFloat::operator *=( const double & aFactor )
{
    vValue *= aFactor;
    return *this;
}

LongFloat LongFloat::operator /( const LongFloat & aDivider ) const
{
    return Div( aDivider );
}

LongFloat LongFloat::operator /( const long int & aDivider ) const
{
    return Div( aDivider );
}

LongFloat LongFloat::operator /( const int & aDivider ) const
{
    return Div( aDivider );
}

LongFloat LongFloat::operator /( const double & aDivider ) const
{
    return Div( aDivider );
}
    
LongFloat & LongFloat::operator /=( const LongFloat & aDivider )
{
    vValue /= aDivider.vValue;
    return *this;
}

LongFloat & LongFloat::operator /=( const long int & aDivider )
{
    vValue /= aDivider;
    return *this;
}

LongFloat & LongFloat::operator /=( const int & aDivider )
{
    vValue /= aDivider;
    return *this;
}

LongFloat & LongFloat::operator /=( const double & aDivider )
{
    vValue /= aDivider;
    return *this;
}

LongFloat LongFloat::operator %( const LongFloat & aDivider ) const
{
    return Mod( aDivider );
}

LongFloat LongFloat::operator %( const long int & aDivider ) const
{
    return Mod( aDivider );
}
    
LongFloat LongFloat::operator %( const int & aDivider ) const
{
    return Mod( aDivider );
}
    
LongFloat LongFloat::operator %( const double & aDivider ) const
{
    return Mod( aDivider );
}

LongFloat & LongFloat::operator %=( const LongFloat & aDivider )
{
    *this = Mod( aDivider );
    return *this;
}

LongFloat & LongFloat::operator %=( const long int & aDivider )
{
    *this = Mod( aDivider );
    return *this;
}

LongFloat & LongFloat::operator %=( const int & aDivider )
{
    *this = Mod( aDivider );
    return *this;
}

LongFloat & LongFloat::operator %=( const double & aDivider )
{
    *this = Mod( aDivider );
    return *this;
}



LongFloat LongFloat::operator =( const LongFloat & aNew )
{
    vValue = aNew.vValue;
    return *this;
}

bool LongFloat::operator ==( const LongFloat & aNumber ) const
{
    return vValue == aNumber.vValue;
}

bool LongFloat::operator ==( const long int & aNumber ) const
{
    return vValue == aNumber;
}

bool LongFloat::operator ==( const int & aNumber ) const
{
    return vValue == aNumber;
}
 
bool LongFloat::operator ==( const double & aNumber ) const
{
    return vValue == aNumber;
}

bool LongFloat::operator <( const LongFloat & aNumber ) const
{
    return vValue < aNumber.vValue;
}

bool LongFloat::operator <( const long int & aNumber ) const
{
    return vValue < aNumber;
}

bool LongFloat::operator <( const int & aNumber ) const
{
    return vValue < aNumber;
}

bool LongFloat::operator <( const double & aNumber ) const
{
    return vValue < aNumber;
}

bool LongFloat::operator <=( const LongFloat & aNumber ) const
{
    return vValue <= aNumber.vValue;
}

bool LongFloat::operator <=( const long int & aNumber ) const
{
    return vValue <= aNumber;
}

bool LongFloat::operator <=( const int & aNumber ) const
{
    return vValue <= aNumber;
}

bool LongFloat::operator <=( const double & aNumber ) const
{
    return vValue <= aNumber;
}
 
bool LongFloat::operator >( const LongFloat & aNumber ) const
{
    return vValue > aNumber.vValue;
}

bool LongFloat::operator >( const long int & aNumber ) const
{
    return vValue > aNumber;
}

bool LongFloat::operator >( const int & aNumber ) const
{
    return vValue > aNumber;
}

bool LongFloat::operator >( const double & aNumber ) const
{
    return vValue > aNumber;
}

bool LongFloat::operator >=( const LongFloat & aNumber ) const
{
    return vValue >= aNumber.vValue;
}

bool LongFloat::operator >=( const long int & aNumber ) const
{
    return vValue >= aNumber;
}

bool LongFloat::operator >=( const int & aNumber ) const
{
    return vValue >= aNumber;
}

bool LongFloat::operator >=( const double & aNumber ) const
{
    return vValue >= aNumber;
}

LongFloat::operator double() const
{
    return vValue.get_d();
}

LongFloat::operator long double() const
{
    return vValue.get_d();
}

LongFloat::operator long int() const
{
    return vValue.get_si();
}

LongFloat::operator unsigned int() const
{
    return vValue.get_si();
}

LongFloat::operator int() const
{
    return vValue.get_si();
}

int LongFloat::snprintf( char * aBuffer, unsigned int aBufferSize, unsigned int aMaxFracLength ) const
{
    gmp_snprintf( aBuffer, aBufferSize,"%.*Ff", 50, vValue.get_mpf_t() );  
    unsigned int len = strlen( aBuffer );
    unsigned int real_len = len;
    unsigned int frac_len = 0;
    
    for( unsigned int pos = 0; pos < len; ++pos )
        if( aBuffer[len - pos - 1] == '.' || aBuffer[len - pos - 1] == ',' )
        {
            frac_len = pos;
            break;
        }
    
    //printf("\n!!! \"%s\" real_len=%i   frac_len=%i\n", aBuffer, real_len, frac_len);
    
    if( frac_len )
    {
        for( unsigned int pos = 0; pos < len; ++pos )
            if( aBuffer[len - pos - 1] == '0' )
            {
                --real_len;
                --frac_len;
            }
            else
                break;
        
        if( aBuffer[real_len - 1] == '.' || aBuffer[real_len - 1] == ',' )
        {
            --real_len;
            frac_len = 0;
        }
    
        if( !real_len )
        {
            aBuffer[0] = '0';
            aBuffer[1] = '\0';
            return 1;
        }
        
        if( frac_len > aMaxFracLength )
            real_len -= frac_len - aMaxFracLength;      
                
        aBuffer[real_len] = '\0';
                
    }   
    
    return real_len;
}

int LongFloat::sscanf( const char * aBuffer )
{
    return vValue.set_str( aBuffer, 10 ) == 0 ? 1 : 0;
}
