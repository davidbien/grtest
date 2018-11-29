#ifndef __GR_ITER_H
#define __GR_ITER_H

__DGRAPH_BEGIN_NAMESPACE

// _gr_iter.h

// The various graph iterators. Default implementation.

// non-safe node iterator base class:
template < class _TyGraphNodeBase, class _TyGraphLinkBase >
class _graph_node_iterator_base_notsafe
{
  typedef _graph_node_iterator_base_notsafe<  _TyGraphNodeBase, _TyGraphLinkBase >  _TyThis;

protected:

  _TyGraphNodeBase *      m_pgnbCur;

public:

  typedef _TyGraphNodeBase *  _TyInitArg;

  explicit _graph_node_iterator_base_notsafe( _TyGraphNodeBase * _pgnbCur = 0 ) _STLP_NOTHROW
    : m_pgnbCur( _pgnbCur )
  { }

  explicit _graph_node_iterator_base_notsafe( const _TyThis & _r ) _STLP_NOTHROW
    : m_pgnbCur( _r.PGNBCur() )
  { }

  template < class _TyNodeIter >
  explicit _graph_node_iterator_base_notsafe( const _TyNodeIter & _r ) _STLP_NOTHROW
    : m_pgnbCur( _r.PGNBCur() )
  { }

  template < class _TyNodeIter >
  _graph_node_iterator_base_notsafe( const _TyNodeIter & _r, __false_type ) _STLP_NOTHROW
    : m_pgnbCur( 0 )
  { }

  void  Clear() _STLP_NOTHROW
  {
    m_pgnbCur = 0;
  }

  _TyGraphNodeBase *      PGNBCur() const _STLP_NOTHROW
  {
    return m_pgnbCur;
  }
  _TyGraphNodeBase *      PBObjCur() const _STLP_NOTHROW
  {
    return m_pgnbCur;
  }
  void                    SetPGNBCur( _TyGraphNodeBase * _pgnbCur ) _STLP_NOTHROW
  {
    m_pgnbCur = _pgnbCur;
  }

  // Test if empty:
  bool operator ! () const _STLP_NOTHROW
  {
    return !m_pgnbCur;
  }

  _TyGNIndex    UParents() const _STLP_NOTHROW
  {
    return m_pgnbCur->UParents();
  }
  bool          FParents() const _STLP_NOTHROW
  {
    return m_pgnbCur->FParents();
  }
  _TyGraphLinkBase ** FirstParent() const _STLP_NOTHROW
  {
    return m_pgnbCur->PPGLBParentHead();
  }
  void          GoParent( _TyGNIndex _u ) _STLP_NOTHROW
  {
    m_pgnbCur = m_pgnbCur->PGNBGetNthParent( _u );
  }
  void  ExchangeParentsOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const _STLP_NOTHROW
  {
    m_pgnbCur->ExchangeParentsOrdered( _uMin, _uMax );
  }
  void  ExchangeParents( _TyGNIndex _u0, _TyGNIndex _u1 ) const _STLP_NOTHROW
  {
    if ( _u0 < _u1 )
    {
      m_pgnbCur->ExchangeParentsOrdered( _u0, _u1 );
    }
    else
    {
      m_pgnbCur->ExchangeParentsOrdered( _u1, _u0 );
    }
  }
  void  MoveParentUp( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    m_pgnbCur->MoveParentUp( _uRemove, _uInsert );
  }
  void  MoveParentDown( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    m_pgnbCur->MoveParentDown( _uRemove, _uInsert );
  }
  void  MoveParent( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    m_pgnbCur->MoveParent( _uRemove, _uInsert );
  }
  void  SetParentPos(long _lFromChild, long _lParentPos)
  {
    _TyGraphLinkBase ** ppglbChild = _TyGraphLinkBase::PPGLBGetNthChild( m_pgnbCur->PPGLBChildHead(), _lFromChild );
    if ( !*ppglbChild )
    {
      throw _graph_nav_except("_graph_node_iterator_base_notsafe::SetParentPos(): child index beyond end.");
    }
    _TyGraphLinkBase ** ppglbParent = _TyGraphLinkBase::PPGLBGetNthParent( 
          (*ppglbChild)->PGNBChild()->PPGLBParentHead(), _lParentPos );
    (*ppglbChild)->RemoveParent();
    (*ppglbChild)->InsertParent( ppglbParent );
  }

  void  AddParent( _TyGraphNodeBase * _pgnbAdd, _TyGraphLinkBase * _pglbAdd,
                  _TyGraphLinkBase ** _rpglInsertBeforeParent,
                  _TyGraphLinkBase ** _rpglInsertBeforeAddChild )
  {
    m_pgnbCur->AddParent( *_pgnbAdd, *_pglbAdd, *_rpglInsertBeforeParent, *_rpglInsertBeforeAddChild );
  }

  _TyGNIndex    UChildren() const _STLP_NOTHROW
  {
    return m_pgnbCur->UChildren();
  }
  bool          FChildren() const _STLP_NOTHROW
  {
    return m_pgnbCur->FChildren();
  }
  _TyGraphLinkBase ** FirstChild() const _STLP_NOTHROW
  {
    return m_pgnbCur->PPGLBChildHead();
  }
  void          GoChild( _TyGNIndex _u ) _STLP_NOTHROW
  {
    m_pgnbCur = m_pgnbCur->PGNBGetNthChild( _u );
  }
  void  ExchangeChildrenOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const _STLP_NOTHROW
  {
    m_pgnbCur->ExchangeChildrenOrdered( _uMin, _uMax );
  }
  void  ExchangeChildren( _TyGNIndex _u0, _TyGNIndex _u1 ) const _STLP_NOTHROW
  {
    if ( _u0 < _u1 )
    {
      m_pgnbCur->ExchangeChildrenOrdered( _u0, _u1 );
    }
    else
    {
      m_pgnbCur->ExchangeChildrenOrdered( _u1, _u0 );
    }
  }
  void  MoveChildUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_pgnbCur->MoveChildUp( _uRemove, _uInsert );
  }
  void  MoveChildDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_pgnbCur->MoveChildDown( _uRemove, _uInsert );
  }
  void  MoveChild( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    m_pgnbCur->MoveChild( _uRemove, _uInsert );
  }
  void  SetChildPos(long _lFromParent, long _lChildPos)
  {
    _TyGraphLinkBase ** ppglbParent = _TyGraphLinkBase::PPGLBGetNthParent( m_pgnbCur->PPGLBParentHead(), _lFromParent );
    if ( !*ppglbParent )
    {
      throw _graph_nav_except("_graph_node_iterator_base_notsafe::SetChildPos(): Parent index beyond end.");
    }
    _TyGraphLinkBase ** ppglbChild = _TyGraphLinkBase::PPGLBGetNthChild( 
          (*ppglbParent)->PGNBParent()->PPGLBChildHead(), _lChildPos );
    (*ppglbParent)->RemoveChild();
    (*ppglbParent)->InsertChild( ppglbChild );
  }

  void  AddChild( _TyGraphNodeBase * _pgnbAdd, _TyGraphLinkBase * _pglbAdd,
                  _TyGraphLinkBase ** _rpglInsertBeforeChild,
                  _TyGraphLinkBase ** _rpglInsertBeforeAddParent )
  {
    m_pgnbCur->AddChild( *_pgnbAdd, *_pglbAdd, *_rpglInsertBeforeChild, *_rpglInsertBeforeAddParent );
  }

  // relation operators:
  _TyGNIndex  URelations( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? UChildren() : UParents();
  }
  bool        FRelations( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? FChildren() : FParents();
  }
  _TyGraphLinkBase ** FirstRelation( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? FirstChild() : FirstParent();
  }
  void          GoRelation( bool _fChild, _TyGNIndex _u ) _STLP_NOTHROW
  {
    if ( _fChild )  
      GoChild( _u );
    else
      GoParent( _u );
  }

  // Operations:
  template < class t_TyIter >
  void      FirstParent( t_TyIter & _r ) const  // Can't be sure this won't throw - depends on t_TyIter.
  {
    _r.SetPPGLBCur( m_pgnbCur->PPGLBParentHead() );
  }
  template < class t_TyIter >
  void      FirstChild( t_TyIter & _r ) const
  {
    _r.SetPPGLBCur( m_pgnbCur->PPGLBChildHead() );
  }
  template < class t_TyIter >
  void      FirstRelation( bool _fChild, t_TyIter & _r ) const
  {
    _r.SetPPGLBCur( _fChild ? m_pgnbCur->PPGLBChildHead() : m_pgnbCur->PPGLBParentHead() );
  }

