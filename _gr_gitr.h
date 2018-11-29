#ifndef __GR_GITR_H
#define __GR_GITR_H

#include <functional>
#include <slist>
#include <hash_set>
#include <hash_map>

// _gr_gitr.h

// This module implements a graph forward iterator that allows iteration of the entire graph
//  in a defined order. This allows two graphs with the same node/link
//  configuration to be iterated correspondingly ( this allows comparison of elements
//  located at the same position in each graph, etc. )

#define __GR_GITR_INITSIZENODES _GR_HASH_INITSIZENODES
#define __GR_GITR_INITSIZELINKS _GR_HASH_INITSIZELINKS

__DGRAPH_BEGIN_NAMESPACE

struct _gfi_unfinished_node_hash_el
{
  int         m_iVisitOrder;
  _TyGNIndex  m_iRemainingLinks;
	_gfi_unfinished_node_hash_el()
		: m_iVisitOrder(0),
		m_iRemainingLinks(0)
	{
	}
};

// Use a most base class that does not include the allocator - this let's us get the compiler's
//  default structure copying mechanism:
// i.e. this should be a POD.
template <  class t_TyGraphNodeBase,
            class t_TyGraphLinkBase >
struct _graph_fwd_iter_most_base
{
private:
  typedef _graph_fwd_iter_most_base< t_TyGraphNodeBase, t_TyGraphLinkBase > _TyThis;
public:
  t_TyGraphNodeBase * m_pgnbCur;  // Current node.
  t_TyGraphLinkBase * m_pglbCur;  // Current link - may be null - if we are currently at the node.
  // Iteration options:
  bool  m_fClosedDirected;  // Iteration is closed and directed.
  bool  m_fDirectionDown;   // Current iteration direction is down.

  typedef bool ( _TyThis:: * _TyPMFnQueryIterLink )( t_TyGraphLinkBase * );
  // REVIEW: Does a pointer to member function make it a non-POD ?
  _TyPMFnQueryIterLink    m_pmfnQueryIterLink;

  bool  FAtEnd()
  {
    return !m_pgnbCur && !m_pglbCur;
  }
};

// Pull out the relevant objects that identify a unique point in the iteration.
// This let's us compare base_base's without all the trappings ( for one thing
//  this let's us a have simple (specialized) begin() and end() iterators.
// This also let's us pass an allocator from the graph during begin() and end().
template <  class t_TyGraphNodeBase,
            class t_TyGraphLinkBase,
            class t_TyAllocator >
struct _graph_fwd_iter_base_base
 :  public _graph_fwd_iter_most_base< t_TyGraphNodeBase, t_TyGraphLinkBase >,
    public _alloc_base< _gfi_unfinished_node_array_el< t_TyGraphNodeBase >, t_TyAllocator >
{
private:
  typedef _graph_fwd_iter_most_base< t_TyGraphNodeBase, t_TyGraphLinkBase >                 _TyBase;
  typedef _graph_fwd_iter_base_base< t_TyGraphNodeBase, t_TyGraphLinkBase, t_TyAllocator >  _TyThis;
protected:
  typedef _alloc_base< _gfi_unfinished_node_array_el< t_TyGraphNodeBase >, t_TyAllocator >  _TyAllocUNBase;
public:

  typedef t_TyGraphNodeBase   _TyGraphNodeBase;
  typedef t_TyGraphLinkBase   _TyGraphLinkBase;

  typedef t_TyAllocator                     _TyAllocatorAsPassed;
  typedef typename _TyAllocUNBase::_TyAllocatorType  _TyUnfinishedArrayAllocator;

  _graph_fwd_iter_base_base(  t_TyGraphNodeBase * _pgnbCur,
                              t_TyGraphLinkBase * _pglbCur,
                              bool _fClosedDirected,
                              bool _fDirectionDown,
                              t_TyAllocator const & _rAlloc,
                              bool ) _STLP_NOTHROW
    : _TyAllocUNBase( _rAlloc )
  {
    m_pgnbCur = _pgnbCur;
    m_pglbCur = _pglbCur;
    m_fClosedDirected = _fClosedDirected;
    m_fDirectionDown = _fDirectionDown;
  }

  _graph_fwd_iter_base_base( _TyThis const & _r, bool )  _STLP_NOTHROW
    : _TyAllocUNBase( _r.get_allocator() ),
      _TyBase( _r )
  {
  }

  bool  FInitialized() const _STLP_NOTHROW  { return true; }
  void  _Init() const _STLP_NOTHROW         { }
  
  t_TyGraphNodeBase * PGNBCur() const _STLP_NOTHROW { return m_pgnbCur; }
  t_TyGraphLinkBase * PGLBCur() const _STLP_NOTHROW { return m_pglbCur; }

  void SetPGNBCur( t_TyGraphNodeBase *  _pgnbCur ) _STLP_NOTHROW
  {
    m_pgnbCur = _pgnbCur;
  }
  void SetPGLBCur( t_TyGraphLinkBase *  _pglbCur ) _STLP_NOTHROW
  {
    m_pglbCur = _pglbCur;
  }

  // We are comparing positions not options.
  bool  operator == ( _TyThis const & _r ) const _STLP_NOTHROW
  {
    return m_pglbCur == _r.m_pglbCur && m_pgnbCur == _r.m_pgnbCur;
  }
};

template < class t_TyGraphEl >
struct _gr_select_always : public unary_function< t_TyGraphEl const *, bool >
{
  bool  operator () ( t_TyGraphEl const * ) const _STLP_NOTHROW
  {
    return true;
  }
};

template <  class t_TyGraphEl, 
            class t_TyCompare = equal_to< typename t_TyGraphEl::_TyElement > >
struct _gr_select_value : public unary_function< t_TyGraphEl const *, bool >
{
private:
  typedef _gr_select_value< t_TyGraphEl, t_TyCompare >  _TyThis;
public:

  typedef t_TyCompare                       _TyCompare;
  typedef typename t_TyGraphEl::_TyElement  _TyValue;

  _TyValue const &  m_rv;
  _TyCompare        m_comp;

  _gr_select_value( _TyValue const &  _rv,
                    _TyCompare const & _comp = _TyCompare() )
    : m_rv( _rv ),
      m_comp( _comp )
  {
  }
  _gr_select_value( _TyThis const & _r )
    : m_rv( _r.m_rv ),
      m_comp( _r.m_comp )
  {
  }

  bool  operator () ( t_TyGraphEl const * _pge ) const _STLP_NOTHROW
  {
    return m_comp( _pge->RElConst(), m_rv );
  }
};

template <  class t_TyGraphEl, 
            class t_TyCompare = equal_to< typename t_TyGraphEl::_TyElement > >
struct _gr_select_value_modifiable : public unary_function< t_TyGraphEl const *, bool >
{
private:
  typedef _gr_select_value_modifiable< t_TyGraphEl, t_TyCompare > _TyThis;
public:

  typedef t_TyCompare                       _TyCompare;
  typedef typename t_TyGraphEl::_TyElement  _TyValue;

  _TyCompare      m_comp;
  _TyValue        m_v;

