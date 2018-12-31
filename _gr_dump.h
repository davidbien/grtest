#ifndef __GR_DUMP_H
#define __GR_DUMP_H

// _gr_dump.h

// Human-readable(barely:-) dumping of graph to a stream.
// Dumped in depth first order - need to maintain a context stack
//  if you are trying to reconstruct by hand.

#include <map>

__DGRAPH_BEGIN_NAMESPACE

enum EDumpOptions
{
  e_doNone,
  e_doMapPointers
    // We map the node/link pointers to unique ids as we dump.
    // This allows comparison of graphs that aren't at the same
    //  place in memory.
};

template <  class t_TyGraphNodeBase, 
            class t_TyGraphLinkBase, 
            class t_TyStreamObject,
            class t_TyAllocator >
struct _dump_object_base
{
private:
  typedef _dump_object_base<  t_TyGraphNodeBase, t_TyGraphLinkBase, 
                              t_TyStreamObject, t_TyAllocator >     _TyThis;
public:
  typedef t_TyAllocator _TyAllocator;
  typedef t_TyGraphLinkBase _TyGraphLinkBase;
  typedef t_TyGraphNodeBase _TyGraphNodeBase;

  typedef typename t_TyStreamObject::_TyInitArg   _TyInitArg;
  typedef typename t_TyStreamObject::_TyStreamPos _TyStreamPos;
  typedef typename t_TyStreamObject::_TyIONodeEl  _TyIONodeEl;
  typedef typename t_TyStreamObject::_TyIOLinkEl  _TyIOLinkEl;

  t_TyStreamObject  m_ros;            // The stream into which we are dumping.
  bool              m_fDirectionDown; // The current direction of the iteration.
  bool              m_fWaitWriteStateNumber;
  int               m_iStateNumber;
  bool              m_fOutputOn;      // Allow caller to turn off output.
  EDumpOptions      m_doOptions;  // Dump options.

  // Graph object -> id mapping stuff:
  typedef size_t  _TyObjectId;
  _TyObjectId       m_idNodeCur;   // Current node id.
  _TyObjectId       m_idLinkCur;   // Current link id.
  typedef typename _Alloc_traits< typename map< void *, _TyObjectId, less< void * > >::value_type, _TyAllocator >::allocator_type _TyAllocatorMapPv;
  typedef map< void *, _TyObjectId, less< void * >, _TyAllocatorMapPv > _TyMapToIds;
  _TyMapToIds       m_mapNodeIds;
  _TyMapToIds       m_mapLinkIds;

  __DEBUG_STMT( bool m_fSetDirection )

  _dump_object_base(  _TyInitArg _ros, bool _fDirectionDown, 
                      _TyIONodeEl const & _rione, 
                      _TyIOLinkEl const & _riole,
                      t_TyAllocator const & _rA )
    : m_ros( _ros, _rione, _riole ),
      m_fDirectionDown( _fDirectionDown ),
      m_fWaitWriteStateNumber( false ),
      m_iStateNumber( 0 ),
      m_fOutputOn( true ),
      m_doOptions( e_doNone ),
      m_idNodeCur( 0 ),
      m_idLinkCur( 0 ),
      m_mapNodeIds( typename _TyMapToIds::key_compare(), _rA ),
      m_mapLinkIds( typename _TyMapToIds::key_compare(), _rA )
#ifndef NDEBUG
      ,m_fSetDirection( 0 )
#endif //!NDEBUG
  {
  }

  template < class t_TyOptions >
  void  SetOutputOptions( t_TyOptions _o )
  {
    m_doOptions = _o;
  }

  void          _SetDirection( bool _fDirectionDown )
  {
    __DEBUG_STMT( m_fSetDirection = 0 )
    m_fDirectionDown = _fDirectionDown;
  }

  _TyStreamPos  _Tell() { return m_ros.TellP(); }
  void          _Seek( _TyStreamPos _sp ) { (void)m_ros.SeekP( _sp ); }

  void _WriteStateNumber()
  {
    if ( m_fWaitWriteStateNumber )
    {
      m_fWaitWriteStateNumber = false;
    }
    else
    {
      if ( m_fOutputOn )
      {
        __THROWPT( e_ttFileOutput | e_ttMemory );
        m_ros << "{" << m_iStateNumber << "}:";
      }
      m_iStateNumber++;
    }
  }

