#ifndef __GR_SAFE_H
#define __GR_SAFE_H

// _gr_safe.h

// Default implementation of safe graph objects - this is a rather expensive - but also very
//  safe implementation - all viewing of nodes and links is referenced and the viewer is
//  updated upon destruction of the node/link. This allows essentially constant time 
//  updating of viewers ( and constant time determination of existence of viewers ). This
//  enables constant time path augmentation. Node and link removal will be proportional to
//  the number of viewers on the link/node.

__DGRAPH_BEGIN_NAMESPACE

typedef signed char             _TyPositionType;

struct _graph_connection_link;
typedef _graph_connection_link  _TyGraphConnectionLink;

enum EGraphPositionTypes
{
  s_egptHeadPosition = 1,
    // This is a position at the head of a parent or child list.
    //  This also indicates that the object containing the pointer is a graph node.
    //  Similarly the lack of this flag indicates that the object is a graph node.
  s_egptChildPosition = 0x00,
  s_egptParentPosition = 0x80
    // Indicates that the pointer is a structure member 
    //  of a child or parent link list object.
};

// Operations:
inline bool PT_FIsHeadPosition( _TyPositionType const & _pt )
{
  return _TyPositionType( s_egptHeadPosition ) & _pt;
}
inline bool PT_FIsParentPosition( _TyPositionType const & _pt )
{
  return _TyPositionType( s_egptParentPosition ) & _pt;
}

// Connection link - this implements referenced viewers of various objects:
// We need this to be a doubly linked list as clients ( GraphPathNode, graph_point_iterator_base, etc. )
//  need to be able to remove the link by merely having a pointer to it.
// TODO: Issue: thread-safe access to this data structure? _graph_connection_link_ts and 
//                then need to templatize everything by t_TyConnectionLink.

// Type of graph connection link.
enum EGraphConnectionLink
{
  s_egclGraph = 0,
    // This graph connection link due to a viewing 
    //  of this graph node by graph container.
  s_egclGraphNodeIterator,
    // This graph connection link due to a viewing 
    //  of this graph node by a node iterator.
  s_egclLinkPositionIterator,
    // This graph connection link due to a viewing 
    //  of this graph link by a link position iterator.
  s_egclGraphLinkIdentIterator,
    // This graph connection link due to a viewing
    //  of this graph link by a link identity iterator.
  s_egclGraphPathIterator,
    // This graph connection link due to a viewing
    //  of this graph link by a graph path iterator.
  s_egclShadowGraphObject
    // This graph connection link due to a viewing
    //  of this graph node by a shadow graph object.
};

// This should be a POD.
struct _graph_connection_link
{
  EGraphConnectionLink      m_egclType;
  void *                    m_pvConnection; // Connection.
  _graph_connection_link ** m_ppgclPrevNext;// Pointer to address of previous's next pointer ( allows removal with a single pointer ).
  _graph_connection_link *  m_pgclNext;     // Next.

  void  insert_link( _graph_connection_link *& _pgclAfter )
  {
    m_ppgclPrevNext = &_pgclAfter;
    m_pgclNext = _pgclAfter;
    if ( _pgclAfter )
    {
      _pgclAfter->m_ppgclPrevNext = &m_pgclNext;
    }
    _pgclAfter = this;
  }

  void  remove_link()
  {
    if ( m_pgclNext )
    {
      m_pgclNext->m_ppgclPrevNext = m_ppgclPrevNext;
    }
    *m_ppgclPrevNext = m_pgclNext;
  }
};

// Safe graph link:

// POD
template < class t_TyGraphNodeSafe >
class _graph_link_safe_base : 
  public _graph_link_base< typename t_TyGraphNodeSafe::_TyGraphNodeBase >
{
  typedef _graph_link_safe_base< t_TyGraphNodeSafe >                        _TyThis;
  typedef _graph_link_base< typename t_TyGraphNodeSafe::_TyGraphNodeBase >  _TyBase;

public:

  typedef t_TyGraphNodeSafe                           _TyGraphNodeSafe;
  typedef t_TyGraphNodeSafe                           _TyGraphNodeBase;
  typedef typename _TyGraphNodeBase::_TyGraphNodeBase _TyGraphNodeBaseBase;
  typedef _TyThis                                     _TyGraphLinkSafe;
  typedef _TyBase                                     _TyGraphLinkBase;
  
  _TyPositionType             m_ptNextParentType; // The type information for the next parent pointer.
  unsigned char               m_rgucDummy[sizeof(void*)-sizeof(_TyPositionType)];// Space for placement.
  _TyPositionType             m_ptNextChildType;  // The type information for the next child pointer.
  _TyGraphConnectionLink *    m_pgclHead;     // Head of graph path nodes connection link linked list.

  void      PushConnection( _TyGraphConnectionLink * _pglclgp ) __STL_NOTHROW
  {
    _pglclgp->insert_link( m_pgclHead );
  }

  void      Init() __STL_NOTHROW
  {
    m_pgclHead = 0;
    _InitPositionTypes();
  }

private:
  void  _InitPositionTypes( ) __STL_NOTHROW
  {
    m_ptNextParentType = _TyPositionType( s_egptParentPosition );
    m_ptNextChildType = _TyPositionType( s_egptChildPosition );
  }
public:

  static _TyPositionType & RPTGetPositionType( _TyGraphLinkBase ** _ppglb )
  {
    return *reinterpret_cast< _TyPositionType* >( reinterpret_cast< char*>( _ppglb ) + 
      offsetof(_TyGraphLinkSafe,m_ptNextParentType)-
      offsetof(_TyGraphLinkBase,m_pglbNextParent) );
  }

  static void AssertValidPT( const _TyPositionType & _rpt ) __STL_NOTHROW
  {
    // should be no garbage:
    assert( !( _rpt & ~( s_egptHeadPosition | s_egptParentPosition ) ) );
  }

  // Return the pointer to the appropriate graph object from the address of the next pointer:
  static void PGrObjFromPPGLB(  _TyGraphLinkBase ** _ppglb, 
                                _TyGraphLinkSafe *& _pgls, 
                                _TyGraphNodeSafe *& _pgns ) __STL_NOTHROW
  {
    // Structs must be set up appropriately - may need to push some packing pragmas if these assert(s) fail.
    assert( offsetof(_TyGraphLinkSafe,m_ptNextParentType)-offsetof(_TyGraphLinkBase,m_pglbNextParent) ==
            offsetof(_TyGraphNodeSafe,m_ptNextParentType)-offsetof(_TyGraphNodeBase,m_pglbParents) );
    assert( offsetof(_TyGraphLinkSafe,m_ptNextChildType)-offsetof(_TyGraphLinkBase,m_pglbNextChild) ==
            offsetof(_TyGraphNodeSafe,m_ptNextChildType)-offsetof(_TyGraphNodeBase,m_pglbChildren) );

    _TyPositionType & _rpt = _TyGraphLinkSafe::RPTGetPositionType( _ppglb );
    // We will either reference the graph node ( if this is the head of the list )
    //  or the graph link object in which this pointer is contained.
    AssertValidPT( _rpt );
    bool fHeadPos = PT_FIsHeadPosition( _rpt );
    bool fParentPos = PT_FIsParentPosition( _rpt );
    if ( fHeadPos )
    {
      _pgls = 0;  // This is checked by the caller first.
      _pgns = static_cast< _TyGraphNodeSafe * >( fParentPos ? 
        _TyGraphNodeBaseBase::PGNBGetThisFromPPGLBParentList( _ppglb ) :
        _TyGraphNodeBaseBase::PGNBGetThisFromPPGLBChildList( _ppglb ) );
    }
    else
    {
      // Then in a link:
      _pgls = static_cast< _TyGraphLinkSafe * >( fParentPos ? 
        _TyGraphLinkBase::PGLBGetThisFromPPGLBNextParent( _ppglb ) :
        _TyGraphLinkBase::PGLBGetThisFromPPGLBNextChild( _ppglb ) );
    }
  }

  void InsertParent( _TyThis ** _ppgl )
  {
    _TyGraphLinkBase::InsertParent( (_TyGraphLinkBase**)_ppgl );
  }
  void InsertChild( _TyThis ** _ppgl )
  {
    _TyGraphLinkBase::InsertChild( (_TyGraphLinkBase**)_ppgl );
  }
};

