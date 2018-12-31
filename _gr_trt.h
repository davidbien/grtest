#ifndef __GR_TRT_H
#define __GR_TRT_H

// _gr_trt.h

// The traits types map to types implemented in the following header.
// This allows us to easily change ( and even have multiple different instances ) of
//  the graph base implementation ( i.e. some thread-safe, some not ). This also allows
//  ( for instance ) the association of a lookup table for nodes - without the introduction
//  of additional memory objects - the types needed for the lookup table can be included in
//  the graph node type.
// Note: The word "safe" as currently used does not mean thread-safe - in merely means
//  that the reference(s) to the graph object maintained by an iterator are cleared when
//  the graph object's lifetime ends. This is an "okay" scheme ( but still not great ) 
//  in a single threaded enviroment - it prevents iterators attempting to access non-existent 
//  objects ( though this requires a check by the programmer before use of the iterator ). 
// Full thread-safety could ( and will ) be implemented as an alternate set of
//  graph base classes ( as described above ).

#ifndef __DGRAPH_USE_STLPORT
#include "_aloctrt.h"
#endif //!__DGRAPH_USE_STLPORT

#include "_gr_alst.h"
#include "_gr_iter.h"
#include "_gr_type.h"
#include "_gr_def.h"
#include "_gr_safe.h"
#include "_gr_pitr.h"
#include "_gr_sitr.h"
#include "_gr_disn.h"
#include "_gr_gitr.h"
#include "_gr_sril.h"
#include "_gr_dump.h"
#include "_gr_outp.h"
#include "_gr_inpt.h"
#include "_gr_stin.h"
#include "_gr_inpw.h"
#include "_gr_inip.h"

#include "_gr_stio.h"
#ifdef __GR_DEFINEOLEIO
#include "_gr_olio.h"
#endif //__GR_DEFINEOLEIO

// This module implements the default graph traits template.

__DGRAPH_BEGIN_NAMESPACE

// abstract iterator types:
struct _TyGraphNodeIteratorType
{
};

struct _TyGraphLinkPosIteratorType
{
};

struct _TyGraphLinkIdentIteratorType
{
};

struct _TyGraphPathIteratorType
{
};

template <  class t_TyNodeEl, class t_TyLinkEl,
            class t_TyGraphNodeBase, class t_TyGraphLinkBase, 
            class t_TyPathNodeBase >
struct _graph_traits_base
{
  typedef typename t_TyGraphNodeBase::_TyGraphNodeBase  _TyGraphNodeBaseBase;
  typedef typename t_TyGraphLinkBase::_TyGraphLinkBase  _TyGraphLinkBaseBase;
  typedef typename t_TyPathNodeBase::_TyPathNodeBase    _TyPathNodeBaseBase;
  typedef t_TyGraphNodeBase                             _TyGraphNodeBase;
  typedef t_TyGraphLinkBase                             _TyGraphLinkBase;
  typedef t_TyPathNodeBase                              _TyPathNodeBase;

  typedef t_TyNodeEl                                    _TyNodeEl;
  typedef t_TyLinkEl                                    _TyLinkEl;
};

// struct _graph_traits_safety_base:
// This struct has the only concrete references to graph base types 
//  ( _graph_node_base, _graph_link_base, _graph_path_base ).

// non-safe graph traits - can't use safe iterators with a graph created thusly:
template <  class t_TyNodeEl, class t_TyLinkEl,
            class t_TyAllocatorGraphNode, class t_TyAllocatorGraphLink,
            class t_TyAllocatorPathNodeBase, class t_TyAllocatorPathNodeSafe, 
            bool t_fIsSafeGraph,
            class t_TyGraphNode, class t_TyGraphLink, class t_TyPathNode >
struct _graph_traits_safety_base
  : public _graph_traits_base<  t_TyNodeEl, t_TyLinkEl, 
                                typename t_TyGraphNode::_TyGraphNodeBase,
                                typename t_TyGraphLink::_TyGraphLinkBase,
                                typename t_TyPathNode::_TyPathNodeBase >
{
private:
  typedef _graph_traits_base<  t_TyNodeEl, t_TyLinkEl,
    typename t_TyGraphNode::_TyGraphNodeBase,
	  typename t_TyGraphLink::_TyGraphLinkBase,
	  typename t_TyPathNode::_TyPathNodeBase > _TyBase;
  typedef _graph_traits_safety_base<  t_TyNodeEl, t_TyLinkEl,
                                      t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                      t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe, 
                                      t_fIsSafeGraph,
                                      t_TyGraphNode, t_TyGraphLink, t_TyPathNode >      _TyThis;
public:

  static const bool ms_fIsSafeGraph = t_fIsSafeGraph;

  typedef typename _TyBase::_TyPathNodeBase _TyPathNodeBase;
  typedef _graph_base< _TyPathNodeBase, t_TyAllocatorPathNodeBase >   _TyGraphBase;
  typedef _graph_base< _TyPathNodeBase, t_TyAllocatorPathNodeBase >   _TyGraphBaseBase;
  typedef t_TyGraphNode                                               _TyGraphNode;
  typedef t_TyGraphLink                                               _TyGraphLink;
  typedef t_TyPathNode                                                _TyPathNode;

  // Define the various allocator types:
  typedef t_TyAllocatorGraphNode    _TyGraphNodeAllocatorAsPassed;
  typedef t_TyAllocatorGraphLink    _TyGraphLinkAllocatorAsPassed;
  typedef t_TyAllocatorPathNodeBase _TyPathNodeBaseAllocatorAsPassed;

  // The resultant allocators - typed to the object they are allocating:
  typedef typename _Alloc_traits< _TyGraphNode, t_TyAllocatorGraphNode >::allocator_type          _TyGraphNodeAllocator;
  typedef typename _Alloc_traits< _TyGraphLink, t_TyAllocatorGraphLink >::allocator_type          _TyGraphLinkAllocator;
  typedef typename _Alloc_traits< _TyPathNode, t_TyAllocatorPathNodeBase >::allocator_type        _TyPathNodeBaseAllocator;
  // Don't define types for path node safe allocator - only present in the safe version.

  typedef _allocator_set_notsafe< t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                  t_TyAllocatorPathNodeBase >                           _TyAllocatorSet;
};

// safe graph-traits - allows use of either safe or non-safe iterators:
template <  class t_TyNodeEl, class t_TyLinkEl,
            class t_TyAllocatorGraphNode, class t_TyAllocatorGraphLink,
            class t_TyAllocatorPathNodeBase, class t_TyAllocatorPathNodeSafe,
            class t_TyGraphNode, class t_TyGraphLink, class t_TyPathNode >
