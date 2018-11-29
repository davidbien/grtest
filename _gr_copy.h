#ifndef __GR_COPY_H
#define __GR_COPY_H

#include "_gr_disn.h"

#ifdef __GR_DSIN_USEHASH
#include <hash_map>
#else __GR_DSIN_USEHASH
#include <map>
#endif __GR_DSIN_USEHASH

__DGRAPH_BEGIN_NAMESPACE

// _gr_copy.h

// This module declares utility stuff used in the copying of directed graphs.

#define __GR_COPY_INITSIZENODES _GR_HASH_INITSIZENODES
#define __GR_COPY_INITSIZELINKS _GR_HASH_INITSIZELINKS

template <  class t_TyGraphLinkBaseSrc, 
            class t_TyGraphLinkBaseDst >
struct _gc_context
{
  t_TyGraphLinkBaseSrc ** m_ppglbSrc;
  t_TyGraphLinkBaseDst ** m_ppglbDst;
};

template < class t_TyGraphSrc, class t_TyGraphDst >
struct _gc_default_copy_node
{
  typedef typename t_TyGraphSrc::_TyGraphNode _TyGraphNodeSrc;
  typedef typename t_TyGraphDst::_TyGraphNode _TyGraphNodeDst;
  void  operator()( _TyGraphNodeDst & _rgnDst, _TyGraphNodeSrc const & _rgnSrc )
  {
    // Default just calls the copy constructor:
    t_TyGraphDst::template _construct_node_el1
      < const typename t_TyGraphSrc::_TyGraphNode::_TyNodeEl & >
      ( &_rgnDst, _rgnSrc.RElConst() );
  }
};

template < class t_TyGraphSrc, class t_TyGraphDst >
struct _gc_default_copy_link
{
  typedef typename t_TyGraphSrc::_TyGraphLink _TyGraphLinkSrc;
  typedef typename t_TyGraphDst::_TyGraphLink _TyGraphLinkDst;
  void  operator()( _TyGraphLinkDst & _rglDst, _TyGraphLinkSrc const & _rglSrc )
  {
    // Default just calls the copy constructor:
    t_TyGraphDst::template _construct_link_el1
      < const typename t_TyGraphSrc::_TyGraphLink::_TyLinkEl & >
      ( &_rglDst, _rglSrc.RElConst() );
  }
};

template <  class t_TyGraphSrc, class t_TyGraphDst, 
            class t_TyAllocator = allocator<char>,
            class t_TyNodeCopyObject = _gc_default_copy_node< t_TyGraphSrc, t_TyGraphDst >,
            class t_TyLinkCopyObject = _gc_default_copy_link< t_TyGraphSrc, t_TyGraphDst > >
struct _graph_copy_struct
{
private:
  typedef _graph_copy_struct< t_TyGraphSrc, t_TyGraphDst >  _TyThis;
public:

  typedef typename t_TyGraphSrc::_TyGraphNode                   _TyGraphNodeSrc;
  typedef typename t_TyGraphSrc::_TyGraphNodeBaseBase _TyGraphNodeBaseBaseSrc;
  typedef typename t_TyGraphSrc::_TyGraphLinkBaseBase _TyGraphLinkBaseBaseSrc;
  typedef typename t_TyGraphDst::_TyGraphNode _TyGraphNodeDst;
  typedef typename t_TyGraphDst::_TyGraphLink _TyGraphLinkDst;
  typedef typename t_TyGraphDst::_TyGraphNodeBaseBase _TyGraphNodeBaseBaseDst;
  typedef typename t_TyGraphDst::_TyGraphLinkBaseBase _TyGraphLinkBaseBaseDst;

  typedef _gc_unconnected_node< _TyGraphNodeBaseBaseDst >       _TyUnconnectedNode;
  typedef _TyGraphLinkBaseBaseDst *                             _TyUnconnectedLink;

#ifdef __GR_DSIN_USEHASH
  // Use most base classes for lookup - conserve code:
  typedef hash_map< const _TyGraphNodeBaseBaseSrc*, _TyUnconnectedNode, _gr_hash_ptr< const _TyGraphNodeBaseBaseSrc* >,
                    equal_to< const _TyGraphNodeBaseBaseSrc* >, t_TyAllocator > _TyUnconnectedNodes;
  typedef hash_map< const _TyGraphLinkBaseBaseSrc*, _TyUnconnectedLink, _gr_hash_ptr< const _TyGraphLinkBaseBaseSrc* >,
                    equal_to< const _TyGraphLinkBaseBaseSrc* >, t_TyAllocator > _TyUnconnectedLinks;
  static const typename _TyUnconnectedNodes::size_type ms_stInitSizeNodes = __GR_COPY_INITSIZENODES;
  static const typename _TyUnconnectedLinks::size_type ms_stInitSizeLinks = __GR_COPY_INITSIZELINKS;
#else __GR_DSIN_USEHASH
  typedef map<  const _TyGraphNodeBaseBaseSrc*, _TyUnconnectedNode, 
                less< const _TyGraphNodeBaseBaseSrc* >, t_TyAllocator > _TyUnconnectedNodes;
  typedef map<  const _TyGraphLinkBaseBaseSrc*, _TyUnconnectedLink, 
                less< const _TyGraphLinkBaseBaseSrc* >, t_TyAllocator > _TyUnconnectedLinks;
#endif __GR_DSIN_USEHASH

  typedef typename _TyUnconnectedNodes::value_type  _TyUNValueType;
  typedef typename _TyUnconnectedNodes::iterator    _TyUNIterator;
  typedef typename _TyUnconnectedLinks::value_type  _TyULValueType;
  typedef typename _TyUnconnectedLinks::iterator    _TyULIterator;

