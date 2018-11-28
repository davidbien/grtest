#ifndef __GR_INC_H
#define __GR_INC_H

// _dgr_inc.h

// Include file for directed graph implementation.

#include <stddef.h>
#include <assert.h>
#include <alloc.h>
#include <stdexcept>

#include "bienutil/bienutil.h"
#include "bienutil/_namdexc.h"
#include "bienutil/_debug.h"
#include "bienutil/_util.h"
#include "bienutil/_dbgthrw.h"

#ifndef __DGRAPH_DEFAULT_ALLOCATOR
#ifndef NDEBUG
#define __DGRAPH_DEFAULT_ALLOCATOR __STD::__allocator< char, __STD::malloc_alloc >
#else !NDEBUG
#define __DGRAPH_DEFAULT_ALLOCATOR __STD::allocator< char >
#endif !NDEBUG
#endif __DGRAPH_DEFAULT_ALLOCATOR

// Choose namespace:
#if !defined( __STL_USE_NAMESPACES ) && !defined( __DGRAPH_USE_NAMESPACE )
#define __DGRAPH_GLOBALNAMESPACE
#endif

#ifdef __DGRAPH_GLOBALNAMESPACE
#define __DGRAPH_BEGIN_NAMESPACE
#define __DGRAPH_END_NAMESPACE
#define __DGRAPH_USING_NAMESPACE
#define __DGRAPH_NAMESPACE
#else __DGRAPH_GLOBALNAMESPACE
#ifndef __DGRAPH_USE_NAMESPACE
#define __DGRAPH_USE_NAMESPACE ns_dgraph
#endif __DGRAPH_USE_NAMESPACE
#define __DGRAPH_BEGIN_NAMESPACE namespace __DGRAPH_USE_NAMESPACE { __BIENUTIL_USING_NAMESPACE
#define __DGRAPH_END_NAMESPACE }
#define __DGRAPH_USING_NAMESPACE using namespace __DGRAPH_USE_NAMESPACE;
#define __DGRAPH_NAMESPACE __DGRAPH_USE_NAMESPACE::
#endif __DGRAPH_GLOBALNAMESPACE

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
  size_t operator()( const t_TyP & __s ) const __STL_NOTHROW
  { 
    return reinterpret_cast< size_t >( __s ); 
  }
};

#ifdef __DGRAPH_COUNT_EL_ALLOC_LIFETIME
extern int gs_iNodesAllocated;
extern int gs_iLinksAllocated;
extern int gs_iNodesConstructed;
extern int gs_iLinksConstructed;
#endif __DGRAPH_COUNT_EL_ALLOC_LIFETIME

__DGRAPH_END_NAMESPACE

// The size for hash tables in various data structures - increase if using larger graphs -
//  decrease if using smaller graphs - if not already then should allow passing to
//  routines that use hashes:
#define _GR_HASH_INITSIZENODES  1000
#define _GR_HASH_INITSIZELINKS  1000

#include "bienutil/_allbase.h"
#include "bienutil/_sdp.h"
#include "bienutil/_sdpn.h"
#include "bienutil/_fcdtor.h"
#include "dgraph/_gr_trt.h"
#include "dgraph/_gr_titr.h"
#include "dgraph/_gr_copy.h"
#include "dgraph/_gr_dtor.h"
#include "dgraph/_gr_rndm.h"
#include "dgraph/_graph.h"

#endif __GR_INC_H
