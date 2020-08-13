# if !defined( __array_h__ )
# define  __array_h__

# include <stdlib.h>
# include <string.h>
# include <assert.h>
# include <errno.h>

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 4514 4710 )
# endif // _MSC_VER

struct  __the_array_element_ptr
{
};

inline  void* operator new ( size_t, __the_array_element_ptr* lpdata )
  {  return lpdata;  }

// Common destruction methods
template <class A>
inline  void  __safe_array_destruct( A* lplist, int  lcount )
  {  for ( ; lcount-- > 0; lplist++ ) lplist->~A(); }
template <>
inline  void  __safe_array_destruct( char*, int ) {}
template <>
inline  void  __safe_array_destruct( unsigned char*, int ) {}
template <>
inline  void  __safe_array_destruct( short*, int ) {}
template <>
inline  void  __safe_array_destruct( unsigned short*, int ) {}
template <>
inline  void  __safe_array_destruct( int*, int ) {}
template <>
inline  void  __safe_array_destruct( unsigned int*, int ) {}
template <>
inline  void  __safe_array_destruct( long*, int ) {}
template <>
inline  void  __safe_array_destruct( unsigned long*, int ) {}
template <>
inline  void  __safe_array_destruct( float*, int ) {}
template <>
inline  void  __safe_array_destruct( double*, int ) {}
template <>
inline  void  __safe_array_destruct( char**, int ) {}
template <>
inline  void  __safe_array_destruct( unsigned char**, int ) {}
template <>
inline  void  __safe_array_destruct( short**, int ) {}
template <>
inline  void  __safe_array_destruct( unsigned short**, int ) {}
template <>
inline  void  __safe_array_destruct( int**, int ) {}
template <>
inline  void  __safe_array_destruct( unsigned int**, int ) {}
template <>
inline  void  __safe_array_destruct( long**, int ) {}
template <>
inline  void  __safe_array_destruct( unsigned long**, int ) {}
template <>
inline  void  __safe_array_destruct( float**, int ) {}
template <>
inline  void  __safe_array_destruct( double**, int ) {}

// Common construction methods
template <class A>
inline  void  __safe_array_construct_def( A* p, int c )
  {
    bzero( p, c * sizeof(A) );
    while ( c-- > 0 )
      new( (__the_array_element_ptr*)(p++) )A;
  }
template <>
inline  void  __safe_array_construct_def( unsigned long* p, int c )
  {  bzero( p, c * sizeof(unsigned long) );  }
template <>
inline  void  __safe_array_construct_def( unsigned int* p, int c )
  {  bzero( p, c * sizeof(unsigned int) );  }
template <>
inline  void  __safe_array_construct_def( unsigned short* p, int c )
  {  bzero( p, c * sizeof(unsigned short) );  }
template <>
inline  void  __safe_array_construct_def( unsigned char* p, int c )
  {  bzero( p, c * sizeof(unsigned char) );  }
template <>
inline  void  __safe_array_construct_def( long* p, int c )
  {  bzero( p, c * sizeof(long) );  }
template <>
inline  void  __safe_array_construct_def( int* p, int c )
  {  bzero( p, c * sizeof(int) );  }
template <>
inline  void  __safe_array_construct_def( short* p, int c )
  {  bzero( p, c * sizeof(short) );  }
template <>
inline  void  __safe_array_construct_def( char* p, int c )
  {  bzero( p, c * sizeof(char) );  }
template <>
inline  void  __safe_array_construct_def( double* p, int c )
  {  bzero( p, c * sizeof(double) );  }
template <>
inline  void  __safe_array_construct_def( float* p, int c )
  {  bzero( p, c * sizeof(float) );  }
// Copy construction methods
template <class A, class B>
inline  void  __safe_array_construct_cpy( A* p, B r )
  {
    new( (__the_array_element_ptr*)p )A( r );
  }
