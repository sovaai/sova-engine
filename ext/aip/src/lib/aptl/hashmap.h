# if !defined( __hashmap_h__ )
# define  __hashmap_h__

# include <assert.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 )
# endif // _MSC_VER

template <class T>
inline  unsigned  hashmap_gethash( const T& t )
  {  return t.GetHash();  }

inline  unsigned  hashmap_gethash( char k )
  {  return (unsigned)( k >= 0 ? k : -k );  }
inline  unsigned  hashmap_gethash( short k )
  {  return (unsigned)( k >= 0 ? k : -k );  }
inline  unsigned  hashmap_gethash( int k )
  {  return (unsigned)( k >= 0 ? k : -k );  }
inline  unsigned  hashmap_gethash( long k )
  {  return (unsigned)k;  }
inline  unsigned  hashmap_gethash( unsigned char k )
  {  return k;  }
inline  unsigned  hashmap_gethash( unsigned short k )
  {  return k;  }
inline  unsigned  hashmap_gethash( unsigned int k )
  {  return k;  }
inline  unsigned  hashmap_gethash( unsigned long k )
  {  return (unsigned)k;  }
inline  unsigned  hashmap_gethash( unsigned long long k )
  {  return (unsigned)k;  }
inline  unsigned  hashmap_gethash( const char* psz )
  {
    unsigned  int nHash = 0;
    while ( *psz )
      nHash = (nHash << 5) + nHash + (unsigned char)*psz++;
    return nHash;
  }
inline  unsigned  hashmap_gethash( const unsigned char* psz )
  {
    unsigned  int nHash = 0;
    while ( *psz )
      nHash = (nHash << 5) + nHash + (unsigned char)*psz++;
    return nHash;
  }

template <class T>
inline  bool      hashmap_iseq( const T& t1, const T& t2 )
  {  return t1 == t2;  }
inline  bool      hashmap_iseq( const char* p1, const char* p2 )
  {  return p1 == p2 || strcmp( p1, p2 ) == 0;  }
inline  bool      hashmap_iseq( const unsigned char* p1, const unsigned char* p2 )
  {  return p1 == p2 || strcmp( (const char*)p1, (const char*)p2 ) == 0;  }

template <class Key, class ArgKey, class Val, class ArgVal>
class   hashmap
{
  struct  keyrec
  {
    Key       key;
    Val       val;
    unsigned  pos;
    keyrec*   lpn;
  protected:
          keyrec( ArgKey    k, ArgVal  t,
                  unsigned  p, keyrec* n ): key( k ),
                                            val( t ),
                                            pos( p ),
                                            lpn( n )
            {
            }
          keyrec( ArgKey    k,
                  unsigned  p, keyrec* n ): key( k ),
                                            pos( p ),
                                            lpn( n )
            {
            }
         ~keyrec()
            {
            }
    void* operator new ( size_t, keyrec*  pv )
            {
              return pv;
            }
    void  operator delete ( void* pv )
            {
              free( pv );
            }
  public:
    static  keyrec* Create( ArgKey    k, ArgVal  t,
                            unsigned  p, keyrec* n )
              {
                keyrec* newrec = (keyrec*)malloc( sizeof(keyrec) );

                if ( newrec != NULL )
                  new ( newrec ) keyrec( k, t, p, n );
                return newrec;
              }
    static  keyrec* Create( ArgKey    k,
                            unsigned  p, keyrec* n )
              {
                keyrec* newrec = (keyrec*)malloc( sizeof(keyrec) );

                if ( newrec != NULL )
                  new ( newrec ) keyrec( k, p, n );
                return newrec;
              }
    void    Delete()
              {
                delete this;
              }
  };
private:
                hashmap( const hashmap<Key, ArgKey, Val, ArgVal>& );
  hashmap<Key, ArgKey, Val, ArgVal>& operator = (
                  const hashmap<Key, ArgKey, Val, ArgVal>& );
public:
                hashmap( unsigned tablen = 69959 );
               ~hashmap();
// Map work methods
  int           Delete( ArgKey );
  void          DelAll();
  unsigned      GetLen() const;
  int           Insert( ArgKey, ArgVal );
  int           Insert( ArgKey, ArgVal, Val*& );
  Val*          Insert( ArgKey );
  Val*          Search( ArgKey ) const;
// Enumerator support methods
  void*         Enum( const void* ) const;
  static  Key&  GetKey( const void* );
  static  Val&  GetVal( const void* );
protected:
  int       Alloc()
              {
                assert( pitems == NULL );
                assert( maplen != 0 );

                if ( pitems != NULL || maplen == 0 )
                  return EINVAL;
                if ( (pitems = (keyrec**)malloc( maplen * sizeof(keyrec*) )) == NULL )
                  return ENOMEM;
                else memset( pitems, 0, maplen * sizeof(keyrec*) );
                return 0;
              }
protected:
  keyrec**  pitems;
  unsigned  maplen;
  unsigned  ncount;
};

// Map inline implementation