  _gr_select_value_modifiable(  _TyValue const &  _rv,
                                _TyCompare const & _comp = _TyCompare() )
    : m_v( _rv ),
      m_comp( _comp )
  {
  }
  _gr_select_value_modifiable( _TyThis const & _r )
    : m_v( _r.m_v ),
      m_comp( _r.m_comp )
  {
  }

  _TyThis & operator = ( _TyThis const & _r )
  {
    m_v = _r.m_v;
    m_comp = _r.m_comp;
    return *this;
  }

  bool  operator () ( t_TyGraphEl const * _pge ) const _STLP_NOTHROW
  {
    return m_comp( _pge->RElConst(), m_v );
  }
};

template <  class t_TyGraphNodeBase, 
            class t_TyGraphLinkBase, 
            class t_TyAllocator,
            bool  t_fControlledLinkIteration >
struct _graph_fwd_iter_base
  : public _graph_fwd_iter_base_base< t_TyGraphNodeBase, t_TyGraphLinkBase, t_TyAllocator >
{
private:
  typedef _graph_fwd_iter_base_base<  t_TyGraphNodeBase, 
                                      t_TyGraphLinkBase, t_TyAllocator >    _TyBase;
  typedef _graph_fwd_iter_base< t_TyGraphNodeBase, t_TyGraphLinkBase, 
                                t_TyAllocator, t_fControlledLinkIteration > _TyThis;
public:

  typedef _TyBase _TyFwdIterBaseBase;

  bool  m_fInitialized; // This is in case we throw during initialization.
                        // Without this flag we could get into an undefined state
                        // if exceptions thrown during initialization.                           

  t_TyGraphNodeBase * m_pgnbIterationRoot; // The root of the iteration.

  int m_iCurVisitOrder; // The current visit order for unfinished nodes.

// Use a hash table for both node and link lookup.
// Need only one node hash table - the context list also maintains .
// Need two link hash tables - one for each direction - this allows reasonable
//  management of links ( removal of a single node from a hash is not necessarily
//  constant time - whereas wholesale clear()-ing is constant time per node ).

  typedef struct _gfi_unfinished_node_hash_el   _TyUnfinishedNodeHashEl;

  typedef hash_map< t_TyGraphNodeBase*, _TyUnfinishedNodeHashEl, _gr_hash_ptr< t_TyGraphNodeBase* >,
                    equal_to< t_TyGraphNodeBase* >, t_TyAllocator > _TyUnfinishedNodes;
  static const typename _TyUnfinishedNodes::size_type ms_stInitSizeNodes = __GR_GITR_INITSIZENODES;
  typedef typename _TyUnfinishedNodes::iterator    _TyUNIter;
  typedef typename _TyUnfinishedNodes::value_type  _TyUNValType;

  _TyUnfinishedNodes  m_nodesUnfinished;
  bool m_fNodeToBeRemoved; // throw-safe state management.
  _TyUNIter m_itNodeToBeRemoved;
  _TyUNIter m_itNodeLastInserted; // A pointer to the node last inserted - this is only valid after one has been
                                  // This is useful for some algorithms - it allows them to return a bogus
                                  // remaining link count - which it will update later. Particularly for
                                  // output to a stream since we want the output to occur on the transition
                                  //  from one state to the next ( whereas this iterator is modelled to change
                                  //  all state on the transition to a state ).

  typedef hash_set< t_TyGraphLinkBase*, _gr_hash_ptr< t_TyGraphLinkBase* >,
                    equal_to< t_TyGraphLinkBase* >, t_TyAllocator > _TyVisitedLinks;
  static const typename _TyVisitedLinks::size_type ms_stInitSizeLinks = __GR_GITR_INITSIZELINKS;
  typedef typename _TyVisitedLinks::iterator       _TyVLIter;
  typedef typename _TyVisitedLinks::value_type     _TyVLValType;

  _TyVisitedLinks     m_linksVisitedDown;
  _TyVisitedLinks     m_linksVisitedUp;

// State maintenance:
// All we need for the context data structure is stack functionality - i.e. slist:
// ( to make this template a little smaller (code-wise) we could hard code the context list (C-style))
  typedef t_TyGraphLinkBase * _TyIterationCtxt;

  typedef slist< _TyIterationCtxt, t_TyAllocator >  _TyContexts;

  typedef typename _TyContexts::iterator    _TyContextIter;
  typedef typename _TyContexts::value_type  _TyContextValType;

  _TyContexts     m_contexts;
  int             m_iContexts;

  // Unfinished node array - this maintained as a simple array:
  typedef _gfi_unfinished_node_array_el< t_TyGraphNodeBase >  _TyUnfinishedNodeArrayEl;
  _TyUnfinishedNodeArrayEl *  m_punStart;
  _TyUnfinishedNodeArrayEl *  m_punEnd;
  _TyUnfinishedNodeArrayEl *  m_punCur;

  // comparison object for sort:
  typedef less< _TyUnfinishedNodeArrayEl >    _TyCompareVisitedTime;

  // Notification callbacks:
  // The way it works is this ( no reason for it necessarily to be this way - but
  //  I thought it was kind of cool - seemed a bit faster in the general case - i.e.
  //  we generally won't be using these callbacks - a little cheaper then a virtual 
  //  (perhaps) - since we have no virtuals currently ):
  // 1) These are pointers to member functions of this class.
  // 2) They are initialized to zero in the constructors.
  // 3) They are copied in the copy constructor.
  // 4) If they are non-zero then they are called at some critical point during
  //    the iteration. ( i.e. critical to some algorithm )
  // 5) If a notification callback throws then it is assumed that the state should
  //    not change ( it did not change completely ) and it is reverted.
  // 6) To use the callback you derive from this class and delare a non-virtual
  //    method with the same signature as the callback of interest. Then
  //    set the callback by using a static_cast<> of the derived member function
  //    pointer to the base class type. This is a safe cast as long as there are
  //    no virtual base classes involved ( this is a guess ).

  // This method is called whenever an unfinished node is encountered.
  // If this is a new unfinished node then the this notification is to return the 
  //  number of relations of the new unfinished node in the opposite direction of the 
  //  current iteration. ( It could also return something bogus (>=1) and update the count
  //  later - this is perfectly fine for the algorithm ).
  // It can of course ( why would it exist otherwise ) do other things during this
  //  notification ( one thing it shouldn't do is modify the structure of the graph ).
  // If this is an old unfinished node return value is ignored.
  typedef _TyGNIndex ( _TyThis:: * _TyPMFnNotifyUnfinished )( bool _fNew, t_TyGraphNodeBase *, t_TyGraphLinkBase * );
  _TyPMFnNotifyUnfinished m_pmfnNotifyUnfinished;

// methods:

  _graph_fwd_iter_base( t_TyGraphNodeBase * _pgnbCur,
                        t_TyGraphLinkBase * _pglbCur,
                        bool _fClosedDirected,
                        bool _fDirectionDown,
                        t_TyAllocator const & _rAlloc,
                        bool _fInit = true )
    : _TyBase( _pgnbCur, _pglbCur, _fClosedDirected, _fDirectionDown, _rAlloc, _fInit ),
      m_fInitialized( false ),
      m_nodesUnfinished( ms_stInitSizeNodes, 
                         typename _TyUnfinishedNodes::hasher(), 
                         typename _TyUnfinishedNodes::key_equal(), 
                         _rAlloc ),
      m_linksVisitedDown( ms_stInitSizeLinks, 
                         typename _TyVisitedLinks::hasher(), 
                         typename _TyVisitedLinks::key_equal(), 
                         _rAlloc ),
      m_linksVisitedUp( ms_stInitSizeLinks, 
                        typename _TyVisitedLinks::hasher(), 
                        typename _TyVisitedLinks::key_equal(), 
                        _rAlloc ),
      m_contexts( _rAlloc ),
      m_punStart( 0 ),
      m_pmfnNotifyUnfinished( 0 )
  {
    __THROWPT( e_ttMemory );
    if ( _fInit && ( m_pgnbCur || m_pglbCur ) )
    {
      _Init();
    }
  }

  // Expensive constructor - copies all state - allows iteration from current point until end.
  explicit _graph_fwd_iter_base( _TyThis const & _r, bool _fInit = true )
    : _TyBase( _r ),
      m_fInitialized( _r.m_fInitialized ),  // If we throw during constuction the whole object goes away.
      m_iCurVisitOrder( _r.m_iCurVisitOrder ),
      m_nodesUnfinished( _r.m_nodesUnfinished ),
      m_linksVisitedDown( _r.m_linksVisitedDown ),
      m_linksVisitedUp( _r.m_linksVisitedUp ),
      m_contexts( _r.m_contexts ),
      m_iContexts( _r.m_iContexts ),
      m_punStart( 0 ),
      m_pmfnNotifyUnfinished( _r.m_pmfnNotifyUnfinished )
  {
    __THROWPT( e_ttMemory );

    // Copy the unfinished node array - even if we are at the end:
    // REVIEW: All we really need to copy is the remaining to be processed unfinished nodes ( >=1 ).
    //          We could use this as a minor optimization.
    if ( _r.m_punStart )
    {
      _CopyNodeArray( _r );
    }

    if ( !m_fInitialized && _fInit && ( m_pgnbCur || m_pglbCur ) )
    {
      _Init();
    }
  }

  // Allow initialization with the base - this allows
  //  statements such as git = graph.begin() - letting begin() be typed as _TyBase.
  // We can keep explicit since we know the typed iterator will be using it:
  // REVIEW: Could move {m_fClosedDirected} and {m_fDirectionDown} to base - this would allow
  //  the base to contain all the configurable iteration options.
  explicit _graph_fwd_iter_base( _TyBase const & _r, bool _fInit = true )
    : _TyBase( _r ),
      m_fInitialized( false ),
      m_nodesUnfinished( ms_stInitSizeNodes, 
                         typename _TyUnfinishedNodes::hasher(), 
                         typename _TyUnfinishedNodes::key_equal(), 
                         _r.get_allocator() ),
      m_linksVisitedDown( ms_stInitSizeLinks, 
                         typename _TyVisitedLinks::hasher(), 
                         typename _TyVisitedLinks::key_equal(), 
                         _r.get_allocator() ),
      m_linksVisitedUp( ms_stInitSizeLinks, 
                        typename _TyVisitedLinks::hasher(), 
                        typename _TyVisitedLinks::key_equal(), 
                        _r.get_allocator() ),
      m_contexts( _r.get_allocator() ),
      m_punStart( 0 ),
      m_pmfnNotifyUnfinished( 0 )
  {
    __THROWPT( e_ttMemory );
    if ( _fInit && ( m_pgnbCur || m_pglbCur ) )
    {
      _Init();
    }
  }

  ~_graph_fwd_iter_base() _STLP_NOTHROW
  {
    if ( m_punStart )
    {
      _ClearNodeArray();
    }
  }

  bool FInitialized() const _STLP_NOTHROW
  {
    return m_fInitialized;
  }

  // Return if we are at the beginning of an iteration:
  bool  FAtBegin()
  {
    return !m_fInitialized || ( !m_punStart && !m_iContexts && m_pgnbCur && !m_pglbCur );
  }

  // This should only before the iteration is begun:
  void  SetDirection( bool _fDirectionDown )
  {
    assert( FAtBegin() );
    m_fInitialized = false;
    m_fDirectionDown = _fDirectionDown;
    _Init();
  }

  pair< t_TyGraphLinkBase*, t_TyGraphNodeBase* > PairCurObjs() const _STLP_NOTHROW
  {
    // Return the current object(s) to the caller:
    return pair< t_TyGraphLinkBase*, t_TyGraphNodeBase* >( m_pgnbCur, m_pglbCur );
  }

  // Two types of assignment - assignment with full blown iterator ( us ) - 
  //  which copies state ( expensive ).
  _TyThis &
  operator = ( _TyThis const & _r )
  {
    // we become unitialized in case we throw during copying of state:
    m_fInitialized = false;
    ((_TyBase)(*this)) = _r;

    if ( _r.m_fInitialized )
    {
      m_iCurVisitOrder = _r.m_iCurVisitOrder;
      m_nodesUnfinished = _r.m_nodesUnfinished;
      m_linksVisitedDown = _r.m_linksVisitedDown;
      m_linksVisitedUp = _r.m_linksVisitedUp;
      m_contexts = _r.m_contexts;
      m_iContexts = _r.m_iContexts;

      if ( m_punStart )
      {
        _ClearNodeArray();
        m_punStart = 0;
      }

      if ( _r.m_punStart )
      {
        _CopyNodeArray( _r );
      }
      else
      {
        m_punStart = 0;
      }
      m_fInitialized = true;
    }
    else
    {
      if ( m_pgnbCur || m_pglbCur )
      {
        _Init();
      }
    }
  }
    
  // Assignment with base class - this re-initializes this iterator:
  _TyThis &
  operator = ( _TyBase const & _r )
  {
    m_fInitialized = false;
    ((_TyBase&)*this) =  _r;
    if ( m_pgnbCur || m_pglbCur )
    {
      _Init();
    }
    return *this;
  }

  void  Reset()
  {
    m_fInitialized = false;
    if ( m_pgnbCur || m_pglbCur )
    {
      _Init();
    }
  }

  // Don't go above/below the current element on this path.
  // Better know the circumstances when making this call.
  void  SkipContext()
  {
    _NextContext();
  }
  
protected:

  void  _ClearState() _STLP_NOTHROW
  {
    m_iCurVisitOrder = 0;
    m_nodesUnfinished.clear();
    m_linksVisitedDown.clear();
    m_linksVisitedUp.clear();
    m_contexts.clear();
    m_iContexts = 0;
    if ( m_punStart )
    {
      _ClearNodeArray();
      m_punStart = 0;
    }
  }

  // Initialize the iteration.
  void  _Init()
  {
    assert( PGNBCur() );  // TODO: Allow initialization with a node OR a link - this would
    assert( !PGLBCur() ); //  start the iteration at that link.
    assert( !m_fInitialized );  // Should currently be uninitialized.

    // Ensure have no state before adding any:
    _ClearState();

    // If this node has any relations in the opposite direction of the current iteration then
    //  we need to add it to the unfinished nodes map:
    _TyUnfinishedNodeHashEl unh;
    if ( PGNBCur()->FRelations( !m_fDirectionDown ) )
    {
      if ( !m_pmfnNotifyUnfinished )
      {
        // Generally the function pointer will be NULL.
        if ( t_fControlledLinkIteration )
        {
          // Then need to count the matching links:
          for ( _TyGraphLinkBase ** ppglb = PGNBCur()->PPGLBRelationHead( !m_fDirectionDown );
                *ppglb;
                ppglb = (*ppglb)->PPGLBGetNextRelation( !m_fDirectionDown ) )
          {
            if ( (this->*m_pmfnQueryIterLink)( *ppglb ) )
            {
              ++unh.m_iRemainingLinks;
            }
          }
        }
        else
        {
          unh.m_iRemainingLinks = PGNBCur()->URelations( !m_fDirectionDown );
        }
      }
      else
      {
        // Let this method do some stuff and return the number of relations in the
        //  opposite direction.
        unh.m_iRemainingLinks = (this->*m_pmfnNotifyUnfinished)( true, PGNBCur(), 0 );
      }

      if ( !t_fControlledLinkIteration || unh.m_iRemainingLinks )
      {
        unh.m_iVisitOrder = m_iCurVisitOrder++;
        _TyUNValType  vtUN( PGNBCur(), unh );
        m_itNodeLastInserted = _PIBInsertNode( vtUN ).first;
      }
    }

    m_pgnbIterationRoot = PGNBCur();

    m_fInitialized = true;
  }

  void  _CopyNodeArray( _TyThis const & _r )
  {
    __THROWPT( e_ttMemory );
    assert( !m_punStart );  // for throw safety.
    typename _TyUnfinishedArrayAllocator::size_type stN = _r.m_punEnd - _r.m_punStart;
    assert( stN );
    _TyBase::allocate_n( m_punStart, stN );// throws.
    m_punEnd = m_punStart + stN;
    m_punCur = m_punStart + ( _r.m_punCur - _r.m_punStart );
  }
  void  _ClearNodeArray() _STLP_NOTHROW
  {
    if ( m_punStart )
    {
      _TyBase::deallocate_n( m_punStart, m_punEnd - m_punStart );
    }
  }

  pair< _TyUNIter, bool > _PIBInsertNode( _TyUNValType const & _rvt )
  {
    __THROWPT( e_ttMemory );
    return m_nodesUnfinished.insert( _rvt );
  }

  void _RemoveNode( _TyUNIter const & _rit ) _STLP_NOTHROW
  {
    m_nodesUnfinished.erase( _rit );
  }

  bool  _FAtUnfinishedNode() const _STLP_NOTHROW 
  { 
    if ( !m_iContexts && m_punStart )
    {
      // we could be along a single strand from an unfinished node - if so then
      // we are not at an unfinished node:
      if ( PGLBCur() )
      {
        // Then if the relation is the current
        return PGLBCur()->PGNBRelation( !m_fDirectionDown ) == (m_punCur-1)->m_pgnbUnfinished;
      }
      else
      {
        return false;// Never at an unfinished node when we have a current node.
      }
    }
    return false;
  }

  _TyVLIter _ITInsertLink( t_TyGraphLinkBase * _pglb )
  {
    __THROWPT( e_ttMemory );
    // We are inserting in the opposite direction of the current iteration:

    pair< _TyVLIter, bool > pib;
    // _RVLGet( !m_fDirectionDown ).insert( _pglb )
    // CHANGE: <dbien>: Avoid using a reference as this aliases the lookup
    //  ( had a crash in the optimized version ).
    if ( m_fDirectionDown )
    {
      pib = m_linksVisitedUp.insert( _pglb );
    }
    else
    {
      pib = m_linksVisitedDown.insert( _pglb );
    }
    assert( pib.second );
    return pib.first;
  }
  void  _RemoveLink( _TyVLIter const & _rit ) _STLP_NOTHROW
  {
    // Remove in the opposite direction:
    // _RVLGet( !m_fDirectionDown ).erase( _rit ); avoid aliasing
    if ( m_fDirectionDown )
    {
      m_linksVisitedUp.erase( _rit );
    }
    else
    {
      m_linksVisitedDown.erase( _rit );
    }
  }

  bool  _FVisitedLink( t_TyGraphLinkBase * _pglb ) _STLP_NOTHROW
  {
    // If we are doing a controlled iteration then must first check if we are to
    //  iterate this link:
    if ( t_fControlledLinkIteration )
    {
      if ( !(this->*m_pmfnQueryIterLink)( _pglb ) )
      {
        return true;  // Indicate that we have already been here.
      }
    }

    // We only search for visited links in the same direction as the current iteration:
    if ( m_fDirectionDown )
    {
      _TyVLIter vlit = m_linksVisitedDown.find( _pglb );
      return vlit != m_linksVisitedDown.end();
    }
    else
    {
      _TyVLIter vlit = m_linksVisitedUp.find( _pglb );
      return vlit != m_linksVisitedUp.end();
    }
  }
  void  _ClearVisitedLinks() _STLP_NOTHROW
  {
    // We are finished processing unfinished nodes in one direction - no longer need
    //  the visited links:
    if ( m_fDirectionDown )
    {
      m_linksVisitedUp.clear();
    }
    else
    {
      m_linksVisitedDown.clear();
    }
  }

  t_TyGraphLinkBase * _PGLBFindUnvisitedLink( t_TyGraphLinkBase * _pglb ) _STLP_NOTHROW
  {
    for( ; _pglb && _FVisitedLink( _pglb ); 
          _pglb = _pglb->PGLBGetNextRelation( m_fDirectionDown ) )
    {
    }
    return _pglb;
  }

  void  _PushContext( t_TyGraphLinkBase * _pglb )
  {
    __THROWPT( e_ttMemory );
    // Push the passed link onto top of context stack:
    m_contexts.push_front( _pglb );
    m_iContexts++;
  }

  // Move to the next graph element in the iteration:
  void  _Next();

  // We need to find the next context.
  // 1) Check the context stack - if not empty then pop it and that is our context.
  // 2) If !(1) then we check the unfinished node array - if any unfinished nodes left to be
  //    processed then will current context from there. If this array has transitioned to
  //    empty with this request then we need to empty the visited links hash ( in the current direction ).
  // 3) If !(2) then we check the unfinished node hash - if anything in there we transfer it to
  //    a new unfinished node array and empty the hash. Then we sort the unfinished node array by
  //    visit time. Set the current context to the first element in the result and increment the current pointer. 
  // 4) If !(4) then we are finished with the entire iteration - empty any visited nodes from the hashes - 
  //    set both graph element pointers to NULL.
  void
  _NextContext()
  {
    if ( m_contexts.empty() )
    {
      // To be throw-safe we must allocate all the new stuff before clearing
      //  any old node array - if we are to clear it then we save it in these locals:
      _TyUnfinishedNodeArrayEl *  punStartDealloc = 0;
      _TyUnfinishedNodeArrayEl *  punEndDealloc;

      if ( m_punStart )
      {
        // Then we have some unfinished nodes - if we are transitioning to the end then
        //  do some data structure maintenance:
        if ( m_punCur == m_punEnd )
        {
          // Clear the current unfinished node array:
          punStartDealloc = m_punStart;
          punEndDealloc = m_punEnd;
          m_punStart = 0;
        }
        else
        {
          // we have an unfinished node left to process - find the first unvisited link
          //  from this node - then increment {m_punCur}:
          t_TyGraphLinkBase * pglbUnvisited = 
            _PGLBFindUnvisitedLink( *( m_punCur->m_pgnbUnfinished->PPGLBRelationHead( m_fDirectionDown ) ) );
          assert( pglbUnvisited );  // Otherwise why is this node in the unfinished array.
          SetPGNBCur( m_punCur->m_pgnbUnfinished );
          SetPGLBCur( pglbUnvisited );
          m_punCur++; // We could clear the unfinished array at this point if we were at the end - 
                      //  but we still would need the visited link hash - and then we wouldn't have the
                      //  transition from m_punStart!=0 -> m_punStart==0 to decide to clear everything.
          // ACTUALLY SHOULD ALWAYS KEEP m_punCur when at m_punEnd - we backtrack in _FAtUnfinishedNode().
          return;
        }
      }

      assert( !m_punStart );
      
      // If we are doing a closed-directed iteration then we are done.
      // Otherwise check the hash for unfinished nodes:
      if ( m_fClosedDirected || 
           ( !!m_nodesUnfinished.size() == m_fNodeToBeRemoved ) )
      {
        if ( m_fClosedDirected )
        {
          // REVIEW: The state saved in {m_nodesUnfinished} could be used
          //  by another closed directed iteration in the same direction, starting, for instance
          //  at an alternate root of the graph - this would even allow multiple
          //  iterations in the same direction starting at different roots - but with
          //  the property that every node and link is only iterated once.
          //  One has to be knowledgable about the structure of the graph in these
          //  special usage situations. Thus we could add an option ( m_fSaveData, 
          //  for instance ) that would keep all the data structures in the appropriate
          //  configuration for multiple iterations in the same direction ( which is to
          //  say - change nothing. ) But for now we merely...
          // Empty all the data structures:
          m_fNodeToBeRemoved = false;
          m_nodesUnfinished.clear();
          m_linksVisitedUp.clear();
          m_linksVisitedDown.clear();
        }
        // Nothing left to do - we are at the end:
        SetPGNBCur( 0 );
        SetPGLBCur( 0 );
      }
      else
      {
        _STLP_TRY
        {
          // Unfinished nodes left to process in the opposite direction:
          _ProcessUnfinishedNodes();
        }
        // Restore state on throw - allows correct transition to occur next time:
        _STLP_UNWIND( ( ( m_punStart = punStartDealloc ), ( m_punEnd = punEndDealloc ) ) );
        // NO THROWING AFTER THIS.
      }

      // Now we can clear anything we need to:
      if ( punStartDealloc )
      {
        _TyBase::deallocate_n( punStartDealloc, punEndDealloc - punStartDealloc );
        // Clear the visited link hash in the opposite direction ( we have changed directions -
        // in _ProcessUnfinishedNodes() ).
        _ClearVisitedLinks();
      }
    }
    else
    {
      // We have contexts left in this direction:
      t_TyGraphLinkBase * pglbContext = m_contexts.front();
      assert( pglbContext );
      SetPGLBCur( pglbContext );
      SetPGNBCur( pglbContext->PGNBRelation( !m_fDirectionDown ) );
      m_contexts.pop_front();
      m_iContexts--;
    }
  }

  void
  _ProcessUnfinishedNodes()
  {
    assert( !m_punStart );
    assert( m_nodesUnfinished.size() > ( m_fNodeToBeRemoved ? 1 : 0 ) );

    // We have nodes in the opposite direction to process - they are in {m_nodesUnfinished}.
    // Allocate an array of _TyUnfinishedNodeArrayEl objects and fill with the hash - then
    //  sort by visit time.
    allocate_n( m_punStart, m_nodesUnfinished.size() - ( m_fNodeToBeRemoved ? 1 : 0 ) ); // throws.
    _ProcessUnfinishedNodesNoThrow();
  }

  void _ProcessUnfinishedNodesNoThrow() _STLP_NOTHROW
  {
    // NO THROWING WITHIN THIS METHOD - loss of state.
    m_punEnd = m_punStart + m_nodesUnfinished.size() - m_fNodeToBeRemoved;
    m_punCur = m_punStart+1;  // We will process the first here.

    // If we were to remove a node - remove it now and indicate that the node no
    //  longer needs to be removed:
    if ( m_fNodeToBeRemoved )
      {
        m_nodesUnfinished.erase( m_itNodeToBeRemoved );
        m_fNodeToBeRemoved = false;
      }

    // Copy the info from the hash to the array:
    _TyUnfinishedNodeArrayEl *  punCur = m_punStart;
    for ( _TyUNIter itHash = m_nodesUnfinished.begin();
          ( m_nodesUnfinished.end() != itHash );
          ++itHash, ++punCur )
    {
      _TyUNValType &  rvtHash = *itHash;
      punCur->m_pgnbUnfinished = rvtHash.first;
      punCur->m_iVisitOrder = rvtHash.second.m_iVisitOrder;
    }

    // Now sort by visit time:
    sort( m_punStart, m_punEnd, _TyCompareVisitedTime() );

    // Change the direction of the iteration:
    m_fDirectionDown = !m_fDirectionDown;
    // Clear the unfinished node hash:
    m_nodesUnfinished.clear();

    // Now find an unvisited link on this node:
    t_TyGraphLinkBase * pglbUnvisited = 
      _PGLBFindUnvisitedLink( *( m_punStart->m_pgnbUnfinished->PPGLBRelationHead( m_fDirectionDown ) ) );
    assert( pglbUnvisited );  // Otherwise why is this node in the unfinished array.
    SetPGNBCur( m_punStart->m_pgnbUnfinished );
    SetPGLBCur( pglbUnvisited );
  }

};