struct _graph_traits_safety_base< t_TyNodeEl, t_TyLinkEl,
                                  t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                  t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe, true,
                                  t_TyGraphNode, t_TyGraphLink, t_TyPathNode >
  : public _graph_traits_base<  t_TyNodeEl, t_TyLinkEl, 
                                typename t_TyGraphNode::_TyGraphNodeSafe,
                                typename t_TyGraphLink::_TyGraphLinkSafe,
                                typename t_TyPathNode::_TyPathNodeSafe >
{
private:
  typedef _graph_traits_base<  t_TyNodeEl, t_TyLinkEl,
    typename t_TyGraphNode::_TyGraphNodeSafe,
    typename t_TyGraphLink::_TyGraphLinkSafe,
    typename t_TyPathNode::_TyPathNodeSafe > _TyBase;
  typedef _graph_traits_safety_base<  t_TyNodeEl, t_TyLinkEl,
                                      t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                      t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe, 
                                      true, t_TyGraphNode, t_TyGraphLink, t_TyPathNode >  _TyThis;
public:
  static const bool ms_fIsSafeGraph = true;

  typedef typename _TyBase::_TyPathNodeBase _TyPathNodeBase;
  typedef typename _TyBase::_TyPathNodeBaseBase _TyPathNodeBaseBase;
  typedef _graph_safe_base< _TyPathNodeBase, t_TyAllocatorPathNodeBase,
                            t_TyAllocatorPathNodeSafe >                   _TyGraphBase;
  typedef _graph_base< _TyPathNodeBase, t_TyAllocatorPathNodeBase >       _TyGraphBaseBase;
  typedef t_TyGraphNode                                                   _TyGraphNode;
  typedef t_TyGraphLink                                                   _TyGraphLink;
  typedef t_TyPathNode                                                    _TyPathNode;

  // Define the various allocator types:
  // The allocators that the template supllied:
  typedef t_TyAllocatorGraphNode      _TyGraphNodeAllocatorAsPassed;
  typedef t_TyAllocatorGraphLink      _TyGraphLinkAllocatorAsPassed;
  typedef t_TyAllocatorPathNodeBase   _TyPathNodeBaseAllocatorAsPassed;
  typedef t_TyAllocatorPathNodeSafe   _TyPathNodeSafeAllocatorAsPassed;
  
  // The allocators that will be used to allocate objects:
  typedef typename _Alloc_traits< _TyGraphNode, t_TyAllocatorGraphNode >::allocator_type            _TyGraphNodeAllocator;
  typedef typename _Alloc_traits< _TyGraphLink, t_TyAllocatorGraphLink >::allocator_type            _TyGraphLinkAllocator;
  typedef typename _Alloc_traits< _TyPathNodeBaseBase, t_TyAllocatorPathNodeBase >::allocator_type  _TyPathNodeBaseAllocator;
  typedef typename _Alloc_traits< _TyPathNode, t_TyAllocatorPathNodeSafe >::allocator_type          _TyPathNodeSafeAllocator;

  typedef _allocator_set_safe<  t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe >    _TyAllocatorSet;
};

// iterator traits:

// Base class indexing for non-safe iterators:
template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed,
  class t_TyIterType >
struct _get_notsafe_iter_base_class;

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed >
struct _get_notsafe_iter_base_class< t_TyGraphNodeBase, t_TyGraphLinkBase, 
  t_TyPathNodeBase, t_TyPathNodeBaseAllocatorAsPassed, _TyGraphNodeIteratorType >
{
  typedef _graph_node_iterator_base_notsafe< t_TyGraphNodeBase, t_TyGraphLinkBase >       _TyBaseClass;
};

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed >
struct _get_notsafe_iter_base_class< t_TyGraphNodeBase, t_TyGraphLinkBase, 
  t_TyPathNodeBase, t_TyPathNodeBaseAllocatorAsPassed, _TyGraphLinkPosIteratorType >
{
  typedef _graph_link_pos_iterator_base_notsafe< t_TyGraphNodeBase, t_TyGraphLinkBase >   _TyBaseClass;
};

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed >
struct _get_notsafe_iter_base_class< t_TyGraphNodeBase, t_TyGraphLinkBase, 
  t_TyPathNodeBase, t_TyPathNodeBaseAllocatorAsPassed,  _TyGraphLinkIdentIteratorType >
{
    typedef _graph_link_ident_iterator_base_notsafe< t_TyGraphNodeBase, t_TyGraphLinkBase > _TyBaseClass;
};

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed >
struct _get_notsafe_iter_base_class< t_TyGraphNodeBase, t_TyGraphLinkBase, 
  t_TyPathNodeBase, t_TyPathNodeBaseAllocatorAsPassed, _TyGraphPathIteratorType >
{
  typedef _graph_path_iterator_base_notsafe< t_TyPathNodeBase, 
    t_TyPathNodeBaseAllocatorAsPassed > _TyBaseClass;
};

// non-safe:
template < class t_TyGraphTraits, bool t_fIsSafe >
struct _graph_iterator_traits_safety_base
{
  static const bool ms_fIsSafeIterator = t_fIsSafe;
  typedef typename __boolean_type< ms_fIsSafeIterator >::_type  _TyFIsSafeIterator;

  typedef t_TyGraphTraits                                     _TyGraphTraits;

  typedef typename t_TyGraphTraits::_TyGraphNodeBaseBase      _TyGraphNodeBaseBase;
  typedef typename t_TyGraphTraits::_TyGraphLinkBaseBase      _TyGraphLinkBaseBase;
  typedef typename t_TyGraphTraits::_TyPathNodeBaseBase       _TyPathNodeBaseBase;
  typedef typename t_TyGraphTraits::_TyGraphNodeBaseBase      _TyGraphNodeBase; // Choose the base - even if on a safe graph.
  typedef typename t_TyGraphTraits::_TyGraphLinkBaseBase      _TyGraphLinkBase; // ditto.
  typedef typename t_TyGraphTraits::_TyPathNodeBaseBase       _TyPathNodeBase;
  typedef typename t_TyGraphTraits::_TyGraphNode              _TyGraphNode;
  typedef typename t_TyGraphTraits::_TyGraphLink              _TyGraphLink;

  typedef typename t_TyGraphTraits::_TyPathNodeBaseAllocatorAsPassed  _TyPathNodeBaseAllocatorAsPassed;
  typedef typename t_TyGraphTraits::_TyPathNodeBaseAllocator          _TyPathNodeBaseAllocator;
  typedef _TyPathNodeBaseAllocator                                    _TyPathNodeAllocator;
  typedef _TyPathNodeBaseAllocatorAsPassed                            _TyPathNodeAllocatorAsPassed;

  // Templates to generate types:
  template < class t_TyIteratorType > struct get_iter_base_class
  {
    typedef typename _get_notsafe_iter_base_class< _TyGraphNodeBase, _TyGraphLinkBase,
      _TyPathNodeBase, _TyPathNodeBaseAllocatorAsPassed, t_TyIteratorType >::_TyBaseClass _TyBaseClass;
  };
};

// Base class indexing for safe iterators:
template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed,
  class t_TyIterType >
