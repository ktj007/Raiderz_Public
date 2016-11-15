#pragma once
#include "RApplication.h"
#include "RDebuggingInfo.h"

namespace rs3 {

class RS_API RD3DApplication : public RApplication
{

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	int			m_nIconResID;		///< 아이콘 리소스 ID. 상속 받은 Application 생성자에서 세팅해주면 된다.
	DWORD		m_dwWindowStyle;
	RECT		m_rcWindowBounds;

	void			AdjustWindowSizeAndShowWindow( int x, int y, int nWidth, int nHeight, bool bFullScreen);

	virtual RSceneManager* CreateSceneManager();
	virtual bool	CreateDevice();
	virtual void	DestroyDevice();
	virtual DWORD	GetWindowStyle(bool bFullScreen);
public:
	RD3DApplication(void);
	virtual ~RD3DApplication(void);


	virtual bool	ProcessMessage(MSG&  msg);
	// 더이상 메시지를 처리할 필요가 없으면 pReturn에 윈도우메시지의 리턴값을 넣고 true를 리턴
	virtual bool	OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn) { return false; }

	virtual int		MainLoop();
	virtual void	Activate(bool bActive);

	virtual void	OnLostDevice();
	virtual void	OnResetDevice();

	void			ToggleFullScreen();


private:
	RDebuggingInfo	m_DebuggingInfo;

};

}
