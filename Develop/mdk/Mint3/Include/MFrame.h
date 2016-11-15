#pragma once

#include "MWidget.h"
#include "MLookNFeel.h"
#include "MBmButton.h"

#define MTITLEBAR_HEIGHT		18


namespace mint3 {

class MFrame;
class MFrameLook{
protected:
	MRECT	m_WinRectOffset;			// Look 상의 윈도우 영역 오프셋 값을 저장하는 변수
	int		m_nTitlebarHeight;			// 타이틀바 높이

public:
	MFrameLook() { m_WinRectOffset = MRECT( 0, 0, 0, 0);	m_nTitlebarHeight = MTITLEBAR_HEIGHT;	}
	virtual void OnDraw(MFrame* pFrame, MDrawContext* pDC);
	virtual MRECT GetClientRect(MFrame* pFrame, MRECT& r);

	// Look 상의 윈도우 영역 오프셋 값을 설정 및 구하는 함수
	void SetWinRectOffset( int x, int y, int w, int h)		{ m_WinRectOffset = MRECT( x, y, w, h);	}
	const MRECT& SetWinRectOffset()							{ return m_WinRectOffset;				}

	// 타이틀바의 높이를 설정 및 구하는 함수
	void SetTitlebarHeight( int height)						{ m_nTitlebarHeight = height;			}
	int GetTitlebarHeight()									{ return m_nTitlebarHeight;				}

};

struct MFrameBtn
{
	bool			m_bVisible;
	bool			m_bMouseOver;
	bool			m_bLButtonDown;
//	bool			m_bDowned;
	MRECT			m_Rect;
	MAnchorStyle	m_Anchor;
	MFrameBtn() { 
		m_bVisible = false; 
		m_bLButtonDown = false; 
		m_bMouseOver = false; 
		m_Rect = MRECT(0,0,0,0);
	}
	bool IsButtonDown() { if ((m_bLButtonDown==true) && (m_bMouseOver==true)) return true; return false; }
};

/// 움직일 수 있는 프레임으로 구성된 위젯
class MFrame : public MWidget{
	DECLARE_LOOK(MFrameLook)
	DECLARE_LOOK_CLIENT()
protected:
	bool	m_bDragWidget;		///< Drag Widget
	MPOINT	m_DragPoint;

	bool	m_bMovable;			///< 움직일 수 있는지 여부
	bool	m_bCanShade;		///< Shade 할 수 있는지 여부 - false이면 Shade할 수 없다.
	bool	m_bShade;
	MSIZE	m_BeforeShade;		///< Before Shade

	MRECT		m_OldRect;
	MFrameBtn	m_BtnClose;
	MFrameBtn	m_BtnMinimize;

public:
	bool		m_bTitleBar;	///< Use Title Bar

protected:
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	virtual void OnSize(int w, int h);
	virtual void OnShow(void);

	void ResizeBtnsByAnchors(int w, int h);
	void OnCloseButtonClick();
	void OnMinimizeButtonClick();
public:
	MFrame(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MFrame(void);

	void SetShade(bool bShade);
	void SetMovable(bool bValue) { m_bMovable = bValue; }
	MFrameBtn*	GetCloseButton() { return &m_BtnClose; }
	MFrameBtn*	GetMinimizeButton() { return &m_BtnMinimize; }

	virtual bool IsFocus(void);


#define MINT_FRAME	"Frame"
	virtual const char* GetClassName(void){ return MINT_FRAME; }
};

} // namespace mint3