#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyNodeIter >
  _TyThis & operator = ( _TyNodeIter const & _r ) _STLP_NOTHROW
#else __STL_MEMBER_TEMPLATES
  _TyThis & operator = ( _TyThis const & _r ) _STLP_NOTHROW
#endif __STL_MEMBER_TEMPLATES
  {
    // An error here indicates that you are assigning incompatible iterators:
    m_pgnbCur = _r.PGNBCur();
    return *this;
  }

#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyNodeIter >
  bool operator == ( _TyNodeIter const & _r ) const _STLP_NOTHROW
#else __STL_MEMBER_TEMPLATES
  bool operator == ( _TyThis const & _r ) const _STLP_NOTHROW
#endif __STL_MEMBER_TEMPLATES
  {
    // An error here indicates that you are comparing incompatible iterators:
    return PGNBCur() == _r.PGNBCur();
  }

#ifdef __STL_MEMBER_TEMPLATES
  // Safe version - exact matching of templates - no derived->base conversion:
  template < class t_TySwap >
  void
  swap( t_TySwap & )
  {
    ___semantic_error_object  error;
  }
#endif __STL_MEMBER_TEMPLATES

  // This is unsafe without member templates.
  void
  swap( _TyThis & _r ) _STLP_NOTHROW
  {
    swap( m_pgnbCur, _r.m_pgnbCur );
  }

};

template < class _TyGraphNodeBase, class _TyGraphLinkBase >
class _graph_link_ident_iterator_base_notsafe;

// This iterator maintains a constant position within either the child or the parent list.
// It also has an end() position that is a valid insert after position.
template < class _TyGraphNodeBase, class _TyGraphLinkBase >
class _graph_link_pos_iterator_base_notsafe
{
  typedef _graph_link_pos_iterator_base_notsafe<  _TyGraphNodeBase, _TyGraphLinkBase >  _TyThis;

protected:

  _TyGraphLinkBase **     m_ppglbCur;

public:

  typedef _TyGraphLinkBase ** _TyInitArg;

  typedef _graph_link_ident_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase > _TyLinkIdentIter;

  explicit _graph_link_pos_iterator_base_notsafe( _TyGraphLinkBase ** _ppglbCur = 0 ) _STLP_NOTHROW
    : m_ppglbCur( _ppglbCur )
  { }

  template < class _TyLinkPosIter >
  explicit _graph_link_pos_iterator_base_notsafe( const _TyLinkPosIter & _r ) _STLP_NOTHROW
    : m_ppglbCur( _r.PPGLBCur() )
  { }

  template < class _TyLinkPosIter >
  _graph_link_pos_iterator_base_notsafe( const _TyLinkPosIter & _r, __false_type ) _STLP_NOTHROW
    : m_ppglbCur( 0 )
  { }

  void  Clear() _STLP_NOTHROW
  {
    m_ppglbCur = 0;
  }

  void    SetPPGLBCur( _TyGraphLinkBase ** _ppglbCur ) _STLP_NOTHROW
  {
    m_ppglbCur = _ppglbCur;
  }

  _TyGraphLinkBase **     PPGLBCur() const _STLP_NOTHROW
  {
    return m_ppglbCur;
  }
  _TyGraphLinkBase *      PGLBCur() const _STLP_NOTHROW
  {
    return *m_ppglbCur;
  }
  _TyGraphLinkBase *      PBObjCur() const _STLP_NOTHROW
  {
    return *m_ppglbCur;
  }

