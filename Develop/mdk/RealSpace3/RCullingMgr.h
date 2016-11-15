#ifndef _RCullingMGR_H_ 
#define _RCullingMGR_H_

#include "ROctreeManager.h"
#include "RRenderingList.h"
#include "RCullingList.h"
#include "ROccluderList.h"

#ifdef _DEBUG
#define VALIDATE_RENDERINGLIST
#endif

namespace	rs3
{
	//전방선언
	class RCullingMgrObserver;	
	class RCullingMgr;

	//----------------------------------------------------------------------
	// class RCullingCallBack
	// 컬링컬백함수 재정의
	class RCullingCallBack : public ROctreeCallBack
	{
	public:
		RCullingCallBack(RCullingMgr* pCullingMgr);
		~RCullingCallBack(void);

	public:
		virtual void	CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE state) override {};	//보통 culling용 컬백
		virtual void	CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state) override; //coherent culling용 컬백
		RCullingMgr*	GetCullingMgr(void)	{	return m_pCullingMgr;	}

	private:
		RCullingMgr*	m_pCullingMgr;
	};

	class ROcclusionCullingCallBack : public ROctreeCallBack
	{
	public:
		ROcclusionCullingCallBack(RCullingMgr* pCullingMgr);
		~ROcclusionCullingCallBack(void);

	public:
		virtual void	CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE state) override {};	//보통 culling용 컬백
		virtual void	CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state) override; //coherent culling용 컬백
		RCullingMgr*	GetCullingMgr(void)	{	return m_pCullingMgr;	}

	private:
		RCullingMgr*	m_pCullingMgr;
	};

	//----------------------------------------------------------------------
	// class RCullingMgr
	class RCullingMgr
	{
		friend RCullingCallBack;		//RCullingCallBack의 함수에서 컬링리스트에 씬노드를 담아야 하는데, 그 접근함수가 이함수의 private라서
		friend ROcclusionCullingCallBack;
		friend RCullingMgrObserver;		//?
	public:
		//생성/소멸자
		RCullingMgr(void);
		virtual ~RCullingMgr(void);

	public:

		//컬링관리
		void						Cull(RWorldSceneNode* pWorldScene,const RPlane *pPlanes, int planesNum,bool bEnableToOverwriteSPT = false);
		void						CullSubNode(RCameraSceneNode* pCamera, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar);

		//오클루전컬링
		void						OcclusionCull(RWorldSceneNode* pWorldScene,const RPlane *pPlanes, int planesNum,bool bEnableToOverwriteSPT = false);

		//컬링후 업데이트
		void						CulledListUpdate(RCameraSceneNode* pCamera);

		//접근자
		SCENE_SET*				GetCulledList(RRENDERPASS pass)			{	return m_CullingList.GetCulledList(pass);	}
		SCENE_SET*				GetRenderingList(RRENDERPASS pass)		{	return m_pRenderingList->GetRenderingList(pass);					}
		RRenderChain<RActorNodeKey>*			GetActorNodeChain(void)					{	return m_pRenderingList->GetActorNodeRenderChain();				}
		RRenderChain<RActorNodeKey>*			GetAlphaActorNodeChain(void)			{	return m_pRenderingList->GetAlphaActorNodeRenderChain();			}
		RRenderChain<RActorNodeKey>*			GetAlphaAdditiveActorNodeChain(void)	{	return m_pRenderingList->GetAlphaAdditiveActorNodeRenderChain();	}
		SCENE_SET*								GetAlpha2PassActor(void)				{ return m_pRenderingList->GetAlpha2PassActor();	}
		RRenderChain<RShadowActorNodeKey>*			GetActorNodeShadowChain(void)	{	return m_pRenderingList->GetShadowActorNodeRenderChain();	}
		RRenderChain<RShadowActorNodeKey>*			GetActorNodeDoubleShadowChain(void)	{	return m_pRenderingList->GetDoubleShadowActorNodeRenderChain();	}

		MAP_TREECHAIN*			GetTreeChain(void)						{	return m_pRenderingList->GetTreeRenderChain();					}

		//디버그
		void					RenderCulledListForDebug(DWORD color);

		//유틸
		void					MakeRenderingListFromCullingList(RRenderingList* pValidatedRenderingList);

		template<class RenderingList>
		void SetRenderingList()
		{
			if(m_pRenderingList)
				delete m_pRenderingList;
			m_pRenderingList = new RenderingList;

#ifdef VALIDATE_RENDERINGLIST
			if(m_pValidatedRenderingList)
				delete m_pValidatedRenderingList;
			m_pValidatedRenderingList = new RenderingList;
#endif
		}

	private:
		//BVH관리
		void						AddSceneNodeToSPT(RSceneNode *pNode);
		void						RemoveSceneNodeFromSPT(RSceneNode *pNode);
		void						MoveSceneNodeInSPT(RSceneNode *pNode);
		void						NotifyedThatWorldSceneRemoved(void);

		//CulledList관리
		void						AddSceneNodeToCulledList(RSceneNode *pNode);
		void						RemoveSceneNodeFromCulledList(RSceneNode* pNode);
		bool						IsSceneNodeInCulledList(RSceneNode* pNode);
		void						ClearCulledList(void);	

		void						PrepareToCulling(RWorldSceneNode* pWorldScene,bool bEnableToOverwriteSPT = false);
		void						BuildSPT( RWorldSceneNode* pWorldScene);

		//옵저버
		void						RegisterObserverTo(void);
		void						UnRegsiterObserverTo(void);

		//RenderingList관리
		void						AddSceneNodeToRenderingList(RSceneNode *pNode);
		void						RemoveSceneNodeFromRenderingList(RSceneNode *pNode);
		void						ClearRederingList(void);
		void						RefreshRenderingList(RSceneNode* pNode);

	private:	//컬링리스트
		RCullingList		m_CullingList;

	private:	//렌더링리스트
		RRenderingList*		m_pRenderingList;
		RRenderingList*		m_pValidatedRenderingList;

	protected:	//공간분할
		typedef std::map<RSceneNode*,ROctreeItem*> RSCENENODE_TO_ITEM_MAP;
		RSCENENODE_TO_ITEM_MAP	m_MapRSceneNodeToItem;

		ROctreeManager*		m_pSPT;
		ROctreeCallBack*	m_pCullingCallBack;
		RWorldSceneNode*	m_pRegisteredWorldRootScene;
	};

	//----------------------------------------------------------------------
	// class RCullingMgrObserver
	// 옵저버
	class RCullingMgrObserver
	{
	public:
		void	OnWorldRootSceneDestroyed(void);
		void	OnSceneNodeAddedToSPT(RSceneNode* pSceneNode);
		void	OnSceneNodeRemovedFromSPT(RSceneNode* pSceneNode);
		void	OnSceneNodeMoveInSPT(RSceneNode* pSceneNode);
		void	OnRefreshRenderingList(RSceneNode* pNode );

	public:
		void	Register(RCullingMgr* pCullingMgr);
		void	UnRegister(RCullingMgr* pCullingMgr);
		bool	IsRegisteredCullingMGR(RCullingMgr* pCullingMgr);

		void	Clear(void);

	private:
		typedef std::set<RCullingMgr*> CULLING_OBSERVER;
		std::set<RCullingMgr*>		m_Observer;
	};

}

#endif