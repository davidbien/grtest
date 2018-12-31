#ifndef __GR_OLE_H
#define __GR_OLE_H

#include <ole2.h>
#include "..\_oleutil.h"

// OLE graph elements - an implementation.
// NOTE: I am not sure that this code has ever been run ( pretty sure it hasn't ).

__DGRAPH_BEGIN_NAMESPACE

template < bool t_fCopyElement = false >
struct _ole_graph_element
{
private:
  typedef _ole_graph_element< t_fCopyElement >  _TyThis;
public:

  IUnknown * m_punk;

  explicit _ole_graph_element( IUnknown * _punk )
    : m_punk( _punk )
  {
#ifndef NDEBUG
    // Insure that we got sent the innermost unknown:
    IUnknown  * punkDebug;
    __QI( _punk, punkDebug );
    assert( _punk == punkDebug );
    punkDebug->Release();
#endif //!NDEBUG
    m_punk->AddRef();
  }
  explicit _ole_graph_element( _TyThis const & _r )
  {
    if ( t_fCopyElement )
    {
      // Then we copy the element using IPersistStream:
      m_punk = 0; // In case of throw.
      IPersistStream *  ppsSrc;
      __QI( _r.m_punk, ppsSrc );
      _BIEN_TRY
      {
        CLSID clsid;
        __ThrowOLEFAIL( pps->GetClassID( &clsid ) );
        IUnknown *  punk;
        __CoCreateLocal( clsid, punk );

        _BIEN_TRY
        {
          IPersistStream *  ppsDst;
          __QI( punk, ppsDst );

          _BIEN_TRY
          {
            IStream * ps;
            CreateStreamOnHGlobal( NULL, TRUE, &ps );
            _BIEN_TRY
            {
              __ThrowOLEFAIL( ppsSrc->Save( ps, FALSE ) );
              __ThrowOLEFAIL( ps->Seek( 0ll, STREAM_SEEK_SET, 0 ) );
              __ThrowOLEFAIL( ppsDst->Load( ps ) );
            }
            _BIEN_UNWIND( ps->Release() );
            ps->Release();
          }
          _BIEN_UNWIND( ppsDst->Release() );
          ppsDst->Release();
        }
        _BIEN_UNWIND( punk->Release() );
        m_punk = punk;
      }
      _BIEN_UNWIND( ppsSrc->Release() );
      ppsSrc->Release();
    }
    else
    {
      ( m_punk = _r.m_punk )->AddRef();
    }
  }

  ~_ole_graph_element() __STLP_NOTHROW
  {
    m_punk->Release();
  }

  void
  Save( IStream * _ps, bool _fClearDirty = false ) const
  {
    IPersistStream *  pps;
    __QI( m_punk, pps );
    _BIEN_TRY
    {
      CLSID clsid;
      __TOF( pps->GetClassID( &clsid ) );
      __TOF( WriteClassStm( _ps, clsid ) );
      __TOF( pps->Save( _ps, _fClearDirty ) );
    }
    _BIEN_UNWIND( pps->Release() );
    pps->Release();
  }

  void
  Load( IStream * _ps )
  {
    // This constructs this object - it may currently contain garbage:
    __TOF( OleLoadFromStream( _ps, __uuidof(m_punk), reinterpret_cast< void** >( &m_punk ) ) );
  }
};

// Specialize for OLE elements - we will QI the IPersistStream.
template <>
__INLINE void
_RawWriteGraphEl( IStream *& _rps, _ole_graph_element const & _rEl )
{
  _rEl.Save( _rps, false );
}

// Specialize for IStream - default version just reads raw memory:
template < class t_TyRead >
__INLINE void
_RawReadGraphEl( IStream *& _rps, _ole_graph_element & _rEl )
{ 
  _rEl.Load( _rps );
}

__DGRAPH_END_NAMESPACE

#endif //__GR_OLE_H
