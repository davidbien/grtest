#ifndef __GR_TITR_H
#define __GR_TITR_H

// _gr_titr.h

// This module implements typed graph iterators.

__DGRAPH_BEGIN_NAMESPACE

// Node iterator:

// Typed iterator template:

// non-const version - this allows access to all base class methods:
template <  class t_TyIterTraits,
            class t_TyFIsConstIterator = typename t_TyIterTraits::_TyFIsConstIterator >
class _graph_typed_iterator
  : public t_TyIterTraits::_TyIterBase
{
  // All typed iterators are friends:
  template < class t__TyIterTraits, class t__TyFIsConstIterator >
  friend class _graph_typed_iterator;

protected:

  // Extract the various types from the trait object:
  typedef typename t_TyIterTraits::_TyNodeEl    _TyNodeEl;  // Not const specified.
  typedef typename t_TyIterTraits::_TyNodePtr   _TyNodePtr; // const specified.
  typedef typename t_TyIterTraits::_TyNodeRef   _TyNodeRef;
  typedef typename t_TyIterTraits::_TyLinkEl    _TyLinkEl;
  typedef typename t_TyIterTraits::_TyLinkPtr   _TyLinkPtr;
  typedef typename t_TyIterTraits::_TyLinkRef   _TyLinkRef;
  typedef typename t_TyIterTraits::_TyValueEl   _TyValueEl;
  typedef typename t_TyIterTraits::_TyValuePtr  _TyValuePtr;
  typedef typename t_TyIterTraits::_TyValueRef  _TyValueRef;

  typedef typename t_TyIterTraits::_TyGraphNode         _TyGraphNode;
  typedef typename t_TyIterTraits::_TyGraphNodeBase     _TyGraphNodeBase;
  typedef typename t_TyIterTraits::_TyGraphNodeBaseBase _TyGraphNodeBaseBase;
  typedef typename t_TyIterTraits::_TyGraphLink         _TyGraphLink;
  typedef typename t_TyIterTraits::_TyGraphLinkBase     _TyGraphLinkBase;
  typedef typename t_TyIterTraits::_TyGraphLinkBaseBase _TyGraphLinkBaseBase;

  typedef typename t_TyIterTraits::_TyGraphValue          _TyGraphValue;
  typedef typename t_TyIterTraits::_TyGraphValueBase      _TyGraphValueBase;
  typedef typename t_TyIterTraits::_TyGraphValueBaseBase  _TyGraphValueBaseBase;

  typedef typename t_TyIterTraits::_TyPathNodeAllocator         _TyPathNodeAllocator;
  typedef typename t_TyIterTraits::_TyPathNodeAllocatorAsPassed _TyPathNodeAllocatorAsPassed;

  typedef typename t_TyIterTraits::_TyInitArg       _TyInitArg;
  typedef typename t_TyIterTraits::_TyInitArgBase   _TyInitArgBase;

  typedef typename t_TyIterTraits::_TyPassIterNonConst                    _TyPassIterNonConst;
  typedef typename t_TyIterTraits::_TyPassIterConst                       _TyPassIterConst;

private:
  typedef typename t_TyIterTraits::_TyIterBase                            _TyBase;
  typedef _graph_typed_iterator< t_TyIterTraits, t_TyFIsConstIterator >   _TyThis;

public:

  typedef typename t_TyIterTraits::_TyIterBase                            _TyIterBase;
  typedef t_TyIterTraits                                                  _TyIterTraits;
  typedef t_TyFIsConstIterator                                            _TyFIsConstIterator;

  // Access to const/non-const iterator types:
  typedef _graph_typed_iterator< typename t_TyIterTraits::
            template rebind_const< false >::other >   iterator;
  typedef _graph_typed_iterator< typename t_TyIterTraits::
            template rebind_const< true >::other >    const_iterator;

  // Value types:
  typedef _TyNodeEl     node_value_type;
  typedef _TyNodePtr    node_pointer;
  typedef _TyNodeRef    node_reference;
                        
  typedef _TyLinkEl     link_value_type;
  typedef _TyLinkPtr    link_pointer;
  typedef _TyLinkRef    link_reference;
                        
  typedef _TyValueEl    value_type;
  typedef _TyValuePtr   pointer;
  typedef _TyValueRef   reference;

  _graph_typed_iterator()
  {
  }

  // Provide a copy constructor - the template for construction
  //  via iterator does not catch:
  _graph_typed_iterator( _graph_typed_iterator const & _r )
    : _TyBase( static_cast< const _TyIterBase & >( _r ) )
  {
  }

  explicit _graph_typed_iterator( _TyInitArg _piaCur )
    : _TyIterBase( ( _TyInitArgBase )( _piaCur ) )
  {
  }

  _graph_typed_iterator(  _TyInitArg _piaCur, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode )
    : _TyIterBase( ( _TyInitArgBase )( _piaCur ), _allocPathNode )
  {
  }

// Construction with an iterator and another allocator:
  template < class t_TyIter >
  _graph_typed_iterator(  t_TyIter const & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ), _allocPathNode )
  {
    __TRANSFER_CONST( typename t_TyIter::_TyFIsConstIterator, _TyFIsConstIterator );
    __ASSERT_SAME_TYPE( _TyGraphNode, typename t_TyIter::_TyGraphNode );
    __ASSERT_SAME_TYPE( _TyGraphLink, typename t_TyIter::_TyGraphLink );
  }

  // Specialize the versions that take a "pass" path - we will take posession from the pass path object.
  _graph_typed_iterator(  _TyPassIterConst & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode ) _BIEN_NOTHROW
  : _TyIterBase( _allocPathNode )
  {
    __TRANSFER_CONST( typename _TyPassIterConst::_TyFIsConstIterator, _TyFIsConstIterator );
    _r.swap( static_cast< _TyIterBase& >( *this ) );
  }
  _graph_typed_iterator(  _TyPassIterNonConst & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode ) _BIEN_NOTHROW
    : _TyIterBase( _allocPathNode )
  {
    __TRANSFER_CONST( typename _TyPassIterNonConst::_TyFIsConstIterator, _TyFIsConstIterator );
    _r.swap( static_cast< _TyIterBase& >( *this ) );
  }

