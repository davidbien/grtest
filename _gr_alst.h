#ifndef __GR_ALST
#define __GR_ALST

__DGRAPH_BEGIN_NAMESPACE

// Allocator sets for dgraphs.

template <  class t_TyAllocatorGraphNode,
            class t_TyAllocatorGraphLink >
struct _allocator_set_base
{
  t_TyAllocatorGraphNode    m_allocGraphNode;
  t_TyAllocatorGraphLink    m_allocGraphLink;

  _allocator_set_base(  t_TyAllocatorGraphNode _allocGraphNode = t_TyAllocatorGraphNode(),
                        t_TyAllocatorGraphLink _allocGraphLink = t_TyAllocatorGraphLink() )
    : m_allocGraphNode( _allocGraphNode ),
      m_allocGraphLink( _allocGraphLink )
  {
  }

  _allocator_set_base( _allocator_set_base const & _r )
    : m_allocGraphNode( _r.m_allocGraphNode ),
      m_allocGraphLink( _r.m_allocGraphLink )
  {
  } 
};

template < class t_TyAllocatorPathNodeBase >
struct _allocator_set_notsafe_base
{
  t_TyAllocatorPathNodeBase m_allocPathNodeBase;

  _allocator_set_notsafe_base(  t_TyAllocatorPathNodeBase _allocPathNodeBase = t_TyAllocatorPathNodeBase() )
    : m_allocPathNodeBase( _allocPathNodeBase )
  {
  }

  _allocator_set_notsafe_base( _allocator_set_notsafe_base const & _r )
    : m_allocPathNodeBase( _r.m_allocPathNodeBase )
  {
  } 

  // Any other _allocator_set_safe_base<> can initialize this one:
  template <  class t__TyAllocatorPathNodeBase >
  _allocator_set_notsafe_base( _allocator_set_notsafe_base< t__TyAllocatorPathNodeBase > const & _r )
    : m_allocPathNodeBase( _r.m_allocPathNodeBase )
  {
  } 
};

template <  class t_TyAllocatorPathNodeBase,
            class t_TyAllocatorPathNodeSafe >
struct _allocator_set_safe_base
{
  t_TyAllocatorPathNodeBase m_allocPathNodeBase;
  t_TyAllocatorPathNodeSafe m_allocPathNodeSafe;

  _allocator_set_safe_base( t_TyAllocatorPathNodeBase _allocPathNodeBase = t_TyAllocatorPathNodeBase(),
                            t_TyAllocatorPathNodeSafe _allocPathNodeSafe = t_TyAllocatorPathNodeSafe() )
    : m_allocPathNodeBase( _allocPathNodeBase ),
      m_allocPathNodeSafe( _allocPathNodeSafe )
  {
  }

#if 0 // Following template takes care of this:
  _allocator_set_safe_base( _allocator_set_safe_base const & _r )
    : m_allocPathNodeBase( _r.m_allocPathNodeBase ),
      m_allocPathNodeSafe( _r.m_allocPathNodeSafe )
  {
  } 
#endif 0

  // Any other _allocator_set_safe_base<> can initialize this one:
  template <  class t__TyAllocatorPathNodeBase,
              class t__TyAllocatorPathNodeSafe >
  _allocator_set_safe_base( _allocator_set_safe_base< t__TyAllocatorPathNodeBase,
                              t__TyAllocatorPathNodeSafe > const & _r )
    : m_allocPathNodeBase( _r.m_allocPathNodeBase ),
      m_allocPathNodeSafe( _r.m_allocPathNodeSafe )
  {
  } 
};

template <  class t_TyAllocatorGraphNode,
            class t_TyAllocatorGraphLink,
            class t_TyAllocatorPathNodeBase >