  _TyGraphLinkBase **     PPGLBCur_Parent() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->PPGLBGetThisParent();
  }
  _TyGraphLinkBase **     PPGLBCur_Child() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->PPGLBGetThisChild();
  }

  _TyGraphNodeBase *      PGNBParent() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->PGNBParent();
  }
  _TyGraphNodeBase *      PGNBChild() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->PGNBChild();
  }

  // Insert the given link before the current link position - this must be in the
  //  parent list ( place a bug could occur ): 
  void  InsertLinkBeforeParent( _TyGraphLinkBase * _pglb ) const _STLP_NOTHROW
  {
    _pglb->InsertParent( m_ppglbCur );
  }

  // Insert the given link before the current link position - this must be in the
  //  child list ( place a bug could occur ): 
  void  InsertLinkBeforeChild( _TyGraphLinkBase * _pglb ) const _STLP_NOTHROW
  {
    _pglb->InsertChild( m_ppglbCur );
  }

  void  InsertLinkBeforeRelation( bool _fChild, _TyGraphLinkBase * _pglb ) const _STLP_NOTHROW
  {
    if ( _fChild )
      InsertLinkBeforeChild( _pglb );
    else
      InsertLinkBeforeParent( _pglb );
  }

  // Test if empty:
  bool  operator ! () const _STLP_NOTHROW
  {
    return !m_ppglbCur;
  }
  bool  FIsLast() const _STLP_NOTHROW
  {
    return !*m_ppglbCur;
  }

  // Node operations - these are the same as the node iterator:
  _TyGNIndex    UParents() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->m_pgnbNodeParent->UParents();
  }
  bool          FParents() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->m_pgnbNodeParent->FParents();
  }
  void          FirstParent() _STLP_NOTHROW
  {
    // A crash indicates that likely the iterator was at the end.
    m_ppglbCur = (*m_ppglbCur)->m_pgnbNodeParent->PPGLBParentHead();
  }
  bool          FIsLastParent() const _STLP_NOTHROW
  {
    return FIsLast();
  }
  void          GoParent( _TyGNIndex _u ) _STLP_NOTHROW
  {
    // A crash indicates that likely the iterator was at the end.
    m_ppglbCur = _TyGraphLinkBase::PPGLBGetNthParent( (*m_ppglbCur)->m_pgnbNodeParent->PPGLBParentHead(), _u );
  }
  void  ExchangeParentsOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const _STLP_NOTHROW
  {
    (*m_ppglbCur)->m_pgnbNodeParent->ExchangeParentsOrdered( _uMin, _uMax );
  }
  void  ExchangeParents( _TyGNIndex _u0, _TyGNIndex _u1 ) const _STLP_NOTHROW
  {
    if ( _u0 < _u1 )
    {
      (*m_ppglbCur)->m_pgnbNodeParent->ExchangeParentsOrdered( _u0, _u1 );
    }
    else
    {
      (*m_ppglbCur)->m_pgnbNodeParent->ExchangeParentsOrdered( _u1, _u0 );
    }
  }
  void  MoveParentUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    (*m_ppglbCur)->m_pgnbNodeParent->MoveParentUp( _uRemove, _uInsert );
  }
  void  MoveParentDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    (*m_ppglbCur)->m_pgnbNodeParent->MoveParentDown( _uRemove, _uInsert );
  }
  void  MoveParent( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    (*m_ppglbCur)->m_pgnbNodeParent->MoveParent( _uRemove, _uInsert );
  }

  _TyGNIndex    UChildren() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->m_pgnbNodeChild->UChildren();
  }
  bool          FChildren() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->m_pgnbNodeChild->FChildren();
  }
  void          FirstChild() _STLP_NOTHROW
  {
    // A crash indicates that likely the iterator was at the end.
    m_ppglbCur = (*m_ppglbCur)->m_pgnbNodeChild->PPGLBChildHead();
  }
  bool          FIsLastChild() const _STLP_NOTHROW
  {
    return FIsLast();
  }
  void          GoChild( _TyGNIndex _u ) _STLP_NOTHROW
  {
    m_ppglbCur = _TyGraphLinkBase::PPGLBGetNthChild( (*m_ppglbCur)->m_pgnbNodeChild->PPGLBChildHead(), _u );
  }
  void  ExchangeChildrenOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const _STLP_NOTHROW
  {
    (*m_ppglbCur)->m_pgnbNodeChild->ExchangeChildrenOrdered( _uMin, _uMax );
  }
  void  ExchangeChildren( _TyGNIndex _u0, _TyGNIndex _u1 ) const _STLP_NOTHROW
  {
    if ( _u0 < _u1 )
    {
      (*m_ppglbCur)->m_pgnbNodeChild->ExchangeChildrenOrdered( _u0, _u1 );
    }
    else
    {
      (*m_ppglbCur)->m_pgnbNodeChild->ExchangeChildrenOrdered( _u1, _u0 );
    }
  }
  void  MoveChildUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    (*m_ppglbCur)->m_pgnbNodeChild->MoveChildUp( _uRemove, _uInsert );
  }
  void  MoveChildDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    (*m_ppglbCur)->m_pgnbNodeChild->MoveChildDown( _uRemove, _uInsert );
  }
  void  MoveChild( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    (*m_ppglbCur)->m_pgnbNodeChild->MoveChild( _uRemove, _uInsert );
  }

  // Link operations:
  _TyGNIndex  UParentsBefore() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->UParentsBefore();
  }
  _TyGNIndex  UParentsAfter() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->UParents();
  }
  void        NextParent() _STLP_NOTHROW
  {
    m_ppglbCur = (*m_ppglbCur)->PPGLBGetNextParent();
  }
  void        PrevParent() _STLP_NOTHROW
  {
    m_ppglbCur = (*m_ppglbCur)->PPGLBGetPrevParent();
  }

  _TyGNIndex  UChildrenBefore() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->UChildrenBefore();
  }
  _TyGNIndex  UChildrenAfter() const _STLP_NOTHROW
  {
    return (*m_ppglbCur)->UChildren();
  }
  void        NextChild() _STLP_NOTHROW
  {
    m_ppglbCur = (*m_ppglbCur)->PPGLBGetNextChild();
  }
  void        PrevChild() _STLP_NOTHROW
  {
    m_ppglbCur = (*m_ppglbCur)->PPGLBGetPrevChild();
  }

  // relation operators:
  _TyGNIndex  URelations( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? UChildren() : UParents();
  }
  bool        FRelations( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? FChildren() : FParents();
  }
  void          FirstRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )
      FirstChild();
    else
      FirstParent();
  }
  bool          FIsLastRelation( bool ) const _STLP_NOTHROW
  {
    return FIsLast();
  }
  void          GoRelation( bool _fChild, _TyGNIndex _u ) _STLP_NOTHROW
  {
    if ( _fChild )  
      GoChild( _u );
    else
      GoParent( _u );
  }
  void          NextRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )  
      NextChild();
    else            
      NextParent();
  }
  void          PrevRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )  
      PrevChild();
    else            
      PrevParent();
  }

  // Operations between link iterators:
  template < class _TyLinkIterator >
  void  ExchangeSiblingParents( _TyLinkIterator const & _rSiblingParent ) const _STLP_NOTHROW
  {
    assert( PGLBCur() != _rSiblingParent.PGLBCur() );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
  }
  template < class _TyLinkIterator >
  void  ExchangeSiblingChildren( _TyLinkIterator const & _rSiblingChild ) const _STLP_NOTHROW
  {
    assert( _rSiblingChild.PGLBCur() != PGLBCur() );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
  }

  // In the default case compare the positions - this allows comparison between
  //  path iterators and position iterators - equality returned only when the
  //  path iterator is in the same position ( i.e. including whether in parent list
  //  or child list ).
  template < class _TyLinkPosIterator >
  bool operator == ( _TyLinkPosIterator const & _r ) const _STLP_NOTHROW
  {
    return PPGLBCur() == _r.PPGLBCur();
  }

  // When comparing with the identity link iterator compare the link referred to by
  //  by this position.
  bool operator == ( _TyLinkIdentIter const & _r ) const _STLP_NOTHROW
  {
    return PGLBCur() == _r.PGLBCur();
  }

  template < class _TyLinkPosIterator >
  _TyThis & operator = ( _TyLinkPosIterator const & _r ) _STLP_NOTHROW
  {
    // an error here indicates you are using the wrong type of iterator to initialize:
    SetPPGLBCur( _r.PPGLBCur() );
  }

  template < class _TyLinkPosIterator >
  void  SetToParentList( _TyLinkPosIterator const & _r ) _STLP_NOTHROW
  {
    SetPPGLBCur( _r.PPGLBCur_Parent() );
  }
  template < class _TyLinkPosIterator >
  void  SetToChildList( _TyLinkPosIterator const & _r ) _STLP_NOTHROW
  {
    SetPPGLBCur( _r.PPGLBCur_Child() );
  }

#ifdef __STL_MEMBER_TEMPLATES
  // Safe version - exact matching of templates - no derived->base conversion:
  template < class t_TySwap >
  void
  swap( t_TySwap & )
  {
    ___semantic_error_object  error;
  }
#endif __STL_MEMBER_TEMPLATES

  void
  swap( _TyThis & _r ) _STLP_NOTHROW
  {
    swap( m_ppglbCur, _r.m_ppglbCur );
  }
};

// This iterator maintains a link identity within either the child or the parent list.
template < class _TyGraphNodeBase, class _TyGraphLinkBase >
class _graph_link_ident_iterator_base_notsafe
{
  typedef _graph_link_ident_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase > _TyThis;

protected:

  _TyGraphLinkBase *      m_pglbCur;

public:

  typedef _TyGraphLinkBase *  _TyInitArg;

  typedef _graph_link_pos_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase > _TyLinkPosIter;

  explicit _graph_link_ident_iterator_base_notsafe( _TyGraphLinkBase * _pglbCur = 0 ) _STLP_NOTHROW
    : m_pglbCur( _pglbCur )
  { }

  template < class t_TyLinkIter >
  explicit _graph_link_ident_iterator_base_notsafe( const t_TyLinkIter & _r ) _STLP_NOTHROW
    : m_pglbCur( _r.PGLBCur() )
  { }

  template < class t_TyLinkIter >
  _graph_link_ident_iterator_base_notsafe( const t_TyLinkIter & _r, __false_type ) _STLP_NOTHROW
    : m_pglbCur( 0 )
  { }

  void  Clear() _STLP_NOTHROW
  {
    m_pglbCur = 0;
  }

  void    SetPGLBCur( _TyGraphLinkBase * _pglbCur ) _STLP_NOTHROW
  {
    m_pglbCur = _pglbCur;
  }

