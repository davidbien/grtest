#ifndef __GR_TST0_H
#define __GR_TST0_H

// _gr_tst0.h

#include "_gr_inc.h"

// This function creates a graph with two adjacent unfinished nodes 
//	( special case for various algorithms ).

// The simplest such config is:
// R->C
// P1<-C
// P1<-C

template < class t_TyGraph >
void
CreateTestGraph0( t_TyGraph & g )
{
	g.destroy();

	typedef t_TyGraph	_TyGraph;

	// Root is zero:
	typename _TyGraph::_TyGraphNode * pgn0 = g.create_node1( 0 );
	g.set_root_node( pgn0 );

	typename _TyGraph::_TyGraphNode * pgn1 = g.create_node1( 1 );
	typename _TyGraph::_TyGraphLink * pgl1 = g.create_link1( 1 );
	
	g.get_root()->AddChild( *pgn1, *pgl1, 
													*g.get_root()->PPGLBChildHead(), 
													*pgn1->PPGLBParentHead() );
	
	typename _TyGraph::_TyGraphNode * pgn2 = g.create_node1( 2 );
	typename _TyGraph::_TyGraphLink * pgl2 = g.create_link1( 2 );

	pgn1->AddParent(	*pgn2, *pgl2, 
										*pgn1->PPGLBParentHead(), 
										*pgn2->PPGLBChildHead() );

	typename _TyGraph::_TyGraphLink * pgl3 = g.create_link1( 3 );
	pgn1->AddParent(	*pgn2, *pgl3, 
										*pgn1->PPGLBParentHead(), 
										*pgn2->PPGLBChildHead() );

}

#endif __GR_TST0_H
