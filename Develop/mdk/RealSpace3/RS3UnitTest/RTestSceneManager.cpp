#include "stdafx.h"
#include "RWorldSceneNode.h"
#include "RSceneManager.h"
#include "RDeviceNull.h"

using namespace rs3;

class MockWorldSceneNodeWithUpdateOderDescription : public RWorldSceneNode
{
public:
	MockWorldSceneNodeWithUpdateOderDescription(void)
	{
		m_UpdateOder = UPDATE_NONE;
		m_bValidUpdateOder = true;
	}
	virtual	~MockWorldSceneNodeWithUpdateOderDescription(void)
	{
	}

public:
	virtual	void							UpdateAutoUpdateSceneNode() override
	{
		m_bValidUpdateOder = (UPDATE_NONE==m_UpdateOder);
		m_UpdateOder = UPDATE_AUTO;
	}
	virtual	void							UpdateBoundingVolumeAllSceneNodes() override
	{
		m_bValidUpdateOder = (UPDATE_AUTO==m_UpdateOder);
		m_UpdateOder = UPDATE_BOUNDINGVOLUME;
	}

	bool	IsValidUpdateOrder(void)	{	return m_bValidUpdateOder;	}

private:
	enum	UPDATE_ORDER
	{
		UPDATE_NONE = 0,
		UPDATE_AUTO,
		UPDATE_BOUNDINGVOLUME
	};
	UPDATE_ORDER	m_UpdateOder;
	bool			m_bValidUpdateOder;
};

class StubSceneManager : public RSceneManager
{
public:
	RWorldSceneNode* AddRootSceneNode(RWorldSceneNode* pWorldSceneNode)
	{
		m_rootSceneNodeMap.insert(pWorldSceneNode);
		return pWorldSceneNode;
	}
};

class FixtureSceneManager : public testing::Test
{
protected:
	FixtureSceneManager(void)
	{
		// 디바이스 생성
		RDeviceNull* pDevice = new RDeviceNull;
		bool bRet = pDevice->Create( NULL );
		_ASSERT( bRet == true );

		REngine::RegisterDevice( pDevice );

		// 씬 매니저 생성
		m_pSceneManager = new StubSceneManager;
		REngine::RegisterSceneManager( m_pSceneManager );
	}

	virtual ~FixtureSceneManager(void)
	{
		// 씬매니저
		m_pSceneManager->Clear();
		m_pSceneManager->Destroy();
		SAFE_DELETE(m_pSceneManager);
		REngine::RegisterSceneManager( NULL );

		// 디바이스
		RDevice* pDevice = REngine::GetDevicePtr();
		_ASSERT( pDevice != NULL );
		pDevice->Destroy();
		delete pDevice;
		REngine::RegisterDevice(NULL);
	}

protected:
	StubSceneManager*	m_pSceneManager;
};

TEST_F( FixtureSceneManager, TestUpdate_AutoUpdateShouldBeFollowedByUpdateBoundingVolume )
{
	// 설치
	MockWorldSceneNodeWithUpdateOderDescription* pWorld = new MockWorldSceneNodeWithUpdateOderDescription;
	m_pSceneManager->AddRootSceneNode(pWorld);
	
	// 실행
	m_pSceneManager->UpdateTimerCustom(1);
	m_pSceneManager->Update();

	// 검증
	EXPECT_TRUE( pWorld->IsValidUpdateOrder() );

}