// out-of-lines:
 // Move to the next graph element in the iteration:
template <  class t_TyGraphNodeBase, 
            class t_TyGraphLinkBase, 
            class t_TyAllocator,
            bool  t_fControlledLinkIteration >
void  
_graph_fwd_iter_base< t_TyGraphNodeBase, t_TyGraphLinkBase, 
                      t_TyAllocator,
                      t_fControlledLinkIteration >::_Next()
{
  if ( !m_fInitialized )
  {
    if ( m_pgnbCur || m_pglbCur )
    {
      _Init();
    }
    else
    {
      assert( 0 );  // We are at the end ( and have never been initialized ).
      return;
    }
  }

  if ( PGLBCur() )
  {
    // Find the next link to be visited from this node - this is either the next
    //  iteration point or a context to push on the context stack:
    t_TyGraphLinkBase * pglbNextRel = PGLBCur()->PGLBGetNextRelation( m_fDirectionDown );
    if ( t_fControlledLinkIteration )
    {
      // Then need to move to the next link of interest:
      while( pglbNextRel && !(this->*m_pmfnQueryIterLink)( pglbNextRel ) )
      {
        pglbNextRel = pglbNextRel->PGLBGetNextRelation( m_fDirectionDown );
      }
    }

    if ( _FAtUnfinishedNode() )
    {
      pglbNextRel = _PGLBFindUnvisitedLink( pglbNextRel );
    }
    // else we know that we have not iterated this link ( even if we have iterated to the node beyond ).

    // See if we are connected to a node ( we will iterate disconnected links ):
    t_TyGraphNodeBase * pgnbNextNode = PGLBCur()->PGNBRelation( m_fDirectionDown );
    if ( pgnbNextNode )
    {
      // If this node has more than one relation in the opposite direction ( PGNBCur() ) then
      //  we need to check if it is in the unfinished node map ( or put it there ):
      // Special case: If this node is the root of the iteration then this optimization on lookup
      //  does not apply.
      t_TyGraphLinkBase * pglbOppRels = 
        *(pgnbNextNode->PPGLBRelationHead( !m_fDirectionDown ));

      pair< _TyUNIter, bool > pibUnfinished;
      if ( t_fControlledLinkIteration )
      {
        // Then it is perhaps fastest to look up the node in the hash immediately -
        //  if not found then we need to check if more than one link in this 
        //  direction satisfies the query.
        pibUnfinished.first = m_nodesUnfinished.find( pgnbNextNode );
        if ( m_nodesUnfinished.end() != pibUnfinished.first )
        {
          pibUnfinished.second = false; // Found the node ( don't look-up below ).
        }
        else
        {
          assert( pgnbNextNode != m_pgnbIterationRoot );

          int iFound = 0;
          do
          {
            if (  ( pglbOppRels == PGLBCur() ) || 
                  ( (this->*m_pmfnQueryIterLink)( pglbOppRels ) ) )
            {
              ++iFound;
            }
          }
          while( pglbOppRels = pglbOppRels->PGLBGetNextRelation( !m_fDirectionDown ) );
          
          if ( iFound > 1 )
          {
            pglbOppRels = *(pgnbNextNode->PPGLBRelationHead( !m_fDirectionDown ));
            _TyUNValType vtUnfinished( pgnbNextNode, _TyUnfinishedNodeHashEl() );
            vtUnfinished.second.m_iRemainingLinks = iFound-1;
            pibUnfinished = _PIBInsertNode( vtUnfinished );
            assert( pibUnfinished.second );
          }
        }
      }
      else
      {
        if ( pgnbNextNode != m_pgnbIterationRoot )
        {
          pglbOppRels = pglbOppRels->PGLBGetNextRelation( !m_fDirectionDown );
        }
      }

      _TyUNValType *  pvtUN;
      _TyVLIter vlitInserted;
      if ( pglbOppRels )
      {
        // Then we have come to an unfinished node:
        if ( !t_fControlledLinkIteration )  // otherwise inserted above.
        {
          _TyUNValType vtUnfinished( pgnbNextNode, _TyUnfinishedNodeHashEl() );
          pibUnfinished = _PIBInsertNode( vtUnfinished ); // throws - but no state changed yet.
        }
        pvtUN = &(*pibUnfinished.first);
        if ( pibUnfinished.second )
        {
          // New unfinished node:
          m_itNodeLastInserted = pibUnfinished.first; // Save for caller.
          pvtUN->second.m_iVisitOrder = m_iCurVisitOrder++;
          if ( !m_pmfnNotifyUnfinished )
          {
            if ( !t_fControlledLinkIteration )// otherwise set above.
            {
              pvtUN->second.m_iRemainingLinks = pglbOppRels->UCountRelations( !m_fDirectionDown );
            }
          }
          else
          {
            _STLP_TRY // We can't assume that this method doesn't throw.
            {
              // NOTE: In order to be throw-safe, the derived object ( which supplied 
              //  m_pmfnNotifyUnfinished - and is calling this method ) must handle
              //  state appropriately on throw - particularly there is no mechanism to inform
              //  the derived class that our state did not change - except that we will throw
              //  through the caller ( whatever was thrown to us ).
              // Let this method do some stuff and return the number of relations in the
              //  opposite direction.
              pvtUN->second.m_iRemainingLinks = (this->*m_pmfnNotifyUnfinished)( true, pgnbNextNode, PGLBCur() );
            }
            _STLP_UNWIND( _RemoveNode( pibUnfinished.first ) ); // Restore state and re-throw.
          }
          assert( pvtUN->second.m_iRemainingLinks );
        }
        else
        {
          if ( m_pmfnNotifyUnfinished )
          {
            // Notify the derived that we have encountered an old unfinished node:
            (void)(this->*m_pmfnNotifyUnfinished)( false, pgnbNextNode, PGLBCur() );
          }
          // We have been to this unfinished node - decrement the remaining links - 
          // we will check below ( after any throwing can occur ) and remove the node if
          //  no remaining links:
          --pvtUN->second.m_iRemainingLinks;
        }

        // If we have any remaining links on this unfinished node we need to insert the link:
        if ( pvtUN->second.m_iRemainingLinks )
        {
          m_fNodeToBeRemoved = false;
          _STLP_TRY
          {
            // If this throws then we will be in an inconsistent state - unless we remove any new 
            //  unfinished node:
            vlitInserted = _ITInsertLink( PGLBCur() );  // throws.
          }
#ifdef __STL_USE_EXCEPTIONS
          catch( ... )
          {
            if ( pibUnfinished.second )
            {
              m_iCurVisitOrder--; // We incremented this above.
              assert( pibUnfinished.first->second.m_iVisitOrder == m_iCurVisitOrder );
              _RemoveNode( pibUnfinished.first );
            }
            else
            {
              // Then this is an old node - we decremented the remaining links above:
              pvtUN->second.m_iRemainingLinks++;
            }
            throw;
          }
#endif __STL_USE_EXCEPTIONS
        }
        else
        {
          // We will remove the node below - indicate this in the current state - this ensure's state
          //  consistency on throw:
          m_fNodeToBeRemoved = true;
          m_itNodeToBeRemoved = pibUnfinished.first;
        }
      }

      if ( !pglbOppRels || pibUnfinished.second )
      {
        // Then we are at a node we have not yet visited - this is the new iteration point.
        // If we are moving from a node that has remaining links to visit then we need to
        //  save the context - this requires that we find the next context:
        if ( pglbNextRel )
        {
          // push the next context on the context stack.
          _STLP_TRY
          {
            _PushContext( pglbNextRel );  // throws - if we throw maintain current state.
          }
#ifdef __STL_USE_EXCEPTIONS
          catch( ... )
          {
            // We may have added a link and a node - remove it so to maintain state:
            if ( pglbOppRels )
            {
              _RemoveLink( vlitInserted );
              if ( pibUnfinished.second )
                {
                  _RemoveNode( pibUnfinished.first );
                }
            }
            throw;
          }
#endif __STL_USE_EXCEPTIONS
        }
        SetPGNBCur( pgnbNextNode );
        SetPGLBCur( 0 );
      }
      else
      {
        // we have already visited the node below - need to move to the next link or context:
        if ( pglbNextRel )
        {
          SetPGLBCur( pglbNextRel );
        }
        else
        {
          _STLP_TRY
          {
            _NextContext(); // throws.
          }
#ifdef __STL_USE_EXCEPTIONS
          catch( ... )
          {
            // We may have added a link to the visited map - remove to maintain state:
            if ( pglbOppRels )
            {
              if ( pvtUN->second.m_iRemainingLinks )
              {
                _RemoveLink( vlitInserted );
              }
              pvtUN->second.m_iRemainingLinks++;  // we decremented the remaining links.
            }
            throw;
          }
#endif __STL_USE_EXCEPTIONS
        }

        if ( m_fNodeToBeRemoved )
        {
          // No remaining links - remove the unfinished node from the hash:
          _RemoveNode( pibUnfinished.first ); // shouldn't throw - if it does we will be left in an inconsistent state
        }
      }
    }
    else
    {
      // no node below move to next link or context:
      if ( pglbNextRel )
      {
        SetPGLBCur( pglbNextRel );
      }
      else
      {
        m_fNodeToBeRemoved = false;
        _NextContext(); // throws - but we have altered no state yet.
      }
    }
  }
  else
  if ( PGNBCur() )
  {
    // Go to the first link from this node - if none then call _NextContext().
    t_TyGraphLinkBase * pglbFirst = *( PGNBCur()->PPGLBRelationHead( m_fDirectionDown ) );
    if ( t_fControlledLinkIteration )
    {
      // Then need to move to the next link of interest:
      while( pglbFirst && !(this->*m_pmfnQueryIterLink)( pglbFirst ) )
      {
        pglbFirst = pglbFirst->PGLBGetNextRelation( m_fDirectionDown );
      }
    }

    if ( pglbFirst )
    {
      SetPGLBCur( pglbFirst );
    }
    else
    {
      m_fNodeToBeRemoved = false;
      _NextContext(); // throws - but we have altered no state yet.
    }
  }
  else
  {
    assert( 0 );  // Attempt to iterate beyond end().
  }
}

