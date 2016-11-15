#include "stdafx.h"
#include "RWorldSceneNode.h"
#include "MFileSystem.h"
#include "RDebugPassRenderer.h"
#include "REmitter.h"
// http://dev/redmine/issues/16758
/*
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
*/

namespace rs3 {

MImplementRTTI(RWorldSceneNode, RSceneNode);

// http://dev/redmine/issues/16758
/*
using namespace tbb;

void cAutoSceneNode::update( RSceneNodeUpdateVisitor& _pSceneNodeUpdator )
{
	for(RAutoUpdateNodeList::iterator itr = m_listAutoUpdateSceneNode.begin(); itr!= m_listAutoUpdateSceneNode.end(); )
	{
		if(itr->bValid)
		{
			RSceneNode* pUpdatableSceneNode = itr->pSceneNode;
			if (_pSceneNodeUpdator.OnPreOrderVisit(pUpdatableSceneNode))
				_pSceneNodeUpdator.OnPostOrderVisit(pUpdatableSceneNode);
			++itr;
		}
		else
			itr = m_listAutoUpdateSceneNode.erase(itr);
	}
}


bool cAutoSceneNode::ExistSceneNode( RSceneNode* pSceneNode )
{
	for(RAutoUpdateNodeList::iterator itr = m_listAutoUpdateSceneNode.begin(); itr!= m_listAutoUpdateSceneNode.end(); )
	{
		_ASSERT( true == itr->bValid );
		if( pSceneNode == itr->pSceneNode ) return true;
	}
	return false;
}

void cAutoSceneNode::pushBack( RUPDATENODE& node )
{
	m_listAutoUpdateSceneNode.push_back( node );
}


list<RUPDATENODE>::iterator cAutoSceneNode::getEnd()
{
	return m_listAutoUpdateSceneNode.end();
}

void cEmitter::preEmitterUpdate( RSceneNodeUpdateVisitor& _pSceneNodeUpdator )
{
	for(RAutoUpdateNodeList::iterator itr = m_listAutoUpdateSceneNode.begin(); itr!= m_listAutoUpdateSceneNode.end(); )
	{
		if(itr->bValid)
		{
			RSceneNode* pUpdatableSceneNode = itr->pSceneNode;
			if (_pSceneNodeUpdator.OnPreOrderVisit(pUpdatableSceneNode))
			{
				((REmitter*)pUpdatableSceneNode)->SetCurrentUpdate( true );
				((REmitter*)pUpdatableSceneNode)->TBBPreCreateParticle();
			}
			else
				((REmitter*)pUpdatableSceneNode)->SetCurrentUpdate( false );
			++itr;
		}
		else
		{
			itr = m_listAutoUpdateSceneNode.erase(itr);
		}
	}
}


void cEmitter::parallelEmitterUpdate( RSceneNodeUpdateVisitor& _pSceneNodeUpdator )
{
	class TBBEmiterUpdate
	{
	public:
		TBBEmiterUpdate(RAutoUpdateNodeList* _listEmitter, RSceneNodeUpdateVisitor*	_Updator) :
		  m_listEmitter(_listEmitter), m_Updator( _Updator )
		  {

		  }

		  ~TBBEmiterUpdate(){}

		  void operator()( const tbb::blocked_range< size_t >& r ) const 
		  {
			  list<RUPDATENODE>::iterator itr = m_listEmitter->begin();

			  for(size_t i = 0 ; i < r.begin() && itr != m_listEmitter->end() ; ++i )
				  ++itr;

			  for( size_t i = 0 ; i < r.end() - r.begin() && itr != m_listEmitter->end() ; ++i )
			  {
				  RSceneNode* pUpdatableSceneNode = itr->pSceneNode;
				  if( ((REmitter*)pUpdatableSceneNode)->GetCurrentUpdate() )
				  {
					  ((REmitter*)pUpdatableSceneNode)->TBBPostUpdate();
					  pUpdatableSceneNode->UpdateSceneNodeControllers( RSNCTRL_POSTUPDATE );
				  }

				  ++itr;
			  }
		  }

	public:
		RAutoUpdateNodeList*		m_listEmitter;
		RSceneNodeUpdateVisitor*	m_Updator;
	};

	parallel_for( blocked_range< size_t >( 0 , m_listAutoUpdateSceneNode.size(), m_listAutoUpdateSceneNode.size() / 3 + 1 ), 
		TBBEmiterUpdate( &m_listAutoUpdateSceneNode, &_pSceneNodeUpdator ) );	
}


void cEmitter::postEmitterUpdate( RSceneNodeUpdateVisitor& _pSceneNodeUpdator )
{
	for(RAutoUpdateNodeList::iterator itr = m_listAutoUpdateSceneNode.begin(); itr!= m_listAutoUpdateSceneNode.end(); )
	{
		if(itr->bValid)
		{
			RSceneNode* pUpdatableSceneNode = itr->pSceneNode;

			if( ((REmitter*)pUpdatableSceneNode)->GetCurrentUpdate() )
				((REmitter*)pUpdatableSceneNode)->SetCardinal();

			((REmitter*)pUpdatableSceneNode)->TBBDeleteParticle();
		}

		++itr;
	}
}


void cEmitter::seriesEmitterUpdate( RSceneNodeUpdateVisitor& _pSceneNodeUpdator )
{
	for(RAutoUpdateNodeList::iterator itr = m_listAutoUpdateSceneNode.begin(); itr!= m_listAutoUpdateSceneNode.end(); )
	{
		if(itr->bValid)
		{
			RSceneNode* pUpdatableSceneNode = itr->pSceneNode;
			if (_pSceneNodeUpdator.OnPreOrderVisit(pUpdatableSceneNode))
				_pSceneNodeUpdator.OnPostOrderVisit(pUpdatableSceneNode);
			++itr;
		}
		else
			itr = m_listAutoUpdateSceneNode.erase(itr);
	}
}


void cEmitter::update( RSceneNodeUpdateVisitor& _pSceneNodeUpdator )
{
	if( !m_listAutoUpdateSceneNode.empty() )
	{
		bool bTBB( false );
		if( !bTBB )
			seriesEmitterUpdate( _pSceneNodeUpdator );		
		else
		{
			preEmitterUpdate( _pSceneNodeUpdator );
			parallelEmitterUpdate( _pSceneNodeUpdator );
			postEmitterUpdate( _pSceneNodeUpdator );
		}
	}
}

void cEmitter::pushBack( RUPDATENODE& node )
{
	m_listAutoUpdateSceneNode.push_back( node );
}

list<RUPDATENODE>::iterator cEmitter::getEnd()
{
	return m_listAutoUpdateSceneNode.end();
}

*/



RWorldSceneNode::RWorldSceneNode(void) : 
	RSceneNode(RRP_WORLDSCENE),
	m_pEnvironmentSet(NULL),
	m_pTerrain(NULL),
	m_pStaticMeshSceneNode(NULL),
	m_pDirectionalLight(NULL)
{
	m_bCullingNode = false;

	SetAttached( true );
	// bAttached가 T상태로 생성 되는 것은 씬 노드의 루트 씬이라고 가정 한다.
	m_pRootScene = this;

	 // http://dev/redmine/issues/16758
//	m_pTaskInit = new task_scheduler_init(task_scheduler_init::automatic);
}

RWorldSceneNode::~RWorldSceneNode()
{
	Clear();	//RSceneNode의 소멸자에서도 호출을 해주지만, 이것을 먼저 호출하고 밑에 것을 호출되어야 해서, 여기서 명시적으로 호출
	m_CullingMgrObserver.OnWorldRootSceneDestroyed();

	// http://dev/redmine/issues/16758
	/*
	m_pTaskInit->terminate();
	SAFE_DELETE(m_pTaskInit);
	*/
}

bool RWorldSceneNode::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	return true;
}

