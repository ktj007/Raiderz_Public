#include "stdafx.h"
#include "MockWorld.h"
#include "RTerrain.h"
#include "RSceneManager.h"
#include "XCollisionTree.h"

class MockTerrain : public RTerrain
{
public:
	MockTerrain()
	{
		
	}

	virtual ~MockTerrain()
	{
		
	}
	
	void InitForMock(int nWidth, int nHeight, float fXSize, float fYSize)
	{
		_ASSERT(m_pHeightField==NULL);
		int nPatchSize = 4;
		int nLodLevel = 2;

		m_pHeightField = new RHeightField();
		if ( m_pHeightField->Create(nWidth, nHeight, MVector2(fXSize, fYSize)) == false )
		{
			SAFE_DELETE(m_pHeightField);
			_ASSERT(0);
		}

		int nMapSize = m_pHeightField->GetWidthMap() * m_pHeightField->GetHeightMap();
		int nMapWidth = m_pHeightField->GetWidthMap();

		m_iCount = nMapWidth/nPatchSize;
		m_iPatchNum = m_iCount*m_iCount;

		bool bRet = CreateNormalFieldMap(nWidth, nHeight, fXSize, fYSize);
		_ASSERT(bRet);

		bRet = CreateDisplacementmapPatch(fXSize, fYSize, nPatchSize, nLodLevel);
		_ASSERT(bRet);

		SetUsable(true);
	}

	// RTerrain의 OnUpdate에서 GrassSystem과 TreeSystem을 만든다.
	// Mock에서는 만들지 않는다.
	virtual void OnUpdate(){}
};

class MockCollisionTree : public XCollisionTree
{
public:
	void InitForMock( RTerrain* pTerrain )
	{
		SetTerrain(pTerrain);
	}
};

MockWorld::MockWorld()
{
	//_ASSERT(RTerrain::IsEmptyPipeProcessor() == true);
	//RTerrain::CreatePipeProcessor(false);
}
MockWorld::~MockWorld()
{
	//_ASSERT(RTerrain::IsEmptyPipeProcessor() == false);
	//RTerrain::DestroyPipeProcessor();
}

bool MockWorld::Init(XWorldInitParam& worldInitParam)
{
	_ASSERT( worldInitParam.nFieldID == -1);
	_ASSERT( worldInitParam.strZoneName.empty() );

	// Mock Field Info 만들기
	_ASSERT(m_pInfo == NULL);
	m_pInfo = new XFieldInfo();
	m_pInfo->m_nFieldID		= 8003;
	m_pInfo->m_strFieldName	= L"BirdTown";
	m_pInfo->m_strFieldFile	= L"BirdTown";
	m_pInfo->m_nZoneID		= 8003;

	// Mock Terrain 만들기(2^n+1)
	_ASSERT(m_pTerrain == NULL);
	MockTerrain* pMockTerrain = new MockTerrain();
	pMockTerrain->InitForMock(17, 17, 10000.f, 10000.f);
	REngine::GetSceneManager().AddSceneNode(pMockTerrain, true);

	m_pTerrain = pMockTerrain;

	// Mock Zone 만들기
	if (m_pZone)
	{
		delete m_pZone;
	}

	MockZone* pMockZone = new MockZone;
	pMockZone->InitForTest(pMockTerrain);
	m_pZone = pMockZone;

	return true;
}

void MockZone::InitForTest(RTerrain* pTerrain)
{
	if (m_pCollisionTree)
	{
		delete m_pCollisionTree;
	}

	MockCollisionTree* pMockCollisionTree = new MockCollisionTree;
	pMockCollisionTree->InitForMock(pTerrain);
	m_pCollisionTree = pMockCollisionTree;
}