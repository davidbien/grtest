#ifndef __GR_DTOR_H
#define __GR_DTOR_H

// _gr_dtor.h

// This module provides functions for destructing graph nodes.

__DGRAPH_BEGIN_NAMESPACE

template < class t_TyGraphBase >  struct _graph_destroy_base;

template < class t_TyGraph >
struct _graph_destroy_struct
  : public _graph_destroy_base< typename t_TyGraph::_TyGraphBaseBase >
{
private:
  typedef _graph_destroy_struct< t_TyGraph >                          _TyThis;
  typedef _graph_destroy_base< typename t_TyGraph::_TyGraphBaseBase > _TyBase;
public:

  _graph_destroy_struct(  t_TyGraph & _rg,
                          typename t_TyGraph::_TyGraphNode * _pgn )
    : _TyBase( _pgn ),
      m_rg( _rg )
  {
    // Set up callbacks in base:
    _TyBase::m_pmfnDeallocateNode = static_cast< typename _TyBase::_TyPMFnDeallocateNode >( &_TyThis::_DeallocateNode );
    _TyBase::m_pmfnDestructNode = static_cast< typename _TyBase::_TyPMFnDestructNode >( &_TyThis::_DestructNode );
    _TyBase::m_pmfnDeallocateLink = static_cast< typename _TyBase::_TyPMFnDeallocateLink >( &_TyThis::_DeallocateLink );
    _TyBase::m_pmfnDestructLink = static_cast< typename _TyBase::_TyPMFnDestructLink >( &_TyThis::_DestructLink );
  }

  t_TyGraph & m_rg;

protected:

  typedef typename t_TyGraph::_TyGraphNode          _TyGraphNode;
  typedef typename t_TyGraph::_TyGraphLink          _TyGraphLink;
  typedef typename t_TyGraph::_TyGraphNodeBaseBase  _TyGraphNodeBaseBase;
  typedef typename t_TyGraph::_TyGraphLinkBaseBase  _TyGraphLinkBaseBase;

  void  _DeallocateNode( _TyGraphNodeBaseBase * _pgnb )
  { 
    assert( _pgnb );
    m_rg._deallocate_node( static_cast< _TyGraphNode* >( _pgnb ) );
  }
  void  _DestructNode( _TyGraphNodeBaseBase * _pgnb )
  {
    assert( _pgnb );
    m_rg._destruct_node( static_cast< _TyGraphNode* >( _pgnb ) );
  }

  void  _DeallocateLink( _TyGraphLinkBaseBase * _pglb )
  {
    assert( _pglb );
    m_rg._deallocate_link( static_cast< _TyGraphLink* >( _pglb ) );
  }
  void  _DestructLink( _TyGraphLinkBaseBase * _pglb )
  {
    assert( _pglb );
    m_rg._destruct_link( static_cast< _TyGraphLink* >( _pglb ) );
  }
};

template < class t_TyGraphBase >
struct _graph_destroy_base
{
private:
  typedef _graph_destroy_base< t_TyGraphBase >  _TyThis;
public:
  typedef typename t_TyGraphBase::_TyGraphNodeBase      _TyGraphNodeBase;
  typedef typename t_TyGraphBase::_TyGraphLinkBase      _TyGraphLinkBase;

  _graph_destroy_base( _TyGraphNodeBase * _pgnb )
    : m_pgnbDestroy( _pgnb )
  {
  }

  typedef void  ( _TyThis :: * _TyPMFnDeallocateNode )( _TyGraphNodeBase * );
  _TyPMFnDeallocateNode m_pmfnDeallocateNode;

  typedef void ( _TyThis :: * _TyPMFnDestructNode )( _TyGraphNodeBase * );
  _TyPMFnDestructNode m_pmfnDestructNode;

  typedef void  ( _TyThis :: * _TyPMFnDeallocateLink )( _TyGraphLinkBase * );
  _TyPMFnDeallocateLink m_pmfnDeallocateLink;

  typedef void ( _TyThis :: * _TyPMFnDestructLink )( _TyGraphLinkBase * );
  _TyPMFnDestructLink m_pmfnDestructLink;

