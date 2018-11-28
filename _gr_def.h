#ifndef __GR_DEF_H
#define __GR_DEF_H

// _gr_def.h

// This module defines the default graph objects.

#if defined( __STL_USE_EXCEPTIONS )
//    !defined( __GR_DONTTHROWBADGRAPHERRORS )
// Throw graph navigation errors.
#define __GR_THROWGRAPHNAVERRORS
#endif

#ifdef __GR_THROWGRAPHNAVERRORS
// We use the SGI exception stuff:
#include <stdexcept>
#endif __GR_THROWGRAPHNAVERRORS

__DGRAPH_BEGIN_NAMESPACE

#ifdef __GR_THROWGRAPHNAVERRORS

class _graph_nav_except : public _t__Named_exception< __DGRAPH_DEFAULT_ALLOCATOR >
{
  typedef _t__Named_exception< __DGRAPH_DEFAULT_ALLOCATOR > _TyBase;
public:
  _graph_nav_except( const string_type & __s ) : _TyBase( __s ) {}
};

#else __GR_THROWGRAPHNAVERRORS
#error This not currently supported. Need error propagation - not too tough, but...
#endif __GR_THROWGRAPHNAVERRORS

// Predeclare:
class _graph_node_base;
template < class t_TyGraphNodeBase >  struct _graph_link_base;

// Graph link stuff.
// Graph links link graph nodes to graph nodes.

template < class t_TyGraphLinkBase >
_TyGNIndex
GLB_UCountParents( const t_TyGraphLinkBase * _pglb ) __STL_NOTHROW
{
  _TyGNIndex u = 0;
  for ( ;
        _pglb;
        _pglb = _pglb->m_pglbNextParent, ++u )
    ;
  return u;
}

template < class t_TyGraphLinkBase >
_TyGNIndex
GLB_UCountParentsBefore(  const t_TyGraphLinkBase * _pglb, 
                          const t_TyGraphLinkBase ** _ppglbHead ) __STL_NOTHROW
{
  // Walk the head to the element - this let's the end element work correctly ( _pglb == 0 ).
  _TyGNIndex u = 0;
  for ( ;
        _pglb != *_ppglbHead;
        _ppglbHead = &( (*_ppglbHead)->m_pglbNextParent ), ++u )
    ;
  return u;
}

template < class t_TyGraphLinkBase >
t_TyGraphLinkBase * const *
GLB_PPGLBGetNthParent( t_TyGraphLinkBase * const * _ppglbAfter, _TyGNIndex _u ) __STL_NOTHROW
{
  for ( ; _u-- && *_ppglbAfter; _ppglbAfter = &( (*_ppglbAfter)->m_pglbNextParent ) )
    ;
  return _ppglbAfter;
}

template < class t_TyGraphLinkBase >
_TyGNIndex
GLB_UCountChildren( const t_TyGraphLinkBase * _pglb ) __STL_NOTHROW
{
  _TyGNIndex u = 0;
  for ( ;
        _pglb;
        _pglb = _pglb->m_pglbNextChild, ++u )
    ;
  return u;
}

template < class t_TyGraphLinkBase >
_TyGNIndex
GLB_UCountChildrenBefore( const t_TyGraphLinkBase * _pglb, 
                          const t_TyGraphLinkBase ** _ppglbHead ) __STL_NOTHROW
{
  // Walk the head to the element - this let's the end element work correctly ( _pglb == 0 ).
  _TyGNIndex u = 0;
  for ( ;
        _pglb != *_ppglbHead;
        _ppglbHead = &( *_ppglbHead->m_pglbNextChild ), ++u )
    ;
  return u;
}

template < class t_TyGraphLinkBase >
t_TyGraphLinkBase * const *
GLB_PPGLBGetNthChild( t_TyGraphLinkBase * const * _ppglbAfter, _TyGNIndex _u ) __STL_NOTHROW
{
  for ( ; _u-- && *_ppglbAfter; _ppglbAfter = &( (*_ppglbAfter)->m_pglbNextChild ) )
    ;
  return _ppglbAfter;
}

// POD
template < class t_TyGraphNodeBase >
struct _graph_link_base
{
  typedef _graph_link_base< t_TyGraphNodeBase > _TyThis;

  _TyThis **          m_ppglbPrevNextParent;
  _TyThis **          m_ppglbPrevNextChild;
  t_TyGraphNodeBase * m_pgnbNodeParent;
  t_TyGraphNodeBase * m_pgnbNodeChild;
  _TyThis *           m_pglbNextParent; // Keep these last - as we will maintain the same relative positions in
  _TyThis *           m_pglbNextChild;  //  both the graph link->safe graph link and the graph node->safe graph node
                                        //  this let's us place information about the position of the link with 
                                        //  minimal costs ( and still keeps the non-safe and safe versions of the 
                                        //  data structures the same - sharing the same base code ).
  // PUT NO MEMBERS HERE.

  // No need for construction as all fields are populated by use.

public:

  typedef t_TyGraphNodeBase   _TyGraphNodeBase;
  typedef _TyThis             _TyGraphLinkBase;

  void  Init() __STL_NOTHROW
  {
    // Nothing to be done - all should be initialized by user.
  }

  bool  FIsConstructed() const __STL_NOTHROW
  {
    // Currently a link is constructed ( i.e. the construction of the link is owned
    //  by the graph ) when it has both a parent and a child node connected to it.
    // If constructed links that have one or no nodes connected to it then a boolean
    //  would need to be added ( m_fIsConstructed ).
    return m_pgnbNodeParent && m_pgnbNodeChild;
  }

