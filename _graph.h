#ifndef __GRAPH_H
#define __GRAPH_H

// dgraph.h

// This module implements a directed (perhaps cyclic ) graph.

__DGRAPH_BEGIN_NAMESPACE

#ifndef __DGRAPH_USE_STLPORT
#define __DGRAPH_INSTANCED_ALLOCATORS // Only STLport supports instanceless allocators for its STL.
#endif

// need a define to allow instanced allocators ( they are rare ).
#ifdef __DGRAPH_INSTANCED_ALLOCATORS
#define __DGRAPH_STATIC_ALLOC_DECL  
#define __DGRAPH_STATIC_ALLOC_DECL_CONST const
#else //__DGRAPH_INSTANCED_ALLOCATORS
#define __DGRAPH_STATIC_ALLOC_DECL static
#define __DGRAPH_STATIC_ALLOC_DECL_CONST
#endif //__DGRAPH_INSTANCED_ALLOCATORS

template < class t_TyNodeEl, class t_TyLinkEl, 
  bool t_fIsSafeGraph = false, class t_TyAllocator = allocator<char>,
  class t_TyGraphTraits = 
    typename _graph_traits_map< t_TyNodeEl, t_TyLinkEl, t_fIsSafeGraph, t_TyAllocator >::_TyGraphTraits >