// Safe graph node base class - maintains references to viewers.
// POD
class _graph_node_safe_base : public _graph_node_base
{
#ifndef NDEBUG
  // Needs access to assert.
  template < class t__TyGraphNodeSafe >
  friend class _graph_link_safe_base;
#endif !NDEBUG

  template <  class t_TyPathNodeSafe, 
    class t_TyPathNodeBaseAllocator,
    class t_TyPathNodeSafeAllocator >
    friend class _graph_safe_base;

  typedef _graph_node_base          _TyBase;
  typedef _graph_node_safe_base     _TyThis;

  _TyPositionType             m_ptNextParentType; // The type information for the next parent pointer.
  unsigned char               m_rgucDummy[sizeof(void*)-sizeof(_TyPositionType)];// Space for placement.
  _TyPositionType             m_ptNextChildType;  // The type information for the next child pointer.
  _TyGraphConnectionLink  *   m_pgclHead;         // Head of graph node connection link linked list.

  __INLINE void _InitPositionTypes( ) __STL_NOTHROW
  {
    m_ptNextParentType = _TyPositionType( s_egptHeadPosition | s_egptParentPosition );
    m_ptNextChildType = _TyPositionType( s_egptHeadPosition | s_egptChildPosition );
  }

public:
  
  typedef _TyThis                             _TyGraphNodeSafe;
  typedef _TyBase                             _TyGraphNodeBase;
  typedef _graph_link_safe_base< _TyThis >    _TyGraphLinkSafe;
  // REVIEW: <dbien>: should we make the base the graph link base ?
  typedef _TyGraphLinkSafe                    _TyGraphLinkBase;
  typedef _TyGraphLinkSafe::_TyGraphLinkBase  _TyGraphLinkBaseBase;

  __INLINE void Init() __STL_NOTHROW
  { 
    _TyBase::Init();
    m_pgclHead = 0;
    _InitPositionTypes();
  }

  __INLINE void     PushConnection( _TyGraphConnectionLink * _pgnclgp ) __STL_NOTHROW
  {
    _pgnclgp->insert_link( m_pgclHead );
  }
};

template < class t_TyGraphNodeSafe, class t_TyGraphLinkSafe >
class _graph_path_node_safe_base
  : public _graph_path_node_base< typename t_TyGraphNodeSafe::_TyGraphNodeBase, 
                                  typename t_TyGraphLinkSafe::_TyGraphLinkBase >
{
  template <  class t_TyPathNodeSafe, 
    class t_TyPathNodeBaseAllocator,
    class t_TyPathNodeSafeAllocator >
    friend class _graph_safe_base;

  typedef _graph_path_node_base<  typename t_TyGraphNodeSafe::_TyGraphNodeBase, 
                                  typename t_TyGraphLinkSafe::_TyGraphLinkBase >  _TyBase;
  typedef _graph_path_node_safe_base< t_TyGraphNodeSafe, t_TyGraphLinkSafe >      _TyThis;

  // The graph node to which this iterator is connected is being deinitialized.
  // We don't need to unlink the connection link.
  void  _node_deinit() __STL_NOTHROW
  {
    m_pgnbNode = 0;
  }

  // The graph link to which this iterator is connected is being deinitialized.
  // We don't need to unlink the connection link.
  void  _link_deinit() __STL_NOTHROW
  {
    m_pglbLink = 0;
  }

public:

  typedef _TyBase             _TyPathNodeBase;
  typedef _TyThis             _TyPathNodeSafe;
  typedef t_TyGraphNodeSafe   _TyGraphNodeSafe;
  typedef t_TyGraphLinkSafe   _TyGraphLinkSafe;

  _TyGraphConnectionLink m_gclNode;   // link to this graph_path_node from the graph_node.
  _TyGraphConnectionLink m_gclLink;   // link to this graph_path_node from the graph_link for the next node in the path.

  explicit _graph_path_node_safe_base( _TyBase const & _r )
    : _TyBase( _r )
  {
    _init();
  }

  _graph_path_node_safe_base( _TyGraphNodeSafe * _pgns, 
                              _TyGraphLinkSafe * _pgls )
    : _TyBase( _pgns, _pgls )
  { 
    _init();
  }

  _TyGraphNodeSafe * PGNS() { return static_cast< _TyGraphNodeSafe * >( m_pgnbNode ); }
  _TyGraphLinkSafe * PGLS() { return static_cast< _TyGraphLinkSafe * >( m_pglbLink ); }

protected:

  void  _init()
  {
    m_gclNode.m_pvConnection = (void*)this;
    m_gclNode.m_egclType = s_egclGraphPathIterator;
    m_gclLink.m_pvConnection = (void*)this;
    m_gclLink.m_egclType = s_egclGraphPathIterator;
  }
};

