#ifndef __GR_INIP_H
#define __GR_INIP_H

// _gr_inip.h

// This module implements the stream iteration portion of an inplace 
//  input iterator for an existing graph.
// No care is taken ( currently, in this implementation ) to ensure that
//  the element being read is of the correct type. The current working
//  state is such that either:
//  a) Any graph element must be able to read any other graph element.
//  b) ( and this is implied by (a) ) No check is made to ensure that
//    the type of element being read into is of the type located currently
//    on the input stream.
//  c) If (a) is not the case then the caller needs to know that each element
//    will be the same at each point in the iteration.
// A simple templatization can implement (b) above.

__DGRAPH_BEGIN_NAMESPACE

// This object implements those operations that need have no knowledge of the full type specifications
//  of the graph elements.
template <  class t_TyInputStreamBase,
            class t_TyGraphNodeBase, class t_TyGraphLinkBase,
            bool t_fUseSeek = true,   // In case our input stream does not support _Tell()/_Seek().
            // Setting this to true allows the writer to write unconstucted links
            //  if it is false and the writer encounters an unconstucted link it will throw
            //  a bad_graph exception.
            bool t_fAllowUnconstructedLinks = false >
struct _graph_inplace_input_iter_base
{
private:
  typedef _graph_inplace_input_iter_base< t_TyInputStreamBase, t_TyGraphNodeBase, t_TyGraphLinkBase, 
                                          t_fUseSeek, t_fAllowUnconstructedLinks > _TyThis;
public:
  typedef t_TyGraphNodeBase _TyGraphNodeBase;
  typedef t_TyGraphLinkBase _TyGraphLinkBase;

  typedef typename t_TyInputStreamBase::_TyGraphNodeBaseReadPtr _TyGraphNodeBaseReadPtr;
  typedef typename t_TyInputStreamBase::_TyGraphLinkBaseReadPtr _TyGraphLinkBaseReadPtr;

  // Input stream object:
  typedef typename t_TyInputStreamBase::_TyStreamPos  _TyStreamPos;
  t_TyInputStreamBase & m_ris;

  bool m_fProcessedGraphFooter;// Processed graph footer ?

  // Read and construct nodes/links:
  typedef void ( _TyThis :: * _TyPMFnReadNode )( _TyGraphNodeBase * );
  _TyPMFnReadNode   m_pmfnReadNode;

  typedef void ( _TyThis :: * _TyPMFnReadLink )( _TyGraphLinkBase * );
  _TyPMFnReadLink   m_pmfnReadLink;

  // These should not be destroyed - they are status:
  _TyGraphNodeBase *  m_pgnbCur;  // The current link and node.
  _TyGraphLinkBase *  m_pglbCur;

  // to seek or not to seek
  void  _Seek( _TyStreamPos _sp )
  {
    if ( t_fUseSeek )
    {
      m_ris._Seek( _sp );
    }
  }
  void  _Tell( _TyStreamPos & _sp )
  {
    if ( t_fUseSeek )
    {
      _sp = m_ris._Tell( );
    }
  }

  explicit _graph_inplace_input_iter_base( t_TyInputStreamBase & _ris ) :
    m_ris( _ris ),
    m_pgnbCur( 0 ),
    m_pglbCur( 0 ),
    m_fProcessedGraphFooter( false )
  {
    __THROWPT( e_ttMemory );
  }

  ~_graph_inplace_input_iter_base()
  {
  }
  
  bool FAtBeg() const _STLP_NOTHROW
  {
    return !PGNBCur() && !PGLBCur() && !m_fProcessedGraphFooter;
  }
  bool FAtEnd() const _STLP_NOTHROW
  {
    return !PGNBCur() && !PGLBCur() && m_fProcessedGraphFooter;
  }

  _TyGraphNodeBase *  PGNBCur() const _STLP_NOTHROW { return m_pgnbCur; }
  _TyGraphLinkBase *  PGLBCur() const _STLP_NOTHROW { return m_pglbCur; }

protected:

  void SetPGNBCur( _TyGraphNodeBase * _pgnb ) _STLP_NOTHROW
  {
    m_pgnbCur = _pgnb;
  }
  void SetPGLBCur( _TyGraphLinkBase * _pglb ) _STLP_NOTHROW
  {
    m_pglbCur = _pglb;
  }

  // This method does the work - read the next record - create the appropriate graph objects
  //   - throw on error.
  // Read the next graph element into the object that is non-zero.
  void  _Next(  _TyGraphNodeBase * _pgnb, 
                _TyGraphLinkBase * _pglb )
  {
    assert( !_pgnb ^ !_pglb );

    // Save stream position for possible throw - this is also good for debugging - if we encounter
    //  an error in the stream we can go back and re-examine:
    _TyStreamPos  sp;
    _Tell( sp );

    SetPGNBCur( _pgnb );
    SetPGLBCur( _pglb );

    __STL_TRY
    {
      // We check for both context and direction switches so that an input iterator can be kept
      //  "inline" with a forward iterator - this could allow some iteresting transformations:
      while ( _FReadOne() )
      {
        _Tell( sp ); // Each successful read of a context or direction advances the state.
      }
    }
    __STL_UNWIND( _Seek( sp ) );
  }

  bool  _FReadOne()
  {
    typename _binary_rep_tokens< __false_type >::_TyToken uc;
    m_ris._ReadToken( &uc );

    switch( uc )
    {
      case  _binary_rep_tokens< __false_type >::ms_ucDirectionUp:
      case  _binary_rep_tokens< __false_type >::ms_ucDirectionDown:
      {
        return true;
      }
      break;
      case  _binary_rep_tokens< __false_type >::ms_ucContextPush:
      case  _binary_rep_tokens< __false_type >::ms_ucContextPop:
      {
        return true;
      }
      break;

      case  _binary_rep_tokens< __false_type >::ms_ucNode:
      {
        _ReadNode();
      }
      break;

      case  _binary_rep_tokens< __false_type >::ms_ucUnfinishedNode:
      {
        _ReadUnfinishedNode();
      }
      break;

#ifdef __GR_BINARY_WRITENODEFOOTER
      case  _binary_rep_tokens< __false_type >::ms_ucNodeFooter:
      {
        throw bad_graph_stream( "_FReadOne(): Encountered a node footer token at top level." );
      }
      break;
#endif __GR_BINARY_WRITENODEFOOTER

      case  _binary_rep_tokens< __false_type >::ms_ucLink:
      {
        _ReadLink();
      }
      break;

      case  _binary_rep_tokens< __false_type >::ms_ucLinkFromUnfinished:
      {
        _ReadLinkFromUnfinished();
      }
      break;

      case  _binary_rep_tokens< __false_type >::ms_ucNormalLinkFooter:
      {
        throw bad_graph_stream( "_FReadOne(): Encountered a normal link footer token at top level." );
      }
      break;
      case  _binary_rep_tokens< __false_type >::ms_ucUnfinishedLinkFooter:
      {
        throw bad_graph_stream( "_FReadOne(): Encountered an unfinished link footer token at top level." );
      }
      break;
      case _binary_rep_tokens< __false_type >::ms_ucGraphFooter:
      {
        _ProcessGraphFooter();
      }
      break;
      default:
      {
        char  cpError[256];
        sprintf( cpError, "_FReadOne(): Encountered bogus token [%d] at top level.", uc );
      }
      break;
    }

    return false;
  }

  void  _ReadElement()
  {
    if ( m_pgnbCur )
    {
      (this->*m_pmfnReadNode)( m_pgnbCur );
    }
    else
    {
      (this->*m_pmfnReadLink)( m_pglbCur );
    }
  }