class dgraph
  : public t_TyGraphTraits::_TyGraphBase,
    public _alloc_base< typename t_TyGraphTraits::_TyGraphNode, 
                        typename t_TyGraphTraits::_TyGraphNodeAllocatorAsPassed >,
    public _alloc_base< typename t_TyGraphTraits::_TyGraphLink, 
                        typename t_TyGraphTraits::_TyGraphLinkAllocatorAsPassed >
{
  typedef typename t_TyGraphTraits::_TyGraphBase            _TyBaseGraph;
  typedef dgraph< t_TyNodeEl, t_TyLinkEl, t_fIsSafeGraph, 
                  t_TyAllocator, t_TyGraphTraits >          _TyThis;

public:
  
  typedef t_TyGraphTraits                         _TyGraphTraits;
  typedef typename _TyGraphTraits::_TyGraphNode   _TyGraphNode;
  typedef typename _TyGraphTraits::_TyGraphLink   _TyGraphLink;
  typedef typename _TyGraphTraits::_TyPathNode    _TyPathNode;
  typedef t_TyNodeEl                              _TyNodeEl;
  typedef t_TyLinkEl                              _TyLinkEl;

  // Iterator types:

  // Iterator position - this identifies a point of iteration for the forward iter ( below ).
  typedef typename _TyGraphTraits::_TyGraphFwdIterPosConst    _TyGraphFwdIterPosConst;
  typedef typename _TyGraphTraits::_TyGraphFwdIterPosNonConst _TyGraphFwdIterPosNonConst;

  // forward iterator type that iterates entire graph:
  typedef typename _TyGraphTraits::_TyGraphFwdIterConst       _TyGraphFwdIterConst;
  typedef typename _TyGraphTraits::_TyGraphFwdIterNonConst    _TyGraphFwdIterNonConst;

  typedef _TyGraphFwdIterPosConst     const_iterator_pos;
  typedef _TyGraphFwdIterPosNonConst  iterator_pos;

  typedef _TyGraphFwdIterConst        const_iterator;
  typedef _TyGraphFwdIterNonConst     iterator;

  // non-safe:
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyNodeIteratorConstTraits >          _TyNodeIterConst;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyNodeIteratorNonConstTraits >       _TyNodeIterNonConst;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyLinkPosIteratorConstTraits >       _TyLinkPosIterConst;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyLinkPosIteratorNonConstTraits >    _TyLinkPosIterNonConst;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyLinkIdentIteratorConstTraits >     _TyLinkIdentIterConst;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyLinkIdentIteratorNonConstTraits >  _TyLinkIdentIterNonConst;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyPathIteratorConstTraits >          _TyPathIterConst;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyPathIteratorNonConstTraits >       _TyPathIterNonConst;

  // safe - these are ONLY safe on a safe graph - they are the same as above on a non-safe graph:
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyNodeIteratorConstSafeTraits >          _TyNodeIterConstSafe;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyNodeIteratorNonConstSafeTraits >       _TyNodeIterNonConstSafe;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyLinkPosIteratorConstSafeTraits >       _TyLinkPosIterConstSafe;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyLinkPosIteratorNonConstSafeTraits >    _TyLinkPosIterNonConstSafe;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyLinkIdentIteratorConstSafeTraits >     _TyLinkIdentIterConstSafe;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyLinkIdentIteratorNonConstSafeTraits >  _TyLinkIdentIterNonConstSafe;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyPathIteratorConstSafeTraits >          _TyPathIterConstSafe;
  typedef _graph_typed_iterator
    < typename _TyGraphTraits::_TyPathIteratorNonConstSafeTraits >       _TyPathIterNonConstSafe;

  // Dump iterator - only const currently:
  typedef typename _TyGraphTraits::_TyDumpOstreamIteratorConst    _TyDumpIteratorConst;

  // Binary output iterator - this type supports const output to ostream:
  typedef typename _TyGraphTraits::_TyBinaryOstreamIterConst      _TyBinaryOstreamIterConst;
  // Binary output iterator - this type supports input from istream:
  typedef typename _TyGraphTraits:: template _get_input_iterator< _TyThis,
    typename _TyGraphTraits::_TyBinaryIstreamInput,
    typename _TyGraphTraits::_TyBinaryIstreamIterBase >::_TyBinaryInputIterNonConst _TyBinaryIstreamIterNonConst;

#ifdef __GR_DEFINEOLEIO
  typedef typename _TyGraphTraits::_TyBinaryOLEOutputIterConst      _TyBinaryOLEOutputIterConst;
  // Binary output iterator - this type supports input from istream:
  typedef typename _TyGraphTraits:: template _get_input_iterator< _TyThis,
    typename _TyGraphTraits::_TyBinaryOLEInputObject,
    typename _TyGraphTraits::_TyBinaryOLEInputIterBase >::_TyBinaryInputIterNonConst  _TyBinaryOLEInputIterNonConst;
#endif //__GR_DEFINEOLEIO

  typedef typename _TyGraphTraits::_TyAllocatorSet                _TyAllocatorSet;  // Set of all required allocators.
  typedef typename _TyGraphTraits::_TyGraphNodeAllocator          _TyGraphNodeAllocator;
  typedef typename _TyGraphTraits::_TyGraphLinkAllocator          _TyGraphLinkAllocator;
  typedef typename _TyGraphTraits::_TyGraphNodeAllocatorAsPassed  _TyGraphNodeAllocatorAsPassed;
  typedef typename _TyGraphTraits::_TyGraphLinkAllocatorAsPassed  _TyGraphLinkAllocatorAsPassed;

  typedef _TyBaseGraph _TyGraphBase;

private:

  typedef _alloc_base< _TyGraphNode, _TyGraphNodeAllocatorAsPassed >    _TyBaseAllocGraphNode;
  typedef _alloc_base< _TyGraphLink, _TyGraphLinkAllocatorAsPassed >    _TyBaseAllocGraphLink;

public:

  explicit dgraph( _TyAllocatorSet const & _rAllocSet = _TyAllocatorSet() ) _BIEN_NOTHROW
    : _TyBaseAllocGraphNode( _rAllocSet.m_allocGraphNode ),
      _TyBaseAllocGraphLink( _rAllocSet.m_allocGraphLink ),
      _TyBaseGraph( static_cast< typename _TyAllocatorSet::_TyBaseAllocatorSetSafety const & >( _rAllocSet ) )
  {
  }
  
  explicit dgraph( _TyThis const & _r )
    : _TyBaseAllocGraphNode( _r.get_node_allocator() ),
      _TyBaseAllocGraphLink( _r.get_link_allocator() ),
      _TyBaseGraph( static_cast< const _TyGraphBase & >( _r ) )
  {
    replace_copy( _r );
  }

  // Construct from another graph - this copies all the nodes and links - 
  //  obtains allocators from the passed graph:
  template < class t_TyGraph >
  explicit dgraph( t_TyGraph const & _r )
    : _TyBaseAllocGraphNode( _r.get_node_allocator() ),
      _TyBaseAllocGraphLink( _r.get_link_allocator() ),
      _TyBaseGraph( static_cast< typename t_TyGraph::_TyGraphBase const & >( _r ) )
  {
    replace_copy( _r );
  }

  // Construct from another graph and a set of allocators:
  template <  class t_TyGraph >
  dgraph( t_TyGraph const & _r,
          _TyAllocatorSet const & _rAllocSet )
    : _TyBaseAllocGraphNode( _rAllocSet.m_allocGraphNode ),
      _TyBaseAllocGraphLink( _rAllocSet.m_allocGraphLink ),
      _TyBaseGraph( _rAllocSet )
  {
    replace_copy( _r );
  }

  ~dgraph()
  {
    destroy();
  }

  void  clear()
  {
    destroy();
  }

 // root node:
  _TyGraphNode *  get_root() _BIEN_NOTHROW
  {
    return static_cast< _TyGraphNode * >( _TyBaseGraph::_GetRootNode() );
  }
  const _TyGraphNode *  get_root() const _BIEN_NOTHROW
  {
    return static_cast< const _TyGraphNode * >( _TyBaseGraph::_GetRootNode() );
  }
  void set_root_node( _TyGraphNode * _pgn ) _BIEN_NOTHROW
  {
    _TyBaseGraph::_SetRootNode( _pgn );
  }

// Node creation - these could be static but to allow for instanced allocators
//  we make them non-static members:
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphNode *  create_node()
  {
    _TyGraphNode * pgn = _allocate_node();
    _BIEN_TRY
    {
		  _construct_node( pgn );
    }
    _BIEN_UNWIND( _deallocate_node( pgn ) );
    return pgn;
  }

  template < class t_TyP1 >
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphNode *  create_node1( t_TyP1 _p1 )
  {
    _TyGraphNode * pgn = _allocate_node();
    _BIEN_TRY
    {
		  _TyThis::template _construct_node1< t_TyP1 >( pgn, _p1 );
    }
    _BIEN_UNWIND( _deallocate_node( pgn ) );
    return pgn;
  }
  
  template < class t_TyP1, class t_TyP2 >
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphNode *  create_node2( t_TyP1 _p1, t_TyP2 _p2 )
  {
    _TyGraphNode * pgn = _allocate_node();
    _BIEN_TRY
    {
		  _TyThis::template _construct_node2< t_TyP1, t_TyP2 >( pgn, _p1, _p2 );
    }
    _BIEN_UNWIND( _deallocate_node( pgn ) );
    return pgn;
  }

// link creation:
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphLink *  create_link()
  {
    _TyGraphLink * pgl = _allocate_link();
    _BIEN_TRY
    {
		  _construct_link( pgl );
    }
    _BIEN_UNWIND( _deallocate_link( pgl ) );
    return pgl;
  }

  template < class t_TyP1 >
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphLink *  create_link1( t_TyP1 _p1 )
  {
    _TyGraphLink * pgl = _allocate_link();
    _BIEN_TRY
    {
		  _TyThis::template _construct_link1< t_TyP1 >( pgl, _p1 );
    }
    _BIEN_UNWIND( _deallocate_link( pgl ) );
    return pgl;
  }
  
  template < class t_TyP1, class t_TyP2 >
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphLink *  create_link2( t_TyP1 _p1, t_TyP2 _p2 )
  {
    _TyGraphLink * pgl = _allocate_link();
    _BIEN_TRY
    {
		  _TyThis::template _construct_link2< t_TyP1, t_TyP2 >( pgl, _p1, _p2 );
    }
    _BIEN_UNWIND( _deallocate_link( pgl ) );
    return pgl;
  }

// Iterator positions - these are not full blown iterators - only describe a starting position
//  in the graph:
  // begin() returns an iterator position that when passed to an actual iterator will enable
  //  forward iteration in a child-wise depth-first manner.
  // NOTE: Forward iterators do not have methods to modify the graph - this is because modification
  //  will result in undefined bahavior ( currently potentially even crashing ).
  // When thread-safety is added is issue will be addressed.
  _TyGraphFwdIterPosConst begin() const _BIEN_NOTHROW
  {
    return _TyGraphFwdIterPosConst( get_root(), 0, false, true, 
                                    _TyBaseGraph::get_base_path_allocator() );
  }
  _TyGraphFwdIterPosNonConst begin() _BIEN_NOTHROW
  {
    return _TyGraphFwdIterPosNonConst(  get_root(), 0, false, true, 
                                        _TyBaseGraph::get_base_path_allocator() );
  }

  // rbegin() does not iterate the nodes in reverse - it just starts the iteration
  //  in the up ( parent-wise ) direction.
  _TyGraphFwdIterPosConst rbegin() const _BIEN_NOTHROW
  {
    return _TyGraphFwdIterPosConst( get_root(), 0, false, false, 
                                    _TyBaseGraph::get_base_path_allocator() );
  }
  _TyGraphFwdIterPosNonConst rbegin() _BIEN_NOTHROW
  {
    return _TyGraphFwdIterPosNonConst(  get_root(), 0, false, false, 
                                        _TyBaseGraph::get_base_path_allocator() );
  }

  _TyGraphFwdIterPosConst end() const _BIEN_NOTHROW
  {
    return _TyGraphFwdIterPosConst( 0, 0, false, true,
                                    _TyBaseGraph::get_base_path_allocator() );
  }
  _TyGraphFwdIterPosNonConst end() _BIEN_NOTHROW
  {
    return _TyGraphFwdIterPosNonConst(  0, 0, false, true, 
                                        _TyBaseGraph::get_base_path_allocator() );
  }

  _TyGraphFwdIterPosConst rend() const _BIEN_NOTHROW
  {
    return end();
  }
  _TyGraphFwdIterPosNonConst rend() _BIEN_NOTHROW
  {
    return end();
  }

// Iterator transformation ( const vs. non-const ). Access is controlled by graph object -
//  if one has a non-const graph then transformation of a const iterator to non-const one is
//  supported.

  // Turn a const iterator into a non-const one:
  template < class t_TyIterator >
  typename t_TyIterator::iterator
  transform_nonconst( t_TyIterator const & _rConstIter )
  {
    typedef typename t_TyIterator::iterator _TyTransform;
    return _TyTransform( _rConstIter, __gr_const_cast_iter() ); // tag allows transformation.
  }

  // Turn a non-const iterator into a const one:
  template < class t_TyIterator >
  typename t_TyIterator::const_iterator
  transform_const( t_TyIterator const & _rNonConstIter ) const
  {
    typedef typename t_TyIterator::const_iterator _TyTransform;
    return _TyTransform( _rNonConstIter );  // transformation from non-const->const is well defined.
  }

  template < class t_TyGraph >
  _TyThis &
  operator = ( t_TyGraph const & _r )
  {
    replace_copy( _r );
    return *this;
  }

#if 0
  // Comparison compares both structure and values.
  template < class t_TyGraph >
  bool
  operator == ( t_TyGraph const & _r ) const
  {
    pair< _TyGraphFwdIterPosConst, t_TyGraph::_TyGraphFwdIterPosConst >
      pResult = compare( _r );
    return pResult.first.FAtEnd() && pResult.second.FAtEnd();
  }

  // Compare both structure and values.
  template < class t_TyGraph >
  pair< const_iterator_pos, typename t_TyGraph::const_iterator_pos >
  compare( t_TyGraph const & _r ) const
  {
    // Algorithm - since we can't be sure ( whatsoever ) that just cuz a graph iterates with the
    //  same values that it has the same structure (duh) we will attempt the following algorithm
    //  ( without proof of correctness or efficiency ).
    // At each node check that the number of parents and children are the same. 
    //  ( actually this is checked via iteration - if a node has a different number of
    //    parents or children then the iteration will differ at some point. )
    //  ( TODO: is this true - come up with counter-example ).
    // What we really need to check is that the links from a given node have the following properties:
    //  1) The (parent/child) link position is the same for both links ( this requires checking the 
    //    (child/parent) node's (parent/child) list ).
    //  2) That unique nodes correspond in each graph - i.e. given graphs A and B and links a<i>,a<j> and b<i>,b<j>
    //    if a<i>.parent == a<j>.parent then b<i>.parent == b<j>.parent ( for links a<n> b<n> at iteration position n ).
    // ( It turns out that the links we need to check are those that enter into a multiple opposite relation
    //    node ( what I called an "unfinished" node in the fwd iterator stuff and an "unconnected" node
    //    in the copy stuff ) )
    if ( &_r == this )
    {
    }
    else
    {
      // Get an iterator on both graphs and inspect various things:

    }
  }
#endif //0

  // Just compare the values encountered in the iteration, stopping:
  //  1) Where the values are different. <or>
  //  2) Where the graph element encountered is different ( link vs. node ).
  // Return the positions in each graph where the difference was encountered.
  template < class t_TyGraph >
  pair< const_iterator_pos, typename t_TyGraph::const_iterator_pos >
  compare_values( t_TyGraph const & _gOther ) const
  {
    typedef pair< const_iterator_pos, typename t_TyGraph::const_iterator_pos >  _TyResult;

    const_iterator itThis( begin() );
    typename t_TyGraph::const_iterator itOther = _gOther.begin();

    for ( ; 
          !itThis.FAtEnd() && !itOther.FAtEnd();
          ++itThis, ++itOther )
    {
      const _TyGraphLink *  pglThis = itThis.PGLCur();
      const typename t_TyGraph::_TyGraphLink *  pglOther = itOther.PGLCur();
      if ( !!pglThis == !!pglOther )
      {
        if ( pglThis )
        {
          // If we could have non-constructed links:
          if (  ( pglThis->FIsConstructed() == pglOther->FIsConstructed() ) &&
                ( !pglThis->FIsConstructed() ||
                  ( pglThis->RElConst() == pglOther->RElConst() ) ) )
          {
            continue;
          }
        }
        else
        {
          const _TyGraphNode *  pgnThis = itThis.PGNCur();
          const typename t_TyGraph::_TyGraphNode *  pgnOther = itOther.PGNCur();
          if ( pgnThis->RElConst() == pgnOther->RElConst() )
          {
            continue;
          }
        }       
      }

      // If we got here then we are done:
      break;
    }
          
    return _TyResult( itThis, itOther );
  }

  template < class t_TyGraph >
  void  replace_copy( t_TyGraph const & _r )
  {
    // This destroys the current graph first - could do otherwise:
    destroy();

    _graph_copy_struct< t_TyGraph, _TyThis, 
      typename _TyBaseGraph::_TyPathNodeBaseAllocatorAsPassed > 
        gcs( _r, *this, false, true, _TyBaseGraph::get_base_path_allocator() );
    gcs.m_pgnSrcCopyRoot = _r.get_root();
    if ( gcs.m_pgnSrcCopyRoot )
    {
      gcs.copy();
      set_root_node( gcs.PGNTransferNewRoot() );
    }
  }

  template < class t_TyGraph, class t_TyCopyNodeObject, 
                              class t_TyCopyLinkObject >
  void  replace_copy( t_TyGraph const & _r, 
                      t_TyCopyNodeObject const & _rcno,
                      t_TyCopyLinkObject const & _rclo )
  {
    // This destroys the current graph first - could do otherwise:
    destroy();

    _graph_copy_struct< t_TyGraph, _TyThis, 
      typename _TyBaseGraph::_TyPathNodeBaseAllocatorAsPassed,
      t_TyCopyNodeObject, t_TyCopyLinkObject > 
        gcs( _r, *this, false, true, _TyBaseGraph::get_base_path_allocator(), _rcno, _rclo );
    gcs.m_pgnSrcCopyRoot = _r.get_root();
    if ( gcs.m_pgnSrcCopyRoot )
    {
      gcs.copy();
      set_root_node( gcs.PGNTransferNewRoot() );
    }
  }

  void save( ostream & _ros ) const
  {
    _TyBinaryOstreamIterConst boi( _ros, begin() );
    __DEBUG_STMT( int _i = 0 )
    while ( !boi.FAtEnd() )
    {
      ++boi;
      __DEBUG_STMT( ++_i );
    }
  }

  void replace_load( istream & _ris )
  {
    // Again we destroy the current graph first, this could happen
    //  after successful load, perhaps with a bool parameter.
    destroy();

    _TyBinaryIstreamIterNonConst bii( *this, _ris, _TyBaseGraph::get_base_path_allocator()  );

    __DEBUG_STMT( int _i = 0 )
    do
    {
      ++bii;
      __DEBUG_STMT( ++_i );
    }
    while( !bii.FAtEnd() );

    set_root_node( bii.PGNTransferNewRoot() );
  }

#ifdef __GR_DEFINEOLEIO
  void save( IStream * _pis ) const
  {
    _TyBinaryOLEOutputIterConst boi( _pis, begin() );
    __DEBUG_STMT( int _i = 0 )
    while ( !boi.FAtEnd() )
    {
      ++boi;
      __DEBUG_STMT( ++_i );
    }
  }
  template < class t_TyIONodeEl, class t_TyIOLinkEl, class t_TyFUseSeek >
  void save( IStream * _pis,  t_TyIONodeEl const & _rione, 
                              t_TyIOLinkEl const & _riole, 
                              t_TyFUseSeek = __true_type() ) const
  {
    typedef _IStream_object< t_TyIONodeEl, t_TyIOLinkEl > _TyIStreamObject;
    typedef _binary_output_object<  _TyGraphNode, _TyGraphLink, 
                                    _TyIStreamObject, 
                                    _TyPathNodeBaseAllocatorAsPassed,
                                    false, false >                                  _TyBinaryOLEOutputObject;
    typedef typename _TyBinaryOLEOutputObject::_TyOutputStreamBase                  _TyBinaryOLEOutputBase;
    typedef _graph_output_iter_base<  _TyBinaryOLEOutputBase, 
                                      _TyPathNodeBaseAllocatorAsPassed,
                                      __type_to_bool< t_TyFUseSeek >::ms_cfValue >  _TyBinaryOLEOutputIterBase;
    typedef _graph_output_iterator< _TyGraphNode, _TyGraphLink, _TyBinaryOLEOutputObject,
                                    _TyBinaryOLEOutputIterBase, __true_type >       _TyBinaryOLEOutputIterConst;

    _TyBinaryOLEOutputIterConst boi( _pis, begin(), _rione, _riole );
    __DEBUG_STMT( int _i = 0 )
    while ( !boi.FAtEnd() )
    {
      ++boi;
      __DEBUG_STMT( ++_i );
    }
  }

  void replace_load( IStream * _pis )
  {
    destroy();

    _TyBinaryOLEInputIterNonConst bii( *this, _pis, _TyBaseGraph::get_base_path_allocator()  );

    __DEBUG_STMT( int _i = 0 )
    do
    {
      ++bii;
      __DEBUG_STMT( ++_i );
    }
    while( !bii.FAtEnd() );

    set_root_node( bii.PGNTransferNewRoot() );
  }
  
  template < class t_TyIONodeEl, class t_TyIOLinkEl, class t_TyFUseSeek >
  void replace_load( IStream * _pis,  t_TyIONodeEl const & _rione, 
                                      t_TyIOLinkEl const & _riole, 
                                      t_TyFUseSeek = __true_type() )
  {
    destroy();

    typedef _IStream_object< t_TyIONodeEl, t_TyIOLinkEl > _TyIStreamObject;
    typedef _binary_input_object< _TyGraphNode, _TyGraphLink, 
                                  _TyIStreamObject, 
                                  false >                                     _TyBinaryOLEInputObject;
    typedef typename _TyBinaryOLEInputObject::_TyInputObjectBase              _TyBinaryOLEInputBase;
    typedef _graph_input_iter_base< _TyBinaryOLEInputBase, _TyGraphBaseBase, 
                                    _TyPathNodeBaseAllocatorAsPassed,
                                    __type_to_bool< t_TyFUseSeek >::ms_cfValue, 
                                    false >                                   _TyBinaryOLEInputIterBase;
    // Binary output iterator - this type supports input from istream:
    typedef typename _TyGraphTraits:: template _get_input_iterator< _TyThis,
      _TyBinaryOLEInputObject,_TyBinaryOLEInputIterBase >::_TyBinaryInputIterNonConst  _TyBinaryOLEInputIterNonConst;

    _TyBinaryOLEInputIterNonConst bii( *this, _pis, _TyBaseGraph::get_base_path_allocator(), _rione, _riole );

    __DEBUG_STMT( int _i = 0 )
    do
    {
      ++bii;
      __DEBUG_STMT( ++_i );
    }
    while( !bii.FAtEnd() );

    set_root_node( bii.PGNTransferNewRoot() );
  }

  // Load in-place - reads existing elements from stream.
  template < class t_TyIONodeEl, class t_TyIOLinkEl, class t_TyFUseSeek >
  void in_place_load( IStream * _pis, t_TyIONodeEl const & _rione, 
                                      t_TyIOLinkEl const & _riole, 
                                      t_TyFUseSeek = __true_type() )
  {
    typedef _IStream_object< t_TyIONodeEl, t_TyIOLinkEl > _TyIStreamObject;
    typedef _binary_input_object< _TyGraphNode, _TyGraphLink, 
                                  _TyIStreamObject, 
                                  false >                                     _TyBinaryOLEInputObject;
    typedef typename _TyBinaryOLEInputObject::_TyInputObjectBase              _TyBinaryOLEInputBase;
    typedef _graph_inplace_input_iter_base< _TyBinaryOLEInputBase, 
                                    _TyGraphNodeBaseBase, _TyGraphLinkBaseBase,
                                    __type_to_bool< t_TyFUseSeek >::ms_cfValue, 
                                    false >                                   _TyBinaryOLEInputIterBase;
    typedef _graph_inplace_input_iterator<  _TyGraphNode, _TyGraphLink,
                                            _TyBinaryOLEInputObject, _TyBinaryOLEInputIterBase,
                                            __false_type >                    _TyBinaryOLEInPlaceIter;

    // We iterate the current graph and the input stream at the same time.
    _TyBinaryOLEInPlaceIter bipi( _pis, _rione, _riole );
    iterator itThis = begin();

    while( !itThis.FAtEnd() && !bipi.FAtEnd() )
    {
      bipi.Next( itThis.PGLCur() ? 0 : itThis.PGNCur(), itThis.PGLCur() );
      ++itThis;
    }
  }

#endif //__GR_DEFINEOLEIO

  void  replace_random( size_t _stNodes,
                        size_t _stExtraLinks,
                        unsigned int _uRandSeed )
  {
    destroy();

    _random_graph_generator< _TyThis, 
      typename _TyBaseGraph::_TyPathNodeBaseAllocatorAsPassed >
        rgg( *this, _stNodes, _stExtraLinks, _uRandSeed, _TyBaseGraph::get_base_path_allocator() );

    set_root_node( rgg.PGNTransferNewRoot() );
  }

  // Destroy the graph nodes starting at the root.
  void
  destroy() _BIEN_NOTHROW
  {
    if ( get_root() )
    {
      _graph_destroy_struct<_TyThis>  gds( *this, get_root() );
      set_root_node( 0 );
      gds.destroy();
    }
  }

  // Destroy all nodes starting at <_pgn>.
  // Note: <_pgn> should NOT be connected to the root - a debug test ensures this.
  void
  destroy_node( _TyGraphNode * _pgn ) _BIEN_NOTHROW
  {
    assert( _pgn );
    _graph_destroy_struct<_TyThis>  gds( *this, _pgn );
    // Check to ensure that the the passed node is not connected to the root.
#ifdef __GRAPH_DEBUG_DTOR
    gds.SetCheckNode( get_root() );
#endif //__GRAPH_DEBUG_DTOR
    gds.destroy();
  }

  // Destroy a single node.
  // If connected then connections are ignored.
  __DGRAPH_STATIC_ALLOC_DECL void
  destroy_single_node( _TyGraphNode * _pgn ) _BIEN_NOTHROW
  {
    assert( _pgn );
    _destruct_node( _pgn );
    _deallocate_node( _pgn );
  }

  // Destroy only the link passed ( i.e. not any connect parent/child ).
  __DGRAPH_STATIC_ALLOC_DECL void
  destroy_link( _TyGraphLink * _pgl ) _BIEN_NOTHROW
  {
    assert( _pgl );
    _destruct_link( _pgl );
    _deallocate_link( _pgl );
  }

  // Dump a human-readable version of the graph to the given ostream:
  void  dump( ostream & _ros, 
              EDumpOptions _edo = e_doNone ) const
  {
    _TyDumpIteratorConst  dit(  _ros, get_root(), 0, false, true, 
                                _TyBaseGraph::get_base_path_allocator() );
    dit.SetOutputOptions( _edo );

    for ( ; !dit.FAtEnd(); ++dit )
      ;
  }
  void  dump_node(  ostream & _ros, _TyGraphNode * _pgn, 
                    EDumpOptions _edo = e_doNone ) const
  {
    _TyDumpIteratorConst  dit(  _ros, _pgn, 0, false, true, 
                                _TyBaseGraph::get_base_path_allocator() );
    dit.SetOutputOptions( _edo );

    for ( ; !dit.FAtEnd(); ++dit )
      ;
  }

// Allocation stuff:
// accessors:
#ifdef __DGRAPH_INSTANCED_ALLOCATORS
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphNodeAllocator const & get_node_allocator_ref() __DGRAPH_STATIC_ALLOC_DECL_CONST _BIEN_NOTHROW 
  { return _TyBaseAllocGraphNode::get_allocator_ref(); }
#endif //__DGRAPH_INSTANCED_ALLOCATORS
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphNodeAllocator &       get_node_allocator_ref() _BIEN_NOTHROW 
  { return _TyBaseAllocGraphNode::get_allocator_ref(); }
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphNodeAllocatorAsPassed get_node_allocator() __DGRAPH_STATIC_ALLOC_DECL_CONST _BIEN_NOTHROW 
  { return _TyBaseAllocGraphNode::get_allocator(); }

#ifdef __DGRAPH_INSTANCED_ALLOCATORS
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphLinkAllocator const & get_link_allocator_ref() __DGRAPH_STATIC_ALLOC_DECL_CONST _BIEN_NOTHROW 
  { return _TyBaseAllocGraphLink::get_allocator_ref(); }
#endif //__DGRAPH_INSTANCED_ALLOCATORS
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphLinkAllocator &       get_link_allocator_ref() _BIEN_NOTHROW 
  { return _TyBaseAllocGraphLink::get_allocator_ref(); }
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphLinkAllocatorAsPassed get_link_allocator() __DGRAPH_STATIC_ALLOC_DECL_CONST _BIEN_NOTHROW 
  { return _TyBaseAllocGraphLink::get_allocator(); }

// allocation of objects:
  __DGRAPH_STATIC_ALLOC_DECL _TyGraphNode * _allocate_node( )
  {
    _TyGraphNode * pgn = _TyBaseAllocGraphNode::allocate_type();
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iNodesAllocated++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
    return pgn;
  }
  __DGRAPH_STATIC_ALLOC_DECL void _deallocate_node( _TyGraphNode * _rpgn )
  {
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iNodesAllocated--;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
    _TyBaseAllocGraphNode::deallocate_type( _rpgn );
  }

  __DGRAPH_STATIC_ALLOC_DECL _TyGraphLink * _allocate_link( )
  {
    _TyGraphLink * pgl = _TyBaseAllocGraphLink::allocate_type();
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iLinksAllocated++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
    return pgl;
  }
  __DGRAPH_STATIC_ALLOC_DECL void _deallocate_link( _TyGraphLink * _rpgl )
  {
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iLinksAllocated--;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
    _TyBaseAllocGraphLink::deallocate_type( _rpgl );
  }

// construction of objects:
  static void _construct_node( _TyGraphNode * _pgn )
  {
    _pgn->Init();
    _construct_node_el( _pgn );
  }
  template < class t_TyP1 >
  static void _construct_node1( _TyGraphNode * _pgn, 
                                t_TyP1 _p1 )
  {
    _pgn->Init();
    _TyThis::template _construct_node_el1< t_TyP1 >( _pgn, _p1 );
  }
  template < class t_TyP1, class t_TyP2 >
  static void _construct_node2( _TyGraphNode * _pgn, 
                                t_TyP1 _p1, t_TyP2 _p2 )
  {
    _pgn->Init();
    _TyThis::template _construct_node_el2< t_TyP1, t_TyP2 >( _pgn, _p1, _p2 );
  }

  static void _construct_node_el( _TyGraphNode * _pgn )
  {
    __THROWPT( e_ttMemory );
    new( &( _pgn->RElObject() ) ) _TyNodeEl();
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iNodesConstructed++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
  }
  template < class t_TyP1 >
  static void _construct_node_el1(  _TyGraphNode * _pgn, 
                                    t_TyP1 _p1 )
  {
    __THROWPT( e_ttMemory );
    new( &( _pgn->RElObject() ) ) _TyNodeEl( _p1 );
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iNodesConstructed++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
  }
  template < class t_TyP1, class t_TyP2 >
  static void _construct_node_el2(  _TyGraphNode * _pgn, 
                                    t_TyP1 _p1, t_TyP2 _p2 )
  {
    __THROWPT( e_ttMemory );
    new( &( _pgn->RElObject() ) ) _TyNodeEl( _p1, _p2 );
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iNodesConstructed++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
  }

  static void _construct_link( _TyGraphLink * _pgl )
  {
    _pgl->Init();
    _construct_link_el( _pgl );
  }
  template < class t_TyP1 >
  static void _construct_link1( _TyGraphLink * _pgl, 
                                t_TyP1 _p1 )
  {
    _pgl->Init();
    _TyThis::template _construct_link_el1< t_TyP1 >( _pgl, _p1 );
  }
  template < class t_TyP1, class t_TyP2 >
  static void _construct_link2( _TyGraphLink * _pgl, 
                                t_TyP1 _p1, t_TyP2 _p2 )
  {
    _pgl->Init();
    _TyThis::template _construct_link_el2< t_TyP1, t_TyP2 >( _pgl, _p1, _p2 );
  }

  static void _construct_link_el( _TyGraphLink * _pgl )
  {
    __THROWPT( e_ttMemory );
    new( &( _pgl->RElObject() ) ) _TyLinkEl();
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iLinksConstructed++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
  }
  template < class t_TyP1 >
  static void _construct_link_el1(  _TyGraphLink * _pgl, 
                                    t_TyP1  _p1 )
  {
    __THROWPT( e_ttMemory );
    new( &( _pgl->RElObject() ) ) _TyLinkEl( _p1 );
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iLinksConstructed++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
  }

  template < class t_TyP1, class t_TyP2 >
  static void _construct_link_el2(  _TyGraphLink * _pgl, 
                                    t_TyP1 _p1, t_TyP2 _p2 )
  {
    __THROWPT( e_ttMemory );
    new( &( _pgl->RElObject() ) ) _TyLinkEl( _p1, _p2 );
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iLinksConstructed++;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
  }

  // destruction:
  static void _destruct_node( _TyGraphNode * _pgn )
  {
    _TyBaseGraph::_deinit_node( _pgn );
    _destruct_node_el( _pgn );
  }
  static void _destruct_node_el( _TyGraphNode * _pgn )
  {
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iNodesConstructed--;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
    _pgn->RElObject()._TyNodeEl::~_TyNodeEl();
  }

  static void _destruct_link( _TyGraphLink * _pgl )
  {
    _TyBaseGraph::_deinit_link( _pgl );
    _destruct_link_el( _pgl );
  }
  static void _destruct_link_el( _TyGraphLink * _pgl )
  {
#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
    gs_iLinksConstructed--;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME
    _pgl->RElObject()._TyLinkEl::~_TyLinkEl();
  }
};

__DGRAPH_END_NAMESPACE

#endif //__GRAPH_H
