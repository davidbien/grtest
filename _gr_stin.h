#ifndef __GR_STIN_H
#define __GR_STIN_H

// _gr_stin.h

// This module defines the input objects used to read graphs from streams.

__DGRAPH_BEGIN_NAMESPACE

// Function that knows how to read raw data to a stream:
template < class t_TyInputStream, class t_TyRead >
void
_RawReadGraphEl( t_TyInputStream & _ris, t_TyRead & _rel )
{ 
  assert( 0 ); // Should specialize for each stream type.
}

template <  class t_TyGraphNodeBaseReadPtr, 
            class t_TyGraphLinkBaseReadPtr,
            class t_TyStreamObject,
            // This causes all available information to be read - i.e.
            //  the names ( pointers ) of all links and nodes - this is not needed by all readers.
            // The "extra information" attribute of the writer must correspond to that of the reader.
            bool t_fReadExtraInformation >  
struct _binary_input_base
{
private:
  typedef _binary_input_base< t_TyGraphNodeBaseReadPtr, 
                              t_TyGraphLinkBaseReadPtr,
                              t_TyStreamObject,
                              t_fReadExtraInformation > _TyThis;
public:

  typedef t_TyGraphNodeBaseReadPtr  _TyGraphNodeBaseReadPtr;
  typedef t_TyGraphLinkBaseReadPtr  _TyGraphLinkBaseReadPtr;

  typedef typename t_TyStreamObject::_TyInitArg   _TyInitArg;
  typedef typename t_TyStreamObject::_TyStreamPos _TyStreamPos;
  typedef typename t_TyStreamObject::_TyIONodeEl _TyIONodeEl;
  typedef typename t_TyStreamObject::_TyIOLinkEl _TyIOLinkEl;
  typedef typename _binary_rep_tokens< std::false_type >::_TyToken   _TyToken;

  t_TyStreamObject  m_ris; // The stream from which we are reading.

  _binary_input_base( _TyInitArg _ris, 
                      _TyIONodeEl const & _rione, 
                      _TyIOLinkEl const & _riole )
    : m_ris( _ris, _rione, _riole )
  {
  }

  _TyStreamPos _Tell()
  {
    return m_ris.TellG();
  }
  void _Seek( _TyStreamPos _sp )
  {
    m_ris.SeekG( _sp );
  }

  void  _ReadToken( _TyToken * _puc )
  {
    __THROWPT( e_ttFileInput );
    m_ris.Read( _puc, sizeof( _TyToken ) );
  }
  void _ReadNodePtr( _TyGraphNodeBaseReadPtr * _pgnbr )
  {
    __THROWPT( e_ttFileInput );
    m_ris.Read( _pgnbr, sizeof *_pgnbr );
  }
  void _ReadLinkPtr( _TyGraphLinkBaseReadPtr * _pglbr )
  {
    __THROWPT( e_ttFileInput );
    m_ris.Read( _pglbr, sizeof *_pglbr );
  }

  void  _ReadNodeHeaderData( _TyGraphNodeBaseReadPtr * _pgnbr )
  {
    __THROWPT( e_ttFileInput );
    if ( t_fReadExtraInformation )
    {
      _ReadNodePtr( _pgnbr );
    }
    else
    {
      *_pgnbr = 0;  // Indicate that we din't read it.
    }
  }

  void  _ReadUnfinishedHeaderData( _TyGraphNodeBaseReadPtr * _pgnbr,
                                   _TyGraphLinkBaseReadPtr * _pglbr )
  {
    __THROWPT( e_ttFileInput );
    _ReadNodePtr( _pgnbr );
    _ReadLinkPtr( _pglbr );
  }

  void _ReadNodeFooter()
  {
    __THROWPT( e_ttFileInput );
#ifdef __GR_BINARY_WRITENODEFOOTER
    _TyToken  uc;
    _ReadToken( &uc );
    if ( _binary_rep_tokens< std::false_type >::ms_ucNodeFooter != uc )
    {
      throw bad_graph_stream( "_ReadNodeFooter(): Expected node footer token." );
    }
#endif //__GR_BINARY_WRITENODEFOOTER
  }

  void  _ReadLinkName( _TyGraphLinkBaseReadPtr * _pglbr )
  {
    __THROWPT( e_ttFileInput );
    _ReadLinkPtr( _pglbr );
  }