struct _get_safe_iter_base_class;

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed >
struct _get_safe_iter_base_class< t_TyGraphNodeBase, t_TyGraphLinkBase, 
  t_TyPathNodeBase, t_TyPathNodeBaseAllocatorAsPassed, _TyGraphNodeIteratorType >
{
  typedef _graph_node_iterator_base_safe< t_TyGraphNodeBase, t_TyGraphLinkBase >        _TyBaseClass;
};

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed >
struct _get_safe_iter_base_class< t_TyGraphNodeBase, t_TyGraphLinkBase, 
  t_TyPathNodeBase, t_TyPathNodeBaseAllocatorAsPassed, _TyGraphLinkPosIteratorType >
{
  typedef _graph_link_pos_iterator_base_safe< t_TyGraphNodeBase, t_TyGraphLinkBase >    _TyBaseClass;
};

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed >
struct _get_safe_iter_base_class< t_TyGraphNodeBase, t_TyGraphLinkBase, 
  t_TyPathNodeBase, t_TyPathNodeBaseAllocatorAsPassed,  _TyGraphLinkIdentIteratorType >
{
    typedef _graph_link_ident_iterator_base_safe< t_TyGraphNodeBase, t_TyGraphLinkBase >  _TyBaseClass;
};

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase,
  class t_TyPathNodeBase, class t_TyPathNodeBaseAllocatorAsPassed >
struct _get_safe_iter_base_class< t_TyGraphNodeBase, t_TyGraphLinkBase, 
  t_TyPathNodeBase, t_TyPathNodeBaseAllocatorAsPassed, _TyGraphPathIteratorType >
{
  typedef _graph_path_iterator_base_safe< t_TyPathNodeBase, 
    t_TyPathNodeBaseAllocatorAsPassed > _TyBaseClass;
};

// safe:
template < class t_TyGraphTraits >
struct _graph_iterator_traits_safety_base< t_TyGraphTraits, true >
{
  static const bool ms_fIsSafeIterator = true;
  typedef typename __boolean_type< true >::_type              _TyFIsSafeIterator;

  typedef t_TyGraphTraits                                     _TyGraphTraits;

  typedef typename t_TyGraphTraits::_TyGraphNodeBaseBase      _TyGraphNodeBaseBase;
  typedef typename t_TyGraphTraits::_TyGraphLinkBaseBase      _TyGraphLinkBaseBase;
  typedef typename t_TyGraphTraits::_TyPathNodeBaseBase       _TyPathNodeBaseBase;
  typedef typename t_TyGraphTraits::_TyGraphNodeBase          _TyGraphNodeBase; // Choose the safe base.
  typedef typename t_TyGraphTraits::_TyGraphLinkBase          _TyGraphLinkBase; // ditto.
  typedef typename t_TyGraphTraits::_TyPathNodeBase           _TyPathNodeBase;
  typedef typename t_TyGraphTraits::_TyGraphNode              _TyGraphNode;
  typedef typename t_TyGraphTraits::_TyGraphLink              _TyGraphLink;

  typedef typename t_TyGraphTraits::_TyPathNodeBaseAllocatorAsPassed  _TyPathNodeBaseAllocatorAsPassed;
  typedef typename t_TyGraphTraits::_TyPathNodeBaseAllocator          _TyPathNodeBaseAllocator;
  typedef typename t_TyGraphTraits::_TyPathNodeSafeAllocatorAsPassed  _TyPathNodeSafeAllocatorAsPassed;
  typedef typename t_TyGraphTraits::_TyPathNodeSafeAllocator          _TyPathNodeSafeAllocator;
  typedef _TyPathNodeSafeAllocator                                    _TyPathNodeAllocator;
  typedef _TyPathNodeSafeAllocatorAsPassed                            _TyPathNodeAllocatorAsPassed;

  // Templates to generate types:
  template < class t_TyIteratorType > struct get_iter_base_class
  {
    typedef typename _get_safe_iter_base_class< _TyGraphNodeBase, _TyGraphLinkBase,
      _TyPathNodeBase, _TyPathNodeSafeAllocatorAsPassed, t_TyIteratorType >::_TyBaseClass _TyBaseClass;
  };
};

// constant versus non-constant specialization:
template < class t_TyGraphTraits, bool t_fIsConstIterator >
struct _graph_iterator_traits_const_base
{
  static const bool ms_fIsConstIterator = t_fIsConstIterator;
  typedef typename __boolean_type< t_fIsConstIterator >::_type  _TyFIsConstIterator;
  typedef typename t_TyGraphTraits::_TyNodeEl _TyNodeEl;
  typedef _TyNodeEl *                         _TyNodePtr;
  typedef _TyNodeEl &                         _TyNodeRef;
  typedef typename t_TyGraphTraits::_TyLinkEl _TyLinkEl;
  typedef _TyLinkEl *                         _TyLinkPtr;
  typedef _TyLinkEl &                         _TyLinkRef;
};

// constant specialization:
template < class t_TyGraphTraits >
struct _graph_iterator_traits_const_base< t_TyGraphTraits, true >
{
  static const bool ms_fIsConstIterator = true;
  typedef typename __boolean_type< true >::_type  _TyFIsConstIterator;
  typedef typename t_TyGraphTraits::_TyNodeEl _TyNodeEl;
  typedef const _TyNodeEl *                   _TyNodePtr;
  typedef const _TyNodeEl &                   _TyNodeRef;
  typedef typename t_TyGraphTraits::_TyLinkEl _TyLinkEl;
  typedef const _TyLinkEl *                   _TyLinkPtr;
  typedef const _TyLinkEl &                   _TyLinkRef;
};

// graph iterator traits:
// Only declare specializations for this.
template <  class t_TyGraphTraits, class t_TyIteratorType,
            bool t_fIsConstIterator, bool t_fIsSafeIterator >
struct _graph_iterator_traits;

// For graph node iterator:
template < class t_TyGraphTraits, bool t_fIsConstIterator, bool t_fIsSafeIterator >
struct _graph_iterator_traits<  t_TyGraphTraits, _TyGraphNodeIteratorType,
                                t_fIsConstIterator, t_fIsSafeIterator >