  // Read a normal node from the stream:
  void  _ReadNode()
  {
    _TyGraphNodeBaseReadPtr pgnbrNode;
    m_ris._ReadNodeHeaderData( &pgnbrNode );  // May be information before the node element. // throws.

    _ReadElement();

    // Now read any node footer before adding to the graph ( state safe ):
    m_ris._ReadNodeFooter();
  }

  void _ReadUnfinishedNode()
  {
    // Read the node and link names:
    _TyGraphLinkBaseReadPtr pglbrLink;
    _TyGraphNodeBaseReadPtr pgnbrNode;
    m_ris._ReadUnfinishedHeaderData( &pgnbrNode, &pglbrLink );

    _ReadElement();

    // Read the link names:
    _TyGraphLinkBaseReadPtr pglbrRead;
    while( m_ris._ReadLinkName( &pglbrRead ), pglbrRead )
      ;
  }

  void  _ReadLink()
  {
    // Allow this method to potentially read the name of the link:
    _TyGraphLinkBaseReadPtr pglbrLink;
    m_ris._ReadLinkHeaderData( &pglbrLink );

    if ( !t_fAllowUnconstructedLinks || m_ris._FReadLinkConstructed() )
    {
      _ReadElement();
    }

    _ReadLinkFooter();
  }

  void  _ReadLinkFromUnfinished()
  {
    _TyGraphLinkBaseReadPtr pglbrLink;
    _TyGraphNodeBaseReadPtr pgnbrNode;
    m_ris._ReadLinkFromUnfinishedHeaderData( &pglbrLink, &pgnbrNode );

    if ( !t_fAllowUnconstructedLinks || m_ris._FReadLinkConstructed() )
    {
      _ReadElement();
    }
    _ReadLinkFooter();
  }

  void  _ReadLinkFooter( )
  {
    _binary_rep_tokens< __false_type >::_TyToken  uc;
    m_ris._ReadLinkFooter( &uc );
    if ( _binary_rep_tokens< __false_type >::ms_ucNormalLinkFooter == uc )
    {
      // nothing.
    }
    else
    {
      if ( _binary_rep_tokens< __false_type >::ms_ucUnfinishedLinkFooter != uc )
      {
        throw bad_graph_stream( "_ReadLinkFooter(): Found bad link footer token." );
      }

      // Possibly read the link and node names:
      _TyGraphLinkBaseReadPtr pglbrLink;
      _TyGraphNodeBaseReadPtr pgnbrNode;
      m_ris._ReadUnfinishedLinkFooterData( &pglbrLink, &pgnbrNode );
    }
  }

  void _ProcessGraphFooter()
  {
    SetPGNBCur( 0 );
    SetPGLBCur( 0 );
    m_fProcessedGraphFooter = true;
  };
};

template <  class t_TyGraphNode, class t_TyGraphLink, class t_TyInputStream,
            class t_TyBaseInputIter, class t_TyFIsConstIterator >
