#include "stdafx.h"
#include "RSceneNodeID.h"
#include "RSceneNodeInstance.h"
#include "RWorldSceneNode.h"
#include "RCameraSceneNode.h"
#include "RCameraFPSSceneNode.h"
#include "RCameraArcBallSceneNode.h"
#include "RPointLightSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "RStaticMeshSceneNode.h"
#include "RTextSceneNode.h"
#include "REmitter.h"
#include "RTerrain.h"
#include "RActor.h"
#include "REffectActor.h"
#include "RDirectionalLightSceneNode.h"
#include "RSkyManager.h"
#include "RWaterSceneNode.h"
#include "REffectSceneNode.h"
#include "RDecalProjectionSceneNode.h"
#include "RDecalPolygonSceneNode.h"
#include "RSceneNodeControllerFactory.h"
#include "RTrail.h"
#include "REffectInstance.h"
#include "RClothLocalActorNode.h"
#include "RClothWorldActorNode.h"
#include "ROccluderSceneNode.h"

#include "RUtil.h"	// TODO: 빼자.. splitpath때문

#include "RTreeSceneNode.h"

namespace rs3 {

RSceneNode *RSceneManager::CreateSceneNode(const char *szNodeName)
{
	RSceneNode *pNewNode = NULL;
	if(stricmp(szNodeName,RSID_SCENEINSTANCE)==0)
		pNewNode = new RSceneNodeInstance;
	else if(stricmp(szNodeName,RSID_CAMERASCENENODE)==0)
		pNewNode = new RCameraSceneNode;
	else if(stricmp(szNodeName,RSID_CAMERAFPSSCENENODE)==0)
		pNewNode = new RCameraFPSSceneNode;
	else if(stricmp(szNodeName,RSID_CAMERAARCBALLSCENENODE)==0)
		pNewNode = new RCameraArcBallSceneNode();
	else if(stricmp(szNodeName,RSID_LIGHTSCENENODE)==0)
		pNewNode = new RPointLightSceneNode;
	else if(stricmp(szNodeName,RSID_SPOTLIGHTSCENENODE)==0)
		pNewNode = new RSpotLightSceneNode;
	else if(stricmp(szNodeName,RSID_STATICMESHSCENENODE)==0)
		pNewNode = new RStaticMeshSceneNode;
	else if(stricmp(szNodeName,RSID_TEXTSCENENODE)==0)
		pNewNode = new RTextSceneNode;
	else if(stricmp(szNodeName,RSID_TERRAINSCENENODE)==0)
		pNewNode = new RTerrain;
	else if(stricmp(szNodeName,RSID_ACTOR)==0)
		pNewNode = new RActorMLeaf();
	else if(stricmp(szNodeName,RSID_EFFECTACTOR)==0)
		pNewNode = new REffectActorMLeaf;
	else if(stricmp(szNodeName,RSID_OCCLUDER)==0)
		pNewNode = new ROccluderSceneNode();
	else if(stricmp(szNodeName,RSID_DIRECTIONALLIGHTSCENENODE)==0)
		pNewNode = new RDirectionalLightSceneNode;
	else if(stricmp(szNodeName,RSID_SUBLIGHTSCENENODE)==0)
		pNewNode = new RSubLightSceneNode;
	else if(stricmp(szNodeName,RSID_SKY)==0)
		pNewNode = new RSkyManager();
	else if(stricmp(szNodeName,RSID_TREE)==0)
		pNewNode = new RTreeSceneNode();
	else if(stricmp(szNodeName,RSID_WATERPLANE) == 0)
		pNewNode = new RWaterPlane;
	else if (stricmp(szNodeName,RSID_EFFECT) == 0)
		pNewNode = new REffectSceneNode;
	else if (stricmp(szNodeName,RSID_EMITTER) == 0)
		pNewNode = new REmitter;
	else if (stricmp(szNodeName,RSID_TRAIL) == 0)
		pNewNode = new RTrail;
	else if (stricmp(szNodeName,RSID_EFFECTINSTANCE) == 0)
		pNewNode = new REffectInstanceSceneNode;
	else if (stricmp(szNodeName, RSID_DECAL_PROJECTION) == 0)
		pNewNode = new RDecalProjectionSceneNode;
	else if (stricmp(szNodeName, RSID_DECAL_POLYGON) == 0)
		pNewNode = new RDecalPolygonSceneNode;
	else if(stricmp(szNodeName,RSID_CLOTH)==0)
	{
		//pNewNode = new RClothNode;
		//pNewNode = new RActorNodeCPUCloth;
		_ASSERT(false);	//이제 옷은 여기서 안만듬.왜냐하면 RClothLocalActorNode,RClothWorldActorNode 2가지 종류의 옷이 있는데,
						//단순 RSID_CLOTH가지고는 이2개중 어느 옷인지 알 수 없음.
						//원래는 지워야하는 코드이지만,혹시 몰라서 어썰트 걸어놓음.			
						//옷만들러 여기 안들어온다는 확신이 있기 전까지, 이 코드 지우지 마세요.
	}

	/*if(pNewNode)
	{
		pNewNode->m_pSceneManager = this;
	}*/

	return pNewNode;
}

// Controller Factory
RSceneNodeController* RSceneManager::CreateSceneNodeController(const char* _pSzControllerName)
{
	RSceneNodeControllerFactory factory;
	return factory.CreateSceneNodeController(_pSzControllerName);
}


bool	RSceneManager::LoadSceneNodeFromRootXMLFile(RSceneNode *pSceneNode, const char* szRootXMLFileName, bool bBackgroundCreation,RZoneLoadingQuery* pZoneLoadingQuery)
{
	_ASSERT( pSceneNode );
	_ASSERT( szRootXMLFileName );

	MXml xml;
	if( false == xml.LoadFile(szRootXMLFileName) )	return false;
	
	//이 부분은 XML포맷( Root XML파일에 대한 포맷 ) 에 대한 룰
	MXmlElement* pXmlElement = xml.Doc()->FirstChildElement("ROOT");	_ASSERT( NULL != pXmlElement );

	return LoadXML( pSceneNode, *pXmlElement, bBackgroundCreation,pZoneLoadingQuery);
}

bool	RSceneManager::LoadWorldSceneNodeFromRootXMLFileWithLoadingProgress(RSceneNode *pSceneNode, const char* szRootXMLFileName, bool bBackgroundCreation)
{
	RWorldSceneNode* pWorldSceneNode = MDynamicCast( RWorldSceneNode, pSceneNode );
	_ASSERT( pWorldSceneNode );
	pWorldSceneNode->SetXMLFileName(szRootXMLFileName);
	
	m_pCurrentResourceLoadingGroup = pWorldSceneNode;
	bool bRet = LoadSceneNodeFromRootXMLFile(pSceneNode,szRootXMLFileName,bBackgroundCreation);
	m_pCurrentResourceLoadingGroup->StartLoading( bBackgroundCreation );
	m_pCurrentResourceLoadingGroup = NULL;
	return bRet;
}	

bool	RSceneManager::LoadWorldSceneNodeFromRootXMLFile(RSceneNode *pSceneNode, const char* szRootXMLFileName, bool bBackgroundCreation)
{
	RWorldSceneNode* pWorldSceneNode = MDynamicCast( RWorldSceneNode, pSceneNode );
	_ASSERT( pWorldSceneNode );
	pWorldSceneNode->SetXMLFileName(szRootXMLFileName);

	return LoadSceneNodeFromRootXMLFile(pSceneNode,szRootXMLFileName,bBackgroundCreation);
}

bool	RSceneManager::LoadSceneNodeInstanceFromRootXMLFile(RSceneNode *pSceneNode, const char* szRootXMLFileName, bool bBackgroundCreation)
{
	return LoadSceneNodeFromRootXMLFile(pSceneNode,szRootXMLFileName,bBackgroundCreation);
}

bool	RSceneManager::LoadWorldorInstanceFromRootXMLFile(RSceneNode *pSceneNode, const char* szRootXMLFileName, bool bBackgroundCreation ,RSceneLoadingCallbackInterface* pCallback)
{
	_ASSERT( pSceneNode );
	_ASSERT( szRootXMLFileName );


	if( RWorldSceneNode* pWorldSceneNode = MDynamicCast( RWorldSceneNode, pSceneNode ) )
	{
		return LoadWorldSceneNodeFromRootXMLFile(pSceneNode,szRootXMLFileName,bBackgroundCreation);
	}
	else if( RSceneNodeInstance* pInstanceSceneNode = MDynamicCast( RSceneNodeInstance, pSceneNode ) )
	{
		return LoadSceneNodeInstanceFromRootXMLFile(pSceneNode,szRootXMLFileName,bBackgroundCreation);
	}

	_ASSERT(false);
	return false;
}

bool	RSceneManager::LoadTempEfffectWorldSceneNodeFromRootXMLFile(RSceneNode *pSceneNode, const char* szRootXMLFileName, bool bBackgroundCreation)
{
	return LoadSceneNodeFromRootXMLFile(pSceneNode,szRootXMLFileName,bBackgroundCreation);
}


}