: public _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >,
  public _graph_iterator_traits_const_base< t_TyGraphTraits, t_fIsConstIterator >
{
private:
typedef _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator > _TySafetyBase;
typedef _graph_iterator_traits_const_base< t_TyGraphTraits, t_fIsConstIterator > _TyConstBase;
public:

  typedef typename _TyConstBase::_TyNodeEl   _TyValueEl;
  typedef typename _TyConstBase::_TyNodePtr  _TyValuePtr;
  typedef typename _TyConstBase::_TyNodeRef  _TyValueRef;

  typedef typename _TySafetyBase::_TyGraphNodeBaseBase  _TyGraphValueBaseBase;
  typedef typename _TySafetyBase::_TyGraphNodeBase      _TyGraphValueBase;
  typedef typename _TySafetyBase::_TyGraphNode          _TyGraphValue;
  typedef typename _TySafetyBase::_TyGraphNodeBase      _TyGraphNodeBase;
  typedef typename _TySafetyBase::_TyGraphNode          _TyGraphNode;

  // Initialization argument:
  typedef _TyGraphNode *        _TyInitArg;
  typedef _TyGraphNodeBase *    _TyInitArgBase;
  typedef const _TyGraphNode *  _TyInitArgConst;
  typedef _TyGraphNodeBase *    _TyInitArgBaseConst;

  // Iterator class:
#if 1
  typedef typename 
    _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >::template get_iter_base_class<_TyGraphNodeIteratorType>::_TyBaseClass    _TyIterBase;
#endif //0

#if 1
  typedef _graph_iterator_pass2< typename _TyConstBase::_TyFIsConstIterator, typename 
    _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >::template get_iter_base_class<_TyGraphNodeIteratorType>::_TyBaseClass > _TyPassIter;
  typedef _graph_iterator_pass2< __false_type, typename 
    _TySafetyBase::template get_iter_base_class<_TyGraphNodeIteratorType>::_TyBaseClass > _TyPassIterNonConst;
  typedef _graph_iterator_pass2< __true_type, typename 
    _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >::template get_iter_base_class<_TyGraphNodeIteratorType>::_TyBaseClass > _TyPassIterConst;
#else //1
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TySafetyBase::_TyGraphLinkBase, 
    _TyConstBase::_TyFIsConstIterator, _TyIterBase >                _TyPassIter;
  // Const and non-const types - keeps implementation independent:
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __false_type, _TyIterBase >                       _TyPassIterNonConst;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __true_type, _TyIterBase >                        _TyPassIterConst;
#endif //0

  // rebindability:
  template < class t__TyIteratorType > 
  struct rebind_type
  {
    typedef _graph_iterator_traits< t_TyGraphTraits, t__TyIteratorType, 
                                    t_fIsConstIterator, t_fIsSafeIterator > other;
  };
  template < bool t__fIsConstIterator > 
  struct rebind_const
  {
    typedef _graph_iterator_traits< t_TyGraphTraits, _TyGraphLinkPosIteratorType, 
                                    t__fIsConstIterator, t_fIsSafeIterator > other;
  };
  template < bool t__fIsSafeIterator > 
  struct rebind_safe
  {
    static const bool fAssert = t_TyGraphTraits::ms_fIsSafeGraph || !t__fIsSafeIterator;
    __ASSERT_BOOL( fAssert )
    typedef _graph_iterator_traits< t_TyGraphTraits, _TyGraphLinkPosIteratorType, 
                                    t_fIsConstIterator, t__fIsSafeIterator > other;
  };
  template < class t__TyIteratorType, bool t__fIsConstIterator, bool t__fIsSafeIterator > 
  struct rebind_all
  {
    __ASSERT_BOOL( t_TyGraphTraits::ms_fIsSafeGraph || !t__fIsSafeIterator )
    typedef _graph_iterator_traits< t_TyGraphTraits, t__TyIteratorType, 
                                    t__fIsConstIterator, t__fIsSafeIterator > other;
  };
};

#if 0
template <  class t_TyGraphTraits,
            bool t_fIsConstIterator, bool t_fIsSafeIterator >
  template < class t_TyIteratorType, bool t__fIsConstIterator, bool t__fIsSafeIterator > 
_graph_iterator_traits< t_TyGraphTraits, _TyGraphNodeIteratorType, t_fIsConstIterator, t_fIsSafeIterator >::
  rebind_all< t_TyIteratorType, t__fIsConstIterator, t__fIsSafeIterator >::m_assert;
#endif //0

// graph link position iterator:
template < class t_TyGraphTraits, bool t_fIsConstIterator, bool t_fIsSafeIterator >
struct _graph_iterator_traits<  t_TyGraphTraits, _TyGraphLinkPosIteratorType, 
                                t_fIsConstIterator, t_fIsSafeIterator >
  : public _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >,
    public _graph_iterator_traits_const_base< t_TyGraphTraits, t_fIsConstIterator >
{
private:
	typedef _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator > _TySafetyBase;
	typedef _graph_iterator_traits_const_base< t_TyGraphTraits, t_fIsConstIterator > _TyConstBase;
public:
  typedef typename _TyConstBase::_TyLinkEl   _TyValueEl;
  typedef typename _TyConstBase::_TyLinkPtr  _TyValuePtr;
  typedef typename _TyConstBase::_TyLinkRef  _TyValueRef;
  typedef typename _TyConstBase::_TyFIsConstIterator _TyFIsConstIterator;

  typedef typename _TySafetyBase::_TyGraphLinkBaseBase  _TyGraphValueBaseBase;
  typedef typename _TySafetyBase::_TyGraphLinkBase      _TyGraphLinkBase;
  typedef typename _TySafetyBase::_TyGraphLink          _TyGraphLink;
	typedef typename _TySafetyBase::_TyGraphLinkBase      _TyGraphValueBase;
	typedef typename _TySafetyBase::_TyGraphLink          _TyGraphValue;
	typedef typename _TySafetyBase::_TyGraphNodeBase			_TyGraphNodeBase;

  // Initialization argument:
  typedef _TyGraphLink **     _TyInitArg;
  typedef _TyGraphLinkBase ** _TyInitArgBase;
  typedef const _TyGraphLink * const *      _TyInitArgConst;
  typedef _TyGraphLinkBase ** _TyInitArgBaseConst;

  // Base classes:
  typedef typename 
    _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >::template get_iter_base_class<_TyGraphLinkPosIteratorType>::_TyBaseClass   _TyIterBase;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                _TyFIsConstIterator, _TyIterBase >                  _TyPassIter;
  // Const and non-const types - keeps implementation independent:
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __false_type, _TyIterBase >                         _TyPassIterNonConst;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __true_type, _TyIterBase >                          _TyPassIterConst;

  // rebindability:
  template < class t__TyIteratorType > struct rebind_type
  {
    typedef _graph_iterator_traits< t_TyGraphTraits, t__TyIteratorType, 
                                    t_fIsConstIterator, t_fIsSafeIterator > other;
  };
  template < bool t__fIsConstIterator > struct rebind_const
  {
    typedef _graph_iterator_traits< t_TyGraphTraits, _TyGraphLinkPosIteratorType, 
                                    t__fIsConstIterator, t_fIsSafeIterator > other;
  };
  template < bool t__fIsSafeIterator > struct rebind_safe
  {
    __ASSERT_BOOL( t_TyGraphTraits::ms_fIsSafeGraph || !t__fIsSafeIterator )
    typedef _graph_iterator_traits< t_TyGraphTraits, _TyGraphLinkPosIteratorType, 
                                    t_fIsConstIterator, t__fIsSafeIterator > other;
  };
  template < class t__TyIteratorType, bool t__fIsConstIterator, bool t__fIsSafeIterator > struct rebind_all
  {
    __ASSERT_BOOL( t_TyGraphTraits::ms_fIsSafeGraph || !t__fIsSafeIterator )
    typedef _graph_iterator_traits< t_TyGraphTraits, t__TyIteratorType, 
                                    t__fIsConstIterator, t__fIsSafeIterator > other;
  };
};

