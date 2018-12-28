

// _gr_test.cpp

// Graph test stuff.

#if 0 //def _MSC_VER
#include <stdlib.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif //_MSC_VER

#include <memory>

typedef std::_stlallocator< char, std::__malloc_alloc >	_TyMallocAllocator;

// A NOTE ON MEMORY LEAKS:
// I attempted to make everything use the malloc allocator, which coupled
//  with MSVCRTD debugging heap, gives you nice leak detection.
// But, the SGI STL implementation class __Named_exception uses allocator< char >.
// I have provided my own implementation that is templatized by allocator type,
//  but this did not propagate to all instances of __Named_exception.

#include "_gr_inc.h"

#include <time.h>
#include <fstream>
#include <iostream>
#include <stl/_alloc.c>

__BIENUTIL_USING_NAMESPACE
__DGRAPH_USING_NAMESPACE
#include "_gr_tst0.h"

#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
int gs_iNodesAllocated = 0;
int gs_iLinksAllocated = 0;
int gs_iNodesConstructed = 0;
int gs_iLinksConstructed = 0;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME

#ifdef __NAMDDEXC_STDBASE
#pragma push_macro("std")
#undef std
#endif //__NAMDDEXC_STDBASE
typedef std::_t__Named_exception<> vtyNamedException;
#ifdef __NAMDDEXC_STDBASE
#pragma pop_macro("std")
#endif //__NAMDDEXC_STDBASE

void
print_time( const char * _cpMesg, time_t * _ptStart )
{
	time_t	tCur;
	time( &tCur );
	cout << "[" << ptrdiff_t( tCur - *_ptStart ) << "]:" << _cpMesg;
	cout.flush();
}

void
usage( char * _cpApp )
{
	cerr << "main(): usage\n" << _cpApp << " <num nodes> <num extra links> <rand seed>\n";
}

template < class t_TyGraphLeft, class t_TyGraphRight >
bool
test_compare( t_TyGraphLeft const & _rgLeft, 
              t_TyGraphRight const & _rgRight, 
              time_t * _ptStart )
{
	// Turn off throw testing while comparing:
#ifndef __NDEBUG_THROW
	bool	fWasOn = _throw_object_base::ms_tsb.m_fOn;
	_throw_object_base::ms_tsb.set_on( false );
#endif //!__NDEBUG_THROW

	pair< typename t_TyGraphLeft::const_iterator_pos, 
        typename t_TyGraphRight::const_iterator_pos >
		pitResult = _rgLeft.compare_values( _rgRight );
	if ( pitResult.first.FAtEnd() && pitResult.second.FAtEnd() )
	{
		print_time( "test_compare(): Compare succeeded.\n", _ptStart );
#ifndef __NDEBUG_THROW
		_throw_object_base::ms_tsb.set_on( fWasOn );
#endif //!__NDEBUG_THROW
		return true;
	}
	else
	{
		print_time( "test_compare(): Compare FAILED.\n", _ptStart );
		fstream fsDumpRight( "right.txt", ios::out );
		_rgRight.dump( fsDumpRight );
		fstream fsDumpLeft( "left.txt", ios::out );
		_rgLeft.dump( fsDumpLeft );
#ifndef __NDEBUG_THROW
		_throw_object_base::ms_tsb.set_on( fWasOn );
#endif //!__NDEBUG_THROW
		return false;
	}
}

template < class t_TyGraphDst, class t_TyGraphSrc >
bool
test_copy(  t_TyGraphDst & _rgDst, 
            t_TyGraphSrc const & _rgSrc, 
            time_t * _ptStart )
{
	try
	{
#ifndef __NDEBUG_THROW
	bool	fWasOn = _throw_object_base::ms_tsb.m_fOn;
	_throw_object_base::ms_tsb.set_on( false );
#endif //!__NDEBUG_THROW

		_rgDst.replace_copy( _rgSrc );
		print_time( "test_copy(): After copy.\n", _ptStart );

#ifndef __NDEBUG_THROW
	_throw_object_base::ms_tsb.set_on( fWasOn );
#endif //!__NDEBUG_THROW

		bool	fComp1 = test_compare( _rgDst, _rgSrc, _ptStart );
		bool	fComp2 = test_compare( _rgSrc, _rgDst, _ptStart );
		return fComp1 && fComp2;
	}
	catch( exception & rexc )
	{
		const char * cpWhat = rexc.what();
#ifndef __NDEBUG_THROW
		_throw_object_base::ms_tsb.handle_throw();
#endif //!__NDEBUG_THROW
	}

	return false;
}

