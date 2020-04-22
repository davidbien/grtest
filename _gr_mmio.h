#ifndef __GR_MMIO_H
#define __GR_MMIO_H

// _gr_mmio.h
// graph I/O through mapped memory.
// dbien: 21APR2020

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "_gr_inc.h"

__DGRAPH_BEGIN_NAMESPACE

// Specialize for mmout - default version just writes raw memory:
template < class t_TyWrite >
__INLINE size_t
_StRawWriteGraphEl( void * _pvWrite, size_t _stLeft, t_TyWrite const & _rEl )
{
	if ( sizeof( _rEl ) )
	{
    if ( sizeof( _rEl ) <= _stLeft )
      memcpy( _pvWrite, &_rEl, sizeof( _rEl ) );
    return sizeof( _rEl );
  }
}

// Specialize for mmin - default version just reads raw memory:
template < class t_TyRead >
__INLINE size_t
_StRawReadGraphEl( const void * _pvRead, size_t _stLeft, t_TyRead & _rEl )
{
	if ( sizeof( _rEl ) )
	{
    if ( _stLeft < sizeof( _rEl ) )
      THROWNAMEDEXCEPTION( "_RawReadGraphEl(pv,st): EOF reading element." );
    memcpy( &_rEl, _pvRead, sizeof( _rEl ) );
    return sizeof( _rEl );
	}
}

struct _mm_RawElIO
{
  template < class t_TyEl >
  size_t StWrite( void * _pvWrite, size_t _stLeft, t_TyEl const & _rel )
  {
    return _StRawWriteGraphEl( _pvWrite, _stLeft, _rel );
  }
  template < class t_TyEl >
  size_t StRead( const void * _pvRead, size_t _stLeft, t_TyEl & _rel )
  {
    return _StRawReadGraphEl( _pvRead, _stLeft, _rel );
  }
};

template <  class t_TyOutputNodeEl,
            class t_TyOutputLinkEl = t_TyOutputNodeEl,
            size_t t_knGrowFileByBytes = 65536 >
struct _mmout_object
{
  typedef int _TyInitArg;
  typedef off_t _TyStreamPos;
  typedef t_TyOutputNodeEl _TyIONodeEl;
  typedef t_TyOutputLinkEl _TyIOLinkEl;
  static const size_t s_knGrowFileByBytes = t_knGrowFileByBytes;

  int m_fd{-1}; // This object doesn't own the lifetime of the open file.
  uint8_t * m_pbyMappedBegin{(uint8_t*)MAP_FAILED};
  uint8_t * m_pbyMappedCur{(uint8_t*)MAP_FAILED};
  uint8_t * m_pbyMappedEnd{(uint8_t*)MAP_FAILED};

  t_TyOutputNodeEl  m_one;
  t_TyOutputLinkEl  m_ole;

  _mmout_object( _mmout_object const & ) = delete;
  _mmout_object() = delete;
	_mmout_object(  int _fd,
                  t_TyOutputNodeEl const & _rone,
                  t_TyOutputLinkEl const & _role )
		: m_fd( _fd ),
      m_one( _rone ),
      m_ole( _role )
	{
    _OpenMap();
	}
	_mmout_object(  int _fd,
                  t_TyOutputNodeEl && _rrone,
                  t_TyOutputLinkEl && _rrole )
		: m_fd( _fd ),
      m_one( std::move( _rrone ) ),
      m_ole( std::move( _rrole ) )
	{
    _OpenMap();
	}
  ~_mmout_object()
  {
    bool fInUnwinding = !!std::uncaught_exceptions();
    // We need to truncate the file to m_pbyMappedCur - m_pbyMappedBegin bytes.
    iCloseRet = ftruncate( m_fd, m_pbyMappedCur - m_pbyMappedBegin );
    if ( ( -1 == iCloseRet ) && !fInUnwinding )
        THROWNAMEDEXCEPTIONERRNO( errno, "_mmout_object::~_mmout_object(): failed to ftruncate m_fd[%d]", m_fd );
    size_t stMapped = m_pbyMappedEnd - m_pbyMappedBegin;
    uint8_t * pbyMapped = m_pbyMappedBegin;
    m_pbyMappedBegin = (uint8_t*)MAP_FAILED;
    int iUnmap = _Unmap( pbyMapped, stMapped ); // We don't care if this fails pretty much.
    assert( !iUnmap );
  }