  void        SetParentNode( t_TyGraphNodeBase *  _pgnbNodeParent ) __STL_NOTHROW
  {
    m_pgnbNodeParent = _pgnbNodeParent;
  }
  void        SetChildNode( t_TyGraphNodeBase * _pgnbNodeChild ) __STL_NOTHROW
  {
    m_pgnbNodeChild = _pgnbNodeChild;
  }
  void        SetRelationNode( bool _fChild, t_TyGraphNodeBase *  _pgnbNode ) __STL_NOTHROW
  {
    if ( _fChild )
      SetChildNode( _pgnbNode );
    else
      SetParentNode( _pgnbNode );
  }

  t_TyGraphNodeBase * PGNBParent() const __STL_NOTHROW
  {
    return m_pgnbNodeParent;
  }
  t_TyGraphNodeBase * PGNBChild() const __STL_NOTHROW
  {
    return m_pgnbNodeChild;
  }
  t_TyGraphNodeBase * PGNBRelation( bool _fChild ) const __STL_NOTHROW
  {
    return _fChild ? m_pgnbNodeChild : m_pgnbNodeParent;
  }

  _TyGNIndex  UCountParents() const __STL_NOTHROW
  {
    return GLB_UCountParents( this );
  }

  // Operations:
  _TyGNIndex  UParentsBefore() const __STL_NOTHROW
  {
    return GLB_UCountParentsBefore( this, m_pgnbNodeChild->PPGLBParentHead() );
  }

  static _TyThis * const *  
  PPGLBGetNthParent( _TyThis * const * _ppglbAfter, _TyGNIndex _u ) __STL_NOTHROW
  {
    return GLB_PPGLBGetNthParent( _ppglbAfter, _u );
  }
  static _TyThis ** 
  PPGLBGetNthParent( _TyThis ** _ppglbAfter, _TyGNIndex _u ) __STL_NOTHROW
  {
    return const_cast< _TyThis ** >( GLB_PPGLBGetNthParent( _ppglbAfter, _u ) );
  }

  static _TyThis *  PGLBGetThisFromPPGLBNextParent( _TyThis ** _ppglbNextParent )
  {
    return reinterpret_cast< _TyThis * >( 
            reinterpret_cast< char* >( _ppglbNextParent ) - 
              offsetof( _TyThis, m_pglbNextParent ) );
  }

  // Return the address of the next parent in the list.
  _TyThis * const * PPGLBGetNextParent() const __STL_NOTHROW
  {
    return &m_pglbNextParent;
  }
  _TyThis **  PPGLBGetNextParent() __STL_NOTHROW
  {
    return &m_pglbNextParent;
  }
  _TyThis *   PGLBGetNextParent() const __STL_NOTHROW
  {
    return m_pglbNextParent;
  }
  // Return the address of the pointer to this graph link in the parent list.
  _TyThis **  PPGLBGetThisParent() const __STL_NOTHROW
  {
    return m_ppglbPrevNextParent;
  }
  // Return the previous graph link in the parent list.
  _TyThis *   PGLBGetPrevParent() const __STL_NOTHROW
  {
    return reinterpret_cast< const _TyThis * >( 
      reinterpret_cast< char* >(m_ppglbPrevNextParent) - 
        offsetof(_TyThis,m_pglbNextParent) );
  }
  // Return the pointer to the address of the previous element in the parent list.
  _TyThis **  PPGLBGetPrevParent() const __STL_NOTHROW
  {
    return PGLBGetPrevParent()->m_ppglbPrevNextParent;
  }

  void      InsertParent( _TyThis ** _ppglbBefore ) __STL_NOTHROW
  {
    m_ppglbPrevNextParent = _ppglbBefore;
    m_pglbNextParent = *_ppglbBefore;
    if ( *_ppglbBefore )
    {
      (*_ppglbBefore)->m_ppglbPrevNextParent = &m_pglbNextParent;
    }
    *_ppglbBefore = this;
  }
  void      InsertParentAssume( _TyThis ** _ppglbBefore ) __STL_NOTHROW
  {
    m_ppglbPrevNextParent = _ppglbBefore;
    m_pglbNextParent = *_ppglbBefore;
    (*_ppglbBefore)->m_ppglbPrevNextParent = &m_pglbNextParent;
    *_ppglbBefore = this;
  }
  // Append the parent list to the given tail of some parent list - used to move parent
  //  lists to nodes enmass:
  void      AppendParentListToTail( _TyThis ** _ppglbTail ) __STL_NOTHROW
  {
    assert( !*_ppglbTail ); // Must be tail.
    m_ppglbPrevNextParent = _ppglbTail;
    *_ppglbTail = this;
  }

  void      RemoveParent( ) const __STL_NOTHROW
  {
    if ( m_pglbNextParent )
    {
      m_pglbNextParent->m_ppglbPrevNextParent = m_ppglbPrevNextParent;
    }
    *m_ppglbPrevNextParent = m_pglbNextParent;
  }

  void      RemoveParentAssume( ) const __STL_NOTHROW
  {
    m_pglbNextParent->m_ppglbPrevNextParent = m_ppglbPrevNextParent;
    *m_ppglbPrevNextParent = m_pglbNextParent;
  }

  // Exchange the two parent links - <_ppglbParentBefore> must be before <_ppglbParentAfter>
  //  in the parent list - this allows optimization.
  // Actually <_ppglbParentBefore> can be after <_ppglbParentAfter> as long as <_ppglbParentBefore>
  //  is not the tail of the list.
  static void   ExchangeParentsOrdered( _TyThis * _pglbParentBefore, 
                                        _TyThis * _pglbParentAfter ) __STL_NOTHROW
  {
    if ( !_pglbParentBefore || !_pglbParentAfter )
    {
      throw _graph_nav_except("_graph_link_base::ExchangeParentsOrdered(): bad parameters.");
    }
    assert( _pglbParentBefore != _pglbParentAfter );  // not a nop.
    _TyThis ** ppglbPrevNextAfter = _pglbParentAfter->m_ppglbPrevNextParent;
    _pglbParentAfter->m_ppglbPrevNextParent = _pglbParentBefore->m_ppglbPrevNextParent;
    _pglbParentBefore->m_ppglbPrevNextParent = ppglbPrevNextAfter;
    _TyThis * pglbNextAfter = _pglbParentAfter->m_pglbNextParent;
    if ( pglbNextAfter )
    {
      pglbNextAfter->m_ppglbPrevNextParent = &( _pglbParentBefore->m_pglbNextParent );
    }
    _pglbParentBefore->m_pglbNextParent->m_ppglbPrevNextParent = &( _pglbParentAfter->m_pglbNextParent );
    _pglbParentAfter->m_pglbNextParent = _pglbParentBefore->m_pglbNextParent;
    _pglbParentBefore->m_pglbNextParent = pglbNextAfter;
  }