template < class t_TyGraph >
bool
try_dump( t_TyGraph const & _rg, ostream & _ros )
{
  try
  {
    try
    {
      typename t_TyGraph::_TyDumpIteratorConst	
          dit(	_ros, _rg.get_root(), 0, false, true, _rg.get_base_path_allocator() );
      for ( ; !dit.FAtEnd(); )
      {
        try
        {
          ++dit;
        }
        catch( exception & rexc )
        {
          const char * cpWhat = rexc.what();
  #ifndef __NDEBUG_THROW
          _throw_object_base::ms_tsb.handle_throw();
  #endif //!__NDEBUG_THROW
        }
      }

      return true;
    }
    catch( exception & rexc )
    {
      const char * cpWhat = rexc.what();
  #ifndef __NDEBUG_THROW
      _throw_object_base::ms_tsb.handle_throw();
  #endif //!__NDEBUG_THROW
    }
  }
  catch( ... )
  {
    cerr << "try_dump(): Caught exception in ... - unexpected.\n";
  }

	return false;
}

template < class t_TyGraph >
void
test_dump( t_TyGraph const & _rg, const char * _cpFileName, time_t * _ptStart )
{
	fstream fsDumpOut( _cpFileName, ios::out );
	while( !try_dump( _rg, fsDumpOut ) )
		;
	print_time( "test_dump(): Dump succeeded.\n", _ptStart );

#ifndef __NDEBUG_THROW
	unsigned uHit;
	size_t uPossible;
	_throw_object_base::ms_tsb.get_hit_stats( uHit, uPossible );
	cout << "Hit [" << uHit << "] out of [" << uPossible << "].\n";
	_throw_object_base::ms_tsb.report_unhit( cout );
	_throw_object_base::ms_tsb.clear_hit_map();
#endif //!__NDEBUG_THROW
}

template < class t_TyGraph >
bool
try_save( t_TyGraph const & _rg, ostream & _ros )
{
	bool	fSuccess = false;
	try
	{
		typename t_TyGraph::_TyBinaryOstreamIterConst boi( _ros, _rg.begin() );
		__DEBUG_STMT( int _i = 0 )
		while ( !boi.FAtEnd() )
		{
			try
			{
				++boi;
				__DEBUG_STMT( ++_i )
			}
			catch( exception & rexc )
			{
				const char * cpWhat = rexc.what();
		#ifndef __NDEBUG_THROW
				_throw_object_base::ms_tsb.handle_throw();
		#endif //!__NDEBUG_THROW
			}
		}

		fSuccess = true;
	}
	catch( exception & rexc )
	{
		const char * cpWhat = rexc.what();
#ifndef __NDEBUG_THROW
		_throw_object_base::ms_tsb.handle_throw();
#endif //!__NDEBUG_THROW
	}

	return fSuccess;
}

template < class t_TyGraph >
bool
try_load( t_TyGraph & _rg, istream & _ris )
{
	bool	fSuccess = false;
	try
	{
		_rg.destroy();

		typename t_TyGraph::_TyBinaryIstreamIterNonConst 
      bii( _rg, _ris, _rg.get_base_path_allocator() );

		__DEBUG_STMT( int _i = 0 )
		do
		{
			try
			{
				++bii;
				__DEBUG_STMT( ++_i )
			}
			catch( exception & rexc )
			{
				const char * cpWhat = rexc.what();
		#ifndef __NDEBUG_THROW
				_throw_object_base::ms_tsb.handle_throw();
		#endif //!__NDEBUG_THROW
			}
		}
		while( !bii.FAtEnd() );

		_rg.set_root_node( bii.PGNTransferNewRoot() );

		fSuccess = true;
	}
	catch( exception & rexc )
	{
		const char * cpWhat = rexc.what();
#ifndef __NDEBUG_THROW
		_throw_object_base::ms_tsb.handle_throw();
#endif //!__NDEBUG_THROW
	}

	return fSuccess;
}

