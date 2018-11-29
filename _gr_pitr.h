#ifndef __GR_PITR_H
#define __GR_PITR_H

// _gr_pitr.h

// Template for generic "graph pass iterator". This works for the three single connection iterators:
//  graph_node_iterator, graph_link_pos_iterator, _graph_link_ident_iterator.

// The reason that "pass" iterators exist is to provide a method of passing contructed
//  iterators without copying them - since this is not a trivial operation.

__DGRAPH_BEGIN_NAMESPACE

#if 1
template < class t_TyFIsConstIter, class t_TyBaseClass >
class _graph_iterator_pass2 : public t_TyBaseClass
{
 public:
  typedef t_TyFIsConstIter _TyFIsConstIter;
};

template < class t_TyFIsConstIter, class t_TyBaseClass >
class _graph_path_iterator_pass2
  : public t_TyBaseClass  // No public access to base class methods.
{
 public:
  typedef t_TyFIsConstIter _TyFIsConstIter;
};

#else 1
template <  class t_TyGraphNodeBase, class t_TyGraphLinkBase,
            class t_TyFIsConstIter, class t_TyBaseClass >
class _graph_iterator_pass
  : protected t_TyBaseClass // no public access to base class methods.
{
  typedef t_TyBaseClass                                 _TyBase;
  typedef _graph_iterator_pass< t_TyGraphNodeBase, 
    t_TyGraphLinkBase, 
    t_TyFIsConstIter,
    t_TyBaseClass >         _TyThis;

 protected:

  // These constructors protected - only accessed by graph itself:
  
  // Type for initialization argument:
  typedef typename _TyBase::_TyInitArg                  _TyInitArg;

  explicit _graph_iterator_pass( _TyInitArg _a )
    : _TyBase( _a )
    {
    }

 public:
  
  typedef t_TyFIsConstIter    _TyFIsConstIterator;

  // Only the copy constructor is public:
  template < class t_TyIterPass >
    _graph_iterator_pass( t_TyIterPass & _r ) __STLP_NOTHROW
    : _TyBase( 0 )  // We are initially empty.
    {
      // Swap our existence with the passed:
      __TRANSFER_CONST( typename t_TyIterPass::_TyFIsConstIterator, 
                        _TyFIsConstIterator );
      _r.swap( *this );
    }

  template < class t_TyIterPass >
    _TyThis & operator = ( t_TyIterPass & _r ) __STLP_NOTHROW
  {
    __TRANSFER_CONST( typename t_TyIterPass::_TyFIsConstIterator, 
                      _TyFIsConstIterator );
    _r.swap( *this );
    return *this;
  }

  // We only allow swapping with our base class:
  template < class t_TySwap >
    void  swap( t_TySwap & _r )
    {
      ___semantic_error_object  error;   // not us.
    }

  void  swap( _TyBase & _r )
    {
      _TyBase::swap( _r );  // us.
    }
};


// Use a new class for the path iterator - this is used to allow the graph to pass instances
//  and not copy the nodes.
// NOTE: This is not strongly type-safe ( though it is unlikely someone will screw-up - and if
//  they do then crashing will likely occur ). The type-safeness implemented here only ensures
//  that the base class ( including safety ) is the same - different element types could be
//  mixed ( in sneaky cases this could be kind of interesting ). This may not save much/any code either
//  ( it would be interesting to see ).
template <  class t_TyPathNodeBase, class t_TyPathNodeAllocator, 
            class t_TyFIsConstIter, class t_TyBaseClass >
class _graph_path_iterator_pass
  : protected t_TyBaseClass // No public access to base class methods.
{
  typedef t_TyBaseClass                                           _TyBase;
  typedef _graph_path_iterator_pass<  t_TyPathNodeBase, 
                                      t_TyPathNodeAllocator, 
                                      t_TyFIsConstIter,
                                      t_TyBaseClass >             _TyThis;

public:

  typedef t_TyFIsConstIter    _TyFIsConstIterator;

  // Only the copy constructor is public:
  template < class t_TyPathPass >
  _graph_path_iterator_pass( t_TyPathPass & _r ) __STLP_NOTHROW
    : _TyBase( _r, __false_type() ) // We are initially empty.
  {
    __TRANSFER_CONST( typename t_TyPathPass::_TyFIsConstIterator, 
                      _TyFIsConstIterator );
    _r.swap( static_cast< _TyBase& >( *this ) );  // Swap our existence with the passed.
  }

  template < class t_TyPathPass >
  _TyThis & operator = ( t_TyPathPass & _r ) __STLP_NOTHROW
  {
    __TRANSFER_CONST( typename t_TyPathPass::_TyFIsConstIterator, 
                      _TyFIsConstIterator );
    _r.swap( static_cast< _TyBase& >( *this ) );
    return *this;
  }

  // We only allow swapping with our base class:
  template < class t_TySwap >
  void  swap( t_TySwap & _r )
  {
    ___semantic_error_object  error;   // not us.
  }

  void  swap( _TyBase & _r )
  {
    _TyBase::swap( _r );  // us.
  }
};

#endif 0

__DGRAPH_END_NAMESPACE

#endif __GR_PITR_H