  _TyGraphLinkBase *      PGLBCur() const _STLP_NOTHROW
  {
    return m_pglbCur;
  }
  _TyGraphLinkBase *      PBObjCur() const _STLP_NOTHROW
  {
    return m_pglbCur;
  }
  _TyGraphLinkBase **     PPGLBCur_Parent() const _STLP_NOTHROW
  {
    return m_pglbCur->PPGLBGetThisParent();
  }
  _TyGraphLinkBase **     PPGLBCur_Child() const _STLP_NOTHROW
  {
    return m_pglbCur->PPGLBGetThisChild();
  }

  // Test if empty:
  bool operator ! () const _STLP_NOTHROW
  {
    return !m_pglbCur;
  }
  bool  FIsLast() const _STLP_NOTHROW
  {
    return !m_pglbCur;
  }

  // Node operations - these are the same as the node iterator:
  _TyGNIndex    UParents() const _STLP_NOTHROW
  {
    return m_pglbCur->m_pgnbNodeParent->UParents();
  }
  bool          FParents() const _STLP_NOTHROW
  {
    return m_pglbCur->m_pgnbNodeParent->FParents();
  }
  void          FirstParent() _STLP_NOTHROW
  {
    m_pglbCur = *m_pglbCur->m_pgnbNodeParent->PPGLBParentHead();
  }
  bool          FIsLastParent() const _STLP_NOTHROW
  {
    return FIsLast();
  }
  void          GoParent( _TyGNIndex _u ) _STLP_NOTHROW
  {
    m_pglbCur = *_TyGraphLinkBase::PPGLBGetNthParent( m_pglbCur->m_pgnbNodeParent->PPGLBParentHead(), _u );
  }
  void  ExchangeParentsOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const _STLP_NOTHROW
  {
    m_pglbCur->m_pgnbNodeParent->ExchangeParentsOrdered( _uMin, _uMax );
  }
  void  ExchangeParents( _TyGNIndex _u0, _TyGNIndex _u1 ) const _STLP_NOTHROW
  {
    if ( _u0 < _u1 )
    {
      m_pglbCur->m_pgnbNodeParent->ExchangeParentsOrdered( _u0, _u1 );
    }
    else
    {
      m_pglbCur->m_pgnbNodeParent->ExchangeParentsOrdered( _u1, _u0 );
    }
  }
  void  MoveParentUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_pglbCur->m_pgnbNodeParent->MoveParentUp( _uRemove, _uInsert );
  }
  void  MoveParentDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_pglbCur->m_pgnbNodeParent->MoveParentDown( _uRemove, _uInsert );
  }
  void  MoveParent( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    m_pglbCur->m_pgnbNodeParent->MoveParent( _uRemove, _uInsert );
  }

  _TyGNIndex    UChildren() const _STLP_NOTHROW
  {
    return m_pglbCur->m_pgnbNodeChild->UChildren();
  }
  bool          FChildren() const _STLP_NOTHROW
  {
    return m_pglbCur->m_pgnbNodeChild->FChildren();
  }
  void          FirstChild() _STLP_NOTHROW
  {
    m_pglbCur = *m_pglbCur->m_pgnbNodeChild->PPGLBChildHead();
  }
  bool          FIsLastChild() const _STLP_NOTHROW
  {
    return FIsLast();
  }
  void          GoChild( _TyGNIndex _u ) _STLP_NOTHROW
  {
    m_pglbCur = *_TyGraphLinkBase::PPGLBGetNthChild( m_pglbCur->m_pgnbNodeChild->PPGLBChildHead(), _u );
  }
  void  ExchangeChildrenOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const _STLP_NOTHROW
  {
    m_pglbCur->m_pgnbNodeChild->ExchangeChildrenOrdered( _uMin, _uMax );
  }
  void  ExchangeChildren( _TyGNIndex _u0, _TyGNIndex _u1 ) const _STLP_NOTHROW
  {
    if ( _u0 < _u1 )
    {
      m_pglbCur->m_pgnbNodeChild->ExchangeChildrenOrdered( _u0, _u1 );
    }
    else
    {
      m_pglbCur->m_pgnbNodeChild->ExchangeChildrenOrdered( _u1, _u0 );
    }
  }
  void  MoveChildUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_pglbCur->m_pgnbNodeChild->MoveChildUp( _uRemove, _uInsert );
  }
  void  MoveChildDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_pglbCur->m_pgnbNodeChild->MoveChildDown( _uRemove, _uInsert );
  }
  void  MoveChild( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    m_pglbCur->m_pgnbNodeChild->MoveChild( _uRemove, _uInsert );
  }

  // Link operations:
  _TyGNIndex  UParentsBefore() const _STLP_NOTHROW
  {
    return m_pglbCur->UParentsBefore();
  }
  _TyGNIndex  UParentsAfter() const _STLP_NOTHROW
  {
    return m_pglbCur->UParents();
  }
  void        NextParent() _STLP_NOTHROW
  {
    m_pglbCur = *( m_pglbCur->PPGLBGetNextParent() );
  }
  void        PrevParent() _STLP_NOTHROW
  {
    m_pglbCur = *( m_pglbCur->PPGLBGetPrevParent() );
  }

  _TyGNIndex  UChildrenBefore() const _STLP_NOTHROW
  {
    return m_pglbCur->UChildrenBefore();
  }
  _TyGNIndex  UChildrenAfter() const _STLP_NOTHROW
  {
    return m_pglbCur->UChildren();
  }
  void        NextChild() _STLP_NOTHROW
  {
    m_pglbCur = *( m_pglbCur->PPGLBGetNextChild() );
  }
  void        PrevChild() _STLP_NOTHROW
  {
    m_pglbCur = *( m_pglbCur->PPGLBGetPrevChild() );
  }

  // relation operators:
  _TyGNIndex  URelations( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? UChildren() : UParents();
  }
  bool        FRelations( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? FChildren() : FParents();
  }
  void          FirstRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )  
      FirstChild();
    else
      FirstParent();
  }
  void          GoRelation( bool _fChild, _TyGNIndex _u ) _STLP_NOTHROW
  {
    if ( _fChild )  
      GoChild( _u );
    else
      GoParent( _u );
  }
  void          NextRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )  
      NextChild();
    else            
      NextParent();
  }
  void          PrevRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )  
      PrevChild();
    else            
      PrevParent();
  }

  // Operations between link iterators:
#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyLinkIterator >
  void  ExchangeSiblingParents( _TyLinkIterator const & _rSiblingParent ) const _STLP_NOTHROW
  {
    assert( PGLBCur() != _rSiblingParent.PGLBCur() );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
  }
  template < class _TyLinkIterator >
  void  ExchangeSiblingChildren( _TyLinkIterator const & _rSiblingChild ) const _STLP_NOTHROW
  {
    assert( _rSiblingChild.PGLBCur() != PGLBCur() );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
  }
#else __STL_MEMBER_TEMPLATES
  void  ExchangeSiblingParents( _TyThis const & _rSiblingParent ) const _STLP_NOTHROW
  {
    assert( &_rSiblingParent != this );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
  }
  void  ExchangeSiblingChildren( _TyThis const & _rSiblingChild ) const _STLP_NOTHROW
  {
    assert( &_rSiblingChild != this );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
  }

  void  ExchangeSiblingParents( _TyLinkPosIter const & _rlpiSiblingParent ) const _STLP_NOTHROW
  {
    assert( &_rSiblingParent != this );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
  }
  void  ExchangeSiblingChildren( _TyLinkPosIter const & _rlpiSiblingChild ) const _STLP_NOTHROW
  {
    assert( &_rSiblingChild != this );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
  }
