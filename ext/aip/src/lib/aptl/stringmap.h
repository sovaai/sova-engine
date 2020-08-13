# if !defined( __stringmap_h__ )
# define  __stringmap_h__

# include <assert.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 )
# endif // _MSC_VER

template <class Val, class ArgVal>
class   stringmap
{
  struct  keyrec
  {
    Val       val;
    unsigned  pos;
    keyrec*   lpn;
    char      key[1];
  protected:
          keyrec( ArgVal    t,
                  unsigned  p,
                  keyrec*   n ): val( t ),
                                 pos( p ),
                                 lpn( n )
            {
            }
          keyrec( unsigned  p,
                  keyrec*   n ): pos( p ),
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
    static  keyrec* Create( const char* k,
                            ArgVal      t,
                            unsigned    p,
                            keyrec*     n )
              {
                int     length = k == NULL ? 0 : strlen( k );
                keyrec* newrec = (keyrec*)malloc( sizeof(keyrec) + length );

                if ( newrec != NULL )
                {
                  new ( newrec ) keyrec( t, p, n );
                  memcpy( newrec->key, k, length );
                  newrec->key[length] = '\0';
                }
                return newrec;
              }
    static  keyrec* Create( const char* k,
                            unsigned    p,
                            keyrec*     n )
              {
                int     length = k == NULL ? 0 : strlen( k );
                keyrec* newrec = (keyrec*)malloc( sizeof(keyrec) + length );

                if ( newrec != NULL )
                {
                  new ( newrec ) keyrec( p, n );
                  memcpy( newrec->key, k, length );
                  newrec->key[length] = '\0';
                }
                return newrec;
              }
    void    Delete()
              {
                delete this;
              }
  };
private:
                stringmap( const stringmap<Val, ArgVal>& );
  stringmap<Val, ArgVal>& operator = (
                  const stringmap<Val, ArgVal>& );
public:
                stringmap( unsigned tablen = 69959 );
               ~stringmap();
// Map work methods
  int           Delete( const char* );
  void          DelAll();
  unsigned      GetLen() const;
  unsigned      TabLen() const;
  int           Insert( const char*, ArgVal );
  int           Insert( const char*, ArgVal, Val*& );
  Val*          Insert( const char* );
  Val*          Search( const char* ) const;
// Enumerator support methods
  void*               Enum( const void* );
  static  const char* GetKey( const void* );
  static  Val&        GetVal( const void* );
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
  unsigned  gethash( const char* psz ) const
              {
                unsigned  int nHash = 0;
                while ( *psz )
                  nHash = (nHash << 5) + nHash + (unsigned char)*psz++;
                return nHash;
              }
  bool      iseq( const char* p1, const char* p2 ) const
              {
                return p1 == p2 || strcmp( p1, p2 ) == 0;
              }
private:
  keyrec**  pitems;
  unsigned  maplen;
  unsigned  ncount;
};

// Map inline implementation

template <class Val, class ArgVal>
inline  stringmap<Val, ArgVal>::stringmap( unsigned tablen ):
            pitems( NULL ),
            maplen( tablen ),
            ncount( 0 )
{
}

template <class Val, class ArgVal>
inline  stringmap<Val, ArgVal>::~stringmap()
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

template <class Val, class ArgVal>
inline  int   stringmap<Val, ArgVal>::Delete( const char* k )
{
  if ( pitems != NULL && ncount != 0 )
  {
    unsigned  nhcode = gethash( k ) % maplen;
    keyrec**  ppitem = &pitems[nhcode];
    keyrec*   lpfree;

    assert( *ppitem == NULL || nhcode == (*ppitem)->pos );

    while ( *ppitem != NULL && !iseq( (*ppitem)->key, k ) )
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

template <class Val, class ArgVal>
inline  void  stringmap<Val, ArgVal>::DelAll( )
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

template <class Val, class ArgVal>
inline  unsigned  stringmap<Val, ArgVal>::GetLen() const
{
  return ncount;
}

template <class Val, class ArgVal>
inline  unsigned  stringmap<Val, ArgVal>::TabLen() const
{
  return maplen;
}

template <class Val, class ArgVal>
inline  Val*  stringmap<Val, ArgVal>::Search( const char* k ) const
{
  if ( pitems != NULL )
  {
    unsigned  nhcode = gethash( k ) % maplen;
    keyrec*   lpitem = pitems[nhcode];

    assert( lpitem == NULL || nhcode == lpitem->pos );

    while ( lpitem != NULL && !iseq( lpitem->key, k ) )
      lpitem = lpitem->lpn;
    return ( lpitem != NULL ? &lpitem->val : NULL );
  }
    else
  return NULL;
}

template <class Val, class ArgVal>
inline  int   stringmap<Val, ArgVal>::Insert( const char* k, ArgVal t )
{
  unsigned  nindex = gethash( k ) % maplen;
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
template <class Val, class ArgVal>
inline  int   stringmap<Val, ArgVal>::Insert( const char* k, ArgVal t, Val*& pp )
{
  unsigned  nindex = gethash( k ) % maplen;
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

template <class Val, class ArgVal>
inline  Val*  stringmap<Val, ArgVal>::Insert( const char* k )
{
  unsigned  nindex = gethash( k ) % maplen;
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

template <class Val, class ArgVal>
inline  void*     stringmap<Val, ArgVal>::Enum( const void* pvn )
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

template <class Val, class ArgVal>
inline  const char* stringmap<Val, ArgVal>::GetKey( const void*  pvn )
{
  assert( pvn != NULL );

  return ((keyrec*)pvn)->key;
}

template <class Val, class ArgVal>
inline  Val&      stringmap<Val, ArgVal>::GetVal( const void*  pvn )
{
  assert( pvn != NULL );

  return ((keyrec*)pvn)->val;
}

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif // _MSC_VER

# endif  // __stringmap_h__
