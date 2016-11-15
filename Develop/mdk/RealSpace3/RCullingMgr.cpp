#include "StdAfx.h"
#include "RCullingMgr.h"

//CulledUpdate때문에
#include "RActor.h"
#include "RTreePassRenderer.h"
#include "RTreeSceneNode.h"

namespace	rs3
{

	//----------------------------------------------------------------------
	// class RCullingCallBack
	RCullingCallBack::RCullingCallBack( RCullingMgr* pCullingMgr )
	{
		m_pCullingMgr = pCullingMgr;
	}	

	RCullingCallBack::~RCullingCallBack( void )
	{

	}

	void RCullingCallBack::CallBack_Cull( const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state )
	{
		_ASSERT( NULL != m_pCullingMgr );
	
		CULL_STATE modified_state = CheckStateChangeForCoherentCulling(before_state,current_state);

		RSceneNode* pSceneNode = pItem->GetSceneNode();
		if( modified_state == CS_OUTSIDE )
		{
			_ASSERT( m_pCullingMgr->IsSceneNodeInCulledList(pSceneNode) );
			m_pCullingMgr->RemoveSceneNodeFromCulledList(pSceneNode);
			m_pCullingMgr->RemoveSceneNodeFromRenderingList(pSceneNode);
		}
		else if( modified_state == CS_PARTIAL || modified_state == CS_INSIDE )
		{
			_ASSERT( false == m_pCullingMgr->IsSceneNodeInCulledList(pSceneNode) );
			m_pCullingMgr->AddSceneNodeToCulledList(pSceneNode);
			m_pCullingMgr->AddSceneNodeToRenderingList(pSceneNode);
		}
		
	}

	ROcclusionCullingCallBack::ROcclusionCullingCallBack( RCullingMgr* pCullingMgr )
	{	
		m_pCullingMgr = pCullingMgr;
	}
	ROcclusionCullingCallBack::~ROcclusionCullingCallBack( void )
	{

	}
	void ROcclusionCullingCallBack::CallBack_Cull( const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state )
	{
		_ASSERT( NULL != m_pCullingMgr );
		RSceneNode* pSceneNode = pItem->GetSceneNode();

		if( current_state == CS_OUTSIDE || current_state == CS_PARTIAL )
		{
			if( true == m_pCullingMgr->IsSceneNodeInCulledList(pSceneNode) )
				m_pCullingMgr->RemoveSceneNodeFromCulledList(pSceneNode);
		}
		else if( current_state == CS_INSIDE )
		{
			if( false == m_pCullingMgr->IsSceneNodeInCulledList(pSceneNode) )
				m_pCullingMgr->AddSceneNodeToCulledList(pSceneNode);
		}
		else _ASSERT(false);
	}

	//----------------------------------------------------------------------
	// class RCullingMgr
	RCullingMgr::RCullingMgr(void)	:
		m_pSPT(NULL),
		m_pRegisteredWorldRootScene(NULL),
		m_pRenderingList(NULL),
		m_pValidatedRenderingList(NULL),
		m_pCullingCallBack(NULL)
	{
	}

	RCullingMgr::~RCullingMgr( void )
	{
		if( m_pRegisteredWorldRootScene)	m_pRegisteredWorldRootScene->UnRegisterCullingMgr(this);
		if( NULL != m_pCullingCallBack )
		{
			delete m_pCullingCallBack;
			m_pCullingCallBack = NULL;
		}

		m_pRegisteredWorldRootScene = NULL;	//레퍼런스 해제
		ClearCulledList();					//Culled List 해제
		ClearRederingList();				//Rendering List 해제
		m_MapRSceneNodeToItem.clear();		//map해제

		if( m_pSPT )	//SPT해제
		{
			delete	m_pSPT;
			m_pSPT = NULL;
		}

		if(m_pRenderingList)
		{
			delete m_pRenderingList;
			m_pRenderingList = NULL;
		}

#ifdef VALIDATE_RENDERINGLIST
		if(m_pValidatedRenderingList)
			delete m_pValidatedRenderingList;
#endif
	}

	void RCullingMgr::RenderCulledListForDebug(DWORD color)
	{
		for(int i=0;i<RRP_MAX;++i)
		{
			SCENE_SET* pSceneSet = GetCulledList(RRENDERPASS(i));
			for(SCENE_SET::iterator iter = pSceneSet->begin(); iter != pSceneSet->end() ; ++iter)
			{
				RSceneNode* pNode = (*iter);
				pNode->DebugRenderSphere(color);
			}
		}
	}

