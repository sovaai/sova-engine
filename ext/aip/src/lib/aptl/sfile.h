/* Copyright (C) 2002-2003 Alexey P. Ivanov, MSU, Dept. of Phys., Moscow
 *
 * PROJECT: apTemplate library
 *
 * COMPILER: MS VC++ 6.0, GCC     TARGET: Win32,FreeBSD-x86,Linux-x86
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 25.02.02: v 1.00 created by AlexIV
 *==========================================================================*/
/*!
    \file  sfile.h
    \brief In-memory string file operations
 */
#ifndef __SFILE_H
#define __SFILE_H

#include <stdio.h>
#include <string.h>

#ifdef feof
#undef feof
#endif

class SFILE
{
protected:
  LPCBYTE m_pBeg;
  DWORD   m_nCur, m_nLen;
  int     m_ungetbuf;
public:
  virtual int fclose() { m_pBeg = NULL; m_nCur = m_nLen = 0; m_ungetbuf = EOF; return 0; }
  virtual int fopen( const char *pBuf, size_t nLen=0, const char *mode=NULL )
    { UNREF(mode); fclose();
      if ((m_pBeg=LPCBYTE(pBuf))!=0) m_nLen = (nLen?nLen:strlen((LPCSTR)m_pBeg)); 
      else                           m_nLen = 0;
      return 0;
    }
  SFILE( LPCBYTE pBuf=0, size_t nLen=0, LPCSTR mode=NULL ) : 
    m_pBeg(pBuf), m_nCur(0), m_nLen(nLen), m_ungetbuf(EOF)
    { fopen(LPCSTR(pBuf),nLen,mode); }
  SFILE( LPCSTR szBuf, size_t nLen=0, LPCSTR mode=NULL ) : 
    m_pBeg(LPCBYTE(szBuf)), m_nCur(0), m_nLen(nLen), m_ungetbuf(EOF)
    { fopen(szBuf,nLen,mode); }
  SFILE( const SFILE& rSrc ) : 
    m_pBeg(rSrc.m_pBeg), m_nCur(rSrc.m_nCur), m_nLen(rSrc.m_nLen) 
    { m_ungetbuf = EOF; }
  virtual ~SFILE() { fclose(); }

// Attributes
public:
  LPCBYTE begin() const { return m_pBeg; }
  LPCBYTE end()   const { return m_pBeg ? m_pBeg+m_nLen : m_pBeg; }
  size_t  size()  const { return m_nLen; }
  int     feof()  const { return m_nCur >= m_nLen && m_ungetbuf == EOF; }
  size_t  ftell() const { return m_nCur >= m_nLen ? size_t(-1L) : m_nCur; }

// Operations
public:
  void rewind() { m_nCur = 0; m_ungetbuf = EOF; }
  int  fseek( long offset, int origin = SEEK_SET );
  int  fgetc() { 
       if (m_ungetbuf != EOF) { int c = m_ungetbuf; m_ungetbuf = EOF; return c; }
       return (m_nLen&&(m_nCur<m_nLen)) ? m_pBeg[m_nCur++] : EOF; }
  int  ungetc( int c ) { return ( c == EOF || m_ungetbuf != EOF ) ? EOF : m_ungetbuf=c; }
  size_t fread( void *buffer, size_t itemsize, size_t itemcount );
  char*  fgets( char *str, int n );
};

inline int SFILE::fseek( long offset, int origin/*=SEEK_SET*/ )
{
  m_ungetbuf = EOF;
  switch(origin) {
    case SEEK_SET: break;
    case SEEK_END: offset += m_nLen; break;
    case SEEK_CUR: offset += m_nCur; break;
    default: return -1;
  }
  if ( offset < 0 ) offset = 0;
  m_nCur = offset;
  return 0;
}

inline size_t SFILE::fread( void *buffer, size_t itemsize, size_t itemcount )
{
  if ( feof() ) return 0;
  size_t len = itemsize*itemcount, eof_len = m_nLen - m_nCur;
  if ( eof_len < len ) { itemcount = eof_len/itemsize; len = itemsize*itemcount; }
  if ( len == 0 ) return 0;
  size_t beg = 0;
  if ( m_ungetbuf != EOF ) {
    beg++; len--; *LPSTR(buffer) = (char)m_ungetbuf; m_ungetbuf = EOF;
  }
  memcpy(LPSTR(buffer)+beg,m_pBeg+m_nCur,len);
  m_nCur += len;
  return itemcount;
}

inline char* SFILE::fgets( char *str, int n )
{
  if ( feof() || n <= 0 ) return NULL;
  if ( m_ungetbuf == '\n' ) { m_ungetbuf = EOF; str[0] = '\n'; if ( n > 1 ) str[1] = '\0'; return str; }
  size_t len = m_nLen - m_nCur;
  LPCBYTE p = (LPCBYTE)memchr(m_pBeg+m_nCur,'\n',len);
  if ( p ) len = (p-(m_pBeg+m_nCur))+1;
  if ( len > size_t(n) ) len = size_t(n);
  size_t beg = 0;
  if ( m_ungetbuf != EOF ) {
    beg++; len--; *str = (char)m_ungetbuf; m_ungetbuf = EOF; 
  }
  memcpy(str+beg,m_pBeg+m_nCur,len);
  if ( beg+len < size_t(n) ) str[beg+len] = '\0';
  m_nCur += len;
  return str;
}

inline SFILE *fopen( const char *pBuf, size_t nLen=0, const char *mode=NULL )
{
  if ( !pBuf ) return NULL;
  return new SFILE(pBuf,nLen,mode);
}

inline int fclose( SFILE *stream )
{ delete stream; return 0; }

inline int fgetc( SFILE *stream )
{ return stream->fgetc(); }

inline int ungetc( int c, SFILE *stream )
{ return stream->ungetc(c); }

inline size_t fread( void *buffer, size_t size, size_t count, SFILE *stream )
{ return stream->fread(buffer,size,count); }

inline char* fgets( char *str, int n, SFILE *stream )
{ return stream->fgets(str,n); }

inline int feof( const SFILE *stream )
{ return stream->feof(); }

inline long ftell( const SFILE *stream )
{ return stream->ftell(); }

inline long filelength( const SFILE *stream )
{ return stream->size(); }

inline void rewind( SFILE *stream )
{ stream->rewind(); }

inline int fseek( SFILE *stream, long offset, int origin )
{ return stream->fseek(offset,origin); }

#endif /*__SFILE_H*/