struct _allocator_set_notsafe
  : public _allocator_set_base< t_TyAllocatorGraphNode, t_TyAllocatorGraphLink >,
    public _allocator_set_notsafe_base< t_TyAllocatorPathNodeBase >
{
  typedef _allocator_set_base< t_TyAllocatorGraphNode, t_TyAllocatorGraphLink > _TyBaseAllocatorSet;
  typedef _allocator_set_notsafe_base< t_TyAllocatorPathNodeBase >              _TyBaseAllocatorSetSafety;

  _allocator_set_notsafe( t_TyAllocatorGraphNode _allocGraphNode = t_TyAllocatorGraphNode(),
                          t_TyAllocatorGraphLink _allocGraphLink = t_TyAllocatorGraphLink(),
                          t_TyAllocatorPathNodeBase _allocPathNodeBase = t_TyAllocatorPathNodeBase() )
    : _TyBaseAllocatorSet( _allocGraphNode, _allocGraphLink ),
      _TyBaseAllocatorSetSafety( _allocPathNodeBase )
  {
  }

#if 0 // The following template takes care of this:
  _allocator_set_notsafe( _allocator_set_notsafe const & _r )
    : _TyBaseAllocatorSet( _r ),
      _TyBaseAllocatorSetSafety( _r )
  {
  }
#endif 0

  // Any other non-safe allocator set can initialize this one:
  template <  class t__TyAllocatorGraphNode,
              class t__TyAllocatorGraphLink,
              class t__TyAllocatorPathNodeBase >
  _allocator_set_notsafe( _allocator_set_notsafe< t__TyAllocatorGraphNode, 
                            t__TyAllocatorGraphLink, t__TyAllocatorPathNodeBase > const & _r )
  : _TyBaseAllocatorSet( static_cast< typename _allocator_set_notsafe< t__TyAllocatorGraphNode, 
                            t__TyAllocatorGraphLink, t__TyAllocatorPathNodeBase >::
                            _TyBaseAllocatorSet const & >( _r ) ),
    _TyBaseAllocatorSetSafety( static_cast< typename _allocator_set_notsafe< t__TyAllocatorGraphNode, 
                            t__TyAllocatorGraphLink, t__TyAllocatorPathNodeBase >::
                            _TyBaseAllocatorSetSafety const & >( _r ) )
  {
  }
};

template <  class t_TyAllocatorGraphNode,
            class t_TyAllocatorGraphLink,
            class t_TyAllocatorPathNodeBase,
            class t_TyAllocatorPathNodeSafe >
struct _allocator_set_safe
  : public _allocator_set_base< t_TyAllocatorGraphNode, t_TyAllocatorGraphLink >,
    public _allocator_set_safe_base< t_TyAllocatorPathNodeBase, t_TyAllocatorPathNodeSafe >
{
  typedef _allocator_set_base< t_TyAllocatorGraphNode, t_TyAllocatorGraphLink > _TyBaseAllocatorSet;
  typedef _allocator_set_safe_base< t_TyAllocatorPathNodeBase, 
                        t_TyAllocatorPathNodeSafe >     _TyBaseAllocatorSetSafety;

  _allocator_set_safe(  t_TyAllocatorGraphNode _allocGraphNode = t_TyAllocatorGraphNode(),
                        t_TyAllocatorGraphLink _allocGraphLink = t_TyAllocatorGraphLink(),
                        t_TyAllocatorPathNodeBase _allocPathNodeBase = t_TyAllocatorPathNodeBase(),
                        t_TyAllocatorPathNodeSafe _allocPathNodeSafe = t_TyAllocatorPathNodeSafe() )
    : _TyBaseAllocatorSet( _allocGraphNode, _allocGraphLink ),
      _TyBaseAllocatorSetSafety( _allocPathNodeBase, _allocPathNodeSafe )
  {
  }

  _allocator_set_safe( _allocator_set_safe const & _r )
    : _TyBaseAllocatorSet( _r ),
      _TyBaseAllocatorSetSafety( _r )
  {
  } 

  // Any other non-safe allocator set can initialize this one:
  template <  class t__TyAllocatorGraphNode,
              class t__TyAllocatorGraphLink,
              class t__TyAllocatorPathNodeBase,
              class t__TyAllocatorPathNodeSafe >
  _allocator_set_safe( _allocator_set_safe< t__TyAllocatorGraphNode, 
                            t__TyAllocatorGraphLink, t__TyAllocatorPathNodeBase,
                            t__TyAllocatorPathNodeSafe > const & _r )
  : _TyBaseAllocatorSet( static_cast< typename _allocator_set_safe< t__TyAllocatorGraphNode, 
                            t__TyAllocatorGraphLink, t__TyAllocatorPathNodeBase,
                            t__TyAllocatorPathNodeSafe >::
                            _TyBaseAllocatorSet const & >( _r ) ),
    _TyBaseAllocatorSetSafety( static_cast< typename _allocator_set_safe< t__TyAllocatorGraphNode, 
                            t__TyAllocatorGraphLink, t__TyAllocatorPathNodeBase,
                            t__TyAllocatorPathNodeSafe >::
                            _TyBaseAllocatorSetSafety const & >( _r ) )
  {
  }
};    

__DGRAPH_END_NAMESPACE

#endif __GR_ALST