// graph link identity iterator:
template < class t_TyGraphTraits, bool t_fIsConstIterator, bool t_fIsSafeIterator >
struct _graph_iterator_traits<  t_TyGraphTraits, _TyGraphLinkIdentIteratorType, 
                                t_fIsConstIterator, t_fIsSafeIterator >
  : public _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >,
    public _graph_iterator_traits_const_base< t_TyGraphTraits, t_fIsConstIterator >
{
private:
	typedef _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator > _TySafetyBase;
	typedef _graph_iterator_traits_const_base< t_TyGraphTraits, t_fIsConstIterator > _TyConstBase;
public:
	typedef typename _TyConstBase::_TyLinkEl   _TyValueEl;
	typedef typename _TyConstBase::_TyLinkPtr  _TyValuePtr;
	typedef typename _TyConstBase::_TyLinkRef  _TyValueRef;
	typedef typename _TyConstBase::_TyFIsConstIterator _TyFIsConstIterator;

	typedef typename _TySafetyBase::_TyGraphLinkBaseBase  _TyGraphValueBaseBase;
	typedef typename _TySafetyBase::_TyGraphLinkBase      _TyGraphLinkBase;
	typedef typename _TySafetyBase::_TyGraphLink          _TyGraphLink;
	typedef typename _TySafetyBase::_TyGraphLinkBase      _TyGraphValueBase;
	typedef typename _TySafetyBase::_TyGraphLink          _TyGraphValue;
	typedef typename _TySafetyBase::_TyGraphNodeBase			_TyGraphNodeBase;

  // Initialization argument:
  typedef _TyGraphLink *      _TyInitArg;
  typedef _TyGraphLinkBase *  _TyInitArgBase;
  typedef const _TyGraphLink *      _TyInitArgConst;
  typedef _TyGraphLinkBase *  _TyInitArgBaseConst;

  // Base classes:
  typedef typename 
    _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >::template get_iter_base_class<_TyGraphLinkIdentIteratorType>::_TyBaseClass _TyIterBase;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                _TyFIsConstIterator, _TyIterBase >                  _TyPassIter;
  // Const and non-const types - keeps implementation independent:
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __false_type, _TyIterBase >                         _TyPassIterNonConst;
  typedef _graph_iterator_pass< _TyGraphNodeBase, _TyGraphLinkBase, 
                                __true_type, _TyIterBase >                          _TyPassIterConst;

  // re-bindability:
  template < class t__TyIteratorType > struct rebind_type
  {
    typedef _graph_iterator_traits< t_TyGraphTraits, t__TyIteratorType,
                                    t_fIsConstIterator, t_fIsSafeIterator > other;
  };
  template < bool t__fIsConstIterator > struct rebind_const
  {
    typedef _graph_iterator_traits< t_TyGraphTraits, _TyGraphLinkIdentIteratorType,
                                    t__fIsConstIterator, t_fIsSafeIterator > other;
  };
  template < bool t__fIsSafeIterator > struct rebind_safe
  {
    __ASSERT_BOOL( t_TyGraphTraits::ms_fIsSafeGraph || !t__fIsSafeIterator )
    typedef _graph_iterator_traits< t_TyGraphTraits, _TyGraphLinkIdentIteratorType, 
                                    t_fIsConstIterator, t__fIsSafeIterator > other;
  };
  template < class t__TyIteratorType, bool t__fIsConstIterator, bool t__fIsSafeIterator > struct rebind_all
  {
    __ASSERT_BOOL( t_TyGraphTraits::ms_fIsSafeGraph || !t__fIsSafeIterator )
    typedef _graph_iterator_traits< t_TyGraphTraits, t__TyIteratorType, 
                                    t__fIsConstIterator, t__fIsSafeIterator > other;
  };
};

// graph path iterator:
template < class t_TyGraphTraits, bool t_fIsConstIterator, bool t_fIsSafeIterator >
struct _graph_iterator_traits<  t_TyGraphTraits, _TyGraphPathIteratorType, t_fIsConstIterator, t_fIsSafeIterator >
  : public _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >,
    public _graph_iterator_traits_const_base< t_TyGraphTraits, t_fIsConstIterator >
{
private:
typedef _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator > _TySafetyBase;
typedef _graph_iterator_traits_const_base< t_TyGraphTraits, t_fIsConstIterator > _TyConstBase;
public:
	typedef typename _TyConstBase::_TyNodeEl   _TyValueEl;
	typedef typename _TyConstBase::_TyNodePtr  _TyValuePtr;
	typedef typename _TyConstBase::_TyNodeRef  _TyValueRef;
	typedef typename _TyConstBase::_TyFIsConstIterator _TyFIsConstIterator;

	typedef typename _TySafetyBase::_TyPathNodeBase				_TyPathNodeBase;
	typedef typename _TySafetyBase::_TyPathNodeAllocatorAsPassed	_TyPathNodeAllocatorAsPassed;
	typedef typename _TySafetyBase::_TyGraphLinkBaseBase  _TyGraphValueBaseBase;
	typedef typename _TySafetyBase::_TyGraphLinkBase      _TyGraphLinkBase;
	typedef typename _TySafetyBase::_TyGraphLink          _TyGraphLink;
	typedef typename _TySafetyBase::_TyGraphLinkBase      _TyGraphValueBase;
	typedef typename _TySafetyBase::_TyGraphLink          _TyGraphValue;
	typedef typename _TySafetyBase::_TyGraphNodeBase			_TyGraphNodeBase;
	typedef typename _TySafetyBase::_TyGraphNode					_TyGraphNode;

	typedef _TyGraphNode *      _TyInitArg;
	typedef _TyGraphNodeBase *  _TyInitArgBase;
	typedef const _TyGraphNode *      _TyInitArgConst;
	typedef _TyGraphNodeBase *  _TyInitArgBaseConst;

  // Base classes:
  typedef typename 
    _graph_iterator_traits_safety_base< t_TyGraphTraits, t_fIsSafeIterator >::template get_iter_base_class<_TyGraphPathIteratorType>::_TyBaseClass    _TyIterBase;
  typedef _graph_path_iterator_pass<  _TyPathNodeBase, _TyPathNodeAllocatorAsPassed,
                                      _TyFIsConstIterator, _TyIterBase >          _TyPassIter;
  // Const and non-const types - keeps implementation independent:
  typedef _graph_path_iterator_pass<  _TyPathNodeBase, _TyPathNodeAllocatorAsPassed, 
                                      __false_type, _TyIterBase >                 _TyPassIterNonConst;
  typedef _graph_path_iterator_pass<  _TyPathNodeBase, _TyPathNodeAllocatorAsPassed, 
                                      __true_type, _TyIterBase >                  _TyPassIterConst;

  // re-bindability:
  template < class t__TyIteratorType > struct rebind_type
  {
    typedef _graph_iterator_traits< t_TyGraphTraits, t__TyIteratorType, 
                                    t_fIsConstIterator, t_fIsSafeIterator > other;
  };
  template < bool t__fIsConstIterator > struct rebind_const
  {
    typedef _graph_iterator_traits< t_TyGraphTraits, _TyGraphPathIteratorType, 
                                    t__fIsConstIterator, t_fIsSafeIterator > other;
  };
  template < bool t__fIsSafeIterator > struct rebind_safe
  {
    __ASSERT_BOOL( t_TyGraphTraits::ms_fIsSafeGraph || !t__fIsSafeIterator )
    typedef _graph_iterator_traits< t_TyGraphTraits, _TyGraphPathIteratorType,
                                    t_fIsConstIterator, t__fIsSafeIterator > other;
  };
  template < class t__TyIteratorType, bool t__fIsConstIterator, bool t__fIsSafeIterator > class rebind_all
  {
    // Ensure that we don't attempt to get a safe iterator type from a non-safe graph:
    __ASSERT_BOOL( t_TyGraphTraits::ms_fIsSafeGraph || !t__fIsSafeIterator )
    typedef _graph_iterator_traits< t_TyGraphTraits, t__TyIteratorType,
                                    t__fIsConstIterator, t__fIsSafeIterator > other;
  };
};

