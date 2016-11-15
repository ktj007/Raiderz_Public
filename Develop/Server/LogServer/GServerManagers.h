#pragma once

#include "GGlobal.h"


/// 게임 전체에서 사용하는 매니저 모음
class GServerManagers : public GGlobalManager
{
public:
	////
	GServerManagers();
	virtual ~GServerManagers();
	void Create();
	void Destroy();
};
