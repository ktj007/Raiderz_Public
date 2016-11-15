#include "stdafx.h"
#include "ROctreeManager.h"

namespace rs3
{
	

	//----------------------------------------------------------------------
	// class ROctreeItem
	ROctreeItem::ROctreeItem(void)
		:m_pPrev(NULL),m_pNext(NULL),m_pOtreeNode(NULL),m_pSceneNode(NULL),m_State(CS_NONE),m_DeterminantPlaneIndex(0)
	{
	}

	ROctreeItem::~ROctreeItem(void)
	{
	}

	bool	ROctreeItem::FitsInOctreeNode(ROctreeNode* pOctreeNode)
	{
		return m_pSceneNode->GetSphere().FitsIn(pOctreeNode->m_X,
												pOctreeNode->m_Y,
												pOctreeNode->m_Z,
												pOctreeNode->m_Radius);
	}

	//----------------------------------------------------------------------
	// class ROctreeNode
	ROctreeNode*	ROctreeNode::Create(ROctreeNode* pParent, float x, float y,float z, float radius,int d)
	{
		return new ROctreeNode(pParent,x,y,z,radius,d);
	}		

	void	ROctreeNode::Release(ROctreeNode* pNode)
	{
		//자식노드정보 해제
		for(int i=0;i<O_HEIGHT;i++)
		{
			for(int j=0;j<O_WIDTH;j++)
			{
				if(NULL != pNode->m_pChildren[i][j])	Release(pNode->m_pChildren[i][j]);
				pNode->m_pChildren[i][j] = NULL;
			}
		}

		//내노드 해제
		delete pNode;
	}

	ROctreeNode::ROctreeNode(ROctreeNode* pParent, float x, float y,float z, float radius, int d)
		:m_pParent(pParent),m_X(x),m_Y(y),m_Z(z),m_Radius(radius),m_Depth(d),m_ItemsCount(0),m_pItemsChain(NULL),m_DeterminantPlaneIndex(0)
		,m_State(CS_PARTIAL)
	{
		for(int i=0;i<O_HEIGHT;i++)
			for(int j=0;j<O_WIDTH;j++)
				m_pChildren[i][j] = NULL;
	}

	ROctreeNode::~ROctreeNode(void)
	{
		//체인정보 해제
		ROctreeItem* pItem = m_pItemsChain;
		while( pItem )
		{
			ROctreeItem* pRemoveItem = pItem;
			pItem = pItem->m_pNext;
			delete pRemoveItem;
		}
	}

	bool ROctreeNode::IsEmpty( void )
	{
		if( 0 != m_ItemsCount )	return false;

		for(int i=0;i<O_HEIGHT;i++)
			for(int j=0;j<O_WIDTH;j++)
				if( NULL != m_pChildren[i][j] )	return false;

		return true;
	}

	void ROctreeNode::DetachFromParent( void )
	{
		if( NULL == m_pParent )	return;
		for(int i=0;i<O_HEIGHT;i++)
		{
			for(int j=0;j<O_WIDTH;j++)
			{
				if( this == m_pParent->m_pChildren[i][j] )	
				{
					m_pParent->m_pChildren[i][j] = NULL;
					m_pParent = NULL;

					return;
				}
			}
		}
	}

	void ROctreeNode::Cull_M( const RPlane* pPlanes,int planesNum,CULL_STATE state,ROctreeCallBack *pCallBack,int Masking )
	{
		if ( state == CS_PARTIAL )
		{
			RSphere sphere(RVector(m_X,m_Y,m_Z),m_Radius);
			state = sphere.PlanesTest_M(pPlanes,planesNum,Masking);
		}

		switch(state)
		{
		case CS_PARTIAL:
			{
				CallBackItems_M(pPlanes,planesNum,CS_PARTIAL,pCallBack,Masking);
			}break;

		case CS_INSIDE:
			{
				CallBackItems_M(pPlanes,planesNum,CS_INSIDE,pCallBack,Masking);
			}break;

		case CS_OUTSIDE:
			{
				return;
			}break;
		}

		for(int i=0;i<O_HEIGHT;i++)
			for(int j=0;j<O_WIDTH;j++)
				if( m_pChildren[i][j] )	m_pChildren[i][j]->Cull_M(pPlanes,planesNum,state,pCallBack,Masking);
	}