  static void   ExchangeParents(  _TyThis * _pglbParentBefore, 
                                  _TyThis * _pglbParentAfter ) __STL_NOTHROW
  {
    assert( _pglbParentBefore != _pglbParentAfter );  // not a nop.
    _TyThis ** ppglbPrevNextAfter = _pglbParentAfter->m_ppglbPrevNextParent;
    _pglbParentAfter->m_ppglbPrevNextParent = _pglbParentBefore->m_ppglbPrevNextParent;
    _pglbParentBefore->m_ppglbPrevNextParent = ppglbPrevNextAfter;
    _TyThis * pglbNextAfter = _pglbParentAfter->m_pglbNextParent;
    if ( pglbNextAfter )
    {
      pglbNextAfter->m_ppglbPrevNextParent = &( _pglbParentBefore->m_pglbNextParent );
    }
    if ( _pglbParentBefore->m_pglbNextParent )
    {
      _pglbParentBefore->m_pglbNextParent->m_ppglbPrevNextParent = &( _pglbParentAfter->m_pglbNextParent );
    }
    _pglbParentAfter->m_pglbNextParent = _pglbParentBefore->m_pglbNextParent;
    _pglbParentBefore->m_pglbNextParent = pglbNextAfter;
  }

  _TyGNIndex  UCountChildren() const __STL_NOTHROW
  {
    return GLB_UCountChildren( this );
  }

  _TyGNIndex  UChildrenBefore() const __STL_NOTHROW
  {
    return GLB_UCountChildrenBefore( this, m_pgnbNodeParent->PPGLBChildHead() );
  }

  static _TyThis * const *  
  PPGLBGetNthChild( _TyThis * const * _ppglbAfter, _TyGNIndex _u ) __STL_NOTHROW
  {
    return GLB_PPGLBGetNthChild( _ppglbAfter, _u );
  }
  static _TyThis ** 
  PPGLBGetNthChild( _TyThis ** _ppglbAfter, _TyGNIndex _u ) __STL_NOTHROW
  {
    return const_cast< _TyThis ** >( GLB_PPGLBGetNthChild( _ppglbAfter, _u ) );
  }

  static _TyThis *  PGLBGetThisFromPPGLBNextChild( _TyThis ** _ppglbNextChild )
  {
    return reinterpret_cast< _TyThis * >( 
            reinterpret_cast< char* >( _ppglbNextChild ) - 
              offsetof( _TyThis, m_pglbNextChild ) );
  }

  // Return the address of the next child in the list.
  _TyThis **  PPGLBGetNextChild() __STL_NOTHROW
  {
    return &m_pglbNextChild;
  }
  _TyThis * const * PPGLBGetNextChild() const __STL_NOTHROW
  {
    return &m_pglbNextChild;
  }
  _TyThis *   PGLBGetNextChild() const __STL_NOTHROW
  {
    return m_pglbNextChild;
  }

  // Return the address of the pointer to this graph link in the child list.
  _TyThis **  PPGLBGetThisChild() const __STL_NOTHROW
  {
    return m_ppglbPrevNextChild;
  }
  // Return the previous graph link in the child list.
  _TyThis * PGLBGetPrevChild() const __STL_NOTHROW
  {
    return reinterpret_cast< _TyThis * >( 
      reinterpret_cast< char* >( m_ppglbPrevNextChild ) - 
        offsetof( _TyThis,m_pglbNextChild ) );
  }
  // Return the pointer to the address of the previous element in the child list.
  _TyThis **  PPGLBGetPrevChild() const __STL_NOTHROW
  {
    return PGLBGetPrevChild()->m_ppglbPrevNextChild;
  }

  void      InsertChild( _TyThis ** _ppglbBefore ) __STL_NOTHROW
  {
    m_ppglbPrevNextChild = _ppglbBefore;
    m_pglbNextChild = *_ppglbBefore;
    if ( *_ppglbBefore )
    {
      (*_ppglbBefore)->m_ppglbPrevNextChild = &m_pglbNextChild;
    }
    *_ppglbBefore = this;
  }
  void      InsertChildAssume( _TyThis ** _ppglbBefore ) __STL_NOTHROW
  {
    m_ppglbPrevNextChild = _ppglbBefore;
    m_pglbNextChild = *_ppglbBefore;
    (*_ppglbBefore)->m_ppglbPrevNextChild = &m_pglbNextChild;
    *_ppglbBefore = this;
  }
  // Append the child list to the given tail of some child list - used to move child
  //  lists to nodes enmass:
  void      AppendChildListToTail( _TyThis ** _ppglbTail ) __STL_NOTHROW
  {
    assert( !*_ppglbTail ); // Must be tail.
    m_ppglbPrevNextChild = _ppglbTail;
    *_ppglbTail = this;
  }

  void      RemoveChild( ) const __STL_NOTHROW
  {
    if ( m_pglbNextChild )
    {
      m_pglbNextChild->m_ppglbPrevNextChild = m_ppglbPrevNextChild;
    }
    *m_ppglbPrevNextChild = m_pglbNextChild;
  }
  void      RemoveChildAssume( ) const __STL_NOTHROW
  {
    m_pglbNextChild->m_ppglbPrevNextChild = m_ppglbPrevNextChild;
    *m_ppglbPrevNextChild = m_pglbNextChild;
  }

// Relation methods:
  _TyGNIndex  UCountRelations( bool _fChildren ) const __STL_NOTHROW
  {
    return _fChildren ? GLB_UCountChildren( this ) : GLB_UCountParents( this );
  }

