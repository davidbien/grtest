#ifndef __GR_TYPE_H
#define __GR_TYPE_H

// _gr_type.h

// dgraph: typed links and nodes.

// Templatize each by both the node type and the element type ( design decision ).
// This allows typed access to connect graph objects - at the cost of more specified
//  types - which could potentially cause more code, except:
// i) Most of the methods are inline - if not all.
// ii) The likelihood of coupling some node type with two different link types is slim.

__DGRAPH_BEGIN_NAMESPACE

template < class t_TyNodeEl, class t_TyLinkEl, class t_TyGraphNodeBase >
class _graph_node;

template < class t_TyLinkEl, class t_TyNodeEl, class t_TyGraphLinkBase >
class _graph_link : public t_TyGraphLinkBase
{
  typedef t_TyGraphLinkBase                                         _TyBase;
  typedef _graph_link< t_TyLinkEl, t_TyNodeEl, t_TyGraphLinkBase >  _TyThis;
public:

  typedef t_TyLinkEl                                            _TyLinkEl;
  typedef t_TyNodeEl                                            _TyNodeEl;
  typedef _TyLinkEl                                             _TyElement;
  typedef t_TyGraphLinkBase                                     _TyGraphLinkBase;
  typedef typename t_TyGraphLinkBase::_TyGraphLinkBase          _TyGraphLinkBaseBase;
  typedef typename _TyGraphLinkBase::_TyGraphNodeBase           _TyGraphNodeBase;
  typedef typename _TyGraphNodeBase::_TyGraphNodeBase           _TyGraphNodeBaseBase;
  typedef _graph_node< _TyNodeEl, _TyLinkEl, _TyGraphNodeBase > _TyGraphNode;

  explicit _graph_link( _TyLinkEl const & _rtEl )
    : m_tEl( _rtEl )
  {
  }

  _graph_link()
  {
  }

  // This accesses the constructable/destructable object within 
  //  the _graph_link. This allows implementations to override this 
  //  implementation class to support different types for the 
  //  constructable object and the link element - see "_gr_shwr.h":
  _TyLinkEl & RElObject() _BIEN_NOTHROW
  {
    return m_tEl;
  }

  // Element access:
  _TyLinkEl & REl() _BIEN_NOTHROW
  {
    return m_tEl;
  }
  const _TyLinkEl & REl() const _BIEN_NOTHROW
  {
    return m_tEl;
  }
  _TyLinkEl & RElNonConst() _BIEN_NOTHROW
  {
    return m_tEl;
  }
  const _TyLinkEl & RElConst() const _BIEN_NOTHROW
  {
    return m_tEl;
  }

  operator _TyLinkEl & () _BIEN_NOTHROW 
  { 
    return m_tEl; 
  }
  operator const _TyLinkEl & () const _BIEN_NOTHROW 
  { 
    return m_tEl; 
  }

  // typed child/parent node access:
  _TyGraphNode * PGNChild() _BIEN_NOTHROW
  {
    return static_cast< _TyGraphNode* >( _TyBase::PGNBChild() );
  }
  _TyGraphNode * PGNParent() _BIEN_NOTHROW
  {
    return static_cast< _TyGraphNode* >( _TyBase::PGNBParent() );
  }

  const _TyGraphNode * PGNChild() const _BIEN_NOTHROW
  {
    return const_cast< const _TyGraphNode* >( static_cast< _TyGraphNode* >( _TyBase::PGNBChild() ) );
  }
  const _TyGraphNode * PGNParent() const _BIEN_NOTHROW
  {
    return const_cast< const _TyGraphNode* >( static_cast< _TyGraphNode* >( _TyBase::PGNBParent() ) );
  }

  _TyThis ** PPGLGetNextChild() _BIEN_NOTHROW
  {
    return (_TyThis**)_TyBase::PPGLBGetNextChild();
  }
  const _TyThis * const * PPGLGetNextChild() const _BIEN_NOTHROW
  {
    return (const _TyThis* const *)_TyBase::PPGLBGetNextChild();
  }

  _TyThis ** PPGLGetNextParent() _BIEN_NOTHROW
  {
    return (_TyThis**)_TyBase::PPGLBGetNextParent();
  }
  const _TyThis * const * PPGLGetNextParent() const _BIEN_NOTHROW
  {
    return (const _TyThis* const *)_TyBase::PPGLBGetNextParent();
  }

  _TyThis ** PPGLGetThisChild() _BIEN_NOTHROW
  {
    return (_TyThis**)_TyBase::PPGLBGetThisChild();
  }
  const _TyThis * const * PPGLGetThisChild() const _BIEN_NOTHROW
  {
    return (const _TyThis* const *)_TyBase::PPGLBGetThisChild();
  }

  _TyThis ** PPGLGetThisParent() _BIEN_NOTHROW
  {
    return (_TyThis**)_TyBase::PPGLBGetThisParent();
  }
  const _TyThis * const * PPGLGetThisParent() const _BIEN_NOTHROW
  {
    return (const _TyThis* const *)_TyBase::PPGLBGetThisParent();
  }

