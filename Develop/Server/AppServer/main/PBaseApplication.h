#pragma once

#include "SServerApplication.h"
#include "PServerSystems.h"
#include "PServerManagers.h"


class PBaseApplication : public SServerApplication
{
protected:
	PServerSystems	m_Systems;						///< 게임 전체에서 사용하는 시스템 모음
	PServerManagers	m_Managers;						///< 게임 전체에서 사용하는 매니저 모음

	virtual	bool	OnCreate() override;
	virtual void	OnDestroy() override;
	virtual bool	OnUpdate(float fDelta) override;

	void AnalysisProfiling();
	void StartProfiling();


public:
	PBaseApplication();
	virtual			~PBaseApplication();

};