// Construction with just an iterator:
  template < class t_TyIter >
  explicit _graph_typed_iterator( t_TyIter const & _r )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ) )
  {
    __TRANSFER_CONST( typename t_TyIter::_TyFIsConstIterator, _TyFIsConstIterator );
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
  }

  // Construction that just obtains the allocator - if any - this doesn't check const-correctness
  //  as we are not transferring graph information:
  template < class t_TyIter >
  _graph_typed_iterator( t_TyIter const & _r, std::false_type )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ), std::false_type() )
  {
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
  }

  // Construction that allows transformation to/from const - doesn't check const-correctness
  //  used by the graph to transform iterators:
  template < class t_TyIter >
  _graph_typed_iterator( t_TyIter const & _r, __gr_const_cast_iter )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ) )
  {
  }

  // specialize construction with a "pass" path iterator:
  explicit _graph_typed_iterator( _TyPassIterConst & _r ) _BIEN_NOTHROW
    : _TyIterBase( static_cast< typename _TyPassIterConst::_TyIterBase const & >( _r ), 
                   std::false_type() )
  {
    __TRANSFER_CONST( typename _TyPassIterConst::_TyFIsConstIterator, _TyFIsConstIterator );
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }
  explicit _graph_typed_iterator( _TyPassIterNonConst & _r ) _BIEN_NOTHROW
    : _TyIterBase( static_cast< typename _TyPassIterNonConst::_TyIterBase const & >( _r ), 
                   std::false_type() )
  {
    __TRANSFER_CONST( typename _TyPassIterNonConst::_TyFIsConstIterator, _TyFIsConstIterator );
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

  // assignment - similar to construction except we have already been populated
  //  with a reference to an allocator.
  template < class t_TyIter >
  _TyThis &
  operator = ( t_TyIter const & _r )
  {
    __TRANSFER_CONST( typename t_TyIter::_TyFIsConstIterator, _TyFIsConstIterator );
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
    _TyIterBase::operator = ( static_cast< typename t_TyIter::_TyIterBase const & >( _r ) );
  }

  // Initialization with a graph link position:
  _TyThis &
  operator = ( _TyGraphNode * _pgn )
  {
    SetPGNBCur( _pgn );
    return *this;
  }

  // Initialization with a graph link position:
  _TyThis &
  operator = ( _TyGraphLink ** _ppgl )
  {
    _TyBase::SetPPGLBCur( reinterpret_cast< _TyGraphLinkBaseBase ** >( _ppgl ) );
    return *this;
  }

  // Initialization with a graph link identity:
  _TyThis &
  operator = ( _TyGraphLink * _pgl )
  {
    _TyBase::SetPGLBCur( _pgl );
  }

  // Initialization from a pass iterator:
  _TyThis &
  operator = ( _TyPassIterConst & _r ) _BIEN_NOTHROW
  {
    // We swap our existence with the pass iterator:
    __TRANSFER_CONST( typename _TyPassIterConst::_TyFIsConstIterator, _TyFIsConstIterator );
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

  _TyThis &
  operator = ( _TyPassIterNonConst & _r ) _BIEN_NOTHROW
  {
    // We swap our existence with the pass iterator:
    __TRANSFER_CONST( typename _TyPassIterNonConst::_TyFIsConstIterator, _TyFIsConstIterator );
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

  reference operator* ( ) const _BIEN_NOTHROW
  { 
    return static_cast< _TyGraphValue* >( _TyIterBase::PBObjCur() )->RElNonConst(); 
  }
  pointer operator-> ( ) const _BIEN_NOTHROW
  { 
    return &(operator*()); 
  }

// Accessors:
// NOTE: Because this is a multi-use templatized wrapper - some of these methods
//  will not compile with each iterator base type - this, of course, is by design.
//  If an compile error results then you are using the method on the wrong iterator type.

  // Current node access:
  node_reference    GetCurNode() const _BIEN_NOTHROW
  { 
    return static_cast< _TyGraphNode* >( _TyIterBase::PGNBCur() )->RElNonConst();
  }

  // Accessors to parent and child nodes:
  node_reference    GetParentNode() const _BIEN_NOTHROW
  {
    return static_cast< _TyGraphNode* >( _TyIterBase::PGNBParent() )->RElNonConst(); 
  }
  node_reference    GetChildNode() const
  {
    return static_cast< _TyGraphNode* >( _TyIterBase::PGNBChild() )->RElNonConst(); 
  }
  _TyGraphNode * PGNParent() const _BIEN_NOTHROW
  {
    return static_cast< _TyGraphNode* >( _TyIterBase::PGNBParent() );
  }
  _TyGraphNode * PGNChild() const _BIEN_NOTHROW
  {
    return static_cast< _TyGraphNode* >( _TyIterBase::PGNBChild() );
  }

  // Accessor to current link:
  link_reference    GetCurLink() const _BIEN_NOTHROW
  { 
    return static_cast< _TyGraphLink* >( _TyIterBase::PGNBCur() )->RElNonConst();
  }

  // Typed graph object accessors:
  _TyInitArg PObjCur() const _BIEN_NOTHROW
  {
    return (_TyInitArg)_TyIterBase::PBObjCur();
  }

  _TyGraphLink ** PPGLCur() const _BIEN_NOTHROW
  {
    return (_TyGraphLink**)_TyIterBase::PPGLBCur();
  }
  void SetPPGLCur( _TyGraphLink ** _ppglCur )
  {
    _TyIterBase::SetPPGLBCur( (_TyGraphLinkBase**)_ppglCur );
  }

  _TyGraphLink * PGLCur() const _BIEN_NOTHROW
  {
    return (_TyGraphLink*)_TyIterBase::PGLBCur();
  }
  void SetPPGLCur( _TyGraphLink * _pglCur )
  {
    _TyIterBase::SetPGLBCur( (_TyGraphLinkBase*)_pglCur );
  }

  _TyGraphNode * PGNCur() const _BIEN_NOTHROW
  {
    return (_TyGraphNode*)_TyIterBase::PGNBCur();
  }
  void SetPGNCur( _TyGraphNode * _pgnCur )
  {
    _TyIterBase::SetPGNBCur( _pgnCur );
  }

  _TyGraphLink ** FirstParent() const _BIEN_NOTHROW
  {
    return (_TyGraphLink**)_TyBase::FirstParent();
  }
  _TyGraphLink ** FirstChild() const _BIEN_NOTHROW
  {
    return (_TyGraphLink**)_TyBase::FirstChild();
  }
  void  AddParent(  _TyGraphNode * _pgnAdd, _TyGraphLink * _pglAdd,
                    _TyGraphLink ** _rpglInsertBeforeParent,
                    _TyGraphLink ** _rpglInsertBeforeAddChild )
  {
    _TyBase::AddParent( _pgnAdd, _pglAdd, 
                        (_TyGraphLinkBaseBase **)_rpglInsertBeforeParent, 
                        (_TyGraphLinkBaseBase **)_rpglInsertBeforeAddChild );
  }
  void  AddChild(  _TyGraphNode * _pgnAdd, _TyGraphLink * _pglAdd,
                    _TyGraphLink ** _rpglInsertBeforeChild,
                    _TyGraphLink ** _rpglInsertBeforeAddParent )
  {
    _TyBase::AddChild(  _pgnAdd, _pglAdd, 
                        (_TyGraphLinkBaseBase **)_rpglInsertBeforeChild, 
                        (_TyGraphLinkBaseBase **)_rpglInsertBeforeAddParent );
  }
};

// constant specialization - same as above - but masks access to all 
//  base class non-constant methods via redefinition
//  and introduction of compile-time error upon access.

template < class t_TyIterTraits >
class _graph_typed_iterator< t_TyIterTraits, std::true_type >
  : public t_TyIterTraits::_TyIterBase
{
  // All typed iterators are friends:
  template < class t__TyIterTraits, class t__TyFIsConstIterator >
  friend class _graph_typed_iterator;

protected:

  // Extract the various types from the trait object:
  typedef typename t_TyIterTraits::_TyNodeEl    _TyNodeEl;
  typedef typename t_TyIterTraits::_TyNodePtr   _TyNodePtr;
  typedef typename t_TyIterTraits::_TyNodeRef   _TyNodeRef;
  typedef typename t_TyIterTraits::_TyLinkEl    _TyLinkEl;
  typedef typename t_TyIterTraits::_TyLinkPtr   _TyLinkPtr;
  typedef typename t_TyIterTraits::_TyLinkRef   _TyLinkRef;
  typedef typename t_TyIterTraits::_TyValueEl   _TyValueEl;
  typedef typename t_TyIterTraits::_TyValuePtr  _TyValuePtr;
  typedef typename t_TyIterTraits::_TyValueRef  _TyValueRef;

  typedef typename t_TyIterTraits::_TyGraphNode     _TyGraphNode;
  typedef typename t_TyIterTraits::_TyGraphNodeBase _TyGraphNodeBase;
  typedef typename t_TyIterTraits::_TyGraphNodeBaseBase _TyGraphNodeBaseBase;
  typedef typename t_TyIterTraits::_TyGraphLink     _TyGraphLink;
  typedef typename t_TyIterTraits::_TyGraphLinkBase _TyGraphLinkBase;
  typedef typename t_TyIterTraits::_TyGraphLinkBaseBase _TyGraphLinkBaseBase;

  typedef typename t_TyIterTraits::_TyGraphValue          _TyGraphValue;
  typedef typename t_TyIterTraits::_TyGraphValueBase      _TyGraphValueBase;
  typedef typename t_TyIterTraits::_TyGraphValueBaseBase  _TyGraphValueBaseBase;

  typedef typename t_TyIterTraits::_TyPathNodeAllocator         _TyPathNodeAllocator;
  typedef typename t_TyIterTraits::_TyPathNodeAllocatorAsPassed _TyPathNodeAllocatorAsPassed;

  typedef typename t_TyIterTraits::_TyInitArg           _TyInitArg;
  typedef typename t_TyIterTraits::_TyInitArgBase       _TyInitArgBase;
  typedef typename t_TyIterTraits::_TyInitArgConst      _TyInitArgConst;
  typedef typename t_TyIterTraits::_TyInitArgBaseConst  _TyInitArgBaseConst;

  typedef typename t_TyIterTraits::_TyPassIterNonConst  _TyPassIterNonConst;
  typedef typename t_TyIterTraits::_TyPassIterConst     _TyPassIterConst;

private:
  typedef typename t_TyIterTraits::_TyIterBase                  _TyBase;
  typedef _graph_typed_iterator< t_TyIterTraits, std::true_type >  _TyThis;

public:

  typedef typename t_TyIterTraits::_TyIterBase          _TyIterBase;
  typedef t_TyIterTraits                                _TyIterTraits;
  typedef std::true_type                                   _TyFIsConstIterator;

  // Access to const/non-const iterator types:
  typedef _graph_typed_iterator< typename t_TyIterTraits::
              template rebind_const< false >::other >   iterator;
  typedef _graph_typed_iterator< typename t_TyIterTraits::
              template rebind_const< true >::other >    const_iterator;

  typedef _TyNodeEl                                           node_value_type;
  typedef _TyNodePtr                                          node_pointer;
  typedef _TyNodeRef                                          node_reference;
                                                              
  typedef _TyLinkEl                                           link_value_type;
  typedef _TyLinkPtr                                          link_pointer;
  typedef _TyLinkRef                                          link_reference;
                                                              
  // For this iterator the node is the value:                 
  typedef _TyValueEl                                          value_type;
  typedef _TyValuePtr                                         pointer;
  typedef _TyValueRef                                         reference;

  _graph_typed_iterator()
  {
  }

  explicit _graph_typed_iterator( const _graph_typed_iterator & _r )
    : _TyBase( static_cast< const _TyIterBase & >( _r ) )
  {
  }

  explicit _graph_typed_iterator(  _TyPathNodeAllocatorAsPassed const & _allocPathNode )
    : _TyIterBase( _allocPathNode )
  {
  }

  _graph_typed_iterator(  _TyInitArg _piaCur, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode )
    : _TyIterBase( ( _TyInitArgBase )( _piaCur ), _allocPathNode )
  {
  }
  _graph_typed_iterator(  _TyInitArgConst _piaCur, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode )
    : _TyIterBase( ( _TyInitArgBaseConst )( _piaCur ), _allocPathNode )
  {
  }

  // The non-allocator base class version:
  explicit _graph_typed_iterator( _TyInitArg _piaCur )
    : _TyIterBase( ( _TyInitArgBase )( _piaCur ) )
  {
  }
  explicit _graph_typed_iterator( _TyInitArgConst _piaCur )
    : _TyIterBase( ( _TyInitArgBaseConst )( _piaCur ) )
  {
  }

// Construction with an iterator and another allocator:
  template < class t_TyIter >
  _graph_typed_iterator(  t_TyIter const & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ), _allocPathNode )
  {
    __TRANSFER_CONST( typename t_TyIter::_TyFIsConstIterator, _TyFIsConstIterator );
	__ASSERT_SAME_TYPE( _TyGraphNode, typename t_TyIter::_TyGraphNode );
    __ASSERT_SAME_TYPE( _TyGraphLink, typename t_TyIter::_TyGraphLink );
  }

  // Construction that just obtains the allocator - if any - this doesn't check const-correctness
  //  as we are not transferring graph information:
  template < class t_TyIter >
  explicit _graph_typed_iterator( t_TyIter const & _r, std::false_type )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ), std::false_type() )
  {
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
  }

  // Specialize the versions that take a "pass" path - we will take posession from the pass path object.
  _graph_typed_iterator(  _TyPassIterConst & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode ) _BIEN_NOTHROW
    : _TyIterBase( _allocPathNode )
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }
  _graph_typed_iterator(  _TyPassIterNonConst & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode ) _BIEN_NOTHROW
    : _TyIterBase( _allocPathNode )
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

