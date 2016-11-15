#pragma once

#include "ZGlobal.h"

/// 게임 전체에서 사용하는 시스템 모음
class ZServerSystems : public ZGlobalSystem
{
public:
	////
	ZServerSystems();
	virtual ~ZServerSystems();
	virtual void Create();
	virtual void Destroy();
};
