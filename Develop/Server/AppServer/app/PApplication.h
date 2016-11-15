#pragma once

#include "PBaseApplication.h"
#include "SApplicationHandles.h"
#include "STrayIcon.h"
#include "MSingleton.h"
#include "PServerInfo.h"
#include "PGlobal.h"
#include "PServerSystems.h"
#include "PServerManagers.h"
#include "MTime.h"
#include "MUID.h"
#include "MDebug.h"

class PCmdHandler_App;

/// 어플리케이션 - 서버의 윈도우 폼 등을 관리
class PApplication : public PBaseApplication, public MSingleton<PApplication>
{
private:
	static HWND		m_hListBox;

	STrayIcon		m_TrayIcon;
	HMODULE			m_hRichEd;	
	HWND			m_hRichEdLog;
	HWND			m_hRichEdErrorLog;	
	bool			m_bMakeTrayIcon;

	SApplicationHandles*	m_pApplicationHandles;
	PCmdHandler_App*		m_pAppCmdHandler;


	void			InitWindowHandles();
	void			SetColorRichEdit(HWND hRichEdit, COLORREF color);

	void			Show(bool bVisible);
	void			OnServerStart();
	void			OnServerStop();
	void			AddErrorLogString(const wchar_t* szString, COLORREF color);
	void			AddLogString(const wchar_t* szString, COLORREF color = RGB(0, 0, 0), bool bCarrigeReturn = true);

protected:
	virtual	bool	OnCreate();

	bool SetupTrayIcon();

	virtual void	OnDestroy();
	virtual void	OnPaint();
	virtual void	OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
					PApplication(bool bMakeTrayIcon=false);
	virtual			~PApplication();
	void			LogCommand(const wchar_t* szCmd);
	void			SetServerInfoView(PServerInfo& server_info);

	static void		OutputLogProc(const char* szText, MLogLevel nLevel);
	void			OutputLog(const wchar_t* szText, MLogLevel nLevel);


};