  _TyThis *   PGLBGetNextRelation( bool _fChild ) const __STL_NOTHROW
  {
    return _fChild ? m_pglbNextChild : m_pglbNextParent;
  }

  _TyThis **  PPGLBGetNextRelation( bool _fChild ) __STL_NOTHROW
  {
    return _fChild ? &m_pglbNextChild : &m_pglbNextParent;
  }
  _TyThis * const * PPGLBGetNextRelation( bool _fChild ) const __STL_NOTHROW
  {
    return _fChild ? &m_pglbNextChild : &m_pglbNextParent;
  }

  _TyThis **  PPGLBGetThisRelation( bool _fChild ) const __STL_NOTHROW
  {
    return _fChild ? PPGLBGetThisChild() : PPGLBGetThisParent();
  }

  void      InsertRelation( bool _fChild, _TyThis ** _ppglbBefore ) __STL_NOTHROW
  {
    _fChild ? InsertChild( _ppglbBefore ) : InsertParent( _ppglbBefore );
  }
  void      RemoveRelation( bool _fChild ) const __STL_NOTHROW
  {
    _fChild ? RemoveChild() : RemoveParent();
  }

  // Exchange the two child links - <_ppglbChildBefore> must be before <_ppglbChildAfter>
  //  in the child list - this allows optimization.
  // Actually <_ppglbChildBefore> can be after <_ppglbChildAfter> as long as <_ppglbChildBefore>
  //  is not the tail of the list.
  static void   ExchangeChildrenOrdered(  _TyThis * _pglbChildBefore, 
                                          _TyThis * _pglbChildAfter ) __STL_NOTHROW
  {
    assert( _pglbChildBefore != _pglbChildAfter );  // not a nop.
    _TyThis ** ppglbPrevNextAfter = _pglbChildAfter->m_ppglbPrevNextChild;
    _pglbChildAfter->m_ppglbPrevNextChild = _pglbChildBefore->m_ppglbPrevNextChild;
    _pglbChildBefore->m_ppglbPrevNextChild = ppglbPrevNextAfter;
    _TyThis * pglbNextAfter = _pglbChildAfter->m_pglbNextChild;
    if ( pglbNextAfter )
    {
      pglbNextAfter->m_ppglbPrevNextChild = &( _pglbChildBefore->m_pglbNextChild );
    }
    _pglbChildBefore->m_pglbNextChild->m_ppglbPrevNextChild = &( _pglbChildAfter->m_pglbNextChild );
    _pglbChildAfter->m_pglbNextChild = _pglbChildBefore->m_pglbNextChild;
    _pglbChildBefore->m_pglbNextChild = pglbNextAfter;
  }

  static void   ExchangeChildren( _TyThis * _pglbChildBefore, 
                                  _TyThis * _pglbChildAfter ) __STL_NOTHROW
  {
    assert( _pglbChildBefore != _pglbChildAfter );  // not a nop.
    _TyThis ** ppglbPrevNextAfter = _pglbChildAfter->m_ppglbPrevNextChild;
    _pglbChildAfter->m_ppglbPrevNextChild = _pglbChildBefore->m_ppglbPrevNextChild;
    _pglbChildBefore->m_ppglbPrevNextChild = ppglbPrevNextAfter;
    _TyThis * pglbNextAfter = _pglbChildAfter->m_pglbNextChild;
    if ( pglbNextAfter )
    {
      pglbNextAfter->m_ppglbPrevNextChild = &( _pglbChildBefore->m_pglbNextChild );
    }
    if ( _pglbChildBefore->m_pglbNextChild )
    {
      _pglbChildBefore->m_pglbNextChild->m_ppglbPrevNextChild = &( _pglbChildAfter->m_pglbNextChild );
    }
    _pglbChildAfter->m_pglbNextChild = _pglbChildBefore->m_pglbNextChild;
    _pglbChildBefore->m_pglbNextChild = pglbNextAfter;
  }
};

// Graph node:
// POD
class _graph_node_base
{
  typedef _graph_node_base        _TyThis;

public:

  typedef _graph_node_base            _TyGraphNodeBase;
  typedef _graph_link_base< _TyThis > _TyGraphLinkBase;

protected:

  _TyGraphLinkBase *  m_pglbParents;  // Parent list.
  _TyGraphLinkBase *  m_pglbChildren; // Child list.

public:

  __INLINE void Init()
  {
    m_pglbParents = 0;
    m_pglbChildren = 0;
  }

  _TyGNIndex    UParents() const __STL_NOTHROW
  {
    return m_pglbParents ? m_pglbParents->UCountParents() : 0; // tho' m_pglbParents->UCountParents() should work.
  }
  bool          FParents() const __STL_NOTHROW
  {
    return !!m_pglbParents;
  }
  _TyGraphLinkBase **     PPGLBParentHead() __STL_NOTHROW
  {
    return &m_pglbParents;
  }
  _TyGraphLinkBase * const *      PPGLBParentHead() const __STL_NOTHROW
  {
    return &m_pglbParents;
  }