  // On entry this points to the graph node from which destruction is to begin - 
  //  all nodes connected to this node will be destroyed.
  // On exit the contents of this pointer are undefined ( remains the same - but node is deallocated ).
  _TyGraphNodeBase *          m_pgnbDestroy;

// This method needs to check for un-connected links - i.e. a link one side (relation) of which is NULL.
// These are created in some algorithms ( by design ). These links' elements have already been 
//  destructed and certainly don't need to be again.
void
destroy( ) _STLP_NOTHROW
{
  // Design Goals:
  //  1) Don't allocate any memory - even using _alloca() - since this maps to free store
  //    on some systems. Of course this means that the current structure of the graph
  //    must allow destruction of all the nodes - i.e. nodes connected to the graph must
  //    remain connected to the graph ( or destroyed ) after each step in the algorthm.
  //  
  // Algorithm:
  //  The node to be destroyed will "eat" the rest of the graph. After this process is complete
  // only <_pgnDestroy> will remain - and then it will be destroyed.
  //  A node is "eaten" by destroying the link between the <_pgnDestroy> and then adding any
  // remaining relations ( except for relations that are directly <_pgnDestroy> ) as relations
  // of <_pgnDestroy>. Start with either parents or children until no parents or children remain.

  // Set up initial state - we need to remember the current end of both the parent and child
  //  list of m_pgnbDestroy - we will be appending links to both lists:
  for ( m_ppglbTailChildren = m_pgnbDestroy->PPGLBChildHead();
        *m_ppglbTailChildren;
        m_ppglbTailChildren = (*m_ppglbTailChildren)->PPGLBGetNextChild() )
  { 
  }
  for ( m_ppglbTailParents = m_pgnbDestroy->PPGLBParentHead();
        *m_ppglbTailParents;
        m_ppglbTailParents = (*m_ppglbTailParents)->PPGLBGetNextParent() )
  { 
  }

  // Each pass may create relations for the other - continue until none left:
  while ( m_pgnbDestroy->FChildren() || m_pgnbDestroy->FParents() )
  {
    _destroy_children();
    _destroy_parents();
  }

  // Destroy and deallocate the child node:
  (this->*m_pmfnDestructNode)( m_pgnbDestroy );
  (this->*m_pmfnDeallocateNode)( m_pgnbDestroy );
}

private:

  _TyGraphNodeBase *  m_pgnbRelation;
  _TyGraphLinkBase ** m_ppglbTailChildren;
  _TyGraphLinkBase ** m_ppglbTailParents;
#ifdef __GRAPH_DEBUG_DTOR
  _TyGraphNodeBase    m_pgnbDebugCheck;
public:
  void  SetCheckNode( _TyGraphNodeBase * _pgn )
  {
    m_pgnbDebugCheck = _pgn;
  }
private:
#endif //__GRAPH_DEBUG_DTOR

void
_destroy_children() _STLP_NOTHROW
{
  while( m_pgnbDestroy->FChildren() )
  {
    _TyGraphLinkBase * pglbChildHead = *( m_pgnbDestroy->PPGLBChildHead() );

    assert( pglbChildHead->PGNBParent() == m_pgnbDestroy ); // This assertion firing means that the graph is corrupt.

    // If this is a unconnected link then it has already been destructed:
    if ( !!( m_pgnbRelation = pglbChildHead->PGNBChild() ) )
    {
#ifdef __GRAPH_DEBUG_DTOR
      // We should not run into <m_pgnbDebugCheck> 
      // ( this checks destruction of subgraphs supposedly not connected to the root ).
      assert( m_pgnbDebugCheck != m_pgnbRelation );
#endif //__GRAPH_DEBUG_DTOR

      // Destruct the child link:
      (this->*m_pmfnDestructLink)( pglbChildHead );

      // Then must check if the child is the marker node and if so potentially update the parent tail:
      if ( ( m_pgnbRelation == m_pgnbDestroy ) &&
           ( pglbChildHead->PPGLBGetNextParent() == m_ppglbTailParents ) )
      {
        m_ppglbTailParents = pglbChildHead->PPGLBGetThisParent();
      }

      // Unlink from the child's parent list:
      pglbChildHead->RemoveParent();

      // This algorithm does not create single-link cycles connected to {m_pgnbDestroy}
      //  but if any are present in the pre-destruction graph this will destroy appropriately:
      if ( m_pgnbRelation != m_pgnbDestroy )
      {
        // Append all remaining relations of this child node to m_pgnbDestroy:
        _append_relations();

        // Destroy and deallocate the child node:
        (this->*m_pmfnDestructNode)( m_pgnbRelation );
        (this->*m_pmfnDeallocateNode)( m_pgnbRelation );
      }
    }

    // If this is the tail of the child list for m_pgnbDestroy then update:
    if ( m_ppglbTailChildren == pglbChildHead->PPGLBGetNextChild() )
    {
      m_ppglbTailChildren = pglbChildHead->PPGLBGetThisChild();
    }
    // Unlink from the parent's child list and deallocate:
    pglbChildHead->RemoveChild();
    (this->*m_pmfnDeallocateLink)( pglbChildHead );
  }
}

void
_destroy_parents() _STLP_NOTHROW
{
  // *** Code copied from _destroy_children(), juxtapose ( Children, Parents ), 
  //  ( children, parents ), ( child, parent ), ( Child, Parent ).
  // *** do not edit this routine.
  while( m_pgnbDestroy->FParents() )
  {
    _TyGraphLinkBase * pglbParentHead = *( m_pgnbDestroy->PPGLBParentHead() );

    assert( pglbParentHead->PGNBChild() == m_pgnbDestroy ); // This assertion firing means that the graph is corrupt.

    // If this is a unconnected link then it has already been destructed:
    if ( !!( m_pgnbRelation = pglbParentHead->PGNBParent() ) )
    {
#ifdef __GRAPH_DEBUG_DTOR
      // We should not run into <m_pgnbDebugCheck> 
      // ( this checks destruction of subgraphs supposedly not connected to the root ).
      assert( m_pgnbDebugCheck != m_pgnbRelation );
#endif //__GRAPH_DEBUG_DTOR

      // Destruct the parent link:
      (this->*m_pmfnDestructLink)( pglbParentHead );

      // Then must check if the parent is the marker node and if so potentially update the child tail:
      if ( ( m_pgnbRelation == m_pgnbDestroy ) &&
           ( pglbParentHead->PPGLBGetNextChild() == m_ppglbTailChildren ) )
      {
        m_ppglbTailChildren = pglbParentHead->PPGLBGetThisChild();
      }

      // Unlink from the parent's child list:
      pglbParentHead->RemoveChild();

      // This algorithm does not create single-link cycles connected to {m_pgnbDestroy}
      //  but if any are present in the pre-destruction graph this will destroy appropriately:
      if ( m_pgnbRelation != m_pgnbDestroy )
      {
        // Append all remaining relations of this parent node to m_pgnbDestroy:
        _append_relations();

        // Destroy and deallocate the parent node:
        (this->*m_pmfnDestructNode)( m_pgnbRelation );
        (this->*m_pmfnDeallocateNode)( m_pgnbRelation );
      }
    }

    // If this is the tail of the parent list for m_pgnbDestroy then update:
    if ( m_ppglbTailParents == pglbParentHead->PPGLBGetNextParent() )
    {
      m_ppglbTailParents = pglbParentHead->PPGLBGetThisParent();
    }
    // Unlink from the child's parent list and deallocate:
    pglbParentHead->RemoveParent();
    (this->*m_pmfnDeallocateLink)( pglbParentHead );
  }
}

void
_append_relations() _STLP_NOTHROW
{
  // Append the parent list to <m_pgnbDestroy>'s parent list:
  if ( m_pgnbRelation->FParents() )
  {
    _append_parents();
  }
  if ( m_pgnbRelation->FChildren() )
  {
    _append_children();
  }
}

void
_append_children() _STLP_NOTHROW
{
  (*( m_pgnbRelation->PPGLBChildHead() ))->AppendChildListToTail( m_ppglbTailChildren );

  // Move through the Children list links from the old tail updating the Parent node pointer 
  //  to m_pgnbDestroy - unless the Child node pointer is either empty or is m_pgnbDestroy - 
  //  in which case we destroy the and possibly destruct the link:
  while ( *m_ppglbTailChildren )
  {
    _TyGraphLinkBase * pglbCur = *m_ppglbTailChildren;
    m_ppglbTailChildren = pglbCur->PPGLBGetNextChild();

    if (  !pglbCur->PGNBChild() ||
          ( pglbCur->PGNBChild() == m_pgnbDestroy ) )
    {
      // Unlinking and deallocating this link and possibly destroying:
      if ( pglbCur->PGNBChild() == m_pgnbDestroy )
      {
        (this->*m_pmfnDestructLink)( pglbCur );
        // If this is the current parent tail then update the tail:
        if ( m_ppglbTailParents == pglbCur->PPGLBGetNextParent() )
        {
          m_ppglbTailParents = pglbCur->PPGLBGetThisParent();
        }
        pglbCur->RemoveParent();  // Remove from Child's Parent list.
      }
      // If the current child tail position is in this object then update
      //  before deallocating:
      if ( m_ppglbTailChildren == pglbCur->PPGLBGetNextChild() )
      {
        m_ppglbTailChildren = pglbCur->PPGLBGetThisChild();
      }
      // Remove from Parent's Child list:
      pglbCur->RemoveChild();
      (this->*m_pmfnDeallocateLink)( pglbCur );
    }
    else
    {
      // Keeping the link - change the pointer to Parent to pgnDestroy - note
      //  this may introduce a cycle - this is by design - the destruction algorthm and
      //  data structure both support cycles:
      pglbCur->SetParentNode( m_pgnbDestroy );
    }
  }
}

void
_append_parents() _STLP_NOTHROW
{
  // *** Code copied from _append_children(), juxtapose ( Children, Parents ), 
  //  ( children, parents ), ( child, parent ), ( Child, Parent ).
  // *** do not edit this routine.
  (*( m_pgnbRelation->PPGLBParentHead() ))->AppendParentListToTail( m_ppglbTailParents );

  // Move through the Parents list links from the old tail updating the Child node pointer 
  //  to m_pgnbDestroy - unless the Parent node pointer is either empty or is m_pgnbDestroy - 
  //  in which case we destroy the and possibly destruct the link:
  while ( *m_ppglbTailParents )
  {
    _TyGraphLinkBase * pglbCur = *m_ppglbTailParents;
    m_ppglbTailParents = pglbCur->PPGLBGetNextParent();

    if (  !pglbCur->PGNBParent() ||
          ( pglbCur->PGNBParent() == m_pgnbDestroy ) )
    {
      // Unlinking and deallocating this link and possibly destroying:
      if ( pglbCur->PGNBParent() == m_pgnbDestroy )
      {
        (this->*m_pmfnDestructLink)( pglbCur );
        // If this is the current child tail then update the tail:
        if ( m_ppglbTailChildren == pglbCur->PPGLBGetNextChild() )
        {
          m_ppglbTailChildren = pglbCur->PPGLBGetThisChild();
        }
        pglbCur->RemoveChild(); // Remove from Parent's Child list.
      }
      // If the current parent tail position is in this object then update
      //  before deallocating:
      if ( m_ppglbTailParents == pglbCur->PPGLBGetNextParent() )
      {
        m_ppglbTailParents = pglbCur->PPGLBGetThisParent();
      }
      // Remove from Child's Parent list:
      pglbCur->RemoveParent();
      (this->*m_pmfnDeallocateLink)( pglbCur );
    }
    else
    {
      // Keeping the link - change the pointer to Child to pgnDestroy - note
      //  this may introduce a cycle - this is by design - the destruction algorthm and
      //  data structure both support cycles:
      pglbCur->SetChildNode( m_pgnbDestroy );
    }
  }
}

};  // struct _graph_destroy_base

__DGRAPH_END_NAMESPACE

#endif //__GR_DTOR_H