	void RCullingMgr::AddSceneNodeToSPT( RSceneNode *pNode)
	{
		_ASSERT(NULL != m_pSPT);
		if( false == pNode->UseSpatialTree() ) return;
		_ASSERT( m_MapRSceneNodeToItem.end() == m_MapRSceneNodeToItem.find(pNode) );

		ROctreeItem* pItem = m_pSPT->Add(pNode);	_ASSERT(NULL != pItem);	
		m_MapRSceneNodeToItem.insert(RSCENENODE_TO_ITEM_MAP::value_type(pNode,pItem));
	}

	void RCullingMgr::RemoveSceneNodeFromSPT( RSceneNode *pNode)
	{
		_ASSERT(NULL != m_pSPT);
		if( false == pNode->UseSpatialTree() ) return;
		if( m_MapRSceneNodeToItem.end() != m_MapRSceneNodeToItem.find(pNode) )
		{
			ROctreeItem* pItem = m_MapRSceneNodeToItem[pNode];	_ASSERT(NULL != pItem);
			m_pSPT->Remove(pItem);
			m_MapRSceneNodeToItem.erase(pNode);
		}
	}

	void RCullingMgr::MoveSceneNodeInSPT( RSceneNode *pNode)
	{
		_ASSERT( m_MapRSceneNodeToItem.end() != m_MapRSceneNodeToItem.find(pNode) );//pNode가 있어야함

		ROctreeItem* pItem = m_MapRSceneNodeToItem[pNode];	
		_ASSERT(NULL != pItem);
		m_pSPT->Move(pItem);
	}

	void RCullingMgr::NotifyedThatWorldSceneRemoved(void)
	{
		//이미 다 씬노드에서 삭제되었어야만 함
		_ASSERT( 0 == m_MapRSceneNodeToItem.size() );
		if( NULL != m_pRenderingList )
		{
			_ASSERT( true == m_pRenderingList->IsEmpty() );
		}
		

		m_pRegisteredWorldRootScene = NULL;	//레퍼런스 해제
		ClearCulledList();					//Culled List 해제
		ClearRederingList();				//Rendering List 해제

		//_ASSERT( m_RenderingList.IsEmpty() );

		if( m_pSPT )	//SPT해제
		{
			delete	m_pSPT;
			m_pSPT = NULL;
		}
	}

	void RCullingMgr::AddSceneNodeToCulledList( RSceneNode *pNode)
	{
		_ASSERT( false == m_CullingList.ExistSceneNode(pNode) );
		m_CullingList.AddSceneNode(pNode);
	}

	void RCullingMgr::RemoveSceneNodeFromCulledList( RSceneNode* pNode)
	{
		_ASSERT( true == m_CullingList.ExistSceneNode(pNode) );
		m_CullingList.RemoveSceneNode(pNode);
	}

	bool RCullingMgr::IsSceneNodeInCulledList( RSceneNode* pNode )
	{
		return m_CullingList.ExistSceneNode(pNode);
	}

	void RCullingMgr::ClearCulledList( void )
	{
		m_CullingList.Clear();
	}

	void RCullingMgr::PrepareToCulling( RWorldSceneNode* pWorldScene,bool bEnableToOverwriteSPT)
	{
		RPFC_THISFUNC;
		_ASSERT(NULL != pWorldScene);
		if( NULL == m_pSPT )
		{
			_ASSERT( NULL == m_pRegisteredWorldRootScene );
		}
		else
		{
// 			if( false == bEnableToOverwriteSPT )
// 			{
// 				_ASSERT( pWorldScene == m_pRegisteredWorldRootScene );
// 			}
// 			else
			{
				if( pWorldScene != m_pRegisteredWorldRootScene )
				{
					if( m_pRegisteredWorldRootScene)	m_pRegisteredWorldRootScene->UnRegisterCullingMgr(this);

					m_pRegisteredWorldRootScene = NULL;	//레퍼런스 해제
					ClearCulledList();					//Culled List 해제
					ClearRederingList();				//Rendering List 해제
					m_MapRSceneNodeToItem.clear();		//map해제

					if( m_pSPT )	//SPT해제
					{
						delete	m_pSPT;
						m_pSPT = NULL;
					}
				}
			}

		}


		if( NULL == m_pSPT && NULL == m_pRegisteredWorldRootScene) 
		{
			_ASSERT( 0 == m_MapRSceneNodeToItem.size() );
			
			m_pRegisteredWorldRootScene = pWorldScene;
			m_pRegisteredWorldRootScene->RegisterCullingMgr(this);	//옵저버 등록
			BuildSPT(pWorldScene);						//공간분할정보 생성

		}
	}

