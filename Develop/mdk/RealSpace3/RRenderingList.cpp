#include "stdafx.h"
#include "RRenderingList.h"
#include "RTreeSceneNode.h"

namespace rs3
{
	//----------------------------RActorNodeKey
	RActorNodeKey::RActorNodeKey( void )
		: m_pMaterial( NULL), m_nSubMaterial( NULL), m_nNodeType( NULL), m_pMeshNode( NULL), m_hVF(R_NONE)
	{
	}

	RActorNodeKey::~RActorNodeKey( void )
	{
	}

	bool RActorNodeKey::operator<( const RActorNodeKey& Right ) const
	{
		if(m_nNodeType		< Right.m_nNodeType)			return true;
		if(m_nNodeType		> Right.m_nNodeType)			return false;
		if(m_pMaterial		< Right.m_pMaterial)			return true;
		if(m_pMaterial		> Right.m_pMaterial)			return false;
		if(m_nSubMaterial	< Right.m_nSubMaterial)			return true;
		if(m_nSubMaterial	> Right.m_nSubMaterial)			return false;
		if(m_hVF			< Right.m_hVF)					return true;
		if(m_hVF			> Right.m_hVF)					return false;
		if(m_pMeshNode		< Right.m_pMeshNode)			return true;
		if(m_pMeshNode		> Right.m_pMeshNode)			return false;

		return false;	
	}


	//----------------------------RActorNodeShadowKey
	bool RShadowActorNodeKey::operator<( const RShadowActorNodeKey& Right ) const
	{
		if(m_pMeshNode		< Right.m_pMeshNode)			return true;
		if(m_pMeshNode		> Right.m_pMeshNode)			return false;
		if(m_nSubMaterial	< Right.m_nSubMaterial)			return true;
		if(m_nSubMaterial	> Right.m_nSubMaterial)			return false;

		return false;	
	}



	//----------------------------RRenderingList
	RRenderingList::RRenderingList( void )
	{
	}

	RRenderingList::~RRenderingList( void )
	{
		Clear();
	}

	void RRenderingList::Clear( void )
	{
		for( int pass = 0 ; pass < RRP_MAX ; ++pass )
		{
			m_Set[pass].clear();
		}

		m_setNullActor.clear();
		m_Alpha2PassActorSet.clear();

		m_ActorNodeRenderChain.Clear();
		m_AlphaAdditiveActorNodeRenderChain.Clear();
		m_mapAlphaActorNodeRenderChain.Clear();
		m_mapShadowActorNodeRenderChain.Clear();
		m_mapDoubleShadowActorNodeRenderChain.Clear();
		m_setNullActor.clear();

		ClearTreeChain();
	}

	void RRenderingList::Add(RSceneNode *pNode,RRENDERPASS pass)
	{
		m_Set[pass].insert(pNode);
	}

	void RRenderingList::Remove( RSceneNode *pNode,RRENDERPASS pass )
	{
		_ASSERT( m_Set[pass].end() != m_Set[pass].find(pNode) );
		m_Set[pass].erase(pNode);
	}

	void RRenderingList::RemoveActorNode( RSceneNode* pNode )
	{
		m_ActorNodeRenderChain.RemoveSceneNode(pNode);
		m_AlphaAdditiveActorNodeRenderChain.RemoveSceneNode(pNode);
		m_mapAlphaActorNodeRenderChain.RemoveSceneNode(pNode);
		m_mapShadowActorNodeRenderChain.RemoveSceneNode(pNode);
		m_mapDoubleShadowActorNodeRenderChain.RemoveSceneNode(pNode);
	}

	void RRenderingList::AddAlpha2PassActor( RSceneNode* pNode )
	{
		m_Alpha2PassActorSet.insert(pNode);
	}

	void RRenderingList::RemoveAlpha2PassActor( RSceneNode* pNode )
	{
		m_Alpha2PassActorSet.erase(pNode);
	}

