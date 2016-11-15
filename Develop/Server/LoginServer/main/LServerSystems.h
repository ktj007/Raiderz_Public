#pragma once

#include "LGlobal.h"

/// 게임 전체에서 사용하는 시스템 모음
class LServerSystems : public LGlobalSystem
{
public:
	////
	LServerSystems();
	virtual ~LServerSystems();
	virtual void Create();
	virtual void Destroy();
};