  _TyThis * PGNBGetNthParent( const _TyGNIndex & _u ) const __STL_NOTHROW
  {
    _TyGraphLinkBase * const * ppglb = _TyGraphLinkBase::PPGLBGetNthParent( &m_pglbParents, _u );
    if ( !*ppglb )
    {
      throw _graph_nav_except("_graph_node_base::PGNBGetNthParent(): index beyond end.");
    }
    return (*ppglb)->m_pgnbNodeParent;
  }
  static _TyThis *  PGNBGetThisFromPPGLBParentList( _TyGraphLinkBase ** _ppglbParents )
  {
    return reinterpret_cast< _TyThis * >( reinterpret_cast< char * >( _ppglbParents ) - 
      offsetof(_TyThis,m_pglbParents) );
  }
  void      AddParent( _TyThis & _rgnbAdd, _TyGraphLinkBase & _rgl,
                      _TyGraphLinkBase *& _rpglInsertBeforeParent,
                      _TyGraphLinkBase *& _rpglInsertBeforeAddChild ) __STL_NOTHROW
  {
    _rgl.m_pgnbNodeChild = this;
    _rgl.m_pgnbNodeParent = &_rgnbAdd;
    _rgl.InsertChild( &_rpglInsertBeforeAddChild );
    _rgl.InsertParent( &_rpglInsertBeforeParent );
  }
  void  ExchangeParentsOrdered( _TyGNIndex _uMin, _TyGNIndex _uMax ) const __STL_NOTHROW
  {
    assert( _uMin < _uMax );
    assert( _uMin < UParents() );
    assert( _uMax < UParents() );
    _TyGraphLinkBase * const * _ppglbMin = _TyGraphLinkBase::PPGLBGetNthParent( &m_pglbParents, _uMin );
    _TyGraphLinkBase * const * _ppglbMax = _TyGraphLinkBase::PPGLBGetNthParent( _ppglbMin, _uMax - _uMin );
    _TyGraphLinkBase::ExchangeParentsOrdered( *_ppglbMin, *_ppglbMax );
  }
  void  MoveParentUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) __STL_NOTHROW
  {
    assert( _uRemove < _uInsert );
    assert( _uRemove < UParents()-1 );  // no-op otherwise.
    _TyGraphLinkBase ** _ppglbRemove = 
      _TyGraphLinkBase::PPGLBGetNthParent( &m_pglbParents, _uRemove );
    _TyGraphLinkBase ** _ppglbInsert = 
      _TyGraphLinkBase::PPGLBGetNthParent( _ppglbRemove, _uInsert - _uRemove );
    if ( *_ppglbRemove )
    {
      (*_ppglbRemove)->RemoveParentAssume();
      (*_ppglbRemove)->InsertParent( _ppglbInsert );
    }
    else
    {
      throw _graph_nav_except("_graph_node_base::MoveParentUp(): index beyond end.");
    }
  }
  void  MoveParentDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) __STL_NOTHROW
  {
    assert( _uInsert < _uRemove );
    _TyGraphLinkBase ** _ppglbInsert = _TyGraphLinkBase::PPGLBGetNthParent( &m_pglbParents, _uRemove );
    _TyGraphLinkBase ** _ppglbRemove = _TyGraphLinkBase::PPGLBGetNthParent( _ppglbInsert, _uRemove - _uInsert );
    if ( *_ppglbRemove )
    {
      (*_ppglbRemove)->RemoveParent();
      (*_ppglbRemove)->InsertParentAssume( _ppglbInsert );
    }
    else
    {
      throw _graph_nav_except("_graph_node_base::MoveParentDown(): index beyond end.");
    }
  }
  void  MoveParent( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) __STL_NOTHROW
  {
    if ( _uRemove < _uInsert )
    {
      MoveParentUp( _uRemove, _uInsert );
    }
    else
    {
      MoveParentDown( _uRemove, _uInsert );
    }
  }
  void      RemoveParent( const _TyGNIndex & _uRemove,
                          _TyThis ** _ppgnbRemoved,
                          _TyGraphLinkBase ** _ppglRemoved ) const __STL_NOTHROW
  {
    assert( _uRemove < UParents() );
    *_ppglRemoved = *_TyGraphLinkBase::PPGLBGetNthParent( &m_pglbParents, _uRemove );
    if ( *_ppglRemoved )
    {
      (*_ppglRemoved)->RemoveParent();
      *_ppgnbRemoved = (*_ppglRemoved)->m_pgnbNodeParent;
    }
    else
    {
      throw _graph_nav_except("_graph_node_base::RemoveParent(): index beyond end.");
    }
  }

  _TyGNIndex    UChildren() const __STL_NOTHROW
  {
    return m_pglbChildren ? m_pglbChildren->UCountChildren() : 0;
  }
  bool          FChildren() const __STL_NOTHROW
  {
    return !!m_pglbChildren;
  }
  _TyGraphLinkBase **           PPGLBChildHead() __STL_NOTHROW
  {
    return &m_pglbChildren;
  }
  _TyGraphLinkBase * const *    PPGLBChildHead() const __STL_NOTHROW
  {
    return &m_pglbChildren;
  }

  _TyThis * PGNBGetNthChild( const _TyGNIndex & _u ) const __STL_NOTHROW
  {
    _TyGraphLinkBase * const * ppglb = _TyGraphLinkBase::PPGLBGetNthChild( &m_pglbChildren, _u );
    if ( !*ppglb )
    {
      throw _graph_nav_except("_graph_node_base::PGNBGetNthChild(): index beyond end.");
    }
    return (*ppglb)->m_pgnbNodeChild;
  }
  static _TyThis *  PGNBGetThisFromPPGLBChildList( _TyGraphLinkBase ** _ppglbChildren )
  {
    return reinterpret_cast< _TyThis * >( reinterpret_cast< char * >( _ppglbChildren ) - 
      offsetof(_TyThis,m_pglbChildren) );
  }
  void      AddChild( _TyThis & _rgnbAdd, _TyGraphLinkBase & _rgl,
                      _TyGraphLinkBase *& _rpglInsertBeforeChild,
                      _TyGraphLinkBase *& _rpglInsertBeforeAddParent ) __STL_NOTHROW
  {
    _rgl.m_pgnbNodeParent = this;
    _rgl.m_pgnbNodeChild = &_rgnbAdd;
    _rgl.InsertParent( &_rpglInsertBeforeAddParent );
    _rgl.InsertChild( &_rpglInsertBeforeChild );
  }
  void  ExchangeChildren( _TyGNIndex _uMin, _TyGNIndex _uMax ) __STL_NOTHROW
  {
    assert( _uMin < _uMax );
    assert( _uMin < UChildren() );
    assert( _uMax < UChildren() );
    _TyGraphLinkBase ** _ppglbMin = _TyGraphLinkBase::PPGLBGetNthChild( &m_pglbChildren, _uMin );
    _TyGraphLinkBase ** _ppglbMax = _TyGraphLinkBase::PPGLBGetNthChild( _ppglbMin, _uMax - _uMin );
    _TyGraphLinkBase::ExchangeChildrenOrdered( *_ppglbMin, *_ppglbMax );
  }
  void  MoveChildUp( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) __STL_NOTHROW
  {
    assert( _uRemove < _uInsert );
    assert( _uRemove < UChildren()-1 ); // no-op otherwise.
    _TyGraphLinkBase ** _ppglbRemove = _TyGraphLinkBase::PPGLBGetNthChild( &m_pglbChildren, _uRemove );
    _TyGraphLinkBase ** _ppglbInsert = _TyGraphLinkBase::PPGLBGetNthChild( _ppglbRemove, _uInsert - _uRemove );
    if ( *_ppglbRemove )
    {
      (*_ppglbRemove)->RemoveChildAssume();
      (*_ppglbRemove)->InsertChild( _ppglbInsert );
    }
    else
    {
      throw _graph_nav_except("_graph_node_base::MoveChildUp(): index beyond end.");
    }
  }
  void  MoveChildDown( _TyGNIndex _uRemove, _TyGNIndex _uInsert ) __STL_NOTHROW
  {
    assert( _uInsert < _uRemove );
    _TyGraphLinkBase ** _ppglbInsert = _TyGraphLinkBase::PPGLBGetNthChild( &m_pglbChildren, _uRemove );
    _TyGraphLinkBase ** _ppglbRemove = _TyGraphLinkBase::PPGLBGetNthChild( _ppglbInsert, _uRemove - _uInsert );
    if ( *_ppglbRemove )
    {
      (*_ppglbRemove)->RemoveChild();
      (*_ppglbRemove)->InsertChildAssume( _ppglbInsert );
    }
    else
    {
      throw _graph_nav_except("_graph_node_base::MoveChildDown(): index beyond end.");
    }
  }
  void      MoveChild( const _TyGNIndex & _uRemove, const _TyGNIndex & _uInsert ) __STL_NOTHROW
  {
    if ( _uRemove < _uInsert )
    {
      MoveChildUp( _uRemove, _uInsert );
    }
    else
    {
      MoveChildDown( _uRemove, _uInsert );
    }
  }
  void      RemoveChild(  const _TyGNIndex & _uRemove,
                          _TyThis ** _ppgnbRemoved,
                          _TyGraphLinkBase ** _ppglRemoved ) __STL_NOTHROW
  {
    assert( _uRemove < UChildren() );
    *_ppglRemoved = *_TyGraphLinkBase::PPGLBGetNthChild( &m_pglbChildren, _uRemove );
    if ( *_ppglRemoved )
    {
      (*_ppglRemoved)->RemoveChild();
      *_ppgnbRemoved = (*_ppglRemoved)->m_pgnbNodeChild;
    }
    else
    {
      throw _graph_nav_except("_graph_node_base::RemoveChild(): index beyond end.");
    }
  }

  // Relation methods:
  _TyGNIndex    URelations( bool _fChildren ) const __STL_NOTHROW
  {
    return _fChildren ? UChildren() : UParents();
  }
  _TyGNIndex    FRelations( bool _fChildren ) const __STL_NOTHROW
  {
    return _fChildren ? FChildren() : FParents();
  }

  _TyGraphLinkBase **     PPGLBRelationHead( bool _fChild ) __STL_NOTHROW
  {
    return _fChild ? PPGLBChildHead() : PPGLBParentHead();
  }
  _TyGraphLinkBase * const *      PPGLBRelationHead( bool _fChild ) const __STL_NOTHROW
  {
    return _fChild ? PPGLBChildHead() : PPGLBParentHead();
  }
};

