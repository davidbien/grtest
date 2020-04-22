#ifndef __GR_SRIL
#define __GR_SRIL

// _gr_sril.h

// This module declares objects used during output serialization of the graph.

__DGRAPH_BEGIN_NAMESPACE

// When !t_fUseSeek we don't attempt to tell/seek the stream - this is not throw-safe -
//  i.e. state may not be restored appropriately.

template <  class t_TyOutputStreamBase,
            class t_TyAllocator,
            bool t_fUseSeek = true >  // By default we use the seek.
struct _graph_output_iter_base
  : public _graph_fwd_iter_base<  typename t_TyOutputStreamBase::_TyGraphNodeBase, 
                                  typename t_TyOutputStreamBase::_TyGraphLinkBase, 
                                  t_TyAllocator, false >
{
private:
  typedef _graph_output_iter_base<  t_TyOutputStreamBase, 
                                    t_TyAllocator, t_fUseSeek >               _TyThis;
  typedef _graph_fwd_iter_base< typename t_TyOutputStreamBase::_TyGraphNodeBase, 
                                typename t_TyOutputStreamBase::_TyGraphLinkBase, 
                                t_TyAllocator, false >                        _TyBase;
public:

  typedef typename t_TyOutputStreamBase::_TyGraphLinkBase _TyGraphLinkBase;
  typedef typename t_TyOutputStreamBase::_TyGraphNodeBase _TyGraphNodeBase;
  typedef typename _TyBase::_TyPMFnNotifyUnfinished _TyPMFnNotifyUnfinished;

  // Output stream object - supports some types and functionality - allows dumping
  //  ( for instance ) in both human readable as well as binary forms. Also allows
  //  output ( for instance ) to a FILE*, an istream, or perhaps OLE structured storage.
  // The output stream supports:
  // _TyStreamPos - type for position within the stream ( allows reading/setting ).
  // An set of methods that permit:
  // _TyStreamPos _Tell(void).
  // void _Seek( _TyStreamPos ).
  t_TyOutputStreamBase & m_ros;

  typedef typename t_TyOutputStreamBase::_TyStreamPos _TyStreamPos;

  bool                m_fNewUnfinished;
  _TyGraphLinkBase *  m_pglbUnfinished; // The link the entered into the last unfinished node.

  int                 m_iContextsOutput;      // Our idea of the current number of contexts.
  bool                m_fDirectionDownOutput; // Our idea of the current direction of iteration.

  bool                m_fWroteLinkFooter; // This used when we are not seeking.
  _TyStreamPos        m_spBeforeLinkFooter;

  bool m_fWroteGraphFooter;


  // Derived settable callbacks - this allows the caller to know the types of links and nodes
  // ( keeps code size smaller in the presence of multiply typed graphs ).
  typedef void ( _TyThis :: * _TyPMFnWriteNode )();
  _TyPMFnWriteNode    m_pmfnWriteNode;

  typedef void ( _TyThis :: * _TyPMFnWriteLink )();
  _TyPMFnWriteLink    m_pmfnWriteLink;

protected:
  void _Init()
  {
    // Set up callback:
    _TyBase::m_pmfnNotifyUnfinished = static_cast< _TyPMFnNotifyUnfinished >( &_TyThis::_NotifyUnfinished );
    m_fNewUnfinished = false;

    if ( !_TyBase::m_fInitialized )
    {
      if ( _TyBase::m_pgnbCur || _TyBase::m_pglbCur )
      {
        _TyBase::_Init();
      }
      else
      {
        _TyBase::_ClearState();
      }
    }
  }

  void  _Seek( _TyStreamPos _sp )
  {
    if ( t_fUseSeek )
    {
      m_ros._Seek( _sp );
    }
  }
  void  _Tell( _TyStreamPos & _sp )
  {
    if ( t_fUseSeek )
    {
      _sp = m_ros._Tell( );
    }
  }
public:

  _graph_output_iter_base(  t_TyOutputStreamBase & _ros,
                            _TyGraphNodeBase * _pgnbCur,
                            _TyGraphLinkBase * _pglbCur,
                            bool _fClosedDirected,
                            bool _fDirectionDown,
                            t_TyAllocator const & _rAlloc )
    : _TyBase( _pgnbCur, _pglbCur, _fClosedDirected, _fDirectionDown, _rAlloc, false ),
      m_ros( _ros ),
      m_iContextsOutput( 0 ),
      m_fDirectionDownOutput( !_fDirectionDown ), // Force a direction record to be written immediately.
      m_fWroteGraphFooter( false )
  {
    _Init();
  }

#if 0 // This is too poorly defined now - may b able to define better later.
  explicit _graph_output_iter_base( _TyThis const & _r,
                                    t_TyOutputStreamBase & _ros )
    : _TyBase( _r ),
      m_ros( _ros ),
      m_iContextsOutput( 0 )
  {
    assert( &m_ros != &_r.m_ros );
  }
#endif //0

  // Allow initialization with the base - this allows us to skip sections of the graph
  //  using a graph iterator and then initialize the output iterator with the state
  //  of this iterator and start the serialization from there ( this requires special
  //  consideration on read ).
  // This constructor also expensive.
  explicit _graph_output_iter_base( _TyBase const & _r,
                                    t_TyOutputStreamBase & _ros )
    : _TyBase( _r, false ),
      m_ros( _ros ),
      m_iContextsOutput( _r.m_iContexts ),
      m_fDirectionDownOutput( !_r.m_fDirectionDown ),// Force a direction record to be written immediately.
      m_fWroteGraphFooter( false )
    {
      _Init();
    }

  // Initialization with the base's base class - this allows us to initialize the
  //  output iterator with a iterator_pos ( i.e. dgraph::begin() ).
  explicit _graph_output_iter_base(typename _TyBase::_TyFwdIterBaseBase const & _r,
                                    t_TyOutputStreamBase & _ros )
    : _TyBase( _r, false ),
      m_ros( _ros ),
      m_iContextsOutput( 0 ),
      m_fDirectionDownOutput( !_r.m_fDirectionDown ),// Force a direction record to be written immediately.
    m_fWroteGraphFooter( false )
  {
    _Init();
  }

  ~_graph_output_iter_base() _BIEN_NOTHROW
  {
  }

  // REVIEW: Comparison with end() will return true even if the footer has not been written.
  // May want to override and fix this somehow.
  bool FAtEnd()
  {
    return _TyBase::FAtEnd() && m_fWroteGraphFooter;
  }

  // This should only before the iteration is begun:
  void  SetDirection( bool _fDirectionDown )
  {
    if ( _fDirectionDown != _TyBase::m_fDirectionDown )
    {
      assert( !m_iContextsOutput );
      m_fNewUnfinished = false; // Clear any state.
      m_ros._SetDirection( _fDirectionDown );
      _TyBase::SetDirection( _fDirectionDown );
      m_fDirectionDownOutput = !_fDirectionDown;  // Force a direction record to be written immed.
    }
  }

  _TyGNIndex _NotifyUnfinished( bool _fNew, _TyGraphNodeBase * _pgnb, _TyGraphLinkBase * _pglb )
  {
    if ( _fNew )
    {
      m_fNewUnfinished = true;
      m_pglbUnfinished = _pglb;
      return 1; // Return bogus count - we will count during output.
    }
    else
    {
      assert( _pglb );
      assert( _pgnb );
      // We merely re-write the link footer here - it has already been written in
      //  a default manner:
      if ( t_fUseSeek )
      {
        m_ros._Seek( m_spBeforeLinkFooter );
      }
      else
      {
        // Then indicate that we wrote the footer:
        m_fWroteLinkFooter = true;
      }

      m_ros._WriteLinkFooter( _TyBase::_FAtUnfinishedNode() ? 0 : _pglb, _pgnb ); // Indicate connection to _pgnb.
      return 1; // Return value is ignored.
    }
  }

  // This method does all the work.
  void _Next()
    {
      // We write the current state and then we change it - first we record where
      //  we are right now in case we throw.
      _TyStreamPos  sp;
      _Tell( sp );
      
      // We can manage the context writing separately from the node and link writing 
      //  irrespective of exceptions that may occur afterwards.
      // If we pushed or popped a context since the last check then update stream:
      if ( m_iContextsOutput != _TyBase::m_iContexts )
      {
        _BIEN_TRY
        {
          m_ros._WriteContext( m_iContextsOutput < _TyBase::m_iContexts ); // throws.
          m_iContextsOutput = _TyBase::m_iContexts;
        }
        _BIEN_UNWIND( _Seek( sp ) );
        _Tell( sp );
      }
      
      
      // Take care of any direction change that may have occured since we last checked:
      if ( m_fDirectionDownOutput != _TyBase::m_fDirectionDown )
      {
        _BIEN_TRY
        {
          m_ros._WriteDirectionChange( _TyBase::m_fDirectionDown ); // throws.
          m_fDirectionDownOutput = _TyBase::m_fDirectionDown;
        }
        catch( ... )
        {
          _Seek( sp );
          throw;
        }
//        _BIEN_UNWIND( _Seek( sp ) );
        _Tell( sp );
      }
      
      bool  _fNewUnfinished = m_fNewUnfinished; // save state for throw.
      
      _TyGraphLinkBase *  _pglbCheckWriteFooter = 0;
      bool                _fAtUnfinishedNode;
      _BIEN_TRY
      {
        // See where we are right now:
        if ( _TyBase::PGLBCur() )
        {
          m_ros._WriteLinkHeader( _TyBase::PGLBCur(), 
                                  _TyBase::_FAtUnfinishedNode() ? _TyBase::PGNBCur() : 0 ); // throws.
          
          if ( _TyBase::PGLBCur()->FIsConstructed() )
          {
            (this->*m_pmfnWriteLink)(); // throws.
          }
          
          // Save our place - we will re-write the footer if we encounter an unfinished node.
          if ( t_fUseSeek )
          {
            m_spBeforeLinkFooter = m_ros._Tell();
            m_ros._WriteLinkFooter( _TyBase::_FAtUnfinishedNode() ? 0 : _TyBase::PGLBCur(), 0 ); // Indicate no re-connected node. throws.
          }
          else
          {
            _pglbCheckWriteFooter = _TyBase::PGLBCur();
            _fAtUnfinishedNode = _TyBase::_FAtUnfinishedNode();
            m_fWroteLinkFooter = false; // Indicate that we have not written the link footer
                                      //  we will check for this after calling the base.
          }
        }
        else
        if ( _TyBase::PGNBCur() )
        {
          // If this is a new unfinished node then write the link information.
          if ( m_fNewUnfinished )
          {
            m_ros._WriteNewUnfinishedNodeHeader( _TyBase::PGNBCur(), m_pglbUnfinished );
          }
          else
          {
            // Just write a simple node header.
            m_ros._WriteNodeHeader( _TyBase::PGNBCur() );
          }

          // write the node itself:
          (this->*m_pmfnWriteNode)();
        
          if ( m_fNewUnfinished )
          {
            // We write the link information for the unfinished node now - this
            //  allows easier throw-safe construction of the graph on read:
            // Update the remaining links after writing the footer:
            _TyBase::m_itNodeLastInserted->second.m_iRemainingLinks = 
            m_ros._WriteUnfinishedNodeFooter( _TyBase::PGNBCur(), m_pglbUnfinished ) -
            !!m_pglbUnfinished;
            m_fNewUnfinished = false;
          }
          else
          {
            m_ros._WriteNodeFooter( _TyBase::PGNBCur() );
          }
        }
        else
        {
          if ( !m_fWroteGraphFooter )
          {
            // Then write the graph footer and return.
            _WriteGraphFooter();
          }
          else
          {
            assert( 0 );  // Attempt to iterate beyond the end.
          }
          return;
        }
        
        // Move to the next iteration state:
        _TyBase::_Next(); // throws.
        
        if ( !t_fUseSeek )
        {
          if ( _pglbCheckWriteFooter && !m_fWroteLinkFooter )
          {
            // Then we didn't write the footer:
            m_ros._WriteLinkFooter( _fAtUnfinishedNode ? 0 : _pglbCheckWriteFooter, 0 ); // Indicate no re-connected node. throws.
          }
        }
        
      } 
      catch( ... )
      {
        _Seek( sp );
        m_fNewUnfinished = _fNewUnfinished;
        throw;
      }
//      _BIEN_UNWIND( ( _Seek( sp ), 
//                      ( m_fNewUnfinished = _fNewUnfinished ) ) );

      if ( _TyBase::FAtEnd() && !m_fWroteGraphFooter )
      {
        _WriteGraphFooter();
      }
    }

    void _WriteGraphFooter()
    {
      m_ros._WriteGraphFooter();
      m_fWroteGraphFooter = true;
    }
  
};

