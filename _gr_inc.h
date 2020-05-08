#ifndef __GR_INC_H
#define __GR_INC_H

// _gr_inc.h

// Include file for directed graph implementation.

//#include <stddef.h>
//#include <assert.h>
//#include <_alloc.h>
//#include <stdexcept>
#include <stdexcept>

#include "bienutil.h"
#include "_namdexc.h"
#include "_debug.h"
#include "_util.h"
#include "_dbgthrw.h"

#ifndef __DGRAPH_DEFAULT_ALLOCATOR
#if defined(_USE_STLPORT) && !defined(NDEBUG)
#define __DGRAPH_DEFAULT_ALLOCATOR __STD::_stlallocator< char, __STD::__debug_alloc< __STD::__malloc_alloc > >
#define __DGRAPH_GET_ALLOCATOR(t) __STD::_stlallocator< t, __STD::__debug_alloc< __STD::__malloc_alloc > >
#else // defined(_USE_STLPORT) && !defined(NDEBUG)
#define __DGRAPH_DEFAULT_ALLOCATOR __STD::allocator< char >
#define __DGRAPH_GET_ALLOCATOR(t) __STD::allocator< t >
#endif // defined(_USE_STLPORT) && !defined(NDEBUG)
#endif //__DGRAPH_DEFAULT_ALLOCATOR

#ifndef __DBGTHROW_DEFAULT_ALLOCATOR
#if defined(_USE_STLPORT) && !defined(NDEBUG)
#define __DBGTHROW_DEFAULT_ALLOCATOR __STD::_stlallocator< char, __STD::__debug_alloc< __STD::__malloc_alloc > >
#define __DBGTHROW_GET_ALLOCATOR(t) __STD::_stlallocator< t, __STD::__debug_alloc< __STD::__malloc_alloc > >
#else // defined(_USE_STLPORT) && defined(NDEBUG)
#define __DBGTHROW_DEFAULT_ALLOCATOR __STD::allocator< char >
#define __DBGTHROW_GET_ALLOCATOR(t) __STD::allocator< t >
#endif //defined(_USE_STLPORT) && defined(NDEBUG)
#endif //!__DBGTHROW_DEFAULT_ALLOCATOR


// Choose namespace:
#define __DGRAPH_USE_NAMESPACE ns_dgraph // always use namespaces.
#if !defined( _STLP_USE_NAMESPACES ) && !defined( __DGRAPH_USE_NAMESPACE )
#define __DGRAPH_GLOBALNAMESPACE
#endif

#ifdef __DGRAPH_GLOBALNAMESPACE
#define __DGRAPH_BEGIN_NAMESPACE
#define __DGRAPH_END_NAMESPACE
#define __DGRAPH_USING_NAMESPACE
#define __DGRAPH_NAMESPACE
#else //__DGRAPH_GLOBALNAMESPACE
#ifndef __DGRAPH_USE_NAMESPACE
#define __DGRAPH_USE_NAMESPACE ns_dgraph
#endif //__DGRAPH_USE_NAMESPACE
#define __DGRAPH_BEGIN_NAMESPACE namespace __DGRAPH_USE_NAMESPACE { __BIENUTIL_USING_NAMESPACE using namespace std;
#define __DGRAPH_END_NAMESPACE }
#define __DGRAPH_USING_NAMESPACE using namespace __DGRAPH_USE_NAMESPACE;
#define __DGRAPH_NAMESPACE __DGRAPH_USE_NAMESPACE::
#endif //__DGRAPH_GLOBALNAMESPACE

__DGRAPH_BEGIN_NAMESPACE

typedef unsigned  _TyGNIndex;

// tag allowing construction of CQ iterators without check the CQ of the passed.
// This allows the graph to transform iterators from const->non-const.
struct __gr_const_cast_iter
{
};

template < class t_TyP >
struct _gr_hash_ptr
{
  size_t operator()( const t_TyP & __s ) const _BIEN_NOTHROW
  { 
    return reinterpret_cast< size_t >( __s ); 
  }
};

#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
extern int gs_iNodesAllocated;
extern int gs_iLinksAllocated;
extern int gs_iNodesConstructed;
extern int gs_iLinksConstructed;
#endif //__DGRAPH_COUNT_EL_ALLOC_LIFETIME

__DGRAPH_END_NAMESPACE

// The size for hash tables in various data structures - increase if using larger graphs -
//  decrease if using smaller graphs - if not already then should allow passing to
//  routines that use hashes:
#define _GR_HASH_INITSIZENODES  1000
#define _GR_HASH_INITSIZELINKS  1000

#include "_allbase.h"
#include "_sdp.h"
#include "_sdpn.h"
#include "_fcdtor.h"
#include "_gr_def.h"
#include "_gr_trt.h"
#include "_gr_titr.h"
#include "_gr_copy.h"
#include "_gr_dtor.h"
#include "_gr_rndm.h"
#include "_graph.h"

#endif //__GR_INC_H
