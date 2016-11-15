#pragma once

#include "SServerApplication.h"
#include "LServerSystems.h"
#include "LServerManagers.h"

class LBaseApplication : public SServerApplication
{
protected:
	LServerSystems	m_Systems;						///< 게임 전체에서 사용하는 시스템 모음
	LServerManagers	m_Managers;						///< 게임 전체에서 사용하는 매니저 모음

	virtual	bool	OnCreate() override;
	virtual void	OnDestroy() override;
	virtual bool	OnUpdate(float fDelta) override;

	void AnalysisProfiling();
	void StartProfiling();


public:
	LBaseApplication();
	virtual			~LBaseApplication();

};