// Provide const-qualified types for const vs. non-const version.
// non-const version:
template < class t_TyGraphNode, class t_TyGraphLink, class t_TyFIsConstIterator >
struct _graph_iter_const_base
{
  typedef t_TyFIsConstIterator  _TyFIsConstIterator;

  typedef t_TyGraphNode _TyGraphNode;
  typedef t_TyGraphLink _TyGraphLink;
  typedef typename t_TyGraphNode::_TyGraphNodeBaseBase  _TyGraphNodeBaseBase;
  typedef typename t_TyGraphLink::_TyGraphLinkBaseBase  _TyGraphLinkBaseBase;

  typedef t_TyGraphNode _TyGraphNodeCQ;
  typedef t_TyGraphLink _TyGraphLinkCQ;
  typedef typename t_TyGraphNode::_TyNodeEl _TyNodeElCQ;
  typedef typename t_TyGraphNode::_TyLinkEl _TyLinkElCQ;

  typedef _TyNodeElCQ & node_reference;
  typedef _TyLinkElCQ & link_reference;
  typedef _TyNodeElCQ * node_pointer;
  typedef _TyLinkElCQ * link_pointer;
};

template < class t_TyGraphNode, class t_TyGraphLink >
struct _graph_iter_const_base< t_TyGraphNode, t_TyGraphLink, __true_type >
{
  typedef __true_type _TyFIsConstIterator;

