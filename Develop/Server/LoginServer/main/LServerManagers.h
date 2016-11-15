#pragma once

#include "LGlobal.h"


/// 게임 전체에서 사용하는 매니저 모음
class LServerManagers : public LGlobalManager
{
public:
	////
	LServerManagers();
	virtual ~LServerManagers();
	void Create();
	void Destroy();
};