	void RCullingMgr::BuildSPT( RWorldSceneNode* pWorldScene)
	{
		_ASSERT(NULL == m_pSPT);
		_ASSERT(NULL != pWorldScene);

		m_pSPT = new ROctreeManager(m_pCullingCallBack);

		for(int i=0;i<RRP_MAX;++i)
		{
			const RSceneNodeArray* pSceneArray = pWorldScene->GetNodeArray((RRENDERPASS)i);
			for(unsigned int j=0;j<pSceneArray->size();++j)
			{
				RSceneNode* pSceneNode = pSceneArray->at(j);
				AddSceneNodeToSPT(pSceneNode);
			}
		}
	}

	void RCullingMgr::OcclusionCull( RWorldSceneNode* pWorldScene,const RPlane *pPlanes, int planesNum,bool bEnableToOverwriteSPT /*= false*/ )
	{
		RPFC_THISFUNC;
		PrepareToCulling(pWorldScene,bEnableToOverwriteSPT);
		m_pSPT->Cull_MC(pPlanes,planesNum,m_pCullingCallBack);
	}

	void RCullingMgr::Cull( RWorldSceneNode* pWorldScene,const RPlane *pPlanes, int planesNum,bool bEnableToOverwriteSPT)
	{
		RPFC_B("CullingMgrCull");
		PrepareToCulling(pWorldScene,bEnableToOverwriteSPT);
		m_pSPT->Cull_MC(pPlanes,planesNum,m_pCullingCallBack);
		m_pRenderingList->Update();
		RPFC_E;

		#ifdef VALIDATE_RENDERINGLIST
			m_pValidatedRenderingList->Clear();
			MakeRenderingListFromCullingList(m_pValidatedRenderingList);
			_ASSERT(m_pRenderingList->Equal(*m_pValidatedRenderingList));
		#endif
	}

	void RCullingMgr::MakeRenderingListFromCullingList(RRenderingList* pValidatedRenderingList)
	{
		RPFC_THISFUNC;
		for(int pass = RRP_CULL_BEGIN;pass<=RRP_CULL_END;++pass)
		{
			SCENE_SET*	pSet = NULL;
			pSet = GetCulledList((RRENDERPASS)pass);
			for(SCENE_SET::iterator iter = pSet->begin();iter != pSet->end() ; ++iter)
			{
				RSceneNode* pSceneNode = (*iter);
				pSceneNode->AddToRenderingList(pValidatedRenderingList);
			}
		}
	}

	void RCullingMgr::CulledListUpdate( RCameraSceneNode* pCamera)
	{
		RPFC_THISFUNC;
		
		RSceneNode* pSceneNode;
		SCENE_SET::iterator iterCull;

		SCENE_SET* pTerrainSceneSet = GetCulledList(RRP_TERRAIN);
		for(iterCull =pTerrainSceneSet->begin(); iterCull != pTerrainSceneSet->end() ; ++iterCull)
		{
			pSceneNode = (*iterCull);
			REngine::GetSceneManager().GetPassRenderer(pSceneNode->GetUsingPass())->CullByViewFrustum( pSceneNode, pCamera);				
		}
	
		SCENE_SET* pStaticMeshSceneSet = GetCulledList(RRP_STATICMESH);
		for(iterCull = pStaticMeshSceneSet->begin(); iterCull != pStaticMeshSceneSet->end() ; ++iterCull)
		{
			pSceneNode = (*iterCull);
			REngine::GetSceneManager().GetPassRenderer(pSceneNode->GetUsingPass())->CullByViewFrustum( pSceneNode, pCamera);
		}
	}



