#ifndef __GR_FDIO_H
#define __GR_FDIO_H

// _gr_fdio.h
// graph I/O through file descriptors.
// dbien: 21APR2020

#include <fcntl.h>
#include <unistd.h>
#include "_gr_inc.h"

__DGRAPH_BEGIN_NAMESPACE

// Specialize for fdout - default version just writes raw memory:
template < class t_TyWrite >
__INLINE void
_RawWriteGraphEl( int _fd, t_TyWrite const & _rEl )
{	
	if ( sizeof( _rEl ) )
	{
    ssize_t sstWrit = ::write( _fd, (const void*)&_rEl, sizeof( _rEl ) );
    __THROWPT( e_ttFileOutput );
    if ( sstWrit != sizeof( _rEl ) )
      THROWNAMEDEXCEPTIONERRNO( errno, "_RawWriteGraphEl(fd): failed to write entire element." );
	}
}

// Specialize for fdin - default version just reads raw memory:
template < class t_TyRead >
__INLINE void
_RawReadGraphEl( int _fd, t_TyRead & _rEl )
{	
	if ( sizeof( _rEl ) )
	{
    ssize_t sstRead = ::read( _fd, (void*)&_rEl, sizeof( _rEl ) );
    __THROWPT( e_ttFileInput );
    if ( sstRead != sizeof( _rEl ) )
      THROWNAMEDEXCEPTIONERRNO( errno, "_RawReadGraphEl(fd): failed to read entire element." );
	}
}

struct _fd_RawElIO
{
  template < class t_TyEl >
  void Write( int _fd, t_TyEl const & _rel )
  {
    _RawWriteGraphEl( _fd, _rel );
  }
  template < class t_TyEl >
  void Read( int _fd, t_TyEl & _rel )
  {
    _RawReadGraphEl( _fd, _rel );
  }
};

template <  class t_TyOutputNodeEl,
            class t_TyOutputLinkEl = t_TyOutputNodeEl >
struct _fdout_object
{
  typedef int _TyInitArg;
  typedef off_t _TyStreamPos;
  typedef t_TyOutputNodeEl _TyIONodeEl;
  typedef t_TyOutputLinkEl _TyIOLinkEl;

  int m_fd{-1}; // This object doesn't own the lifetime of the open file.
  t_TyOutputNodeEl  m_one;
  t_TyOutputLinkEl  m_ole;

  _fdout_object( _fdout_object const & ) = delete;
  _fdout_object() = delete;
	_fdout_object(  int _fd,
                  t_TyOutputNodeEl const & _rone,
                  t_TyOutputLinkEl const & _role )
		: m_fd( _fd ),
      m_one( _rone ),
      m_ole( _role )
	{
      __THROWPT( e_ttMemory ); // in the cases where where are dynamic members within m_one or m_ole.
	}
	_fdout_object(  int _fd,
                  t_TyOutputNodeEl && _rrone,
                  t_TyOutputLinkEl && _rrole )
		: m_fd( _fd ),
      m_one( std::move( _rrone ) ),
      m_ole( std::move( _rrole ) )
	{
	}

	_TyStreamPos TellP() const
  {
    off_t off = lseek( m_fd, 0, SEEK_CUR );
    __THROWPT( e_ttFileOutput );
    if ( -1 == off )
      THROWNAMEDEXCEPTIONERRNO( errno, "_fdout_object::TellP(): lseek() failed." );
    return off;
  }
	void SeekP( _TyStreamPos _sp )	
  {
    off_t off = lseek( m_fd, _sp, SEEK_SET );
    __THROWPT( e_ttFileOutput );
    if ( -1 == off )
      THROWNAMEDEXCEPTIONERRNO( errno, "_fdout_object::SeekP(): lseek() failed." );
  }

	void Write( const void * _pv, size_t _st )
	{
    if ( _st )
    {
      ssize_t sstWrit = ::write( m_fd, _pv, _st );
      __THROWPT( e_ttFileOutput );
      if ( sstWrit != _st )
        THROWNAMEDEXCEPTIONERRNO( errno, "_fdout_object::Write(): ::write() failed." );
    }
	}

	template < class t_TyEl >
	void WriteNodeEl( t_TyEl const & _rel )
	{
    m_one.Write( m_fd, _rel );
	}
	template < class t_TyEl >
	void WriteLinkEl( t_TyEl const & _rel )
	{
    m_ole.Write( m_fd, _rel );
	}
};

template <  class t_TyInputNodeEl,
            class t_TyInputLinkEl = t_TyInputNodeEl >
struct _fdin_object
{
	typedef int _TyInitArg;
	typedef off_t _TyStreamPos;
	typedef t_TyInputNodeEl _TyIONodeEl;
	typedef t_TyInputLinkEl _TyIOLinkEl;

	int m_fd; // This object doesn't own the lifetime of the open file.

  t_TyInputNodeEl m_ine;
  t_TyInputLinkEl m_ile;

	_fdin_object(  int _fd,
                t_TyInputNodeEl const & _rine,
                t_TyInputLinkEl const & _rile )
		: m_fd( _fd ),
      m_ine( _rine ),
      m_ile( _rile )
	{
      __THROWPT( e_ttMemory ); // in the cases where where are dynamic members within m_ine or m_ile.
	}
	_fdin_object(  int _fd,
                t_TyInputNodeEl && _rrine,
                t_TyInputLinkEl && _rrile )
		: m_fd( _fd ),
      m_ine( std::move( _rrine ) ),
      m_ile( std::move( _rrile ) )
	{
	}

	_TyStreamPos TellG() const
	{ 
    off_t off = lseek( m_fd, 0, SEEK_CUR );
    __THROWPT( e_ttFileInput | e_ttFatal );
    if ( -1 == off )
      THROWNAMEDEXCEPTIONERRNO( errno, "_fdin_object::TellG(): lseek() failed." );
    return off;
  }
	void SeekG( _TyStreamPos _sp )	
  {
    off_t off = lseek( m_fd, _sp, SEEK_SET );
    __THROWPT( e_ttFileInput | e_ttFatal );
    if ( -1 == off )
      THROWNAMEDEXCEPTIONERRNO( errno, "_fdin_object::SeekG(): lseek() failed." );
	}

	void Read( void * _pv, size_t _st )
	{
    ssize_t sstRead = ::read( m_fd, _pv, _st );
    __THROWPT( e_ttFileInput );
    if ( sstRead != _st )
      THROWNAMEDEXCEPTIONERRNO( errno, "_fdin_object::Read(): read() failed.");
	}

	template < class t_TyEl >
	void ReadNodeEl( t_TyEl & _rel )
	{
		m_ine.Read( m_fd, _rel );
	}
	template < class t_TyEl >
	void ReadLinkEl( t_TyEl & _rel )
	{
    	m_ile.Read( m_fd, _rel );
	}
};

__DGRAPH_END_NAMESPACE

#endif //__GR_FDIO_H
