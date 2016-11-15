#include "stdafx.h"
#include "RTypes.h"
#include "RS3UnitTestUtility.h"
#include "RDevice.h" 
#include "RSceneNode.h"
#include "RCullingMgr.h"

using namespace rs3;

class TestOcclusionCullingFixture : public ::testing::Test
{
private:
	virtual void SetUp()
	{
		m_pNullDeviceEngineCreater = new NullDeviceEngineCreater;
		ASSERT_TRUE( NULL != m_pNullDeviceEngineCreater );
	}
	virtual void TearDown()
	{
		delete m_pNullDeviceEngineCreater;
	}

private:
	NullDeviceEngineCreater* m_pNullDeviceEngineCreater;


};

TEST( TestRCullingMgr, DoesExistOcclusionSceneNodeInCullingList )
{
	
}