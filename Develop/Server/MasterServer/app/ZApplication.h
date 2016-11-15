#pragma once

#include "ZBaseApplication.h"
#include "STrayIcon.h"
#include "SAppMonitorEditBox.h"
#include "MSingleton.h"
#include "ZServerInfo.h"
#include "ZGlobal.h"
#include "ZServerSystems.h"
#include "ZServerManagers.h"
#include "MTime.h"
#include "MUID.h"
#include "MDebug.h"

class ZCmdHandler_App;

/// 어플리케이션 - 서버의 윈도우 폼 등을 관리
class ZApplication : public ZBaseApplication, public MSingleton<ZApplication>
{
private:
	static HWND		m_hDlg;
	static HWND		m_hListBox;

	STrayIcon		m_TrayIcon;
	HMODULE			m_hRichEd;
	HWND			m_hCommandLogEdit;
	HWND			m_hRichEdLog;
	HWND			m_hRichEdErrorLog;
	HWND			m_hStatus;
	bool			m_bMakeTrayIcon;

	SAppMonitorEditBox*	m_pMonitorEd;

	ZCmdHandler_App*	m_pAppCmdHandler;

	void			InitWindowHandles();
	void			SetColorRichEdit(HWND hRichEdit, COLORREF color);

	void			Show(bool bVisible);
	void			OnServerStart();
	void			OnServerStop();
	void			UpdateServerInfo();
	void			AddErrorLogString(const wchar_t* szString, COLORREF color);
	void			AddLogString(const wchar_t* szString, COLORREF color = RGB(0, 0, 0), bool bCarrigeReturn = true);
	
	// Child Control
	static BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

	// Msg handler
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
protected:
	virtual	bool	OnCreate();

	bool SetupTrayIcon();
	virtual bool	OnUpdate(float fDelta) override;
	virtual void	OnDestroy() override;
	virtual void	OnPaint() override;
	virtual void	OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
public:
					ZApplication(bool bMakeTrayIcon=false);
	virtual			~ZApplication();
	void			LogCommand(const wchar_t* szCmd);
	void			SetServerInfoView(ZServerInfo& server_info);
	void			SetStatusText(int nID, LPCTSTR str);
	void			SetMonitorText(int nLine, const wchar_t* szMsg);

	static void		OutputLogProc(const char* szText, MLogLevel nLevel);
	void			OutputLog(const wchar_t* szText, MLogLevel nLevel);
};