template <>
inline  void  __safe_array_construct_cpy( unsigned long* p, unsigned long r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( unsigned int* p, unsigned int r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( unsigned short* p, unsigned short r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( unsigned char* p, unsigned char r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( long* p, long r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( int* p, int r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( short* p, short r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( char* p, char r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( double* p, double r )
  {  *p = r;  }
template <>
inline  void  __safe_array_construct_cpy( float* p, float r )
  {  *p = r;  }

template <class T, class R>
class array
{
// forbidden copy operator
  array<T, R>& operator = ( const array<T, R>& )
    {
      assert( false );
      return *this;
    }
public:
        array( int adelta = 0x10 );
        array( const array<T,R>& );
       ~array();
  int   Copy( const array<T,R>& );
  int   Append( R );
  int   Append( int, T* );
  void  Clear();
  int   Delete( int );
  int   Insert( int, R );
  int   Insert( int, int, T* );
  int   GetLen() const;
  int   SetLen( int );
  bool  Search( R rfitem, int& ) const;
  bool  Search( R rfitem, int&, int (*compare)( T&, T& ) ) const;
  bool  LinearSearch( R rfitem, int& );
  bool  LinearSearch( R rfitem, int&, int (*compare)( T&, T& ) );
  void  Resort( int (*compare)( T&, T& ) );
  void  MergeResort( int (*compare)( T&, T& ) );
  operator        T* ();
  operator const  T* () const;
  T&    operator [] ( int );
  const T&  operator [] ( int ) const;
  T&    operator [] ( unsigned int );
  const T&  operator [] ( unsigned int ) const;

  T& Back();
  const T& Back() const;

  int   Limit  () const;
  int   Reserve( int );         // allocate some space
  void  SetGran( int );         // customize the granularity
protected:
  T*    pitems;
  int   ncount;
  int   nlimit;
  int   ndelta;
};

template <class T, class R>
inline  array<T, R>::array( int adelta ):
                                pitems( 0 ),
                                ncount( 0 ),
                                nlimit( 0 ),
                                ndelta( adelta <= 0 ? 0x10 : adelta )
{
}

template <class T, class R>
inline array<T, R>::array( const array<T,R>& r )
  : pitems( 0 ),
    ncount( 0 ),
    nlimit( 0 ),
    ndelta( r.ndelta )
{
  if ( r.ncount > 0 )
  {
    Reserve( r.ncount );
    Insert( 0, r.ncount, const_cast<T*>(static_cast<const T*>(r)) );
  }
}

template <class T, class R>
inline  array<T, R>::~array()
{
  if ( pitems )
  {
    if ( ncount )
      __safe_array_destruct( pitems, ncount );
    free( pitems );
  }
}

template <class T, class R>
inline  int   array<T, R>::Copy( const array<T,R>& refsrc )
{
  __safe_array_destruct( pitems, ncount );
    ncount = 0;
  if ( refsrc.ncount > 0 )
  {
    T*    lpfill;
    T*    lpread;

  // reserve extra space
    if ( Reserve( refsrc.ncount ) != 0 )
      return ENOMEM;

  // copy source data
    lpfill = pitems;
    lpread = refsrc.pitems;
    while ( ncount++ < refsrc.ncount )
      __safe_array_construct_cpy( lpfill++, *lpread++ );

  // set the same memory atomic
    ndelta = refsrc.ndelta;
  }
  return 0;
}

template <class T, class R>
inline  int   array<T, R>::Append( R rfitem )
{
  return Insert( ncount, rfitem );
}

template <class T, class R>
inline  int   array<T, R>::Append( int acount, T* lplist )
{
  return Insert( ncount, acount, lplist );
}

template <class T, class R>
inline  void  array<T, R>::Clear()
{
  if ( ncount > 0 )
    __safe_array_destruct( pitems, ncount );
  ncount = 0;
}

template <class T, class R>
inline  int   array<T, R>::Insert( int nindex, R  rfitem )
{
// Check if valid arguments passed
  if ( nindex < 0 || nindex > ncount )
    return EINVAL;

  assert( ncount <= nlimit );

// Ensure enough space
  if ( ncount == nlimit )
  {
    int   newlimit = nlimit + ndelta;
    T*    newitems;

    assert( newlimit > nindex );

  // Allocate new space
    if ( (newitems = (T*)realloc( pitems, newlimit * sizeof(T) )) == NULL )
      return ENOMEM;
    pitems = newitems;
    nlimit = newlimit;
  }

// Check if the space would be prepared
  if ( nindex < ncount )
    memmove( pitems + nindex + 1, pitems + nindex, (ncount - nindex)
      * sizeof(T) );

// Create the element with the copy constructor
  __safe_array_construct_cpy( pitems + nindex, rfitem );
  ++ncount;
  return 0;
}

template <class T, class R>
inline  int   array<T, R>::Insert( int nindex, int  acount, T*  lplist )
{
  int   nerror;

  while ( acount-- > 0 )
    if ( (nerror = Insert( nindex++, *lplist++ )) != 0 )
      return nerror;
  return 0;
}

template <class T, class R>
inline  int   array<T, R>::Delete( int nindex )
{
  if ( nindex < 0 || nindex >= ncount )
    return EINVAL;
  else __safe_array_destruct( pitems + nindex, 1 );
  if ( nindex < --ncount )
    memmove( pitems + nindex, pitems + nindex + 1, (ncount - nindex)
      * sizeof(T) );
  return 0;
}

template <class T, class R>
inline  int   array<T, R>::GetLen() const
{
  return ncount;
}

template <class T, class R>
inline  int   array<T, R>::SetLen( int length )
{
  if ( length < 0 )
    return EINVAL;
  if ( length < ncount )
    __safe_array_destruct( pitems + length, ncount - length );
  if ( length > ncount )
  {
    if ( length > nlimit )
    {
      int   newlimit = nlimit + ndelta;
      T*    newitems;

      if ( newlimit < length )
        newlimit = length;

    // Allocate new space
      if ( (newitems = (T*)realloc( pitems, newlimit * sizeof(T) )) == NULL )
        return ENOMEM;

      pitems = newitems;
      nlimit = newlimit;
    }
    __safe_array_construct_def( pitems + ncount, length - ncount );
  }
    else
  if ( length == 0 )
  {
    if (pitems) {
      free( pitems );
      pitems = 0;
    }
    nlimit = 0;
  }
  ncount = length;
  return 0;
}

template <class T, class R>
inline  int   array<T,R>::Limit  () const { return nlimit; }

template <class T, class R>
inline  int   array<T,R>::Reserve( int limit_ )
{
  if ( limit_ > nlimit )
  {
    int   newlimit = limit_;
    T*    newitems;

    // Allocate new space
    if ( (newitems = (T*)malloc( newlimit * sizeof(T) )) == NULL )
      return ENOMEM;

    // Copy the data
    if ( ncount > 0 )
      memcpy( newitems, pitems, ncount * sizeof(T) );

    // Set new buffer
    if ( pitems != NULL )
      free( pitems );
    pitems = newitems;
    nlimit = newlimit;
  }
  return 0;
}

template <class T, class R>
inline  void  array<T, R>::SetGran( int adelta )
{
  if ( adelta <= 0 )
  {
    if ( (adelta = ncount / 8) < 0x10 )
      adelta = 0x10;
    adelta = (adelta + 0x0f) & ~0x0f;
  }
  ndelta = adelta;
}

template <class T, class R>
inline  bool  array<T, R>::Search( R  rfitem, int&  refidx ) const
{
  int   l = 0;
  int   h = ncount - 1;
  int   m;
  bool  s = false;

  while ( l <= h )
  {
    m = ( l + h ) >> 1;
    if ( pitems[m] < rfitem ) l = m + 1;
      else
    {
      h = m - 1;
      s |= (pitems[m] == rfitem);
    }
  }
  refidx = (int)l;
  return s;
}

template <class T, class R>
inline  bool  array<T, R>::Search( R  rfitem, int&  refidx, int (*compare)( T& r1, T& r2 ) ) const
{
  int   l = 0;
  int   h = ncount - 1;
  int   m;
  bool  s = false;

  while ( l <= h )
  {
    int   r;

    m = ( l + h ) >> 1;
    r = compare( pitems[m], rfitem );

    if ( r < 0 ) l = m + 1;
      else
    {
      h = m - 1;
      s |= (r == 0);
    }
  }
  refidx = (int)l;
  return s;
}

template <class T, class R>
inline  bool  array<T, R>::LinearSearch( R  rfitem, int&  refidx )
{
  int   nindex;

  for ( nindex = 0; nindex < ncount; nindex++ )
    if ( rfitem == pitems[nindex] )
    {
      refidx = nindex;
      return true;
    }
  refidx = ncount;
  return false;
}

template <class T, class R>
inline  bool  array<T, R>::LinearSearch( R  rfitem, int&  refidx, int (*compare)( T& r1, T& r2 ) )
{
  int   nindex;

  for ( nindex = 0; nindex < ncount; nindex++ )
    if ( compare( pitems[nindex], rfitem ) == 0 )
    {
      refidx = nindex;
      return true;
    }
  refidx = ncount;
  return false;
}

template <class T, class R>
inline  void  array<T, R>::Resort( int (*compare)( T& r1, T& r2 ) )
{
  if ( pitems && ncount )
    qsort( pitems, ncount, sizeof(T), (int (*)( const void*, const void* ))compare );
}

template <class T, class R>
inline  void  array<T, R>::MergeResort( int (*compare)( T& r1, T& r2 ) )
{
  if ( pitems && ncount )
#if defined(OS_FREEBSD) || defined(OS_FREEBSD64)
    if (mergesort( pitems, ncount, sizeof(T), (int (*)( const void*, const void* ))compare ))
#endif
      qsort( pitems, ncount, sizeof(T), (int (*)( const void*, const void* ))compare );
}

template <class T, class R>
inline  array<T, R>::operator T* ()
{
  return pitems;
}

template <class T, class R>
inline  array<T, R>::operator const T* () const
{
  return pitems;
}

template <class T, class R>
inline  T&    array<T, R>::operator [] ( int nindex )
{
  assert( nindex < ncount && nindex >= 0 );
  return pitems[nindex];
}

template <class T, class R>
inline  const T&  array<T, R>::operator [] ( int nindex ) const
{
  assert( nindex < ncount && nindex >= 0 );
  return pitems[nindex];
}

template <class T, class R>
inline  T&    array<T, R>::operator [] ( unsigned int nindex )
{
  assert( (int)nindex < ncount && (int)nindex >= 0 );
  return pitems[nindex];
}

template <class T, class R>
inline  const T&  array<T, R>::operator [] ( unsigned int nindex ) const
{
  assert( (int)nindex < ncount && (int)nindex >= 0 );
  return pitems[nindex];
}

template <class T, class R>
inline T& array<T, R>::Back()
{
    return pitems[ncount - 1];
}

template <class T, class R>
const T& array<T, R>::Back() const
{
    return pitems[ncount - 1];
}

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif // _MSC_VER

# endif  // __array_h__
