#pragma once

#include <windows.h>
#include "CSCommonLib.h"
#include "MCommandLine.h"

#define MAX_SERVER_TICK_DELTA		(1.0f)		///< 아무리 랙이 걸려도 delta값은 1초를 초과하지 않는다.

/// 서버 실행 타입
enum CServerAppType
{
	APPTYPE_WINDOW = 0,		///< 윈도우 어플리케이션
	APPTYPE_CONSOLE,		///< 콘솔 어플리케이션
	APPTYPE_SERVICE			///< 서비스 어플리케이션
};

class CSCOMMON_API CServerApplication
{
private:
	CServerAppType		m_nAppType;
	bool				m_bRun;
	DWORD				m_dwMainThreadID;
	unsigned int		m_nLastTime;

	static LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static CServerApplication* m_pInstance;
	static void SignalHandler( int signo);

	void Update();
	bool InitMainWnd(int x, int y, int width, int height, const char* szClassName, const char* szName, UINT nMenuID, UINT nIconID);

	void CatchTerminationSignal(void handler(int));
protected:
	HWND m_hWnd;
	cml2::MCommandLine		m_CommandLine;

	virtual	bool OnCreate()				{ return true; }
	virtual void OnUpdate(float fDelta)	{ }
	virtual void OnDestroy()			{ }
	virtual void OnPaint()				{ }
	virtual void OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void Destroy();

	int		Run_Window();
	int		Run_Console();
	bool	IsMainThread(DWORD dwCurThreadID) { return (m_dwMainThreadID == dwCurThreadID); }
public:
	CServerApplication();
	virtual ~CServerApplication();
	int	Run();
	void Quit() { m_bRun = false; }
	bool Create(int x, int y, int width, int height, const char* szClassName, const char* szName, UINT nMenuID, UINT nIconID);
	bool CreateConsole(const char* szName);

	const cml2::MCommandLine& GetCommandLine() { return m_CommandLine; }
};