#endif __STL_MEMBER_TEMPLATES

#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyLinkIter >
  bool operator == ( _TyLinkIter const & _r ) const _STLP_NOTHROW
  {
    return PGLBCur() == _r.PGLBCur();
  }
#endif __STL_MEMBER_TEMPLATES

#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyLinkIterator >
  _TyThis & operator = ( _TyLinkIterator const & _r ) _STLP_NOTHROW
  {
    SetPGLBCur( _r.PGLBCur() );
    return *this;
  }
#endif __STL_MEMBER_TEMPLATES

#ifdef __STL_MEMBER_TEMPLATES
  // Safe version - exact matching of templates - no derived->base conversion:
  template < class t_TySwap >
  void
  swap( t_TySwap & )
  {
    ___semantic_error_object  error;
  }

  void
  swap( _TyThis & _r ) _STLP_NOTHROW
  {
    swap( m_pglbCur, _r.m_pglbCur );
  }
#else __STL_MEMBER_TEMPLATES
  // Unsafe version - derived->base conversion allows this to screw-up ( when safe and non-safe mixed ):
  void
  swap( _TyThis & _r ) _STLP_NOTHROW
  {
    swap( m_pglbCur, _r.m_pglbCur );
  }
#endif __STL_MEMBER_TEMPLATES
};

// Base class for both non-safe and safe path iterators - just maintains
//  concurrency and implements a couple of common methods:

template < class t_TyPathNodeBase >
class _graph_path_iterator_base
{
  typedef _graph_path_iterator_base< t_TyPathNodeBase > _TyThis;

  _graph_path_iterator_base( _graph_path_iterator_base const & ); // Prevent copy construction.
  _TyThis & operator = ( _graph_path_iterator_base const & ); // Prevent assignment.
public:

  typedef typename t_TyPathNodeBase::_TyGraphNodeBase _TyGraphNodeBase;
  typedef typename t_TyPathNodeBase::_TyGraphLinkBase _TyGraphLinkBase;
  
  t_TyPathNodeBase *  m_ppnbHead; // Head of graph path node list.
  t_TyPathNodeBase ** m_pppnbTail;// tail of graph path node list - allows fast appending - as well as fast copying.
                                  //  Also allows fast access to the root of the path.
  _TyGNIndex          m_uCount;   // Count of nodes in the path.

  _graph_path_iterator_base()
    : m_ppnbHead( 0 ),
      m_pppnbTail( &m_ppnbHead ),
      m_uCount( 0 )
  {
  }

  // Before using current object accessors, check to see if a path
  //  exists.
  bool has_path()
  {
    return m_ppnbHead;
  }

  // The current object accessors don't check to see if we have a path.
  _TyGraphNodeBase * PGNBCur() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pgnbNode;
  }
  _TyGraphNodeBase * PBObjCur() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pgnbNode;
  }

  // This method let's us transfer our state to a link identity iterator:
  const _TyGraphLinkBase * PGLBCur() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pglbLink;
  }

  // This method let's us transfer our state to a link position iterator:
  const _TyGraphLinkBase ** PPGLBCur_Parent() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pglbLink->PPGLBGetThisParent();
  }
  const _TyGraphLinkBase ** PPGLBCur_Child() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pglbLink->PPGLBGetThisChild();
  }

  // Return according to direction of tip of iterator:
  const _TyGraphLinkBase ** PPGLBCur() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pglbLink->m_pgnbNodeChild == m_ppnbHead->m_pgnbNode ? 
      PPGLBCur_Child() : PPGLBCur_Parent();
  }

  // Test if empty:
  bool operator ! () const _STLP_NOTHROW
  {
    return !has_path();
  }

  _TyGNIndex UParents() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pgnbNode->UParents();
  }
  bool FParents() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pgnbNode->FParents();
  }
  void ExchangeParentsOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const _STLP_NOTHROW
  {
    m_ppnbHead->m_pgnbNode->ExchangeParentsOrdered( _uMin, _uMax );
  }
  void ExchangeParents( _TyGNIndex _u0, _TyGNIndex _u1 ) const _STLP_NOTHROW
  {
    if ( _u0 < _u1 )
    {
      m_ppnbHead->m_pgnbNode->ExchangeParentsOrdered( _u0, _u1 );
    }
    else
    {
      m_ppnbHead->m_pgnbNode->ExchangeParentsOrdered( _u1, _u0 );
    }
  }
  void MoveParentUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_ppnbHead->m_pgnbNode->MoveParentUp( _uRemove, _uInsert );
  }
  void MoveParentDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_ppnbHead->m_pgnbNode->MoveParentDown( _uRemove, _uInsert );
  }
  void MoveParent( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    m_ppnbHead->m_pgnbNode->MoveParent( _uRemove, _uInsert );
  }

  _TyGNIndex UChildren() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pgnbNode->UChildren();
  }
  bool FChildren() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pgnbNode->FChildren();
  }
  void ExchangeChildrenOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const _STLP_NOTHROW
  {
    m_ppnbHead->m_pgnbNode->ExchangeChildrenOrdered( _uMin, _uMax );
  }
  void ExchangeChildren( _TyGNIndex _u0, _TyGNIndex _u1 ) const _STLP_NOTHROW
  {
    if ( _u0 < _u1 )
    {
      m_ppnbHead->m_pgnbNode->ExchangeChildrenOrdered( _u0, _u1 );
    }
    else
    {
      m_ppnbHead->m_pgnbNode->ExchangeChildrenOrdered( _u1, _u0 );
    }
  }
  void MoveChildUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_ppnbHead->m_pgnbNode->MoveChildUp( _uRemove, _uInsert );
  }
  void MoveChildDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) const _STLP_NOTHROW
  {
    m_ppnbHead->m_pgnbNode->MoveChildDown( _uRemove, _uInsert );
  }
  void MoveChild( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) const _STLP_NOTHROW
  {
    m_ppnbHead->m_pgnbNode->MoveChild( _uRemove, _uInsert );
  }

  // Link operations:
  _TyGNIndex UParentsBefore() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pglbLink->UParentsBefore();
  }
  _TyGNIndex UParentsAfter() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pglbLink->UParents();
  }
  _TyGraphNodeBase * NextParent() _STLP_NOTHROW
  {
    // Advance the iterator to the next parent - if one exists:
    _TyGraphLinkBase * pglb = m_ppnbHead->m_pglbLink->PGLBGetNextParent();
    if ( pglb )
    {
      m_ppnbHead->m_pglbLink = pglb;
      m_ppnbHead->m_pgnbNode = pglb->m_pgnbNodeParent;
      return pglb->m_pgnbNodeParent;
    }
    return 0;
  }
  _TyGraphNodeBase * PrevParent() _STLP_NOTHROW
  {
    // Advance the iterator to the previous parent:
    _TyGraphLinkBase * pglb = m_ppnbHead->m_pglbLink->PGLBGetPrevParent();
    if ( pglb )
    {
      m_ppnbHead->m_pglbLink = pglb;
      m_ppnbHead->m_pgnbNode = pglb->m_pgnbNodeParent;
      return pglb->m_pgnbNodeParent;
    }
    return 0;
  }

  _TyGNIndex UChildrenBefore() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pglbLink->UChildrenBefore();
  }
  _TyGNIndex UChildrenAfter() const _STLP_NOTHROW
  {
    return m_ppnbHead->m_pglbLink->UChildren();
  }
  _TyGraphNodeBase * NextChild() _STLP_NOTHROW
  {
    // Advance the iterator to the next child:
    _TyGraphLinkBase * pglb = m_ppnbHead->m_pglbLink->PGLBGetNextChild();
    if ( pglb )
    {
      m_ppnbHead->m_pglbLink = pglb;
      m_ppnbHead->m_pgnbNode = pglb->m_pgnbNodeChild;
      return pglb->m_pgnbNodeChild;
    }
    return 0;
  }
  _TyGraphNodeBase * PrevChild() _STLP_NOTHROW
  {
    // Advance the iterator to the previous child:
    _TyGraphLinkBase * pglb = m_ppnbHead->m_pglbLink->PGLBGetPrevChild();
    if ( pglb )
    {
      m_ppnbHead->m_pglbLink = pglb;
      m_ppnbHead->m_pgnbNode = pglb->m_pgnbNodeChild;
      return pglb->m_pgnbNodeChild;
    }
    return 0;
  }

  // relation operators:
  _TyGNIndex URelations( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? UChildren() : UParents();
  }
  bool FRelations( bool _fChildren ) const _STLP_NOTHROW
  {
    return _fChildren ? FChildren() : FParents();
  }
  bool FIsLastRelation( bool ) const _STLP_NOTHROW
  {
    return FIsLast();
  }
  _TyGraphNodeBase * NextRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )  
      return NextChild();
    else            
      return NextParent();
  }
  _TyGraphNodeBase * PrevRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )  
      return PrevChild();
    else            
      return PrevParent();
  }

  // Operations between link iterators:
#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyLinkIterator >
  void  ExchangeSiblingParents( _TyLinkIterator const & _rSiblingParent ) const _STLP_NOTHROW
#else __STL_MEMBER_TEMPLATES
  void  ExchangeSiblingParents( _TyThis const & _rSiblingParent ) const _STLP_NOTHROW
#endif __STL_MEMBER_TEMPLATES
  {
    assert( PGLBCur() != _rSiblingParent.PGLBCur() );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
  }
#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyLinkIterator >
  void  ExchangeSiblingChildren( _TyLinkIterator const & _rSiblingChild ) const _STLP_NOTHROW
#else __STL_MEMBER_TEMPLATES
  void  ExchangeSiblingChildren( _TyThis const & _rSiblingChild ) const _STLP_NOTHROW
#endif __STL_MEMBER_TEMPLATES
  {
    assert( _rSiblingChild.PGLBCur() != PGLBCur() );
    assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
    _TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
  }

#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyLinkIter >
  bool operator == ( _TyLinkIter const & _r ) const _STLP_NOTHROW
#else __STL_MEMBER_TEMPLATES
  bool operator == ( _TyThis const & _r ) const _STLP_NOTHROW
#endif __STL_MEMBER_TEMPLATES
  {
    return PGLBCur() == _r.PGLBCur();
  }

  // We want derived->base conversion for this method:
  void swap( _TyThis & _r )
  {
    swap( m_ppnbHead, _r.m_ppnbHead );
    swap( m_pppnbTail, _r.m_pppnbTail );
    swap( m_uCount, _r.m_uCount );
  }
  
  bool valid( bool _fCheckCount = true ) const
  {
    t_TyPathNodeBase * const * pppnbPrevNext = &m_ppnbHead;
    _TyGNIndex _uCount = 0;
    t_TyPathNodeBase * ppnbPrev = 0;
    for ( t_TyPathNodeBase * ppnb = m_ppnbHead;
          ppnb; ++_uCount, ( ppnb = ppnb->m_ppnbNext ) )
    {
      ppnbPrev = ppnb;
      if ( ppnb->m_pppnbPrevNext != pppnbPrevNext )
      {
        return false;
      }
      pppnbPrevNext = &ppnb->m_ppnbNext;
    }
    if ( pppnbPrevNext != m_pppnbTail )
    {
      return false;
    }
    // Shouldn't have a link on the tail:
    if ( ppnbPrev && ppnbPrev->m_pglbLink )
    {
      return false;
    }
    return !_fCheckCount || ( _uCount == m_uCount );
  }
};