struct _graph_inplace_input_iterator
  : public t_TyBaseInputIter,
    public _graph_iter_const_base<  t_TyGraphNode, t_TyGraphLink, t_TyFIsConstIterator >
{
private:
  typedef _graph_inplace_input_iterator< t_TyGraphNode, 
            t_TyGraphLink, t_TyInputStream,
            t_TyBaseInputIter, t_TyFIsConstIterator >           _TyThis;
  typedef t_TyBaseInputIter                                     _TyBase;
  typedef _graph_iter_const_base<  t_TyGraphNode, t_TyGraphLink, t_TyFIsConstIterator > _tyConstIterBase;
public:

  typedef _TyBase               _TyIterBase;  // This type supported by all graph iterators.

  typedef _graph_inplace_input_iterator<  t_TyGraphNode, t_TyGraphLink, t_TyInputStream,
                                          t_TyBaseInputIter, __false_type >   iterator;
  typedef _graph_inplace_input_iterator<  t_TyGraphNode, t_TyGraphLink, t_TyInputStream,
                                          t_TyBaseInputIter, __true_type >    const_iterator;

  typedef t_TyInputStream                     _TyInputStream;
  typedef typename _TyInputStream::_TyInitArg _TyInitArg;
  typedef typename _TyInputStream::_TyIONodeEl _TyIONodeEl;
  typedef typename _TyInputStream::_TyIOLinkEl _TyIOLinkEl;

  typedef typename _TyBase::_TyGraphNodeCQ _TyGraphNodeCQ;
  typedef typename _TyBase::_TyGraphLinkCQ _TyGraphLinkCQ;
  typedef typename _tyConstIterBase::node_reference node_reference;
  typedef typename _tyConstIterBase::link_reference link_reference;
  typedef typename _tyConstIterBase::node_pointer node_pointer;
  typedef typename _tyConstIterBase::link_pointer link_pointer;
  typedef typename _tyConstIterBase::_TyGraphNode _TyGraphNode;
  typedef typename _tyConstIterBase::_TyGraphLink _TyGraphLink;
  typedef typename _tyConstIterBase::_TyGraphNodeBaseBase  _TyGraphNodeBaseBase;
  typedef typename _tyConstIterBase::_TyGraphLinkBaseBase  _TyGraphLinkBaseBase;

  _TyInputStream  m_is; // The input stream object.

  _graph_inplace_input_iterator(  _TyInitArg _iia,
                                  _TyIONodeEl const & _rione = _TyIONodeEl(),
                                  _TyIOLinkEl const & _riole = _TyIOLinkEl() )
    : _TyBase( m_is ),
      m_is( _iia, _rione, _riole )
  {
    // Set up callbacks in base:
    m_pmfnReadNode = static_cast< typename _TyBase::_TyPMFnReadNode >
      ( &_TyThis::_ReadNode );
    m_pmfnReadLink = static_cast< typename _TyBase::_TyPMFnReadLink >
      ( &_TyThis::_ReadLink );
  }

  ~_graph_inplace_input_iterator()
  {
  }

  _TyGraphNodeCQ *  PGNCur() const _STLP_NOTHROW
  {
    return const_cast< _TyGraphNodeCQ * >( static_cast< _TyGraphNode * >( PGNBCur() ) );
  }
  _TyGraphLinkCQ *  PGLCur() const _STLP_NOTHROW
  {
    return const_cast< _TyGraphLinkCQ * >( static_cast< _TyGraphLink * >( PGLBCur() ) );
  }

  // note: may not has a node ( may be at end of iteration ).
  node_reference    RNodeEl() const _STLP_NOTHROW
  {
    return const_cast< node_reference >( *static_cast< _TyGraphNode * >( PGNBCur() ) );
  }
  // note: may not have a link!
  link_reference    RLinkEl() const _STLP_NOTHROW
  {
    return const_cast< link_reference >( *static_cast< _TyGraphLink * >( PGLBCur() ) );
  }

  // The way this works: if the link_pointer is non-null then the iteration is currently
  //  at a link. Otherwise the iteration either at node_pointer or at the end() if node_pointer null.
  pair< link_pointer, node_pointer >  PairCur() const _STLP_NOTHROW
  {
    return pair< link_pointer, node_pointer >
      ( PGLBCur() ? &RLinkEl() : 0, PGNBCur() ? &RNodeEl() : 0 );
  }

  void  Next( _TyGraphNode * _pgnbReadInto, _TyGraphLink * _pglbReadInto )
  {
    _Next( _pgnbReadInto, _pglbReadInto );
  }
    
protected:

  void  _ReadNode( _TyGraphNodeBaseBase * _pgnb )
  {
    m_is._ReadNode( static_cast< _TyGraphNode* >( _pgnb ) );
  }
  void  _ReadLink( _TyGraphLinkBaseBase * _pglb )
  {
    m_is._ReadLink( static_cast< _TyGraphLink* >( _pglb ) );
  }
};


__DGRAPH_END_NAMESPACE

#endif __GR_INIP_H
