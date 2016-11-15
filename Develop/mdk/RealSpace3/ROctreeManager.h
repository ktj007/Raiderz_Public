#pragma once

// use coherent culling
#define USE_CC

namespace rs3
{
	class ROctreeItem;
	class ROctreeNode;
	class ROctreeManager;
	class ROMT;

	//----------------------------------------------------------------------
	// 컬링함수 이름
	// Cull vs CullC : 
	//									CullC는 coherent list를 만들기 위한 컬링 함수
	// Cullx vs Cullx_M vs Cullx_MC : 
	//									_M은 masking최적화 적용, _MC는 masking최적화 plane coherency 최적화 적용


	//----------------------------------------------------------------------
	// class ROctreeCallBack
	class ROctreeCallBack
	{
	public:
		virtual void CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE state) = 0;	//보통 culling용 컬백
		virtual void CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state) = 0; //coherent culling용 컬백

		CULL_STATE CheckStateChangeForCoherentCulling(CULL_STATE before_state,CULL_STATE current_state);
		CULL_STATE CheckStateChangeForOcclusionCoherentCulling(CULL_STATE before_state,CULL_STATE current_state);
	};

	//----------------------------------------------------------------------
	// class ROctreeItem
	class ROctreeItem
	{
		friend ROMT;
		friend ROctreeNode;
		friend ROctreeManager;
	public:
		ROctreeItem(void);
		~ROctreeItem(void);

	public:
		bool	FitsInOctreeNode(ROctreeNode* pOctreeNode);

		float	X(void)	{	return m_pSceneNode->GetSphere().center.x;	}
		float	Y(void)	{	return m_pSceneNode->GetSphere().center.y;	}
		float	Z(void)	{	return m_pSceneNode->GetSphere().center.z;	}
		float	R(void)	{	return m_pSceneNode->GetSphere().radius;	}

		RSceneNode*	GetSceneNode(void)	{	return m_pSceneNode;	}

	private:
		inline void			SetState(CULL_STATE state)	{	m_State = state;	}
		inline CULL_STATE	GetState(void)				{	return m_State;		}

	private:
		ROctreeItem*	m_pPrev;
		ROctreeItem*	m_pNext;
		ROctreeNode*	m_pOtreeNode;
		RSceneNode*		m_pSceneNode;
	
	private://최적화를 위해 필요한 변수
		int			m_DeterminantPlaneIndex;	//이 Item을 outside판정나게하는 평면의 인덱스

	private://Coherenct 컬링리스트를 만들기 위해 필요한 변수
		CULL_STATE	m_State;
	};


	//----------------------------------------------------------------------
	// class ROctreeNode
	static const int O_HEIGHT = 2;	//옥트리는 2개층에 4개의 노드들을 가지고 있음. 즉, 2*4 = 8 개의 노드를 가짐
	static const int O_WIDTH  = 4;

	class ROctreeNode
	{
		friend ROMT;
		friend ROctreeItem;
		friend ROctreeManager;
	public:
		ROctreeNode(ROctreeNode* pParent, float x, float y,float z, float radius, int d);
		~ROctreeNode(void);
	public:
		static	ROctreeNode*	Create(ROctreeNode* pParent, float x, float y,float z, float radius,int d);
		static	void			Release(ROctreeNode* pNode);

	private:
		void	DetachFromParent(void);
		inline	bool	IsEmpty(void);

		//Normal용
		void	Cull_M(const RPlane* pPlanes,int planesNum,CULL_STATE state,ROctreeCallBack *pCallBack,int Masking);
		void	CallBackItems_M(const RPlane* pPlanes,int planesNum,CULL_STATE state,ROctreeCallBack *pCallBack,int Masking);

		//Coherent List를 만드는 컬링함수들
		void	CullC_MC(const RPlane* pPlanes,int planesNum,CULL_STATE state,ROctreeCallBack *pCallBack,int Masking);
		void	CallBackItemsC_MC(const RPlane* pPlanes,int planesNum,CULL_STATE state,ROctreeCallBack *pCallBack,int Masking);

	protected:
		float	m_X,m_Y,m_Z;
		float	m_Radius;
		int		m_Depth;
		int		m_ItemsCount;

	private:
		ROctreeNode*	m_pParent;
		ROctreeNode*	m_pChildren[O_HEIGHT][O_WIDTH];
		ROctreeItem*	m_pItemsChain;

	private:
		int			m_DeterminantPlaneIndex;	//이노드를 outside판정나게하는 평면의 인덱스
		CULL_STATE	m_State;
	};

	//----------------------------------------------------------------------
	// class ROctreeNode
	class ROctreeManager
	{
	public:
		ROctreeManager(ROctreeCallBack *pCallBack);
		~ROctreeManager(void);

	public:
		ROctreeItem*	Add(RSceneNode* pSceneNode);
		void			Remove(ROctreeItem *pItem);
		void			Move(ROctreeItem *pItem,RSphere &Sphere);
		void			Move(ROctreeItem *pItem);

	public:
		void	Cull(const RPlane* pPlanes,int planesNum,ROctreeCallBack *pCallBack);
		void	Cull_MC(const RPlane* pPlanes,int planesNum,ROctreeCallBack *pCallBack);

	private:
		int		Insert(ROctreeNode* pOctreeNode,ROctreeItem *pItem);
		void	Detach(ROctreeItem *pItem);	//SPT에서 삭제시킴
		void	DestroyEmptyNodeRecursively(ROctreeNode* pOctreeNode);

	protected://변수
		ROctreeNode*	m_pRootNode;

	protected://속성
		float	m_WorldHalfRadius;	//공간의 반지름
		int		m_MaxDepth;			//트리의 깊이 제한
		float	m_LooseK;			//loose 계수 K

		int		m_ItemsCount;	//이 옥트리에 포함된 씬노드의 갯수 (주의 : 자식옥트리노드에 삽입된 씬노드의 갯수는 포함한게 아님)

	protected://coherent list를 만들기 위해
		typedef struct tagStateItem
		{
			CULL_STATE		state;
			ROctreeItem*	pItem;
			bool operator<( const tagStateItem& Right) const
			{
				if(pItem		< Right.pItem)			return true;
				if(pItem		> Right.pItem)			return false;
				return false;
			}
		}STATE_ITEM;
		std::set<STATE_ITEM>	m_AddedStateItemSet;
		std::set<ROctreeItem*>	m_RemovedItemSet;

	protected:
		ROctreeCallBack *m_pCallBack;
	};

	void	CheckStateChangeAndCallBack(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state,ROctreeCallBack *pCallBack);
}