template < class t_TyGraphSave, class t_TyGraphLoad >
bool
test_saveload(	t_TyGraphSave const & _rgSave, t_TyGraphLoad & _rgLoad, 
								const char * _cpFileName, time_t * _ptStart )
{
	fstream fsOut( _cpFileName, ios::in | ios::out | ios::trunc | ios::binary );
	fsOut.seekp( 0 );

	if ( try_save( _rgSave, fsOut ) )
	{
		print_time( "test_saveload(): Save succeeded.\n", _ptStart );

#ifndef __NDEBUG_THROW
		unsigned uHit;
		size_t uPossible;
		_throw_object_base::ms_tsb.get_hit_stats( uHit, uPossible );
		cout << "Hit [" << uHit << "] out of [" << uPossible << "].\n";
		_throw_object_base::ms_tsb.report_unhit( cout );
		_throw_object_base::ms_tsb.clear_hit_map();
		
		_throw_object_base::ms_tsb.reset_hit_once();
#endif //!__NDEBUG_THROW

		fsOut.seekg( 0 );
		while ( !try_load( _rgLoad, fsOut ) )
		{
			fsOut.seekg( 0 );
		}

		print_time( "test_saveload(): After load.\n", _ptStart );

#ifndef __NDEBUG_THROW
		_throw_object_base::ms_tsb.get_hit_stats( uHit, uPossible );
		cout << "Hit [" << uHit << "] out of [" << uPossible << "].\n";
		_throw_object_base::ms_tsb.report_unhit( cout );
		_throw_object_base::ms_tsb.clear_hit_map();
#endif //!__NDEBUG_THROW

		return test_compare( _rgSave, _rgLoad, _ptStart );
	}
	else
	{
		return false;
	}
}


template <  class t_TyGraph, 
            class t_TyNodeIterator,
            class t_TyLinkIterator,
            class t_TyPathIterator >
struct test_iterators
{
  static void
  test_path( t_TyGraph & _rg )
  {
    t_TyNodeIterator nit( _rg.get_root() );
    t_TyNodeIterator nit2( nit );

    t_TyPathIterator pit( nit, _rg.get_base_path_allocator() );

#if 1
    pit.FirstChild();
    pit.FirstChild();

    t_TyPathIterator pitCopy( pit );

    pitCopy.FirstParent();
    pitCopy.FirstParent();

    pit = pitCopy;
#endif //0
  }
};

int
main( int argc, char ** argv )
{
#ifdef _MSC_VER
	_set_error_mode( _OUT_TO_MSGBOX );	// Allow debugging after assert.
#ifdef _DEBUG
	_CrtSetDbgFlag( _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ) | _CRTDBG_LEAK_CHECK_DF );