  t_TyGraphSrc const &    m_rSrc;
  t_TyGraphDst &          m_rDst;

  bool                    m_fClosedDirectedCopy;
  bool                    m_fCopyDirectionDown;

  _TyGraphNodeSrc const * m_pgnSrcCopyRoot;
  _TyGraphNodeSrc const * m_pgnSrc;
  _TyGraphNodeDst *       m_pgnDst;

  // Use a lookup for both parent-wise and child-wise unconnected links and nodes - they
  //  are disjoint in their usage ( except that we need one while creating the other )
  //  and this manages them more efficiently.
  // REVIEW: We really only need one of each of these. We could remove the double links table
  //  right now - with minimal code changes - the link information is not used when finishing
  //  the copying of an unconnected node ( i.e. after an iteration ends in one direction ).
  // The node information is used - but could be transferred to an array and then cleared ( would
  //  this be better ? ). The array could be _alloca()'d since we only use it locally. This would
  //  likely be better for memory fragmentation - we would clear the fragments.
  _TyUnconnectedNodes m_mapUnconnectedNodesUp;  
  _TyUnconnectedNodes m_mapUnconnectedNodesDown;  
  _TyUnconnectedLinks m_mapUnconnectedLinksUp;  
  _TyUnconnectedLinks m_mapUnconnectedLinksDown;  

  // Use a context stack to eliminate recursion:
  typedef _gc_context< _TyGraphLinkBaseBaseSrc, _TyGraphLinkBaseBaseDst > _TyContext;
  typedef slist< _TyContext, t_TyAllocator >                              _TyContexts;
  _TyContexts m_lContexts;
  int         m_iContexts;

  _TyGraphNodeDst *       m_pgnDstNewRoot;  // The newly created tree.
  _TyGraphNodeDst *       m_pgnDstTempRoot; // This is maintained for throw-safety - a partially constructed sub-graph.

  t_TyNodeCopyObject m_nco;
  t_TyLinkCopyObject m_lco;

  _graph_copy_struct( t_TyGraphSrc const & _rSrc,
                      t_TyGraphDst & _rDst,
                      bool _fClosedDirectedCopy,
                      bool _fCopyDirectionDown,
                      t_TyAllocator const & _rAlloc = t_TyAllocator(),
                      t_TyNodeCopyObject const & _rnco = t_TyNodeCopyObject(),
                      t_TyLinkCopyObject const & _rlco = t_TyLinkCopyObject() ) _STLP_NOTHROW
  : m_rSrc( _rSrc ),
    m_rDst( _rDst ),
    m_fClosedDirectedCopy( _fClosedDirectedCopy ),
    m_fCopyDirectionDown( _fCopyDirectionDown ),
    m_pgnSrcCopyRoot( 0 ),
    m_pgnDstNewRoot( 0 ),
    m_pgnDstTempRoot( 0 ),
    m_nco( _rnco ),
    m_lco( _rlco ),
#ifdef __GR_DSIN_USEHASH
    m_mapUnconnectedNodesDown( ms_stInitSizeNodes, 
                               typename _TyUnconnectedNodes::hasher(), 
                               typename _TyUnconnectedNodes::key_equal(), 
                               _rAlloc ),
    m_mapUnconnectedNodesUp( ms_stInitSizeNodes, 
                             typename _TyUnconnectedNodes::hasher(), 
                             typename _TyUnconnectedNodes::key_equal(), 
                             _rAlloc ),
    m_mapUnconnectedLinksDown( ms_stInitSizeLinks, 
                               typename _TyUnconnectedLinks::hasher(), 
                               typename _TyUnconnectedLinks::key_equal(), 
                               _rAlloc ),
    m_mapUnconnectedLinksUp( ms_stInitSizeLinks, 
                             typename _TyUnconnectedLinks::hasher(), 
                             typename _TyUnconnectedLinks::key_equal(), 
                             _rAlloc ),
#else __GR_DSIN_USEHASH
    m_mapUnconnectedNodesDown( typename _TyUnconnectedNodes::key_compare(), 
                               _rAlloc ),
    m_mapUnconnectedNodesUp( typename _TyUnconnectedNodes::key_compare(), 
                             _rAlloc ),
    m_mapUnconnectedLinksDown( typename _TyUnconnectedLinks::key_compare(), 
                               _rAlloc ),
    m_mapUnconnectedLinksUp( typename _TyUnconnectedLinks::key_compare(), 
                             _rAlloc ),
#endif __GR_DSIN_USEHASH
    m_lContexts( _rAlloc ),
    m_iContexts( 0 )
  {
    __THROWPT( e_ttMemory );
  }

  ~_graph_copy_struct() _STLP_NOTHROW
  {
    // If the caller did not transfer ownership of the new graph then we should destroy
    //  it now:
    if ( m_pgnDstNewRoot )
    {
      m_rDst.destroy_node( m_pgnDstNewRoot );
    }
    if ( m_pgnDstTempRoot )
    {
      m_rDst.destroy_node( m_pgnDstTempRoot );
    }
  }

  // Access the current node and link lookup table bases on the current copy direction:
  _TyUnconnectedNodes & RNodesCur()  _STLP_NOTHROW
  {
    return m_fCopyDirectionDown ? m_mapUnconnectedNodesDown : m_mapUnconnectedNodesUp;
  }
  _TyUnconnectedLinks & RLinksCur() _STLP_NOTHROW
  {
    return m_fCopyDirectionDown ? m_mapUnconnectedLinksDown : m_mapUnconnectedLinksUp;
  }

