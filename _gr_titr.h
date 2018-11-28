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
            __STL_TEMPLATE rebind_const< false >::other >   iterator;
  typedef _graph_typed_iterator< typename t_TyIterTraits::
            __STL_TEMPLATE rebind_const< true >::other >    const_iterator;

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
  template < class _TyIter >
  _graph_typed_iterator(  _TyIter const & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode )
    : _TyIterBase( static_cast< typename _TyIter::_TyIterBase const & >( _r ), _allocPathNode )
  {
    __TRANSFER_CONST( typename _TyIter::_TyFIsConstIterator, _TyFIsConstIterator );
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
  }

  // Specialize the versions that take a "pass" path - we will take posession from the pass path object.
  _graph_typed_iterator(  _TyPassIterConst & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode ) __STL_NOTHROW
  : _TyIterBase( _allocPathNode )
  {
    ___semantic_error_object  error;  // we are not const.
  }
  _graph_typed_iterator(  _TyPassIterNonConst & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode ) __STL_NOTHROW
    : _TyIterBase( _allocPathNode )
  {
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
  _graph_typed_iterator( t_TyIter const & _r, __false_type )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ), __false_type() )
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
  explicit _graph_typed_iterator( _TyPassIterConst & _r ) __STL_NOTHROW
    : _TyIterBase( static_cast< typename _TyPassIterConst::_TyIterBase const & >( _r ), 
                   __false_type() )
  {
    ___semantic_error_object  error;  // we are not const.
  }
  explicit _graph_typed_iterator( _TyPassIterNonConst & _r ) __STL_NOTHROW
    : _TyIterBase( static_cast< typename _TyPassIterNonConst::_TyIterBase const & >( _r ), 
                   __false_type() )
  {
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
    SetPPGLBCur( reinterpret_cast< _TyGraphLinkBaseBase ** >( _ppgl ) );
    return *this;
  }

  // Initialization with a graph link identity:
  _TyThis &
  operator = ( _TyGraphLink * _pgl )
  {
    SetPGLBCur( _pgl );
  }

  // Initialization from a pass iterator:
  _TyThis &
  operator = ( _TyPassIterConst & _r ) __STL_NOTHROW
  {
    // we are not const:
    ___semantic_error_object  error;
  }

  _TyThis &
  operator = ( _TyPassIterNonConst & _r ) __STL_NOTHROW
  {
    // We swap our existence with the pass iterator:
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

  reference operator* ( ) const __STL_NOTHROW
  { 
    return static_cast< _TyGraphValue* >( PBObjCur() )->RElNonConst(); 
  }
  pointer operator-> ( ) const __STL_NOTHROW
  { 
    return &(operator*()); 
  }

// Accessors:
// NOTE: Because this is a multi-use templatized wrapper - some of these methods
//  will not compile with each iterator base type - this, of course, is by design.
//  If an compile error results then you are using the method on the wrong iterator type.

  // Current node access:
  node_reference    GetCurNode() const __STL_NOTHROW
  { 
    return static_cast< _TyGraphNode* >( PGNBCur() )->RElNonConst();
  }

  // Accessors to parent and child nodes:
  node_reference    GetParentNode() const __STL_NOTHROW
  {
    return static_cast< _TyGraphNode* >( PGNBParent() )->RElNonConst(); 
  }
  node_reference    GetChildNode() const
  {
    return static_cast< _TyGraphNode* >( PGNBChild() )->RElNonConst(); 
  }
  _TyGraphNode * PGNParent() const __STL_NOTHROW
  {
    return static_cast< _TyGraphNode* >( PGNBParent() );
  }
  _TyGraphNode * PGNChild() const __STL_NOTHROW
  {
    return static_cast< _TyGraphNode* >( PGNBChild() );
  }

  // Accessor to current link:
  link_reference    GetCurLink() const __STL_NOTHROW
  { 
    return static_cast< _TyGraphLink* >( PGNBCur() )->RElNonConst();
  }

  // Typed graph object accessors:
  _TyInitArg PObjCur() const __STL_NOTHROW
  {
    return (_TyInitArg)PBObjCur();
  }

  _TyGraphLink ** PPGLCur() const __STL_NOTHROW
  {
    return (_TyGraphLink**)PPGLBCur();
  }
  void SetPPGLCur( _TyGraphLink ** _ppglCur )
  {
    SetPPGLBCur( (_TyGraphLinkBase**)_ppglCur );
  }

  _TyGraphLink * PGLCur() const __STL_NOTHROW
  {
    return (_TyGraphLink*)PGLBCur();
  }
  void SetPPGLCur( _TyGraphLink * _pglCur )
  {
    SetPGLBCur( (_TyGraphLinkBase*)_pglCur );
  }

  _TyGraphNode * PGNCur() const __STL_NOTHROW
  {
    return (_TyGraphNode*)PGNBCur();
  }
  void SetPGNCur( _TyGraphNode * _pgnCur )
  {
    SetPGNBCur( _pgnCur );
  }

  _TyGraphLink ** FirstParent() const __STL_NOTHROW
  {
    return (_TyGraphLink**)_TyBase::FirstParent();
  }
  _TyGraphLink ** FirstChild() const __STL_NOTHROW
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
class _graph_typed_iterator< t_TyIterTraits, __true_type >
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
  typedef _graph_typed_iterator< t_TyIterTraits, __true_type >  _TyThis;

public:

  typedef typename t_TyIterTraits::_TyIterBase          _TyIterBase;
  typedef t_TyIterTraits                                _TyIterTraits;
  typedef __true_type                                   _TyFIsConstIterator;

  // Access to const/non-const iterator types:
  typedef _graph_typed_iterator< typename t_TyIterTraits::
              __STL_TEMPLATE rebind_const< false >::other >   iterator;
  typedef _graph_typed_iterator< typename t_TyIterTraits::
              __STL_TEMPLATE rebind_const< true >::other >    const_iterator;

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
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
  }

  // Construction that just obtains the allocator - if any - this doesn't check const-correctness
  //  as we are not transferring graph information:
  template < class t_TyIter >
  explicit _graph_typed_iterator( t_TyIter const & _r, __false_type )
    : _TyIterBase( static_cast< typename t_TyIter::_TyIterBase const & >( _r ), __false_type() )
  {
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
  }

  // Specialize the versions that take a "pass" path - we will take posession from the pass path object.
  _graph_typed_iterator(  _TyPassIterConst & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode ) __STL_NOTHROW
    : _TyIterBase( _allocPathNode )
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }
  _graph_typed_iterator(  _TyPassIterNonConst & _r, 
                          _TyPathNodeAllocatorAsPassed const & _allocPathNode ) __STL_NOTHROW
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
  explicit _graph_typed_iterator( _TyPassIterConst & _r ) __STL_NOTHROW
    : _TyIterBase( static_cast< typename _TyPassIterConst::_TyIterBase const & >( _r ), __false_type() )
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }
  explicit _graph_typed_iterator( _TyPassIterNonConst & _r ) __STL_NOTHROW
    : _TyIterBase( static_cast< typename _TyPassIterNonConst::_TyIterBase const & >( _r ), __false_type() )
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

  // assignment - similar to construction except we have already been populated
  //  with a reference to an allocator.
  template < class _TyIter >
  _TyThis &
  operator = ( _TyIter const & _r )
  {
    __TRANSFER_CONST( _TyFIsConstIterator, typename _TyIter::_TyFIsConstIterator );
    __ASSERT_SAME_TYPE3( _TyGraphNode, typename t_TyIter::_TyGraphNode, _TyAssertNode );
    __ASSERT_SAME_TYPE3( _TyGraphLink, typename t_TyIter::_TyGraphLink, _TyAssertLink );
    // call base class:
    _TyIterBase::operator = ( static_cast< typename _TyIter::_TyIterBase const & >( _r ) );
    return *this;
  }

  // Initialization from a pass iterator:
  _TyThis &
  operator = ( _TyPassIterConst & _r ) __STL_NOTHROW
  {
    // We swap our existence with the pass iterator:
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }
  _TyThis &
  operator = ( _TyPassIterNonConst & _r ) __STL_NOTHROW
  {
    _r.swap( static_cast< _TyIterBase & >( *this ) );
  }

  reference operator* ( ) const __STL_NOTHROW
  { 
    return static_cast< const _TyGraphValue* >( PBObjCur() )->RElConst(); 
  }
  pointer operator-> ( ) const __STL_NOTHROW
  { 
    return &(operator*()); 
  }

  // Accessors to parent and child nodes:
  node_reference    GetParentNode() const __STL_NOTHROW
  { 
    return static_cast< _TyGraphNode* >( PGNBParent() )->RElConst(); 
  }
  node_reference    GetChildNode() const __STL_NOTHROW
  { 
    return static_cast< _TyGraphNode* >( PGNBChild() )->RElConst(); 
  }
  const _TyGraphNode * PGNParent() const __STL_NOTHROW
  {
    return const_cast< _TyGraphNode const *>( static_cast< _TyGraphNode* >( PGNBParent() ) );
  }
  const _TyGraphNode * PGNChild() const __STL_NOTHROW
  {
    return const_cast< _TyGraphNode const *>( static_cast< _TyGraphNode* >( PGNBChild() ) );
  }

  // Accessor to current link ( if any ):
  link_reference GetCurLink() const __STL_NOTHROW
  {
    return static_cast< _TyGraphLink* >( PGLBCur() )->RElConst(); 
  }

  // Typed graph object accessors:
  _TyInitArgConst PObjCur() const __STL_NOTHROW
  {
    return (_TyInitArgConst)PBObjCur();
  }

  const _TyGraphLink * const * PPGLCur() const __STL_NOTHROW
  {
    return (const _TyGraphLink* const *)PPGLBCur();
  }
  void SetPPGLCur( const _TyGraphLink * const * _ppglCur )
  {
    _TyIterBase::SetPPGLBCur( (_TyGraphLinkBase**)_ppglCur );
  }

  const _TyGraphLink * PGLCur() const __STL_NOTHROW
  {
    return (const _TyGraphLink*)PGLBCur();
  }
  void SetPGLCur( const _TyGraphLink * _pglCur )
  {
    SetPGLBCur( (_TyGraphLinkBase*)_pglCur );
  }

  const _TyGraphNode * PGNCur() const __STL_NOTHROW
  {
    return (const _TyGraphNode*)PGNBCur();
  }
  void SetPGNCur( const _TyGraphNode * _pgnCur )
  {
    SetPGNBCur( (_TyGraphNode*)_pgnCur );
  }