// Template functions for graph path node:
template < class t_TyPathNodeBase >
t_TyPathNodeBase **
GPNB_PPPNBGetNth( const t_TyPathNodeBase * const * _pppnbAfter, _TyGNIndex _u ) __STL_NOTHROW
{
  for ( ; _u--; _pppnbAfter = &( (*_pppnbAfter)->m_ppnbNext ) )
    ;
  return const_cast< t_TyPathNodeBase ** >( _pppnbAfter );
}

template < class t_TyPathNodeBase >
t_TyPathNodeBase **
GPNB_PPPNBGetNthPrev( const t_TyPathNodeBase * const * _pppnbBefore, _TyGNIndex _u ) __STL_NOTHROW
{
  for ( ; _u--; _pppnbBefore = t_TyPathNodeBase::PPPNBGetPrevPrevNext( _pppnbBefore ) )
    ;
  return const_cast< t_TyPathNodeBase ** >( _pppnbBefore );
}

template <  class t_TyGraphNodeBase, class t_TyGraphLinkBase >
class _graph_path_node_base
{
  typedef _graph_path_node_base< t_TyGraphNodeBase, t_TyGraphLinkBase > _TyThis;

public:

  typedef t_TyGraphLinkBase     _TyGraphLinkBase;
  typedef t_TyGraphNodeBase     _TyGraphNodeBase;
  typedef _TyThis               _TyPathNodeBase;
  
