#ifndef __GR_STIO_H
#define __GR_STIO_H

// _gr_stio.h

// graph standard I/O.

// abstract the output stream - allows us to have different types:

#include <istream>
#include <ostream>

__DGRAPH_BEGIN_NAMESPACE

struct _iostream_DumpElIO
{
  template < class t_TyEl >
  void Write( ostream & _ros, t_TyEl const & _rel )
  {
    _ros << "	Value: [" << _rel << "].\n";
  }
  template < class t_TyEl >
  void Read( istream & _ris, t_TyEl & _rel )
  {
    // This method should never be instantiated:
    ___semantic_error_object  error;
  }
};

struct _iostream_RawElIO
{
  template < class t_TyEl >
  void Write( ostream & _ros, t_TyEl const & _rel )
  {
    _RawWriteGraphEl( _ros, _rel );
  }
  template < class t_TyEl >
  void Read( istream & _ris, t_TyEl & _rel )
  {
    _RawReadGraphEl( _ris, _rel );
  }
};

template <  class t_TyOutputNodeEl,
            class t_TyOutputLinkEl = t_TyOutputNodeEl >
struct _ostream_object
{
	typedef ostream &         _TyInitArg;
	typedef streampos         _TyStreamPos;
  typedef t_TyOutputNodeEl  _TyIONodeEl;
  typedef t_TyOutputLinkEl  _TyIOLinkEl;

	ostream &	m_ros;

  t_TyOutputNodeEl  m_one;
  t_TyOutputLinkEl  m_ole;

	_ostream_object(  ostream & _ros,
                    t_TyOutputNodeEl const & _rone,
                    t_TyOutputLinkEl const & _role )
		: m_ros( _ros ),
      m_one( _rone ),
      m_ole( _role )
	{
	}

	_TyStreamPos	TellP() const							{ return m_ros.tellp(); }
	void					SeekP( _TyStreamPos _sp )	{ (void)m_ros.seekp( _sp ); }

	void					Write( const void * _pv, size_t _st )
	{
		m_ros.write( (const char*)_pv, _st );
	}

	template < class t_TyEl >
	void					WriteNodeEl( t_TyEl const & _rel )
	{
    m_one.Write( m_ros, _rel );
	}
	template < class t_TyEl >
	void					WriteLinkEl( t_TyEl const & _rel )
	{
    m_ole.Write( m_ros, _rel );
	}

	// Wrap << for dump:
	template < class t_TyOut >
	_ostream_object &	operator << ( t_TyOut const & _r )
	{
		m_ros << _r;
		return *this;
	}
};

// Specialize for ostream - default version just writes raw memory:
template < class t_TyWrite >
__INLINE void
_RawWriteGraphEl( ostream & _ros, t_TyWrite const & _rEl )
{	
	if ( sizeof( _rEl ) )
	{
		_ros.write(	reinterpret_cast< const char* >( &_rEl ), 
								sizeof( _rEl ) );
	}
}

template <  class t_TyInputNodeEl,
            class t_TyInputLinkEl = t_TyInputNodeEl >
struct _istream_object
{
	typedef istream &       _TyInitArg;
	typedef streampos       _TyStreamPos;
  typedef t_TyInputNodeEl _TyIONodeEl;
  typedef t_TyInputLinkEl _TyIOLinkEl;

	istream &	m_ris;

  t_TyInputNodeEl m_ine;
  t_TyInputLinkEl m_ile;

	_istream_object(  istream & _ris,
                    t_TyInputNodeEl const & _rine,
                    t_TyInputLinkEl const & _rile )
		: m_ris( _ris ),
      m_ine( _rine ),
      m_ile( _rile )
	{
	}

	_TyStreamPos	TellG() const							{ return m_ris.tellg(); }
	void					SeekG( _TyStreamPos _sp )	{ (void)m_ris.seekg( _sp ); }

	void					Read( void * _pv, size_t _st )
	{
		m_ris.read( (char*)_pv, _st );
	}

	template < class t_TyEl >
	void					ReadNodeEl( t_TyEl & _rel )
	{
    m_ine.Read( m_ris, _rel );
	}
	template < class t_TyEl >
	void					ReadLinkEl( t_TyEl & _rel )
	{
    m_ile.Read( m_ris, _rel );
	}
};

// Specialize for istream - default version just reads raw memory:
template < class t_TyRead >
inline void
_RawReadGraphEl( istream & _ris, t_TyRead & _rEl )
{	
	if ( sizeof( _rEl ) )
	{
		_ris.read(	reinterpret_cast< char* >( &_rEl ), 
								sizeof( _rEl ) );
	}
}

__DGRAPH_END_NAMESPACE

#endif __GR_STIO_H
