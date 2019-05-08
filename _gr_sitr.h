#ifndef __GR_SITR
#define __GR_SITR

// _gr_sitr.h

// This module implements safe iterators for the graph.

#include <algorithm>

__DGRAPH_BEGIN_NAMESPACE

// Safe node iterator - a connection is maintained between the node and the iterator
//  such that if the node is destroyed then the iterator's pointer is nullified,
// Could allow user data and callback function.
template < class t_TyGraphNodeSafe, class t_TyGraphLinkSafe >
class _graph_node_iterator_base_safe
  : public _graph_node_iterator_base_notsafe< typename t_TyGraphNodeSafe::_TyGraphNodeBase, 
                                              typename t_TyGraphLinkSafe::_TyGraphLinkBase >
{
  template <  class t_TyPathNodeSafe, 
    class t_TyPathNodeBaseAllocator,
    class t_TyPathNodeSafeAllocator >
      friend class _graph_safe_base;

protected:
  // Get the base classes from the safe classes:
  typedef typename t_TyGraphNodeSafe::_TyGraphNodeBase  _TyGraphNodeBase;
  typedef typename t_TyGraphLinkSafe::_TyGraphLinkBase  _TyGraphLinkBase;

private:
  typedef _graph_node_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase > _TyIterBase;
  typedef _graph_node_iterator_base_safe< t_TyGraphNodeSafe, t_TyGraphLinkSafe >  _TyThis;

protected:

  _TyGraphConnectionLink  m_gclNode;  // link from graph-object to this object.

  void  _Reference( _TyGraphNodeBase * _pgnbCur ) _BIEN_NOTHROW
  {
    assert( !PGNSCur() );   // should always have valid or NULL value when graph connection is not referencing.
    _TyIterBase::m_pgnbCur = _pgnbCur;   // No throw after this.
    PGNSCur()->PushConnection( &m_gclNode );  // Crash here ? Perhaps you mixed non-safe and safe graphs.
  }

  void  _Dereference() _BIEN_NOTHROW
  {
    if ( _TyIterBase::PGNBCur() )
    {
      m_gclNode.remove_link();
    }
  }

  // The graph node to which this iterator is connected is being deinitialized.
  // We don't need to unlink the connection link.
  void  _node_deinit() _BIEN_NOTHROW
  {
    assert( _TyIterBase::PGNBCur() );
    _TyIterBase::Clear();
  }

  void  _init() _BIEN_NOTHROW
  {
    m_gclNode.m_pvConnection = (void*)this;
    m_gclNode.m_egclType = s_egclGraphNodeIterator;
  }

public:

  explicit _graph_node_iterator_base_safe( t_TyGraphNodeSafe * _pgnbCur = 0 ) _BIEN_NOTHROW
    : _TyIterBase( static_cast< _TyGraphNodeBase * >( 0 ) )  // In case of throw.
  {
    _init();
    if ( _pgnbCur )
    {
      _Reference( _pgnbCur );
    }
  }

  explicit _graph_node_iterator_base_safe( const _TyThis & _r ) _BIEN_NOTHROW
    : _TyIterBase( static_cast< _TyGraphNodeBase * >( 0 ) )
  {
    _init();
    if ( _r.PGNBCur() )
    {
      _Reference( _r.PGNBCur() );
    }
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template < class t_TySafeNodeIter >
  explicit _graph_node_iterator_base_safe( const t_TySafeNodeIter & _r ) _BIEN_NOTHROW
    : _TyIterBase( static_cast< _TyGraphNodeBase * >( 0 ) )
  {
    _init();
    if ( _r.PGNBCur() )
    {
      _Reference( _r.PGNBCur() );
    }
  }
#endif //_STLP_MEMBER_TEMPLATES

  // Non-copying copy constructor - this for allocator passing - allows generic wrapper.
#ifdef _STLP_MEMBER_TEMPLATES
  template < class t_TySafeNodeIter >
  explicit _graph_node_iterator_base_safe( const t_TySafeNodeIter & _r, __false_type ) _BIEN_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
  explicit _graph_node_iterator_base_safe( const _TyThis & _r, __false_type ) _BIEN_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
    : _TyIterBase( static_cast< _TyGraphNodeBase * >( 0 ) )
  {
    _init();
  }

  ~_graph_node_iterator_base_safe() _BIEN_NOTHROW
  {
    _Dereference();
  }

  void  Clear() _BIEN_NOTHROW
  {
    _Dereference();
    _TyIterBase::Clear();
  }

  t_TyGraphNodeSafe *     PGNSCur() const _BIEN_NOTHROW
  {
    return static_cast< t_TyGraphNodeSafe * >( _TyIterBase::m_pgnbCur );
  }

#ifdef _STLP_MEMBER_TEMPLATES
  // Once again this can be misused when multiple graph types ( safe and non-safe ) are mixed.
  template < class t_TyNodeIter >
  _TyThis & operator = ( const t_TyNodeIter & _r ) _BIEN_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
  _TyThis & operator = ( const _TyThis & _r ) _BIEN_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
  {
    SetPGNBCur( _r.PGNBCur() );
    return *this;
  }

  void      SetPGNBCur( _TyGraphNodeBase * _pgnbCur ) _BIEN_NOTHROW
  {
    if ( _TyIterBase::PGNBCur() )
    {
      m_gclNode.remove_link();
      if ( !!( _TyIterBase::m_pgnbCur = _pgnbCur ) )
      {
        PGNSCur()->PushConnection( &m_gclNode );
      }
    }
    else
    if ( _pgnbCur )
    {
      _Reference( _pgnbCur );
    }
  }

  // Set to value of passed non-NULL node - assume currently populated link.
  void      SetPGNBCur_Populated( _TyGraphNodeBase * _pgnb ) _BIEN_NOTHROW
  {
    assert( m_gclNode );
    assert( _pgnb );
    m_gclNode.remove_link();
    (_TyIterBase::m_pgnbCur = _pgnb)->PushConnection( &m_gclNode );
  }

#ifdef _STLP_MEMBER_TEMPLATES
  // Set to value of passed iterator - assume currently populated link - won't throw.
  // Typesafe problem when multiple graph types ( safe and non-safe ) are mixed.
  template < class t_TyNodeIter >
  void      Set_Populated( const t_TyNodeIter & _r ) _BIEN_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
  void      Set_Populated( const _TyThis & _r ) _BIEN_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
  {
    SetPGNBCur_Populated( _r.PGNBCur() );
  }

  void          GoParent( _TyGNIndex _u ) _BIEN_NOTHROW
  {
    assert( _TyIterBase::m_pgnbCur );
    // Remove the reference to the current node and re-use it to reference the
    //  new node:
    m_gclNode.remove_link();
    _TyIterBase::GoParent( _u );
    PGNSCur()->PushConnection( &m_gclNode );
  }
  void          GoChild( _TyGNIndex _u ) _BIEN_NOTHROW
  {
    assert( _TyIterBase::m_pgnbCur );
    // Remove the reference to the current node and re-use it to reference the
    //  new node:
    m_gclNode.remove_link();
    _TyIterBase::GoChild( _u );
    PGNSCur()->PushConnection( &m_gclNode );
  }

  // relation operators:
  // Re-declare so that gets overridden methods:
  void          GoRelation( bool _fChild, _TyGNIndex _u ) _BIEN_NOTHROW
  {
    if ( _fChild )  
      GoChild( _u );
    else
      GoParent( _u );
  }

#ifdef _STLP_MEMBER_TEMPLATES
  // make converion safe by catching any non-_TyThis objects - i.e. no derived-base conversion:
  template < class t_TySwap >
  void
  swap( t_TySwap & _r ) _BIEN_NOTHROW
  {
	  __ASSERT_SAME_TYPE( t_TySwap, _TyThis );
    _TyGraphNodeBase * pgnbCur = _TyIterBase::PGNBCur();
    SetPGNBCur( _r._TyIterBase::PGNBCur() );
    _r.SetPGNBCur( pgnbCur );
  }
#else //_STLP_MEMBER_TEMPLATES
  void
  swap( _TyThis & _r ) _BIEN_NOTHROW
  {
    _TyGraphNodeBase * pgnbCur = PGNBCur();
    SetPGNBCur( _r.PGNBCur() );
    _r.SetPGNBCur( pgnbCur );
  }
#endif //_STLP_MEMBER_TEMPLATES
};

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase >
class _graph_link_ident_iterator_base_notsafe;

// This iterator maintains a constant position within either the child or the parent list.
// It also has an end() position that is a valid insert before position.
template < class t_TyGraphNodeSafe, class t_TyGraphLinkSafe >
class _graph_link_pos_iterator_base_safe
: public _graph_link_pos_iterator_base_notsafe< typename t_TyGraphNodeSafe::_TyGraphNodeBase, 
  typename t_TyGraphLinkSafe::_TyGraphLinkBase >
{
   template < class t_TyPathNodeSafe, 
    class t_TyPathNodeBaseAllocator,
    class t_TyPathNodeSafeAllocator >
    friend class _graph_safe_base;

protected:
  // Get the base classes from the safe classes:
  typedef typename t_TyGraphNodeSafe::_TyGraphNodeBase  _TyGraphNodeBase;
  typedef typename t_TyGraphLinkSafe::_TyGraphLinkBase  _TyGraphLinkBase;

private:
  typedef _graph_link_pos_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >   _TyIterBase;
  typedef _graph_link_pos_iterator_base_safe< _TyGraphNodeBase, _TyGraphLinkBase >      _TyThis;

protected:

  _TyGraphConnectionLink  m_gclObj; // link from graph-object to this object.

  void  _init( ) _BIEN_NOTHROW
  {
    m_gclObj.m_pvConnection = (void*)this;
    m_gclObj.m_egclType = s_egclLinkPositionIterator;
  }

  void  _PushConnection( _TyGraphLinkBase ** _ppglbCur ) _BIEN_NOTHROW
  {
    t_TyGraphLinkSafe * _pgls;
    t_TyGraphNodeSafe * _pgns;
    t_TyGraphLinkSafe::PGrObjFromPPGLB( _ppglbCur, _pgls, _pgns );
    if ( _pgls )
    {
      _pgls->PushConnection( &m_gclObj ); // Crash here ? Perhaps you mixed non-safe and safe graphs.
    }
    else
    {
      _pgns->PushConnection( &m_gclObj ); // Crash here ? Perhaps you mixed non-safe and safe graphs.
    }
  }

  void  _Reference( _TyGraphLinkBase ** _ppglbCur ) _BIEN_NOTHROW
  {
    assert( !_TyIterBase::PPGLBCur() );
    _PushConnection( _ppglbCur );
    _TyIterBase::SetPPGLBCur( _ppglbCur );
  }

  void  _Dereference() _BIEN_NOTHROW
  {
    if ( _TyIterBase::PPGLBCur() )
    {
      m_gclObj.remove_link();
    }
  }

  // The graph node to which this iterator is connected is being deinitialized.
  // We don't need to unlink the connection link.
  void  _node_deinit() _BIEN_NOTHROW
  {
    assert( _TyIterBase::PPGLBCur() );
#ifndef NDEBUG
    t_TyGraphLinkSafe * _pgls;
    t_TyGraphNodeSafe * _pgns;
    t_TyGraphLinkSafe::PGrObjFromPPGLB( _TyIterBase::PPGLBCur(), _pgls, _pgns );
    assert( !_pgls && _pgns );  // We should currently be at the head of a relation list.
#endif //!NDEBUG
    
    _TyIterBase::Clear();
  }

  // The graph link to which this iterator is connected is being deinitialized.
  // We don't need to unlink the connection link.
  void  _link_deinit() _BIEN_NOTHROW
  {
    assert( _TyIterBase::PPGLBCur() );
#ifndef NDEBUG
    t_TyGraphLinkSafe * _pgls;
    t_TyGraphNodeSafe * _pgns;
    t_TyGraphLinkSafe::PGrObjFromPPGLB( _TyIterBase::PPGLBCur(), _pgls, _pgns );
    assert( _pgls );  // We should currently not be at the head of a relation list.
#endif //!NDEBUG
    
    _TyIterBase::Clear();
  }

public:

  typedef _graph_link_ident_iterator_base_notsafe<  _TyGraphNodeBase, _TyGraphLinkBase >  _TyLinkIdentIter;

protected:

  // Initialize with child/parent position - if this iterator is located at the head
  //  of either list then the node located at that head must be supplied ( else crash ).
  explicit _graph_link_pos_iterator_base_safe( _TyGraphLinkBase ** _ppglbCur ) _BIEN_NOTHROW
    : _TyIterBase( (_TyGraphLinkBase **)0 )
  {
    _init();
    if ( _ppglbCur )
    {
      _Reference( _ppglbCur );
    }
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template < class t_TyLinkPosIter >
  explicit _graph_link_pos_iterator_base_safe( const t_TyLinkPosIter & _r ) _BIEN_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
  explicit _graph_link_pos_iterator_base_safe( const _TyThis & _r ) _BIEN_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
    : _TyIterBase( (_TyGraphLinkBase **)0 )
  {
    _init();
    if ( _r.PPGLBCur() )
    {
      _Reference( _r.PPGLBCur() );
    }
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template < class t_TyLinkPosIter >
  explicit _graph_link_pos_iterator_base_safe( const t_TyLinkPosIter & _r, __false_type ) _BIEN_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
  explicit _graph_link_pos_iterator_base_safe( const _TyThis & _r, __false_type ) _BIEN_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
    : _TyIterBase( (_TyGraphLinkBase **)0 )
  {
    _init();
  }

public:

  void  Clear() _BIEN_NOTHROW
  {
    _Dereference();
    _TyIterBase::Clear();
  }

// Accessors:
  void    SetPPGLBCur( _TyGraphLinkBase ** _ppglbCur ) _BIEN_NOTHROW
  {
    if ( _TyIterBase::PPGLBCur() )
    { 
      m_gclObj.remove_link();
      if ( !!( _TyIterBase::m_ppglbCur = _ppglbCur ) )
      {
        _PushConnection( _TyIterBase::m_ppglbCur );
      }
    }
    else
    if ( _ppglbCur )
    {
      _Reference( _ppglbCur );
    }
  }

  // Set a non-NULL <_pglbCur> into an iterator:
  void    SetPPGLBCur_Populated( _TyGraphLinkBase ** _ppglbCur ) _BIEN_NOTHROW
  { 
    assert( _ppglbCur );
    if ( _TyIterBase::PPGLBCur() )
    {
      m_gclObj.remove_link();
      _TyIterBase::m_ppglbCur = _ppglbCur;
      _PushConnection( _TyIterBase::m_ppglbCur );
    }
    else
    {
      _Reference( _ppglbCur );
    }
  }

  // Node operations - these are the same as the node iterator:

  void          GoParent( _TyGNIndex _u ) _BIEN_NOTHROW
  {
    if ( *_TyIterBase::m_ppglbCur )
    {
      m_gclObj.remove_link();
      if ( _u )
      {
        _TyIterBase::GoParent( _u );
        t_TyGraphLinkSafe * _pgls = static_cast< t_TyGraphLinkSafe * >( _TyGraphLinkBase::PGLBGetThisFromPPGLBNextParent( _TyIterBase::m_ppglbCur ) );
        _pgls->PushConnection( &m_gclObj );
      }
      else
      {
        // Then we are referencing the head:
        (*_TyIterBase::m_ppglbCur)->m_pgnbNodeParent->PushConnection( &m_gclObj );
        _TyIterBase::m_ppglbCur = (*_TyIterBase::m_ppglbCur)->m_pgnbNodeParent->PPGLParentHead();
      }
    }
    else
    {
      assert( 0 );  // This is not supported - my feeling is that this is not necessary and it
                    //  certainly isn't well defined - which parent would we go to ?
    }
  }

  void          GoChild( _TyGNIndex _u ) _BIEN_NOTHROW
  {
    if ( *_TyIterBase::m_ppglbCur )
    {
      m_gclObj.remove_link();
      if ( _u )
      {
        _TyIterBase::GoChild( _u );
        t_TyGraphLinkSafe * _pgls = static_cast< t_TyGraphLinkSafe * >( _TyGraphLinkBase::PGLBGetThisFromPPGLBNextChild( _TyIterBase::m_ppglbCur ) );
        _pgls->PushConnection( &m_gclObj );
      }
      else
      {
        // Then we are referencing the head:
        static_cast< t_TyGraphNodeSafe * >( (*_TyIterBase::m_ppglbCur)->m_pgnbNodeChild )->PushConnection( &m_gclObj );
        _TyIterBase::m_ppglbCur = (*_TyIterBase::m_ppglbCur)->m_pgnbNodeChild->PPGLChildHead();
      }
    }
    else
    {
      assert( 0 );  // This is not supported - my feeling is that this is not necessary and it
                    //  certainly isn't well defined - which child would we go to ?
    }
  }
  void          GoRelation( bool _fChild, _TyGNIndex _u ) _BIEN_NOTHROW
  {
    if ( _fChild )  
      GoChild( _u );
    else
      GoParent( _u );
  }

  // Link operations:
  // The safe iterator gives enough info to compute the number of parents
  //  before the tail position appropriately:
  _TyGNIndex  UParentsBefore() const _BIEN_NOTHROW
  {
    t_TyGraphLinkSafe * _pgls;
    t_TyGraphNodeSafe * _pgns;
    t_TyGraphLinkSafe::PGrObjFromPPGLB( _TyIterBase::m_ppglbCur, _pgls, _pgns );
    if ( _pgls )
    {
      return _pgls->UParentsBefore() + 1;
    }
    else
    {
      return 0;
    }
  }
  void        NextParent() _BIEN_NOTHROW
  {
    m_gclObj.remove_link();
    _TyIterBase::NextParent();
    // This would have to be a link:
    t_TyGraphLinkSafe * _pgls = static_cast< t_TyGraphLinkSafe * >( 
      _TyGraphLinkBase::PGLBGetThisFromPPGLBNextChild( _TyIterBase::m_ppglbCur ) );
    _pgls->PushConnection( &m_gclObj );
  }
  void        PrevParent() _BIEN_NOTHROW
  {
    m_gclObj.remove_link();
    _TyIterBase::PrevParent();
    // This may be a graph node:
    _PushConnection( _TyIterBase::m_ppglbCur );
  }

  _TyGNIndex  UChildrenBefore() const _BIEN_NOTHROW
  {
    t_TyGraphLinkSafe * _pgls;
    t_TyGraphNodeSafe * _pgns;
    t_TyGraphLinkSafe::PGrObjFromPPGLB( _TyIterBase::m_ppglbCur, _pgls, _pgns );
    if ( _pgls )
    {
      return _pgls->UChildrenBefore() + 1;
    }
    else
    {
      return 0;
    }
  }
  void        NextChild() _BIEN_NOTHROW
  {
    m_gclObj.remove_link();
    _TyIterBase::NextChild();
    // This would have to be a link:
    t_TyGraphLinkSafe * _pgls = static_cast< t_TyGraphLinkSafe * >( 
      _TyGraphLinkBase::PGLBGetThisFromPPGLBNextChild( _TyIterBase::m_ppglbCur ) );
    _pgls->PushConnection( &m_gclObj );
  }
  void        PrevChild() _BIEN_NOTHROW
  {
    m_gclObj.remove_link();
    _TyIterBase::PrevChild();
    // This may be a graph node:
    _PushConnection( _TyIterBase::m_ppglbCur );
  }
  void        NextRelation( bool _fChild ) _BIEN_NOTHROW
  {
    if ( _fChild )  
      NextChild();
    else            
      NextParent();
  }
  void          PrevRelation( bool _fChild ) _BIEN_NOTHROW
  {
    if ( _fChild )  
      PrevChild();
    else            
      PrevParent();
  }

  // Operations between link iterators:
#ifdef _STLP_MEMBER_TEMPLATES
  template < class t_TyLinkIterator >
  void  ExchangeSiblingParents( t_TyLinkIterator const & _rSiblingParent ) const _BIEN_NOTHROW
  {
    assert( _TyIterBase::PGLBCur() != _rSiblingParent._TyIterBase::PGLBCur() );
    assert( _TyIterBase::PGLBCur()->m_pgnbNodeChild == _rSiblingParent._TyIterBase::PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeParents( _TyIterBase::PGLBCur(), _rSiblingParent._TyIterBase::PGLBCur() );
  }
  template < class t_TyLinkIterator >
  void  ExchangeSiblingChildren( t_TyLinkIterator const & _rSiblingChild ) const _BIEN_NOTHROW
  {
    assert( _rSiblingChild._TyIterBase::PGLBCur() != _TyIterBase::PGLBCur() );
    assert( _TyIterBase::PGLBCur()->m_pgnbNodeChild == _rSiblingChild._TyIterBase::PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeChildren( _TyIterBase::PGLBCur(), _rSiblingChild._TyIterBase::PGLBCur() );
  }
#else //_STLP_MEMBER_TEMPLATES
  void  ExchangeSiblingParents( _TyThis const & _rSiblingParent ) const _BIEN_NOTHROW
  {
    assert( &_rSiblingParent != this );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
  }
  void  ExchangeSiblingChildren( _TyThis const & _rSiblingChild ) const _BIEN_NOTHROW
  {
    assert( &_rSiblingChild != this );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
  }

  void  ExchangeSiblingParents( _TyLinkIdentIter const & _rlpiSiblingParent ) const _BIEN_NOTHROW
  {
    assert( &_rSiblingParent != this );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
  }
  void  ExchangeSiblingChildren( _TyLinkIdentIter const & _rlpiSiblingChild ) const _BIEN_NOTHROW
  {
    assert( &_rSiblingChild != this );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
  }
#endif //_STLP_MEMBER_TEMPLATES

#ifdef _STLP_MEMBER_TEMPLATES
  template < class t_TyLinkPosIterator >
  _TyThis & operator = ( t_TyLinkPosIterator const & _r ) _BIEN_NOTHROW
  {
    SetPPGLBCur( _r.PPGLBCur() );
    return *this;
  }

  template < class t_TyLinkPosIterator >
  void  SetToParent( t_TyLinkPosIterator const & _r ) _BIEN_NOTHROW
  {
    SetPPGLBCur_Populated( _r.PPGLBCur_Parent() );
  }
  template < class t_TyLinkPosIterator >
  void  SetToChild( t_TyLinkPosIterator const & _r ) _BIEN_NOTHROW
  {
    SetPPGLBCur_Populated( _r.PPGLBCur_Child() );
  }
#endif //_STLP_MEMBER_TEMPLATES

#ifdef _STLP_MEMBER_TEMPLATES
  // Catch any non-_TyThis types:
  template < class t_TySwap >
  void
  swap( t_TySwap & _r ) _BIEN_NOTHROW
  {
	__ASSERT_SAME_TYPE( t_TySwap, _TyThis );
    _TyGraphLinkBase ** ppglbCur = _TyIterBase::PPGLBCur();
    SetPPGLBCur( _r._TyIterBase::PPGLBCur() );
    _r.SetPPGLBCur( ppglbCur );
  }
#else //_STLP_MEMBER_TEMPLATES
  void
  swap( _TyThis & _r ) _BIEN_NOTHROW
  {
    _TyGraphLinkBase ** ppglbCur = PPGLBCur();
    SetPPGLBCur( _r.PPGLBCur() );
    _r.SetPPGLBCur( ppglbCur );
  }
#endif //_STLP_MEMBER_TEMPLATES
};

// This iterator maintains a link identity within either the child or the parent list.
// It also has an end() position that is a valid insert before position.
template < class t_TyGraphNodeSafe, class t_TyGraphLinkSafe >
class _graph_link_ident_iterator_base_safe
  : public _graph_link_ident_iterator_base_notsafe< typename t_TyGraphNodeSafe::_TyGraphNodeBase, 
                                                    typename t_TyGraphLinkSafe::_TyGraphLinkBase >
{
  template <  class t_TyPathNodeSafe, 
    class t_TyPathNodeBaseAllocator,
    class t_TyPathNodeSafeAllocator >
    friend class _graph_safe_base;

protected:
  // Get the base classes from the safe classes:
  typedef typename t_TyGraphNodeSafe::_TyGraphNodeBase  _TyGraphNodeBase;
  typedef typename t_TyGraphLinkSafe::_TyGraphLinkBase  _TyGraphLinkBase;

private:
  typedef _graph_link_ident_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >   _TyIterBase;
  typedef _graph_link_ident_iterator_base_safe< _TyGraphNodeBase, _TyGraphLinkBase >      _TyThis;

protected:

  _TyGraphConnectionLink  m_gclLink;  // link from graph-object to this object.

  void  _init() _BIEN_NOTHROW
  {
    m_gclLink.m_pvConnection = (void*)this;
    m_gclLink.m_egclType = s_egclGraphLinkIdentIterator;
  }

  void  _Reference( _TyGraphLinkBase * _pglbCur ) _BIEN_NOTHROW
  {
    assert( !PGLSCur() );
    _TyIterBase::m_pglbCur = _pglbCur;
    PGLSCur()->PushConnection( &m_gclLink );  // Crash here ? Perhaps you mixed non-safe and safe graphs.
  }

  void  _Dereference() _BIEN_NOTHROW
  {
    if ( PGLSCur() )
    {
      m_gclLink.remove_link();
    }
  }

  // The graph link to which this iterator is connected is being deinitialized.
  // We don't need to unlink the connection link.
  void  _link_deinit() _BIEN_NOTHROW
  {
    assert( _TyIterBase::PGLBCur() );
    _TyIterBase::Clear();
  }

public:

  typedef _graph_link_ident_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase > _TyIdentPosIter;

  explicit _graph_link_ident_iterator_base_safe( _TyGraphLinkBase * _pglbCur = 0 ) _BIEN_NOTHROW
    : _TyIterBase( static_cast< _TyGraphLinkBase * >( 0 ) )
  {
    _init();
    if ( _pglbCur )
    {
      _Reference( _pglbCur );
    }
  }

// Init with iterator that has an allocator:
#ifdef _STLP_MEMBER_TEMPLATES
  template < class t_TyLinkIdentIter >
  explicit _graph_link_ident_iterator_base_safe( const t_TyLinkIdentIter & _r ) _BIEN_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
  explicit _graph_link_ident_iterator_base_safe( const _TyThis & _r ) _BIEN_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
    : _TyIterBase( static_cast< _TyGraphLinkBase * >( 0 ) )
  { 
    _init();
    if ( _r.PGLBCur() )
    {
      _Reference( _r.PGLBCur() );
    }
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template < class t_TyLinkIdentIter >
  explicit _graph_link_ident_iterator_base_safe( const t_TyLinkIdentIter & _r, __false_type ) _BIEN_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
  explicit _graph_link_ident_iterator_base_safe( const _TyThis & _r, __false_type ) _BIEN_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
    : _TyIterBase( static_cast< _TyGraphLinkBase * >( 0 ) )
  { 
    _init();
  }

  void  Clear() _BIEN_NOTHROW
  {
    _Dereference();
    _TyIterBase::Clear();
  }

  t_TyGraphLinkSafe * PGLSCur() const _BIEN_NOTHROW
  {
    return static_cast< t_TyGraphLinkSafe * >( _TyIterBase::m_pglbCur );
  }

  void    SetPGLBCur( _TyGraphLinkBase * _pglbCur ) _BIEN_NOTHROW
  {
    if ( PGLSCur() )
    { 
      m_gclLink.remove_link();
      if ( !!( _TyIterBase::m_pglbCur = _pglbCur ) )
      {
        PGLSCur()->PushConnection( &m_gclLink );
      }
    }
    else
    if ( _pglbCur )
    {
      _Reference( _pglbCur );
    }
  }

  // Set a non-NULL <_pglbCur> into a populated iterator:
  void    SetPGLBCur_Populated( _TyGraphLinkBase * _pglbCur ) _BIEN_NOTHROW
  { 
    assert( PGLSCur() );
    assert( _pglbCur );
    m_gclLink.remove_link();
    _TyIterBase::m_pglbCur = _pglbCur;
    PGLSCur()->PushConnection( &m_gclLink );
  }

  // Node operations - these are the same as the node iterator:
  void          GoParent( _TyGNIndex _u ) _BIEN_NOTHROW
  {
    m_gclLink.remove_link();
    _TyIterBase::GoParent( _u );
    PGLSCur()->PushConnection( &m_gclLink );
  }
  void          GoChild( _TyGNIndex _u ) _BIEN_NOTHROW
  {
    m_gclLink.remove_link();
    _TyIterBase::GoChild( _u );
    PGLSCur()->PushConnection( &m_gclLink );
  }
  void          GoRelation( bool _fChild, _TyGNIndex _u ) _BIEN_NOTHROW
  {
    if ( _fChild )  
      GoChild( _u );
    else
      GoParent( _u );
  }

  // Link operations:
  void        NextParent() _BIEN_NOTHROW
  {
    m_gclLink.remove_link();
    _TyIterBase::NextParent( );
    PGLSCur()->PushConnection( &m_gclLink );
  }
  void        PrevParent() _BIEN_NOTHROW
  {
    m_gclLink.remove_link();
    _TyIterBase::PrevParent( );
    PGLSCur()->PushConnection( &m_gclLink );
  }

  void        NextChild() _BIEN_NOTHROW
  {
    m_gclLink.remove_link();
    _TyIterBase::NextChild( );
    PGLSCur()->PushConnection( &m_gclLink );
  }
  void        PrevChild() _BIEN_NOTHROW
  {
    m_gclLink.remove_link();
    _TyIterBase::PrevChild( );
    PGLSCur()->PushConnection( &m_gclLink );
  }
  void          NextRelation( bool _fChild ) _BIEN_NOTHROW
  {
    if ( _fChild )  
      NextChild();
    else            
      NextParent();
  }
  void          PrevRelation( bool _fChild ) _BIEN_NOTHROW
  {
    if ( _fChild )  
      PrevChild();
    else            
      PrevParent();
  }

  template < class t_TyLinkIter >
  bool operator == ( t_TyLinkIter const & _r ) const _BIEN_NOTHROW
  {
    return _TyIterBase::PGLBCur() == _r._TyIterBase::PGLBCur();
  }

  template < class t_TyLinkIterator >
  _TyThis & operator = ( t_TyLinkIterator const & _r )
  {
    SetPGLBCur( _r.PGLBCur() );
    return *this;
  }

#ifdef _STLP_MEMBER_TEMPLATES
  // Catch any non-_TyThis types:
  template < class t_TySwap >
  void
  swap( t_TySwap & _r ) _BIEN_NOTHROW
  {
	 __ASSERT_SAME_TYPE( t_TySwap, _TyThis );
    _TyGraphLinkBase * pglbCur = _TyIterBase::PGLBCur();
    SetPGLBCur( _r.PGLBCur() );
    _r.SetPGLBCur( pglbCur );
  }
#else //_STLP_MEMBER_TEMPLATES
  void
  swap( _TyThis & _r ) _BIEN_NOTHROW
  {
    _TyGraphLinkBase * pglbCur = PGLBCur();
    SetPGLBCur( _r.PGLBCur() );
    _r.SetPGLBCur( pglbCur );
  }
#endif //_STLP_MEMBER_TEMPLATES
};

// Path iterator - maintains path.
template < class t_TyPathNodeSafe,  class t_TyPathNodeSafeAllocator >
class _graph_path_iterator_base_safe
  : public _graph_path_iterator_base< typename t_TyPathNodeSafe::_TyPathNodeBase >,
    public _alloc_base< t_TyPathNodeSafe, t_TyPathNodeSafeAllocator >
{
  template <  class t__TyPathNodeSafe, 
              class t__TyPathNodeBaseAllocator,
              class t__TyPathNodeSafeAllocator >
  friend class _graph_safe_base;

public:

  // Get the base classes from the safe classes:
  typedef typename t_TyPathNodeSafe::_TyGraphNodeSafe   _TyGraphNodeSafe;
  typedef typename t_TyPathNodeSafe::_TyGraphLinkSafe   _TyGraphLinkSafe;

  typedef typename _TyGraphNodeSafe::_TyGraphNodeBase   _TyGraphNodeBase;
  typedef typename _TyGraphLinkSafe::_TyGraphLinkBase   _TyGraphLinkBase;
  typedef typename t_TyPathNodeSafe::_TyPathNodeBase    _TyPathNodeBase;

private:

  typedef _graph_path_iterator_base< _TyPathNodeBase >                _TyBase;
  typedef _graph_path_iterator_base_safe< t_TyPathNodeSafe, 
                                          t_TyPathNodeSafeAllocator > _TyThis;

  typedef _graph_node_iterator_base_notsafe<  _TyGraphNodeBase, 
                                              _TyGraphLinkBase >      _TyGraphNodeIter;
  typedef _graph_node_iterator_base_safe< _TyGraphNodeSafe, 
                                          _TyGraphLinkSafe >          _TyGraphNodeIterSafe;
  typedef _graph_link_pos_iterator_base_notsafe<  _TyGraphNodeBase, 
                                                  _TyGraphLinkBase >  _TyGraphLinkPosIter;
  typedef _graph_link_pos_iterator_base_safe< _TyGraphNodeBase, 
                                              _TyGraphLinkBase >      _TyGraphLinkPosIterSafe;
public:

  typedef _alloc_base< t_TyPathNodeSafe, t_TyPathNodeSafeAllocator >  _TyBaseAllocPathNodeSafe;
  typedef t_TyPathNodeSafeAllocator                                   _TyPathNodeSafeAllocatorAsPassed;
  typedef typename _TyBaseAllocPathNodeSafe::_TyAllocatorType         _TyPathNodeSafeAllocator;

protected:

  void  _UpdateConnLink( t_TyPathNodeSafe & _rpns ) _BIEN_NOTHROW
  {
    _rpns.m_gclLink.remove_link();
    _rpns.m_gclNode.remove_link();
    _rpns.PGNS()->PushConnection( &_rpns.m_gclNode );
    _rpns.PGLS()->PushConnection( &_rpns.m_gclLink );
  }

  // Note: This method does nothing with the count - that must be kept up-to-date
  //  by the caller.
  void  _AppendPath( const _TyPathNodeBase * _ppnbAppend )
  {
    assert( _TyBase::valid( 1 ) );

    if ( _ppnbAppend )
    {
      // Save the identity of the old tail - we will remove after successful appendage.
      t_TyPathNodeSafe * ppnsTailOld = static_cast< t_TyPathNodeSafe * >( *_TyBase::m_pppnbTail );

      // Save the position of the old tail - we will revert if we throw:
      _TyPathNodeBase **  _pppnbTailOld = _TyBase::m_pppnbTail;
      _BIEN_TRY
      {
        // Special case the last element - we won't be allocating a link.
        __SDP( t_TyPathNodeSafe, _TyPathNodeSafeAllocator, 
               _GetPNSAllocator(), _ppnsNew );
        for ( ;
              _ppnbAppend->m_ppnbNext;
              _ppnbAppend = _ppnbAppend->m_ppnbNext )
        {
          _ppnsNew.allocate();
#ifndef _BIEN_USE_EXCEPTIONS
          if ( !_ppnsNew )
          {
            _DestroyPath( _pppnbTailOld );
            return;
          }
#endif //!_BIEN_USE_EXCEPTIONS
          new ( _ppnsNew ) t_TyPathNodeSafe( *_ppnbAppend );

          _ppnsNew->push_back_node( _TyBase::m_pppnbTail );

          _ppnsNew->PGNS()->PushConnection( &( _ppnsNew->m_gclNode ) );
          _ppnsNew->PGLS()->PushConnection( &( _ppnsNew->m_gclLink ) );
          __SDP_TRANSFER(_ppnsNew);
        }
        _ppnsNew.allocate();
#ifndef _BIEN_USE_EXCEPTIONS
        if ( !_ppnsNew )
        {
          _DestroyPath( _pppnbTailOld );
          return;
        }
#endif //!_BIEN_USE_EXCEPTIONS
        new ( _ppnsNew ) t_TyPathNodeSafe( *_ppnbAppend );
        assert( !_ppnsNew->m_pglbLink );

        _ppnsNew->push_back_node( _TyBase::m_pppnbTail );
        _ppnsNew->PGNS()->PushConnection( &( _ppnsNew->m_gclNode ) );
        __SDP_TRANSFER(_ppnsNew);
      }
      _BIEN_UNWIND( _DestroyPath( _pppnbTailOld ) )

      // If we had an old tail then update by removing now - for throw-safety:
      if ( ppnsTailOld )
      {
        assert( !ppnsTailOld->m_pglbLink );
        assert( ppnsTailOld->m_pgnbNode == ppnsTailOld->m_ppnbNext->m_pgnbNode ); // Paths must connect.
        ppnsTailOld->remove_node_in_middle();
        ppnsTailOld->m_gclNode.remove_link();
        _TyBaseAllocPathNodeSafe::deallocate_type( ppnsTailOld );
      }
    }
    assert( _TyBase::valid( 0 ) );
  }

  void _AppendReversePath( const _TyPathNodeBase * _ppnbAppend )
  {
    assert( _TyBase::valid( 1 ) );
    if ( _ppnbAppend )
    {
      // Save the identity of the old tail - we will remove after successful appendage.
      _TyPathNodeBase * ppnsTailOld = static_cast< t_TyPathNodeSafe * >( *_TyBase::m_pppnbTail );

      // Save the old tail - we will revert if we throw:
      _TyPathNodeBase **  _pppnbTailOld = _TyBase::m_pppnbTail;
      _BIEN_TRY
      {
        __SDP(  t_TyPathNodeSafe, _TyPathNodeSafeAllocator, 
                _GetPNSAllocator(), _ppnsNew );
        _ppnsNew.allocate( );
        __SDP_CHECK_VOID( _ppnsNew );
        new ( _ppnsNew ) t_TyPathNodeSafe( _ppnbAppend->m_pgnbNode, 0 ); // This is the new tail.

        _ppnsNew->PGNS()->PushConnection( &( _ppnsNew->m_gclNode ) );
        __SDP_TRANSFER(_ppnsNew)->push_back_node( _TyBase::m_pppnbTail );

        // We will pass the graph link pointer as we copy:
        _TyGraphLinkBase *  _pglbNext = _ppnbAppend->m_pglbLink;

        for ( ;
              !!( _ppnbAppend = _ppnbAppend->m_ppnbNext );
            )
        {
          _ppnsNew.allocate( );
#ifndef _STLP_USE_EXCEPTIONS
          if ( !_ppnsNew )
          {
            _DestroyPath( _pppnbTailOld );
            return;
          }
#endif //!_BIEN_USE_EXCEPTIONS
          new ( _ppnsNew ) t_TyPathNodeSafe( _ppnbAppend->m_pgnbNode, _pglbNext );

          _ppnsNew->PGNS()->PushConnection( &( _ppnsNew->m_gclNode ) );
          _ppnsNew->PGLS()->PushConnection( &( _ppnsNew->m_gclLink ) );

          _pglbNext = _ppnbAppend->m_pglbLink;
          __SDP_TRANSFER(_ppnsNew)->insert_node_in_middle( *_pppnbTailOld );
        }
        assert( !_pglbNext );
      }
      _BIEN_UNWIND( _DestroyPath( _pppnbTailOld ) )

      // We succeeded - if we had an old tail then remove it now:
      if ( ppnsTailOld )
      {
        assert( !ppnsTailOld->m_pglbLink );
        assert( ppnsTailOld->m_pgnbNode == ppnsTailOld->m_ppnbNext->m_pgnbNode ); // Paths must connect.
        ppnsTailOld->remove_node_in_middle();
        ppnsTailOld->m_gclNode.remove_link();
        _TyBaseAllocPathNodeSafe::deallocate_type( ppnsTailOld );
      }
    }
    assert( _TyBase::valid( 0 ) );
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template < class _TyPathIter >
  void  _OverwritePath( _TyPathIter const & _r )
#else //_STLP_MEMBER_TEMPLATES
  void  _OverwritePath( _TyThis const & _r )
#endif //_STLP_MEMBER_TEMPLATES
  { 
    _OverwritePath( _r.m_ppnbHead, _r.m_pppnbTail, _r.m_uCount );
  }

  void  _OverwritePath( const _TyPathNodeBase * const & _ppnbHead, 
                        const _TyPathNodeBase * const * _pppnbTail, 
                        _TyGNIndex _uCount )
  { 
    assert( _TyBase::valid( 1 ) );

    const _TyPathNodeBase * const * _pppnbCopyUntil = _pppnbTail;

    // We will overwrite the current path with the one passed - 
    //  first make the list sizes the same:
    if ( _TyBase::m_uCount > _uCount )
    {
      // Then we need to destroy some nodes:
      _TyPathNodeBase ** _pppnb = 
        _TyPathNodeBase::PPGPNBGetNth(  &(_TyBase::m_ppnbHead), _TyBase::m_pppnbTail, 
                                        _TyBase::m_uCount, _uCount );
      _DestroyPath( _pppnb );
    }
    else
    if ( _TyBase::m_uCount < _uCount )
    {
      // Then we need to create some nodes - copy as we allocate:
      _TyPathNodeBase ** _pppnbTailOld = _TyBase::m_pppnbTail; // Save the old tail - in case we throw:
      _BIEN_TRY
      {
        // Insert the first one normally - this one updates the tail - the rest we know
        //  won't touch the tail so we can use the faster method:
        _pppnbCopyUntil = _TyPathNodeBase::PPPNBGetPrevPrevNext( _pppnbCopyUntil );
        __SDP(  t_TyPathNodeSafe, _TyPathNodeSafeAllocator, 
                _GetPNSAllocator(), _ppnsNew );
        _ppnsNew.allocate( );
        __SDP_CHECK_VOID( _ppnsNew );
        new ( _ppnsNew ) t_TyPathNodeSafe( **_pppnbCopyUntil );

        if ( _ppnsNew->m_pglbLink )
        {
          _ppnsNew->PGLS()->PushConnection( &( _ppnsNew->m_gclLink ) );
        }
        _ppnsNew->PGNS()->PushConnection( &( _ppnsNew->m_gclNode ) );
        
        __SDP_TRANSFER(_ppnsNew)->push_back_node( _TyBase::m_pppnbTail );

        // Now insert the next nodes into the list before the position of the old tail:
        for ( ++_TyBase::m_uCount; _TyBase::m_uCount++ < _uCount; )
        {
          _pppnbCopyUntil = _TyPathNodeBase::PPPNBGetPrevPrevNext( _pppnbCopyUntil );
          _ppnsNew.allocate( );
#ifndef _BIEN_USE_EXCEPTIONS
          if ( !_ppnsNew )
          {
            _DestroyPath( _pppnbTailOld );
            return;
          }
#endif //!_BIEN_USE_EXCEPTIONS
          new ( _ppnsNew ) t_TyPathNodeSafe( **_pppnbCopyUntil );

          if ( _ppnsNew->m_pglbLink )
          {
            _ppnsNew->PGLS()->PushConnection( &( _ppnsNew->m_gclLink ) );
          }
          _ppnsNew->PGNS()->PushConnection( &( _ppnsNew->m_gclNode ) );

          __SDP_TRANSFER(_ppnsNew)->insert_node_in_middle( *_pppnbTailOld );
        }
      }
      _BIEN_UNWIND( _DestroyPath( _pppnbTailOld ) ) // revert and rethrow.
    }

    // Now overwrite the already existing nodes in the path - as no allocation occurs we need not try:
    _TyPathNodeBase * _ppnbCopyTo = _TyBase::m_ppnbHead;
    for ( const _TyPathNodeBase * const * _pppnbCopyFrom = &( _ppnbHead );
          _pppnbCopyFrom != _pppnbCopyUntil;
          _pppnbCopyFrom = &( (*_pppnbCopyFrom)->m_ppnbNext ),
          _ppnbCopyTo = _ppnbCopyTo->m_ppnbNext )
    {
      // We will already have the exact same number of connection links as those
      //  from which we are copying - just re-position them:
      t_TyPathNodeSafe &  _rpnsCopyTo = static_cast< t_TyPathNodeSafe & >( *_ppnbCopyTo );
      _rpnsCopyTo.m_gclNode.remove_link();
      _rpnsCopyTo.m_pgnbNode = (*_pppnbCopyFrom)->m_pgnbNode;
      _rpnsCopyTo.PGNS()->PushConnection( &( _rpnsCopyTo.m_gclNode ) );

      if ( _rpnsCopyTo.m_pglbLink )
      {
        _rpnsCopyTo.m_gclLink.remove_link();
      }

      if ( (*_pppnbCopyFrom)->m_pglbLink )
      {
        _rpnsCopyTo.m_pglbLink = (*_pppnbCopyFrom)->m_pglbLink;
        _rpnsCopyTo.PGLS()->PushConnection( &( _rpnsCopyTo.m_gclLink ) );
      }
      else
      {
        _rpnsCopyTo.m_pglbLink = 0;
      }
    }

    _TyBase::m_uCount = _uCount;

    assert( _TyBase::valid( 1 ) );
  }

  // Specialize the case for overwriting the current path with a node iterator.
  void  _OverwritePath( _TyGraphNodeIter const & _r )
  {
    Clear();
    if ( _TyBase::PGNBCur() )
    {
      _AppendNode( _r._TyBase::PGNBCur(), 0 );
    }
  }

  // Specialize the case for overwriting the current path with a position iterator.
  void  _OverwritePath( _TyGraphLinkPosIter const & _r )
  {
    Clear();
    _AppendLinkPosIter( _r );
  }

  void  _PushNode( _TyGraphNodeBase * _pgnb, _TyGraphLinkBase * _pglb )
  {
    __SDP(  t_TyPathNodeSafe, _TyPathNodeSafeAllocator, 
            _GetPNSAllocator(), _ppnsAlloc );
    _ppnsAlloc.allocate( );
    __SDP_CHECK_VOID( _ppnsAlloc );
    new ( _ppnsAlloc ) t_TyPathNodeSafe(  static_cast< _TyGraphNodeSafe * >( _pgnb ), 
                                          static_cast< _TyGraphLinkSafe * >( _pglb ) );

    if ( _ppnsAlloc->m_pglbLink )
    {
      _ppnsAlloc->PGLS()->PushConnection( &( _ppnsAlloc->m_gclLink ) );
    }
    _ppnsAlloc->PGNS()->PushConnection( &( _ppnsAlloc->m_gclNode ) );
        
    __SDP_TRANSFER(_ppnsAlloc)->insert_node( _TyBase::m_ppnbHead, _TyBase::m_pppnbTail );
    _TyBase::m_uCount++;
  }

  void  _AppendNode( _TyGraphNodeBase * _pgnb, _TyGraphLinkBase * _pglb )
  {
    assert( !_TyBase::m_uCount );
    __SDP(  t_TyPathNodeSafe, _TyPathNodeSafeAllocator, 
            _GetPNSAllocator(), _ppnsAlloc );
    _ppnsAlloc.allocate( );
    __SDP_CHECK_VOID( _ppnsAlloc );
    new ( _ppnsAlloc ) t_TyPathNodeSafe(  static_cast< _TyGraphNodeSafe * >( _pgnb ), 
                                          static_cast< _TyGraphLinkSafe * >( _pglb ) );

    if ( _ppnsAlloc->m_pglbLink )
    {
      _ppnsAlloc->PGLS()->PushConnection( &( _ppnsAlloc->m_gclLink ) );
    }
    _ppnsAlloc->PGNS()->PushConnection( &( _ppnsAlloc->m_gclNode ) );

    __SDP_TRANSFER(_ppnsAlloc)->push_back_node( _TyBase::m_pppnbTail );
    _TyBase::m_uCount++;
  }

  void  _AppendLinkPosIter( _TyGraphLinkPosIter const & _r )
  {
    assert( !_TyBase::m_uCount );
    _TyPathNodeBase ** _pppnbOldTail = _TyBase::m_pppnbTail;
    _TyGNIndex _uOldCount;
    _BIEN_TRY
    {
      if ( _r.PGLBCur() )
      {
        if ( _r.PPGLBCur() == _r.PPGLBCur_Parent() )
        {
          // Create a path from the child to the parent:
          _AppendNode( _r.PGLBCur()->m_pgnbNodeParent, _r.PGLBCur() );
          _AppendNode( _r.PGLBCur()->m_pgnbNodeChild, 0 );
        }
        else
        {
          // Create a path from the parent to the child:
          _AppendNode( _r.PGLBCur()->m_pgnbNodeChild, _r.PGLBCur() );
          _AppendNode( _r.PGLBCur()->m_pgnbNodeParent, 0 );
        }
      }
    }
    _BIEN_UNWIND( ( _DestroyPath( _pppnbOldTail ), _TyBase::m_uCount = _uOldCount ) )
  }

  void  _DestroyPath( _TyPathNodeBase ** _pppnbStart ) _BIEN_NOTHROW
  {
    t_TyPathNodeSafe * ppnsOldTail = static_cast< t_TyPathNodeSafe * >( *_TyBase::m_pppnbTail );
    t_TyPathNodeSafe * ppnsCur = static_cast< t_TyPathNodeSafe * >( *_pppnbStart );
    _TyBase::m_pppnbTail = _pppnbStart;
    while ( ppnsCur != ppnsOldTail )
    {
      t_TyPathNodeSafe * ppns = ppnsCur;
      ppnsCur = static_cast< t_TyPathNodeSafe * >( ppnsCur->m_ppnbNext );

      ppns->m_gclNode.remove_link();
      if ( ppns->m_pglbLink )
      {
        ppns->m_gclLink.remove_link();
      }

      ppns->~t_TyPathNodeSafe();
      _TyBaseAllocPathNodeSafe::deallocate_type( ppns );
    }
    // If the new tail has a pointer to a link then rid it:
    if ( ppnsOldTail && ppnsOldTail->m_pglbLink )
    {
      ppnsOldTail->m_gclLink.remove_link();
      ppnsOldTail->m_pglbLink = 0; 
    }
  }

public:

  explicit _graph_path_iterator_base_safe( 
            _TyPathNodeSafeAllocatorAsPassed const & _allocPathNodeSafe 
              = _TyPathNodeSafeAllocatorAsPassed() ) _BIEN_NOTHROW
    : _TyBaseAllocPathNodeSafe( _allocPathNodeSafe )
  {
  }

  explicit _graph_path_iterator_base_safe( _graph_path_iterator_base_safe const & _r )
    : _TyBaseAllocPathNodeSafe( _r )
  {
    // This copies the path:
    _AppendPath( _r.m_ppnbHead );
    _TyBase::m_uCount = _r._TyBase::m_uCount; // throwsafe
  }

// Construction with a path and a different allocator:
#ifdef _STLP_MEMBER_TEMPLATES
  template < class _TyPathIter >
  explicit _graph_path_iterator_base_safe(  
    _TyPathIter const & _r,
    _TyPathNodeSafeAllocatorAsPassed const & _allocPathNode )
#else //_STLP_MEMBER_TEMPLATES
  explicit _graph_path_iterator_base_safe(  
    _TyThis const & _r,
    _TyPathNodeSafeAllocatorAsPassed const & _allocPathNode )
#endif //_STLP_MEMBER_TEMPLATES
    : _TyBaseAllocPathNodeSafe( _allocPathNode )
  {
    _AppendPath( _r.m_ppnbHead );
    _TyBase::m_uCount = _r._TyBase::m_uCount; // throwsafe
  }
    
  // Specialize construction with both safe and non-safe graph node iterators:
  explicit _graph_path_iterator_base_safe(  
    _TyGraphNodeIter const & _r,
    _TyPathNodeSafeAllocatorAsPassed const & _allocPathNode )
    : _TyBaseAllocPathNodeSafe( _allocPathNode )
  {
    if ( _r.PGNBCur() )
    {
      _AppendNode( _r.PGNBCur(), 0 );
    }
  }
  explicit _graph_path_iterator_base_safe(  
    _TyGraphNodeIterSafe const & _r,
    _TyPathNodeSafeAllocatorAsPassed const & _allocPathNode )
    : _TyBaseAllocPathNodeSafe( _allocPathNode )
  {
    if ( _r.PGNBCur() )
    {
      _AppendNode( _r.PGNBCur(), 0 );
    }
  }

  // Also specialize construction with a link position iterator - this produces a path
  //  that ends at the node located at current position of that iterator - i.e. if the
  //  link iterator is in the middle of the child list then the path iterator will start at 
  //  parent above that child and end at that child. Likewise for a parent list position.
  explicit _graph_path_iterator_base_safe(  
    _TyGraphLinkPosIter const & _r,
    _TyPathNodeSafeAllocatorAsPassed const & _allocPathNode )
    : _TyBaseAllocPathNodeSafe( _allocPathNode )
  {
    _AppendLinkPosIter( _r );
  }

  explicit _graph_path_iterator_base_safe(  
    _TyGraphLinkPosIterSafe const & _r,
    _TyPathNodeSafeAllocatorAsPassed const & _allocPathNode )
    : _TyBaseAllocPathNodeSafe( _allocPathNode )
  {
    _AppendLinkPosIter( static_cast< _TyGraphLinkPosIter const & >( _r ) );
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template < class _TyPathIter >
  explicit _graph_path_iterator_base_safe( _TyPathIter const & _r )
    : _TyBaseAllocPathNodeSafe( _r._GetPNSAllocatorAsPassed() )
  {
    // This copies the path:
    _AppendPath( _r.m_ppnbHead );
    _TyBase::m_uCount = _r._TyBase::m_uCount; // throwsafe
  }
#endif //_STLP_MEMBER_TEMPLATES

#ifdef _STLP_MEMBER_TEMPLATES
  template < class _TyPathIter >
  explicit _graph_path_iterator_base_safe( 
    _TyPathIter const & _r, __false_type ) _BIEN_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
  explicit _graph_path_iterator_base_safe( 
    _TyThis const & _r, __false_type ) _BIEN_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
    : _TyBaseAllocPathNodeSafe( _r._GetPNSAllocatorAsPassed() )
  {
    // This does not copy the path - just gets the allocator.
  }

  ~_graph_path_iterator_base_safe() _BIEN_NOTHROW
  {
    assert( _TyBase::valid( 1 ) );
    _TyPathNodeBase *& ppnbDestroy = _TyBase::m_ppnbHead;
    _DestroyPath( &ppnbDestroy );
  }

// Allocator access:
  _TyPathNodeSafeAllocator const & _GetPNSAllocator() const _BIEN_NOTHROW
  {
    return _TyBaseAllocPathNodeSafe::get_allocator_ref();
  }
  _TyPathNodeSafeAllocator & _GetPNSAllocator() _BIEN_NOTHROW
  {
    return _TyBaseAllocPathNodeSafe::get_allocator_ref();
  }
  _TyPathNodeSafeAllocatorAsPassed _GetPNSAllocatorAsPassed() const _BIEN_NOTHROW
  {
    return _TyBaseAllocPathNodeSafe::get_allocator();
  }

  void  Clear() _BIEN_NOTHROW
  {
    _DestroyPath( &_TyBase::m_ppnbHead );
  }

  _TyGraphNodeSafe * FirstParent()
  {
    _TyGraphLinkBase * _pglbParent = *_TyBase::m_ppnbHead->m_pgnbNode->PPGLBParentHead();
    if ( _pglbParent )
    {
      _PushNode( _pglbParent->m_pgnbNodeParent, _pglbParent );
      return static_cast< _TyGraphNodeSafe * >(  _pglbParent->m_pgnbNodeParent );
    }
    return 0;
  }
  _TyGraphNodeSafe * GoParent( _TyGNIndex _u )
  {
    // Add the <_u>th parent to the front of the path:
    _TyGraphLinkBase * _pglbParent = 
      _TyGraphLinkBase::PGLBGetNthParent( _TyBase::m_ppnbHead->m_pgnbNode->PPGLParentHead(), _u );
    if ( _pglbParent )
    {
      _PushNode( _pglbParent->m_pgnbNodeParent, _pglbParent );
      return static_cast< _TyGraphNodeSafe * >( _pglbParent->m_pgnbNodeParent );
    }
    return 0;
  }

  _TyGraphNodeSafe * FirstChild()
  {
    // Add the 1st parent to the front of the path:
    _TyGraphLinkBase * _pglbChild = *_TyBase::m_ppnbHead->m_pgnbNode->PPGLBChildHead();
    if ( _pglbChild )
    {
      _PushNode( _pglbChild->m_pgnbNodeChild, _pglbChild );
      return static_cast< _TyGraphNodeSafe * >( _pglbChild->m_pgnbNodeChild );
    }
    return 0;
  }
  _TyGraphNodeSafe * GoChild( _TyGNIndex _u )
  {
    // Add the <_u>th child to the front of the path:
    _TyGraphLinkBase * _pglbChild = 
      _TyGraphLinkBase::PGLBGetNthChild( _TyBase::m_ppnbHead->m_pgnbNode->PPGLChildHead(), _u );
    if ( _pglbChild )
    {
      _PushNode( _pglbChild->m_pgnbNodeChild, _pglbChild );
      return static_cast< _TyGraphNodeSafe * >( _pglbChild->m_pgnbNodeChild );
    }
    return 0;
  }

  _TyGraphNodeSafe * FirstRelation( bool _fChild ) _BIEN_NOTHROW
  {
    if ( _fChild )  
      return FirstChild();
    else            
      return FirstParent();
  }
  _TyGraphNodeSafe * GoRelation( bool _fChild, _TyGNIndex _u )
  {
    if ( _fChild )  
      return GoChild( _u );
    else
      return GoParent( _u );
  }

  // Link operations:
  _TyGraphNodeSafe * NextParent() _BIEN_NOTHROW
  {
    if ( _TyBase::NextParent() )
    {
      _UpdateConnLink( static_cast< t_TyPathNodeSafe * >( _TyBase::m_ppnbHead ) );
      return static_cast< _TyGraphNodeSafe * >( _TyBase::m_ppnbHead->m_pgnbNode );
    }
    return 0;      
  }
  _TyGraphNodeSafe * PrevParent() _BIEN_NOTHROW
  {
    if ( _TyBase::PrevParent() )
    {
      _UpdateConnLink( static_cast< t_TyPathNodeSafe * >( _TyBase::m_ppnbHead ) );
      return static_cast< _TyGraphNodeSafe * >( _TyBase::m_ppnbHead->m_pgnbNode );
    }
    return 0;
  }

  _TyGraphNodeSafe * NextChild() _BIEN_NOTHROW
  {
    if ( _TyBase::NextChild() )
    {
      _UpdateConnLink( static_cast< t_TyPathNodeSafe * >( _TyBase::m_ppnbHead ) );
      return static_cast< _TyGraphNodeSafe * >( _TyBase::m_ppnbHead->m_pgnbNode );
    }
    return 0;
  }
  _TyGraphNodeSafe * PrevChild() _BIEN_NOTHROW
  {
    if ( _TyBase::PrevParent() )
    {
      _UpdateConnLink( static_cast< t_TyPathNodeSafe * >( _TyBase::m_ppnbHead ) );
      return static_cast< _TyGraphNodeSafe * >( _TyBase::m_ppnbHead->m_pgnbNode );
    }
    return 0;
  }

  _TyGraphNodeSafe * NextRelation( bool _fChild ) _BIEN_NOTHROW
  {
    if ( _TyBase::NextRelation( _fChild ) )
    {
      _UpdateConnLink( static_cast< t_TyPathNodeSafe * >( _TyBase::m_ppnbHead ) );
      return static_cast< _TyGraphNodeSafe * >( _TyBase::m_ppnbHead->m_pgnbNode );
    }
    return 0;
  }
  _TyGraphNodeSafe * PrevRelation( bool _fChild ) _BIEN_NOTHROW
  {
    if ( _TyBase::PrevRelation( _fChild ) )
    {
      _UpdateConnLink( static_cast< t_TyPathNodeSafe * >( _TyBase::m_ppnbHead ) );
      return static_cast< _TyGraphNodeSafe * >( _TyBase::m_ppnbHead->m_pgnbNode );
    }
    return 0;
  }

  // Operations between link iterators:

  _TyThis & operator = ( _TyThis const & _r ) _BIEN_NOTHROW
  {
    // Overwrite the current path with that passed:
    _OverwritePath( _r );
    return *this;
  }

#ifdef _STLP_MEMBER_TEMPLATES
  template < class _TyIter >
  _TyThis & operator = ( _TyIter const & _r ) _BIEN_NOTHROW
  {
    // Overwrite the current path with that passed:
    _OverwritePath( _r );
    return *this;
  }
#endif //_STLP_MEMBER_TEMPLATES

#ifdef _STLP_MEMBER_TEMPLATES
  // Safe version - exact matching of templates - no derived->base conversion:
  template < class t_TySwap >
  void
  swap( t_TySwap & _r )
  {
    __ASSERT_SAME_TYPE( t_TySwap, _TyThis );
    _TyBase::swap( static_cast< _TyBase& >( _r ) );
  }
#else //_STLP_MEMBER_TEMPLATES
  void
  swap( _TyThis & _r ) _BIEN_NOTHROW
  {
    _TyBase::swap( static_cast< _TyBase& >( _r ) );
  }
#endif //_STLP_MEMBER_TEMPLATES

#if 0
  bool valid( bool _fCheckCount = true ) const
  {
    if ( !_TyBase::valid( _fCheckCount ) )
      return false;
    return true;
  }
#endif //0

};

__DGRAPH_END_NAMESPACE

#endif //__GR_SITR