  t_TyGraphLinkBase *   m_pglbLink;     // link - may be null - if no previous graph path node.
  t_TyGraphNodeBase *   m_pgnbNode;     // node.
  _TyThis **            m_pppnbPrevNext;// pointer to address of the previous element's next pointer. 
  _TyThis *             m_ppnbNext;     // next in path.

  // unsafe constructor:
  _graph_path_node_base() __STL_NOTHROW
  {
  }

  explicit _graph_path_node_base( _TyThis const & _r ) __STL_NOTHROW
    : m_pgnbNode( _r.m_pgnbNode ),
      m_pglbLink( _r.m_pglbLink )
  {
  }

#if 0 // Not a safe constructor - use first ctor - just as unsafe - but advertises as such. 
  explicit _graph_path_node_base( const t_TyGraphNodeBase * _pgnbNode ) __STL_NOTHROW
    : m_pgnbNode( _pgnbNode )
  {
  }
#endif 0

  _graph_path_node_base(  t_TyGraphNodeBase * _pgnbNode,
                          t_TyGraphLinkBase * _pglbLink ) __STL_NOTHROW
    : m_pgnbNode( _pgnbNode ),
      m_pglbLink( _pglbLink )
  {
  }

  ~_graph_path_node_base() __STL_NOTHROW
  {
  }

  // These methods are not const safe - they are meant to be used
  //  in the implementation only.
  static _TyThis *  PGNBGetThisFromPrevNext( const _TyThis * const * _pppnbBefore ) __STL_NOTHROW
  {
    return (_TyThis*)( ((char*)_pppnbBefore) - offsetof( _TyThis, m_ppnbNext ) );
  }

  static _TyThis ** PPPNBGetPrevPrevNext( const _TyThis * const * _pppnbBefore ) __STL_NOTHROW
  {
    return PGNBGetThisFromPrevNext( _pppnbBefore )->m_pppnbPrevNext;
  }

  static _TyThis **   PPGPNBGetNth( const _TyThis * const * _pppnbAfter, 
                                    _TyGNIndex _u ) __STL_NOTHROW
  {
    return GPNB_PPPNBGetNth( _pppnbAfter, _u );
  }

  static _TyThis **   PPGPNBGetNthPrev( const _TyThis * const * _pppnbBefore, 
                                        _TyGNIndex _u ) __STL_NOTHROW
  {
    return GPNB_PPPNBGetNthPrev( _pppnbBefore, _u );
  }

  static _TyThis **   PPGPNBGetNth( const _TyThis * const * _pppnbHead, 
                                    const _TyThis * const * _pppnbTail, 
                                    _TyGNIndex _uCount, _TyGNIndex _u ) __STL_NOTHROW
  {
    // Choose the fastest way to get the desired node:
    if ( _u > ( _uCount / 2 ) )
    {
      return GPNB_PPPNBGetNthPrev( _pppnbTail, _uCount - _u );
    }
    else
    {
      return GPNB_PPPNBGetNth( _pppnbHead, _u );
    }
  }

  void  insert_node(  _TyThis *& _rpgnBefore,  
                      _TyThis **& _rpppnbTail ) __STL_NOTHROW
  {
    m_pppnbPrevNext = &_rpgnBefore;
    m_ppnbNext = _rpgnBefore;
    if ( _rpgnBefore )
    {
      _rpgnBefore->m_pppnbPrevNext = &m_ppnbNext;
    }
    else
    {
      assert( m_pppnbPrevNext == _rpppnbTail );
      _rpppnbTail = &m_ppnbNext;
    }
    _rpgnBefore = this;
  }

  // Insert into the middle of the list - assumes 
  void  insert_node_in_middle( _TyThis *& _rpgnBefore ) __STL_NOTHROW
  {
    assert( _rpgnBefore );
    m_pppnbPrevNext = &_rpgnBefore;
    m_ppnbNext = _rpgnBefore;
    _rpgnBefore->m_pppnbPrevNext = &m_ppnbNext;
    _rpgnBefore = this;
  }

  // Push this node onto the tail:
  void  push_back_node( _TyThis **& _rpppnbTail ) __STL_NOTHROW
  {
    m_pppnbPrevNext = _rpppnbTail;
    *_rpppnbTail = this;
    m_ppnbNext = 0;
    _rpppnbTail = &m_ppnbNext;
  }

  void  remove_node( _TyThis **& _rpppnbTail ) __STL_NOTHROW
  {
    if ( m_ppnbNext )
    {
      m_ppnbNext->m_pppnbPrevNext = m_pppnbPrevNext;
    }
    *m_pppnbPrevNext = m_ppnbNext;
    if ( _rpppnbTail == &m_ppnbNext )
    {
      _rpppnbTail = m_pppnbPrevNext;
    }
  }

  void  remove_node_in_middle( ) __STL_NOTHROW
  {
    m_ppnbNext->m_pppnbPrevNext = m_pppnbPrevNext;
    *m_pppnbPrevNext = m_ppnbNext;
  }

  void operator = ( const _TyThis & _r ) __STL_NOTHROW
  {
    m_pglbLink = _r.m_pglbLink;
    m_pgnbNode = _r.m_pgnbNode;
  }
};

// Predeclare iterator types:
template < class t_TyGraphNodeBase, class t_TyGraphLinkBase >     class _graph_node_iterator_base_notsafe;
template < class t_TyGraphNodeBase, class t_TyGraphLinkBase >     class _graph_link_pos_iterator_base_notsafe;
template < class t_TyGraphNodeBase, class t_TyGraphLinkBase >     class _graph_link_ident_iterator_base_notsafe;
template < class t_TyPathNodeBase, class t_TyPathNodeAllocator >  class _graph_path_iterator_base_notsafe;

// Passing allocators:
template <  class _TyGraphNodeSafe, class _TyGraphLinkSafe, 
            class t_TyFIsConstIter, class _TyGraphNodeIterBaseSafe >  class _graph_iterator_pass;
