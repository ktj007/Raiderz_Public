#pragma once

#include "CServerApplication.h"
#include "STrayIcon.h"
#include "MSingleton.h"
#include "LServerInfo.h"
#include "LGlobal.h"
#include "LServerSystems.h"
#include "LServerManagers.h"
#include "MTime.h"
#include "MUID.h"
#include "MDebug.h"

class GEntity;

/// 어플리케이션 - 서버의 윈도우 폼 등을 관리
class LApplication : public CServerApplication, public MSingleton<LApplication>
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
	unsigned int	m_nLastTime;
	bool			m_bMakeTrayIcon;

	LServerSystems	m_Systems;						///< 게임 전체에서 사용하는 시스템 모음
	LServerManagers	m_Managers;						///< 게임 전체에서 사용하는 매니저 모음

	void			InitWindowHandles();
	void			SetColorRichEdit(HWND hRichEdit, COLORREF color);

	void			Show(bool bVisible);
	void			AnalysisProfiling();
	void			StartProfiling();
	void			OnServerStart();
	void			OnServerStop();
	void			AddErrorLogString(const char* szString, COLORREF color);
	void			AddLogString(const char* szString, COLORREF color = RGB(0, 0, 0), bool bCarrigeReturn = true);
	
	// Child Control
	static BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

	// Msg handler
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
protected:
	virtual	bool	OnCreate(LPSTR cmdLine);

	bool SetupTrayIcon();
	virtual void	OnUpdate();
	virtual void	OnDestroy();
	virtual void	OnPaint();
	virtual void	OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
					LApplication(bool bMakeTrayIcon=false);
	virtual			~LApplication();
	void			LogCommand(const char* szCmd);
	void			SetServerInfoView(LServerInfo& server_info);
	void			SetStatusText(int nID, LPCTSTR str);

	static void		OutputLogProc(const char* szText, MLogLevel nLevel);	
};