#endif
#endif //_MSC_VER

	// Extract the random graph parameters:
	if ( argc < 4 )
	{
		usage( argv[0] );
		return -1;
	}

	int	iNumNodes = atoi( argv[1] );
	if ( iNumNodes <= 0 )
	{
		usage( argv[0] );
		cerr << "Need positive number of nodes.\n";
	}
	int iNumExtraLinks = atoi( argv[2] );
	if ( iNumExtraLinks < 0 )
	{
		usage( argv[0] );
		cerr << "Need non-negative number of extra links.\n";
	}
	int iRandSeed = atoi( argv[3] );

	int iThrowOneOnly = -1;
	if ( argc > 4 )
	{
		iThrowOneOnly = atoi( argv[4] );
	}

	time_t	tStart;
	time( &tStart );

	try
		{
      typedef _TyMallocAllocator _TyAllocator;
			typedef dgraph< int, int, false, _TyAllocator > _TyGraph;
			_TyGraph g;
			g.replace_random( iNumNodes, iNumExtraLinks, iRandSeed );
			// 1000 2000 1241
			// g.replace_random( 15000, 15000, 823 );
	    // g.replace_random( 160000, 160000, 823 );
			// g.replace_random( 10, 10, 1000 );
			// g.replace_random( 4, 4, 1000 );
			// g.replace_random( 5, 5, 10000 );
			// CreateTestGraph0( g );

			print_time( "main(): Random graph created.\n", &tStart );

#ifndef __NDEBUG_THROW
			// Set the seed at this point:
			_throw_object_base::ms_tsb.set_seed( iRandSeed );
			_throw_object_base::ms_tsb.set_on( true );
			if ( iThrowOneOnly > 0 )
			{
				_throw_object_base::ms_tsb.throw_one_only( iThrowOneOnly );
			}
			
			// Set up special throwing stuff:
			const int kiNSpecial = 10;
			_throw_object_with_throw_rate		rgtobtrSpecial[kiNSpecial];

			rgtobtrSpecial[9].m_cpFileName = "C:\\dv\\dslib\\_gr_stin.h";
			rgtobtrSpecial[9].m_ulLineNumber = 105;
			rgtobtrSpecial[9].m_iThrowRate = 4;
			rgtobtrSpecial[9].m_fHitOnce = false;

			rgtobtrSpecial[8].m_cpFileName = "C:\\dv\\dslib\\_gr_dump.h";
			rgtobtrSpecial[8].m_ulLineNumber = 88;
			rgtobtrSpecial[8].m_iThrowRate = 2;
			rgtobtrSpecial[8].m_fHitOnce = false;

			rgtobtrSpecial[7].m_cpFileName = "C:\\dv\\dslib\\_gr_dump.h";
			rgtobtrSpecial[7].m_ulLineNumber = 232;
			rgtobtrSpecial[7].m_iThrowRate = 2;
			rgtobtrSpecial[7].m_fHitOnce = true;

			rgtobtrSpecial[6].m_cpFileName = "C:\\dv\\dslib\\_gr_stin.h";
			rgtobtrSpecial[6].m_ulLineNumber = 173;
			rgtobtrSpecial[6].m_iThrowRate = 4;
			rgtobtrSpecial[6].m_fHitOnce = false;

			rgtobtrSpecial[5].m_cpFileName = "C:\\dv\\dslib\\_gr_outp.h";
			rgtobtrSpecial[5].m_ulLineNumber = 297;
			rgtobtrSpecial[5].m_iThrowRate = 2;
			rgtobtrSpecial[5].m_fHitOnce = true;

			rgtobtrSpecial[4].m_cpFileName = "C:\\dv\\dslib\\_gr_outp.h";
			rgtobtrSpecial[4].m_ulLineNumber = 152;
			rgtobtrSpecial[4].m_iThrowRate = 4;
			rgtobtrSpecial[4].m_fHitOnce = false;

			rgtobtrSpecial[3].m_cpFileName = "C:\\dv\\dslib\\../memlib/_allbase.h";
			rgtobtrSpecial[3].m_ulLineNumber = 112;
			rgtobtrSpecial[3].m_iThrowRate = 2;
			rgtobtrSpecial[3].m_fHitOnce = false;

			rgtobtrSpecial[2].m_cpFileName = "C:\\dv\\dslib\\_gr_gitr.h";
			rgtobtrSpecial[2].m_ulLineNumber = 236;
			rgtobtrSpecial[2].m_iThrowRate = 2;
			rgtobtrSpecial[2].m_fHitOnce = true;

			rgtobtrSpecial[1].m_cpFileName = "C:\\dv\\dslib\\_gr_gitr.h";
			rgtobtrSpecial[1].m_ulLineNumber = 295;
			rgtobtrSpecial[1].m_iThrowRate = 2;
			rgtobtrSpecial[1].m_fHitOnce = true;

			rgtobtrSpecial[0].m_cpFileName = "C:\\dv\\dslib\\_gr_inpt.h";
			rgtobtrSpecial[0].m_ulLineNumber = 261;
			rgtobtrSpecial[0].m_iThrowRate = 2;
			rgtobtrSpecial[0].m_fHitOnce = true;

			_throw_object_base::ms_tsb.m_ptobtrStart = rgtobtrSpecial;
			_throw_object_base::ms_tsb.m_ptobtrEnd = rgtobtrSpecial + kiNSpecial;

			sort( rgtobtrSpecial, rgtobtrSpecial + kiNSpecial );

#if 0
// 	try
// 	{
// 		TestThrow();
// 	}
// 	catch( ... )
// 	{
// 		cout << "Caught in main().\n";
// 	}
#endif //0
	
#endif //!__NDEBUG_THROW

#define __TEST_COPY

#define __TEST_NONOLE
#ifdef __TEST_NONOLE

#ifdef __TEST_COPY
			_TyGraph gCopy;
			while ( !test_copy( gCopy, g, &tStart ) )
				;
#else //__TEST_COPY
			_TyGraph &	gCopy = g;
#endif //__TEST_COPY

			test_dump( gCopy, "graph.txt", &tStart );
			typedef dgraph< int, int, true, _TyAllocator > _TyGraphSafe;
			_TyGraphSafe gsCopyLoaded;
#ifndef __NDEBUG_THROW
			_throw_object_base::ms_tsb.reset_hit_once();
#endif //!__NDEBUG_THROW
			while ( !test_saveload( gCopy, gsCopyLoaded, "graph.bin", &tStart ) )
				;

			// Copy the graph to a graph of doubles:
			typedef dgraph< double, double, true, _TyAllocator > _TyGraphDoubleSafe;
#ifdef __TEST_COPY
			_TyGraphDoubleSafe gdCopy;
			while ( !test_copy( gdCopy, g, &tStart ) )
				;
			test_dump( gdCopy, "dgraph.txt", &tStart );
			typedef dgraph< double, double, false, _TyAllocator > _TyGraphDouble;
			_TyGraphDouble gdCopyLoaded;
			while( !test_saveload( gdCopy, gdCopyLoaded, "dgraph.bin", &tStart ) )
				;
#endif //__TEST_COPY

#endif //__TEST_NONOLE

#ifdef __GR_DEFINEOLEIO

#ifndef __NDEBUG_THROW
			_throw_object_base::ms_tsb.set_on( false );
#endif //!__NDEBUG_THROW

			IStream *	pis;
			__ThrowOLEFAIL( CreateStreamOnHGlobal( NULL, TRUE, &pis ) );
			g.save( pis );
			print_time( "main(): After OLE save.\n", &tStart );
			LARGE_INTEGER liSeek0;
			liSeek0.QuadPart = 0ll;
			__ThrowOLEFAIL( pis->Seek( liSeek0, STREAM_SEEK_SET, 0 ) );
			_TyGraph	gLoadOLE;
			gLoadOLE.replace_load( pis );
			print_time( "main(): After OLE load.\n", &tStart );
			test_compare( g, gLoadOLE, &tStart );
			pis->Release();
#endif //__GR_DEFINEOLEIO

#if 1
#if 0
    test_iterators< 
      _TyGraph, 
      _TyGraph::_TyNodeIterNonConst,
      _TyGraph::_TyLinkIdentIterConst,
      _TyGraph::_TyPathIterConst >::test_path( g );
#endif //0

#if 0
    test_iterators< 
      _TyGraphSafe, 
      _TyGraphSafe::_TyNodeIterNonConst,
      _TyGraphSafe::_TyLinkIdentIterNonConstSafe,
      _TyGraphSafe::_TyPathIterConstSafe >::test_path( gsCopyLoaded );
#endif //0
#endif //1
		}
	catch( exception & rexc )
	{
		const char * cpWhat = rexc.what();
		print_time( "main(): Within catch.\n", &tStart );
	}
			
	print_time( "main(): At exit.\n", &tStart );

