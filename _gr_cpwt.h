#ifndef __GR_CPWT_H
#define __GR_CPWT_H

// _gr_cpwt.h

// copy on write graph elements
// This is related to shadowing - except when non-const shadowed elements are accessed - they are copied
//  from the shadowed graph before being returned to the caller.

// This implementation places the element in-place in the structure with the idea that
//  elements are generally small. This also necessates no knowledge of the allocator.
// If larger elements ( than say 64 bytes ) are needed then a new implementation which allocates
//  dynamic elements can be created.

__DGRAPH_BEGIN_NAMESPACE

template < class t_TyShadowObjectSafe, int t_iSizeOfElement >
struct _cpwt_graph_element_base
{
private:
  typedef _cpwt_graph_element_base< t_TyShadowObjectSafe >  _TyThis;
public:

  t_TyShadowObjectSafe *  m_psos; // If this is null then we have an element.
  struct
  {
    _TyConnectionLink       m_gcl;
    char                    m_cpEl[ t_iSizeOfElement ];
  };
  
  _cpwt_graph_element_base()
    : m_psos( 0 )
  {
  }

  explicit _cpwt_graph_element_base( t_TyShadowObjectSafe * _psos )
    : m_psos( _psos )
  {
    m_gcl.m_egclType = s_egclShadowGraphObject;
    m_gcl.m_pvConnection = this;
    _Connect();
  }
  explicit _cpwt_graph_element_base( _TyThis const & _r )
    : m_psos( _r.m_psos )
  {
    if ( m_psos )
    {
      m_gcl.m_egclType = s_egclShadowGraphObject;
      m_gcl.m_pvConnection = this;
      _Connect();
    }
  }

  ~_cpwt_graph_element_base()
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
};

// This is one implementation of copy-on-write - it is limited to shadowing graphs that
//  are not shadow elements themselves. This is a pretty severe limitation - unless the
//  usage paradigm is that then of "master graph" with non-interacting "slave graphs".
template < class t_TyShadowObject >
struct _cpwt_graph_element
  : public _cpwt_graph_element_base<  typename t_TyShadowObject::_TyGraphObjectSafe,
                                      sizeof( typename t_TyShadowObject::_TyElement ) >
{
private:
  typedef _cpwt_graph_element_base< typename t_TyShadowObject::_TyGraphObjectSafe,
                                    sizeof( typename t_TyShadowObject::_TyElement ) > _TyBase;
  typedef _cpwt_graph_element< t_TyShadowObject >                                     _TyThis;
public:

  typedef typename t_TyShadowObject::_TyElement _TyShadowElement;
  
  explicit _cpwt_graph_element( t_TyShadowObject * _pso )
    : _TyBase( _pso )
  {
  }
  explicit _cpwt_graph_element( _TyThis const & _r )
    : _TyBase( _r )
  {
    if ( !m_psos )
    {
      // Then we need to call the copy constuctor on the element -
      //  we can't shadow a shadow element ( unless it is also shadowed ):
      // REVIEW: Perhaps we could - if all elements in the graph were shadowed.
      _RElInt()._TyShadowElement::_TyShadowElement( _r._RElInt() );
    }
  }

  explicit _cpwt_graph_element( _TyShadowElement const & _r )
    : _TyBase()
  {
    _RElInt()._TyShadowElement::_TyShadowElement( _r );
  }

  t_TyShadowObject *  PSO() const __STLP_NOTHROW
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
    if ( m_psos )
    {
      _CopyOnWrite();
    }
    return _RElInt();
  }

  _TyShadowElement &  RElConst() const
  {
    return m_psos ? PSO()->RElConst() : _RElInt();
  }

  // REVIEW: Since we are a destructor we are not expected to throw - but
  //  since we need to copy the element on destruct we will be left in an inconsistent
  //  state ( and therefore probably should throw ). Since this is a rather intractable problem
  //  perhaps we should though a severe exception ( to indicate that data has been lost ).
  // One problem is that the graph that was being destroyed is now likely totally screwed up - 
  //  i.e. cycles have been introduced. This graph will be screwed up too - since we were unable
  //  to appropriately copy the element. All things point at a specialized exception indicating
  //  which graphs are screwed up. This would allow an app to not honor data from them. Since there
  //  is no way to destroy the graph and then throw an exception ( without special code in _gr_dtor ).
  // Another problem is that we don't even know which graph this is ( to which we are connected ).
  void  _element_deinit()
  {
    // We copy the element as if a write:
    _BIEN_TRY
    {
      _CopyOnWrite();
    }
#ifdef _BIEN_USE_EXCEPTIONS
    catch( ... )
    {
      // This graph is inconsistent - throw an inconsistent graph exception:
      throw inconsistent_graph( "_cpwt_graph_element::_element_deinit(): Throw during copy-on-write." );
    }
#endif //_BIEN_USE_EXCEPTIONS
  }

protected:
  _TyShadowElement & _RElInt() const __STLP_NOTHROW
  {
    return *((_TyShadowElement*)m_cpEl);
  }
  void _CopyOnWrite()
  {
    assert( m_psos );
    m_gcl.remove_link();  // Disconnect from graph element - we are overwriting {m_gcl}.
    _BIEN_TRY
    {
      _RElInt()._TyShadowElement::_TyShadowElement( RElConst() );
    }
    _BIEN_UNWIND( _TyBase::_TyBase( m_psos ) ); // Re-connect if we throw while copying.
    m_psos = 0;
  }
};

__DGRAPH_END_NAMESPACE

#endif //__GR_CPWT_H