  typedef t_TyGraphNode _TyGraphNode;
  typedef t_TyGraphLink _TyGraphLink;
  typedef typename t_TyGraphNode::_TyGraphNodeBaseBase  _TyGraphNodeBaseBase;
  typedef typename t_TyGraphLink::_TyGraphLinkBaseBase  _TyGraphLinkBaseBase;

  typedef const t_TyGraphNode _TyGraphNodeCQ;
  typedef const t_TyGraphLink _TyGraphLinkCQ;
  typedef const typename t_TyGraphNode::_TyNodeEl _TyNodeElCQ;
  typedef const typename t_TyGraphNode::_TyLinkEl _TyLinkElCQ;

  typedef _TyNodeElCQ & node_reference;
  typedef _TyLinkElCQ & link_reference;
  typedef _TyNodeElCQ * node_pointer;
  typedef _TyLinkElCQ * link_pointer;
};

template <  class t_TyGraphNode, class t_TyGraphLink,
            class t_TyBaseClass, class t_TyFIsConstIterator,
            bool  t_fControlledLinkIteration = false,
            class t_TyLinkSelect = _gr_select_always< t_TyGraphLink > >
struct _graph_iter
  : public t_TyBaseClass,
    public _graph_iter_const_base<  t_TyGraphNode, 
                                    t_TyGraphLink, 
                                    t_TyFIsConstIterator >
{
private:
  typedef t_TyBaseClass                                       _TyBase;
  typedef _graph_iter_const_base<	t_TyGraphNode,
	  t_TyGraphLink,
	  t_TyFIsConstIterator >	_tyConstIterBase;
  typedef _graph_iter<  t_TyGraphNode, t_TyGraphLink, 
                        t_TyBaseClass, t_TyFIsConstIterator, 
                        t_fControlledLinkIteration,
                        t_TyLinkSelect >                      _TyThis;
public:

  typedef typename _tyConstIterBase::node_reference node_reference;
  typedef typename _tyConstIterBase::link_reference link_reference;
  typedef typename _tyConstIterBase::node_pointer node_pointer;
  typedef typename _tyConstIterBase::link_pointer link_pointer;
  typedef typename _tyConstIterBase::_TyGraphNodeCQ _TyGraphNodeCQ;
  typedef typename _tyConstIterBase::_TyGraphLinkCQ _TyGraphLinkCQ;
  typedef typename _tyConstIterBase::_TyGraphNode _TyGraphNode;
  typedef typename _tyConstIterBase::_TyGraphLink _TyGraphLink;
  typedef typename _tyConstIterBase::_TyGraphNodeBaseBase  _TyGraphNodeBaseBase;
  typedef typename _tyConstIterBase::_TyGraphLinkBaseBase  _TyGraphLinkBaseBase;
  typedef t_TyLinkSelect  _TyLinkSelect;

  typedef _graph_iter<  t_TyGraphNode, t_TyGraphLink, 
                        t_TyBaseClass, __false_type,
                        t_fControlledLinkIteration, t_TyLinkSelect >    iterator;
  typedef _graph_iter<  t_TyGraphNode, t_TyGraphLink, 
                        t_TyBaseClass, __true_type,
                        t_fControlledLinkIteration, t_TyLinkSelect >    const_iterator;

  _TyLinkSelect   m_lsSelectLink;

  _graph_iter(  _TyGraphNodeCQ * _pgn, 
                _TyGraphLinkCQ * _pgl, 
                bool _fClosedDirected,
                bool _fDirectionDown,
                typename _TyBase::_TyAllocatorAsPassed const & _rAlloc = 
                  typename _TyBase::_TyAllocatorAsPassed(),
                _TyLinkSelect const & _rlsSelectLink = _TyLinkSelect() )
    : _TyBase( const_cast< _TyGraphNode* >( _pgn ), 
               const_cast< _TyGraphLink* >( _pgl ), 
               _fClosedDirected, _fDirectionDown, _rAlloc, 
               !t_fControlledLinkIteration ),
      m_lsSelectLink( _rlsSelectLink )
  {
    if ( t_fControlledLinkIteration )
    {
      m_pmfnQueryIterLink = static_cast< typename _TyBase::_TyPMFnQueryIterLink >( &_TyThis::QueryLink );
      if ( m_pgnbCur || m_pglbCur )
      {
        _Init();
      }
    }
  }

#if 0 // This doesn't work under MSVC so we will rearchitect.
  // Allow initialization with either const or non-const self - but check whether transfer
  //  of const-ness is ok.  
  template < class t__TyBaseClass, class t__TyFIsConst >
  _graph_iter( _graph_iter< typename t__TyBaseClass::_TyGraphNode, typename t__TyBaseClass::_TyGraphLink,
                            t__TyBaseClass, t__TyFIsConst,
                            t_fControlledLinkIteration, t_TyLinkSelect > const & _r )
    : _TyBase( _r, !t_fControlledLinkIteration ),
      m_lsSelectLink( _r.m_lsSelectLink )
  {
    __TRANSFER_CONST( t__TyFIsConst, t_TyFIsConstIterator );

    if ( t_fControlledLinkIteration )
    {
      m_pmfnQueryIterLink = static_cast< typename _TyBase::_TyPMFnQueryIterLink >( &_TyThis::QueryLink );
      if ( !FInitialized() && ( m_pgnbCur || m_pglbCur ) )
      {
        _Init();
      }
    }
  }
#else 0
	template < class t_tyGraphIter >
	_graph_iter( t_tyGraphIter const & _r )
		: _TyBase(_r, !t_fControlledLinkIteration),
			m_lsSelectLink(_r.m_lsSelectLink)
	{
		__TRANSFER_CONST( typename t_tyGraphIter::_TyFIsConstIterator, t_TyFIsConstIterator );

		if (t_fControlledLinkIteration)
		{
			m_pmfnQueryIterLink = static_cast<typename _TyBase::_TyPMFnQueryIterLink>(&_TyThis::QueryLink);
			if (!FInitialized() && (m_pgnbCur || m_pglbCur))
			{
				_Init();
			}
		}
	}
#endif 0

#if 0 // This is not working under MSVC.
	// Special initialization allows graph to transform const->non-const. 
  // ( doesn't check const transfer ).
  template < class t__TyBaseClass, class t__TyFIsConst >
  _graph_iter(  _graph_iter<  typename t__TyBaseClass::_TyGraphNode, typename t__TyBaseClass::_TyGraphLink,
                              t__TyBaseClass, t__TyFIsConst,
                              t_fControlledLinkIteration, t_TyLinkSelect > const & _r,
                __gr_const_cast_iter )
    : _TyBase( _r, !t_fControlledLinkIteration ),
      m_lsSelectLink( _r.m_lsSelectLink )
  {
    if ( t_fControlledLinkIteration )
    {
      m_pmfnQueryIterLink = static_cast< typename _TyBase::_TyPMFnQueryIterLink >( &_TyThis::QueryLink );
      if ( !FInitialized() && ( m_pgnbCur || m_pglbCur ) )
      {
        _Init();
      }
    }
  }
#else 0
	// Special initialization allows graph to transform const->non-const. 
	// ( doesn't check const transfer ).
	template < class t_TyGraphIter >
	_graph_iter(t_TyGraphIter const & _r,
		__gr_const_cast_iter)
		: _TyBase(_r, !t_fControlledLinkIteration),
			m_lsSelectLink(_r.m_lsSelectLink)
	{
		if (t_fControlledLinkIteration)
		{
			m_pmfnQueryIterLink = static_cast<typename _TyBase::_TyPMFnQueryIterLink>(&_TyThis::QueryLink);
			if (!FInitialized() && (m_pgnbCur || m_pglbCur))
			{
				_Init();
			}
		}
	}
#endif 0
  bool  QueryLink( typename _tyConstIterBase::_TyGraphLinkBaseBase * _pglb )
  {
    return m_lsSelectLink( static_cast< _TyGraphLink * >( _pglb ) );
  }

  _TyGraphNodeCQ *  PGNCur() const _STLP_NOTHROW
  {
    return const_cast< _TyGraphNodeCQ * >( static_cast< _TyGraphNode * >( PGNBCur() ) );
  }
  _TyGraphLinkCQ *  PGLCur() const _STLP_NOTHROW
  {
    return const_cast< _TyGraphLinkCQ * >( static_cast< _TyGraphLink * >( PGLBCur() ) );
  }

  // note: may not has a node ( may be at end of iteration ).
  node_reference    RNodeEl() const _STLP_NOTHROW
  {
    return const_cast< node_reference >( *static_cast< _TyGraphNode * >( PGNBCur() ) );
  }
  // note: may not have a link!
  link_reference    RLinkEl() const _STLP_NOTHROW
  {
    return const_cast< link_reference >( *static_cast< _TyGraphLink * >( PGLBCur() ) );
  }

  // The way this works: if the link_pointer is non-null then the iteration is currently
  //  at a link. Otherwise the iteration either at node_pointer or at the end() if node_pointer null.
  pair< link_pointer, node_pointer >  PairCur() const _STLP_NOTHROW
  {
    return pair< link_pointer, node_pointer >( PGLBCur() ? &RLinkEl() : 0, PGNBCur() ? &RNodeEl() : 0 );
  }

  // Allow comparison with either const or non-const self:
  template < class t__TyBaseClass, class t__TyFIsConst >
  bool
  operator == ( _graph_iter<  _TyGraphNode, _TyGraphLink, 
                  t__TyBaseClass, t__TyFIsConst,
                  t_fControlledLinkIteration, t_TyLinkSelect > const & _r ) const _STLP_NOTHROW
  {
    return _TyBase::operator == ( _r );
  }

  // Allow assignment between various versions of self - must check const transfer:
  template < class t__TyBaseClass, class t__TyFIsConst >
  _TyThis &
  operator = ( _graph_iter< _TyGraphNode, _TyGraphLink, 
                  t__TyBaseClass, t__TyFIsConst,
                  t_fControlledLinkIteration, t_TyLinkSelect > const & _r )
  {
    _SetEqual( _r );
    return static_cast< _TyThis & >( _TyBase::operator = ( _r ) );
  }

  // Allow assignment between various versions of self - must check const transfer:
  template < class t__TyBaseClass, class t__TyFIsConst >
  void
  _SetEqual( _graph_iter< _TyGraphNode, _TyGraphLink, 
                  t__TyBaseClass, t__TyFIsConst,
                  t_fControlledLinkIteration, t_TyLinkSelect > const & _r )
  {
    __TRANSFER_CONST( t__TyFIsConst, t_TyFIsConstIterator );
    if ( t_fControlledLinkIteration )
    {
      m_lsSelectLink = _r.m_lsSelectLink;
    }
    _TyBase::operator = ( _r );
  }

  // We only implement pre-increment - since copying the iterator is non-trivial.
  // This will throw OOM exceptions.
  _TyThis & operator ++()
  {
    _Next();  // Call base class method - an error here indicates you are attempting 
              //  to iterate a position - rather than an iterator.
    return *this;
  }
};

__DGRAPH_END_NAMESPACE

#endif __GR_GITR_H
