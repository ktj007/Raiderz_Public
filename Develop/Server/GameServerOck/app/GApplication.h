#pragma once

#include "SServerApplication.h"
#include "GServer.h"
#include "STrayIcon.h"
#include "SAppMonitorEditBox.h"
#include "MSingleton.h"
#include "GServerInfo.h"
#include "GGlobal.h"
#include "GServerSystems.h"
#include "GServerManagers.h"
#include "GConfig_Arg.h"
#include "GConfig_Lua.h"
#include "GAutoTestRunner.h"
#include "GBirdMonitor.h"
#include "GBaseApplication.h"

class GEntity;
class GValidatorSilo;
class GRuntimeValidator;
class GCmdHandler_App;

class GAppBirdMonitorListener : public IBirdMonitorListener
{
public:
	virtual void OnOutput(int nLine, const wchar_t* szMsg);
};

/// 어플리케이션 - 서버의 윈도우 폼 등을 관리
class GApplication : public GBaseApplication, public MSingleton<GApplication>
{
private:
	static HWND		m_hDlg;
	static HWND		m_hListBox;

	static void DrawMiniMap(HWND hWnd, int mx, int my);
	static void TrackingToEntity(MUID uidEntity);

	STrayIcon		m_TrayIcon;
	HMODULE			m_hRichEd;
	HWND			m_hCommandLogEdit;
	HWND			m_hRichEdLog;
	HWND			m_hRichEdErrorLog;
	HWND			m_hStatus;
	MRegulator*		m_pMinimapRglt;
	bool			m_bDrawMinimap;
	bool			m_bMakeTrayIcon;
	static vector<MUID> m_vecSelectedList;

	HINSTANCE			m_hInst;
	SAppMonitorEditBox*	m_pMonitorEd;

	GAppBirdMonitorListener	m_BirdMonitorListener;
	GRuntimeValidator* m_pRuntimeValidator;
	
	GCmdHandler_App*	m_pAppCmdHandler;

	void			InitWindowHandles();
	void			SetColorRichEdit(HWND hRichEdit, COLORREF color);
	void			AddErrorLogString(const wchar_t* szString, COLORREF color);
	void			AddLogString(const wchar_t* szString, COLORREF color = RGB(0, 0, 0));

	void			AddStringToRichEditor(HWND hRichEditor, const wchar_t* szString, COLORREF color);
	void			UpdateMinimap(float fDelta);
	void			Show(bool bVisible);
	void			StartDrawMinimap();
	void			StopDrawMinimap();

	void			OnServerStart();
	void			OnServerStop();
	void			UpdateServerInfo();
	void			DrawValidationAlert();
	

	// Child Control
	static BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

	// Msg handler
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
protected:
	virtual	bool	OnCreate() override;
	virtual void	OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void	OnDestroy() override;
	virtual void	OnPaint() override;
	virtual bool	OnUpdate(float fDelta) override;

	void SetupControls( HWND hWnd );
	bool SetupTrayIcon();
	void SetupFieldComboBox();


	virtual wstring	_MakeTitleCaption();
	virtual void	_RunRuntimeValidator();
	virtual wstring	_MakeServerVersion();
	virtual bool	_ValidateResouces();
	virtual void	_LogCommand( const wchar_t* szMsg );
	virtual void	_Quit(int nRetCode);


public:
					GApplication(bool bMakeTrayIcon=false);
	virtual			~GApplication();
	void			SetServerInfoView(GServerInfo& server_info);
	void			SetStatusText(int nID, LPCTSTR str);
	void			OutputLog(const wchar_t* szText, MLogLevel nLevel);

	void			SetMonitorText(int nLine, const wchar_t* szMsg);
	static void		OutputLogProc(const char* szText, MLogLevel nLevel);
	

};