	void RRenderingList::AddTreeChain( RTreeSceneNode *pNode )
	{
		SCENE_SET* pSet = FindTreeChainSet(pNode->m_pTreeResource);
		if( NULL == pSet ) pSet = CreateTreeChainSet(pNode->m_pTreeResource);
		pSet->insert(pNode);
	}

	void RRenderingList::RemoveTreeChain( RTreeSceneNode* pNode )
	{
		SCENE_SET* pSet = FindTreeChainSet(pNode->m_pTreeResource);

		if( NULL == pSet)//등록은 NULL리소스로 등록이 됬지만, 해제시에는 리소스가 로딩되었을 경우
		{
			pSet = FindTreeChainSet(NULL);
			_ASSERT(NULL != pSet);

			_ASSERT( pSet->find(pNode) != pSet->end() );//있어야만 한다
			pSet->erase(pNode);

			if( 0 == pSet->size() )
			{
				m_mapTreeChain.erase(NULL);
				delete pSet;
			}
		}
		else
		{
			pSet->erase(pNode);

			if( 0 == pSet->size() )
			{
				m_mapTreeChain.erase(pNode->m_pTreeResource);
				delete pSet;
			}
		}


	}


	SCENE_SET* RRenderingList::CreateTreeChainSet( RTreeResource* pResource )
	{
		_ASSERT(m_mapTreeChain.end() == m_mapTreeChain.find(pResource));
		SCENE_SET* pSet = new SCENE_SET;
		m_mapTreeChain.insert(PAIR_TREECHAIN(pResource,pSet));
		return pSet;
	}


	SCENE_SET* RRenderingList::FindTreeChainSet( RTreeResource* pResource )
	{
		MAP_TREECHAIN::iterator iter;
		iter = m_mapTreeChain.find(pResource);
		if( iter == m_mapTreeChain.end() )	return NULL;
		return iter->second;
	}



	void RRenderingList::ClearTreeChain( void )
	{
		MAP_TREECHAIN::iterator iter;
		for( iter = m_mapTreeChain.begin() ; iter != m_mapTreeChain.end() ; ++iter )
		{
			SCENE_SET* pSet = iter->second;
			delete pSet;
		}
		m_mapTreeChain.clear();
	}

	bool RRenderingList::IsEmpty( void )
	{
		Update();

		for( int pass = 0 ; pass < RRP_MAX ; ++pass )
		{
			if( false == m_Set[pass].empty() )
			{
				return false;
			}
		}
		
		if( false == m_mapTreeChain.empty() )
		{
			return false;
		}

		if( false == m_setNullActor.empty() )
		{
			return false;
		}

		if( false == m_ActorNodeRenderChain.IsEmpty() )
		{
			return false;
		}
		
		if( false == m_AlphaAdditiveActorNodeRenderChain.IsEmpty() )
		{
			return false;
		}

		if( false == m_mapAlphaActorNodeRenderChain.IsEmpty() )
		{
			return false;
		}

		if( false == m_Alpha2PassActorSet.empty() )
		{
			return false;
		}

		if( false == m_mapShadowActorNodeRenderChain.IsEmpty() )
		{
			return false;
		}

		if( false == m_mapDoubleShadowActorNodeRenderChain.IsEmpty() )
		{
			return false;
		}

		return true;
	}

	void RRenderingList::Update( void )
	{
		//NULL리소스로 렌더체인을 이룬 트리씬노드들 다시 탐색하여, 리소스 로딩 완료되었으면 제대로된 트리렌더체인에 넣어준다
		SCENE_SET* pSet = FindTreeChainSet(NULL);
		if( NULL != pSet )
		{
			for( SCENE_SET::iterator iter = pSet->begin() ; iter != pSet->end() ; )
			{
				RSceneNode* pNode = (*iter);
				RTreeSceneNode *pTree = (RTreeSceneNode*) pNode;
				if( NULL != pTree->m_pTreeResource )
				{
					pSet->erase(iter++);
					AddTreeChain(pTree);
					continue;
				}	
				++iter;
			}
			if( pSet->empty() )	
			{
				m_mapTreeChain.erase(NULL);
				delete pSet;
			}
		}
		
		//Null Actor처리
		std::set<RSceneNode*>::iterator NullActorIter;
		for( NullActorIter = m_setNullActor.begin() ; NullActorIter != m_setNullActor.end() ; )
		{
			RSceneNode* pActor = (*NullActorIter);
			if( pActor->IsLoadingCompleted() )
			{
				pActor->AddToRenderingList(this);
				m_setNullActor.erase(NullActorIter++);
			}
			else
			{
				++NullActorIter;
			}
		}
	}

