#ifndef __GR_OLIO_H
#define __GR_OLIO_H

// _gr_olio.h

// OLE i/o stuff.

#include <ole2.h>
#include "_oleutil.h"

// _gr_ole.h

// Graph OLE I/O.

__DGRAPH_BEGIN_NAMESPACE

// Specialize for IStream - default version just writes raw memory:
template < class t_TyWrite >
__INLINE void
_RawWriteGraphEl( IStream *& _rs, t_TyWrite const & _rEl )
{ 
  // We don't care about partial writes - so no need to check the third param:
  __ThrowOLEFAIL( _rs->Write( &_rEl, sizeof( _rEl ), 0 ) );
}

// Specialize for IStream - default version just reads raw memory:
template < class t_TyRead >
__INLINE void
_RawReadGraphEl( IStream *& _rs, t_TyRead & _rEl )
{ 
  // We don't care about partial reads - so no need to check the third param:
  __ThrowOLEFAIL( _rs->Read( &_rEl, sizeof( _rEl ), 0 ) );
}

struct _IStream_RawElIO
{
  template < class t_TyEl >
  void Write( IStream * _ps, t_TyEl const & _rel )
  {
    _RawWriteGraphEl( _ps, _rel );
  }
  template < class t_TyEl >
  void Read( IStream * _ps, t_TyEl & _rel )
  {
    _RawReadGraphEl( _ps, _rel );
  }
};

// The OLE object - supports both read and write:
template <  class t_TyIONodeEl,
            class t_TyIOLinkEl = t_TyIONodeEl >
struct _IStream_object
{
  typedef IStream *       _TyInitArg;
  typedef ULARGE_INTEGER  _TyStreamPos;
  typedef t_TyIONodeEl    _TyIONodeEl;
  typedef t_TyIOLinkEl    _TyIOLinkEl;

  IStream * m_ps;

  _TyIONodeEl  m_ione;
  _TyIOLinkEl  m_iole;

  _IStream_object(  IStream * _ps,
                    _TyIONodeEl const & _rione,
                    _TyIOLinkEl const & _riole )
    : m_ps( _ps ),
      m_ione( _rione ),
      m_iole( _riole )
  {
    m_ps->AddRef();
  }
  ~_IStream_object()
  {
    m_ps->Release();
  }

  _TyStreamPos  TellG() const
  { 
    return _Tell();
  }
  void          SeekG( _TyStreamPos _sp ) 
  { 
    _Seek( _sp ); 
  }
  _TyStreamPos  TellP() const             
  { 
    return _Tell();
  }
  void          SeekP( _TyStreamPos _sp ) 
  { 
    _Seek( _sp ); 
  }

  void          Write( const void * _pv, ULONG _len )
  {
    __ThrowOLEFAIL( m_ps->Write( _pv, _len, 0 ) );
  }

  void          Read( void * _pv, ULONG _len )
  {
    __ThrowOLEFAIL( m_ps->Read( _pv, _len, 0 ) );
  }

  template < class t_TyEl >
  void          WriteNodeEl( t_TyEl const & _rel )
  {
    m_ione.Write( m_ps, _rel );
  }
  template < class t_TyEl >
  void          ReadNodeEl( t_TyEl & _rel )
  {
    m_ione.Read( m_ps, _rel );
  }

  template < class t_TyEl >
  void          WriteLinkEl( t_TyEl const & _rel )
  {
    m_iole.Write( m_ps, _rel );
  }
  template < class t_TyEl >
  void          ReadLinkEl( t_TyEl & _rel )
  {
    m_iole.Read( m_ps, _rel );
  }

protected:

  _TyStreamPos  _Tell() const             
  {
    _TyStreamPos  sp;
		static const LARGE_INTEGER llZero = { 0ll };
    __ThrowOLEFAIL( m_ps->Seek( llZero, STREAM_SEEK_CUR, &sp ) );
    return sp;
  }
  void          _Seek( _TyStreamPos _sp ) 
  { 
    __ThrowOLEFAIL( m_ps->Seek( *(LARGE_INTEGER*)&_sp, STREAM_SEEK_SET, 0 ) );
  }
};

__DGRAPH_END_NAMESPACE

#endif //__GR_OLIO_H