  void _WriteContext( bool _fPush )
  {
    _WriteStateNumber();
    m_fWaitWriteStateNumber = true;

    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
      m_ros << ( _fPush ? "<Push" : "<Pop" ) << " context>\n";
    }
  }
  void _WriteDirectionChange( bool _fDirectionDown )
  {
    // We should always be up-to-date:
    assert( !m_fSetDirection || ( m_fDirectionDown == !_fDirectionDown ) );

    _WriteStateNumber();
    m_fWaitWriteStateNumber = true;

    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
      m_ros << "<Change direction to " << 
        ( _fDirectionDown ? "down>\n" : "up>\n" );
    }
    __DEBUG_STMT( m_fSetDirection  = 1 )
    m_fDirectionDown = _fDirectionDown;
  }

  void  _WriteNodePtr( t_TyGraphNodeBase * _pgnb )
  {
    m_ros << "(";
    if ( e_doMapPointers == m_doOptions )
    {
      typedef pair< typename _TyMapToIds::iterator, bool > _TyPib;
      _TyPib pib = m_mapNodeIds.
        insert( typename _TyMapToIds::value_type( _pgnb, m_idNodeCur ) );

      m_ros << pib.first->second;

      if ( pib.second )
      {
        ++m_idNodeCur;
      }
    }
    else
    {
      m_ros << _pgnb;
    }
    m_ros << ")";
  }

  void  _WriteLinkPtr( t_TyGraphLinkBase * _pglb )
  {
    m_ros << "<";
    if ( e_doMapPointers == m_doOptions )
    {
      typedef pair< typename _TyMapToIds::iterator, bool > _TyPib;
      _TyPib pib = m_mapLinkIds.
        insert( typename _TyMapToIds::value_type( _pglb, m_idLinkCur ) );

      m_ros << pib.first->second;

      if ( pib.second )
      {
        ++m_idLinkCur;
      }
    }
    else
    {
      m_ros << _pglb;
    }
    m_ros << ">";
  }

  // Return the number of relatives in the opposite direction of the current iteration.
  void _WriteNewUnfinishedNodeHeader( t_TyGraphNodeBase * _pgnb, 
                                      t_TyGraphLinkBase * _pglb )
  {
    _WriteStateNumber();
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
#ifdef __GR_DUMP_DONTWRITENAMES
      _pgnb = 0;
      _pglb = 0;
#endif //__GR_DUMP_DONTWRITENAMES
      m_ros << "New unfinished node: ";
      _WriteNodePtr( _pgnb );
      m_ros << " entered from link ";
      _WriteLinkPtr( _pglb );
      m_ros << ".\n";
    }
  }

  int _WriteUnfinishedNodeFooter( t_TyGraphNodeBase * _pgnb, 
                                  t_TyGraphLinkBase * _pglb )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
#ifdef __GR_DUMP_DONTWRITENAMES
      t_TyGraphNodeBase * _pgnbWrite = 0;
      t_TyGraphLinkBase * _pglbWrite = 0;
#else //__GR_DUMP_DONTWRITENAMES
      t_TyGraphNodeBase *& _pgnbWrite = _pgnb;
      t_TyGraphLinkBase *& _pglbWrite = _pglb;
#endif //__GR_DUMP_DONTWRITENAMES
      
      m_ros << "Unfinished node footer: ";
      _WriteNodePtr( _pgnbWrite );
      m_ros << " entered from link ";
      _WriteLinkPtr( _pglbWrite );
      m_ros << ".\n";
      m_ros << ( m_fDirectionDown ? "Parent" : "Child" ) << " relations [";
      int  iRelations = 0;
      t_TyGraphLinkBase ** _ppglb;
      for ( _ppglb = _pgnb->PPGLBRelationHead( !m_fDirectionDown );
            (*_ppglb)->PGLBGetNextRelation( !m_fDirectionDown );
            _ppglb = (*_ppglb)->PPGLBGetNextRelation( !m_fDirectionDown ),
            iRelations++ )
      {
#ifdef __GR_DUMP_DONTWRITENAMES
        _WriteLinkPtr( 0 );
#else //__GR_DUMP_DONTWRITENAMES
        _WriteLinkPtr( *_ppglb );
#endif //__GR_DUMP_DONTWRITENAMES
        m_ros << ",";
      }
      
      // Write last link:
#ifdef __GR_DUMP_DONTWRITENAMES
      _WriteLinkPtr( 0 );
#else //__GR_DUMP_DONTWRITENAMES
      _WriteLinkPtr( *_ppglb );
