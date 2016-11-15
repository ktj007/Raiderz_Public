#pragma once

#include "PGlobal.h"


/// 게임 전체에서 사용하는 매니저 모음
class PServerManagers : public PGlobalManager
{
public:
	////
	PServerManagers();
	virtual ~PServerManagers();
	void Create();
	void Destroy();
};
