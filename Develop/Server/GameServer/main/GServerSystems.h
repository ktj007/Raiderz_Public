#pragma once

#include "GGlobal.h"

/// 게임 전체에서 사용하는 시스템 모음
class GServerSystems : public GGlobalSystem
{
public:
	////
	GServerSystems();
	virtual ~GServerSystems();
	virtual void Create();
	virtual void Destroy();
};
