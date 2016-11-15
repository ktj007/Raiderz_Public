#include "stdafx.h"
#include "RTypes.h"
#include "RSceneNode.h"
#include "ROctreeManager.h"


/*
	보통 Cull에 테스트 붙인 내용

*/

/*	Coherent Cull에 테스트 붙인 내용

	- 아이템 한개와 한평면 컬링
	- 아이템 한개와 2평면 컬링
	- 움직인 아이템 컬링
	- 반지름이 변환 아이템 컬링
	- 평면의 위치가 변경된 후 컬링

	!미흡한면 :
				 기본적인 테스트 밖에 이루어 지지 않았음. 
				 여러 문제 될 수 있는 상황이 많으나, 
				 제작당시 버그가 난립했을때 단위테스트를 붙였으면 더욱 견고한 테스트가 완성되었을 거임
*/

using namespace rs3;

//유틸리티
static void SetSceneNodeSphere(RSceneNode *pSceneNode,float x,float y,float z,float radius)
{
	pSceneNode->SetSphere(RSphere (RVector(x,y,z),radius));
}

//---------------------------------------------------------------------[ Octree Callback ]
class TestOctreeCallBack : public ROctreeCallBack
{
public:
	virtual void CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE state)
	{
		if( state == CS_PARTIAL || state == CS_INSIDE )
		{
			//현 프레임에서는 보이니
			//추가
			m_CullingList.insert(pItem->GetSceneNode());
		}
		else if( state == CS_OUTSIDE )
		{
			//현 프레임에 안보이니 
			//추가 안해준다. 즉 아무것도 안해줌
		}
		else
		{
			ADD_FAILURE();
		}

	}

	virtual void CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state) {};

public:
	bool	ExistInCullingList(RSceneNode* pSceneNode)	
	{ 
		if(	m_CullingList.end() == m_CullingList.find(pSceneNode) )
		{
			return false;
		}
		return true;
	}

	void	ResetCullingList(void)
	{
		m_CullingList.clear();
	}

public:
	std::set<RSceneNode*>	m_CullingList;
};

class TestOctreeMangerFixture : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		m_pOctreeManger = new ROctreeManager(&m_OctreeCallBack);
	}
	virtual void TearDown()
	{
		delete m_pOctreeManger;
	}

protected:
	ROctreeManager*					m_pOctreeManger;
	TestOctreeCallBack				m_OctreeCallBack;
};

//---------------------------------------------------------------------[ Coherent Octree Callback ]
class TestCoherentOctreeCallBack : public ROctreeCallBack
{
public:
	virtual void CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE state)	{};
	virtual void CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state)
	{
		CULL_STATE modified_state = CheckStateChangeForCoherentCulling(before_state,current_state);

		if( modified_state == CS_OUTSIDE )
		{
			//전 프레임에서는 보였다가, 현 프레임에서는 안보이니
			//제거
			ASSERT_TRUE( ExistInCullingList(pItem->GetSceneNode()) );
			m_CullingList.erase(pItem->GetSceneNode());
		}
		else if( modified_state == CS_PARTIAL || modified_state == CS_INSIDE )
		{
			//전 프레임에서는 안보였다가, 현 프레임에서는 안보이니 (현 프레임에서 새로 삽입된 아이템일 수 도 있다.
			//추가
			ASSERT_FALSE( ExistInCullingList(pItem->GetSceneNode()) );
			m_CullingList.insert(pItem->GetSceneNode());
		}
		else 
		{
			ADD_FAILURE();
		}
	}
public:
	bool	ExistInCullingList(RSceneNode* pSceneNode)	
	{ 
		if(	m_CullingList.end() == m_CullingList.find(pSceneNode) )
		{
			return false;
		}
		return true;
	}

public:
	std::set<RSceneNode*>	m_CullingList;
};

class TestCoherentOctreeMangerFixture : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		m_pOctreeManger = new ROctreeManager(&m_CoherentOctreeCallBack);
	}
	virtual void TearDown()
	{
		delete m_pOctreeManger;
	}

protected:
	ROctreeManager*					m_pOctreeManger;
	TestCoherentOctreeCallBack		m_CoherentOctreeCallBack;

};


TEST_F(TestOctreeMangerFixture,Cull)
{
	RSceneNode SceneNode;	
	SetSceneNodeSphere(&SceneNode,0,0,0,1);
	m_pOctreeManger->Add(&SceneNode);

	RPlane plane(RVector(1,0,0),RVector(0,0,0));
	m_pOctreeManger->Cull(&plane,1,&m_OctreeCallBack);

	EXPECT_EQ(true,m_OctreeCallBack.ExistInCullingList(&SceneNode));
}

