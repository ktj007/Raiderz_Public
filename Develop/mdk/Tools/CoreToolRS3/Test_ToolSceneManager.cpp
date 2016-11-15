#include "stdafx.h"
#include "RTerrain.h"
#include "UnitTest++.h"

SUITE(SceneManager)
{
	TEST(TerrainBaseMapName)
	{
		using namespace rs3;

		// rule test
		RSceneManager *pSceneManager = REngine::GetSceneManagerPtr();
		RTerrain *pTerrain = static_cast<RTerrain*>(pSceneManager->CreateSceneNode(RSID_TERRAINSCENENODE));
		CHECK(pTerrain);

		const std::string &resourceName = pTerrain->GetName();
		CHECK(resourceName.length() == 0);

		if ( pTerrain )
		{
			delete pTerrain;
			pTerrain = NULL;
		}
	}
}