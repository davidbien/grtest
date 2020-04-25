#ifndef __GR_SITR
#define __GR_SITR

// _gr_sitr.h

// This module implements safe iterators for the graph.

// REVIEW: TODO: Why have an extra memory object for the connection link when there is a one-to-one
//	correspondence between links and iteration objects <and> the iteration object owns the object
//	lifetime of the connection link? In other words they could share the same memory object with ease.
// It makes exception handling much easier as well.

// Safe node iterator - a connection is maintained between the node and the iterator
//	such that if the node is destroyed then the iterator's pointer is nullified,
// Could allow user data and callback function 
template < class t_TyGraphNodeSafe, class t_TyGraphLinkSafe >
class _graph_node_iterator_base_safe
	: public _graph_node_iterator_base_notsafe< typename t_TyGraphNodeSafe::__TyGraphNodeBase, 
																							typename t_TyGraphLinkSafe::__TyGraphLinkBase >
{
protected:
	// Get the base classes from the safe classes:
	typedef typename t_TyGraphNodeSafe::__TyGraphNodeBase	_TyGraphNodeBase;
	typedef typename t_TyGraphLinkSafe::__TyGraphLinkBase	_TyGraphLinkBase;

private:
	typedef _graph_node_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >	_TyIterBase;
	typedef _graph_node_iterator_base_safe<	t_TyGraphNodeSafe, t_TyGraphLinkSafe >	_TyThis;

protected:

	_TyGraphConnectionLink	m_gclNode;	// link from graph-object to this object.

	void	_Reference( _TyGraphNodeBase * _pgnbCur ) __STLP_NOTHROW
	{
		assert( !PGNSCur() );		// should always have valid or NULL value when graph connection is not referencing.
		m_pgnbCur = _pgnbCur;		// No throw after this.
		PGNSCur()->PushConnection( &m_gclNode );	// Crash here ? Perhaps you mixed non-safe and safe graphs.
	}

	void	_Dereference() __STLP_NOTHROW
	{
		if ( PGNBCur() )
		{
			m_gclNode.remove_link();
		}
	}

	// The graph node to which this iterator is connected is being deinitialized.
	// We don't need to unlink the connection link.
	void	_node_deinit() __STLP_NOTHROW
	{
		assert( PGNBCur() );
		_TyIterBase::Clear();
	}

	void	_init() __STLP_NOTHROW
	{
		m_gclNode.m_pvConnection = (void*)this;
		m_gclNode.m_egclType = s_egclGraphNodeIterator;
	}

public:

	explicit _graph_node_iterator_base_safe(	t_TyGraphNodeSafe * _pgnbCur ) __STLP_NOTHROW
		: _TyIterBase( 0 )	// In case of throw.
	{
		_init();
		if ( _pgnbCur )
		{
			_Reference( _pgnbCur );
		}
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class t_TySafeNodeIter >
	explicit _graph_node_iterator_base_safe( const t_TySafeNodeIter & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_node_iterator_base_safe( const _TyThis & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		:	_TyIterBase( 0 )
	{
		_init();
		if ( _r.PGNBCur() )
		{
			_Reference( _r.PGNBCur() );
		}
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class t_TySafeNodeIter >
	explicit _graph_node_iterator_base_safe( const t_TySafeNodeIter & _r, std::false_type ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_node_iterator_base_safe( const _TyThis & _r, std::false_type ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		:	_TyIterBase( 0 )
	{
		_init();
		__semantic_error_object	error;	// Does this get called and if so why ?
	}

// Initialize with a reference and a different allocator:
#ifdef _STLP_MEMBER_TEMPLATES
	// This is not as safe as it might be - but misuse would involve assigning from the
	//	iterator of another ( non-safe ) graph - which is a bug in itself - perhaps we
	//	should record graph identities in debug to ensure iterators are used on the
	//	appropriate graph. The way around this ( not the actual bug ) is to templatize the 
	//	non-safe graph node iterator base class by the safe graph node and link structs - this 
	//	way no mingling can occur. The problem with that is that if ( for some reason ) we 
	//	want multiple graph types in the implementation ( some safe, some not ) then the base 
	//	graph code will not be shared between the two ( unless the compiler is smart ) - this 
	//	is very easy to test for a given compiler - just change the template above and then 
	//	see if the code gets larger ( or slower ). Of course it is very safe to initialize 
	//	with a non-safe iterator that iterates safe graph nodes.
	template < class t_TyNodeIter >
	explicit _graph_node_iterator_base_safe(	const t_TyNodeIter & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_node_iterator_base_safe(	const _TyThis & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		:	_TyIterBase( 0 )
	{
		_init();
		if ( _r.PGNBCur() )
		{
			_Reference( _r.PGNBCur() );
		}
	}

	~_graph_node_iterator_base_safe() __STLP_NOTHROW
	{
		_Dereference();
	}

	void	Clear() __STLP_NOTHROW
	{
		_Dereference();
		_TyIterBase::Clear();
	}

	t_TyGraphNodeSafe *			PGNSCur() const __STLP_NOTHROW
	{
		return static_cast< t_TyGraphNodeSafe * >( m_pgnbCur );
	}

#ifdef _STLP_MEMBER_TEMPLATES
	// Once again this can be misused when multiple graph types ( safe and non-safe ) are mixed.
	template < class t_TyNodeIter >
	_TyThis &	operator = ( const t_TyNodeIter & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	_TyThis &	operator = ( const _TyThis & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
	{
		SetPGNBCur( _r.PGNBCur() );
		return *this;
	}

	void			SetPGNBCur( _TyGraphNodeBase * _pgnbCur ) __STLP_NOTHROW
	{	
		if ( PGNBCur() )
		{
			m_gclNode.remove_link();
			if ( m_pgnbCur = _pgnbCur )
			{
				PGNSCur()->PushConnection( &m_gclNode );
			}
		}
		else
		if ( _pgnbCur )
		{
			_Reference( _pgnbCur );
		}
	}

	// Set to value of passed non-NULL node - assume currently populated link - won't throw.
	void			SetPGNBCur_Populated( t_TyGraphNodeBase * _pgnb ) __STLP_NOTHROW
	{
		assert( m_gclNode );
		assert( _pgnb );
		m_gclNode.remove_link();
		(m_pgnbCur = _pgnb)->PushConnection( &m_gclNode );
	}

#ifdef _STLP_MEMBER_TEMPLATES
	// Set to value of passed iterator - assume currently populated link - won't throw.
	// Typesafe problem when multiple graph types ( safe and non-safe ) are mixed.
	template < class t_TyNodeIter >
	void			Set_Populated( const t_TyNodeIter & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	void			Set_Populated( const _TyThis & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
	{
		SetPGNBCur_Populated( _r.PGNBCur() );
	}

	void					GoParent( _TyGNIndex _u ) __STLP_NOTHROW
	{
		assert( m_pgnbCur );
		// Remove the reference to the current node and re-use it to reference the
		//	new node:
		m_gclNode.remove_link();
		_TyIterBase::GoParent( _u );
		PGNSCur()->PushConnection( &m_gclNode );
	}
	void					GoChild( _TyGNIndex _u ) __STLP_NOTHROW
	{
		assert( m_pgnbCur );
		// Remove the reference to the current node and re-use it to reference the
		//	new node:
		m_gclNode.remove_link();
		_TyIterBase::GoChild( _u );
		PGNSCur()->PushConnection( &m_gclNode );
	}

	// relation operators:
	// Re-declare so that gets overridden methods:
	void					GoRelation( bool _fChild, _TyGNIndex _u ) __STLP_NOTHROW
	{
		if ( _fChild )	
			GoChild( _u );
		else
			GoParent( _u );
	}

};

template < class t_TyGraphNodeBase, class t_TyGraphLinkBase >
class _graph_link_ident_iterator_base_notsafe;

// This iterator maintains a constant position within either the child or the parent list.
// It also has an end() position that is a valid insert before position.
template < class t_TyGraphNodeSafe, class t_TyGraphLinkSafe >
class _graph_link_pos_iterator_base_safe
	: public _graph_link_pos_iterator_base_notsafe< typename t_TyGraphNodeSafe::__TyGraphNodeBase, 
																									typename t_TyGraphLinkSafe::__TyGraphLinkBase >
{
protected:
	// Get the base classes from the safe classes:
	typedef typename t_TyGraphNodeSafe::__TyGraphNodeBase	_TyGraphNodeBase;
	typedef typename t_TyGraphLinkSafe::__TyGraphLinkBase	_TyGraphLinkBase;

private:
	typedef _graph_link_pos_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >		_TyIterBase;
	typedef _graph_link_pos_iterator_base_safe<	_TyGraphNodeBase, _TyGraphLinkBase >			_TyThis;

protected:

	_TyGraphConnectionLink	m_gclObj;	// link from graph-object to this object.

	void	_init( ) __STLP_NOTHROW
	{
		m_gclObj.m_ppvConnection = (void*)this;
		m_gclObj.m_egclType = s_egclLinkPositionIterator;
	}

	void	_PushConnection( _TyGraphLinkBase ** _ppglbCur ) __STLP_NOTHROW
	{
		t_TyGraphLinkSafe *	_pgls;
		t_TyGraphNodeSafe *	_pgns;
		t_TyGraphLinkSafe::PGrObjFromPPGLB( _ppglbCur, _pgls, _pgns );
		if ( _pgls )
		{
			_pgls->PushConnection( &m_gclObj );	// Crash here ? Perhaps you mixed non-safe and safe graphs.
		}
		else
		{
			_pgns->PushConnection( &m_gclObj );	// Crash here ? Perhaps you mixed non-safe and safe graphs.
		}

	}

	void	_Reference( _TyGraphLinkBase ** _ppglbCur ) __STLP_NOTHROW
	{
		assert( !PPGLBCur() );
		_PushConnection( _ppglbCur );
		_TyIterBase::SetPPGLBCur( _ppglbCur );
	}

	void	_Dereference() __STLP_NOTHROW
	{
		if ( PPGLBCur() )
		{
			m_gclObj.remove_link();
		}
	}

	// The graph node to which this iterator is connected is being deinitialized.
	// We don't need to unlink the connection link.
	void	_node_deinit() __STLP_NOTHROW
	{
		assert( PPGLBCur() );
#ifndef NDEBUG
		t_TyGraphLinkSafe *	_pgls;
		t_TyGraphNodeSafe *	_pgns;
		t_TyGraphLinkSafe::PGrObjFromPPGLB( PPGLBCur(), _pgls, _pgns );
		assert( !_pgls && _pgns );	// We should currently be at the head of a relation list.
#endif //!NDEBUG
		
		_TyIterBase::Clear();
	}

	// The graph link to which this iterator is connected is being deinitialized.
	// We don't need to unlink the connection link.
	void	_link_deinit() __STLP_NOTHROW
	{
		assert( PPGLBCur() );
#ifndef NDEBUG
		t_TyGraphLinkSafe *	_pgls;
		t_TyGraphNodeSafe *	_pgns;
		t_TyGraphLinkSafe::PGrObjFromPPGLB( PPGLBCur(), _pgls, _pgns );
		assert( _pgls );	// We should currently not be at the head of a relation list.
#endif //!NDEBUG
		
		_TyIterBase::Clear();
	}

public:

	typedef _graph_link_ident_iterator_base_notsafe<	_TyGraphNodeBase, _TyGraphLinkBase >	_TyLinkIdentIter;

protected:

	// Initialize with child/parent position - if this iterator is located at the head
	//	of either list then the node located at that head must be supplied ( else crash ).
	explicit _graph_link_pos_iterator_base_safe( _TyGraphLinkBase ** _ppglbCur ) __STLP_NOTHROW
		: _TyIterBase( 0 )
	{
		_init();
		if ( _ppglbCur )
		{
			_Reference( _ppglbCur );
		}
	}

// Init with an iterator that has an allocator:
#ifdef _STLP_MEMBER_TEMPLATES
	template < class t_TyLinkPosIter >
	explicit _graph_link_pos_iterator_base_safe( const t_TyLinkPosIter & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_link_pos_iterator_base_safe( const _TyThis & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		: _TyIterBase( 0 )
	{
		_init();
		if ( _r.PPGLBCur() )
		{
			_Reference( _r.PPGLBCur() );
		}
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class t_TyLinkPosIter >
	explicit _graph_link_pos_iterator_base_safe( const t_TyLinkPosIter & _r, std::false_type ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_link_pos_iterator_base_safe( const _TyThis & _r, std::false_type ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		: _TyIterBase( 0 )
	{
		_init();
		__semantic_error_object	error;	// Why and when is this called.
	}

// Init with a reference to an iterator and a different allocator:
#ifdef _STLP_MEMBER_TEMPLATES
	template < class t_TyLinkPosIter >
	explicit _graph_link_pos_iterator_base_safe( const t_TyLinkPosIter & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_link_pos_iterator_base_safe( const _TyThis & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		: _TyIterBase( 0 )
	{
		_init();
		if ( _r.PPGLBCur() )
		{
			_Reference( _r.PPGLBCur() );
		}
	}

public:

	void	Clear() __STLP_NOTHROW
	{
		_Dereference();
		_TyIterBase::Clear();
	}

// Accessors:
	void		SetPPGLBCur( _TyGraphLinkBase ** _ppglbCur ) __STLP_NOTHROW
	{
		if ( PPGLBCur() )
		{ 
			m_gclObj.remove_link();
			if ( m_ppglbCur = _ppglbCur )
			{
				_PushConnection( m_ppglbCur );
			}
		}
		else
		if ( _ppglbCur )
		{
			_Reference( _ppglbCur );
		}
	}

	// Set a non-NULL <_pglbCur> into an iterator:
	void		SetPPGLBCur_Populated( _TyGraphLinkBase ** _ppglbCur ) __STLP_NOTHROW
	{	
		assert( _ppglbCur );
		if ( PPGLBCur() )
		{
			m_gclObj.remove_link();
			m_ppglbCur = _ppglbCur;
			_PushConnection( m_ppglbCur );
		}
		else
		{
			_Reference( _ppglbCur );
		}
	}

	// Node operations - these are the same as the node iterator:

	void					GoParent( _TyGNIndex _u ) __STLP_NOTHROW
	{
		if ( *m_ppglbCur )
		{
			m_gclObj.remove_link();
			if ( _u )
			{
				_TyIterBase::GoParent( _u );
				t_TyGraphLinkSafe * _pgls = static_cast< t_TyGraphLinkSafe * >( _TyGraphLinkBase::PGLBGetThisFromPPGLBNextParent( m_ppglbCur ) );
				_pgls->PushConnection( &m_gclObj );
			}
			else
			{
				// Then we are referencing the head:
				(*m_ppglbCur)->m_pgnbNodeParent->PushConnection( &m_gclObj );
				m_ppglbCur = (*m_ppglbCur)->m_pgnbNodeParent->PPGLParentHead();
			}
		}
		else
		{
			assert( 0 );	// This is not supported - my feeling is that this is not necessary and it
										//	certainly isn't well defined - which parent would we go to ?
		}
	}

	void					GoChild( _TyGNIndex _u ) __STLP_NOTHROW
	{
		if ( *m_ppglbCur )
		{
			m_gclObj.remove_link();
			if ( _u )
			{
				_TyIterBase::GoChild( _u );
				t_TyGraphLinkSafe * _pgls = static_cast< t_TyGraphLinkSafe * >( _TyGraphLinkBase::PGLBGetThisFromPPGLBNextChild( m_ppglbCur ) );
				_pgls->PushConnection( &m_gclObj );
			}
			else
			{
				// Then we are referencing the head:
				static_cast< t_TyGraphNodeSafe * >( (*m_ppglbCur)->m_pgnbNodeChild )->PushConnection( &m_gclObj );
				m_ppglbCur = (*m_ppglbCur)->m_pgnbNodeChild->PPGLChildHead();
			}
		}
		else
		{
			assert( 0 );	// This is not supported - my feeling is that this is not necessary and it
										//	certainly isn't well defined - which child would we go to ?
		}
	}
	void					GoRelation( bool _fChild, _TyGNIndex _u ) __STLP_NOTHROW
	{
		if ( _fChild )	
			GoChild( _u );
		else
			GoParent( _u );
	}

	// Link operations:
	// The safe iterator gives enough info to compute the number of parents
	//	before the tail position appropriately:
	_TyGNIndex	UParentsBefore() const __STLP_NOTHROW
	{
		t_TyGraphLinkSafe *	_pgls;
		t_TyGraphNodeSafe *	_pgns;
		t_TyGraphLinkSafe::PGrObjFromPPGLB( _ppglbCur, _pgls, _pgns );
		if ( _pgls )
		{
			return _pgls->UParentsBefore() + 1;
		}
		else
		{
			return 0;
		}
	}
	void				NextParent() __STLP_NOTHROW
	{
		m_gclObj.remove_link();
		_TyIterBase::NextParent();
		// This would have to be a link:
		t_TyGraphLinkSafe * _pgls = static_cast< t_TyGraphLinkSafe * >( 
			_TyGraphLinkBase::PGLBGetThisFromPPGLBNextChild( m_ppglbCur ) );
		_pgls->PushConnection( &m_gclObj );
	}
	void				PrevParent() __STLP_NOTHROW
	{
		m_gclObj.remove_link();
		_TyIterBase::PrevParent();
		// This may be a graph node:
		_PushConnection( m_ppglbCur );
	}

	_TyGNIndex	UChildrenBefore() const __STLP_NOTHROW
	{
		t_TyGraphLinkSafe *	_pgls;
		t_TyGraphNodeSafe *	_pgns;
		t_TyGraphLinkSafe::PGrObjFromPPGLB( _ppglbCur, _pgls, _pgns );
		if ( _pgls )
		{
			return _pgls->UChildrenBefore() + 1;
		}
		else
		{
			return 0;
		}
	}
	void				NextChild() __STLP_NOTHROW
	{
		m_gclObj.remove_link();
		_TyIterBase::NextChild();
		// This would have to be a link:
		t_TyGraphLinkSafe * _pgls = static_cast< t_TyGraphLinkSafe * >( 
			_TyGraphLinkBase::PGLBGetThisFromPPGLBNextChild( m_ppglbCur ) );
		_pgls->PushConnection( &m_gclObj );
	}
	void				PrevChild() __STLP_NOTHROW
	{
		m_gclObj.remove_link();
		_TyIterBase::PrevChild();
		// This may be a graph node:
		_PushConnection( m_ppglbCur );
	}
	void				NextRelation( bool _fChild ) __STLP_NOTHROW
	{
		if ( _fChild )	
			NextChild();
		else						
			NextParent();
	}
	void					PrevRelation( bool _fChild ) __STLP_NOTHROW
	{
		if ( _fChild )	
			PrevChild();
		else						
			PrevParent();
	}

	// Operations between link iterators:
#warning Think this code needs updating.
#ifdef _STLP_MEMBER_TEMPLATES
	template < class t_TyLinkIterator >
	void	ExchangeSiblingParents( t_TyLinkIterator const & _rSiblingParent ) const __STLP_NOTHROW
	{
		assert( PGLBCur() != _rSiblingParent.PGLBCur() );
		assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
		_TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
	}
	template < class t_TyLinkIterator >
	void	ExchangeSiblingChildren( t_TyLinkIterator const & _rSiblingChild ) const __STLP_NOTHROW
	{
		assert( _rSiblingChild.PGLBCur() != PGLBCur() );
		assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
		_TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
	}
#else //_STLP_MEMBER_TEMPLATES
	void	ExchangeSiblingParents( _TyThis const & _rSiblingParent ) const __STLP_NOTHROW
	{
		assert( &_rSiblingParent != this );
		assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
		_TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
	}
	void	ExchangeSiblingChildren( _TyThis const & _rSiblingChild ) const __STLP_NOTHROW
	{
		assert( &_rSiblingChild != this );
		assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
		_TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
	}

	void	ExchangeSiblingParents( _TyLinkIdentIter const & _rlpiSiblingParent ) const __STLP_NOTHROW
	{
		assert( &_rSiblingParent != this );
		assert( PGLBCur()->m_pgnbNodeChild == _rSiblingParent.PGLBCur()->m_pgnbNodeChild );
		_TyGraphLinkBase::ExchangeParents( PGLBCur(), _rSiblingParent.PGLBCur() );
	}
	void	ExchangeSiblingChildren( _TyLinkIdentIter const & _rlpiSiblingChild ) const __STLP_NOTHROW
	{
		assert( &_rSiblingChild != this );
		assert( PGLBCur()->m_pgnbNodeChild == _rSiblingChild.PGLBCur()->m_pgnbNodeChild );
		_TyGraphLinkBase::ExchangeChildren( PGLBCur(), _rSiblingChild.PGLBCur() );
	}
#endif //_STLP_MEMBER_TEMPLATES

#ifdef _STLP_MEMBER_TEMPLATES
	template < class t_TyLinkPosIterator >
	_TyThis & operator = ( t_TyLinkPosIterator const & _r ) __STLP_NOTHROW
	{
		SetPPGLBCur( _r.PPGLBCur() );
		return *this;
	}

	template < class t_TyLinkPosIterator >
	void	SetToParent( t_TyLinkPosIterator const & _r ) __STLP_NOTHROW
	{
		SetPPGLBCur_Populated( _r.PPGLBCur_Parent() );
	}
	template < class t_TyLinkPosIterator >
	void	SetToChild( t_TyLinkPosIterator const & _r ) __STLP_NOTHROW
	{
		SetPPGLBCur_Populated( _r.PPGLBCur_Child() );
	}
#endif //_STLP_MEMBER_TEMPLATES

};

// This iterator maintains a link identity within either the child or the parent list.
// It also has an end() position that is a valid insert before position.
template < class t_TyGraphNodeSafe, class t_TyGraphLinkSafe >
class _graph_link_ident_iterator_base_safe
	: public _graph_link_ident_iterator_base_notsafe< typename t_TyGraphNodeSafe::__TyGraphNodeBase, 
																										typename t_TyGraphLinkSafe::__TyGraphLinkBase >
{
protected:
	// Get the base classes from the safe classes:
	typedef typename t_TyGraphNodeSafe::__TyGraphNodeBase	_TyGraphNodeBase;
	typedef typename t_TyGraphLinkSafe::__TyGraphLinkBase	_TyGraphLinkBase;

private:
	typedef _graph_link_ident_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >		_TyIterBase;
	typedef _graph_link_ident_iterator_base_safe<	_TyGraphNodeBase, _TyGraphLinkBase >			_TyThis;

protected:

	_TyGraphConnectionLink	m_gclLink;	// pointer to link from graph-object to this object.

	void	_init() __STLP_NOTHROW
	{
		m_gclLink.m_ppvConnection = (void*)this;
		m_gclLink.m_egclType = s_egclGraphLinkIdentIterator;
	}

	void	_Reference( _TyGraphLinkBase * _pglbCur ) __STLP_NOTHROW
	{
		assert( !PGLSCur() );
		m_pglbCur = _pglbCur;
		PGLSCur()->PushConnection( &m_gclLink );	// Crash here ? Perhaps you mixed non-safe and safe graphs.
	}

	void	_Dereference() __STLP_NOTHROW
	{
		if ( PGLSCur() )
		{
			m_gclLink.remove_link();
		}
	}

	// The graph link to which this iterator is connected is being deinitialized.
	// We don't need to unlink the connection link.
	void	_link_deinit() __STLP_NOTHROW
	{
		assert( PGLBCur() );
		_TyIterBase::Clear();
	}

public:

	typedef _graph_link_ident_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase >	_TyIdentPosIter;

	explicit _graph_link_ident_iterator_base_safe(	const _TyGraphLinkBase * _pglbCur ) __STLP_NOTHROW
		:	_TyIterBase( 0 )
	{
		_init();
		if ( _pglbCur )
		{
			_Reference( _pglbCur );
		}
	}

// Init with iterator that has an allocator:
#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyLinkIdentIter >
	explicit _graph_link_ident_iterator_base_safe( const _TyLinkIdentIter & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_link_ident_iterator_base_safe( const _TyThis & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		: _TyIterBase( 0 )
	{ 
		_init();
		if ( _r.PGLBCur() )
		{
			_Reference( _r.PGLBCur() );
		}
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyLinkIdentIter >
	explicit _graph_link_ident_iterator_base_safe( const _TyLinkIdentIter & _r, std::false_type ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_link_ident_iterator_base_safe( const _TyThis & _r, std::false_type ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		: _TyIterBase( 0 )
	{ 
		_init();
		__semantic_error_object	error;	// when and why is this used.
	}

// Init with iterator and allocator:
#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyLinkIdentIter >
	explicit _graph_link_ident_iterator_base_safe( const _TyLinkIdentIter & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_link_ident_iterator_base_safe( const _TyThis & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		: _TyIterBase( 0 )
	{ 
		_init();
		if ( _r.PGLBCur() )
		{
			_Reference( _r.PGLBCur() );
		}
	}

	void	Clear() __STLP_NOTHROW
	{
		_Dereference();
		_TyIterBase::Clear();
	}

	t_TyGraphLinkSafe *	PGLSCur() const __STLP_NOTHROW
	{
		return static_cast< t_TyGraphLinkSafe * >( m_pglbCur );
	}

	void		SetPGLBCur( _TyGraphLinkBase * _pglbCur ) __STLP_NOTHROW
	{
		if ( PGLSCur() )
		{ 
			m_gclLink.remove_link();
			if ( m_pglbCur = _pglbCur )
			{
				PGLSCur()->PushConnection( &m_gclLink );
			}
		}
		else
		if ( _pglbCur )
		{
			_Reference( _pglbCur );
		}
	}

	// Set a non-NULL <_pglbCur> into a populated iterator:
	void		SetPGLBCur_Populated( _TyGraphLinkBase * _pglbCur ) __STLP_NOTHROW
	{	
		assert( PGLSCur() );
		assert( _pglbCur );
		m_gclLink.remove_link();
		m_pglbCur = _pglbCur;
		PGLSCur()->PushConnection( &m_gclLink );
	}

	// Node operations - these are the same as the node iterator:
	void					GoParent( _TyGNIndex _u ) __STLP_NOTHROW
	{
		m_gclLink.remove_link();
		_TyIterBase::GoParent( _u );
		PGLSCur()->PushConnection( &m_gclLink );
	}
	void					GoChild( _TyGNIndex _u ) __STLP_NOTHROW
	{
		m_gclLink.remove_link();
		_TyIterBase::GoChild( _u );
		PGLSCur()->PushConnection( &m_gclLink );
	}
	void					GoRelation( bool _fChild, _TyGNIndex _u ) __STLP_NOTHROW
	{
		if ( _fChild )	
			GoChild( _u );
		else
			GoParent( _u );
	}

	// Link operations:
	void				NextParent() __STLP_NOTHROW
	{
		m_gclLink.remove_link();
		_TyIterBase::NextParent( );
		PGLSCur()->PushConnection( &m_gclLink );
	}
	void				PrevParent() __STLP_NOTHROW
	{
		m_gclLink.remove_link();
		_TyIterBase::PrevParent( );
		PGLSCur()->PushConnection( &m_gclLink );
	}

	void				NextChild() __STLP_NOTHROW
	{
		m_gclLink.remove_link();
		_TyIterBase::NextChild( );
		PGLSCur()->PushConnection( &m_gclLink );
	}
	void				PrevChild() __STLP_NOTHROW
	{
		m_gclLink.remove_link();
		_TyIterBase::PrevChild( );
		PGLSCur()->PushConnection( &m_gclLink );
	}
	void					NextRelation( bool _fChild ) __STLP_NOTHROW
	{
		if ( _fChild )	
			NextChild();
		else						
			NextParent();
	}
	void					PrevRelation( bool _fChild ) __STLP_NOTHROW
	{
		if ( _fChild )	
			PrevChild();
		else						
			PrevParent();
	}

	template < class t_TyLinkIter >
	bool operator == ( t_TyLinkIter const & _r ) const __STLP_NOTHROW
	{
		return PGLBCur() == _r.PGLBCur();
	}

	template < class t_TyLinkIterator >
	_TyThis & operator = ( t_TyLinkIterator const & _r )
	{
		SetPGLBCur( _r.PGLBCur() );
		return *this;
	}

};

// Path iterator - maintains path.
template <	class t_TyGraphNodeSafe, class t_TyGraphLinkSafe,
						class t_TyPathNodeSafe,	class t_TyPathNodeSafeAllocator >
class _graph_path_iterator_base_safe
	: public _alloc_base< t_TyPathNodeSafe, t_TyPathNodeSafeAllocator >,	// This allocates safe path nodes for us.

// NOTE: there is a significant design decision here - we could base on the safe types - but that would cause
//	multiple implementations of the same code - one for non-safe and one for safe. However, this way, we have
//	multiple allocators - the base classes allocator will not be used in this class. When instanced allocators
//	are used we will have an extra member in the class that will not be used - this is not horrible but perhaps
//	it can be avoided somehow in the future.
		public _graph_path_iterator_base_notsafe< 
										typename t_TyGraphNodeSafe::__TyGraphNodeBase,
										typename t_TyGraphLinkSafe::__TyGraphLinkBase,
										typename t_TyPathNodeSafe::__TyPathNodeBase,
										typename _Alloc_traits< 
											typename t_TyPathNodeSafe::__TyPathNodeBase, t_TyPathNodeSafeAllocator >::allocator_type >
{
protected:

	// Get the base classes from the safe classes:
	typedef typename t_TyGraphNodeSafe::__TyGraphNodeBase	_TyGraphNodeBase;
	typedef typename t_TyGraphLinkSafe::__TyGraphLinkBase	_TyGraphLinkBase;
	typedef typename t_TyPathNodeSafe::__TyPathNodeBase		_TyPathNodeBase;

	// Get the allocator for the base from that of the safe:
	typedef typename _Alloc_traits< _TyPathNodeBase, t_TyPathNodeSafeAllocator >::allocator_type	_TyPathNodeBaseAllocator;

private:

	typedef _alloc_base< t_TyGraphNodeSafe, t_TyPathNodeSafeAllocator >		_TyBaseAllocPathNode;
	typedef _graph_path_iterator_base_safe<	t_TyGraphNodeSafe, t_TyGraphLinkSafe,	
																					t_TyPathNodeSafe,	t_TyPathNodeSafeAllocator > _TyThis;
	typedef _graph_path_iterator_base_notsafe< _TyGraphNodeBase, _TyGraphLinkBase, 
																						_TyPathNodeBase, _TyPathNodeBaseAllocator > _TyBase;

	// Get the actual allocators from the base:
	typedef typename _TyBaseAllocPathNode::_TyAllocatorType		_TyPathNodeSafeAllocator;

protected:

	void	_InitConnLink( t_TyPathNodeSafe * _ppns ) __STLP_NOTHROW
	{
		_ppns->m_gclNode.m_pvConnection = (void*)_ppns;
		_ppns->m_gclNode.m_pvConnectionContainer = (void*)this;
		_ppns->m_gclLink.m_pvConnection = (void*)_ppns;
		_ppns->m_gclLink.m_pvConnectionContainer = (void*)this;
	}

	void	_UpdateConnLink( t_TyPathNodeSafe & _rpns ) __STLP_NOTHROW
	{
		_rpns.m_gclLink.remove_link();
		_rpns.m_gclNode.remove_link();
		_rpns.m_pgnbNode->PushConnection( &_rpns.m_gclNode );
		_rpns.m_pgnbLink->PushConnection( &_rpns.m_gclLink );
	}

	// Note: This method does nothing with the count - that must be kept up-to-date
	//	by the caller.
	void	_AppendPath( const _TyPathNodeBase * _ppnbAppend )
	{
		if ( _ppnbAppend )
		{
			// Save the identity of the old tail - we will remove after successful appendage.
			_TyPathNodeBase *	_ppnbTailOld = *m_pppnbTail;

			// Save the position of the old tail - we will revert if we throw:
			_TyPathNodeBase **	_pppnbTailOld = m_pppnbTail;
			_BIEN_TRY
			{
				// Special case the last element - we won't be allocating a link.
				__SDP( t_TyPathNodeSafe, _TyPathNodeSafeAllocator, GetPNSAllocator(), _ppnsNew );
				for (	;
							_ppnbAppend->m_ppnbNext;
							_ppnbAppend = _ppnbAppend->m_ppnbNext )
				{
					_TyBaseAllocPathNode::allocate_type( _ppnsNew );
#ifndef _BIEN_USE_EXCEPTIONS
					if ( !_ppnsNew )
					{
						_DestroyPath( _pppnbTailOld );
						return;
					}
#endif //!_BIEN_USE_EXCEPTIONS
					new ( _ppnsNew ) t_TyPathNodeSafe( *_ppnbAppend );
					_InitConnLink( _ppnsNew );

					_ppnsNew->push_back_node( m_pppnbTail );

					_ppnsNew->m_pgnbNode->PushConnection( &( _ppnsNew->m_gclNode ) );
					__SDP_TRANSFER(_ppnsNew)->m_pglbLink->PushConnection( &( _ppnsNew->m_gclLink ) );
				}
				_TyBaseAllocPathNode::allocate_type( _ppnsNew );
#ifndef _BIEN_USE_EXCEPTIONS
				if ( !_ppnsNew )
				{
					_DestroyPath( _pppnbTailOld );
					return;
				}
#endif //!_BIEN_USE_EXCEPTIONS
				new ( _ppnsNew ) t_TyPathNodeSafe( *_ppnbAppend );
				_InitConnLink( _ppnsNew );
				assert( !_ppnsNew->m_pglbLink );

				_ppnsNew->push_back_node( m_pppnbTail );
				__SDP_TRANSFER(_ppnsNew)->m_pgnbNode->PushConnection( &( _ppnsNew->m_gclNode ) );
			}
			_BIEN_UNWIND( _DestroyPath( _pppnbTailOld ) )

			// If we had an old tail then update by removing now - for throw-safety:
			if ( _ppnbTailOld )
			{
				assert( !_ppnbTailOld->m_pglbLink );
				assert( _ppnbTailOld->m_pgnbNode == _ppnbTailOld->m_ppnbNext->m_pgnbNode );	// Paths must connect.
				_ppnbTailOld->remove_node_in_middle();
				_ppnbTailOld->m_gclNode.remove_link();
				_TyBaseAllocPathNode::deallocate_type( _ppnbTailOld );
			}
		}
	}

	void	_AppendReversePath( const _TyPathNodeBase * _ppnbAppend )
	{
		if ( _ppnbAppend )
		{
			// Save the identity of the old tail - we will remove after successful appendage.
			_TyPathNodeBase *	_ppnbTailOld = *m_pppnbTail;

			// Save the old tail - we will revert if we throw:
			_TyPathNodeBase **	_pppnbTailOld = m_pppnbTail;
			_BIEN_TRY
			{
				__SDP( t_TyPathNodeSafe, _TyPathNodeSafeAllocator, GetPNSAllocator(), _ppnsNew );
				_TyBaseAllocPathNode::allocate_type( _ppnsNew );
				__SDP_CHECK_VOID( _ppnsNew );
				new ( _ppnsNew ) t_TyPathNodeSafe( _ppnbAppend->m_pgnbNode, 0 ); // This is the new tail.
				_InitConnLink( _ppnsNew );

				_ppnsNew->m_pgnbNode->PushConnection( &( _ppnsNew->m_gclNode ) );
				__SDP_TRANSFER(_ppnsNew)->push_back_node( m_pppnbTail );

				// We will pass the graph link pointer as we copy:
				_TyGraphLinkBase *	_pglbNext = _ppnbAppend->m_pglbLink;

				for (	;
							_ppnbAppend = _ppnbAppend->m_ppnbNext;
						)
				{
					_TyBaseAllocPathNode::allocate_type( _ppnsNew );
#ifndef _BIEN_USE_EXCEPTIONS
					if ( !_ppnsNew )
					{
						_DestroyPath( _pppnbTailOld );
						return;
					}
#endif //!_BIEN_USE_EXCEPTIONS
					new ( _ppnsNew ) t_TyPathNodeSafe( _ppnbAppend->m_pgnbNode, _pglbNext );
					_InitConnLink( _ppnsNew );

					_ppnsNew->m_pgnbNode->PushConnection( &( _ppnsNew->m_gclNode ) );
					_ppnsNew->m_pglbLink->PushConnection( &( _ppnsNew->m_gclLink ) );

					_pglbNext = _ppnbAppend->m_pglbLink;
					__SDP_TRANSFER(_ppnsNew)->insert_node_in_middle( *_pppnbTailOld );
				}
				assert( !_pglbNext );
			}
			_BIEN_UNWIND( _DestroyPath( _pppnbTailOld ) )

			// We succeeded - if we had an old tail then remove it now:
			if ( _ppnbTailOld )
			{
				assert( !_ppnbTailOld->m_pglbLink );
				assert( _ppnbTailOld->m_pgnbNode == _ppnbTailOld->m_ppnbNext->m_pgnbNode );	// Paths must connect.
				_ppnbTailOld->remove_node_in_middle();
				_ppnbTailOld->m_gclNode.remove_link();
				_TyBaseAllocPathNode::deallocate_type( _ppnbTailOld );
			}
		}
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyPathIter >
	void	_OverwritePath( _TyPathIter const & _r )
#else //_STLP_MEMBER_TEMPLATES
	void	_OverwritePath( _TyThis const & _r )
#endif //_STLP_MEMBER_TEMPLATES
	{	
		_OverwritePath( _r.m_ppnbHead, _r.m_pppnbTail, _r.m_uCount );
	}

	void	_OverwritePath(	const _TyPathNodeBase * const & _ppnbHead, 
												const _TyPathNodeBase ** _pppnbTail, 
												_TyGNIndex _uCount )
	{	
		const _TyPathNodeBase ** _pppnbCopyUntil = _pppnbTail;

		// We will overwrite the current path with the one passed - 
		//	first make the list sizes the same:
		if ( m_uCount > _uCount )
		{
			// Then we need to destroy some nodes:
			const _TyPathNodeBase ** _pppnb = 
				_TyPathNodeBase::PPGPNBGetNth( m_ppnbHead, m_pppnbTail, m_uCount, _uCount );
			_DestroyPath( _pppnb );
		}
		else
		if ( m_uCount < _uCount )
		{
			// Then we need to create some nodes - copy as we allocate:
			const _TyPathNodeBase ** _pppnbTailOld = m_pppnbTail; // Save the old tail - in case we throw:
			_BIEN_TRY
			{
				// Insert the first one normally - this one updates the tail - the rest we know
				//	won't touch the tail so we can use the faster method:
				_pppnbCopyUntil = _TyPathNodeBase::PPGNBGetPrevPrevNext( _pppnbCopyUntil );
				__SDP( t_TyPathNodeSafe, _TyPathNodeSafeAllocator, GetPNSAllocator(), _ppnsNew );
				_TyBaseAllocPathNode::allocate_type( _ppnsNew );
				__SDP_CHECK_VOID( _ppnsNew );
				new ( _ppnsNew ) t_TyPathNodeSafe( **_pppnbCopyUntil );
				_InitConnLink( _ppnsNew );

				if ( _ppnsNew->m_pglbLink )
				{
					_ppnsNew->m_pglbLink->PushConnection( &( _ppnsNew->m_gclLink ) );
				}
				_ppnsNew->m_pgnbNode->PushConnection( &( _ppnsNew->m_gclNode ) );
				
				__SDP_TRANSFER(_ppnsNew)->push_back_node( m_pppnbTail );

				// Now insert the next nodes into the list before the position of the old tail:
				for ( ++m_uCount; m_uCount++ < _uCount; )
				{
					_pppnbCopyUntil = _TyPathNodeBase::PPGNBGetPrevPrevNext( _pppnbCopyUntil );
					_TyBaseAllocPathNode::allocate_type( _ppnsNew );
#ifndef _BIEN_USE_EXCEPTIONS
					if ( !_ppnsNew )
					{
						_DestroyPath( _pppnbTailOld );
						return;
					}
#endif //!_BIEN_USE_EXCEPTIONS
					new ( _ppnsNew ) t_TyPathNodeSafe( **_pppnbCopyUntil );
					_InitConnLink( _ppnsNew );

					if ( _ppnsNew->m_pglbLink )
					{
						_ppnsNew->m_pglbLink->PushConnection( &( _ppnsNew->m_gclLink ) );
					}
					_ppnsNew->m_pgnbNode->PushConnection( &( _ppnsNew->m_gclNode ) );

					__SDP_TRANSFER(_ppnsNew)->insert_node_in_middle( *_pppnbTailOld );
				}
			}
			_BIEN_UNWIND( _DestroyPath( _pppnbTailOld ) )	// revert and rethrow.
		}

		// Now overwrite the already existing nodes in the path - as no allocation occurs we need not try:
		const _TyPathNodeBase * _ppnbCopyTo = m_ppnbHead;
		for ( const _TyPathNodeBase * const * _pppnbCopyFrom = &( _ppnbHead );
					_pppnbCopyFrom != _pppnbCopyUntil;
					_pppnbCopyFrom = &( (*_pppnbCopyFrom)->m_ppnbNext ),
					_ppnbCopyTo = _ppnbCopyTo->m_ppnbNext )
		{
			*_ppnbCopyTo = **_pppnbCopyFrom;

			// We will already have the exact same number of connection links as those
			//	from which we are copying - just re-position them:
			t_TyPathNodeSafe &	_rpnsCopyTo = static_cast< t_TyPathNodeSafe & >( *_ppnbCopyTo );
			_rpnsCopyTo.m_gclNode.remove_link();
			_rpnsCopyTo.m_pgnbNode->PushConnection( &( _rpnsCopyTo.m_gclNode ) );
			if ( _rpnsCopyTo.m_pglbLink )
			{
				_rpnsCopyTo.m_gclLink.remove_link();
				_rpnsCopyTo.m_pglbLink->PushConnection( &( _rpnsCopyTo.m_gclLink ) );
			}
			else
			{
				assert( !static_cast< t_TyPathNodeSafe & >( **_pppnbCopyFrom ).m_pglbLink );
				assert( !_rpnsCopyTo.m_pglbLink );
			}
		}

		m_uCount = _uCount;
	}

#ifdef _STLP_MEMBER_TEMPLATES
	// Specialize the case for overwriting the current path with a node iterator.
	template < >
	void	_OverwritePath( _TyGraphNodeIter const & _r )
	{
		Clear();
		if ( PGNBCur() )
		{
			_AppendNode( _r.PGNBCur(), 0 );
		}
	}

	// Specialize the case for overwriting the current path with a position iterator.
	template < >
	void	_OverwritePath( _TyGraphLinkPosIter const & _r )
	{
		Clear();
		_AppendLinkPosIter( _r );
	}

#endif //_STLP_MEMBER_TEMPLATES

	void	_PushNode( _TyGraphNodeBase * _pgnb, _TyGraphLinkBase * _pglb )
	{
		__SDP( t_TyPathNodeSafe, _TyPathNodeSafeAllocator, GetPNSAllocator(), _ppnsAlloc );
		_TyBaseAllocPathNode::allocate_type( _ppnsAlloc );
		__SDP_CHECK_VOID( _ppnsAlloc );
		new ( _ppnsAlloc ) t_TyPathNodeSafe( _pgnb, _pglb );
		_InitConnLink( _ppnsAlloc );

		if ( _ppnsAlloc->m_pglbLink )
		{
			_ppnsAlloc->m_pglbLink->PushConnection( &( _ppnsAlloc->m_gclLink ) );
		}
		_ppnsAlloc->m_pgnbNode->PushConnection( &( _ppnsAlloc->m_gclNode ) );
				
		__SDP_TRANSFER(_ppnsAlloc)->insert_node( m_ppnbHead, m_pppnbTail );
		m_uCount++;
	}

	void	_AppendNode( _TyGraphNodeBase * _pgnb, _TyGraphLinkBase * _pglb )
	{
		assert( !m_uCount );
		__SDP( t_TyPathNodeSafe, _TyPathNodeSafeAllocator, GetPNSAllocator(), _ppnsAlloc );
		_TyBaseAllocPathNode::allocate_type( _ppnsAlloc );
		__SDP_CHECK_VOID( _ppnsAlloc );
		new ( _ppnsAlloc ) t_TyPathNodeSafe( _pgnb, _pglb );
		_InitConnLink( _ppnsAlloc );

		if ( _ppnsAlloc->m_pglbLink )
		{
			_ppnsAlloc->m_pglbLink->PushConnection( &( _ppnsAlloc->m_gclLink ) );
		}
		_ppnsAlloc->m_pgnbNode->PushConnection( &( _ppnsAlloc->m_gclNode ) );

		__SDP_TRANSFER(_ppnsAlloc)->push_back_node( m_pppnbTail );
		m_uCount++;
	}

	void	_AppendLinkPosIter( _TyGraphLinkPosIter const & _r )
	{
		assert( !m_uCount );
		_TyPathNodeBase ** _pppnbOldTail = m_pppnbTail;
		_TyGNIndex _uOldCount;
		_BIEN_TRY
		{
			if ( _r.PGLBCur() )
			{
				if ( _r.PPGLBCur() == _r.PPGLBCur_Parent() )
				{
					// Create a path from the child to the parent:
					_AppendNode( _r.PGLBCur()->m_pgnbNodeParent, _r.PGLBCur() );
					_AppendNode( _r.PGLBCur()->m_pgnbNodeChild, 0 );
				}
				else
				{
					// Create a path from the parent to the child:
					_AppendNode( _r.PGLBCur()->m_pgnbNodeChild, _r.PGLBCur() );
					_AppendNode( _r.PGLBCur()->m_pgnbNodeParent, 0 );
				}
			}
		}
		_BIEN_UNWIND( _DestroyPath( _pppnbOldTail ), m_uCount = _uOldCount )
	}

	// Re-reference each path node to be connected to <_pvReference>:
	void	_ReReferencePath( void * _pvReference ) __STLP_NOTHROW
	{
		for (	_TyPathNodeBase ** _pppnbStart = &m_ppnbHead;
					_pppnbStart != m_pppnbTail;
					_ppnbStart = _ppnbStart->m_ppnbNext )
		{
			static_cast< t_TyPathNodeSafe * >( _ppnbStart )->m_gclNode.m_pvConnectionContainer = _pvReference;
			static_cast< t_TyPathNodeSafe * >( _ppnbStart )->m_gclLink.m_pvConnectionContainer = _pvReference;
		}
	}

	void	_DestroyPath( _TyPathNodeBase ** _pppnbStart ) __STLP_NOTHROW
	{
		_TyPathNodeBase ** _pppnbOldTail = m_pppnbTail;
		m_pppnbTail = _pppnbStart;
		for ( ;
					_ppnbStart != _pppnbOldTail;
				)
		{
			t_TyPathNodeSafe *	_ppnsDestroy = static_cast< t_TyPathNodeSafe * >( _ppnbStart );
			_ppnbStart = _ppnbStart->m_ppnbNext;

			_ppnsDestroy->m_gclNode.remove_link();
			if ( _ppnsDestroy->m_pglbLink )
			{
				_ppnsDestroy->m_gclLink.remove_link();
			}

			_ppnsDestroy->~t_TyPathNodeSafe();
			_TyBaseAllocPathNode::deallocate_type( _ppnsDestroy );
		}
		// If the new tail has a pointer to a link then rid it:
		if ( *m_pppnbTail && (*m_pppnbTail)->m_pglbLink )
		{
			(*m_pppnbTail)->m_gclLink.remove_link();
			(*m_pppnbTail)->m_pglbLink = 0;	
		}
	}

	// The graph node to which this iterator is connected is being deinitialized.
	// We don't need to unlink the connection link.
	void	_node_deinit( _TyGraphConnectionLink * _pgclNode ) __STLP_NOTHROW
	{
		t_TyPathNodeSafe *	_ppnsDeinit = static_cast< t_TyPathNodeSafe * >( _pgclNode->m_pvConnection );
		assert( &( _ppnsDeinit->m_gclNode ) == _pgclNode );
		_ppnsDeinit->m_pgnbNode = 0;
	}

	// The graph link to which this iterator is connected is being deinitialized.
	// We don't need to unlink the connection link.
	void	_link_deinit( _TyGraphConnectionLink * _pgclLink ) __STLP_NOTHROW
	{
		t_TyPathNodeSafe *	_ppnsDeinit = static_cast< t_TyPathNodeSafe * >( _pgclLink->m_pvConnection );
		assert( &( _ppnsDeinit->m_gclLink ) == _pgclLink );
		_ppnsDeinit->m_pglbLink = 0;
	}

public:

	explicit _graph_path_iterator_base_safe( t_TyPathNodeSafeAllocator const & _allocPathNode ) __STLP_NOTHROW
		:	_TyBaseAllocPathNode( _allocPathNode ),
			_TyBase( _allocPathNode )
	{
	}

// Construction with a path and a different allocator:
#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyPathIter >
	explicit _graph_path_iterator_base_safe(	_TyPathIter const & _r,
																						t_TyPathNodeSafeAllocator const & _allocPathNode )
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_path_iterator_base_safe(	_TyThis const & _r,
																						t_TyPathNodeSafeAllocator const & _allocPathNode )
#endif //_STLP_MEMBER_TEMPLATES
		: _TyBaseAllocPathNode( _allocPathNode ),
			_TyBase( _allocPathNode )
	{
		_AppendPath( _r.m_ppnbHead );
		m_uCount = _r.m_uCount;	// throwsafe
	}
	
	
	// Specialize construction with a graph node iterator:
#ifdef _STLP_MEMBER_TEMPLATES
	template < >
#endif //_STLP_MEMBER_TEMPLATES
	explicit _graph_path_iterator_base_safe(	_TyGraphNodeIter const & _r,
																						t_TyPathNodeSafeAllocator const & _allocPathNode )
		: _TyBaseAllocPathNode( _allocPathNode ),
			_TyBase( _allocPathNode )
	{
		if ( _r.PGNBCur() )
		{
			_AppendNode( _r.PGNBCur(), 0 );
		}
	}

	// Also specialize construction with a link position iterator - this produces a path
	//	that ends at the node located at current position of that iterator - i.e. if the
	//	link iterator is in the middle of the child list then the path iterator will start at 
	//	parent above that child and end at that child. Likewise for a parent list position.
#ifdef _STLP_MEMBER_TEMPLATES
	template < >
#endif //_STLP_MEMBER_TEMPLATES
	explicit _graph_path_iterator_base_safe(	_TyGraphLinkPosIter const & _r,
																						t_TyPathNodeSafeAllocator const & _allocPathNode )
		: _TyBaseAllocPathNode( _allocPathNode ),
			_TyBase( _allocPathNode )
	{
		_AppendLinkPosIter( _r );
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyPathIter >
	explicit _graph_path_iterator_base_safe( _TyPathIter const & _r )
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_path_iterator_base_safe( _TyThis const & _r )
#endif //_STLP_MEMBER_TEMPLATES
		: _TyBaseAllocPathNode( _r._GetPNAllocatorAsPassed() ),
			_TyBase( _r._GetPNAllocatorAsPassed() )	// Don't call copy constructor on base - it copies non-safe nodes.
	{
		// This copies the path:
		_AppendPath( _r.m_ppnbHead );
		m_uCount = _r.m_uCount;	// throwsafe
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyPathIter >
	explicit _graph_path_iterator_base_safe( _TyPathIter const & _r, std::false_type ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	explicit _graph_path_iterator_base_safe( _TyThis const & _r, std::false_type ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
		: _TyBaseAllocPathNode( _r._GetPNAllocatorAsPassed() ),
			_TyBase( _r._GetPNAllocatorAsPassed() )
	{
		// This does not copy the path - just gets the allocators.
	}

	~_graph_path_iterator_base_notsafe() __STLP_NOTHROW
	{
		_DestroyPath( &m_ppnbHead );
	}

// Allocator access:
	_TyPathNodeSafeAllocator const &	_GetPNSAllocator() const __STLP_NOTHROW
	{
		return _TyBaseAllocPathNode::get_allocator();
	}
	t_TyPathNodeSafeAllocator					_GetPNAllocatorAsPassed() const __STLP_NOTHROW
	{
		return _TyBaseAllocPathNode::get_allocator_as_passed();
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyPathIter >
	_TyThis &	operator = ( _TyPathIter const & _r )
#ifdef _STLP_MEMBER_TEMPLATES
	_TyThis &	operator = ( _TyThis const & _r )
#endif //_STLP_MEMBER_TEMPLATES
	{
		_OverwritePath( _r );
	}

	const _TyGraphNodeBase *			PGNBCur() const __STLP_NOTHROW
	{
		return m_ppnbHead->m_pgnbNode;
	}

	// This method let's us transfer our state to a link identity iterator:
	const _TyGraphLinkBase *			PGLBCur() const __STLP_NOTHROW
	{
		return m_ppnbHead->m_pglbLink;
	}

	// This method let's us transfer our state to a link position iterator:
	const _TyGraphLinkBase **			PPGLBCur_Parent() const __STLP_NOTHROW
	{
		return m_ppnbHead->m_pglbLink->PPGLBGetThisParent();
	}
	const _TyGraphLinkBase **			PPGLBCur_Child() const __STLP_NOTHROW
	{
		return m_ppnbHead->m_pglbLink->PPGLBGetThisChild();
	}

	// Return according to direction of tip of iterator:
	const _TyGraphLinkBase **			PPGLBCur() const __STLP_NOTHROW
	{
		return m_ppnbHead->m_pglbLink->m_pgnbNodeChild == m_ppnbHead->m_pgnbNode ? 
			PPGLBCur_Child() : PPGLBCur_Parent();
	}

	// Test if empty:
	bool operator ! () const __STLP_NOTHROW
	{
		return !m_ppnbHead;
	}

	void	Clear() __STLP_NOTHROW
	{
		_DestroyPath( &m_ppnbHead );
	}

	void					GoParent( _TyGNIndex _u )
	{
		// Add the <_u>th parent to the front of the path:
		_TyGraphLinkBase * _pglbParent = _TyGraphLinkBase::PGLBGetNthParent( m_ppnbHead->m_pgnbNode->PPGLParentHead(), _u );
		_PushNode( _pglbParent->m_pgnbNodeParent, _pglbParent );
	}

	void					GoChild( _TyGNIndex _u )
	{
		// Add the <_u>th child to the front of the path:
		_TyGraphLinkBase * _pglbChild = _TyGraphLinkBase::PGLBGetNthChild( m_ppnbHead->m_pgnbNode->PPGLChildHead(), _u );
		_PushNode( _pglbChild->m_pgnbNodeChild, _pglbChild );
	}
	void					GoRelation( bool _fChild, _TyGNIndex _u )
	{
		if ( _fChild )	
			GoChild( _u );
		else
			GoParent( _u );
	}

	// Link operations:
	void				NextParent() __STLP_NOTHROW
	{
		// Advance the iterator to the next parent:
		m_ppnbHead->m_pglbLink = m_ppnbHead->m_pglbLink->PGLBGetNextParent();
		m_ppnbHead->m_pgnbNode = m_ppnbHead->m_pglbLink->m_pgnbNodeParent;
		_UpdateConnLink( static_cast< t_TyPathNodeSafe * >( m_ppnbHead ) );
	}
	void				PrevParent() __STLP_NOTHROW
	{
		// Advance the iterator to the previous parent:
		m_ppnbHead->m_pglbLink = m_ppnbHead->m_pglbLink->PGLBGetPrevParent();
		m_ppnbHead->m_pgnbNode = m_ppnbHead->m_pglbLink->m_pgnbNodeParent;
		_UpdateConnLink( static_cast< t_TyPathNodeSafe * >( m_ppnbHead ) );
	}

	void				NextChild() __STLP_NOTHROW
	{
		// Advance the iterator to the next child:
		m_ppnbHead->m_pglbLink = m_ppnbHead->m_pglbLink->PGLBGetNextChild();
		m_ppnbHead->m_pgnbNode = m_ppnbHead->m_pglbLink->m_pgnbNodeChild;
		_UpdateConnLink( static_cast< t_TyPathNodeSafe * >( m_ppnbHead ) );
	}
	void				PrevChild() __STLP_NOTHROW
	{
		// Advance the iterator to the previous child:
		m_ppnbHead->m_pglbLink = m_ppnbHead->m_pglbLink->PGLBGetPrevChild();
		m_ppnbHead->m_pgnbNode = m_ppnbHead->m_pglbLink->m_pgnbNodeChild;
		_UpdateConnLink( static_cast< t_TyPathNodeSafe * >( m_ppnbHead ) );
	}
	void					NextRelation( bool _fChild ) __STLP_NOTHROW
	{
		if ( _fChild )	
			NextChild();
		else						
			NextParent();
	}
	void					PrevRelation( bool _fChild ) __STLP_NOTHROW
	{
		if ( _fChild )	
			PrevChild();
		else						
			PrevParent();
	}

	// Operations between link iterators:

#ifdef _STLP_MEMBER_TEMPLATES
	template < class t_TyLinkIter >
	bool operator == ( t_TyLinkIter const & _r ) const __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	bool operator == ( _TyThis const & _r ) const __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
	{
		return PGLBCur() == _r.PGLBCur();
	}

#ifdef _STLP_MEMBER_TEMPLATES
	template < class _TyIter >
	_TyThis & operator = ( _TyIter const & _r ) __STLP_NOTHROW
#else //_STLP_MEMBER_TEMPLATES
	_TyThis & operator = ( _TyThis const & _r ) __STLP_NOTHROW
#endif //_STLP_MEMBER_TEMPLATES
	{
		// Overwrite the current path with that passed:
		_OverwritePath( _r );
		return *this;
	}

	// Swap existences - need member templates for type-safety:
#ifdef _STLP_MEMBER_TEMPLATES

	// Swapping with anything other than this should not compile:
	template < class _TyIter >
	void	swap( _TyIter & _r ) __STLP_NOTHROW
	{
		___semantic_error_object	error;
	}

	// Specialize attempt to swap with this - we want this to succeed:
	template < >
	void	swap( _TyThis & _r ) __STLP_NOTHROW
	{
		// Need to move through each path and exchange references inside the connection
		//	link - since link maintains a reference to <this> and <_r>:
		_ReReferencePath( reinterpret_cast< void *> ( &_r ) );
		_r._ReReferencePath( reinterpret_cast< void *> ( this ) );
		_TyBase::swap( static_cast< _TyBase & >(  _r ) );
	}

#else //_STLP_MEMBER_TEMPLATES

	// Non-safe version - programmer can screw up - but it should crash :-)
	void	swap( _TyThis & _r ) __STLP_NOTHROW
	{
		_ReReferencePath( reinterpret_cast< void *> ( &_r ) );
		_r._ReReferencePath( reinterpret_cast< void *> ( this ) );
		_TyBase::swap( _r );
	}

#endif //_STLP_MEMBER_TEMPLATES
};



#endif //__GR_SITR