#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
	cout << "Nodes allocated [" << gs_iNodesAllocated << "].\n";
	cout << "Links allocated [" << gs_iLinksAllocated << "].\n";
	cout << "Nodes constructed [" << gs_iNodesConstructed << "].\n";
	cout << "Links constructed [" << gs_iLinksConstructed << "].\n";
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME

#ifndef __NDEBUG_THROW
	cout << "Num throws [" << _throw_object_base::ms_tsb.m_uNumThrows << "].\n";
	unsigned uHit;
	size_t uPossible;
	_throw_object_base::ms_tsb.get_hit_stats( uHit, uPossible );
	cout << "Hit [" << uHit << "] out of [" << uPossible << "].\n";
	_throw_object_base::ms_tsb.report_unhit( cout );
#endif //!__NDEBUG_THROW

	return 0;
}
	

#if 0
// void 
// TestSubThrow()
// {
// 	try
// 	{
//     cout << "testexcept: _STLP_VENDOR_EXCEPT_STD:" ppmacroxstr( _STLP_VENDOR_EXCEPT_STD ) "\n";
//     cout << "testexcept: _STLP_VENDOR_STD:" ppmacroxstr( _STLP_VENDOR_STD ) "\n";
// 		cout << "testexcept: Throwing in TestSubThrow().\n";
//     //throw std::exception();
//     throw vtyNamedException();
// 		//__THROWPTALWAYS( e_ttMemory )
// 	}
// 	catch( ... )
// 	{
// 		cout << "testexcept: Caught and rethrowing in TestSubThrow().\n";
// 		throw;
// 	}
// }
// 
// void
// TestThrow()
// {
//   try
//   {
//     try
//     {
//       try
//       {
//         TestSubThrow();
//       }
//       catch( _debug_memory_except & _rexc )
//       {
//         const char * cpWhat = _rexc.what();
//         cout << "testexcept: Caught(1) and rethrowing in TestThrow() [" << cpWhat << "].\n";
//         throw;
//       }
//     }
//     catch( vtyNamedException & _rexc )
//     {
//       const char * cpWhat = _rexc.what();
//       cout << "testexcept: Caught(2) and rethrowing in TestThrow() [" << cpWhat << "].\n";
//       throw;
//     }
//   }
// #pragma push_macro("std")
// #undef std
//   catch( std::exception & _rexc )
// #pragma pop_macro("std")
//   {
//     const char * cpWhat = _rexc.what();
//     cout << "testexcept: Caught(4) and rethrowing in TestThrow() [" << cpWhat << "].\n";
//     throw;
//   }
//   catch( stlp_std::exception & _rexc )
//   {
//     const char * cpWhat = _rexc.what();
//     cout << "testexcept: Caught(5) and rethrowing in TestThrow() [" << cpWhat << "].\n";
//     throw;
//   }
// }
#endif //0

