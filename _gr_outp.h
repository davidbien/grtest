#ifndef __GR_OUTP_H
#define __GR_OUTP_H

// _gr_outp.h

// This module defines the graph binary output iterator.

#if defined( __STL_USE_EXCEPTIONS )
//    !defined( __GR_DONTTHROWBADGRAPHERRORS )
#define __GR_THROWBADGRAPHERRORS
#endif

#ifdef __GR_THROWBADGRAPHERRORS
// We use the SGI exception stuff:
#include <stdexcept>
#endif __GR_THROWBADGRAPHERRORS

__DGRAPH_BEGIN_NAMESPACE

#ifdef __GR_THROWBADGRAPHERRORS

class bad_graph : public _t__Named_exception< __DGRAPH_DEFAULT_ALLOCATOR >
{
  typedef _t__Named_exception< __DGRAPH_DEFAULT_ALLOCATOR > _TyBase;
public:
  bad_graph( const string_type & __s ) : _TyBase( __s ) {}
};

#else __GR_THROWBADGRAPHERRORS
#error This not currently supported. Need error propagation - not too tough, but...
#endif __GR_THROWBADGRAPHERRORS


// Function that knows how to write raw data to a stream:
template < class t_TyOutputStream, class t_TyWrite >
void
_RawWriteGraphEl( t_TyOutputStream & _ros, t_TyWrite const & _rel )
{ 
  assert( 0 ); // Should specialize for each stream type.
}

// Tokens for the binary representation:
template < class t_TyDummy = __false_type > // make a template so that constants link.
struct _binary_rep_tokens
{
  typedef unsigned char _TyToken;

  static const _TyToken ms_ucDirectionUp          = 0x01;
  static const _TyToken ms_ucDirectionDown        = 0x02;
  static const _TyToken ms_ucContextPush          = 0x03;
  static const _TyToken ms_ucContextPop           = 0x04;
  static const _TyToken ms_ucNode                 = 0x05;
  static const _TyToken ms_ucUnfinishedNode       = 0x06;
#ifdef __GR_BINARY_WRITENODEFOOTER
  static const _TyToken ms_ucNodeFooter           = 0x07;
#endif __GR_BINARY_WRITENODEFOOTER
  static const _TyToken ms_ucLink                 = 0x08;
  static const _TyToken ms_ucLinkFromUnfinished   = 0x09;
  static const _TyToken ms_ucNormalLinkFooter     = 0x0a;
  static const _TyToken ms_ucUnfinishedLinkFooter = 0x0b;
  static const _TyToken ms_ucGraphFooter  = 0x0c;

  // Link construction indicators:
  static const _TyToken ms_ucLinkConstructed      = 0x01; // A constructed link.
  static const _TyToken ms_ucLinkEmpty            = 0x00; // An unconstructed link.
};

// Some compilers, intel for instance, require that these be declared externally to have linkage.
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucDirectionUp;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucDirectionDown;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucContextPush;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucContextPop;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucNode;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucUnfinishedNode;
#ifdef __GR_BINARY_WRITENODEFOOTER
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucNodeFooter;
#endif __GR_BINARY_WRITENODEFOOTER
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucLink;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucLinkFromUnfinished;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucNormalLinkFooter;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucUnfinishedLinkFooter;
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucGraphFooter;

  // Link construction indicators:
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucLinkConstructed; // A constructed link.
template < class t_TyDummy >
const typename  _binary_rep_tokens< t_TyDummy >::_TyToken 
                _binary_rep_tokens< t_TyDummy >::ms_ucLinkEmpty; // An unconstructed link.

template <  class t_TyGraphNodeBase, class t_TyGraphLinkBase,
            class t_TyStreamObject,
            // This causes all available information to be written - i.e.
            //  the names ( pointers ) of all links and nodes - this is not needed by all readers.
            bool t_fWriteExtraInformation,
            // Setting this to true allows the writer to write unconstucted links.
            //  If it is false and the writer encounters an unconstucted link it will throw
            //  a bad_graph exception.
            bool t_fAllowUnconstructedLinks = false > 
struct _binary_output_base
{
private:
  typedef _binary_output_base<  t_TyGraphNodeBase, t_TyGraphLinkBase,
                                t_TyStreamObject,
                                t_fWriteExtraInformation, 
                                t_fAllowUnconstructedLinks >  _TyThis;
public:

  typedef t_TyGraphLinkBase _TyGraphLinkBase;
  typedef t_TyGraphNodeBase _TyGraphNodeBase;

