#pragma once

#include <windows.h>
#include "MCommandLine.h"
#include "ServerCommonLib.h"
#include "MIOCP.h"

#define MAX_SERVER_TICK_DELTA		(1.0f)		///< 아무리 랙이 걸려도 delta값은 1초를 초과하지 않는다.

class SServerMonitorEventListener;

/// 서버 실행 타입
enum SServerAppType
{
	APPTYPE_WINDOW = 0,		///< 윈도우 어플리케이션
	APPTYPE_CONSOLE,		///< 콘솔 어플리케이션
	APPTYPE_SERVICE			///< 서비스 어플리케이션
};

namespace minet {
class MCommand;
}

class SCOMMON_API SServerApplication
{
private:
	SServerAppType		m_nAppType;
	bool				m_bRun;
	DWORD				m_dwMainThreadID;
	unsigned int		m_nLastTime;
	MIOCP*				m_iocp;

	/// ServerMonitor 로부터의 이벤트 시그널을 감지
	SServerMonitorEventListener*	m_pServerMonitorEventListener;

	static LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MsgProc_ForHook(int code, WPARAM wParam, LPARAM lParam);
	static SServerApplication* m_pInstance;
	static BOOL WINAPI ConsoleCloseHandler(DWORD CtrlType);

	bool Update();
	bool InitMainWnd(int x, int y, int width, int height, const wchar_t* szClassName, const wchar_t* szName, UINT nMenuID, UINT nIconID);

	void CatchTerminationSignal(PHANDLER_ROUTINE handler);
	void CheckServerMonitorEvent();

protected:
	HWND m_hWnd;
	cml2::MCommandLine		m_CommandLine;

	virtual	bool OnCreate()				{ return true; }
	virtual bool OnUpdate(float fDelta)	{ return true; }
	virtual void OnDestroy()			{ }
	virtual void OnPaint()				{ }
	virtual void OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void Destroy();

	int		Run_Window();
	int		Run_Console();
	bool	IsMainThread(DWORD dwCurThreadID) { return (m_dwMainThreadID == dwCurThreadID); }

	bool	StartServerMonitorEventListener(const wchar_t* szServerName);

private:
	struct IOMSG : public OVERLAPPED
	{
		minet::MCommand* cmd;
	};

public:
	class MsgHandler : public MIOCP::IHandler
	{
	public:
		virtual void OnRecvCmd(minet::MCommand* const cmd) = 0;

	private:
		void OnIO(const DWORD& numOfByteTransfered,OVERLAPPED* const overlapped);
		void OnError(const int& errCode);
	};

public:
	SServerApplication();
	virtual ~SServerApplication();
	int	Run(MIOCP* const iocp = NULL);
	void Quit();

	virtual void CloseServer();

	bool Create(int x, int y, int width, int height, const wchar_t* szClassName, const wchar_t* szName, UINT nMenuID, UINT nIconID);
	bool CreateConsole(const wchar_t* szName, wstring strStartParameter = L"");

	const cml2::MCommandLine& GetCommandLine() { return m_CommandLine; }

	void RegisterMsgHandler(const MsgHandler& handler) const;
	void SendCmd(const MsgHandler& handler,minet::MCommand* const cmd) const;
};