  _TyThis * PGLGetPrevChild() _BIEN_NOTHROW
  {
    return (_TyThis*)_TyBase::PGLBGetPrevChild();
  }
  const _TyThis * PGLGetPrevChild() const _BIEN_NOTHROW
  {
    return (const _TyThis*)_TyBase::PGLBGetPrevChild();
  }

  _TyThis * PGLGetPrevParent() _BIEN_NOTHROW
  {
    return (_TyThis*)_TyBase::PGLBGetPrevParent();
  }
  const _TyThis * PGLGetPrevParent() const _BIEN_NOTHROW
  {
    return (const _TyThis*)_TyBase::PGLBGetPrevParent();
  }

  void InsertParent( _TyThis ** _ppgl ) _BIEN_NOTHROW
  {
    _TyGraphLinkBase::InsertParent( (_TyGraphLinkBase**)_ppgl );
  }
  void InsertChild( _TyThis ** _ppgl ) _BIEN_NOTHROW
  {
    _TyGraphLinkBase::InsertChild( (_TyGraphLinkBase**)_ppgl );
  }
  void InsertParentAssume( _TyThis ** _ppgl ) _BIEN_NOTHROW
  {
    _TyGraphLinkBase::InsertParentAssume( (_TyGraphLinkBase**)_ppgl );
  }
  void InsertChildAssume( _TyThis ** _ppgl ) _BIEN_NOTHROW
  {
    _TyGraphLinkBase::InsertChildAssume( (_TyGraphLinkBase**)_ppgl );
  }

protected:
  t_TyLinkEl    m_tEl;
};

template < class t_TyNodeEl, class t_TyLinkEl, class t_TyGraphNodeBase >
class _graph_node : public t_TyGraphNodeBase
{
  typedef t_TyGraphNodeBase                                         _TyBase;
  typedef _graph_node< t_TyNodeEl, t_TyLinkEl, t_TyGraphNodeBase >  _TyThis;
public:

  typedef t_TyNodeEl                                            _TyNodeEl;
  typedef t_TyLinkEl                                            _TyLinkEl;
  typedef _TyNodeEl                                             _TyElement;
  typedef t_TyGraphNodeBase                                     _TyGraphNodeBase;
  typedef typename t_TyGraphNodeBase::_TyGraphNodeBase          _TyGraphNodeBaseBase;
  typedef typename _TyGraphNodeBase::_TyGraphLinkBase           _TyGraphLinkBase;
  typedef typename _TyGraphLinkBase::_TyGraphLinkBase           _TyGraphLinkBaseBase;
  typedef _graph_link< _TyLinkEl, _TyNodeEl, _TyGraphLinkBase > _TyGraphLink;

  explicit _graph_node( _TyNodeEl const & _rtEl )
    : m_tEl( _rtEl )
  {
  }

  _graph_node()
  {
  }

  // This accesses the constructable/destructable object within 
  //  the _graph_node. This allows implementations to override this 
  //  implementation class to support different types for the 
  //  constructable object and the node element - see "_gr_shwr.h":
  _TyNodeEl & RElObject() _BIEN_NOTHROW
  {
    return m_tEl;
  }

  // Element access:
  _TyNodeEl & REl() _BIEN_NOTHROW
  {
    return m_tEl;
  }
  const _TyNodeEl & REl() const _BIEN_NOTHROW
  {
    return m_tEl;
  }
  _TyNodeEl & RElNonConst() _BIEN_NOTHROW
  {
    return m_tEl;
  }
  const _TyNodeEl & RElConst() const _BIEN_NOTHROW
  {
    return m_tEl;
  }

  operator _TyNodeEl & () _BIEN_NOTHROW 
  { 
    return m_tEl; 
  }
  operator const _TyNodeEl & () const _BIEN_NOTHROW 
  { 
    return m_tEl; 
  }

  // child/parent typed link list access:
  _TyGraphLink **           PPGLChildHead() _BIEN_NOTHROW
  {
    return (_TyGraphLink**)_TyBase::PPGLBChildHead();
  }
  const _TyGraphLink * const *    PPGLChildHead() const _BIEN_NOTHROW
  {
    return (const _TyGraphLink* const *)_TyBase::PPGLBChildHead();
  }

  _TyGraphLink **     PPGLParentHead() _BIEN_NOTHROW
  {
    return (_TyGraphLink**)_TyBase::PPGLBParentHead();
  }
  const _TyGraphLink * const *  PPGLParentHead() const _BIEN_NOTHROW
  {
    return (const _TyGraphLink* const *)_TyBase::PPGLBParentHead();
  }

  _TyGraphLink **     PPGLRelationHead( bool _fChild ) _BIEN_NOTHROW
  {
    return (_TyGraphLink**)_TyBase::PPGLBRelationHead( _fChild );
  }
  const _TyGraphLink * const *      PPGLRelationHead( bool _fChild ) const _BIEN_NOTHROW
  {
    return (const _TyGraphLink* const *)_TyBase::PPGLBRelationHead( _fChild );  
  }

protected:
  _TyNodeEl   m_tEl;  // the element in the node.
};

__DGRAPH_END_NAMESPACE

#endif //__GR_TYPE_H