	void ROctreeNode::CallBackItems_M( const RPlane* pPlanes,int planesNum,CULL_STATE state,ROctreeCallBack *pCallBack,int Masking )
	{
		ROctreeItem *pItem = m_pItemsChain;
		while( pItem )
		{
			if (state == CS_PARTIAL)
			{
				CULL_STATE testState = pItem->m_pSceneNode->GetSphere().PlanesTest_M(pPlanes,planesNum,Masking);
				pItem->SetState(testState);
			}
			else
			{
				pItem->SetState(state);
			}

			pCallBack->CallBack_Cull(pPlanes,planesNum,pItem,pItem->GetState());
			pItem = pItem->m_pNext;
		}
	}

	void ROctreeNode::CullC_MC( const RPlane* pPlanes,int planesNum,CULL_STATE state,ROctreeCallBack *pCallBack,int Masking )
	{
		if ( state == CS_PARTIAL )
		{
			RSphere sphere(RVector(m_X,m_Y,m_Z),m_Radius);
			state = sphere.PlanesTest_MC(pPlanes,planesNum,Masking,m_DeterminantPlaneIndex);
		}

		switch(state)
		{
		case CS_PARTIAL:
			{
				m_State = state;
				CallBackItemsC_MC(pPlanes,planesNum,state,pCallBack,Masking);
			}break;

		case CS_INSIDE:
			{
				if( CS_INSIDE == m_State )	return;
				m_State = state;
				CallBackItemsC_MC(pPlanes,planesNum,state,pCallBack,Masking);
			}break;

		case CS_OUTSIDE:
			{
				if( CS_OUTSIDE == m_State )	return;
				m_State = state;
				CallBackItemsC_MC(pPlanes,planesNum,state,pCallBack,Masking);
			}break;
		}

		for(int i=0;i<O_HEIGHT;i++)
			for(int j=0;j<O_WIDTH;j++)
				if( m_pChildren[i][j] )	m_pChildren[i][j]->CullC_MC(pPlanes,planesNum,state,pCallBack,Masking);
	}

	void ROctreeNode::CallBackItemsC_MC( const RPlane* pPlanes,int planesNum,CULL_STATE state,ROctreeCallBack *pCallBack,int Masking )
	{
		ROctreeItem *pItem = m_pItemsChain;
		while( pItem )
		{
			if (state == CS_PARTIAL)
			{
				int TempMasking = Masking;
				CULL_STATE testState = pItem->m_pSceneNode->GetSphere().PlanesTest_MC(pPlanes,planesNum,TempMasking,pItem->m_DeterminantPlaneIndex);
				if( pItem->GetState() != testState )
				{
					CULL_STATE before_state		= pItem->GetState();
					CULL_STATE current_state	= testState;
					pItem->SetState(testState);
					CheckStateChangeAndCallBack(pPlanes,planesNum,pItem,before_state,current_state,pCallBack);
				}
			}
			else
			{
				if( pItem->GetState() != state )
				{
					CULL_STATE before_state		= pItem->GetState();
					CULL_STATE current_state	= state;

					pItem->SetState(state);
					CheckStateChangeAndCallBack(pPlanes,planesNum,pItem,before_state,current_state,pCallBack);
				}
			}
			pItem = pItem->m_pNext;
		}
	}

	//----------------------------------------------------------------------
	// class ROctreeNode
	ROctreeManager::ROctreeManager(ROctreeCallBack *pCallBack)
		:m_WorldHalfRadius(100000.0f),m_MaxDepth(6),m_LooseK(2.0f),m_ItemsCount(0)
	{
		_ASSERT( NULL != pCallBack );
		m_pCallBack = pCallBack;
		m_pRootNode = ROctreeNode::Create(NULL,0,-2000,0,m_LooseK * m_WorldHalfRadius,0);
	}

