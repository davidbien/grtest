#ifndef __GR_INPW_H
#define __GR_INPW_H

// _gr_inpt.h

// This module defines the input iterator typed wrapper for graphs.

__DGRAPH_BEGIN_NAMESPACE

template <  class t_TyGraph, class t_TyInputStream,
            class t_TyBaseInputIter, class t_TyFIsConstIterator >
struct _graph_input_iterator
  : public t_TyBaseInputIter,
    public _graph_iter_const_base<  typename t_TyGraph::_TyGraphNode, 
                                    typename t_TyGraph::_TyGraphLink, 
                                    t_TyFIsConstIterator >
{
private:
  typedef _graph_input_iterator< t_TyGraph, t_TyInputStream,
            t_TyBaseInputIter, t_TyFIsConstIterator >           _TyThis;
  typedef t_TyBaseInputIter                                     _TyBase;
  typedef _graph_iter_const_base<  typename t_TyGraph::_TyGraphNode,
	  typename t_TyGraph::_TyGraphLink,
	  t_TyFIsConstIterator >	_tyConstIterBase;
public:

  typedef _TyBase               _TyIterBase;  // This type supported by all graph iterators.

  typedef _graph_input_iterator<  t_TyGraph, t_TyInputStream,
                                  t_TyBaseInputIter, std::false_type >   iterator;
  typedef _graph_input_iterator<  t_TyGraph, t_TyInputStream,
                                  t_TyBaseInputIter, std::true_type >    const_iterator;

  typedef t_TyInputStream                     _TyInputStream;
  typedef typename _TyInputStream::_TyInitArg _TyInitArg;
  typedef typename _TyInputStream::_TyIONodeEl _TyIONodeEl;
  typedef typename _TyInputStream::_TyIOLinkEl _TyIOLinkEl;
  typedef typename _tyConstIterBase::_TyGraphNodeCQ _TyGraphNodeCQ;
  typedef typename _tyConstIterBase::_TyGraphLinkCQ _TyGraphLinkCQ;
  typedef typename _tyConstIterBase::node_reference node_reference;
  typedef typename _tyConstIterBase::link_reference link_reference;
  typedef typename _tyConstIterBase::node_pointer node_pointer;
  typedef typename _tyConstIterBase::link_pointer link_pointer;
  typedef typename _tyConstIterBase::_TyGraphNode _TyGraphNode;
  typedef typename _tyConstIterBase::_TyGraphLink _TyGraphLink;
  typedef typename _tyConstIterBase::_TyGraphNodeBaseBase  _TyGraphNodeBaseBase;
  typedef typename _tyConstIterBase::_TyGraphLinkBaseBase  _TyGraphLinkBaseBase;

  _TyInputStream  m_is; // The input stream object.

  t_TyGraph & m_rg; // Graph space to read into.

  _graph_input_iterator(  t_TyGraph & _rg,
                          _TyInitArg _iia,
                          typename _TyBase::_TyAllocatorAsPassed const & _rAlloc,
                          _TyIONodeEl const & _rione = _TyIONodeEl(),
                          _TyIOLinkEl const & _riole = _TyIOLinkEl(),
                          bool _fConstructInReverse = false )
    : _TyBase( m_is, _rAlloc, _fConstructInReverse ),
      m_is( _iia, _rione, _riole ),
      m_rg( _rg )
  {
    // Set up callbacks in base:
    _TyBase::m_pmfnAllocInitNode = static_cast< typename _TyBase::_TyPMFnAllocInitNode >
      ( &_TyThis::_AllocInitNode );
    _TyBase::m_pmfnDeallocateNode = static_cast< typename _TyBase::_TyPMFnDeallocateNode >
      ( &_TyThis::_DeallocateNode );
    _TyBase::m_pmfnAllocInitLink = static_cast< typename _TyBase::_TyPMFnAllocInitLink >
      ( &_TyThis::_AllocInitLink );
    _TyBase::m_pmfnDeallocateLink = static_cast< typename _TyBase::_TyPMFnDeallocateLink >
      ( &_TyThis::_DeallocateLink );
    _TyBase::m_pmfnReadNode = static_cast< typename _TyBase::_TyPMFnReadNode >
      ( &_TyThis::_ReadNode );
    _TyBase::m_pmfnReadLink = static_cast< typename _TyBase::_TyPMFnReadLink >
      ( &_TyThis::_ReadLink );
    _TyBase::m_pmfnDestructLinkEl = static_cast< typename _TyBase::_TyPMFnDestructLinkEl >
      ( &_TyThis::_DestructLinkEl );
    _TyBase::m_pmfnDestroySubGraph = static_cast< typename _TyBase::_TyPMFnDestroySubGraph >
      ( &_TyThis::_DestroySubGraph );
  }

  ~_graph_input_iterator()
  {
    if ( _TyBase::m_pglbConstructedEl )
    {
      _DestructLinkEl( _TyBase::m_pglbConstructedEl );
    }
    if ( _TyBase::m_pglbAllocedInited )
    {
      _DeallocateLink( _TyBase::m_pglbAllocedInited );
    }
    if ( _TyBase::m_pgnbTempRoot )
    {
      _DestroySubGraph( _TyBase::m_pgnbTempRoot ); 
    }
    if ( _TyBase::m_pgnbNewRoot )
    {
      _DestroySubGraph( _TyBase::m_pgnbNewRoot ); 
    }
  }

  _TyGraphNode * PGNTransferNewRoot() _BIEN_NOTHROW
    {
      _TyGraphNode * _pgnNewRoot = static_cast< _TyGraphNode * >( _TyBase::m_pgnbNewRoot );
      _TyBase::m_pgnbNewRoot = 0;
      return _pgnNewRoot;
    }

  _TyGraphNodeCQ *  PGNCur() const _BIEN_NOTHROW
  {
    return const_cast< _TyGraphNodeCQ * >( static_cast< _TyGraphNode * >( _TyBase::PGNBCur() ) );
  }
  _TyGraphLinkCQ *  PGLCur() const _BIEN_NOTHROW
  {
    return const_cast< _TyGraphLinkCQ * >( static_cast< _TyGraphLink * >( _TyBase::PGLBCur() ) );
  }

  // note: may not has a node ( may be at end of iteration ).
  node_reference    RNodeEl() const _BIEN_NOTHROW
  {
    return const_cast< node_reference >( *static_cast< _TyGraphNode * >( _TyBase::PGNBCur() ) );
  }
  // note: may not have a link!
  link_reference    RLinkEl() const _BIEN_NOTHROW
  {
    return const_cast< link_reference >( *static_cast< _TyGraphLink * >( _TyBase::PGLBCur() ) );
  }

  // The way this works: if the link_pointer is non-null then the iteration is currently
  //  at a link. Otherwise the iteration either at node_pointer or at the end() if node_pointer null.
  pair< link_pointer, node_pointer >  PairCur() const _BIEN_NOTHROW
  {
    return pair< link_pointer, node_pointer >
      ( _TyBase::PGLBCur() ? &RLinkEl() : 0, _TyBase::PGNBCur() ? &RNodeEl() : 0 );
  }

  _TyThis & operator ++()
  {
    _TyBase::_Next();
    return *this;
  }

protected:

  void  _AllocInitNode( _TyGraphNodeBaseBase *& _rpgnb )
  {
    _TyGraphNode *& _rpgn( (_TyGraphNode*&)_rpgnb );
    _rpgn = m_rg._allocate_node( );
    _rpgn->Init();
  }
  void  _DeallocateNode( _TyGraphNodeBaseBase * _pgnb )
  {
    if ( _pgnb )
    {
      m_rg._deallocate_node( static_cast< _TyGraphNode* >( _pgnb ) );
    }
  }

  void  _AllocInitLink( _TyGraphLinkBaseBase *& _rpglb )
  {
    _TyGraphLink *& _rpgl( (_TyGraphLink*&) _rpglb );
    _rpgl = m_rg._allocate_link();
    _rpgl->Init();
  }
  void  _DeallocateLink( _TyGraphLinkBaseBase * _pglb )
  {
    if ( _pglb )
    {
      m_rg._deallocate_link( static_cast< _TyGraphLink* >( _pglb ) );
    }
  }

  void  _ReadNode( _TyGraphNodeBaseBase * _pgnb )
  {
    m_is._ReadNode( static_cast< _TyGraphNode* >( _pgnb ) );
  }
  void  _ReadLink( _TyGraphLinkBaseBase * _pgnb )
  {
    m_is._ReadLink( static_cast< _TyGraphLink* >( _pgnb ) );
  }

  void  _DestructLinkEl( _TyGraphLinkBaseBase * _pglb )
  {
    if ( _pglb )
    {
      m_rg._destruct_link_el( static_cast< _TyGraphLink* >( _pglb ) );
    }
  }

  void  _DestroySubGraph( _TyGraphNodeBaseBase * _pgnb )
  {
    assert( _pgnb );
    m_rg.destroy_node( static_cast< _TyGraphNode * >( _pgnb ) );
  }
};

__DGRAPH_END_NAMESPACE

#endif //__GR_INPW_H