	void RRenderingList::AddNullActor( RSceneNode* pNode )
	{
		_ASSERT( m_setNullActor.end() == m_setNullActor.find(pNode) );
		m_setNullActor.insert(pNode);
	}

	void RRenderingList::RemoveNullActor( RSceneNode* pNode )
	{
		_ASSERT( m_setNullActor.end() != m_setNullActor.find(pNode) );
		m_setNullActor.erase(pNode);
	}

	bool RRenderingList::IsInNullActor( RSceneNode* pNode )
	{
		if( m_setNullActor.end() != m_setNullActor.find(pNode) )	return true;
		return false;
	}

	bool RRenderingList::Equal(RRenderingList& rhs)
	{
		//NULL액터용
		if( this->m_setNullActor.size() != rhs.m_setNullActor.size() )
		{
			return true;
		}
		else
		{
			for( SCENE_SET::iterator iter = m_setNullActor.begin() ; iter != m_setNullActor.end() ; ++iter )
			{
				RSceneNode *pSceneNode = (*iter);

				if( false == rhs.IsInNullActor(pSceneNode) )
				{
					return false;
				}
			}
		}

		//일반노드용
		for(int pass=RRP_RENDER_BEGIN ; pass<RRP_MAX ; ++pass)
		{
			SCENE_SET *pSet = &m_Set[pass];
			for( SCENE_SET::iterator iter = pSet->begin() ; iter != pSet->end() ; ++iter )
			{
				RSceneNode *pSceneNode = (*iter);
				SCENE_SET *pRHS = rhs.GetRenderingList((RRENDERPASS)pass);

				if( pSet->size() != pRHS->size() )
				{
					return false;
				}

				if( pRHS->end() == pRHS->find(pSceneNode) )
				{
					return false;
				}
			}
		}


		//트리체인용
		for(MAP_TREECHAIN::iterator iter = m_mapTreeChain.begin() ; iter != m_mapTreeChain.end() ; ++iter )
		{
			RTreeResource *pTreeResource = iter->first;
			SCENE_SET* pSet = iter->second;

			SCENE_SET* pRHSSet = rhs.FindTreeChainSet(pTreeResource);
			if( NULL == pRHSSet )
			{
				return false;
			}
			if( pSet->size() != pRHSSet->size() )
			{
				return false;
			}

			for( SCENE_SET::iterator iterSet = pSet->begin(); iterSet != pSet->end(); ++iterSet )
			{
				RSceneNode *pNode = (*iterSet);
				if( pRHSSet->end() == pRHSSet->find(pNode) )
				{
					return false;
				}
			}
		}

		if( false == m_ActorNodeRenderChain.Equal(rhs.m_ActorNodeRenderChain) )
		{
			return false;
		}

		if( false == m_AlphaAdditiveActorNodeRenderChain.Equal(rhs.m_AlphaAdditiveActorNodeRenderChain) )
		{
			return false;
		}

		if( false == m_mapAlphaActorNodeRenderChain.Equal(rhs.m_mapAlphaActorNodeRenderChain) )
		{
			return false;
		}

		if( false == m_mapShadowActorNodeRenderChain.Equal(rhs.m_mapShadowActorNodeRenderChain) )
		{
			return false;
		}

		if( false == m_mapDoubleShadowActorNodeRenderChain.Equal(rhs.m_mapDoubleShadowActorNodeRenderChain) )
		{
			return false;
		}


		return true;
	}

}