	ROctreeManager::~ROctreeManager(void)
	{
		ROctreeNode::Release(m_pRootNode);
		m_pCallBack = NULL;
	}


	int	ROctreeManager::Insert(ROctreeNode* pOctreeNode,ROctreeItem *pItem)
	{
		int dx,dy,dz;

		if (pOctreeNode->m_Depth + 1 < m_MaxDepth ) 
		{
			float	LoosedRadius		= (m_LooseK * m_WorldHalfRadius) / ( 1 << pOctreeNode->m_Depth );	//이 옥트리 노드의 loosed 반지름
			float	LoosedHalfRadius	= LoosedRadius / 2;											
			float	offset				=	(m_WorldHalfRadius / (1 << pOctreeNode->m_Depth )) / 2;

			dx = pItem->X() < pOctreeNode->m_X ? 0 : 1;
			dy = pItem->Y() < pOctreeNode->m_Y ? 0 : 1;
			dz = pItem->Z() < pOctreeNode->m_Z ? 0 : 1;

			float	cx = pOctreeNode->m_X +  ( (dx == 0) ? -offset : offset );
			float	cy = pOctreeNode->m_Y +  ( (dy == 0) ? -offset : offset );
			float	cz = pOctreeNode->m_Z +  ( (dz == 0) ? -offset : offset );

			if( pItem->m_pSceneNode->GetSphere().FitsIn(cx,cy,cz,LoosedHalfRadius) )
			{//씬노드가 이 옥트리의 자식옥트리의 크기에 들어갈수 있을때
				if( NULL == pOctreeNode->m_pChildren[dz][dy*2+dx] )
				{
					pOctreeNode->m_pChildren[dz][dy*2+dx] = ROctreeNode::Create(pOctreeNode,cx,cy,cz,LoosedHalfRadius,pOctreeNode->m_Depth+1);
					pOctreeNode->m_pChildren[dz][dy*2+dx]->m_State = pOctreeNode->m_State;
				}
				return Insert(pOctreeNode->m_pChildren[dz][dy*2+dx], pItem);
			}
		}

		//옥트리노드 갱신
		if( pOctreeNode->m_pItemsChain ) pOctreeNode->m_pItemsChain->m_pPrev = pItem;
		pItem->m_pNext = pOctreeNode->m_pItemsChain;	//체인갱신
		pOctreeNode->m_pItemsChain = pItem;

		pOctreeNode->m_ItemsCount++;					//갯수갱신

		//아이템 갱신
		pItem->m_pOtreeNode = pOctreeNode;
		pItem->SetState(pOctreeNode->m_State);

		//옥트리 갱신
		m_ItemsCount++;	//갯수갱신

		
		
		return pOctreeNode->m_Depth;

	}




	void	ROctreeManager::Detach(ROctreeItem *pItem)
	{
		_ASSERT( NULL != pItem->m_pOtreeNode);
		_ASSERT( NULL != pItem->m_pOtreeNode->m_pItemsChain);

		//옥트리노드 갱신
		pItem->m_pOtreeNode->m_ItemsCount--;

		//체인갱신
		if( pItem->m_pPrev && pItem->m_pNext )
		{
			pItem->m_pPrev->m_pNext = pItem->m_pNext;
			pItem->m_pNext->m_pPrev = pItem->m_pPrev;
		}
		else if( !pItem->m_pPrev && pItem->m_pNext )
		{
			pItem->m_pNext->m_pPrev = NULL;
			pItem->m_pOtreeNode->m_pItemsChain = pItem->m_pNext;
		}
		else if( pItem->m_pPrev && !pItem->m_pNext )
		{
			pItem->m_pPrev->m_pNext = NULL;
		}
		else if( !pItem->m_pPrev && !pItem->m_pNext )
		{
			pItem->m_pOtreeNode->m_pItemsChain = NULL;
		}

		//옥트리갱신
		m_ItemsCount--;	//갯수

		//item의 체인정보 초기화
		pItem->m_pNext = pItem->m_pPrev = NULL;
		pItem->m_pOtreeNode = NULL;
	}