  typedef typename t_TyStreamObject::_TyInitArg       _TyInitArg;
  typedef typename t_TyStreamObject::_TyStreamPos     _TyStreamPos; // A position within a stream.
  typedef typename t_TyStreamObject::_TyIONodeEl      _TyIONodeEl;
  typedef typename t_TyStreamObject::_TyIOLinkEl      _TyIOLinkEl;

  t_TyStreamObject  m_ros;            // The stream into which we are dumping.
  bool              m_fDirectionDown; // The current direction of the iteration.
  bool              m_fOutputOn;      // Allow caller to turn off output 
                                      //  ( dangerous - if you want to read it in again ).

  __DEBUG_STMT( bool m_fSetDirection )

  _binary_output_base(  _TyInitArg _ria, bool _fDirectionDown,
                        _TyIONodeEl const & _rione,
                        _TyIOLinkEl const & _riole )
    : m_ros( _ria, _rione, _riole ),
      m_fDirectionDown( _fDirectionDown ),
      m_fOutputOn( true )
#ifndef NDEBUG
      ,m_fSetDirection( 0 )
#endif !NDEBUG
  {
  }

  void _SetDirection( bool _fDirectionDown )
  {
    __DEBUG_STMT( m_fSetDirection = 0 )
    m_fDirectionDown = _fDirectionDown;
  }

  _TyStreamPos  _Tell()                   { return m_ros.TellP(); }
  void          _Seek( _TyStreamPos _sp ) { m_ros.SeekP( _sp ); }

  typedef typename _binary_rep_tokens< __false_type >::_TyToken _TyToken;
  void _WriteToken( _TyToken const & uc )
  {
    __THROWPT( e_ttFileOutput );
    m_ros.Write( &uc, sizeof uc );
  }
  void _WritePtr( void * _pv )
  {
    __THROWPT( e_ttFileOutput );
    m_ros.Write( &_pv, sizeof( _pv ) );
  }

  void _WriteContext( bool _fPush )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      _WriteToken( _fPush ? 
                      _binary_rep_tokens< __false_type >::ms_ucContextPush : 
                      _binary_rep_tokens< __false_type >::ms_ucContextPop );
    }
  }
  void _WriteDirectionChange( bool _fDirectionDown )
  {
    // We should always be up-to-date:
    assert( !m_fSetDirection || ( m_fDirectionDown == !_fDirectionDown ) );
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      _WriteToken( _fDirectionDown ? 
                      _binary_rep_tokens< __false_type >::ms_ucDirectionDown : 
                      _binary_rep_tokens< __false_type >::ms_ucDirectionUp );
    }
    __DEBUG_STMT( m_fSetDirection  = 1 )
    m_fDirectionDown = _fDirectionDown; // Change here in case of throw.
  }

  void _WriteNewUnfinishedNodeHeader( t_TyGraphNodeBase * _pgnb, 
                                      t_TyGraphLinkBase * _pglb )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      // Write the token, the node and the link:
      _WriteToken( _binary_rep_tokens< __false_type >::ms_ucUnfinishedNode );
      _WritePtr( _pgnb );
      _WritePtr( _pglb );
    }
  }

  // Return the number of relatives in the opposite direction of the current iteration.
  int _WriteUnfinishedNodeFooter( t_TyGraphNodeBase * _pgnb, 
                                  t_TyGraphLinkBase * _pglb )
  {
    // We write a list of links - terminated by a zero pointer.
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      int  iRelations = 0;
      t_TyGraphLinkBase ** _ppglb;
      for ( _ppglb = _pgnb->PPGLBRelationHead( !m_fDirectionDown );
            ;
            _ppglb = (*_ppglb)->PPGLBGetNextRelation( !m_fDirectionDown ),
            iRelations++ )
      {
        _WritePtr( *_ppglb );
        if ( !*_ppglb )
          break;  // We want to write an extra zero on the end.
      }
      return iRelations;
    }
    else
    {
      return _pgnb->URelations( !m_fDirectionDown );
    }
  }

  void _WriteNodeHeader( t_TyGraphNodeBase * _pgnb )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      _WriteToken( _binary_rep_tokens< __false_type >::ms_ucNode );
      if ( t_fWriteExtraInformation )
      {
        _WritePtr( _pgnb );
      }
    }
  }

  void _WriteNodeFooter( t_TyGraphNodeBase * _pgnb )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
#ifdef __GR_BINARY_WRITENODEFOOTER
      // Not really necessary to write a node footer - but good for 
      //  debugging ( and its only a byte ).
      _WriteToken( _binary_rep_tokens::ms_ucNodeFooter );
