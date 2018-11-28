#ifndef __GR_SHDO_H
#define __GR_SHDO_H

// _gr_shdo.h

// shadow graph stuff

// This module implements typed graph elements that allow "shadowing" of another graph
//  ( with the same element types ).

__DGRAPH_BEGIN_NAMESPACE

template <  class t_TyNodeEl, class t_TyLinkEl, class t_TyGraphNodeBase,
            class t_TyShadowNode, class t_TyShadowLink >
class _sgraph_node;

template < class t_TyShadowObjectSafe >
struct _sgraph_element_base
{
private:
  typedef _sgraph_element_base< t_TyShadowObjectSafe >  _TyThis;
public:
  t_TyShadowObjectSafe *  m_psos;
  _TyConnectionLink       m_gcl;

  explicit _sgraph_element_base( t_TyShadowObjectSafe * _psos )
    : m_psos( _psos )
  {
    m_gcl.m_egclType = s_egclShadowGraphObject;
    m_gcl.m_pvConnection = this;
    _Connect();
  }
  explicit _sgraph_element_base( _TyThis const & _r )
    : m_psos( _r.m_psos )
  {
    m_gcl.m_egclType = s_egclShadowGraphObject;
    m_gcl.m_pvConnection = this;
    if ( m_psos )
    {
      _Connect();
    }   
  }

  ~_sgraph_element_base()
  {
    if ( m_psos )
    {
      m_gcl.remove_link();
    }
  }
  
  void  _Connect()
  {
    m_psos->PushConnection( &m_gcl );
  }
  void  _Disconnect()
  {
    assert( m_psos );
    m_gcl.remove_link();
    m_psos = 0;
  }

  void  _element_deinit()
  {
    m_psos = 0;
  }

protected:
  void  _ValidatePSOS() const
  {
#ifndef NDEBUG
    if ( !m_psos )
    {
      throw bad_shadow_element( "_ValidatePSOS(): Null graph element." );
    }
#endif !NDEBUG
  }
};

template < class t_TyShadowObject >
struct _sgraph_element
  : public _sgraph_element_base< typename t_TyShadowObject::_TyGraphObjectSafe >
{
private:
  typedef _sgraph_element_base< typename t_TyShadowObject::_TyGraphObjectSafe > _TyBase;
  typedef _sgraph_element< t_TyShadowObject >                                   _TyThis;
public:

  typedef typename t_TyShadowObject::_TyElement _TyShadowElement;
  
  _sgraph_element( t_TyShadowObject * _pso = 0 )
    : _TyBase( _pso )
  {
  }
  explicit _sgraph_element( _TyThis const & _r )
    : _TyBase( _r )
  {
  }

  t_TyShadowObject *  PSO() const __STL_NOTHROW
  {
    return static_cast< t_TyShadowObject* >( m_psos );
  }

  // Shadow element access:
  _TyShadowElement &  REl()
  {
    return RElNonConst();
  }
  const _TyShadowElement &  REl() const
  {
    return RElConst();
  }

  _TyShadowElement &  RElNonConst()
  {
    _ValidatePSOS();
    return PSO()->RElNonConst();
  }
  _TyShadowElement &  RElConst() const
  {
    _ValidatePSOS();
    return PSO()->RElConst();
  } 
};

__DGRAPH_END_NAMESPACE

#endif __GR_SHDO_H