// Now mask access to any graph modifying base class methods - via compilation error:
  void  ExchangeParentsOrdered( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  ExchangeParents( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  MoveParentUp( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  MoveParentDown( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  MoveParent( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  ExchangeChildrenOrdered( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  ExchangeChildren( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  MoveChildUp( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  MoveChildDown( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  MoveChild( const _TyGNIndex &, const _TyGNIndex & )
  {
    ___semantic_error_object  error;
  }
  void  AddParent(  _TyGraphNode * _pgnAdd, _TyGraphLink * _pglAdd,
                    _TyGraphLink ** _rpglInsertBeforeParent,
                    _TyGraphLink ** _rpglInsertBeforeAddChild )
  {
    ___semantic_error_object  error;
  }
  void  AddChild(  _TyGraphNode * _pgnAdd, _TyGraphLink * _pglAdd,
                    _TyGraphLink ** _rpglInsertBeforeChild,
                    _TyGraphLink ** _rpglInsertBeforeAddParent )
  {
    ___semantic_error_object  error;
  }

  template < class _TyLinkIterator >
  void  ExchangeSiblingParents( _TyLinkIterator const & _rSiblingParent ) const
  {
    ___semantic_error_object  error;
  }
  template < class _TyLinkIterator >
  void  ExchangeSiblingChildren( _TyLinkIterator const & _rSiblingChild ) const
  {
    ___semantic_error_object  error;
  }
};

__DGRAPH_END_NAMESPACE

#endif __GR_TITR_H