template <class Key, class ArgKey, class Val, class ArgVal>
inline  hashmap<Key, ArgKey, Val, ArgVal>::hashmap( unsigned tablen ):
            pitems( NULL ),
            maplen( tablen ),
            ncount( 0 )
{
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  hashmap<Key, ArgKey, Val, ArgVal>::~hashmap()
{
  if ( pitems != NULL )
  {
    unsigned  nindex;
    keyrec*   lpitem;

    for ( nindex = 0; nindex < maplen; nindex++ )
    {
      lpitem = pitems[nindex];

      while ( lpitem != NULL )
      {
        keyrec* lpfree = lpitem;
        lpitem = lpitem->lpn;
        lpfree->Delete();
      }
    }

  // Delete hash table
    free( pitems );
  }
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  int   hashmap<Key, ArgKey, Val, ArgVal>::Delete( ArgKey k )
{
  if ( pitems != NULL && ncount != 0 )
  {
    unsigned  nhcode = hashmap_gethash( k ) % maplen;
    keyrec**  ppitem = &pitems[nhcode];
    keyrec*   lpfree;

    assert( *ppitem == NULL || nhcode == (*ppitem)->pos );

    while ( *ppitem != NULL && !hashmap_iseq( (*ppitem)->key, k ) )
      ppitem = &(*ppitem)->lpn;
    if ( *ppitem != NULL )
    {
      lpfree = *ppitem;
      *ppitem = lpfree->lpn;
      lpfree->Delete();
      --ncount;
    }
    return 0;
  }
  return EINVAL;
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  void  hashmap<Key, ArgKey, Val, ArgVal>::DelAll( )
{
  unsigned  nindex;
  keyrec*   lpitem;

  if ( pitems != NULL )
  {
    for ( nindex = 0; nindex < maplen; nindex++ )
    {
      lpitem = pitems[nindex];

      while ( lpitem != NULL )
      {
        keyrec* lpfree = lpitem;
        lpitem = lpitem->lpn;
        lpfree->Delete();
      }
    }
    memset( pitems, 0, maplen * sizeof(pitems[0]) );
  }
  ncount = 0;
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  unsigned  hashmap<Key, ArgKey, Val, ArgVal>::GetLen() const
{
  return ncount;
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  Val*  hashmap<Key, ArgKey, Val, ArgVal>::Search( ArgKey k ) const
{
  if ( pitems != NULL )
  {
    unsigned  nhcode = hashmap_gethash( k ) % maplen;
    keyrec*   lpitem = pitems[nhcode];

    assert( lpitem == NULL || nhcode == lpitem->pos );

    while ( lpitem != NULL && !hashmap_iseq( lpitem->key, k ) )
      lpitem = lpitem->lpn;
    return ( lpitem != NULL ? &lpitem->val : NULL );
  }
    else
  return NULL;
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  int   hashmap<Key, ArgKey, Val, ArgVal>::Insert( ArgKey k, ArgVal t )
{
  unsigned  nindex = hashmap_gethash( k ) % maplen;
  keyrec*   newrec;
  int       nerror;

// Ensure the map is allocated
  if ( pitems == NULL && (nerror = Alloc()) != 0 )
    return nerror;

// Allocate the item
  if ( (newrec = keyrec::Create( k, t, nindex, pitems[nindex] )) == NULL )
    return ENOMEM;
  pitems[nindex] = newrec;
  ++ncount;
  return 0;
}

// Returns pointer to inserted element's value through the third parameter.
template <class Key, class ArgKey, class Val, class ArgVal>
inline  int   hashmap<Key, ArgKey, Val, ArgVal>::Insert( ArgKey k, ArgVal t, Val*& pp )
{
  unsigned  nindex = hashmap_gethash( k ) % maplen;
  keyrec*   newrec;
  int       nerror;

// Ensure the map is allocated
  if ( pitems == NULL && (nerror = Alloc()) != 0 )
    return nerror;

// Allocate the item
  if ( (newrec = keyrec::Create( k, t, nindex, pitems[nindex] )) == NULL )
    return ENOMEM;
  pitems[nindex] = newrec;
  ++ncount;

  pp = &newrec->val;
  return 0;
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  Val*  hashmap<Key, ArgKey, Val, ArgVal>::Insert( ArgKey k )
{
  unsigned  nindex = hashmap_gethash( k ) % maplen;
  keyrec*   newrec;
  int       nerror;

// Ensure the map is allocated
  if ( pitems == NULL && (nerror = Alloc()) != 0 )
    return NULL;

// Allocate the item
  if ( (newrec = keyrec::Create( k, nindex, pitems[nindex] )) == NULL )
    return NULL;
  pitems[nindex] = newrec;
  ++ncount;

  return &newrec->val;
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  void*     hashmap<Key, ArgKey, Val, ArgVal>::Enum( const void* pvn ) const
{
  keyrec*   curguy = (keyrec*)pvn;
  unsigned  nindex;

// Check pitems initialized
  if ( pitems == NULL )
    return NULL;

// For the first call, make valid object pointer
  if ( curguy == NULL )
  {
    for ( nindex = 0; nindex < maplen; nindex++ )
      if ( (curguy = pitems[nindex]) != NULL )
        break;
  }
    else
// Get the next item for the selected one
  {
    keyrec* lpcurr = curguy;

    if ( (curguy = curguy->lpn) == NULL )
    {
      for ( nindex = lpcurr->pos + 1; nindex < maplen; nindex++ )
        if ( (curguy = pitems[nindex]) != NULL )
          break;
    }
  }
  return curguy;
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  Key&      hashmap<Key, ArgKey, Val, ArgVal>::GetKey( const void*  pvn )
{
  assert( pvn != NULL );

  return ((keyrec*)pvn)->key;
}

template <class Key, class ArgKey, class Val, class ArgVal>
inline  Val&      hashmap<Key, ArgKey, Val, ArgVal>::GetVal( const void*  pvn )
{
  assert( pvn != NULL );

  return ((keyrec*)pvn)->val;
}

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif // _MSC_VER

# endif  // __hashmap_h__