  void  _ReadLinkHeaderData(  _TyGraphLinkBaseReadPtr * _pglbr )
  {
    __THROWPT( e_ttFileInput );
    if ( t_fReadExtraInformation )
    {
      _ReadLinkPtr( _pglbr );
    }
    else
    {
      // Indicate that we didn't read it:
      *_pglbr = 0;
    }
  }
  void  _ReadLinkFromUnfinishedHeaderData(  _TyGraphLinkBaseReadPtr * _pglbr,
                                            _TyGraphNodeBaseReadPtr * _pgnbr )
  {
    __THROWPT( e_ttFileInput );
    _ReadLinkPtr( _pglbr );
    if ( t_fReadExtraInformation )
    {
      _ReadNodePtr( _pgnbr );
    }
    else
    {
      *_pgnbr = 0;
    }
  }

  bool  _FReadLinkConstructed()
  {
    __THROWPT( e_ttFileInput );
    _TyToken  uc;
    _ReadToken( &uc );
    if (  _binary_rep_tokens< std::false_type >::ms_ucLinkConstructed != uc &&
          _binary_rep_tokens< std::false_type >::ms_ucLinkEmpty != uc )
    {
      throw bad_graph_stream( "_FReadLinkConstructed(): Bad construction token." );
    }

    return _binary_rep_tokens< std::false_type >::ms_ucLinkConstructed == uc;
  }

  void  _ReadLinkFooter( _TyToken * _puc )
  {
    __THROWPT( e_ttFileInput );
    _ReadToken( _puc );
  }

  void  _ReadUnfinishedLinkFooterData(  _TyGraphLinkBaseReadPtr * _pglbr,
                                        _TyGraphNodeBaseReadPtr * _pgnbr )
  {
    __THROWPT( e_ttFileInput );
    _ReadNodePtr( _pgnbr );
    if ( t_fReadExtraInformation )
    {
      assert( *_pglbr );  // This should have been read above.
    }
    else
    {
      if ( !*_pglbr )
      {
        // This may have been read above ( if we had a link from an unfinished node ):
        _ReadLinkPtr( _pglbr );
      }
    }
  }
};

template <  class t_TyGraphNode, class t_TyGraphLink,
            class t_TyStreamObject,
            bool t_fReadExtraInformation,
            class t_TyGraphNodeBaseReadPtr = const typename t_TyGraphNode::_TyGraphNodeBaseBase *,
            class t_TyGraphLinkBaseReadPtr = const typename t_TyGraphLink::_TyGraphLinkBaseBase * >
struct _binary_input_object
  : public _binary_input_base<  t_TyGraphNodeBaseReadPtr,
                                t_TyGraphLinkBaseReadPtr,
                                t_TyStreamObject,
                                t_fReadExtraInformation >
{
private:
  typedef _binary_input_object< t_TyGraphNode, t_TyGraphLink, t_TyStreamObject,
    t_fReadExtraInformation, t_TyGraphNodeBaseReadPtr,
    t_TyGraphLinkBaseReadPtr >  _TyThis;
  typedef _binary_input_base< t_TyGraphNodeBaseReadPtr,
                              t_TyGraphLinkBaseReadPtr,
                              t_TyStreamObject,
                              t_fReadExtraInformation >         _TyBase;
public:

  typedef _TyBase   _TyInputObjectBase;
  typedef typename _TyBase::_TyInitArg _TyInitArg;
  typedef typename _TyBase::_TyIONodeEl _TyIONodeEl;
  typedef typename _TyBase::_TyIOLinkEl _TyIOLinkEl;

  _binary_input_object( _TyInitArg _ris, 
                        _TyIONodeEl const & _rione, 
                        _TyIOLinkEl const & _riole )
    : _TyBase( _ris, _rione, _riole )
  {
  }

  void    _ReadNode( t_TyGraphNode * _pgn )
  {
    __THROWPT( e_ttFileInput | e_ttMemory );
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iNodesConstructed++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
    _TyBase::m_ris.ReadNodeEl( _pgn->RElNonConst() );
  }

  void    _ReadLink( t_TyGraphLink * _pgl )
  {
    __THROWPT( e_ttFileInput | e_ttMemory );
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iLinksConstructed++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
    _TyBase::m_ris.ReadLinkEl( _pgl->RElNonConst() );
  }
};

__DGRAPH_END_NAMESPACE

#endif //__GR_STIN_H