TEST_F(TestOctreeMangerFixture,Move)
{
	RSceneNode SceneNode;	
	SetSceneNodeSphere(&SceneNode,0,0,0,1);
	ROctreeItem* pItem = m_pOctreeManger->Add(&SceneNode);

	RPlane plane(RVector(1,0,0),RVector(0,0,0));
	m_pOctreeManger->Cull(&plane,1,&m_OctreeCallBack);
	EXPECT_EQ(true,m_OctreeCallBack.ExistInCullingList(&SceneNode));

	m_OctreeCallBack.ResetCullingList();

	m_pOctreeManger->Move(pItem,RSphere(RVector(-2,0,0),1));
	m_pOctreeManger->Cull_MC(&plane,1,&m_OctreeCallBack);
	EXPECT_EQ(false,m_OctreeCallBack.ExistInCullingList(&SceneNode));
}


TEST_F(TestCoherentOctreeMangerFixture,CoherentCull)
{
	RSceneNode SceneNode;	
	SetSceneNodeSphere(&SceneNode,0,0,0,1);
	m_pOctreeManger->Add(&SceneNode);

	RPlane plane(RVector(1,0,0),RVector(0,0,0));
	m_pOctreeManger->Cull_MC(&plane,1,&m_CoherentOctreeCallBack);

	EXPECT_EQ(true,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));
}

TEST_F(TestCoherentOctreeMangerFixture,CoherentCullByPlanes)
{
	RSceneNode SceneNode;	
	SetSceneNodeSphere(&SceneNode,0,-1,0,1);
	m_pOctreeManger->Add(&SceneNode);

	static const int PLANE_NUM = 2;
	RPlane planes[PLANE_NUM];
	planes[0].SetPlane(RVector(1,0,0),RVector(0,0,0));
	planes[1].SetPlane(RVector(0,1,0),RVector(0,1,0));
	
	m_pOctreeManger->Cull_MC(&planes[0],1,&m_CoherentOctreeCallBack);
	EXPECT_EQ(true,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));

	m_pOctreeManger->Cull_MC(planes,PLANE_NUM,&m_CoherentOctreeCallBack);
	EXPECT_EQ(false,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));
}

TEST_F(TestCoherentOctreeMangerFixture,MovingNode)
{
	RSceneNode SceneNode;	
	SetSceneNodeSphere(&SceneNode,0,0,0,1);
	ROctreeItem* pItem = m_pOctreeManger->Add(&SceneNode);

	RPlane plane(RVector(1,0,0),RVector(0,0,0));
	m_pOctreeManger->Cull_MC(&plane,1,&m_CoherentOctreeCallBack);
	EXPECT_EQ(true,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));

	//노드의 위치 변경시킨다.
	m_pOctreeManger->Move(pItem,RSphere(RVector(-2,0,0),1));
	m_pOctreeManger->Cull_MC(&plane,1,&m_CoherentOctreeCallBack);
	EXPECT_EQ(false,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));
}

TEST_F(TestCoherentOctreeMangerFixture,ResizingNodeRadius)
{
	RSceneNode SceneNode;	
	SetSceneNodeSphere(&SceneNode,0,0,0,1);
	ROctreeItem* pItem = m_pOctreeManger->Add(&SceneNode);

	RPlane plane(RVector(1,0,0),RVector(2,0,0));
	m_pOctreeManger->Cull_MC(&plane,1,&m_CoherentOctreeCallBack);
	EXPECT_EQ(false,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));

	//반지름 크기 늘려준다
	m_pOctreeManger->Move(pItem,RSphere(RVector(0,0,0),2));
	m_pOctreeManger->Cull_MC(&plane,1,&m_CoherentOctreeCallBack);
	EXPECT_EQ(true,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));

}

TEST_F(TestCoherentOctreeMangerFixture,MovingPlane)
{
	RSceneNode SceneNode;	
	SetSceneNodeSphere(&SceneNode,0,0,0,1);
	m_pOctreeManger->Add(&SceneNode);

	RPlane plane(RVector(1,0,0),RVector(0,0,0));
	m_pOctreeManger->Cull_MC(&plane,1,&m_CoherentOctreeCallBack);
	EXPECT_EQ(true,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));

	plane.SetPlane(RVector(1,0,0),RVector(0.5,0,0));
	m_pOctreeManger->Cull_MC(&plane,1,&m_CoherentOctreeCallBack);
	EXPECT_EQ(true,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));

	plane.SetPlane(RVector(1,0,0),RVector(2,0,0));
	m_pOctreeManger->Cull_MC(&plane,1,&m_CoherentOctreeCallBack);
	EXPECT_EQ(false,m_CoherentOctreeCallBack.ExistInCullingList(&SceneNode));
}

