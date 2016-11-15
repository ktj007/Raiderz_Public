#pragma once

#include "ZGlobal.h"


/// 게임 전체에서 사용하는 매니저 모음
class ZServerManagers : public ZGlobalManager
{
public:
	////
	ZServerManagers();
	virtual ~ZServerManagers();
	void Create();
	void Destroy();
};
