#pragma once

#include "SServerApplication.h"
#include "ZServerInfo.h"
#include "ZGlobal.h"
#include "ZServerSystems.h"
#include "ZServerManagers.h"
#include "MTime.h"
#include "MUID.h"
#include "MDebug.h"


class ZBaseApplication : public SServerApplication
{
protected:
	ZServerSystems	m_Systems;						///< 게임 전체에서 사용하는 시스템 모음
	ZServerManagers	m_Managers;						///< 게임 전체에서 사용하는 매니저 모음

	virtual	bool	OnCreate() override;
	virtual bool	OnUpdate(float fDelta) override;
	virtual void	OnDestroy() override;

	void StartProfiling();
	void AnalysisProfiling();

public:
	ZBaseApplication();
	virtual			~ZBaseApplication();
};