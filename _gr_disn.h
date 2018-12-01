#ifndef __GR_DISN_H
#define __GR_DISN_H

// _gr_disn.h

// Graph disconnected node stuff.
// This stuff shared by _gr_gitr.h, _gr_copy.h, _gr_inpt.h.

__DGRAPH_BEGIN_NAMESPACE

#define __GR_DSIN_USEHASH // Use a hash table instead of an rbtree.

template < class t_TyGraphNodeBaseDst >
struct _gc_unconnected_node
{
  t_TyGraphNodeBaseDst *  m_pgnbDst;
  _TyGNIndex              m_iRemainingUnmatched;
  int                     m_iVisitOrder;  // Even though the copy routine doesn't use this.
};

template < class t_TyGraphLinkBaseDst >
struct _gc_unconnected_link
{
  t_TyGraphLinkBaseDst *  m_pglDst;
};

template < class t_TyGraphNodeBase >
struct _gfi_unfinished_node_array_el
{
private:
  typedef _gfi_unfinished_node_array_el< t_TyGraphNodeBase >  _TyThis;
public:
  t_TyGraphNodeBase * m_pgnbUnfinished;
  int                 m_iVisitOrder;  // This is only here for the sorting step.

  bool  operator < ( _TyThis const & _r ) const _STLP_NOTHROW
  {
    return m_iVisitOrder < _r.m_iVisitOrder;
  }
};

template < class t_TyGraphNodeBase >
struct _gfi_unfinished_node_with_remaining :
  public _gfi_unfinished_node_array_el< t_TyGraphNodeBase >
{
  int m_iRemainingUnmatched;
};

__DGRAPH_END_NAMESPACE

#endif //__GR_DISN_H