std::string RWorldSceneNode::GetXMLFilePath() const
{
	return MGetPurePath(m_strXMLFileName);
}

bool RWorldSceneNode::RegisterRenderCandidate( RSceneNode* pSceneNode )
{
	if(pSceneNode->IsCullingNode())
	{
		_ASSERT(pSceneNode->IsRegisteredCandidate() == false);

		RRENDERPASS pass = pSceneNode->GetUsingPass();
		m_candidateNodes[pass].push_back(pSceneNode);

		pSceneNode->SetRegisteredCandidate( true);
		m_CullingMgrObserver.OnSceneNodeAddedToSPT(pSceneNode);		
	}

	return true;
}

bool RWorldSceneNode::UnregisterRenderCandidate( RSceneNode* pSceneNode )
{
	if(pSceneNode->IsCullingNode())
	{
		RRENDERPASS pass = pSceneNode->GetUsingPass();
		RSceneNodeArray &nodeArray = m_candidateNodes[pass];
		for(RSceneNodeArray::iterator i = nodeArray.begin(); i!=nodeArray.end(); i++) 
		{
			RSceneNode *pNode = *i;
			if(pNode==pSceneNode)
			{
				pSceneNode->SetRegisteredCandidate( false);
				nodeArray.erase(i);

				m_CullingMgrObserver.OnSceneNodeRemovedFromSPT(pSceneNode);
				return true;
			}
		}
		_ASSERT(FALSE);
		return false;
	}

	return true;
}


void RWorldSceneNode::MoveSceneNodeInBVH(RSceneNode *pNode)
{
	m_CullingMgrObserver.OnSceneNodeMoveInSPT(pNode);
}

void RWorldSceneNode::RegisterCullingMgr( RCullingMgr* pCullingMgr )
{
	m_CullingMgrObserver.Register(pCullingMgr);
}

void RWorldSceneNode::UnRegisterCullingMgr( RCullingMgr* pCullingMgr )
{
	m_CullingMgrObserver.UnRegister(pCullingMgr);
}

