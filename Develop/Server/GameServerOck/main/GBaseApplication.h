#pragma once

#include "SServerApplication.h"
#include "GServer.h"
#include "GServerInfo.h"
#include "GGlobal.h"
#include "GServerSystems.h"
#include "GServerManagers.h"
#include "GConfig_Arg.h"
#include "GConfig_Lua.h"
#include "GAutoTestRunner.h"
#include "GBirdMonitor.h"

class GBaseApplication : public SServerApplication
{
protected:
	GServerSystems	m_Systems;						///< 게임 전체에서 사용하는 시스템 모음
	GServerManagers	m_Managers;						///< 게임 전체에서 사용하는 매니저 모음

	GConfig_Arg		m_ConfigArg;
	GConfig_Lua		m_ConfigLua;

	bool			m_bDoValidate;
	bool			m_bDrawValidatorAlert;

	GAutoTestRunner	m_AutoTestRunner;
	MRegulator		m_LimitRunTimeRegulator;

	virtual	bool	OnCreate() override;
	virtual void	OnDestroy() override;
	virtual bool	OnUpdate(float fDelta) override;
	virtual void	OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	virtual void	CloseServer() override;

	bool RunAutoTest();
	bool UpdateAutoTest( float fDelta );
	void InitCommandLine();

	void InitValidate();
	void ValidateResouces();

	void StartProfiling();
	void AnalysisProfiling();

	virtual void _RunRuntimeValidator() {}
	virtual wstring _MakeServerVersion() { return L"GameServer"; }
	virtual wstring _MakeTitleCaption() { return L"GameServer"; }
	virtual bool _ValidateResouces() { return true; }
	virtual void _LogCommand( const wchar_t* szMsg ) {}
	virtual void _Quit(int nRetCode) {}
public:
	GBaseApplication();
	virtual ~GBaseApplication() {}

	void LogCommand(const wchar_t* szCmd);
};