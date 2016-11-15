#include "TSceneManager.h"
#include "RSceneNodeID.h"

namespace EffectTool
{

TSceneManager::TSceneManager() 
{
}

RSceneNode* TSceneManager::CreateSceneNode(const char* nodeName)
{
	std::string strNodeName = nodeName;

	if(strNodeName == RSID_ACTOR)
		strNodeName = RSID_EFFECTACTOR;

	return RSceneManager::CreateSceneNode(strNodeName.c_str());
}

}