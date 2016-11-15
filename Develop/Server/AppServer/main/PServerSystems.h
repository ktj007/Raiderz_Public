#pragma once

#include "PGlobal.h"

/// 게임 전체에서 사용하는 시스템 모음
class PServerSystems : public PGlobalSystem
{
public:
	////
	PServerSystems();
	virtual ~PServerSystems();
	virtual void Create();
	virtual void Destroy();
};
