#pragma once

#include "LBaseApplication.h"
#include "STrayIcon.h"
#include "MSingleton.h"
#include "LServerInfo.h"
#include "MDebug.h"
#include "MTime.h"

class GEntity;
class LCmdHandler_App;

class LConsole : public LBaseApplication, public MSingleton<LConsole>
{
private:
	LCmdHandler_App*	m_pAppCmdHandler;

	MRegulator			m_RegPrintServerTick;

protected:
	virtual	bool	OnCreate();
	virtual bool	OnUpdate(float fDelta) override;
	virtual void	OnDestroy();
	virtual void	OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {}

	void UpdatePrintServerTick(float fDelta);
public:
					LConsole();
	virtual			~LConsole();

	static void		OutputLogProc(const char* szText, MLogLevel nLevel);
	void			OutputLog(const wchar_t* szText, MLogLevel nLevel);

public:
	virtual WORD GetAppDialogId(void) { return 0; }
	virtual WORD GetLogEditID(void) { return 0; }
	virtual WORD GetTrayMenuID(void) { return 0; }
};