// Now produce another base graph traits object that declares iterator types:
// non-safe:
template <  class t_TyNodeEl, class t_TyLinkEl, 
            bool t_fIsSafeGraph,
            class t_TyAllocatorGraphNode,
            class t_TyAllocatorGraphLink,
            class t_TyAllocatorPathNodeBase, 
            class t_TyAllocatorPathNodeSafe,
            class t_TyGraphNode, class t_TyGraphLink, class t_TyPathNode >
struct _graph_traits_decl_iter_traits
  : public _graph_traits_safety_base< t_TyNodeEl, t_TyLinkEl,
                                      t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                      t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe, 
                                      t_fIsSafeGraph, 
                                      t_TyGraphNode, t_TyGraphLink, t_TyPathNode >
{
  typedef _graph_traits_safety_base<  t_TyNodeEl, t_TyLinkEl,
                                      t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                      t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe, 
                                      t_fIsSafeGraph,
                                      t_TyGraphNode, t_TyGraphLink, t_TyPathNode > _TyGraphTraitsBase;


  // The various iterator types - only non-safe iterator types in this graph:
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphNodeIteratorType, true, false >       _TyNodeIteratorConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphNodeIteratorType, false, false >      _TyNodeIteratorNonConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkPosIteratorType, true, false >    _TyLinkPosIteratorConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkPosIteratorType, false, false >   _TyLinkPosIteratorNonConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkIdentIteratorType, true, false >  _TyLinkIdentIteratorConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkIdentIteratorType, false, false > _TyLinkIdentIteratorNonConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphPathIteratorType, true, false >       _TyPathIteratorConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphPathIteratorType, false, false >      _TyPathIteratorNonConstTraits;

  // safe iterator types - we provide the types - but they are NOT safe iterators - this allows
  //  a vanilla wrapper to define types:
  typedef _TyNodeIteratorConstTraits            _TyNodeIteratorConstSafeTraits;
  typedef _TyNodeIteratorNonConstTraits         _TyNodeIteratorNonConstSafeTraits;
  typedef _TyLinkPosIteratorConstTraits         _TyLinkPosIteratorConstSafeTraits;
  typedef _TyLinkPosIteratorNonConstTraits      _TyLinkPosIteratorNonConstSafeTraits;
  typedef _TyLinkIdentIteratorConstTraits       _TyLinkIdentIteratorConstSafeTraits;
  typedef _TyLinkIdentIteratorNonConstTraits    _TyLinkIdentIteratorNonConstSafeTraits;
  typedef _TyPathIteratorConstTraits            _TyPathIteratorConstSafeTraits;
  typedef _TyPathIteratorNonConstTraits         _TyPathIteratorNonConstSafeTraits;
};

// safe graph:
template <  class t_TyNodeEl, class t_TyLinkEl, 
            class t_TyAllocatorGraphNode, 
            class t_TyAllocatorGraphLink,
            class t_TyAllocatorPathNodeBase, 
            class t_TyAllocatorPathNodeSafe,
            class t_TyGraphNode, class t_TyGraphLink, class t_TyPathNode >
struct _graph_traits_decl_iter_traits<  t_TyNodeEl, t_TyLinkEl, true,
                                        t_TyAllocatorGraphNode, 
                                        t_TyAllocatorGraphLink,
                                        t_TyAllocatorPathNodeBase, 
                                        t_TyAllocatorPathNodeSafe,
                                        t_TyGraphNode, t_TyGraphLink, t_TyPathNode >
  : public _graph_traits_safety_base< t_TyNodeEl, t_TyLinkEl,
                                      t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                      t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe, 
                                      true, t_TyGraphNode, t_TyGraphLink, t_TyPathNode >
{
  typedef _graph_traits_safety_base<  t_TyNodeEl, t_TyLinkEl,
                                      t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                      t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe, 
                                      true, t_TyGraphNode, t_TyGraphLink, t_TyPathNode > _TyGraphTraitsBase;

  // non-safe iterator types:
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphNodeIteratorType, true, false >       _TyNodeIteratorConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphNodeIteratorType, false, false >      _TyNodeIteratorNonConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkPosIteratorType, true, false >    _TyLinkPosIteratorConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkPosIteratorType, false, false >   _TyLinkPosIteratorNonConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkIdentIteratorType, true, false >  _TyLinkIdentIteratorConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkIdentIteratorType, false, false > _TyLinkIdentIteratorNonConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphPathIteratorType, true, false >       _TyPathIteratorConstTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphPathIteratorType, false, false >      _TyPathIteratorNonConstTraits;

  // safe iterator types:
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphNodeIteratorType, true, true >        _TyNodeIteratorConstSafeTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphNodeIteratorType, false, true >       _TyNodeIteratorNonConstSafeTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkPosIteratorType, true, true >     _TyLinkPosIteratorConstSafeTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkPosIteratorType, false, true >    _TyLinkPosIteratorNonConstSafeTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkIdentIteratorType, true, true >   _TyLinkIdentIteratorConstSafeTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphLinkIdentIteratorType, false, true >  _TyLinkIdentIteratorNonConstSafeTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphPathIteratorType, true, true >        _TyPathIteratorConstSafeTraits;
  typedef _graph_iterator_traits< _TyGraphTraitsBase, 
            _TyGraphPathIteratorType, false, true >       _TyPathIteratorNonConstSafeTraits;
};