	_TyStreamPos TellP() const
  {
    return m_pbyMappedCur - m_pbyMappedBegin;
  }
	void SeekP( _TyStreamPos _sp )	
  {
    // We will let the caller set the position anywhere at all.
    m_pbyMappedCur = m_pbyMappedBegin + _sp;
  }

	void Write( const void * _pv, size_t _st )
	{
    if ( _st > ( m_pbyMappedEnd - m_pbyMappedCur ) )
      _GrowMap( _st - ( m_pbyMappedEnd - m_pbyMappedCur ) );
    memcpy( m_pbyMappedCur, _pv, _st );
    m_pbyMappedCur += _st;
	}

	template < class t_TyEl >
	void WriteNodeEl( t_TyEl const & _rel )
	{
    size_t stLeft = ( m_pbyMappedEnd - m_pbyMappedCur );
    size_t stNeed = m_one.StWrite( m_pbyMappedCur, stLeft, _rel );
    if ( stNeed > stLeft )
    {
      _GrowMap( stNeed - stLeft );
      size_t stNeed2 = m_one.StWrite( m_pbyMappedCur, ( m_pbyMappedEnd - m_pbyMappedCur ), _rel );
      assert( stNeed == stNeed2 );
    }
	}
	template < class t_TyEl >
	void WriteLinkEl( t_TyEl const & _rel )
	{
    size_t stLeft = ( m_pbyMappedEnd - m_pbyMappedCur );
    size_t stNeed = m_ole.StWrite( m_pbyMappedCur, stLeft, _rel );
    if ( stNeed > stLeft )
    {
      _GrowMap( stNeed - stLeft );
      size_t stNeed2 = m_ole.StWrite( m_pbyMappedCur, ( m_pbyMappedEnd - m_pbyMappedCur ), _rel );
      assert( stNeed == stNeed2 );
    }
	}
protected:
  void _OpenMap()
  {
    _tyFilePos posEnd = ::lseek( m_fd, s_knGrowFileByBytes-1, SEEK_SET );
    if ( -1 == posEnd )
      THROWNAMEDEXCEPTIONERRNO( errno, "_mmout_object::_OpenMap(): Attempting to lseek() failed m_fd[%d].", m_fd );
    ssize_t stRet = ::write( m_fd, "Z", 1 ); // write a single byte to grow the file to s_knGrowFileByBytes.
    if ( -1 == stRet )
      THROWNAMEDEXCEPTIONERRNO( errno, "_mmout_object::_OpenMap(): Attempting to write() failed for m_fd[%d]", m_fd );
    // No need to reset the file pointer to the beginning - and in fact we like it at the end in case someone were to actually try to write to it.
    m_pbyMappedBegin = (uint8_t*)::mmap( 0, s_knGrowFileByBytes, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0 );
    if ( m_pbyMappedBegin == MAP_FAILED )
      THROWNAMEDEXCEPTIONERRNO( errno, "_mmout_object::_OpenMap(): mmap() failed for m_fd[%d]", m_fd );
    m_pbyMappedCur = m_pbyMappedBegin;
    m_pbyMappedEnd = m_pbyMappedCur + s_knGrowFileByBytes;
  }
  void _GrowMap( size_t _stByAtLeast )
  {
    size_t stGrowBy = ( ( ( _stByAtLeast - 1 ) / s_knGrowFileByBytes ) + 1 ) * s_knGrowFileByBytes;
    size_t stMapped = m_pbyMappedEnd - m_pbyMappedBegin;
    uint8_t * pbyOldMapping = m_pbyMappedBegin;
    m_pbyMappedBegin = (uint8_t*)MAP_FAILED;
    int iRet = ::munmap( pbyOldMapping, stMapped );
    assert( !iRet ); // not much to do about this.
    stMapped += stGrowBy;
    iRet = ::lseek( m_fd, stMapped - 1, SEEK_SET );
    if ( -1 == iRet )
      THROWNAMEDEXCEPTIONERRNO( errno, "_mmout_object::_GrowMap(): lseek() failed for m_fd[%d].", m_fd );
    iRet = ::write( m_fd, "Z", 1 ); // just write a single byte to grow the file.
    if ( -1 == iRet )
      THROWNAMEDEXCEPTIONERRNO( errno, "_mmout_object::_GrowMap(): write() failed for m_fd[%d].", m_fd );
    m_pbyMappedBegin = (uint8_t*)::mmap( 0, stMapped, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0 );
    if ( m_pbyMappedBegin == MAP_FAILED )
      THROWNAMEDEXCEPTIONERRNO( errno, "_mmout_object::_GrowMap(): mmap() failed for m_fd[%d].", m_fd );
    m_pbyMappedEnd = m_pbyMappedBegin + stMapped;
    m_pbyMappedCur = m_pbyMappedBegin + ( m_pbyMappedCur - pbyOldMapping );
  }
};