  // Transfer ownership of the newly created root:
  _TyGraphNodeDst * PGNTransferNewRoot() _STLP_NOTHROW
  {
    assert( !m_pgnDstTempRoot );
    _TyGraphNodeDst * _pgn = m_pgnDstNewRoot;
    m_pgnDstNewRoot = 0;
    return _pgn;
  }

  _TyUNIterator ITFindNode( typename t_TyGraphSrc::_TyGraphNodeBaseBase const * _pgnbSrcFind )  _STLP_NOTHROW
  {
    return RNodesCur().find( _pgnbSrcFind );
  }
  _TyUNIterator ITBeginNodes() _STLP_NOTHROW
  { 
    return RNodesCur().begin();
  }
  _TyUNIterator ITEndNodes() _STLP_NOTHROW
  { 
    return RNodesCur().end();
  }
  pair< _TyUNIterator, bool >   PIBInsertNode( const _TyUNValueType & _rdn )
  {
    __THROWPT( e_ttMemory );
    return RNodesCur().insert( _rdn );
  }
  void    RemoveNode( const _TyUNIterator & rit ) _STLP_NOTHROW
  {
    RNodesCur().erase( rit );
  }
  void    ClearNodes() _STLP_NOTHROW
  {
    RNodesCur().clear();
  }

  _TyULIterator ITFindLink( typename t_TyGraphSrc::_TyGraphLinkBaseBase const * _pglbSrcFind ) _STLP_NOTHROW
  {
    return RLinksCur().find( _pglbSrcFind );
  }
  _TyULIterator ITBeginLinks() _STLP_NOTHROW
  { 
    return RLinksCur().begin();
  }
  _TyULIterator ITEndLinks() _STLP_NOTHROW
  { 
    return RLinksCur().end();
  }
  void    InsertLink( const _TyULValueType & _rdn )
  {
    __THROWPT( e_ttMemory );
    pair< _TyULIterator, bool > pibLink = RLinksCur().insert( _rdn );
    assert( pibLink.second );
  }
  void    RemoveLink( const _TyULIterator & rit ) _STLP_NOTHROW
  {
    RLinksCur().erase( rit );
  }
  void    ClearLinks() _STLP_NOTHROW
  {
    RLinksCur().clear();
  }