// Pre-declare safe iterators:
template < class _TyGraphNodeSafe, class _TyGraphLinkSafe > class _graph_node_iterator_base_safe;
template < class _TyGraphNodeSafe, class _TyGraphLinkSafe > class _graph_link_pos_iterator_base_safe;
template < class _TyGraphNodeSafe, class _TyGraphLinkSafe > class _graph_link_ident_iterator_base_safe;
template < class t_TyPathNodeSafe, class t_TyPathNodeSafeAllocator > class _graph_path_iterator_base_safe;

// Safe graph base class:
template <  class t_TyPathNodeSafe, 
            class t_TyPathNodeBaseAllocator,
            class t_TyPathNodeSafeAllocator >
class _graph_safe_base
  : public _alloc_base< t_TyPathNodeSafe, t_TyPathNodeSafeAllocator >,
    public _graph_base< typename t_TyPathNodeSafe::_TyPathNodeBase,
                        t_TyPathNodeBaseAllocator >
{
  typedef _graph_safe_base< t_TyPathNodeSafe, t_TyPathNodeBaseAllocator, 
                        t_TyPathNodeSafeAllocator >                         _TyThis;
protected:
  typedef _alloc_base< t_TyPathNodeSafe, t_TyPathNodeSafeAllocator >        _TyBaseAllocPathNodeSafe;
public:

  typedef typename t_TyPathNodeSafe::_TyGraphNodeSafe   _TyGraphNodeSafe;
  typedef typename t_TyPathNodeSafe::_TyGraphLinkSafe   _TyGraphLinkSafe;
  typedef t_TyPathNodeSafe                              _TyPathNodeSafe;

  // These definitions supercede those from the base class (duh): 
  typedef typename _TyGraphNodeSafe::_TyGraphNodeBase   _TyGraphNodeBaseBase;
  typedef typename _TyGraphLinkSafe::_TyGraphLinkBase   _TyGraphLinkBaseBase;
  typedef typename t_TyPathNodeSafe::_TyPathNodeBase    _TyPathNodeBaseBase;
  typedef _TyGraphNodeSafe                              _TyGraphNodeBase;
  typedef _TyGraphLinkSafe                              _TyGraphLinkBase;
  typedef _TyPathNodeSafe                               _TyPathNodeBase;

  typedef t_TyPathNodeSafeAllocator                                 _TyPathNodeSafeAllocatorAsPassed;
  typedef typename _TyBaseAllocPathNodeSafe::_TyAllocatorType       _TyPathNodeSafeAllocator;

  typedef _allocator_set_safe_base< t_TyPathNodeBaseAllocator, 
                                    t_TyPathNodeSafeAllocator >     _TyAllocatorSet;

private:

  typedef _graph_base< _TyPathNodeBaseBase, t_TyPathNodeBaseAllocator >   _TyBaseGraph;

public:

  typedef _TyThis _TySafeGraphBase;
  typedef _TyBaseGraph _TyGraphBaseBase;

  // Safe allocator types:
  typedef _graph_node_iterator_base_safe< _TyGraphNodeSafe, _TyGraphLinkSafe >        _TyGraphNodeIterBaseSafe;
  typedef _graph_link_pos_iterator_base_safe< _TyGraphNodeSafe, _TyGraphLinkSafe >    _TyGraphLinkPosIterBaseSafe;
  typedef _graph_link_ident_iterator_base_safe< _TyGraphNodeSafe, _TyGraphLinkSafe >  _TyGraphLinkIdentIterBaseSafe;
  typedef _graph_path_iterator_base_safe< t_TyPathNodeSafe, t_TyPathNodeSafeAllocator > _TyGraphPathIterBaseSafe;

  // Passing safe allocator types:
  typedef _graph_iterator_pass< _TyGraphNodeSafe, _TyGraphLinkSafe, 
                                __true_type, _TyGraphNodeIterBaseSafe >       _TyGraphNodeIterSafePassConst;
  typedef _graph_iterator_pass< _TyGraphNodeSafe, _TyGraphLinkSafe, 
                                __true_type, _TyGraphLinkPosIterBaseSafe >    _TyGraphLinkPosIterSafePassConst;
  typedef _graph_iterator_pass< _TyGraphNodeSafe, _TyGraphLinkSafe, 
                                __true_type, _TyGraphLinkIdentIterBaseSafe >  _TyGraphLinkIdentIterSafePassConst;
  typedef _graph_path_iterator_pass<  t_TyPathNodeSafe, t_TyPathNodeSafeAllocator,
                                      __true_type, _TyGraphPathIterBaseSafe > _TyGraphPathIterSafePassConst;

  typedef _graph_iterator_pass< _TyGraphNodeSafe, _TyGraphLinkSafe, 
                                __false_type, _TyGraphNodeIterBaseSafe >        _TyGraphNodeIterSafePassNonConst;
  typedef _graph_iterator_pass< _TyGraphNodeSafe, _TyGraphLinkSafe, 
                                __false_type, _TyGraphLinkPosIterBaseSafe >     _TyGraphLinkPosIterSafePassNonConst;
  typedef _graph_iterator_pass< _TyGraphNodeSafe, _TyGraphLinkSafe, 
                                __false_type, _TyGraphLinkIdentIterBaseSafe >   _TyGraphLinkIdentIterSafePassNonConst;
  typedef _graph_path_iterator_pass<  t_TyPathNodeSafe, t_TyPathNodeSafeAllocator,
                                      __false_type, _TyGraphPathIterBaseSafe >  _TyGraphPathIterSafePassNonConst;

  _graph_safe_base( t_TyPathNodeBaseAllocator const & _allocPathNodeBase,
                    t_TyPathNodeSafeAllocator const & _allocPathNodeSafe ) : 
    _TyBaseGraph( _allocPathNodeBase ),
    _TyBaseAllocPathNodeSafe( _allocPathNodeSafe )
  {
    _init();
  }

  _graph_safe_base( _TyAllocatorSet const & _rAllocSet ) : 
    _TyBaseGraph( _rAllocSet.m_allocPathNodeBase ),
    _TyBaseAllocPathNodeSafe( _rAllocSet.m_allocPathNodeSafe )
  {
    _init();
  }

  explicit _graph_safe_base( _TyThis const & _r ) : 
    _TyBaseGraph( static_cast< _TyBaseGraph const & >( _r ) ),
    _TyBaseAllocPathNodeSafe( _r.get_safe_path_allocator() )
  {
    _init();
  }

  // Specialize contruction with non-safe base class - we will attempt to get the
  //  safe path node allocator from the non-safe:
  template < class t__TyPathNodeBaseBase, class t__TyPathNodeBaseAllocator >
  explicit _graph_safe_base( _graph_base< t__TyPathNodeBaseBase, t__TyPathNodeBaseAllocator > const & _r ) :
    _TyBaseGraph( _r ),
    _TyBaseAllocPathNodeSafe( _r.get_base_path_allocator() )
  {
    _init();
  }

  template < class t_TyGraphSafe >
  explicit _graph_safe_base( t_TyGraphSafe const & _r ) : 
    _TyBaseGraph( static_cast< typename t_TyGraphSafe::_TyGraphBaseBase const & >( _r ) ),
    _TyBaseAllocPathNodeSafe( _r.get_safe_path_allocator() )
  {
    assert( 0 );// REVIEW:Do we ever get here ?
    _init();
  }

  ~_graph_safe_base()
  {
    // Due to derived class, we should never have a root node here:
    assert( !_GetRootNode() );
  }

  // Allocation stuff:
  _TyPathNodeSafeAllocator const &  get_safe_path_allocator_ref() const __STL_NOTHROW { return _TyBaseAllocPathNodeSafe::get_allocator_ref(); }
  _TyPathNodeSafeAllocator &        get_safe_path_allocator_ref() __STL_NOTHROW { return _TyBaseAllocPathNodeSafe::get_allocator_ref(); }
  _TyPathNodeSafeAllocatorAsPassed  get_safe_path_allocator() const __STL_NOTHROW { return _TyBaseAllocPathNodeSafe::get_allocator(); }

  // iterator access:
  _TyGraphNodeIterSafePassNonConst  get_safe_node_iterator( _TyGraphNodeBase * _pgnb = 0 )
  {
    return _TyGraphNodeIterSafePassNonConst( _pgnb );
  }
  _TyGraphNodeIterSafePassConst     get_safe_node_iterator( const _TyGraphNodeBase * _pgnb = 0 ) const
  {
    return _TyGraphNodeIterSafePassConst( _pgnb );
  }
  _TyGraphLinkPosIterSafePassNonConst get_safe_link_pos_iterator( _TyGraphLinkBase ** _ppglb )
  {
    return _TyGraphLinkPosIterSafePassNonConst( _ppglb );
  }
  _TyGraphLinkPosIterSafePassConst  get_safe_link_pos_iterator( const _TyGraphLinkBase ** _ppglb ) const
  {
    return _TyGraphLinkPosIterSafePassConst( _ppglb );
  }
  _TyGraphLinkIdentIterSafePassNonConst get_safe_link_ident_iterator( _TyGraphLinkBase * _pglb )
  {
    return _TyGraphLinkIdentIterSafePassNonConst( _pglb );
  }
  _TyGraphLinkIdentIterSafePassConst    get_safe_link_ident_iterator( const _TyGraphLinkBase * _pglb ) const
  {
    return _TyGraphLinkIdentIterSafePassConst( _pglb );
  }
  _TyGraphPathIterSafePassNonConst  get_safe_path_iterator( _TyGraphNodeBase * _pgnb = 0 )
  {
    return _TyGraphPathIterSafePassNonConst( _pgnb, get_safe_path_allocator() );
  }
  _TyGraphNodeIterSafePassConst     get_safe_path_iterator( const _TyGraphNodeBase * _pgnb = 0 ) const
  {
    return _TyGraphNodeIterSafePassConst( _pgnb, get_safe_path_allocator() );
  }

protected:
  
  // This is the connection link for the root node:
  _TyGraphConnectionLink  m_gclRoot;

  void  _init() __STL_NOTHROW
  {
    m_gclRoot.m_pvConnection = (void*)this;
    m_gclRoot.m_egclType = s_egclGraph;
  }

  _TyGraphNodeSafe * _GetRootNode() __STL_NOTHROW
  {
    return static_cast< _TyGraphNodeSafe * >( _TyBaseGraph::_GetRootNode() );
  }
  const _TyGraphNodeSafe * _GetRootNode() const __STL_NOTHROW
  {
    return static_cast< const _TyGraphNodeSafe * >( _TyBaseGraph::_GetRootNode() );
  }


  void _SetRootNode( _TyGraphNodeSafe * _pgnsRoot ) __STL_NOTHROW
  {
    _DereferenceRoot();
    _TyBaseGraph::_SetRootNode( _pgnsRoot );
    if ( _GetRootNode() ) 
    {
      _GetRootNode()->PushConnection( &m_gclRoot );
    }
  }

  void _DereferenceRoot() __STL_NOTHROW
  {
    if ( _GetRootNode() )
    {
      m_gclRoot.remove_link();
    }
  }

  // The node which this graph is viewing ( i.e. the root ) is going away:
  void  _node_deinit() __STL_NOTHROW
  {
    // just nullify our node reference:
    _TyBaseGraph::_SetRootNode( 0 );
  }

  // This method called when any safe node is being destroyed:
  static void _deinit_node( _TyGraphNodeBase * _pgnbDeinit ) __STL_NOTHROW
  {
    // The node may have objects connected to it - need to inform them of object destruction.
    // Move through all the connection links - no need to waste time unlinking - just
    //  deallocate on the fly.
    for ( _TyGraphConnectionLink * pgcl = _pgnbDeinit->m_pgclHead;
          pgcl; )
    {
      _TyGraphConnectionLink * pgclCur = pgcl;
      pgcl = pgcl->m_pgclNext;  // advance now, allocation is owned by the viewer.

      switch( pgclCur->m_egclType )
      {
        case s_egclGraph:
        {
          _TyThis * pg = static_cast< _TyThis * >( pgclCur->m_pvConnection );
          assert( pg->_GetRootNode() == _pgnbDeinit );
          pg->_node_deinit();
        }
        break;

        case s_egclGraphNodeIterator:
        {
          // node iterator:
          _TyGraphNodeIterBaseSafe *  pni = 
            static_cast< _TyGraphNodeIterBaseSafe* >( pgclCur->m_pvConnection );
          assert( pni->PGNBCur() == _pgnbDeinit );
          pni->_node_deinit();
        }
        break;

        case s_egclLinkPositionIterator:
        {
          // link position iterator - connected to the head of a relation list:
          _TyGraphLinkPosIterBaseSafe  *  plpi = 
              static_cast< _TyGraphLinkPosIterBaseSafe* >( pgclCur->m_pvConnection );
          plpi->_node_deinit();
        }
        break;

        case s_egclGraphPathIterator:
        {
          t_TyPathNodeSafe *  ppns = 
            static_cast< t_TyPathNodeSafe* >( pgclCur->m_pvConnection );
          //assert( ppns->PGNB() == static_cast< _TyGraphNodeBaseBase* >( _pgnbDeinit ) );
          ppns->_node_deinit();
        }
        break;

#ifdef __GR_USESHADOWSTUFF
        case s_egclShadowGraphObject:
        {
          typedef _sgraph_element_base< _TyGraphNodeSafe >  _TyShadowNodeElBase;
          _TyShadowNodeElBase * psneb = static_cast< _TyShadowNodeElBase * >( pgclCur->m_pvConnection );
          psneb->_element_deinit();
        }
        break;
#endif __GR_USESHADOWSTUFF

        default:
        {
          assert( 0 );
        }
        break;
      }
    }
  }

  // This method called when any safe link is being destroyed:
  static void _deinit_link( _TyGraphLinkBase * _pgnbDeinit ) __STL_NOTHROW
  {
    // The link may have objects connected to it - need to inform them of object destruction.
    // Move through all the connection links - no need to waste time unlinking - just
    //  deallocate on the fly.
    for ( _TyGraphConnectionLink * pgcl = _pgnbDeinit->m_pgclHead;
          pgcl; )
    {
      _TyGraphConnectionLink * pgclCur = pgcl;
      pgcl = pgcl->m_pgclNext;  // advance now will be deallocated by the viewer.

      switch( pgclCur->m_egclType )
      {
        case s_egclLinkPositionIterator:
        {
          // link iterator:
          _TyGraphLinkPosIterBaseSafe  *  plpi = 
              static_cast< _TyGraphLinkPosIterBaseSafe* >( pgclCur->m_pvConnection );
          plpi->_link_deinit();
        }
        break;

        case s_egclGraphLinkIdentIterator:
        {
          _TyGraphLinkIdentIterBaseSafe * plii = 
            static_cast< _TyGraphLinkIdentIterBaseSafe* >( pgclCur->m_pvConnection );
          plii->_link_deinit();
        }
        break;

        case s_egclGraphPathIterator:
        {
          t_TyPathNodeSafe *  ppns = 
            static_cast< _TyPathNodeSafe* >( pgclCur->m_pvConnection );
          ppns->_link_deinit();
        }
        break;

#ifdef __GR_USESHADOWSTUFF
        case s_egclShadowGraphObject:
        {
          typedef _sgraph_element_base< _TyGraphLinkSafe >  _TyShadowLinkElBase;
          _TyShadowLinkElBase * psleb = static_cast< _TyShadowLinkElBase * >( pgclCur->m_pvConnection );
          psleb->_element_deinit();
        }
        break;
#endif __GR_USESHADOWSTUFF

        default:
        {
          assert( 0 );
        }
        break;
      }
    }
  }
};

__DGRAPH_END_NAMESPACE

#endif __GR_SAFE_H
