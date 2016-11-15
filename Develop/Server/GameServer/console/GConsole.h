#pragma once

#include "GBaseApplication.h"

class GCommandLogger;
class GValidatorSilo;
class GRuntimeValidator;
class GCmdHandler_App;


class GAppBirdMonitorListener : public IBirdMonitorListener
{
public:
	virtual void OnOutput(int nLine, const wchar_t* szMsg);
};

/// 어플리케이션 - 서버의 윈도우 폼 등을 관리
class GConsole : public GBaseApplication, public MSingleton<GConsole>
{
private:
	GAppBirdMonitorListener	m_BirdMonitorListener;
	GRuntimeValidator*		m_pRuntimeValidator;
	GCmdHandler_App*		m_pAppCmdHandler;

	MRegulator				m_RegPrintGameTick;
protected:
	virtual	bool	OnCreate() override;
	virtual void	OnDestroy() override;
	virtual bool	OnUpdate(float fDelta) override;

	virtual void	_RunRuntimeValidator();
	virtual bool	_ValidateResouces();
	virtual void	_LogCommand( const wchar_t* szMsg );
	virtual void	_Quit(int nRetCode);
	
	void UpdatePrintGameTick(float fDelta);
public:
					GConsole(bool bMakeTrayIcon=false);
	virtual			~GConsole();
	void			OutputLog(const wchar_t* szText, MLogLevel nLevel);

	static void		OutputLogProc(const char* szText, MLogLevel nLevel);
};


