#pragma once

#include "RSceneManager.h"

using namespace rs3;

namespace EffectTool
{

class TSceneManager : public RSceneManager
{
public:
	TSceneManager();

	virtual RSceneNode* CreateSceneNode(const char* nodeName);
};

}