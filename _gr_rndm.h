#ifndef __GR_RNDM_H
#define __GR_RNDM_H

// _gr_rndm.h

// Generate random graphs.

__DGRAPH_BEGIN_NAMESPACE

template < class t_TyGraph, class t_TyAllocator >
struct _random_graph_generator :
  public _alloc_base< typename t_TyGraph::_TyGraphNode *, t_TyAllocator >
{
private:
  typedef _alloc_base< typename t_TyGraph::_TyGraphNode *, t_TyAllocator > _TyAllocBase;
public:
  typedef typename t_TyGraph::_TyGraphNode _TyGraphNode;
  typedef typename t_TyGraph::_TyGraphLink _TyGraphLink;
  _TyGraphNode ** m_rgpgn;

  t_TyGraph &     m_rg;
  size_t          m_stNodes;
  size_t          m_stExtraLinks;
  unsigned int    m_uRandSeed;

  _TyGraphNode *  m_pgnNewRoot;

  _random_graph_generator(  t_TyGraph & _rg,  
                            size_t _stNodes,
                            size_t _stExtraLinks,
                            unsigned int _uRandSeed,
                            t_TyAllocator const & _rAlloc )
    : _TyAllocBase( _rAlloc ),
      m_rg( _rg ),
      m_stNodes( _stNodes ),
      m_stExtraLinks( _stExtraLinks ),
      m_uRandSeed( _uRandSeed ),
      m_rgpgn( 0 ),
      m_pgnNewRoot( 0 )
    {
      allocate_n( m_rgpgn, _stNodes );
      srand( _uRandSeed );
      
      _create_nodes();
      
      _generate();
    }

  ~_random_graph_generator()
  {
    deallocate_n( m_rgpgn, m_stNodes );
    if ( m_pgnNewRoot )
    {
      m_rg.destroy_node( m_pgnNewRoot );
    }
  }

  _TyGraphNode *  PGNTransferNewRoot()
  {
    _TyGraphNode * _pgn = m_pgnNewRoot;
    m_pgnNewRoot = 0;
    return _pgn;
  }

  void
  _create_nodes()
  {
    for ( size_t _st = 0; _st < m_stNodes; ++_st )
    {
      m_rgpgn[ _st ] = m_rg.__STL_TEMPLATE create_node1
        < const size_t & >( _st );
    }
  }

  size_t
  _rand( int _iAmong )
  {
    return rand() % _iAmong;
  }

  void
  _generate()
  {
    // First link all the nodes to each other:
    for ( size_t _st = 1; _st < m_stNodes; ++_st )
    {
      size_t stNode = _rand( _st ); // Choose among the current nodes.
      assert( stNode < _st );
      _TyGraphLink *  pgl = m_rg.create_link1( _st );

      if ( _rand( 2 ) == 0 )
      {
        m_rgpgn[ stNode ]->AddChild( *(m_rgpgn[_st]), *pgl,
                                      *(m_rgpgn[ stNode ]->PPGLBChildHead()),
                                      *(m_rgpgn[_st]->PPGLBParentHead()) );
      }
      else
      {
        m_rgpgn[ stNode ]->AddParent( *(m_rgpgn[_st]), *pgl,
                                      *(m_rgpgn[ stNode ]->PPGLBParentHead()),
                                      *(m_rgpgn[_st]->PPGLBChildHead()) );
      }
    }

    if ( m_stNodes > 1 )
    {
      // Now add random extra links between nodes:
      for ( size_t _st = 0; _st < m_stExtraLinks; ++_st )
      {
        size_t stNode1 = _rand( m_stNodes );  // Choose among the current nodes.
        size_t stNode2;
        do
        {
          stNode2 = _rand( m_stNodes ); // Choose among the current nodes.
        }
        while( stNode1 == stNode2 );

        _TyGraphLink *  pgl = m_rg.create_link1( _st );

        if ( _rand( 2 ) == 0 )
        {
          m_rgpgn[ stNode1 ]->AddChild( *(m_rgpgn[ stNode2 ]), *pgl,
                                        *(m_rgpgn[ stNode1 ]->PPGLBChildHead()),
                                        *(m_rgpgn[ stNode2 ]->PPGLBParentHead()) );
        }
        else
        {
          m_rgpgn[ stNode1 ]->AddParent( *(m_rgpgn[ stNode2 ]), *pgl,
                                        *(m_rgpgn[ stNode1 ]->PPGLBParentHead()),
                                        *(m_rgpgn[ stNode2 ]->PPGLBChildHead()) );
        }
      }
    }

    m_pgnNewRoot = m_rgpgn[ 0 ];
  }

};

__DGRAPH_END_NAMESPACE

#endif __GR_RNDM_H