void RWorldSceneNode::RemoveSceneNodeFromBVH( RSceneNode *pNode )
{
	m_CullingMgrObserver.OnSceneNodeRemovedFromSPT(pNode);
}

void RWorldSceneNode::AddSceneNodeToBVH( RSceneNode* pNode )
{
	m_CullingMgrObserver.OnSceneNodeAddedToSPT(pNode);
}

void RWorldSceneNode::RefreshRenderingList( RSceneNode* pNode )
{
	m_CullingMgrObserver.OnRefreshRenderingList(pNode);
}

rs3::RAutoUpdateNodeListItr RWorldSceneNode::RegisterAutoUpdate( RSceneNode* pSceneNode )
{
	// http://dev/redmine/issues/16758
	/*
	RUPDATENODE node;
	node.bValid = true;
	node.pSceneNode = pSceneNode;
	RAutoUpdateNodeListItr itr;
	
	if( strcmp(pSceneNode->GetNodeIDString(), "REMITTER") == 0 )
	{
		m_cEmitter.pushBack( node );
		itr = m_cEmitter.getEnd();
	}
	else
	{
		m_cAutoSceneNode.pushBack( node );
		itr = m_cAutoSceneNode.getEnd();
	}
	
	--itr;

	_ASSERT( (*itr).pSceneNode == pSceneNode);
	return itr;
	*/

	RUPDATENODE node;
	node.bValid = true;
	node.pSceneNode = pSceneNode;

	m_listAutoUpdateSceneNode.push_back(node);

	RAutoUpdateNodeListItr itr = m_listAutoUpdateSceneNode.end();
	--itr;

	_ASSERT( (*itr).pSceneNode == pSceneNode);
	return itr;
}

void RWorldSceneNode::UnregisterAutoUpdate( RAutoUpdateNodeListItr itr )
{

	// 업데이트때 auto update 를 켜고 끄고 하는 경우가 있으므로
	// 바로 지우지 않고 다음 업데이트때 지워준다
	(*itr).bValid = false;
}


void RWorldSceneNode::UpdateAutoUpdateSceneNode()
{
	// http://dev/redmine/issues/16758
	/*
	UpdateSelect( &m_cAutoSceneNode );
	UpdateSelect( &m_cEmitter );
	*/

	RSceneNodeUpdateVisitor			SceneNodeUpdator;
	for(RAutoUpdateNodeList::iterator itr = m_listAutoUpdateSceneNode.begin(); itr!= m_listAutoUpdateSceneNode.end(); )
	{
		if(itr->bValid)
		{
			// 씬노드 업데이트
			RSceneNode* pUpdatableSceneNode = itr->pSceneNode;
			if (SceneNodeUpdator.OnPreOrderVisit(pUpdatableSceneNode))
				SceneNodeUpdator.OnPostOrderVisit(pUpdatableSceneNode);
			++itr;
		}
		else
		{
			itr = m_listAutoUpdateSceneNode.erase(itr);
		}
	}
}

void RWorldSceneNode::GatherAABB( RBoundingBox* _pGatherAABB )
{
	class RGatherAABBSceneNodeVisitor : public RSceneNodeVisitor
	{
	public:
		RGatherAABBSceneNodeVisitor()
		{
			m_aabb.Initialize();
		}

		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode ) override
		{
			if( RRP_SKYBOX == pSceneNode->GetUsingPass() || RRP_WEATHEREFFECT == pSceneNode->GetUsingPass() )
				return false;

			return true;
		}

		virtual void OnPostOrderVisit( RSceneNode* pSceneNode ) override 
		{
			m_aabb.Add( pSceneNode->GetAABB() );
		}

		RBoundingBox GetGatheredAABB() { return m_aabb; }

	private:
		RBoundingBox m_aabb;
	};

	RGatherAABBSceneNodeVisitor v;
	Traverse( &v );

	_pGatherAABB->Add( v.GetGatheredAABB() );
}

bool RWorldSceneNode::ExistSceneNodeInAutoUpdateList( RSceneNode* pSceneNode )
{
	for(RAutoUpdateNodeList::iterator itr = m_listAutoUpdateSceneNode.begin(); itr!= m_listAutoUpdateSceneNode.end(); )
	{
		_ASSERT( true == itr->bValid );
		if( pSceneNode == itr->pSceneNode ) return true;
	}
	return false;
}

void RWorldSceneNode::UpdateBoundingVolumeAllSceneNodes()
{
	for( size_t pass = RRP_CULL_BEGIN; pass < RRP_CULL_END; ++pass )
	{
		RSceneNodeArray& sceneNodeList = m_candidateNodes[pass];
		for( size_t i = 0; i < sceneNodeList.size(); ++i )
		{
			RSceneNode* pSceneNode = sceneNodeList[i];
			if( pSceneNode->IsNeedUpdateBoundingVolume() )
				pSceneNode->UpdateBoundingVolume();
		}
	}
}

}