template <  class t_TyPathNodeBase, class t_TyPathNodeAllocator, 
            class t_TyFIsConstIter, class t_TyBaseClass >             class _graph_path_iterator_pass;

template < class t_TyPathNodeBase, class t_TyPathNodeAllocator >
struct _graph_base
  // Need this to store non-static allocators - no path nodes are allocated by this object:
  : public _alloc_base< t_TyPathNodeBase, t_TyPathNodeAllocator > 
{
  typedef _graph_base< t_TyPathNodeBase, t_TyPathNodeAllocator >  _TyThis;
protected:
  typedef _alloc_base< t_TyPathNodeBase, t_TyPathNodeAllocator >  _TyBaseAllocPathNodeBase;
public:

  typedef typename t_TyPathNodeBase::_TyGraphNodeBase   _TyGraphNodeBaseBase;
  typedef typename t_TyPathNodeBase::_TyGraphLinkBase   _TyGraphLinkBaseBase;
  typedef t_TyPathNodeBase                              _TyPathNodeBaseBase;
  typedef _TyGraphNodeBaseBase                          _TyGraphNodeBase;
  typedef _TyGraphLinkBaseBase                          _TyGraphLinkBase;
  typedef t_TyPathNodeBase                              _TyPathNodeBase;

  typedef _TyThis _TyGraphBaseBase;

  typedef t_TyPathNodeAllocator                                 _TyPathNodeBaseAllocatorAsPassed;
  typedef typename _TyBaseAllocPathNodeBase::_TyAllocatorType       _TyPathNodeBaseAllocator;
  typedef _allocator_set_notsafe_base< _TyPathNodeBaseAllocatorAsPassed > _TyAllocatorSet;

  // Define the various types of base iterators:
  typedef _graph_node_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >         _TyGraphNodeIterBaseNotSafe;
  typedef _graph_link_pos_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >     _TyGraphLinkPosIterBaseNotSafe;
  typedef _graph_link_ident_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >   _TyGraphLinkIdentIterBaseNotSafe;
  typedef _graph_path_iterator_base_notsafe< t_TyPathNodeBase, _TyPathNodeBaseAllocatorAsPassed >   _TyGraphPathIterBaseNotSafe;

  // Passing non-safe allocator types:
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __true_type, _TyGraphNodeIterBaseNotSafe >        _TyGraphNodeIterNotSafePassConst;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __true_type, _TyGraphLinkPosIterBaseNotSafe >     _TyGraphLinkPosIterNotSafePassConst;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __true_type, _TyGraphLinkIdentIterBaseNotSafe >   _TyGraphLinkIdentIterNotSafePassConst;
  typedef _graph_path_iterator_pass<  t_TyPathNodeBase, _TyPathNodeBaseAllocatorAsPassed,
                                      __true_type, _TyGraphPathIterBaseNotSafe >  _TyGraphPathIterNotSafePassConst;

  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __false_type, _TyGraphNodeIterBaseNotSafe >       _TyGraphNodeIterNotSafePassNonConst;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __false_type, _TyGraphLinkPosIterBaseNotSafe >    _TyGraphLinkPosIterNotSafePassNonConst;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __false_type, _TyGraphLinkIdentIterBaseNotSafe >  _TyGraphLinkIdentIterNotSafePassNonConst;
  typedef _graph_path_iterator_pass<  t_TyPathNodeBase, _TyPathNodeBaseAllocatorAsPassed,
                                      __false_type, _TyGraphPathIterBaseNotSafe > _TyGraphPathIterNotSafePassNonConst;

private:
  //  Guard access to this:
  _TyGraphNodeBase *  m_pgnbRoot;
protected:

  void  _SetRootNode( _TyGraphNodeBase *  _pgnbRoot ) __STL_NOTHROW
  {
    m_pgnbRoot = _pgnbRoot;
  }

  static __INLINE void  _deinit_node( _TyGraphNodeBase * ) __STL_NOTHROW
  {
    // Nothing to do in an unsafe graph.
  }
  static __INLINE void  _deinit_link( _TyGraphLinkBase * ) __STL_NOTHROW
  {
  }

public:

  explicit _graph_base( _TyPathNodeBaseAllocatorAsPassed const & _allocPathNode )
    : _TyBaseAllocPathNodeBase( _allocPathNode ),
      m_pgnbRoot( 0 )
  {
  }

  explicit _graph_base( _TyAllocatorSet const & _rAllocSet )
    : _TyBaseAllocPathNodeBase( _rAllocSet.m_allocPathNodeBase ),
      m_pgnbRoot( 0 )
  {
  }

  explicit _graph_base( _TyThis const & _r )
    : _TyBaseAllocPathNodeBase( _r.get_base_path_allocator() ),
      m_pgnbRoot( 0 )
    {
    }

  template < class t_TyGraphBase >
  explicit _graph_base( t_TyGraphBase & _r )
    : _TyBaseAllocPathNodeBase( _r.get_base_path_allocator() ),
      m_pgnbRoot( 0 )
    {
    }

  // Allocation stuff:
  _TyPathNodeBaseAllocator const &  get_base_path_allocator_ref() const __STL_NOTHROW { return _TyBaseAllocPathNodeBase::get_allocator_ref(); }
  _TyPathNodeBaseAllocator &        get_base_path_allocator_ref() __STL_NOTHROW { return _TyBaseAllocPathNodeBase::get_allocator_ref(); }
  _TyPathNodeBaseAllocatorAsPassed  get_base_path_allocator() const __STL_NOTHROW { return _TyBaseAllocPathNodeBase::get_allocator(); }

  _TyGraphNodeBase * _GetRootNode() __STL_NOTHROW
  {
    return m_pgnbRoot;
  }
  const _TyGraphNodeBase * _GetRootNode() const __STL_NOTHROW
  {
    return m_pgnbRoot;
  }

};

__DGRAPH_END_NAMESPACE

#endif __GR_DEF_H