// Construction with just an iterator:
  template < class t_TyIter >
  explicit _graph_typed_iterator( t_TyIter const & _r )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ) )
  {
    __TRANSFER_CONST( typename t_TyIter::_TyFIsConstIterator, _TyFIsConstIterator );
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
  }

  // specialize construction with a "pass" path iterator:
  explicit _graph_typed_iterator( _TyPassIterConst & _r ) _BIEN_NOTHROW
    : _TyIterBase( static_cast< typename _TyPassIterConst::_TyIterBase const & >( _r ), std::false_type() )
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }
  explicit _graph_typed_iterator( _TyPassIterNonConst & _r ) _BIEN_NOTHROW
    : _TyIterBase( static_cast< typename _TyPassIterNonConst::_TyIterBase const & >( _r ), std::false_type() )
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

  // assignment - similar to construction except we have already been populated
  //  with a reference to an allocator.
  template < class t_TyIter >
  _TyThis &
  operator = ( t_TyIter const & _r )
  {
    __TRANSFER_CONST( _TyFIsConstIterator, typename t_TyIter::_TyFIsConstIterator );
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
    // call base class:
    _TyIterBase::operator = ( static_cast< typename t_TyIter::_TyIterBase const & >( _r ) );
    return *this;
  }

  // Initialization from a pass iterator:
  _TyThis &
  operator = ( _TyPassIterConst & _r ) _BIEN_NOTHROW
  {
    // We swap our existence with the pass iterator:
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }
  _TyThis &
  operator = ( _TyPassIterNonConst & _r ) _BIEN_NOTHROW
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

  reference operator* ( ) const _BIEN_NOTHROW
  { 
    return static_cast< const _TyGraphValue* >( _TyIterBase::PBObjCur() )->RElConst(); 
  }
  pointer operator-> ( ) const _BIEN_NOTHROW
  { 
    return &(operator*()); 
  }

  // Accessors to parent and child nodes:
  node_reference    GetParentNode() const _BIEN_NOTHROW
  { 
    return static_cast< _TyGraphNode* >( _TyIterBase::PGNBParent() )->RElConst(); 
  }
  node_reference    GetChildNode() const _BIEN_NOTHROW
  { 
    return static_cast< _TyGraphNode* >( _TyIterBase::PGNBChild() )->RElConst(); 
  }
  const _TyGraphNode * PGNParent() const _BIEN_NOTHROW
  {
    return const_cast< _TyGraphNode const *>( static_cast< _TyGraphNode* >( _TyIterBase::PGNBParent() ) );
  }
  const _TyGraphNode * PGNChild() const _BIEN_NOTHROW
  {
    return const_cast< _TyGraphNode const *>( static_cast< _TyGraphNode* >( _TyIterBase::PGNBChild() ) );
  }

  // Accessor to current link ( if any ):
  link_reference GetCurLink() const _BIEN_NOTHROW
  {
    return static_cast< _TyGraphLink* >( _TyIterBase::PGLBCur() )->RElConst(); 
  }

  // Typed graph object accessors:
  _TyInitArgConst PObjCur() const _BIEN_NOTHROW
  {
    return (_TyInitArgConst)_TyIterBase::PBObjCur();
  }

  const _TyGraphLink * const * PPGLCur() const _BIEN_NOTHROW
  {
    return (const _TyGraphLink* const *)_TyIterBase::PPGLBCur();
  }
  void SetPPGLCur( const _TyGraphLink * const * _ppglCur )
  {
    _TyIterBase::SetPPGLBCur( (_TyGraphLinkBase**)_ppglCur );
  }

  const _TyGraphLink * PGLCur() const _BIEN_NOTHROW
  {
    return (const _TyGraphLink*)_TyIterBase::PGLBCur();
  }
  void SetPGLCur( const _TyGraphLink * _pglCur )
  {
    SetPGLBCur( (_TyGraphLinkBase*)_pglCur );
  }

  const _TyGraphNode * PGNCur() const _BIEN_NOTHROW
  {
    return (const _TyGraphNode*)_TyIterBase::PGNBCur();
  }
  void SetPGNCur( const _TyGraphNode * _pgnCur )
  {
    SetPGNBCur( (_TyGraphNode*)_pgnCur );
  }