	void	ROctreeManager::Remove(ROctreeItem *pItem)
	{
		ROctreeNode *pOctreeNode = pItem->m_pOtreeNode;

		Detach(pItem);
	#ifdef USE_CC
		CheckStateChangeAndCallBack(NULL,0,pItem,pItem->GetState(),CS_OUTSIDE,m_pCallBack);
	#endif
		delete pItem;
		DestroyEmptyNodeRecursively(pOctreeNode); //노드지울수 있으면 지우기
	}

	ROctreeItem*	ROctreeManager::Add(RSceneNode* pSceneNode)
	{
		ROctreeItem* pItem = new ROctreeItem;
		pItem->m_pSceneNode = pSceneNode;

	#ifdef USE_CC
		CULL_STATE before_state		= pItem->GetState();
		Insert(m_pRootNode,pItem);
		CULL_STATE current_state	= pItem->GetState();
		CheckStateChangeAndCallBack(NULL,0,pItem,before_state,current_state,m_pCallBack);
	#else
		Insert(m_pRootNode,pItem);
	#endif

		return pItem;
	}

	void	ROctreeManager::Move(ROctreeItem *pItem,RSphere &Sphere)
	{
		pItem->m_pSceneNode->SetSphere(Sphere);
		Move(pItem);
	}

	void ROctreeManager::Move( ROctreeItem *pItem )
	{
		if( true == pItem->FitsInOctreeNode(pItem->m_pOtreeNode) )
			return;
	#ifdef USE_CC
		CULL_STATE before_state		= pItem->GetState();
	#endif

		ROctreeNode* pOctreeNode = pItem->m_pOtreeNode;
		Detach(pItem);
		Insert(m_pRootNode,pItem);
	#ifdef USE_CC
		CULL_STATE current_state	= pItem->GetState();
		CheckStateChangeAndCallBack(NULL,0,pItem,before_state,current_state,m_pCallBack);
	#endif
		DestroyEmptyNodeRecursively(pOctreeNode);
	}

	void ROctreeManager::DestroyEmptyNodeRecursively( ROctreeNode* pOctreeNode )
	{
		while(pOctreeNode)
		{
			if( 0 == pOctreeNode->m_Depth ) break;	//루트노드는 못지움
			ROctreeNode* pCurrentNode = pOctreeNode;
			pOctreeNode = pOctreeNode->m_pParent;
			if( pCurrentNode->IsEmpty() )
			{
				pCurrentNode->DetachFromParent();
				ROctreeNode::Release(pCurrentNode);
			}
			else
			{
				break;
			}
		}
	}

	void ROctreeManager::Cull( const RPlane* pPlanes,int planesNum,ROctreeCallBack *pCallBack)
	{
		//평면갯수만큼 partial이라고 마스킹해줌
		int Masking = 0;
		for(int i=0,k=1;i<planesNum;i++,k+=k)
		{
			Masking |= k;
		}

		m_pRootNode->Cull_M(pPlanes,planesNum,CS_PARTIAL,pCallBack,Masking);
	}


	void ROctreeManager::Cull_MC( const RPlane* pPlanes,int planesNum,ROctreeCallBack *pCallBack )
	{
		RPFC_THISFUNC;
		if( NULL == m_pCallBack)	m_pCallBack = pCallBack;
		else						_ASSERT(pCallBack ==  m_pCallBack);

		//평면갯수만큼 partial이라고 마스킹해줌
		int Masking = 0;
		for(int i=0,k=1;i<planesNum;i++,k+=k)
		{
			Masking |= k;
		}
	#ifdef USE_CC
		m_pRootNode->CullC_MC(pPlanes,planesNum,CS_PARTIAL,pCallBack,Masking);
	#else
		m_pRootNode->Cull_MC(pPlanes,planesNum,CS_PARTIAL,pCallBack,Masking);
	#endif
	}


