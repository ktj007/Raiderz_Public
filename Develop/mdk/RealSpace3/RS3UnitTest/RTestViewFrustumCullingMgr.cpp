#include "stdafx.h"
#include "RTypes.h"
#include "RS3UnitTestUtility.h"
#include "RDevice.h" 
#include "RSceneNode.h"
#include "RWorldSceneNode.h"
#include "RActor.h"
#include "ROccluderSceneNode.h"
#include "RViewFrustumCullingMgr.h"

using namespace rs3;

class TestViewFrustumCullingMgr : public NullDeviceFixture
{
};

TEST_F( TestViewFrustumCullingMgr, DoesExistOcclusionSceneNodeInCullingList )
{
	RViewFrustumCullingMgr		ViewFrustumCullingMgr;
	RWorldSceneNode	WorldSceneNode;

	RSceneNode* pSceneNodes[] =
	{
		new RSceneNode,
		new ROccluderSceneNode,
		new RActor,
	};

	int NUM = sizeof(pSceneNodes) / sizeof(RSceneNode*);
	for(int i=0;i<NUM;i++)
	{
		RSceneNode& SceneNode = *pSceneNodes[i];

		WorldSceneNode.AddChild(&SceneNode);
		ViewFrustumCullingMgr.Cull(&WorldSceneNode,NULL,0);
		SCENE_SET* pCullingList = ViewFrustumCullingMgr.GetCulledList(SceneNode.GetUsingPass());
		EXPECT_TRUE( pCullingList->end() != pCullingList->find(&SceneNode) );
		WorldSceneNode.RemoveChild(&SceneNode);	

		SAFE_DELETE( pSceneNodes[i] );
	}
}