// full graph traits object:
template <  class t_TyNodeEl, class t_TyLinkEl, 
            bool t_fIsSafeGraph,
            class t_TyAllocatorGraphNode = allocator<char>,   // Cascade the default allocators.
            class t_TyAllocatorGraphLink = t_TyAllocatorGraphNode,
            class t_TyAllocatorPathNodeBase = t_TyAllocatorGraphLink, 
            class t_TyAllocatorPathNodeSafe = t_TyAllocatorPathNodeBase,
            class t_TyGraphNode = _graph_node< t_TyNodeEl, t_TyLinkEl, _graph_node_base >, 
            class t_TyGraphLink = _graph_link< t_TyLinkEl, t_TyNodeEl, _graph_link_base< _graph_node_base > >,
            class t_TyPathNode = _graph_path_node_base< _graph_node_base, _graph_link_base< _graph_node_base > > >
struct _graph_traits
  : public _graph_traits_decl_iter_traits<  t_TyNodeEl, t_TyLinkEl, t_fIsSafeGraph,
                                            t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                                            t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe,
                                            t_TyGraphNode, t_TyGraphLink, t_TyPathNode >
{
private:
	typedef _graph_traits_decl_iter_traits<  t_TyNodeEl, t_TyLinkEl, t_fIsSafeGraph,
		t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
		t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe,
		t_TyGraphNode, t_TyGraphLink, t_TyPathNode > _TyBase;
public:
  // Declare forward iterator stuff - common for all graphs - no safe version of the iterator.
	typedef typename _TyBase::_TyGraphNode _TyGraphNode;
	typedef typename _TyBase::_TyGraphLink _TyGraphLink;
	typedef typename _TyBase::_TyGraphNodeBaseBase _TyGraphNodeBaseBase;
	typedef typename _TyBase::_TyGraphLinkBaseBase _TyGraphLinkBaseBase;
	typedef typename _TyBase::_TyGraphBaseBase _TyGraphBaseBase;

	typedef _graph_fwd_iter_base_base<  _TyGraphNodeBaseBase, _TyGraphLinkBaseBase,
                                      t_TyAllocatorPathNodeBase > _TyGraphFwdIterPosBase;
  typedef _graph_fwd_iter_base< _TyGraphNodeBaseBase, _TyGraphLinkBaseBase, 
                                t_TyAllocatorPathNodeBase, false >        _TyGraphFwdIterBase;
  typedef _graph_fwd_iter_base< _TyGraphNodeBaseBase, _TyGraphLinkBaseBase, 
                                t_TyAllocatorPathNodeBase, true >         _TyGraphFwdIterSelectBase;

  // Iterator position - this identifies a point of iteration for the forward iter ( below ).
  typedef _graph_iter<  _TyGraphNode, _TyGraphLink, 
                        _TyGraphFwdIterPosBase, __true_type >     _TyGraphFwdIterPosConst;
  typedef _graph_iter<  _TyGraphNode, _TyGraphLink,
                        _TyGraphFwdIterPosBase, __false_type >    _TyGraphFwdIterPosNonConst;

  // forward iterator type that iterates entire graph:
  typedef _graph_iter<  _TyGraphNode, _TyGraphLink,
                        _TyGraphFwdIterBase, __true_type >        _TyGraphFwdIterConst;
  typedef _graph_iter<  _TyGraphNode, _TyGraphLink,
                        _TyGraphFwdIterBase, __false_type >       _TyGraphFwdIterNonConst;

  // Now a template that allows access to the link selection forward iterator:
  template < class t_TySelectLink >
  struct _get_link_select_iter
  {
    // Selection position types:
    typedef _graph_iter<  _TyGraphNode, _TyGraphLink,
                          _TyGraphFwdIterPosBase, __false_type,
                          true, t_TySelectLink >                  _TyGraphFwdIterPosSelectNonConst;
    typedef _graph_iter<  _TyGraphNode, _TyGraphLink,
                          _TyGraphFwdIterPosBase, __true_type,
                          true, t_TySelectLink >                  _TyGraphFwdIterPosSelectConst;
    // Selection iterators:
    typedef _graph_iter<  _TyGraphNode, _TyGraphLink,
                          _TyGraphFwdIterSelectBase, __false_type,
                          true, t_TySelectLink >                  _TyGraphFwdIterSelectNonConst;
    typedef _graph_iter<  _TyGraphNode, _TyGraphLink,
                          _TyGraphFwdIterSelectBase, __true_type,
                          true, t_TySelectLink >                  _TyGraphFwdIterSelectConst;
  };

  // dump iterator - we only define the const version - but if desired
  //  a non-const version can be accessed, via _TyDumpOstreamIteratorConst::iterator.
  typedef _dump_object< _TyGraphNode, _TyGraphLink, 
                        _ostream_object< _iostream_DumpElIO >, 
                        t_TyAllocatorPathNodeBase >  _TyDumpOstreamObject;
  typedef typename _TyDumpOstreamObject::_TyOutputStreamBase          _TyDumpOstreamObjectBase;
  typedef _graph_output_iter_base<  _TyDumpOstreamObjectBase, 
                                    t_TyAllocatorPathNodeBase,
#ifndef __NDEBUG_THROW
                                    true > /*use seek when want to be throw safe*/
#else //!__NDEBUG_THROW
                                    false > /*don't use seek - since output to cout can be garbled*/
#endif //!__NDEBUG_THROW
                                            _TyDumpOstreamIterBase;

  typedef _graph_output_iterator< _TyGraphNode, _TyGraphLink, _TyDumpOstreamObject,
                                  _TyDumpOstreamIterBase, __true_type > _TyDumpOstreamIteratorConst;

  // binary output iterator:
  // Default is const and doesn't allow unconstructed ( unconnected ) links to be written:
  typedef _binary_output_object<  _TyGraphNode, _TyGraphLink, 
                                  _ostream_object< _iostream_RawElIO >,
                                  t_TyAllocatorPathNodeBase, 
                                  false, false >                    _TyBinaryOstreamOutput;
  typedef typename _TyBinaryOstreamOutput::_TyOutputStreamBase      _TyBinaryOstreamBase;
  typedef _graph_output_iter_base<  _TyBinaryOstreamBase, 
                                    t_TyAllocatorPathNodeBase,
                                    true > /*use seek*/             _TyBinaryOstreamIterBase;

  typedef _graph_output_iterator< _TyGraphNode, _TyGraphLink, _TyBinaryOstreamOutput,
                                  _TyBinaryOstreamIterBase, __true_type >   _TyBinaryOstreamIterConst;
  // binary input iterator:
  // Default is const and doesn't allow unconstructed ( unconnected ) links to be read:
  typedef _binary_input_object< _TyGraphNode, _TyGraphLink, 
                                _istream_object< _iostream_RawElIO >, 
                                false >                                     _TyBinaryIstreamInput;
  typedef typename _TyBinaryIstreamInput::_TyInputObjectBase                _TyBinaryIstreamBase;
  typedef _graph_input_iter_base< _TyBinaryIstreamBase, _TyGraphBaseBase, 
                                  t_TyAllocatorPathNodeBase,
                                  true, false >                             _TyBinaryIstreamIterBase;
  // Define a template to access the full type of the input iterator:
  template <  class t_TyMostDerivedGraph, 
              class t_TyInputObject, 
              class t_TyInputIterBase >
  struct _get_input_iterator
  {
    typedef _graph_input_iterator< t_TyMostDerivedGraph, 
                t_TyInputObject, t_TyInputIterBase, 
                __false_type >                                    _TyBinaryInputIterNonConst;
    typedef _graph_input_iterator< t_TyMostDerivedGraph, 
                t_TyInputObject, t_TyInputIterBase, 
                __true_type >                                     _TyBinaryInputIterConst;
  
  };
  // Need to have the most derived graph type to declare the input iterator itself.

#ifdef __GR_DEFINEOLEIO
  // Define some types for OLE stream binary I/O:
  typedef _binary_output_object<  _TyGraphNode, _TyGraphLink, 
                                  _IStream_object< _IStream_RawElIO >, 
                                  t_TyAllocatorPathNodeBase,
                                  false, false >                    _TyBinaryOLEOutputObject;
  typedef typename _TyBinaryOLEOutputObject::_TyOutputStreamBase    _TyBinaryOLEOutputBase;
  typedef _graph_output_iter_base<  _TyBinaryOLEOutputBase, 
                                    t_TyAllocatorPathNodeBase,
                                    true > /*use seek*/             _TyBinaryOLEOutputIterBase;

  typedef _graph_output_iterator< _TyGraphNode, _TyGraphLink, _TyBinaryOLEOutputObject,
                                  _TyBinaryOLEOutputIterBase, __true_type >   _TyBinaryOLEOutputIterConst;
  // binary input iterator:
  // Default is const and doesn't allow unconstructed ( unconnected ) links to be read:
  typedef _binary_input_object< _TyGraphNode, _TyGraphLink, 
                                _IStream_object< _IStream_RawElIO >, 
                                false >                                     _TyBinaryOLEInputObject;
  typedef typename _TyBinaryOLEInputObject::_TyInputObjectBase              _TyBinaryOLEInputBase;
  typedef _graph_input_iter_base< _TyBinaryOLEInputBase, _TyGraphBaseBase, 
                                  t_TyAllocatorPathNodeBase,
                                  true, false >                             _TyBinaryOLEInputIterBase;
  // Need to have the most derived graph type to declare the iterator itself.
#endif //__GR_DEFINEOLEIO
};