#endif //__GR_DUMP_DONTWRITENAMES
      m_ros << "].\n";
    
      return ++iRelations;
    }
    else
    {
      return _pgnb->URelations( !m_fDirectionDown );
    }
  }

  void _WriteNodeHeader( t_TyGraphNodeBase * _pgnb )
  {
    _WriteStateNumber();
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
#ifdef __GR_DUMP_DONTWRITENAMES
      _pgnb = 0;
#endif //__GR_DUMP_DONTWRITENAMES
      m_ros << "Normal node: ";
      _WriteNodePtr( _pgnb );
      m_ros << ".\n";
    }
  }

  void _WriteNodeFooter( t_TyGraphNodeBase * _pgnb )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
#ifdef __GR_DUMP_DONTWRITENAMES
      _pgnb = 0;
#endif //__GR_DUMP_DONTWRITENAMES
      m_ros << "Node footer: ";
      _WriteNodePtr( _pgnb );
      m_ros << ".\n";
    }
  }

  void _WriteLinkHeader(  t_TyGraphLinkBase * _pglb, 
                          t_TyGraphNodeBase * _pgnbUnfinished )
  {
    _WriteStateNumber();
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
#ifdef __GR_DUMP_DONTWRITENAMES
      t_TyGraphLinkBase * _pglbWrite = 0;
#else //__GR_DUMP_DONTWRITENAMES
      t_TyGraphLinkBase *& _pglbWrite = _pglb;
#endif //__GR_DUMP_DONTWRITENAMES
      m_ros << "Link: ";
      _WriteLinkPtr( _pglbWrite );
      if ( _pgnbUnfinished )
      {
#ifdef __GR_DUMP_DONTWRITENAMES
        _pgnbUnfinished = 0;
#endif //__GR_DUMP_DONTWRITENAMES
        m_ros << " iterating unfinished node ";
        _WriteNodePtr( _pgnbUnfinished );
      }
      m_ros << ".\n";
      if ( !_pglb->FIsConstructed() )
      {
        m_ros << "	***Link is Empty***.\n";
      }
    }
  }
  void _WriteLinkFooter(  t_TyGraphLinkBase * _pglb, 
                          t_TyGraphNodeBase * _pgnb )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
#ifdef __GR_DUMP_DONTWRITENAMES
      _pglb = 0;
#endif //__GR_DUMP_DONTWRITENAMES
      m_ros << "Link footer: ";
      _WriteLinkPtr( _pglb );
      if ( _pgnb )
      {
#ifdef __GR_DUMP_DONTWRITENAMES
        _pgnb = 0;
#endif //__GR_DUMP_DONTWRITENAMES
        m_ros << " enters into unfinished node ";
        _WriteNodePtr( _pgnb );
      }
      m_ros << ".\n";
    }
  }
  void _WriteGraphFooter()
    {
      if ( m_fOutputOn )
      {
        __THROWPT( e_ttFileOutput | e_ttMemory );
        m_ros << "< Graph Footer >\n";
      }
    }
};

template <  class t_TyGraphNode, class t_TyGraphLink, 
            class t_TyStreamObject, class t_TyAllocator >
struct _dump_object
  : public _dump_object_base< typename t_TyGraphNode::_TyGraphNodeBaseBase,
                              typename t_TyGraphLink::_TyGraphLinkBaseBase, 
                              t_TyStreamObject, t_TyAllocator >
{
private:
  typedef _dump_object< t_TyGraphNode, t_TyGraphLink, 
                        t_TyStreamObject, t_TyAllocator >                     _TyThis;
  typedef _dump_object_base<  typename t_TyGraphNode::_TyGraphNodeBaseBase,
                              typename t_TyGraphLink::_TyGraphLinkBaseBase,
                              t_TyStreamObject, t_TyAllocator >               _TyBase;
public:

  typedef _TyBase   _TyOutputStreamBase;
  typedef typename _TyBase::_TyInitArg _TyInitArg;
  typedef typename _TyBase::_TyIONodeEl _TyIONodeEl;
  typedef typename _TyBase::_TyIOLinkEl _TyIOLinkEl;

  _dump_object( _TyInitArg _ros, bool _fDirectionDown,
                _TyIONodeEl const & _rione, 
                _TyIOLinkEl const & _riole,
                t_TyAllocator const & _rA )
    : _TyBase( _ros, _fDirectionDown, _rione, _riole, _rA )
  {
  }

  void    _WriteNode( const t_TyGraphNode * _pgn )
  {
    if ( _TyBase::m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
      _TyBase::m_ros.WriteNodeEl( _pgn->RElConst() );
    }
  }

  void    _WriteLink( const t_TyGraphLink * _pgl )
  {
    if ( _TyBase::m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput | e_ttMemory );
      _TyBase::m_ros.WriteLinkEl( _pgl->RElConst() );
    }
  }
};

__DGRAPH_END_NAMESPACE

#endif //__GR_DUMP_H
