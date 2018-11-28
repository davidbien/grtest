#ifndef __GR_INPT_H
#define __GR_INPT_H

// _gr_inpt.h

// This module implements an input iterator for graphs.

#include <stdio.h>

#define __GR_INPT_INITSIZENODES _GR_HASH_INITSIZENODES
#define __GR_INPT_INITSIZELINKS _GR_HASH_INITSIZELINKS

//#define __GR_DONTTHROWBADGRAPHERRORS
  // Hook to disable the throwing of graph read errors.
  // Would have to propagate errors if we defined this - 
  //  currently using exceptions for error propagation.
  // Seems to make sense when reading streams.

#if defined( __STL_USE_EXCEPTIONS )
//    !defined( __GR_DONTTHROWBADGRAPHERRORS )
#define __GR_THROWBADGRAPHSTREAMERRORS
#else
#error Currently no stream error propagation at this point.
#endif

#ifdef __GR_THROWBADGRAPHSTREAMERRORS
// We use the SGI exception stuff:
#include <stdexcept>

__DGRAPH_BEGIN_NAMESPACE

class bad_graph_stream : public _t__Named_exception< __DGRAPH_DEFAULT_ALLOCATOR >
{
  typedef _t__Named_exception< __DGRAPH_DEFAULT_ALLOCATOR > _TyBase;
public:
  bad_graph_stream( const string_type & __s ) : _TyBase( __s ) {}
};

#else __GR_THROWBADGRAPHSTREAMERRORS
#error This not currently supported. Need error propagation - not too tough, but...
#endif __GR_THROWBADGRAPHSTREAMERRORS

// This object implements those operations that need have no knowledge of the full type specifications
//  of the graph elements.
template <  class t_TyInputStreamBase,
            class t_TyGraphBase,
            class t_TyAllocator = typename t_TyGraphBase::_TyPathNodeBaseAllocatorAsPassed,
            bool t_fUseSeek = true,   // In case our input stream does not support _Tell()/_Seek().
            // Setting this to true allows the writer to write unconstucted links
            //  if it is false and the writer encounters an unconstucted link it will throw
            //  a bad_graph exception.
            bool t_fAllowUnconstructedLinks = false >