// Utility traits objects - these declare the default appropriate types for safe
//  versus non-safe graphs:
template <  class t_TyNodeEl, class t_TyLinkEl,
            class t_TyAllocatorGraphNode = allocator<char>,   // Cascade the default allocators.
            class t_TyAllocatorGraphLink = t_TyAllocatorGraphNode,
            class t_TyAllocatorPathNodeBase = t_TyAllocatorGraphLink, 
            class t_TyAllocatorPathNodeSafe = t_TyAllocatorPathNodeBase >
struct _graph_traits_notsafe
  : public _graph_traits< t_TyNodeEl, t_TyLinkEl, false,
                          t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                          t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe,
                          _graph_node< t_TyNodeEl, t_TyLinkEl, _graph_node_base >, 
                          _graph_link< t_TyLinkEl, t_TyNodeEl, _graph_link_base< _graph_node_base > >,
                          _graph_path_node_base< _graph_node_base, 
                            _graph_link_base< _graph_node_base > > >
{
};

template <  class t_TyNodeEl, class t_TyLinkEl,
            class t_TyAllocatorGraphNode = allocator<char>,   // Cascade the default allocators.
            class t_TyAllocatorGraphLink = t_TyAllocatorGraphNode,
            class t_TyAllocatorPathNodeBase = t_TyAllocatorGraphLink, 
            class t_TyAllocatorPathNodeSafe = t_TyAllocatorPathNodeBase >
struct _graph_traits_safe
  : public _graph_traits< t_TyNodeEl, t_TyLinkEl, true,
                          t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                          t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe,
                          _graph_node< t_TyNodeEl, t_TyLinkEl, _graph_node_safe_base >, 
                          _graph_link< t_TyLinkEl, t_TyNodeEl, _graph_link_safe_base< _graph_node_safe_base > >,
                          _graph_path_node_safe_base< _graph_node_safe_base, 
                            _graph_link_safe_base< _graph_node_safe_base > > >
{
#ifdef __GR_USESHADOWSTUFF
  // Define graph traits for a shadow graph of this graph:
  typedef _graph_traits< t_TyNodeEl, t_TyLinkEl, false,
                          t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                          t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe,
                          _sgraph_node< t_TyNodeEl, t_TyLinkEl, _graph_node_base >, 
                          _sgraph_link< t_TyLinkEl, t_TyNodeEl, _graph_link_base< _graph_node_base > >,
                          _graph_path_node_base< _graph_node_base, 
                            _graph_link_base< _graph_node_base > >  _TyShadowTraitsNotSafe;
  typedef _graph_traits< t_TyNodeEl, t_TyLinkEl, true,
                          t_TyAllocatorGraphNode, t_TyAllocatorGraphLink,
                          t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe,
                          _sgraph_node< t_TyNodeEl, t_TyLinkEl, _graph_node_safe_base >, 
                          _sgraph_link< t_TyLinkEl, t_TyNodeEl, _graph_link_safe_base< _graph_node_safe_base > >,
                          _graph_path_node_safe_base< _graph_node_safe_base, 
                            _graph_link_safe_base< _graph_node_safe_base > >  _TyShadowTraitsSafe;
#endif //__GR_USESHADOWSTUFF
};

// Now declare a mapping type given the default graph parameters:
template <  class t_TyNodeEl, class t_TyLinkEl, bool t_fIsSafeGraph, class t_TyAllocator >
struct _graph_traits_map
{
  typedef _graph_traits_safe< t_TyNodeEl, t_TyLinkEl, t_TyAllocator > _TyGraphTraits;
};

template <  class t_TyNodeEl, class t_TyLinkEl, class t_TyAllocator >
struct _graph_traits_map< t_TyNodeEl, t_TyLinkEl, false, t_TyAllocator >
{
  typedef _graph_traits_notsafe< t_TyNodeEl, t_TyLinkEl, t_TyAllocator >  _TyGraphTraits;
};

__DGRAPH_END_NAMESPACE

#endif //__GR_TRT_H