template <  class t_TyInputNodeEl,
            class t_TyInputLinkEl = t_TyInputNodeEl >
struct _mmin_object
{
	typedef int _TyInitArg;
	typedef off_t _TyStreamPos;
	typedef t_TyInputNodeEl _TyIONodeEl;
	typedef t_TyInputLinkEl _TyIOLinkEl;

	int m_fd{-1}; // This object doesn't own the lifetime of the open file.
  uint8_t * m_pbyMappedBegin{(uint8_t*)MAP_FAILED};
  uint8_t * m_pbyMappedCur{(uint8_t*)MAP_FAILED};
  uint8_t * m_pbyMappedEnd{(uint8_t*)MAP_FAILED};

  t_TyInputNodeEl m_ine;
  t_TyInputLinkEl m_ile;

  _mmin_object( _mmin_object const & ) = delete;
  _mmin_object() = delete;
	_mmin_object(  int _fd,
                t_TyInputNodeEl const & _rine,
                t_TyInputLinkEl const & _rile )
		: m_fd( _fd ),
      m_ine( _rine ),
      m_ile( _rile )
	{
    _OpenMap();
	}
	_mmin_object(  int _fd,
                t_TyInputNodeEl && _rrine,
                t_TyInputLinkEl && _rrile )
		: m_fd( _fd ),
      m_ine( std::move( _rrine ) ),
      m_ile( std::move( _rrile ) )
	{
    _OpenMap();
	}

	_TyStreamPos TellG() const
	{
    return m_pbyMappedCur - m_pbyMappedBegin;
  }
	void SeekG( _TyStreamPos _sp )	
  {
    m_pbyMappedCur = m_pbyMappedBegin + _sp;
	}

	void Read( void * _pv, size_t _st )
	{
    if ( _st > ( m_pbyMappedEnd - m_pbyMappedCur ) )
      THROWNAMEDEXCEPTIONERRNO( errno, "_mmin_object::Read(): EOF.")
    memcpy( _pv, m_pbyMappedCur, _st );
    m_pbyMappedCur += _st;
	}

	template < class t_TyEl >
	void ReadNodeEl( t_TyEl & _rel )
	{
		size_t stRead = m_ine.StRead( m_fd, _rel ); // throws on EOF.
    m_pbyMappedCur += stRead;
	}
	template < class t_TyEl >
	void ReadLinkEl( t_TyEl & _rel )
	{
		size_t stRead = m_ile.StRead( m_fd, _rel ); // throws on EOF.
    m_pbyMappedCur += stRead;
	}
protected:
  void _OpenMap()
  {
    // Now get the size of the file and then map it.
    off_t posEnd = lseek( m_fd, 0, SEEK_END );
    if ( -1 == posEnd )
        THROWNAMEDEXCEPTIONERRNO( errno, "_mmin_object::Open(): Attempting to seek to EOF failed for m_fd[%d]", m_fd );
    // No need to reset the file pointer to the beginning - and in fact we like it at the end in case someone were to actually try to read from it.
    m_pbyMappedBegin = (uint8_t*)mmap( 0, posEnd, PROT_READ, MAP_SHARED, m_fd, 0 );
    if ( m_pbyMappedBegin == (uint8_t*)MAP_FAILED )
        THROWNAMEDEXCEPTIONERRNO( errno, "_mmin_object::Open(): mmap() failed for m_fd[%d]", m_fd );
    m_pbyMappedCur = m_pbyMappedBegin;
    m_pbyMappedEnd = m_pbyMappedCur + posEnd;
  }
};

__DGRAPH_END_NAMESPACE

#endif //__GR_MMIO_H