struct _graph_input_iter_base
  : public _alloc_base< _gfi_unfinished_node_with_remaining< typename t_TyGraphBase::_TyGraphNodeBase >, 
                        t_TyAllocator >
{
private:
  typedef _graph_input_iter_base< t_TyInputStreamBase, t_TyGraphBase, 
                                  t_TyAllocator, t_fUseSeek, 
                                  t_fAllowUnconstructedLinks >        _TyThis;
public:
  typedef typename t_TyGraphBase::_TyGraphNodeBase _TyGraphNodeBase;
  typedef typename t_TyGraphBase::_TyGraphLinkBase _TyGraphLinkBase;
protected:
  typedef _alloc_base< _gfi_unfinished_node_with_remaining< _TyGraphNodeBase >, t_TyAllocator > _TyAllocUNBase;
public:

  typedef typename t_TyInputStreamBase::_TyGraphNodeBaseReadPtr _TyGraphNodeBaseReadPtr;
  typedef typename t_TyInputStreamBase::_TyGraphLinkBaseReadPtr _TyGraphLinkBaseReadPtr;

  typedef t_TyAllocator _TyAllocatorAsPassed;

  // Use the same lookup objects ( type-wise ) as the copy module - this keeps the code smaller.
  // NOTE: Since we are reading pointers from the file ( generally we don't use them as pointers -
  //  we just use them as unique names - but we could use them as pointers for some very interesting
  //  operations - like having a shadow of a graph that goes in the opposite direction as the parent
  //  graph - linked to it via the pointers ).
  typedef _gc_unconnected_node< _TyGraphNodeBase >  _TyUnfinishedNode;
  typedef _TyGraphLinkBase *                        _TyUnfinishedLink;

#ifdef __GR_DSIN_USEHASH
  typedef hash_map< _TyGraphNodeBaseReadPtr, _TyUnfinishedNode, _gr_hash_ptr< _TyGraphNodeBaseReadPtr >,
                    equal_to< _TyGraphNodeBaseReadPtr >, t_TyAllocator > _TyUnfinishedNodes;
  typedef hash_map< _TyGraphLinkBaseReadPtr, _TyUnfinishedLink, _gr_hash_ptr< _TyGraphLinkBaseReadPtr >,
                    equal_to< _TyGraphLinkBaseReadPtr >, t_TyAllocator > _TyUnfinishedLinks;
  static const _TyUnfinishedNodes::size_type ms_stInitSizeNodes = __GR_INPT_INITSIZENODES;
  static const _TyUnfinishedLinks::size_type ms_stInitSizeLinks = __GR_INPT_INITSIZELINKS;
#else __GR_DSIN_USEHASH
  typedef map<  _TyGraphNodeBaseReadPtr, _TyUnfinishedNode, 
                less< _TyGraphNodeBaseReadPtr >, t_TyAllocator > _TyUnfinishedNodes;
  typedef map<  _TyGraphLinkBaseReadPtr, _TyUnfinishedLink, 
                less< _TyGraphLinkBaseReadPtr >, t_TyAllocator > _TyUnfinishedLinks;
#endif __GR_DSIN_USEHASH

  typedef typename _TyUnfinishedNodes::value_type _TyUNValueType;
  typedef typename _TyUnfinishedNodes::iterator   _TyUNIterator;
  typedef typename _TyUnfinishedLinks::value_type _TyULValueType;
  typedef typename _TyUnfinishedLinks::iterator   _TyULIterator;

  _TyUnfinishedNodes  m_nodesUnfinished;
  _TyUnfinishedLinks  m_linksUnfinishedDown;
  _TyUnfinishedLinks  m_linksUnfinishedUp;

  // This slist shared with the forward iterator ( _gr_gitr.h ) code:
  typedef _TyGraphLinkBase *                        _TyIterationCtxt;
  typedef slist< _TyIterationCtxt, t_TyAllocator >  _TyContexts;
  typedef typename _TyContexts::iterator            _TyContextIter;
  typedef typename _TyContexts::value_type          _TyContextValType;

  _TyContexts         m_contexts;
  int                 m_iContexts;
  _TyGraphLinkBase *  m_pglbPopContext;

  // Unfinished node array - this maintained as a simple array:
  typedef _gfi_unfinished_node_with_remaining< _TyGraphNodeBase > _TyUnfinishedNodeArrayEl;
  _TyUnfinishedNodeArrayEl *  m_punStart;
  _TyUnfinishedNodeArrayEl *  m_punEnd;
  _TyUnfinishedNodeArrayEl *  m_punCur;

  // comparison object for sort:
  typedef less< _TyUnfinishedNodeArrayEl >    _TyCompareVisitedTime;

  // Input stream object:
  typedef typename t_TyInputStreamBase::_TyStreamPos  _TyStreamPos;
  t_TyInputStreamBase & m_ris;

  // Option - we can construct the graph in reverse ( i.e. parent-wise instead of child-wise
  //  and vice versa ):
  bool    m_fConstructInReverse;

  bool  m_fDirectionSet; // Has the direction been set yet ?
  bool    m_fDirectionDown; // Current construction direction.

  int     m_iCurVisitOrder; // The number of the current visited unfinished node.

  bool m_fProcessedGraphFooter;// Processed graph footer ?

  // Derived object must set these function pointers by static_cast<>ing 
  //  member functions of itself to the types defined here:
  // ( I don't believe that virtual base classes can be in between ).
  // allocate/dealloc nodes/links:
  typedef void  ( _TyThis :: * _TyPMFnAllocInitNode )( _TyGraphNodeBase *& );
  typedef void  ( _TyThis :: * _TyPMFnDeallocateNode )( _TyGraphNodeBase * );
  _TyPMFnAllocInitNode  m_pmfnAllocInitNode;  // Must allocate and Init().
  _TyPMFnDeallocateNode m_pmfnDeallocateNode;

  typedef void  ( _TyThis :: * _TyPMFnAllocInitLink )( _TyGraphLinkBase *& );
  typedef void  ( _TyThis :: * _TyPMFnDeallocateLink )( _TyGraphLinkBase * );
  _TyPMFnAllocInitLink  m_pmfnAllocInitLink;  // Must allocate and Init().
  _TyPMFnDeallocateLink m_pmfnDeallocateLink;

  // Read and construct nodes/links:
  typedef void ( _TyThis :: * _TyPMFnReadNode )( _TyGraphNodeBase * );
  _TyPMFnReadNode   m_pmfnReadNode;

  typedef void ( _TyThis :: * _TyPMFnReadLink )( _TyGraphLinkBase * );
  _TyPMFnReadLink   m_pmfnReadLink;

  // Call the destructor on the link's element:
  typedef void ( _TyThis :: * _TyPMFnDestructLinkEl )( _TyGraphLinkBase * );
  _TyPMFnDestructLinkEl m_pmfnDestructLinkEl;

  // Destroy the given sub-graph:
  typedef void ( _TyThis :: * _TyPMFnDestroySubGraph )( _TyGraphNodeBase * );
  _TyPMFnDestroySubGraph m_pmfnDestroySubGraph;

  // NOTE: It is up the derived classes ( the one that knows about the full types )
  //  to destroy these objects if non-NULL in the destructor - in the reverse order of
  //  their declaration here.
  _TyGraphNodeBase *  m_pgnbNewRoot;  // The root of the read tree ( the first element read ).
  _TyGraphNodeBase *  m_pgnbTempRoot; // This is maintained for throw-safety - a partially constructed sub-graph.
  _TyGraphLinkBase *  m_pglbAllocedInited;  // An allocated and initialized link ( not constructed )
  _TyGraphLinkBase *  m_pglbConstructedEl;  // This is a constructed graph link that is not owned by
                                          //  anything since it is missing a relation on one side.
                                          // This maintains throw-safety. The element in this link
                                          //  needs to be destroyed.

  // These should not be destroyed - they are status:
  _TyGraphNodeBase *  m_pgnbCur;  // The current link and node.
  _TyGraphLinkBase *  m_pglbCur;

  // These contain transient state - so as to not have to be stored by sub-objects:
  // Their contents are not guaranteed to be valid.
  // There state is managed by {m_ris}.
  _TyGraphNodeBaseReadPtr m_pgnbrNode;
  _TyGraphLinkBaseReadPtr m_pglbrLink;


  // Throw-safety and file validity stuff - we need to ensure the unique-ness of link names.
  bool          m_fThrowWhileInsertingLinksLast;
  bool          m_fThrowWhileInsertingLinks;
  _TyGNIndex    m_iLinksInsertedBeforeThrowLast;
  _TyGNIndex    m_iLinksInsertedBeforeThrow;


  // to seek or not to seek
  void  _Seek( _TyStreamPos _sp )
  {
    if ( t_fUseSeek )
    {
      m_ris._Seek( _sp );
    }
  }
  void  _Tell( _TyStreamPos & _sp )
  {
    if ( t_fUseSeek )
    {
      _sp = m_ris._Tell( );
    }
  }

  explicit _graph_input_iter_base(  t_TyInputStreamBase & _ris,
                                    t_TyAllocator const & _rAlloc = t_TyAllocator(),
                                    bool _fConstructInReverse = false ) :
    _TyAllocUNBase( _rAlloc ),
    m_ris( _ris ),
    m_fConstructInReverse( _fConstructInReverse ),
#ifdef __GR_DSIN_USEHASH
    m_nodesUnfinished( ms_stInitSizeNodes, 
                       typename _TyUnfinishedNodes::hasher(), 
                       typename _TyUnfinishedNodes::key_equal(), 
                       _rAlloc ),
    m_linksUnfinishedDown( ms_stInitSizeLinks, 
                           typename _TyUnfinishedLinks::hasher(), 
                           typename _TyUnfinishedLinks::key_equal(), 
                           _rAlloc ),
    m_linksUnfinishedUp( ms_stInitSizeLinks, 
                         typename _TyUnfinishedLinks::hasher(), 
                         typename _TyUnfinishedLinks::key_equal(), 
                         _rAlloc ),
#else __GR_DSIN_USEHASH
    m_nodesUnfinished( typename _TyUnfinishedNodes::key_compare(), 
                       _rAlloc ),
    m_linksUnfinishedDown(  typename _TyUnfinishedLinks::key_compare(), 
                            _rAlloc ),
    m_linksUnfinishedUp(  typename _TyUnfinishedLinks::key_compare(), 
                          _rAlloc ),
#endif __GR_DSIN_USEHASH
    m_pgnbNewRoot( 0 ),
    m_pgnbTempRoot( 0 ),
    m_pglbConstructedEl( 0 ),
    m_pglbAllocedInited( 0 ),
    m_pgnbCur( 0 ),
    m_pglbCur( 0 ),
    m_iContexts( 0 ),
    m_pglbPopContext( 0 ),
    m_punStart( 0 ),
    m_fDirectionDown( true ),
    m_fDirectionSet( false ),
    m_iCurVisitOrder( 0 ),
    m_fProcessedGraphFooter( false ),
    m_fThrowWhileInsertingLinks( false ),
    m_iLinksInsertedBeforeThrow( 0 ),
    m_iLinksInsertedBeforeThrowLast( 0 )  // This needs initialization - since 
  {
    __THROWPT( e_ttMemory );
  }

  ~_graph_input_iter_base()
  {
    _ClearNodeArray();
  }
  
  bool FAtBeg() const __STL_NOTHROW
    {
      return !PGNBCur() && !PGLBCur() && !m_fProcessedGraphFooter;
    }
  bool FAtEnd() const __STL_NOTHROW
    {
      return !PGNBCur() && !PGLBCur() && m_fProcessedGraphFooter;
    }

  _TyGraphNodeBase *  PGNBCur() const __STL_NOTHROW { return m_pgnbCur; }
  _TyGraphLinkBase *  PGLBCur() const __STL_NOTHROW { return m_pglbCur; }

protected:

  void SetPGNBCur( _TyGraphNodeBase * _pgnb ) __STL_NOTHROW
    {
      m_pgnbCur = _pgnb;
    }
  void SetPGLBCur( _TyGraphLinkBase * _pglb ) __STL_NOTHROW
    {
      m_pglbCur = _pglb;
    }

  void _ClearNodeArray() __STL_NOTHROW
    {
      if ( m_punStart )
      {
        _TyAllocUNBase::deallocate_n( m_punStart, m_punEnd - m_punStart );
      }
    }

  _TyUnfinishedLinks &  RULGet( bool _fDirectionDown ) __STL_NOTHROW
  {
    return _fDirectionDown ? m_linksUnfinishedDown : m_linksUnfinishedUp;
  }

  // This method does the work - read the next record - create the appropriate graph objects
  //   - throw on error.
  void  _Next()
  {
    // Save stream position for possible throw - this is also good for debugging - if we encounter
    //  an error in the stream we can go back and re-examine:
    _TyStreamPos  sp;
    _Tell( sp );

    __STL_TRY
    {
      // We check for both context and direction switches so that an input iterator can be kept
      //  "inline" with a forward iterator - this could allow some iteresting transformations:
      while ( _FReadOne() )
      {
        _Tell( sp ); // Each successful read of a context or direction advances the state.
      }
    }
    __STL_UNWIND( _Seek( sp ) );
  }

  bool  _FReadOne()
  {
    _TyGraphNodeBase *  pgnbSave;
    _TyGraphLinkBase *  pglbSave;
    if ( m_pglbPopContext )
    {
      // Then set the current link and node in case of throw:
      pgnbSave = PGNBCur();
      pglbSave = PGLBCur();
      SetPGNBCur( m_pglbPopContext->PGNBRelation( !m_fDirectionDown ) );
      SetPGLBCur( m_pglbPopContext );
    }
    __STL_TRY
    {
      typename _binary_rep_tokens< __false_type >::_TyToken uc;
      m_ris._ReadToken( &uc );

      switch( uc )
      {
        case  _binary_rep_tokens< __false_type >::ms_ucDirectionUp:
        case  _binary_rep_tokens< __false_type >::ms_ucDirectionDown:
        {
          _ChangeDirection( uc == _binary_rep_tokens< __false_type >::ms_ucDirectionDown );
          return true;
        }
        break;
        case  _binary_rep_tokens< __false_type >::ms_ucContextPush:
        case  _binary_rep_tokens< __false_type >::ms_ucContextPop:
        {
          _ChangeContext( uc == _binary_rep_tokens< __false_type >::ms_ucContextPush );
          return true;
        }
        break;

        case  _binary_rep_tokens< __false_type >::ms_ucNode:
        {
          _ReadNode();
        }
        break;

        case  _binary_rep_tokens< __false_type >::ms_ucUnfinishedNode:
        {
          _ReadUnfinishedNode();
        }
        break;

  #ifdef __GR_BINARY_WRITENODEFOOTER
        case  _binary_rep_tokens< __false_type >::ms_ucNodeFooter:
        {
          throw bad_graph_stream( "_FReadOne(): Encountered a node footer token at top level." );
        }
        break;
  #endif __GR_BINARY_WRITENODEFOOTER

        case  _binary_rep_tokens< __false_type >::ms_ucLink:
        {
          _ReadLink();
        }
        break;

        case  _binary_rep_tokens< __false_type >::ms_ucLinkFromUnfinished:
        {
          _ReadLinkFromUnfinished();
        }
        break;

        case  _binary_rep_tokens< __false_type >::ms_ucNormalLinkFooter:
        {
          throw bad_graph_stream( "_FReadOne(): Encountered a normal link footer token at top level." );
        }
        break;
        case  _binary_rep_tokens< __false_type >::ms_ucUnfinishedLinkFooter:
        {
          throw bad_graph_stream( "_FReadOne(): Encountered an unfinished link footer token at top level." );
        }
        break;
        case _binary_rep_tokens< __false_type >::ms_ucGraphFooter:
        {
          _ProcessGraphFooter();
        }
        break;
        default:
        {
          char  cpError[256];
          sprintf( cpError, "_FReadOne(): Encountered bogus token [%d] at top level.", uc );
          throw bad_graph_stream( cpError );
        }
        break;
      }

      m_pglbPopContext = 0; // Successful processed the context pop.
    }
    __STL_UNWIND( if ( m_pglbPopContext )
                  {
                    SetPGNBCur( pgnbSave );
                    SetPGLBCur( pglbSave );
                  } );

    return false;
  }

  // Change construction direction - this indicates that the next link should come from an unfinished node.
  void  _ChangeDirection( bool _fDirectionDown )
  {
    // Set the direction to the opposite of what it is now,
    //  we may be constructing in reverse:
    assert( ! m_fDirectionSet ||
            ( m_fConstructInReverse && ( _fDirectionDown == m_fDirectionDown ) ) ||
            ( !m_fConstructInReverse && ( _fDirectionDown == !m_fDirectionDown ) ) );

    if ( m_iContexts )
    {
      throw bad_graph_stream( "_ChangeDirection(): Have more than zero contexts." );
    }

    if ( m_punStart && ( m_punCur != m_punEnd ) )
    {
      throw bad_graph_stream( "_ChangeDirection(): We have not processed all the unfinished nodes in the current direction." );
    }

    if ( PGNBCur() )
    {
      _ProcessUnfinishedNodes();  // We need to process the unfinished nodes.
    }
    // else this is the initial direction indicator.
    m_fDirectionDown = m_fConstructInReverse ? !_fDirectionDown : _fDirectionDown;
    m_fDirectionSet = true;
  }

  void  _ProcessUnfinishedNodes()
  {
    // We had better have some unfinished nodes left:
    assert( m_nodesUnfinished.size() );
    // We should either not currently be processing unfinished nodes or 
    //  have finished copying the last unfinished node:
    assert( !m_punStart || ( m_punCur == m_punEnd ) );

    _ClearNodeArray();  // Clear any old unfinished nodes that we may have.
    m_punStart = 0;     // ts

    allocate_n( m_punStart, m_nodesUnfinished.size() ); // throws.

    _ProcessUnfinishedNodesNoThrow();
  }

  void _ProcessUnfinishedNodesNoThrow() __STL_NOTHROW
  {
    // NO THROWING WITHIN THIS METHOD - loss of state.
    m_punEnd = m_punStart + m_nodesUnfinished.size();
    m_punCur = m_punStart;

    // Copy the info from the hash to the array:
    _TyUnfinishedNodeArrayEl *  punCur = m_punStart;
    for ( _TyUNIterator itHash = m_nodesUnfinished.begin();
          ( m_nodesUnfinished.end() != itHash );
          ++itHash, ++punCur )
    {
      _TyUNValueType &  rvtHash = *itHash;
      assert( rvtHash.second.m_iRemainingUnmatched );
      // No need to save the unfinished node name - that is associated
      //  with each link that leaves it in the file. ( Though this too is
      //  unnecessary - except for error checking and for allowing some
      //  derived class manipulation using the pointers from the file ).
      punCur->m_iRemainingUnmatched = rvtHash.second.m_iRemainingUnmatched; // We maintain this so can test validity of file.
      punCur->m_pgnbUnfinished = rvtHash.second.m_pgnbDst;
      punCur->m_iVisitOrder = rvtHash.second.m_iVisitOrder;
    }

    // Now sort by visit time:
    sort( m_punStart, m_punEnd, _TyCompareVisitedTime() );

    // Clear the unfinished node hash:
    m_nodesUnfinished.clear();

    // Clear the link lookup in the opposite dir ( we haven't changed directions yet ):
    RULGet( !m_fDirectionDown ).clear();
  }

  // A context is either being pushed or popped.
  void  _ChangeContext( bool _fPush )
  {
    if ( m_pglbPopContext )
    {
      throw bad_graph_stream("_ChangeContext(): Just had a pop-context.");
    }

    if ( _fPush )
    {
      if ( !PGLBCur() )
      {
        throw bad_graph_stream( "_ChangeContext(): Don't have a link context to push.");
      }
      __THROWPT( e_ttMemory );
      m_contexts.push_front( PGLBCur() );
      m_iContexts++;
    }
    else
    {
      // Popping - set the current link and node:
      if ( !m_iContexts )
      {
        throw bad_graph_stream("_ChangeContext(): Pop from empty context stack.");
      }
      m_pglbPopContext = m_contexts.front();
      // Don't set the link and node now - this would change state if we threw:
      m_contexts.pop_front();
      m_iContexts--;
    }
  }

  void  _CheckNodeContext()
  {
    // We had better be at a node insertion point:
    assert( !m_pgnbNewRoot || PGLBCur() );

    // If we threw previously this may be non-null ( and should be destroyed ):
    if ( m_pgnbTempRoot )
    {
      (this->*m_pmfnDestroySubGraph)( m_pgnbTempRoot );
      m_pgnbTempRoot = 0;
    }

    if ( t_fAllowUnconstructedLinks )
    {
      assert( !m_pglbConstructedEl || PGLBCur() );
    }
    else
    {
      assert( m_pglbConstructedEl == PGLBCur() );
    }

    if ( m_pgnbNewRoot && !m_pglbConstructedEl )
    {
      if ( t_fAllowUnconstructedLinks && PGLBCur() )
      {
        // Then if this link is not constructed then it cannot have a node connected to it - 
        //  since it would then have a node on both sides - indicating that it should be constructed.
        throw bad_graph_stream( "_CheckNodeContext(): Node to be added to unconstructed link.\n");
      }
      else
      {
        assert( !PGLBCur() );
        // Then have two node records in a row ( likely ):
        throw bad_graph_stream( "_CheckNodeContext(): Encountered a normal node record when expecting a link.\n");
      }
    }
  }

  // Read a normal node from the stream:
  void  _ReadNode()
  {
    _CheckNodeContext();  // Check that it is ok to read a node now.

    // Before reading any more allocate the node:
    _TyGraphNodeBase *  pgnbNew;
    (this->*m_pmfnAllocInitNode)( pgnbNew );
    // Set up destruction object:
    CMFDtor1_void< _TyThis, _TyGraphNodeBase * >
      fcdDeallocNode( this, m_pmfnDeallocateNode, pgnbNew ); //ts

    m_ris._ReadNodeHeaderData( &m_pgnbrNode );  // May be information before the node element. // throws.

    // Once we have read the node successfully it has become a full blown
    //  node and must be destroyed by the graph:
    (this->*m_pmfnReadNode)( pgnbNew );
    fcdDeallocNode.Reset(); // No longer own the allocation.
    m_pgnbTempRoot = pgnbNew; // Until this get's linked to the current graph throw-safe(ts).

    // Now read any node footer before adding to the graph ( state safe ):
    m_ris._ReadNodeFooter();

    _InsertNewNodeNoThrow( pgnbNew, pgnbNew->PPGLBRelationHead( !m_fDirectionDown ) );
  }

  void _InsertNewNodeNoThrow( _TyGraphNodeBase * _pgnbNew,
                              _TyGraphLinkBase ** _ppglbPos ) __STL_NOTHROW
  {
    if ( !m_pgnbNewRoot )
    {
      assert( !PGLBCur() && !PGNBCur() ); 
      m_pgnbNewRoot = _pgnbNew;
    }
    else
    {
      // We will make this the relation of the current link:
      assert( !m_pglbConstructedEl->PGNBRelation( m_fDirectionDown ) );
      m_pglbConstructedEl->SetRelationNode( m_fDirectionDown, _pgnbNew );
      m_pglbConstructedEl->InsertRelation( !m_fDirectionDown, _ppglbPos );
      m_pglbConstructedEl = 0;  // The construction of the link now owned by the graph.
    }
    m_pgnbTempRoot = 0;

    SetPGNBCur( _pgnbNew );
    SetPGLBCur( 0 );
  }

  void _ReadUnfinishedNode()
  {
    _CheckNodeContext();  // Check that it is ok to read a node now.

    // Before reading any more allocate the node:
    _TyGraphNodeBase *  pgnbNew;
    (this->*m_pmfnAllocInitNode)( pgnbNew );
    // Set up destruction object:
    CMFDtor1_void< _TyThis, _TyGraphNodeBase * >
      fcdDeallocNode( this, m_pmfnDeallocateNode, pgnbNew ); //ts

    // Read the node and link names:
    _TyGraphLinkBaseReadPtr pglbrLink; // Don't save the link name.
    m_ris._ReadUnfinishedHeaderData( &m_pgnbrNode, &pglbrLink );

    // If we find a null link name and we are not at the first node then throw:
    // Likewise if we found a link name and we are at the first node then throw.
    if ( !!m_pgnbNewRoot == !pglbrLink )
    {
      if ( m_pgnbNewRoot )
      {
        throw bad_graph_stream( "_ReadUnfinishedNode(): Found null link name when one expected.");
      }
      else
      {
        throw bad_graph_stream( "_ReadUnfinishedNode(): Found link name when one not expected.");
      }
    }

    // Once we have read the node successfully it has become a full blown
    //  node and must be destroyed by the graph:
    (this->*m_pmfnReadNode)( pgnbNew );
    fcdDeallocNode.Reset(); // No longer own the allocation.
    m_pgnbTempRoot = pgnbNew; // Until this get's linked to the current graph throw-safe(ts).

    // We now read a null-terminated list of link names and allocate links ( but of course
    //  we can't construct - we don't have the data yet ):

    // throw-safety issue:
    //  We need to add these links to the link lookup - since we need to associate the link names
    // with the newly constructed links - if we throw we will not be able to restore the link lookup
    //  to its original state ( unless we use some type of _alloca() strategy ). In any case - it won't
    //  hurt much to have them in there - it's just that we might see them again ( when we re-insert 
    //  them - if we handle the exception ).
    m_fThrowWhileInsertingLinksLast = m_fThrowWhileInsertingLinks;
    m_fThrowWhileInsertingLinks = true;
    if ( m_fThrowWhileInsertingLinksLast )
    {
      m_iLinksInsertedBeforeThrowLast = max(  m_iLinksInsertedBeforeThrowLast, 
                                              m_iLinksInsertedBeforeThrow );
    }
    m_iLinksInsertedBeforeThrow = 0;
    // If we need to check whether a bad-graph file may name a link twice then we can enable
    //  the above statements in NDEBUG.
    // Well - it turns out we need to - otherwise we could have runaway link creation -
    //  which would kind of stink - but since a link name would likely repeat before too long
    //  now these statements will prevent this.

    _TyGraphLinkBase ** ppglbFoundCur = 0;  // This saves the position of PGLBCur() when found.
    // If not found then throw bad_graph_stream.

    _TyGraphLinkBaseReadPtr pglbrRead;
    m_ris._ReadLinkName( &pglbrRead );

    for ( _TyGraphLinkBase ** ppglbCurPos = pgnbNew->PPGLBRelationHead( !m_fDirectionDown );
          !!pglbrRead;  // This lets the type override operator!().
        )
    {
      if ( pglbrRead == pglbrLink )
      {
        if ( ppglbFoundCur )
        {
          throw bad_graph_stream( "_ReadUnfinishedNode(): Found the connecting link more than once in relation list." );
        }
        else
        {
          ppglbFoundCur = ppglbCurPos;
        }
      }
      else
      {
        // Allocate a new link:
        _TyGraphLinkBase *  pglbNew;
        (this->*m_pmfnAllocInitLink)( pglbNew );
        // Set up deallocation object:
        CMFDtor1_void< _TyThis, _TyGraphLinkBase * >
          fcdDeallocLink( this, m_pmfnDeallocateLink, pglbNew ); //ts

        // Add this link to the link lookup:
        _TyULValueType ulInsert( pglbrRead, pglbNew );
        __THROWPT( e_ttMemory );
        pair< _TyULIterator, bool > pib = RULGet( m_fDirectionDown ).insert( ulInsert );// throws.
        m_iLinksInsertedBeforeThrow++;  // We have registered the link.

        if ( !pib.second )
        {
          // Link already in the lookup - must check if it happened last time:
          if (  !m_fThrowWhileInsertingLinksLast ||
                ( m_iLinksInsertedBeforeThrow > m_iLinksInsertedBeforeThrowLast ) )
          {
            // Then a true duplicate - throw an error:
            throw bad_graph_stream( "_ReadUnfinishedNode(): Found a duplicate link name in relation list." );
          }
          else
          {
            // We threw last time - update the link name to the currently allocated:
            pib.first->second = pglbNew;
          }
        }
        
        // Now link the link onto the node and transfer ownership from the deallocation object:
        pglbNew->SetRelationNode( m_fDirectionDown, pgnbNew );
        pglbNew->SetRelationNode( !m_fDirectionDown, 0 ); // This node doesn't exist yet ( and may never ).
        pglbNew->InsertRelation( !m_fDirectionDown, ppglbCurPos );
        fcdDeallocLink.Reset();

        // Move to the next node in the list:
        ppglbCurPos = (*ppglbCurPos)->PPGLBGetNextRelation( !m_fDirectionDown );
      }

      m_ris._ReadLinkName( &pglbrRead ); // read another name.
    }

    // Now if we were expected to find a link name that we didn't then throw:
    if ( !!pglbrLink && !ppglbFoundCur )
    {
      throw bad_graph_stream( "_ReadUnfinishedNode(): Couldn't find entering link in relation list.");
    }

    // Insert the node last - if a node named this exists then we need to throw:
    _TyUnfinishedNode unInsert;
    unInsert.m_pgnbDst = pgnbNew;
    unInsert.m_iRemainingUnmatched = m_iLinksInsertedBeforeThrow;
    unInsert.m_iVisitOrder = m_iCurVisitOrder;
    _TyUNValueType  vtUN( m_pgnbrNode, unInsert );
    __THROWPT( e_ttMemory );
    pair< _TyUNIterator, bool > pibNode = m_nodesUnfinished.insert( vtUN );
    if ( !pibNode.second )
    {
      throw bad_graph_stream( "_ReadUnfinishedNode(): Found duplicate unfinished node name." );
    }

    // No throws after this.
    m_fThrowWhileInsertingLinks = false;
    m_iLinksInsertedBeforeThrow = 0;
    m_iLinksInsertedBeforeThrowLast = 0;

    m_iCurVisitOrder++;

    // Now add the new node to the existing sub-graph:
    _InsertNewNodeNoThrow( pgnbNew, ppglbFoundCur ); // does not throw.
  }

  void  _ReadLink()
  {
    if ( m_pglbConstructedEl )
    {
      assert( PGLBCur() );
      // Then we were expecting a node to be read - we had a constructed link:
      throw bad_graph_stream( "_ReadLink(): Expected a node found a link." );
    }

    // Allow this method to potentially read the name of the link:
    m_ris._ReadLinkHeaderData( &m_pglbrLink );

    if ( !t_fAllowUnconstructedLinks || m_ris._FReadLinkConstructed() )
    {
      // A constructed link - the link element follows:
      // Allocate a new link - if this ends up being an unfinished link then
      //  we will exchange positions with it:
      if ( !m_pglbAllocedInited )
      {
        // Then we don't have a left over allocated one - allocate a new one:
        (this->*m_pmfnAllocInitLink)( m_pglbAllocedInited );
      }

      (this->*m_pmfnReadLink)( m_pglbAllocedInited );
      m_pglbConstructedEl = m_pglbAllocedInited;  // construction now owned by this object.
      // Graph must desconstruct this {m_pglbConstructedEl} - but not deallocate
      //  - deallocation is owned in {m_pglbAllocedInited} - but will soon be
      //  owned by the graph {m_pgnbNewRoot} - while the construction will still be owned by (in) 
      //  {m_pglbConstructedEl}.
    }
    // Need to protect {m_pglbConstructedEl} - must zero if we throw from here on out.
    __STL_TRY
    {
      // Read the footer:
      _ReadLinkFooter( 0 );
    }
    // Since we can throw after constructing the link we need to revert state.
    // Derived should accept zero.
    __STL_UNWIND( ( (this->*m_pmfnDestructLinkEl)( m_pglbConstructedEl ),
                    m_pglbConstructedEl = 0 ) );
  }

  void  _ReadLinkFromUnfinished()
  {
    if ( m_pglbConstructedEl )
    {
      throw bad_graph_stream( "_ReadLinkFromUnfinished(): Not expecting an unfinished link - have constructed link." );
    }

    // Read the link name and look up in the link lookup -
    //  may also read the node name at this point ( but maybe not ):
    m_ris._ReadLinkFromUnfinishedHeaderData( &m_pglbrLink, &m_pgnbrNode );

    _TyUnfinishedLinks & rul = RULGet( !m_fDirectionDown );
    _TyULIterator it = rul.find( m_pglbrLink );
    if ( rul.end() == it )
    {
      throw bad_graph_stream( "_ReadLinkFromUnfinished(): Bad link name - not found.");
    }
    else
    {
      _TyULValueType & vtUL = *it;
      if ( vtUL.second->PGNBRelation( !m_fDirectionDown ) != 
            m_punCur->m_pgnbUnfinished )
      {
        throw bad_graph_stream( "_ReadLinkFromUnfinished(): Bad link name - wrong unfinished node being processed.");
      }
      else
      if ( vtUL.second->PGNBRelation( m_fDirectionDown ) )
      {
        throw bad_graph_stream( "_ReadLinkFromUnfinished(): Duplicate link name.");
      }

      if ( !t_fAllowUnconstructedLinks || m_ris._FReadLinkConstructed() )
      {
        (this->*m_pmfnReadLink)( vtUL.second );
        m_pglbConstructedEl = vtUL.second;  // construction now owned by this object.
      }
      // Need to protect {m_pglbConstructedEl} - must zero if we throw from here on out.
      __STL_TRY
      {
        _ReadLinkFooter( &vtUL );
      }
      // restore state - must accept null:
      __STL_UNWIND( ( (this->*m_pmfnDestructLinkEl)( m_pglbConstructedEl ),
                      m_pglbConstructedEl = 0 ) );
    }
    
  }

  void  _ReadLinkFooter( _TyULValueType * _pvtUnfinished )
  {
    _binary_rep_tokens< __false_type >::_TyToken  uc;
    m_ris._ReadLinkFooter( &uc );
    if ( _binary_rep_tokens< __false_type >::ms_ucNormalLinkFooter == uc )
    {
      if ( PGLBCur() )
      {
        assert( t_fAllowUnconstructedLinks || PGLBCur()->PGNBRelation( m_fDirectionDown ) );
      }

      if ( _pvtUnfinished )
      {
        // Then the link already linked in - set in current state:
        SetPGLBCur( _pvtUnfinished->second );
        SetPGNBCur( _pvtUnfinished->second->PGNBRelation( !m_fDirectionDown ) );
        // no throwing after this.

        // Update m_iRemainingLinks - if it hits zero then we need to increment m_punCur:
        if ( !--m_punCur->m_iRemainingUnmatched )
        {
          ++m_punCur;
        }
      }
      else
      {
        // Then need to link in the newly allocated link in - after the current link -
        //  or as the first relation of the current node.
        assert( m_pglbAllocedInited );
        m_pglbAllocedInited->SetRelationNode( !m_fDirectionDown, PGNBCur() );
        m_pglbAllocedInited->SetRelationNode( m_fDirectionDown, 0 );
        m_pglbAllocedInited->InsertRelation( m_fDirectionDown, 
          PGLBCur() ? PGLBCur()->PPGLBGetNextRelation( m_fDirectionDown ) :
                      PGNBCur()->PPGLBRelationHead( m_fDirectionDown ) );
        // Now update the current link:
        SetPGLBCur( m_pglbAllocedInited );
        m_pglbAllocedInited = 0;  // Now owned by the graph.
      }
      // no throwing here.
    }
    else
    {
      if ( _binary_rep_tokens< __false_type >::ms_ucUnfinishedLinkFooter != uc )
      {
        throw bad_graph_stream( "_ReadLinkFooter(): Found bad link footer token." );
      }
      if ( !m_pglbConstructedEl )
      {
        throw bad_graph_stream( "_ReadLinkFooter(): Unconstructed link to be connected to an unfinished node." );
      }

      // Possibly read the link and node names:
      m_ris._ReadUnfinishedLinkFooterData( &m_pglbrLink, &m_pgnbrNode );

      // Lookup the link in the current direction's lookup
      _TyUnfinishedLinks & rul = RULGet( m_fDirectionDown );
      _TyULIterator itUL = rul.find( m_pglbrLink );

      if ( rul.end() == itUL )
      {
        throw bad_graph_stream( "_ReadLinkFooter(): Link not found in unfinished lookup." );
      }
      // Is this link already connected ?
      _TyULValueType &  rulTerminate = *itUL;
      if ( rulTerminate.second->PGNBRelation( !m_fDirectionDown ) )
      {
        throw bad_graph_stream( "_ReadLinkFooter(): Duplicate link name found." );
      }

      // Now lookup the node - we need to evetually decrement the remaining link count:
      _TyUNIterator itUN = m_nodesUnfinished.find( m_pgnbrNode );
      if ( m_nodesUnfinished.end() == itUN )
      {
        throw bad_graph_stream( "_ReadLinkFooter(): Bad unfinished node name." );
      }
      _TyUNValueType &  rvtUN = *itUN;

      // no throwing after this.
      if ( _pvtUnfinished )
      {
        // Then we have an extra link lying around - if we have a place for it then save it
        //  otherwise dealloc:
        if ( m_pglbAllocedInited )
        {
          (this->*m_pmfnDeallocateLink)( m_pglbAllocedInited );
        }
        m_pglbAllocedInited = rulTerminate.second;  // Save the allocated link for later.
        rulTerminate.second = m_pglbConstructedEl;  // We change the mapping in the lookup.
        // This protects against duplicate link names.
        
        // Insert the constructed link before the placeholder:
        m_pglbConstructedEl->SetRelationNode( m_fDirectionDown, rvtUN.second.m_pgnbDst );
        m_pglbConstructedEl->InsertRelation( !m_fDirectionDown, 
          m_pglbAllocedInited->PPGLBGetThisRelation( !m_fDirectionDown ) );
        SetPGLBCur( m_pglbConstructedEl );
        m_pglbConstructedEl = 0;  // Graph now owns the constructed link.

        // Remove the placeholder:
        m_pglbAllocedInited->RemoveRelation( !m_fDirectionDown );

        // Update m_iRemainingLinks - if it hits zero then we need to increment m_punCur:
        if ( !--m_punCur->m_iRemainingUnmatched )
        {
          ++m_punCur;
        }
      }
      else
      {
        // Then (m_pglbAllocedInited,m_pglbConstructedEl) contain the constructed link -
        //  need to substitute for the current placeholder and update the lookup:
        m_pglbConstructedEl->SetRelationNode( m_fDirectionDown, rvtUN.second.m_pgnbDst );
        m_pglbConstructedEl->InsertRelation( !m_fDirectionDown, 
          rulTerminate.second->PPGLBGetThisRelation( !m_fDirectionDown ) );

        m_pglbConstructedEl->SetRelationNode( !m_fDirectionDown, PGNBCur() );
        m_pglbConstructedEl->InsertRelation( m_fDirectionDown, 
          PGLBCur() ? PGLBCur()->PPGLBGetNextRelation( m_fDirectionDown ) :
                      PGNBCur()->PPGLBRelationHead( m_fDirectionDown ) );

        SetPGLBCur( m_pglbConstructedEl );
        m_pglbConstructedEl = 0;  // Graph now owns the constructed link.

        rulTerminate.second->RemoveRelation( !m_fDirectionDown );
        swap( rulTerminate.second, m_pglbAllocedInited );       
      }
      // no throwing here.
      if ( !--rvtUN.second.m_iRemainingUnmatched )
      {
        // Then remove the node from the unfinished lookup:
        m_nodesUnfinished.erase( itUN );  
      }
      // Could erase itUL as well - but no real need.
    }
    // no throwing here.
  }

  void _ProcessGraphFooter()
    {
      if ( m_punStart && m_punCur != m_punEnd )
      {
        throw bad_graph_stream( "_ProcessGraphFooter(): Not finished processing unfinished nodes." );
      }
      if ( m_iContexts )
      {
        throw bad_graph_stream( "_ProcessGraphFooter(): Not finished processing all contexts." );
      }
      if ( m_pglbConstructedEl )
      {
        throw bad_graph_stream( "_ProcessGraphFooter(): Had constructed link - expecting node." );
      }
      if ( m_fProcessedGraphFooter )
      {
        throw bad_graph_stream( "_ProcessGraphFooter(): Already processed graph footer." );
      }
      
      _ClearNodeArray();
      m_punStart = 0;
      SetPGNBCur( 0 );
      SetPGLBCur( 0 );
      m_fProcessedGraphFooter = true;
    };
};

__DGRAPH_END_NAMESPACE

#endif __GR_INPT_H
