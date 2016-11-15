#pragma once

#include "SServerApplication.h"
#include "SApplicationHandles.h"
#include "STrayIcon.h"
#include "MSingleton.h"
#include "LServerInfo.h"
#include "LGlobal.h"
#include "MTime.h"
#include "MUID.h"
#include "MDebug.h"
#include "LBaseApplication.h"

class GEntity;
class LCmdHandler_App;

/// 어플리케이션 - 서버의 윈도우 폼 등을 관리
class LApplication : public LBaseApplication, public MSingleton<LApplication>
{
private:
	static HWND		m_hListBox;

	STrayIcon		m_TrayIcon;
	HMODULE			m_hRichEd;	
	HWND			m_hRichEdLog;
	HWND			m_hRichEdErrorLog;	
	
	bool			m_bMakeTrayIcon;

	SApplicationHandles*	m_pApplicationHandles;

	
	LCmdHandler_App*	m_pAppCmdHandler;

	void			InitWindowHandles();
	void			SetColorRichEdit(HWND hRichEdit, COLORREF color);

	void			Show(bool bVisible);
	void			AddErrorLogString(const wchar_t* szString, COLORREF color);
	void			AddLogString(const wchar_t* szString, COLORREF color = RGB(0, 0, 0), bool bCarrigeReturn = true);
	
protected:
	virtual	bool	OnCreate();
	bool SetupTrayIcon();
	virtual void	OnDestroy();
	virtual void	OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
					LApplication(bool bMakeTrayIcon=false);
	virtual			~LApplication();
	void			LogCommand(const wchar_t* szCmd);
	void			SetServerInfoView(LServerInfo& server_info);

	static void		OutputLogProc(const char* szText, MLogLevel nLevel);
	void			OutputLog(const wchar_t* szText, MLogLevel nLevel);

	
};