  bool          FNoDisconnected() _STLP_NOTHROW
  {
    return  m_mapUnconnectedNodesDown.begin() == m_mapUnconnectedNodesDown.end() &&
            m_mapUnconnectedNodesUp.begin() == m_mapUnconnectedNodesUp.end() &&
            m_mapUnconnectedLinksDown.begin() == m_mapUnconnectedLinksDown.end() &&
            m_mapUnconnectedLinksUp.begin() == m_mapUnconnectedLinksUp.end();
  }
// Copy from source graph to destination graph.
// If <m_fClosedDirectedCopy> then contiguous direction-wise node region is copied ( i.e. copy direction
//  is never reversed to copy a) above/below the root node for the copy <or> b) up/down branches that
//  go in the opposite direction than the copy. ( of course cycles may cause this to happen ).
// If <m_fCopyDirectionDown> then copy is started in downward ( child-wise ) direction.
// The new graph is orphaned from the original destination graph - i.e. caller needs to connect if so desired.
// If the caller does not connect then the graph may be leaked if original graph is destroyed.
void
copy( )
{
  // If nothing to do then return immediately:
  if (  !m_pgnSrcCopyRoot &&
        !( m_pgnSrcCopyRoot = m_rSrc.get_root() ) )
  {
    return;
  }

  m_pgnSrc = m_pgnSrcCopyRoot;

  // Create the new root node:
  {
    _TyGraphNodeDst * pgnDst = m_rDst._allocate_node();
    _STLP_TRY
    {
      pgnDst->Init();
      m_nco( *pgnDst, *m_pgnSrc );
    }
    _STLP_UNWIND( m_rDst._deallocate_node( pgnDst ) );
    m_pgnDst = m_pgnDstNewRoot = pgnDst;
  }

  // Set up the initial state of the disconnected node map - even if this is
  //  a <m_fClosedDirectedCopy> we may need to add the root node of the copy
  //  to the map - a cycle might allow us to get back to it:
  if ( m_pgnSrc->FRelations( !m_fCopyDirectionDown ) )
  {
    // Then we should create links for each of the relations in the opposite direction of
    //  the copy, insert them in the correct order in the link list:
    _TyGNIndex iRelations = 0;
    typename t_TyGraphDst::_TyLinkPosIterNonConst lpiDst(
        m_pgnDstNewRoot->PPGLRelationHead( !m_fCopyDirectionDown ) );
    typename t_TyGraphSrc::_TyLinkPosIterConst    lpiSrc( 
        m_pgnSrc->PPGLRelationHead( !m_fCopyDirectionDown ) );
    for ( ;
          !lpiSrc.FIsLastRelation( !m_fCopyDirectionDown );
          ++iRelations,
          lpiSrc.NextRelation( !m_fCopyDirectionDown ),
          lpiDst.NextRelation( !m_fCopyDirectionDown ) ) // Move to point to the end of the list.
    {
      __SDP(  typename t_TyGraphDst::_TyGraphLink, 
              typename t_TyGraphDst::_TyGraphLinkAllocator,
              m_rDst.get_link_allocator(), pglNew );
      __SDP_PTRREF( pglNew ) = m_rDst._allocate_link();
      __SDP_CHECK_VOID( pglNew );
      // Don't construct the link element now - a disconnected link's element 
      //  is not constructed ( invariant ) - but use internal initialization:
      pglNew->Init();
      pglNew->SetRelationNode( m_fCopyDirectionDown, m_pgnDstNewRoot );
      pglNew->SetRelationNode( !m_fCopyDirectionDown, 0 ); // This node doesn't exist yet ( and may never ).
      lpiDst.InsertLinkBeforeRelation( !m_fCopyDirectionDown, __SDP_TRANSFER( pglNew ) ); // now owned by graph node.

      // Add this link to the link map:
      _TyULValueType ulInsert( lpiSrc.PGLCur(), lpiDst.PGLCur() );
      InsertLink( ulInsert );// throws.
    }

    // We have allocated and inserted all the disconnected links - now insert
    //  this node into the disconnected node map:
    _TyUnconnectedNode unInsert;
    unInsert.m_pgnbDst = m_pgnDstNewRoot;
    unInsert.m_iRemainingUnmatched = iRelations;
    _TyUNValueType vtInsert( m_pgnSrc, unInsert );
    (void)PIBInsertNode( vtInsert ); // throws
  }

  // perform the copy - recurse:
  do_copy( );

  // Check for disconnected nodes and augment graph appropriately:
  if ( !m_fClosedDirectedCopy )
  {
    // a pass in a given direction can only generate nodes that are disconnected in the
    //  opposite direction - since we handle all such disconnected nodes we need only
    //  check the current directions node map:
    for(  ; 
          ITBeginNodes() != ITEndNodes();
          m_fCopyDirectionDown = !m_fCopyDirectionDown )
    {
      check_disconnected_nodes();
    }
    assert( FNoDisconnected() );  // Should have handled all nodes and links.
  }
  else
  {
    assert( 0 );  // This needs to be done.
    // TODO: Should remove stray links from the destination.
  }
}

void
do_copy( )
{
  if ( m_fCopyDirectionDown )
  {
    do_copy_down( );
  }
  else
  {
    do_copy_up( );
  }
}

void
check_disconnected_nodes()
{
  // Due to the nature of directed graphs ( and this algorithm ) we know
  //  that the set of disconnected nodes, and the set of disconnected links in each 
  //  disconnected node, in the current direction will not be changed
  //  by the handling of disconnected nodes - i.e. we can iterate the container safely
  //  and linearly.

  for ( _TyUNIterator it = ITBeginNodes();
        it != ITEndNodes();
        ++it )
  { 
    _TyUNValueType const & vtUN = *it;

    // Set up initial recursion state:
    m_pgnSrc = static_cast< const _TyGraphNodeSrc* >( vtUN.first );
    m_pgnDst = static_cast< _TyGraphNodeDst* >( vtUN.second.m_pgnbDst );
    // Change the direction of the copy:
    m_fCopyDirectionDown = !m_fCopyDirectionDown;
    do_copy();
    m_fCopyDirectionDown = !m_fCopyDirectionDown;
  }

  // Clear the links and nodes instead of removing as found - this is faster:
  ClearNodes();
  ClearLinks();
}

// We do these separately to allow the compiler to optimize - without checking a bool
//  for the relationship continuously:
void
do_copy_down( )
{
  typename t_TyGraphSrc::_TyLinkPosIterConst lpiSrcChild;
  lpiSrcChild.SetPPGLCur( m_pgnSrc->PPGLChildHead() );
  typename t_TyGraphDst::_TyLinkPosIterNonConst lpiDstChild;
  lpiDstChild.SetPPGLCur( m_pgnDst->PPGLChildHead() );

  // Either we are creating all the links or none - if we aren't doing a
  //  <m_fClosedDirectedCopy> then we may have created all the links already
  //  in a disconnected state:
  assert( lpiDstChild.FIsLastChild() || 
          ( !m_fClosedDirectedCopy && 
            m_pgnSrc->UChildren() == m_pgnDst->UChildren() ) );

  while( !lpiSrcChild.FIsLastChild() )
    {
      bool fHaveDstChildren = !lpiDstChild.FIsLastChild();

      // If we have the destination children then this link may already be connected 
      //  ( and constructed ) - in that case just continue to the next:
      if ( !fHaveDstChildren || !lpiDstChild.PGNBChild() )
        {
          // If this child is in the disconnected map then the link has already been created for us
          //  and after we connect we should continue on to the next child.
        
          // We know that if this child has only one parent ( us ) then we can assume
          //  that it is not in the map this is cheaper than checking the map for the node:
          typename t_TyGraphSrc::_TyLinkPosIterConst lpiSrcParent( lpiSrcChild.PGNChild()->PPGLParentHead() );
          lpiSrcParent.NextParent();
    
          // Unless - special case - the node at the child is the node where the copy started.
          pair< _TyUNIterator, bool > pibFoundNode;
          if ( !lpiSrcParent.FIsLast() || ( lpiSrcChild.PGNChild() == m_pgnSrcCopyRoot ) )
            {
              // Find or insert the node - we know it will be there ( eventually ):
              _TyUNValueType vtInsert( lpiSrcChild.PGNChild(), _TyUnconnectedNode() );
              pibFoundNode = PIBInsertNode( vtInsert );
              assert( ( lpiSrcChild.PGNChild() != m_pgnSrcCopyRoot ) || !pibFoundNode.second );
            }
          else
            {
              pibFoundNode.second = true;
            }

          if ( pibFoundNode.second )
            {
              // Create the new node:              
              {
                _TyGraphNodeDst * pgnDst = m_rDst._allocate_node();
                _STLP_TRY
                {
                  pgnDst->Init();
                  m_nco( *pgnDst, *lpiSrcChild.PGNChild() );
                }
                _STLP_UNWIND( m_rDst._deallocate_node( pgnDst ) );
                m_pgnDstTempRoot = pgnDst;
              }

              // Now create a link for each parent of this child - when we encounter the
              //  current child link during this parent traversal - remember it - we
              //  we link this new subgraph into the main graph after allocating the links:
              typename t_TyGraphDst::_TyGraphLink **  ppglCurChildDst;
              __DEBUG_STMT( ppglCurChildDst = 0 )
              typename t_TyGraphDst::_TyLinkPosIterNonConst lpiDstParent;
              lpiDstParent.SetPPGLCur( m_pgnDstTempRoot->PPGLParentHead() );
              _TyGNIndex  iParents = 0;
              for ( lpiSrcParent.SetPPGLCur( lpiSrcChild.PGNChild()->PPGLParentHead() );
                    !lpiSrcParent.FIsLastParent();
                    iParents++,
                      lpiSrcParent.NextParent() ) 
                {
                  if (  fHaveDstChildren &&
                        ( lpiSrcParent.PGLCur() == lpiSrcChild.PGLCur() ) )
                    {
                      // Then this destination link has already been allocated - this allocation
                      //  is owned by its parent - to which it is connected - save this
                      //  position in the child's parent list - we will insert the link after allocating
                      //  and inserting all of the others ( this maintains throw-safety - allocation ownership ):
                      ppglCurChildDst = lpiDstParent.PPGLCur();
                    }
                  else
                    {
                      __SDP(  typename t_TyGraphDst::_TyGraphLink, 
                              typename t_TyGraphDst::_TyGraphLinkAllocator,
                              m_rDst.get_link_allocator(), pglNew );
                      __SDP_PTRREF( pglNew ) = m_rDst._allocate_link( );
                      __SDP_CHECK_VOID( pglNew );
                      // Don't construct the link now - as this link now - but use internal initialization:
                      pglNew->Init();
                      pglNew->SetChildNode( m_pgnDstTempRoot );
                      pglNew->SetParentNode( 0 ); // not connected to the parent initially - this allows clean
                      //  destruction in the case of a throw.
                      lpiDstParent.InsertLinkBeforeParent( __SDP_TRANSFER( pglNew ) ); // now owned by graph node.

                      if ( lpiSrcParent.PGLCur() == lpiSrcChild.PGLCur() )
                        {
                          ppglCurChildDst = lpiDstParent.PPGLCur(); // This needs to be connected to the parent below.
                        }
                      else
                        {
                          // Associate the destination and source links in the lookup table:
                          _TyULValueType ulInsert( lpiSrcParent.PGLCur(), lpiDstParent.PGLCur() );
                          InsertLink( ulInsert );// throws.
                        }

                      lpiDstParent.NextParent(); // Move to point to the end of the list.
                    }
                }
              assert( ppglCurChildDst );

              typename t_TyGraphDst::_TyGraphLink * pglCurChildDst = 
                fHaveDstChildren ? lpiDstChild.PGLCur() : *ppglCurChildDst;

              if ( 1 < iParents ) // More than one to connect.
                {
                  assert( true == pibFoundNode.second );
                  // We have multiple parents and this node was not in the map:
                  // already inserted - get the reference from the map:
                  _TyUNValueType & rvtInsert = *pibFoundNode.first;
                  assert( m_pgnDstTempRoot );
                  rvtInsert.second.m_pgnbDst = m_pgnDstTempRoot;
                  rvtInsert.second.m_iRemainingUnmatched = --iParents;
                }

              // Now we have created all the links and the node and possibly registered the node as disconnected,
              //  need to:
              // a) Construct the link - may throw - therefore do this first - graph doesn't "own" the link
              //    element ( i.e. doesn't need to destruct the link element ) until that link element
              //    has both a child and parent node ( under the current paradigm ). If we want to allow constructed
              //    link elements that lack a parent and/or child then we will need a flag that indicates 
              //    whether the link element has been constructed. This brings up whether the link needs both
              //    the nodes involved to be constructed.
              // b) Then link the newly created sub-graph into the current destination sub-graph.
              // c) Transfer ownership of the newly created sub-graph - now owned by current destination sub-graph.
              m_lco( *pglCurChildDst, *lpiSrcChild.PGLCur() );

              // no throwing statements until the destination owns the new graph ( else we leak the link element ).

              if ( fHaveDstChildren )
                {
                  // We already had the child link allocated and inserted into the destination child list.
                  // Link to the new child node - this transfers ownership.
                  pglCurChildDst->InsertParent( ppglCurChildDst );  // should not throw.
                  pglCurChildDst->SetChildNode( m_pgnDstTempRoot );
                }
              else
                {
                  // If this statement can throw then we should use a smart destructor holder ( the allocation
                  //  however is owned by the node and is cleaned up via fcdCleanupNew ).
                  // Link into current destination subgraph:
                  lpiDstChild.InsertLinkBeforeChild( pglCurChildDst );
                  pglCurChildDst->SetParentNode( m_pgnDst );  // This shouldn't throw.
                }

              // we now own the new subgraph, no longer clean up on throw:
              m_pgnDstTempRoot = 0; // Ownership of newly allocated subgraph is transferred.

              // Move to child node - perhaps push current context:
              if ( lpiSrcChild.PGNChild()->FChildren() )
                {
                  assert( lpiSrcChild.PGNParent() == m_pgnSrc );
                  assert( pglCurChildDst->PGNParent() == m_pgnDst );
                  assert( lpiDstChild.PGNParent() == m_pgnDst );

                  m_pgnSrc = lpiSrcChild.PGNChild();
                  m_pgnDst = pglCurChildDst->PGNChild();

                  // Push this context if we have remaining source children to copy:
                  lpiSrcChild.NextChild();
                  if ( !lpiSrcChild.FIsLastChild() )
                    {
                      _TyContext  ctxt;
                      ctxt.m_ppglbSrc = lpiSrcChild.PPGLBCur();
                      ctxt.m_ppglbDst = lpiDstChild.PPGLBCur(); // inc later.

                      __THROWPT( e_ttMemory );
                      m_lContexts.push_front( ctxt ); // throws.
                      m_iContexts++;
                    }

                  lpiSrcChild.SetPPGLCur( m_pgnSrc->PPGLChildHead() );
                  lpiDstChild.SetPPGLCur( m_pgnDst->PPGLChildHead() );

                  continue; // Need to skip code below.
                }
              // else continue to next child.
            }
          else
            {
              //assert( !fHaveDstChildren );  // if the node is in the map then we should not
              //  already have the destination children.
              // REVIEW: Unless we just created this unfinished node during this iteration.
              // Leave the assertion above to ensure that the test cases hit this problem.
              // The solution is to that problem is merely to remove the node following
              //  the newly inserted node - the allocated node being used as a place-holder.

              // Then we need to search for the corresponding link - if it is the first of
              //  the unmatched then increment the unmatched.
              _TyUNValueType &  rdnFound = *pibFoundNode.first;

              _TyULIterator itFoundNodeLink = ITFindLink( lpiSrcChild.PGLBCur() );
              assert( ITEndLinks() != itFoundNodeLink );  // it had better be there :-)
              _TyGraphLinkBaseBaseDst * pglbFoundDst = (*itFoundNodeLink).second;
              assert( pglbFoundDst );

              // REVIEW: This is not necessary - may be faster not to remove the link.
              // Except when doing a closed directed copy - then we need to know which links remain.
              RemoveLink( itFoundNodeLink );  

              if ( !--rdnFound.second.m_iRemainingUnmatched )
                {
                  // Then we have matched all the links - node is fully connected - remove from map:
                  RemoveNode( pibFoundNode.first );
                  // don't use rdnFound anymore.
                }

              // The link has been initialized but not constructed:
              m_rDst.template _construct_link_el1
                < const typename t_TyGraphSrc::_TyGraphLink::_TyLinkEl & >
                (  static_cast< _TyGraphLinkDst * >( pglbFoundDst ), 
                   *lpiSrcChild );

              // No throwing until graph owns this node.

              // Remove any place-holder link:
              // REVIEW: Since this would be rare - we could save the allocated link for re-use:
              if ( fHaveDstChildren )
                {
                  _TyGraphLinkDst * pglDealloc = lpiDstChild.PGLCur();
                  lpiDstChild.PGLCur()->RemoveChild();
                  m_rDst._deallocate_link( pglDealloc );
                }

              lpiDstChild.InsertLinkBeforeChild( pglbFoundDst );
              pglbFoundDst->SetParentNode( m_pgnDst );  // Graph now owns the constructed link.
            }
        }

      // continue on to next child.
      // If we have come to the end of the children then we must pop a context or return:
      lpiSrcChild.NextChild();
      if ( lpiSrcChild.FIsLastChild() )
        {
          if ( m_iContexts )
            {
              _TyContext & _rctxt = m_lContexts.front();
              lpiSrcChild.SetPPGLBCur( _rctxt.m_ppglbSrc );
              lpiDstChild.SetPPGLBCur( _rctxt.m_ppglbDst );
              assert( !lpiSrcChild.FIsLastChild() );
              assert( !lpiDstChild.FIsLastChild() );
              m_lContexts.pop_front();
              m_iContexts--;
              // Restore the current nodes:
              m_pgnSrc = lpiSrcChild.PGNParent();
              m_pgnDst = lpiDstChild.PGNParent();
              lpiDstChild.NextChild();  // Increment after extracting parent.
            }
          // Otherwise loop will exit.
        }
      else
        {
          lpiDstChild.NextChild();
        }
    }
}

void
do_copy_up()
{
  // *** Code copied from do_copy_down() juxtapose( "Children", "Parents" ) and ("Child", "Parent") ***
  // *** Don't edit this method - change do_copy_down() then copy the code
  // *** I know this isn't the best code management method, but...
  // *** it works
  typename t_TyGraphSrc::_TyLinkPosIterConst lpiSrcParent;
  lpiSrcParent.SetPPGLCur( m_pgnSrc->PPGLParentHead() );
  typename t_TyGraphDst::_TyLinkPosIterNonConst lpiDstParent;
  lpiDstParent.SetPPGLCur( m_pgnDst->PPGLParentHead() );

  // Either we are creating all the links or none - if we aren't doing a
  //  <m_fClosedDirectedCopy> then we may have created all the links already
  //  in a disconnected state:
  assert( lpiDstParent.FIsLastParent() || 
          ( !m_fClosedDirectedCopy && 
            m_pgnSrc->UParents() == m_pgnDst->UParents() ) );

  while( !lpiSrcParent.FIsLastParent() )
    {
      bool fHaveDstParents = !lpiDstParent.FIsLastParent();

      // If we have the destination parents then this link may already be connected 
      //  ( and constructed ) - in that case just continue to the next:
      if ( !fHaveDstParents || !lpiDstParent.PGNBParent() )
        {

          // If this parent is in the disconnected map then the link has already been created for us
          //  and after we connect we should continue on to the next parent.

          // We know that if this parent has only one child ( us ) then we can assume
          //  that it is not in the map this is cheaper than checking the map for the node:
          typename t_TyGraphSrc::_TyLinkPosIterConst lpiSrcChild( lpiSrcParent.PGNParent()->PPGLChildHead() );
          lpiSrcChild.NextChild();
  
          // Unless - special case - the node at the parent is the node where the copy started.
          pair< _TyUNIterator, bool > pibFoundNode;
          if ( !lpiSrcChild.FIsLast() || ( lpiSrcParent.PGNParent() == m_pgnSrcCopyRoot ) )
            {
              // Find or insert the node - we know it will be there ( eventually ):
              _TyUNValueType vtInsert( lpiSrcParent.PGNParent(), _TyUnconnectedNode() );
              pibFoundNode = PIBInsertNode( vtInsert );
              assert( ( lpiSrcParent.PGNParent() != m_pgnSrcCopyRoot ) || !pibFoundNode.second );
            }
          else
            {
              pibFoundNode.second = true;
            }

          if ( pibFoundNode.second )
            {
              // Create the new node:              
              {
                _TyGraphNodeDst * pgnDst = m_rDst._allocate_node();
                _STLP_TRY
                {
                  pgnDst->Init();
                  m_nco( *pgnDst, *lpiSrcParent.PGNParent() );
                }
                _STLP_UNWIND( m_rDst._deallocate_node( pgnDst ) );
                m_pgnDstTempRoot = pgnDst;
              }
  
              // Now create a link for each child of this parent - when we encounter the
              //  current parent link during this child traversal - remember it - we
              //  we link this new subgraph into the main graph after allocating the links:
              typename t_TyGraphDst::_TyGraphLink **  ppglCurParentDst;
              __DEBUG_STMT( ppglCurParentDst = 0 )
              typename t_TyGraphDst::_TyLinkPosIterNonConst lpiDstChild;
              lpiDstChild.SetPPGLCur( m_pgnDstTempRoot->PPGLChildHead() );
              _TyGNIndex  iChildren = 0;
              for ( lpiSrcChild.SetPPGLCur( lpiSrcParent.PGNParent()->PPGLChildHead() );
                    !lpiSrcChild.FIsLastChild();
                    iChildren++,
                      lpiSrcChild.NextChild() ) 
                {
                  if (  fHaveDstParents &&
                        ( lpiSrcChild.PGLCur() == lpiSrcParent.PGLCur() ) )
                    {
                      // Then this destination link has already been allocated - this allocation
                      //  is owned by its child - to which it is connected - save this
                      //  position in the parent's child list - we will insert the link after allocating
                      //  and inserting all of the others ( this maintains throw-safety - allocation ownership ):
                      ppglCurParentDst = lpiDstChild.PPGLCur();
                    }
                  else
                    {
                      __SDP(  typename t_TyGraphDst::_TyGraphLink, 
                              typename t_TyGraphDst::_TyGraphLinkAllocator,
                              m_rDst.get_link_allocator(), pglNew );
                      __SDP_PTRREF( pglNew ) = m_rDst._allocate_link( );
                      __SDP_CHECK_VOID( pglNew );
                      // Don't construct the link now - as this link now - but use internal initialization:
                      pglNew->Init();
                      pglNew->SetParentNode( m_pgnDstTempRoot );
                      pglNew->SetChildNode( 0 );  // not connected to the child initially - this allows clean
                      //  destruction in the case of a throw.
                      lpiDstChild.InsertLinkBeforeChild( __SDP_TRANSFER( pglNew ) ); // now owned by graph node.

                      if ( lpiSrcChild.PGLCur() == lpiSrcParent.PGLCur() )
                        {
                          ppglCurParentDst = lpiDstChild.PPGLCur(); // This needs to be connected to the child below.
                        }
                      else
                        {
                          // Associate the destination and source links in the lookup table:
                          _TyULValueType ulInsert( lpiSrcChild.PGLCur(), lpiDstChild.PGLCur() );
                          InsertLink( ulInsert );// throws.
                        }

                      lpiDstChild.NextChild(); // Move to point to the end of the list.
                    }
                }
              assert( ppglCurParentDst );

              typename t_TyGraphDst::_TyGraphLink * pglCurParentDst = 
                fHaveDstParents ? lpiDstParent.PGLCur() : *ppglCurParentDst;

              if ( 1 < iChildren )  // More than one to connect.
                {
                  assert( true == pibFoundNode.second );
                  // We have multiple children and this node was not in the map:
                  // already inserted - get the reference from the map:
                  _TyUNValueType & rvtInsert = *pibFoundNode.first;
                  assert( m_pgnDstTempRoot );
                  rvtInsert.second.m_pgnbDst = m_pgnDstTempRoot;
                  rvtInsert.second.m_iRemainingUnmatched = --iChildren;
                }

              // Now we have created all the links and the node and possibly registered the node as disconnected,
              //  need to:
              // a) Construct the link - may throw - therefore do this first - graph doesn't "own" the link
              //    element ( i.e. doesn't need to destruct the link element ) until that link element
              //    has both a parent and child node ( under the current paradigm ). If we want to allow constructed
              //    link elements that lack a child and/or parent then we will need a flag that indicates 
              //    whether the link element has been constructed. This brings up whether the link needs both
              //    the nodes involved to be constructed.
              // b) Then link the newly created sub-graph into the current destination sub-graph.
              // c) Transfer ownership of the newly created sub-graph - now owned by current destination sub-graph.
              m_rDst.template _construct_link_el1
                < const typename t_TyGraphSrc::_TyGraphLink::_TyLinkEl & >
                ( pglCurParentDst, *lpiSrcParent );

              // no throwing statements until the destination owns the new graph ( else we leak the link element ).

              if ( fHaveDstParents )
                {
                  // We already had the parent link allocated and inserted into the destination parent list.
                  // Link to the new parent node - this transfers ownership.
                  pglCurParentDst->InsertChild( ppglCurParentDst ); // should not throw.
                  pglCurParentDst->SetParentNode( m_pgnDstTempRoot );
                }
              else
                {
                  // If this statement can throw then we should use a smart destructor holder ( the allocation
                  //  however is owned by the node and is cleaned up via fcdCleanupNew ).
                  // Link into current destination subgraph:
                  lpiDstParent.InsertLinkBeforeParent( pglCurParentDst );
                  pglCurParentDst->SetChildNode( m_pgnDst );  // This shouldn't throw.
                }

              // we now own the new subgraph, no longer clean up on throw:
              m_pgnDstTempRoot = 0; // Ownership of newly allocated subgraph is transferred.

              // Move to parent node - perhaps push current context:
              if ( lpiSrcParent.PGNParent()->FParents() )
                {
                  assert( lpiSrcParent.PGNChild() == m_pgnSrc );
                  assert( pglCurParentDst->PGNChild() == m_pgnDst );
                  assert( lpiDstParent.PGNChild() == m_pgnDst );

                  m_pgnSrc = lpiSrcParent.PGNParent();
                  m_pgnDst = pglCurParentDst->PGNParent();

                  // Push this context if we have remaining source parents to copy:
                  lpiSrcParent.NextParent();
                  if ( !lpiSrcParent.FIsLastParent() )
                    {
                      _TyContext  ctxt;
                      ctxt.m_ppglbSrc = lpiSrcParent.PPGLBCur();
                      ctxt.m_ppglbDst = lpiDstParent.PPGLBCur(); // inc later.

                      __THROWPT( e_ttMemory );
                      m_lContexts.push_front( ctxt ); // throws.
                      m_iContexts++;
                    }

                  lpiSrcParent.SetPPGLCur( m_pgnSrc->PPGLParentHead() );
                  lpiDstParent.SetPPGLCur( m_pgnDst->PPGLParentHead() );

                  continue; // Need to skip code below.
                }
              // else continue to next parent.
            }
          else
            {
              //assert( !fHaveDstParents ); // if the node is in the map then we should not
              //  already have the destination parents.
              // REVIEW: Unless we just created this unfinished node during this iteration.
              // Leave the assertion above to ensure that the test cases hit this problem.
              // The solution is to that problem is merely to remove the node following
              //  the newly inserted node - the allocated node being used as a place-holder.

              // Then we need to search for the corresponding link - if it is the first of
              //  the unmatched then increment the unmatched.
              _TyUNValueType &  rdnFound = *pibFoundNode.first;

              _TyULIterator itFoundNodeLink = ITFindLink( lpiSrcParent.PGLBCur() );
              assert( ITEndLinks() != itFoundNodeLink );  // it had better be there :-)
              _TyGraphLinkBaseBaseDst * pglbFoundDst = (*itFoundNodeLink).second;
              assert( pglbFoundDst );

              // REVIEW: This is not necessary - may be faster not to remove the link.
              // Except when doing a closed directed copy - then we need to know which links remain.
              RemoveLink( itFoundNodeLink );  

              if ( !--rdnFound.second.m_iRemainingUnmatched )
                {
                  // Then we have matched all the links - node is fully connected - remove from map:
                  RemoveNode( pibFoundNode.first );
                  // don't use rdnFound anymore.
                }

              // The link has been initialized but not constructed:
              m_rDst.template _construct_link_el1
                < const typename t_TyGraphDst::_TyGraphLink::_TyLinkEl & >
                (  static_cast< _TyGraphLinkDst * >( pglbFoundDst ), 
                   *lpiSrcParent );

              // No throwing until graph owns this node.

              // Remove any place-holder link:
              // REVIEW: Since this would be rare - we could save the allocated link for re-use:
              if ( fHaveDstParents )
                {
                  _TyGraphLinkDst * pglDealloc = lpiDstParent.PGLCur();
                  lpiDstParent.PGLCur()->RemoveParent();
                  m_rDst._deallocate_link( pglDealloc );
                }

              lpiDstParent.InsertLinkBeforeParent( pglbFoundDst );
              pglbFoundDst->SetChildNode( m_pgnDst ); // Graph now owns the constructed link.
            }
        }

      // continue on to next parent.
      // If we have come to the end of the parents then we must pop a context or return:
      lpiSrcParent.NextParent();
      if ( lpiSrcParent.FIsLastParent() )
        {
          if ( m_iContexts )
            {
              _TyContext & _rctxt = m_lContexts.front();
              lpiSrcParent.SetPPGLBCur( _rctxt.m_ppglbSrc );
              lpiDstParent.SetPPGLBCur( _rctxt.m_ppglbDst );
              assert( !lpiSrcParent.FIsLastParent() );
              assert( !lpiDstParent.FIsLastParent() );
              m_lContexts.pop_front();
              m_iContexts--;
              // Restore the current nodes:
              m_pgnSrc = lpiSrcParent.PGNChild();
              m_pgnDst = lpiDstParent.PGNChild();
              lpiDstParent.NextParent();  // Increment after extracting child.
            }
          // Otherwise loop will exit.
        }
      else
        {
          lpiDstParent.NextParent();
        }
    }
}

}; // struct _graph_copy_struct

__DGRAPH_END_NAMESPACE

#endif __GR_COPY_H
