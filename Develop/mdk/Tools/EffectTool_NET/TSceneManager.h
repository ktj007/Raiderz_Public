#pragma once

#include "RSceneManager.h"

namespace rs3 {
	class REffectInstance;
}

using namespace rs3;

class TSceneManager : public RSceneManager
{
public:
	TSceneManager();

	virtual RSceneNode* CreateSceneNode(const char *szNodeName);
};