	void CheckStateChangeAndCallBack( const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state,ROctreeCallBack *pCallBack )
	{
		pCallBack->CallBack_Cull(pPlanes,planesNum,pItem,before_state,current_state);
	}

	CULL_STATE ROctreeCallBack::CheckStateChangeForCoherentCulling( CULL_STATE before_state,CULL_STATE current_state )
	{
		CULL_STATE modified_state = CS_NONE;

				if( before_state == CS_NONE		&& current_state == CS_OUTSIDE	)	;
		else	if( before_state == CS_NONE		&& current_state == CS_PARTIAL	)	modified_state = CS_PARTIAL;
		else	if( before_state == CS_NONE		&& current_state == CS_INSIDE	)	modified_state = CS_INSIDE;
		else	if( before_state == CS_NONE		&& current_state == CS_NONE		)	_ASSERT(false);

		else	if( before_state == CS_OUTSIDE	&& current_state == CS_OUTSIDE	)	;
		else	if( before_state == CS_PARTIAL	&& current_state == CS_OUTSIDE	)	modified_state = CS_OUTSIDE;
		else	if( before_state == CS_INSIDE	&& current_state == CS_OUTSIDE	)	modified_state = CS_OUTSIDE;

		else	if( before_state == CS_OUTSIDE	&& current_state == CS_PARTIAL	)	modified_state = CS_PARTIAL;
		else	if( before_state == CS_PARTIAL	&& current_state == CS_PARTIAL	)	;
		else	if( before_state == CS_INSIDE	&& current_state == CS_PARTIAL	)	;

		else	if( before_state == CS_OUTSIDE	&& current_state == CS_INSIDE	)	modified_state = CS_INSIDE;
		else	if( before_state == CS_PARTIAL	&& current_state == CS_INSIDE	)	;
		else	if( before_state == CS_INSIDE	&& current_state == CS_INSIDE	)	;

		else	_ASSERT(false);

		return modified_state;
	}

	CULL_STATE ROctreeCallBack::CheckStateChangeForOcclusionCoherentCulling( CULL_STATE before_state,CULL_STATE current_state )
	{
		CULL_STATE modified_state = CS_NONE;

				if( before_state == CS_NONE		&& current_state == CS_OUTSIDE	)	;
		else	if( before_state == CS_NONE		&& current_state == CS_PARTIAL	)	;
		else	if( before_state == CS_NONE		&& current_state == CS_INSIDE	)	modified_state = CS_INSIDE;
		else	if( before_state == CS_NONE		&& current_state == CS_NONE		)	_ASSERT(false);

		else	if( before_state == CS_OUTSIDE	&& current_state == CS_OUTSIDE	)	;
		else	if( before_state == CS_PARTIAL	&& current_state == CS_OUTSIDE	)	;
		else	if( before_state == CS_INSIDE	&& current_state == CS_OUTSIDE	)	modified_state = CS_OUTSIDE;

		else	if( before_state == CS_OUTSIDE	&& current_state == CS_PARTIAL	)	;
		else	if( before_state == CS_PARTIAL	&& current_state == CS_PARTIAL	)	;
		else	if( before_state == CS_INSIDE	&& current_state == CS_PARTIAL	)	modified_state = CS_PARTIAL;

		else	if( before_state == CS_OUTSIDE	&& current_state == CS_INSIDE	)	modified_state = CS_INSIDE;
		else	if( before_state == CS_PARTIAL	&& current_state == CS_INSIDE	)	modified_state = CS_INSIDE;
		else	if( before_state == CS_INSIDE	&& current_state == CS_INSIDE	)	;

		else	_ASSERT(false);

		return modified_state;
	}
}