// Path iterator - maintains path.
template < class t_TyPathNodeBase,  class t_TyPathNodeBaseAllocator >
class _graph_path_iterator_base_notsafe
  : public _graph_path_iterator_base< t_TyPathNodeBase >,
    public _alloc_base< t_TyPathNodeBase, t_TyPathNodeBaseAllocator >
{
  typedef _graph_path_iterator_base< t_TyPathNodeBase >                 _TyBase;
  typedef _alloc_base< t_TyPathNodeBase, t_TyPathNodeBaseAllocator >    _TyBaseAllocPathNode;
  typedef _graph_path_iterator_base_notsafe< t_TyPathNodeBase, t_TyPathNodeBaseAllocator > _TyThis;

  typedef _graph_node_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >     _TyGraphNodeIter;
  typedef _graph_link_pos_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase > _TyGraphLinkPosIter;

public:

  typedef t_TyPathNodeBaseAllocator                       _TyPathNodeBaseAllocatorAsPassed;
  typedef typename _TyBaseAllocPathNode::_TyAllocatorType _TyPathNodeBaseAllocator;

protected:

  // Note: This method does nothing with the count - that must be kept up-to-date
  //  by the caller.
  void  _AppendPath( const t_TyPathNodeBase * _ppnbAppend )
  {
    assert( valid(1) );

    if ( _ppnbAppend )
    {
      // Save the identity of the old tail - we will remove after successful appendage.
      t_TyPathNodeBase *  _ppnbTailOld = *m_pppnbTail;

      // Save the position of the old tail - we will revert if we throw:
      t_TyPathNodeBase ** _pppnbTailOld = m_pppnbTail;
      __STL_TRY
      {
        __SDP(  t_TyPathNodeBase, _TyPathNodeBaseAllocator, 
                _GetPNBAllocator(), ppnbNew );
        for ( ;
              _ppnbAppend;
              _ppnbAppend = _ppnbAppend->m_ppnbNext )
        {
          ppnbNew.allocate();
#ifndef __STL_USE_EXCEPTIONS
          if ( !ppnbNew )
          {
            _DestroyPath( _pppnbTailOld );
            return;
          }
#endif !__STL_USE_EXCEPTIONS
          new ( ppnbNew ) t_TyPathNodeBase( *_ppnbAppend );
          __SDP_TRANSFER(ppnbNew)->push_back_node( m_pppnbTail );
        }
      }
      __STL_UNWIND( _DestroyPath( _pppnbTailOld ) )

      // If we had an old tail then update by removing now - for throw-safety:
      if ( _ppnbTailOld )
      {
        assert( !_ppnbTailOld->m_pglbLink );
        assert( _ppnbTailOld->m_pgnbNode == _ppnbTailOld->m_ppnbNext->m_pgnbNode ); // Paths must connect.
        _ppnbTailOld->remove_node_in_middle();
        _TyBaseAllocPathNode::deallocate_type( _ppnbTailOld );
      }
    }
    assert( valid(0) );
  }

  void  _AppendReversePath( const t_TyPathNodeBase * _ppnbAppend )
  {
    assert( valid(1) );
    if ( _ppnbAppend )
    {
      // Save the identity of the old tail - we will remove after successful appendage.
      t_TyPathNodeBase *  _ppnbTailOld = *m_pppnbTail;

      // Save the old tail - we will revert if we throw:
      t_TyPathNodeBase ** _pppnbTailOld = m_pppnbTail;
      __STL_TRY
      {
        __SDP(  t_TyPathNodeBase, _TyPathNodeBaseAllocator, 
                _GetPNBAllocator(),  ppnbNew );
        ppnbNew.allocate();
        __SDP_CHECK_VOID( ppnbNew );
        new ( ppnbNew ) t_TyPathNodeBase( _ppnbAppend->m_pgnbNode, 0 );  // This is the new tail.
        // We will pass the graph link pointer as we copy:
        _TyGraphLinkBase *  _pglbNext = _ppnbAppend->m_pglbLink;
        __SDP_TRANSFER(ppnbNew)->push_back_node( m_pppnbTail );

        for ( ;
              _ppnbAppend = _ppnbAppend->m_ppnbNext;
            )
        {
          ppnbNew.allocate( );
#ifndef __STL_USE_EXCEPTIONS
          if ( !ppnbNew )
          {
            _DestroyPath( _pppnbTailOld );
            return;
          }
#endif !__STL_USE_EXCEPTIONS
          new ( ppnbNew ) t_TyPathNodeBase( _ppnbAppend->m_pgnbNode );
          ppnbNew->m_pglbLink = _pglbNext;
          _pglbNext = _ppnbAppend->m_pglbLink;
          __SDP_TRANSFER(ppnbNew)->insert_node_in_middle( *_pppnbTailOld );
        }
        assert( !_pglbNext );
      }
      __STL_UNWIND( _DestroyPath( _pppnbTailOld ) )

      // We succeeded - if we had an old tail then remove it now:
      if ( _ppnbTailOld )
      {
        assert( !_ppnbTailOld->m_pglbLink );
        assert( _ppnbTailOld->m_pgnbNode == _ppnbTailOld->m_ppnbNext->m_pgnbNode ); // Paths must connect.
        _ppnbTailOld->remove_node_in_middle();
        _TyBaseAllocPathNode::deallocate_type( _ppnbTailOld );
      }
    }
    assert( valid(0) );
  }

#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyPathIter >
  void  _OverwritePath( _TyPathIter const & _r )
#else __STL_MEMBER_TEMPLATES
  void  _OverwritePath( _TyThis const & _r )
#endif __STL_MEMBER_TEMPLATES
  { 
    _OverwritePath( _r.m_ppnbHead, _r.m_pppnbTail, _r.m_uCount );
  }

  void  _OverwritePath( const t_TyPathNodeBase * const & _ppnbHead, 
                        const t_TyPathNodeBase * const * _pppnbTail, 
                        _TyGNIndex _uCount )
  { 
    const t_TyPathNodeBase * const * _pppnbCopyUntil = _pppnbTail;

    assert( valid(1) );

    // We will overwrite the current path with the one passed - 
    //  first make the list sizes the same:
    if ( m_uCount > _uCount )
    {
      // Then we need to destroy some nodes:
      t_TyPathNodeBase ** _pppnb = 
        t_TyPathNodeBase::PPGPNBGetNth( &m_ppnbHead, m_pppnbTail, 
                                        m_uCount, _uCount );
      _DestroyPath( _pppnb );
    }
    else
    if ( m_uCount < _uCount )
    {
      // Then we need to create some nodes - copy as we allocate:
      t_TyPathNodeBase ** _pppnbTailOld = m_pppnbTail; // Save the old tail - in case we throw:
      __STL_TRY
      {
        // Insert the first one normally - this one updates the tail - the rest we know
        //  won't touch the tail so we can use the faster method:
        _pppnbCopyUntil = t_TyPathNodeBase::PPPNBGetPrevPrevNext( _pppnbCopyUntil );
        __SDP( t_TyPathNodeBase, _TyPathNodeBaseAllocator, _GetPNBAllocator(),  ppnbNew );
        ppnbNew.allocate( );
        __SDP_CHECK_VOID( ppnbNew );
        new ( ppnbNew ) t_TyPathNodeBase( **_pppnbCopyUntil );
        __SDP_TRANSFER(ppnbNew)->push_back_node( m_pppnbTail );

        // Now insert the next nodes into the list before the position of the old tail:
        for ( ++m_uCount; m_uCount++ < _uCount; )
        {
          _pppnbCopyUntil = t_TyPathNodeBase::PPPNBGetPrevPrevNext( _pppnbCopyUntil );
          ppnbNew.allocate( );
#ifndef __STL_USE_EXCEPTIONS
          if ( !ppnbNew )
          {
            _DestroyPath( _pppnbTailOld );
            return;
          }
#endif !__STL_USE_EXCEPTIONS
          new ( ppnbNew ) t_TyPathNodeBase( **_pppnbCopyUntil );
          __SDP_TRANSFER(ppnbNew)->insert_node_in_middle( *_pppnbTailOld );
        }
      }
      __STL_UNWIND( _DestroyPath( _pppnbTailOld ) ) // revert and rethrow.
    }

    // Now overwrite the already existing nodes in the path - as no allocation occurs we need not try:
    t_TyPathNodeBase * _ppnbCopyTo = m_ppnbHead;
    for ( const t_TyPathNodeBase * const * _pppnbCopyFrom = &( _ppnbHead );
          _pppnbCopyFrom != _pppnbCopyUntil;
          _pppnbCopyFrom = &( (*_pppnbCopyFrom)->m_ppnbNext ),
          _ppnbCopyTo = _ppnbCopyTo->m_ppnbNext )
    {
      *_ppnbCopyTo = **_pppnbCopyFrom;
    }

    m_uCount = _uCount;

    assert( valid(0) );
  }

  // Specialize the case for overwriting the current path with a node iterator.
  void  _OverwritePath( _TyGraphNodeIter const & _r )
  {
    Clear();
    if ( _r.PGNBCur() )
    {
      _AppendNode( _r.PGNBCur(), 0 );
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
    assert( valid(1) );
    __SDP(  t_TyPathNodeBase, _TyPathNodeBaseAllocator, 
            _GetPNBAllocator(), ppnbAlloc );
    ppnbAlloc.allocate();
    __SDP_CHECK_VOID( ppnbAlloc );
    new ( ppnbAlloc ) t_TyPathNodeBase( _pgnb, _pglb );
    __SDP_TRANSFER(ppnbAlloc)->insert_node( m_ppnbHead, m_pppnbTail );
    m_uCount++;
  }

  void  _AppendNode( _TyGraphNodeBase * _pgnb, _TyGraphLinkBase * _pglb )
  {
    assert( valid(1) );
    __SDP(  t_TyPathNodeBase, _TyPathNodeBaseAllocator, 
            _GetPNBAllocator(), ppnbAlloc );
    ppnbAlloc.allocate();
    __SDP_CHECK_VOID( ppnbAlloc );
    new ( ppnbAlloc ) t_TyPathNodeBase( _pgnb, _pglb );
    __SDP_TRANSFER(ppnbAlloc)->push_back_node( m_pppnbTail );
    m_uCount++;
  }

  void  _AppendLinkPosIter( _TyGraphLinkPosIter const & _r )
  {
    // REVIEW: <dbien>: Don't know why i put this here, but
    //  i guess that algorithm is currently specific.
    assert( !m_uCount ); 
    t_TyPathNodeBase ** pppnbOldTail = m_pppnbTail;
    _TyGNIndex _uOldCount;
    __STL_TRY
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
    __STL_UNWIND( ( _DestroyPath( pppnbOldTail ), m_uCount = _uOldCount ) )

    // If we had an old tail then we need to add the path link that
    //  connects to the first added node.
    assert( !*pppnbOldTail ); // TBD if necessary.
  }

  void  _DestroyPath( t_TyPathNodeBase ** _pppnbStart ) _STLP_NOTHROW
  {
    t_TyPathNodeBase * ppnbOldTail = *m_pppnbTail;
    t_TyPathNodeBase * ppnbCur = *_pppnbStart;
    m_pppnbTail = _pppnbStart;
    while ( ppnbCur != ppnbOldTail )
    {
      t_TyPathNodeBase * ppnb = ppnbCur;
      ppnbCur = ppnbCur->m_ppnbNext;
      ppnb->t_TyPathNodeBase::~t_TyPathNodeBase();
      _TyBaseAllocPathNode::deallocate_type( ppnb );
    }
    // If the new tail has a pointer to a link then rid it:
    if ( ppnbOldTail )
    {
      assert( 0 );  // Does this happen?
      ppnbOldTail->m_pglbLink = 0;
    }
  }

public:

  explicit _graph_path_iterator_base_notsafe( 
    _TyPathNodeBaseAllocatorAsPassed const & _alloc ) _STLP_NOTHROW
    : _TyBaseAllocPathNode( _alloc )
  {
  }

  explicit _graph_path_iterator_base_notsafe( _graph_path_iterator_base_notsafe const & _r )
   : _TyBaseAllocPathNode( _r )     
  {
    _AppendPath( _r.m_ppnbHead );
    m_uCount = _r.m_uCount; // throwsafe
  }

// Construction with a path and a different allocator:
#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyPathIter >
  _graph_path_iterator_base_notsafe(  _TyPathIter const & _r,
                                      _TyPathNodeBaseAllocatorAsPassed const & _alloc )
#else __STL_MEMBER_TEMPLATES
  _graph_path_iterator_base_notsafe(  _TyThis const & _r,
                                      _TyPathNodeBaseAllocatorAsPassed const & _alloc )
#endif __STL_MEMBER_TEMPLATES
    : _TyBaseAllocPathNode( _alloc )
  {
    _AppendPath( _r.m_ppnbHead );
    m_uCount = _r.m_uCount; // throwsafe
  }
  
  
  // Specialize construction with a graph node iterator:
  _graph_path_iterator_base_notsafe(  _TyGraphNodeIter const & _r,
                                      _TyPathNodeBaseAllocatorAsPassed const & _alloc )
    : _TyBaseAllocPathNode( _alloc )
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
  _graph_path_iterator_base_notsafe(  _TyGraphLinkPosIter const & _r,
                                      _TyPathNodeBaseAllocatorAsPassed const & _alloc )
    : _TyBaseAllocPathNode( _alloc )
  {
    _AppendLinkPosIter( _r );
  }

#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyPathIter >
  explicit _graph_path_iterator_base_notsafe( _TyPathIter const & _r )
    : _TyBaseAllocPathNode( _r._GetPNBAllocatorAsPassed() )
  {
    // This copies the path:
    _AppendPath( _r.m_ppnbHead );
    m_uCount = _r.m_uCount; // throwsafe
  }
#endif __STL_MEMBER_TEMPLATES

  // Obtain the path node allocator from iterator but do not copy the nodes:
#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyPathIter >
  explicit _graph_path_iterator_base_notsafe( _TyPathIter const & _r, __false_type )
#else __STL_MEMBER_TEMPLATES
  explicit _graph_path_iterator_base_notsafe( _TyThis const & _r, __false_type )
#endif __STL_MEMBER_TEMPLATES
    : _TyBaseAllocPathNode( _r._GetPNBAllocatorAsPassed() )
  {
  }

  ~_graph_path_iterator_base_notsafe() _STLP_NOTHROW
  {
    assert( valid( 1 ) );
    _DestroyPath( &m_ppnbHead );
  }

// allocator access:
  // Return the actual path node allocator:
  _TyPathNodeBaseAllocator & _GetPNBAllocator() _STLP_NOTHROW
  {
    return _TyBaseAllocPathNode::get_allocator_ref();
  }
  _TyPathNodeBaseAllocator const & _GetPNBAllocator() const _STLP_NOTHROW
  {
    return _TyBaseAllocPathNode::get_allocator_ref();
  }
  // Return the allocator as passed in template arg:
  _TyPathNodeBaseAllocatorAsPassed _GetPNBAllocatorAsPassed() const _STLP_NOTHROW
  {
    return _TyBaseAllocPathNode::get_allocator();
  }

  void  Clear() _STLP_NOTHROW
  {
    _DestroyPath( &m_ppnbHead );
  }

  _TyGraphNodeBase * FirstParent()
  {
    _TyGraphLinkBase * _pglbParent = *m_ppnbHead->m_pgnbNode->PPGLBParentHead();
    if ( _pglbParent )
    {
      _PushNode( _pglbParent->m_pgnbNodeParent, _pglbParent );
      return _pglbParent->m_pgnbNodeParent;
    }
    return 0;
  }
  _TyGraphNodeBase * GoParent( _TyGNIndex _u )
  {
    // Add the <_u>th parent to the front of the path:
    _TyGraphLinkBase * _pglbParent = *_TyGraphLinkBase::PPGLBGetNthParent( m_ppnbHead->m_pgnbNode->PPGLBParentHead(), _u );
    if ( _pglbParent )
    {
      _PushNode( _pglbParent->m_pgnbNodeParent, _pglbParent );
      return _pglbParent->m_pgnbNodeParent;
    }
    return 0;
  }
  _TyGraphNodeBase * FirstChild()
  {
    // Add the 1st parent to the front of the path:
    _TyGraphLinkBase * _pglbChild = *m_ppnbHead->m_pgnbNode->PPGLBChildHead();
    if ( _pglbChild )
    {
      _PushNode( _pglbChild->m_pgnbNodeChild, _pglbChild );
      return _pglbChild->m_pgnbNodeChild;
    }
    return 0;
  }
  _TyGraphNodeBase * GoChild( _TyGNIndex _u )
  {
    // Add the <_u>th child to the front of the path:
    _TyGraphLinkBase * _pglbChild = *_TyGraphLinkBase::PPGLBGetNthChild( m_ppnbHead->m_pgnbNode->PPGLBChildHead(), _u );
    if ( _pglbChild )
    {
      _PushNode( _pglbChild->m_pgnbNodeChild, _pglbChild );
      return _pglbChild->m_pgnbNodeChild;
    }
    return 0;
  }

  _TyGraphNodeBase * FirstRelation( bool _fChild ) _STLP_NOTHROW
  {
    if ( _fChild )  
      return FirstChild();
    else            
      return FirstParent();
  }
  _TyGraphNodeBase * GoRelation( bool _fChild, _TyGNIndex _u ) _STLP_NOTHROW
  {
    if ( _fChild )  
      return GoChild( _u );
    else
      return GoParent( _u );
  }

  _TyThis & operator = ( _graph_path_iterator_base_notsafe const & _r )
  {
    // Overwrite the current path with that passed:
    _OverwritePath( _r );
    return *this;
  }

#ifdef __STL_MEMBER_TEMPLATES
  template < class _TyIter >
  _TyThis & operator = ( _TyIter const & _r )
  {
    // Overwrite the current path with that passed:
    _OverwritePath( _r );
    return *this;
  }
#endif __STL_MEMBER_TEMPLATES

#ifdef __STL_MEMBER_TEMPLATES
  // Safe version - exact matching of templates - no derived->base conversion:
  // otherwise base class is accepted ( and can be misused ).
  template < class t_TySwap >
  void
  swap( t_TySwap & )
  {
    ___semantic_error_object  error;
  }
#endif __STL_MEMBER_TEMPLATES

  void
  swap( _TyThis & _r ) _STLP_NOTHROW
  {
    _TyBase::swap( static_cast< _TyBase& >( _r ) );
  }
};

__DGRAPH_END_NAMESPACE

#endif __GR_ITER_H