	void RCullingMgr::CullSubNode(RCameraSceneNode* pCamera, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar)
	{
		RPFC_THISFUNC;
		SCENE_SET::iterator iterCull;
		RSceneNode *pSceneNode;

		SCENE_SET* pTerrainSceneSet = GetCulledList(RRP_TERRAIN);
		for(iterCull = pTerrainSceneSet->begin(); iterCull != pTerrainSceneSet->end() ; ++iterCull)
		{
			pSceneNode = (*iterCull);
			REngine::GetSceneManager().GetPassRenderer(pSceneNode->GetUsingPass())->CullByPlanes(pCamera, pSceneNode, cullDescs, pPlaneNear, pPlaneFar);
		}

		SCENE_SET* pStaticMeshSceneSet = GetCulledList(RRP_STATICMESH);
		for(iterCull = pStaticMeshSceneSet->begin(); iterCull != pStaticMeshSceneSet->end() ; ++iterCull)
		{
			pSceneNode = (*iterCull);
			REngine::GetSceneManager().GetPassRenderer(pSceneNode->GetUsingPass())->CullByPlanes(pCamera, pSceneNode, cullDescs, pPlaneNear, pPlaneFar);
		}
	}

	void RCullingMgr::AddSceneNodeToRenderingList( RSceneNode *pNode )
	{
		pNode->AddToRenderingList(m_pRenderingList);
	}

	void RCullingMgr::RemoveSceneNodeFromRenderingList( RSceneNode *pNode )
	{
		pNode->RemoveFromRenderingList(m_pRenderingList);
	}	

	void RCullingMgr::ClearRederingList( void )
	{
		if ( NULL != m_pRenderingList )
			m_pRenderingList->Clear();
	}

	void RCullingMgr::RefreshRenderingList( RSceneNode* pNode )
	{
		if(NULL == m_pRenderingList) return;
		if(false == IsSceneNodeInCulledList(pNode) )	return;
		pNode->RemoveFromRenderingList(m_pRenderingList);
		pNode->AddToRenderingList(m_pRenderingList);
	}

	//----------------------------------------------------------------------
	// class RCullingMgrObserver
	void RCullingMgrObserver::OnWorldRootSceneDestroyed( void )
	{
		CULLING_OBSERVER::iterator iter;
		for(iter = m_Observer.begin(); iter != m_Observer.end(); ++iter)
		{
			RCullingMgr* pCullingMgr = (*iter);
			pCullingMgr->NotifyedThatWorldSceneRemoved();
		}
		Clear();
	}

	void RCullingMgrObserver::OnSceneNodeAddedToSPT(RSceneNode* pSceneNode)
	{
		CULLING_OBSERVER::iterator iter;
		for(iter = m_Observer.begin(); iter != m_Observer.end(); ++iter)
		{
			RCullingMgr* pCullingMgr = (*iter);
			pCullingMgr->AddSceneNodeToSPT(pSceneNode);
		}
	}

	void RCullingMgrObserver::OnSceneNodeRemovedFromSPT( RSceneNode* pSceneNode )
	{
		CULLING_OBSERVER::iterator iter;
		for(iter = m_Observer.begin(); iter != m_Observer.end(); ++iter)
		{
			RCullingMgr* pCullingMgr = (*iter);
			pCullingMgr->RemoveSceneNodeFromSPT(pSceneNode);
		}
	}

	void RCullingMgrObserver::OnSceneNodeMoveInSPT( RSceneNode* pSceneNode )
	{
		CULLING_OBSERVER::iterator iter;
		for(iter = m_Observer.begin(); iter != m_Observer.end(); ++iter)
		{
			RCullingMgr* pCullingMgr = (*iter);
			pCullingMgr->MoveSceneNodeInSPT(pSceneNode);
		}
	}

	void RCullingMgrObserver::OnRefreshRenderingList( RSceneNode* pNode )
	{
		CULLING_OBSERVER::iterator iter;
		for(iter = m_Observer.begin(); iter != m_Observer.end(); ++iter)
		{
			RCullingMgr* pCullingMgr = (*iter);
			pCullingMgr->RefreshRenderingList(pNode);
		}
	}

	void RCullingMgrObserver::Register( RCullingMgr* pCullingMgr )
	{
		_ASSERT(NULL != pCullingMgr);
		m_Observer.insert(pCullingMgr);
	}

	void RCullingMgrObserver::UnRegister( RCullingMgr* pCullingMgr )
	{
		_ASSERT(NULL != pCullingMgr);
		m_Observer.erase(pCullingMgr);
	}

	bool RCullingMgrObserver::IsRegisteredCullingMGR( RCullingMgr* pCullingMgr )
	{
		CULLING_OBSERVER::iterator iter;
		iter = m_Observer.find(pCullingMgr);
		if( iter == m_Observer.end() )	return false;
		return true;
	}

	void RCullingMgrObserver::Clear( void )
	{
		m_Observer.clear();
	}	



}