#endif __GR_BINARY_WRITENODEFOOTER
    }
  }

  void _WriteLinkHeader(  t_TyGraphLinkBase * _pglb, 
                          t_TyGraphNodeBase * _pgnbUnfinished )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      if ( _pgnbUnfinished )
      {
        // Then we need to write the node name ( pointer ):
        _WriteToken( _binary_rep_tokens< __false_type >::ms_ucLinkFromUnfinished );
        _WritePtr( _pglb );
        if ( t_fWriteExtraInformation )
        {
          // This is unnecessary - the links are uniquely identified by their own name.
          _WritePtr( _pgnbUnfinished );
        }
      }
      else
      {
        _WriteToken( _binary_rep_tokens< __false_type >::ms_ucLink );
        if ( t_fWriteExtraInformation )
        {
          _WritePtr( _pglb );
        }
      }
      if ( t_fAllowUnconstructedLinks )
      {
        // Write whether or not the link is constructed:
        _WriteToken( _pglb->FIsConstructed() ? 
                        _binary_rep_tokens< __false_type >::ms_ucLinkConstructed : 
                        _binary_rep_tokens< __false_type >::ms_ucLinkEmpty );
      }
      else
      {
        if ( !_pglb->FIsConstructed() )
        {
          throw bad_graph( "_WriteLinkHeader(): Attempt to write an unconstructed link to stream when not permitted." );
        }
      }
    }
  }
  void          _WriteLinkFooter( t_TyGraphLinkBase * _pglb, 
                                  t_TyGraphNodeBase * _pgnbUnfinished )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      if ( _pgnbUnfinished )
      {
        // Write a footer record - the link and the node.
        _WriteToken( _binary_rep_tokens< __false_type >::ms_ucUnfinishedLinkFooter );
        _WritePtr( _pgnbUnfinished );
        if ( !t_fWriteExtraInformation && _pglb ) // If zero then we already wrote it in the header.
        {
          // We already wrote this in the header.
          _WritePtr( _pglb );
        }
      }
      else
      {
        // Normal link footer
        _WriteToken( _binary_rep_tokens< __false_type >::ms_ucNormalLinkFooter );
      }
    }
  }

  void _WriteGraphFooter()
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      _WriteToken( _binary_rep_tokens< __false_type >::ms_ucGraphFooter );
    }
  }
};

template <  class t_TyGraphNode, class t_TyGraphLink,
            class t_TyStreamObject, class t_TyAllocator,
            bool t_fWriteExtraInformation,
            bool t_fAllowUnconstructedLinks = false >
struct _binary_output_object
  : public _binary_output_base< typename t_TyGraphNode::_TyGraphNodeBaseBase,
                                typename t_TyGraphLink::_TyGraphLinkBaseBase,
                                t_TyStreamObject,
                                t_fWriteExtraInformation,
                                t_fAllowUnconstructedLinks >
{
private:
  typedef _binary_output_object<  t_TyGraphNode, t_TyGraphLink, 
                                  t_TyStreamObject, t_TyAllocator,
                                  t_fWriteExtraInformation, 
                                  t_fAllowUnconstructedLinks >                  _TyThis;
  typedef _binary_output_base<  typename t_TyGraphNode::_TyGraphNodeBaseBase,
                                typename t_TyGraphLink::_TyGraphLinkBaseBase,
                                t_TyStreamObject,
                                t_fWriteExtraInformation,
                                t_fAllowUnconstructedLinks >                    _TyBase;
public:

  typedef _TyBase _TyOutputStreamBase;
  typedef typename _TyBase::_TyInitArg _TyInitArg;
  typedef typename _TyBase::_TyIONodeEl _TyIONodeEl;
  typedef typename _TyBase::_TyIOLinkEl _TyIOLinkEl;

  _binary_output_object(  _TyInitArg _ros, bool _fDirectionDown,
                          _TyIONodeEl const & _rione,
                          _TyIOLinkEl const & _riole,
                          t_TyAllocator const & ) // don't need the allocator.
    : _TyBase( _ros, _fDirectionDown, _rione, _riole )
  {
  }

  void    _WriteNode( const t_TyGraphNode * _pgn )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      m_ros.WriteNodeEl( _pgn->RElConst() );
    }
  }

  void    _WriteLink( const t_TyGraphLink * _pgl )
  {
    if ( m_fOutputOn )
    {
      __THROWPT( e_ttFileOutput );
      m_ros.WriteLinkEl( _pgl->RElConst() );
    }
  }
};

__DGRAPH_END_NAMESPACE

#endif __GR_OUTP_H