// Declare the output iterator wrapper - this wraps objects that are eventually
//  based on _graph_output_iter_base:

template <  class t_TyGraphNode, class t_TyGraphLink, class t_TyOutputStream,
            class t_TyBaseOutIter, class t_TyFIsConstIterator >
struct _graph_output_iterator
  : public t_TyBaseOutIter,
    public _graph_iter_const_base< t_TyGraphNode, t_TyGraphLink, t_TyFIsConstIterator >
{
private:
  typedef t_TyBaseOutIter _TyBase;
  typedef _graph_iter_const_base<	t_TyGraphNode, t_TyGraphLink, t_TyFIsConstIterator > _tyConstIterBase;
  typedef _graph_output_iterator _TyThis;
public:
  typedef _TyBase _TyIterBase;  // This type supported by all graph iterators.

  typedef _graph_output_iterator< t_TyGraphNode, t_TyGraphLink, t_TyOutputStream,
                                  t_TyBaseOutIter, std::false_type >   iterator;
  typedef _graph_output_iterator< t_TyGraphNode, t_TyGraphLink, t_TyOutputStream,
                                  t_TyBaseOutIter, std::true_type >    const_iterator;

  typedef t_TyOutputStream _TyOutputStream;
  typedef typename _TyOutputStream::_TyInitArg _TyInitArg;
  typedef typename _TyOutputStream::_TyIONodeEl _TyIONodeEl;
  typedef typename _TyOutputStream::_TyIOLinkEl _TyIOLinkEl;

  typedef typename _tyConstIterBase::_TyGraphNodeCQ _TyGraphNodeCQ;
  typedef typename _tyConstIterBase::_TyGraphLinkCQ _TyGraphLinkCQ;
  typedef typename _tyConstIterBase::node_reference node_reference;
  typedef typename _tyConstIterBase::link_reference link_reference;
  typedef typename _tyConstIterBase::node_pointer node_pointer;
  typedef typename _tyConstIterBase::link_pointer link_pointer;
  typedef typename _tyConstIterBase::_TyGraphNode _TyGraphNode;
  typedef typename _tyConstIterBase::_TyGraphLink _TyGraphLink;
  typedef typename _tyConstIterBase::_TyGraphNodeBaseBase  _TyGraphNodeBaseBase;
  typedef typename _tyConstIterBase::_TyGraphLinkBaseBase  _TyGraphLinkBaseBase;

  _TyOutputStream m_os; // The output stream object.

  _graph_output_iterator( _TyInitArg _oia,
                          _TyGraphNodeCQ * _pgnbCur,
                          _TyGraphLinkCQ * _pglbCur,
                          bool _fClosedDirected,
                          bool _fDirectionDown,
                          typename _TyBase::_TyAllocatorAsPassed const & _rAlloc,
                          _TyIONodeEl const & _rione = _TyIONodeEl(),
                          _TyIOLinkEl const & _riole = _TyIOLinkEl() )
    : _TyBase(  m_os, 
                const_cast< t_TyGraphNode* >( _pgnbCur ), 
                const_cast< t_TyGraphLink* >( _pglbCur ), 
                _fClosedDirected, _fDirectionDown, _rAlloc ),
      m_os( _oia, _fDirectionDown, _rione, _riole, _rAlloc )
  {
    // Set up function pointers in the base:
    _TyBase::m_pmfnWriteNode = static_cast< typename _TyBase::_TyPMFnWriteNode >( &_TyThis::_WriteNode );
    _TyBase::m_pmfnWriteLink = static_cast< typename _TyBase::_TyPMFnWriteLink >( &_TyThis::_WriteLink );
  }

  // Initialization with the base's base class - this allows us to initialize the
  //  output iterator with a iterator_pos ( i.e. dgraph::begin() ).
  explicit _graph_output_iterator( _TyInitArg _oia,
    typename _TyBase::_TyFwdIterBaseBase const & _r,
    _TyIONodeEl const & _rione = _TyIONodeEl(),
    _TyIOLinkEl const & _riole = _TyIOLinkEl() )
    : _TyBase( _r, m_os ),
      m_os( _oia, _r.m_fDirectionDown, _rione, _riole, _r.get_allocator() )
  {
    // Set up function pointers in the base:
    _TyBase::m_pmfnWriteNode = static_cast< typename _TyBase::_TyPMFnWriteNode >( &_TyThis::_WriteNode );
    _TyBase::m_pmfnWriteLink = static_cast< typename _TyBase::_TyPMFnWriteLink >( &_TyThis::_WriteLink );
  }

  // Template that accesses similar named method in the output object:
  template < class t_TyOptions >
  void  SetOutputOptions( t_TyOptions _o )
  {
    m_os.template SetOutputOptions< t_TyOptions >( _o );
  }

  _TyGraphNodeCQ *  PGNCur()
  {
    return const_cast< _TyGraphNodeCQ * >( static_cast< t_TyGraphNode * >( _TyBase::PGNBCur() ) );
  }
  _TyGraphLinkCQ *  PGLCur()
  {
    return const_cast< _TyGraphLinkCQ * >( static_cast< t_TyGraphLink * >( _TyBase::PGLBCur() ) );
  }

  // note: may not has a node ( may be at end of iteration ).
  node_reference    RNodeEl() const _BIEN_NOTHROW
  {
    return const_cast< node_reference >( *static_cast< _TyGraphNode * >( _TyBase::PGNBCur() ) );
  }
  // note: may not have a link!
  link_reference    RLinkEl() const _BIEN_NOTHROW
  {
    return const_cast< link_reference >( *static_cast< _TyGraphLink * >( _TyBase::PGLBCur() ) );
  }

  // The way this works: if the link_pointer is non-null then the iteration is currently
  //  at a link. Otherwise the iteration either at node_pointer or at the end() if node_pointer null.
  pair< link_pointer, node_pointer >  PairCur() const _BIEN_NOTHROW
  {
    return pair< link_pointer, node_pointer >( _TyBase::PGLBCur() ? &RLinkEl() : 0, _TyBase::PGNBCur() ? &RNodeEl() : 0 );
  }
  
  _TyThis & operator ++()
  {
    _TyBase::_Next();
    return *this;
  }

protected:

  void  _WriteNode()
  {
    m_os._WriteNode( PGNCur() );
  }
  void  _WriteLink()
  {
    m_os._WriteLink( PGLCur() );
  }
};

__DGRAPH_END_NAMESPACE

#endif //__GR_SRIL