// Now mask access to any graph modifying base class methods - via compilation error:
protected:
  void  ExchangeParentsOrdered( const _TyGNIndex &, const _TyGNIndex & );
  void  ExchangeParents( const _TyGNIndex &, const _TyGNIndex & );
  void  MoveParentUp( const _TyGNIndex &, const _TyGNIndex & );
  void  MoveParentDown( const _TyGNIndex &, const _TyGNIndex & );
  void  MoveParent( const _TyGNIndex &, const _TyGNIndex & );
  void  ExchangeChildrenOrdered( const _TyGNIndex &, const _TyGNIndex & );
  void  ExchangeChildren( const _TyGNIndex &, const _TyGNIndex & );
  void  MoveChildUp( const _TyGNIndex &, const _TyGNIndex & );
  void  MoveChildDown( const _TyGNIndex &, const _TyGNIndex & );
  void  MoveChild( const _TyGNIndex &, const _TyGNIndex & );
  void  AddParent(  _TyGraphNode * _pgnAdd, _TyGraphLink * _pglAdd,
                    _TyGraphLink ** _rpglInsertBeforeParent,
                    _TyGraphLink ** _rpglInsertBeforeAddChild );
  void  AddChild(  _TyGraphNode * _pgnAdd, _TyGraphLink * _pglAdd,
                    _TyGraphLink ** _rpglInsertBeforeChild,
                    _TyGraphLink ** _rpglInsertBeforeAddParent );

  template < class _TyLinkIterator >
  void  ExchangeSiblingParents( _TyLinkIterator const & _rSiblingParent ) const;
  template < class _TyLinkIterator >
  void  ExchangeSiblingChildren( _TyLinkIterator const & _rSiblingChild ) const;
};

__DGRAPH_END_NAMESPACE

#endif //__GR_TITR_H
