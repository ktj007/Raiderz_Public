#ifndef _GAPPLICATION_H
#define _GAPPLICATION_H

#include "CServerApplication.h"
//#include "GServer.h"
#include "TrayIcon.h"
#include "MSingleton.h"
#include "GServerInfo.h"
#include "GGlobal.h"
#include "GServerSystems.h"
#include "GServerManagers.h"
#include "MTime.h"

class GEntity;

/// 어플리케이션 - 서버의 윈도우 폼 등을 관리
class GApplication : public CServerApplication, public MSingleton<GApplication>
{
private:
	static HWND		m_hDlg;
	static HWND		m_hListBox;

	CTrayIcon		m_TrayIcon;
	HMODULE			m_hRichEd;
	HWND			m_hCommandLogEdit;
	HWND			m_hRichEdLog;
	HWND			m_hRichEdErrorLog;
	HWND			m_hStatus;
	unsigned int	m_nLastTime;
	MRegulator*		m_pMinimapRglt;
	bool			m_bDrawMinimap;
	bool			m_bMakeTrayIcon;
	static vector<MUID> m_vecSelectedList;

	GServerSystems	m_Systems;						///< 게임 전체에서 사용하는 시스템 모음
	GServerManagers	m_Managers;						///< 게임 전체에서 사용하는 매니저 모음

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
					GApplication(bool bMakeTrayIcon=false);
	virtual			~GApplication();
	void			LogCommand(const char* szCmd);
	void			SetServerInfoView(GServerInfo& server_info);
	void			SetStatusText(int nID, LPCTSTR str);

	static void		OutputLogProc(const char* szText, MLogLevel nLevel);	
};


#endif