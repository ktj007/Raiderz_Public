#include "stdafx.h"
#include "MFrame.h"
#include "MColorTable.h"
#include "Mint.h"

namespace mint3 {


/*
class MTitleBar : public MWidget{
protected:
	virtual void OnDraw(MDrawContext* pDC){
		pDC->SetColor(MColorTable::MENUBAR_PLANE);
		pDC->FillRectangle(m_Rect);
		pDC->SetColor(MColorTable::MENUBAR_TEXT);
		pDC->Text(m_Rect, m_szText, MAM_LEFT);
	}
public:
	MTitleBar(char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL)
		: MWidget(szName, pParent, pListener){}
	//virtual ~MTitleBar(void){}
};

class MClient : public MWidget{
protected:
	virtual void OnDraw(MDrawContext* pDC){
	}
public:
	MClient(char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL)
		: MWidget(szName, pParent, pListener){}
};
*/

#define FRAME_W	300
#define FRAME_X	10
#define FRAME_H	300
#define FRAME_Y	10

void MFrameLook::OnDraw(MFrame* pFrame, MDrawContext* pDC)
{
	pDC->SetOpacity(pFrame->GetOpacity());
	MRECT r = pFrame->GetInitialClientRect();
	pDC->SetColor( MColorTable::MFRAME_PLANE );
	pDC->FillRectangle(r);
	pDC->SetColor(128,128,128,255);
	pDC->Rectangle(r);

	/*
	// Frame Board
	MRECT r(0, 0, pFrame->m_Rect.w, pFrame->m_Rect.h);
	pDC->SetColor( MColorTable::FRAME_PLANE );
	pDC->FillRectangle(r);
	pDC->SetColor( MColorTable::FRAME_OUTLINE );
	pDC->Rectangle(r);
	*/

	// TitleBar
	if(pFrame->m_bTitleBar==true)
	{
		int nTitleBarHeight = MTITLEBAR_HEIGHT;
		// 화면 크기의 상대값을 사용하는가?
// 		if(pFrame->GetUseRatioToStandardScreenSize())
// 		{
// 			// 화면 크기에 따른 비율
// 			nTitleBarHeight = (int)((float)nTitleBarHeight * pFrame->GetRatioToStandardScreenHeight());
// 		}
		MRECT TitleBarRect(r.x, r.y, r.w, nTitleBarHeight);
		pDC->SetColor(MColorTable::MMENUBAR_PLANE);
		pDC->FillRectangle(TitleBarRect);
		pDC->SetColor(MColorTable::MMENUBAR_TEXT);

//		if (pDC->BeginFont())
		{
			pDC->Text(TitleBarRect, pFrame->GetText(), MAM_LEFT);
//			pDC->EndFont();
		}
	}
}

MRECT MFrameLook::GetClientRect(MFrame* pFrame, MRECT& r)
{
//	int nTitleBarHeight = (pFrame->m_bTitleBar==true?MTITLEBAR_HEIGHT:0);
//	return MRECT(r.x+1, r.y + nTitleBarHeight, r.w-2, r.h - nTitleBarHeight-2);
	return r;
}

IMPLEMENT_LOOK(MFrame, MFrameLook)


bool MFrame::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if (IsFocusEnable() == false)
		return false;

	MWidget::OnEvent(pEvent, pListener);
	//if(m_bTitleBar==true) return false;	// 타이틀바가 없는 프레임은 이벤트를 받지 않는다.

	MRECT TitleBarRect(0, 0, m_Rect.w, m_pCustomLook ? m_pCustomLook->GetTitlebarHeight() : MTITLEBAR_HEIGHT);
	MRECT WidgetRect(0, 0, m_Rect.w, m_Rect.h);
	MPOINT sp = MClientToScreen(this, pEvent->Pos);
	//MPOINT sp(m_Rect.x, m_Rect.y);
	switch(pEvent->nMessage){
	case MWM_LBUTTONDOWN:

		if(m_bTitleBar==true && TitleBarRect.InPoint(pEvent->Pos)==true) {

			if ( m_BtnClose.m_Rect.InPoint(pEvent->Pos)==true)
			{
				m_BtnClose.m_bLButtonDown = true;
			}
			else if (m_BtnMinimize.m_Rect.InPoint(pEvent->Pos)==true)
			{
				m_BtnMinimize.m_bLButtonDown = true;
			}
			else
			{
				SetCapture();
				m_bDragWidget = true;
				MPOINT wp = MClientToScreen(GetParent(), MPOINT(m_Rect.x, m_Rect.y));
				sp.x -= wp.x;
				sp.y -= wp.y;
				m_DragPoint = sp;
			}
			return true;
		}
		else if(WidgetRect.InPoint(pEvent->Pos)==true){
			return true;
		}
		break;
	case MWM_LBUTTONUP:
		if (m_bTitleBar==true && m_BtnClose.m_Rect.InPoint(pEvent->Pos)==true)
		{
			if (m_BtnClose.m_bLButtonDown==true) OnCloseButtonClick();
		}
		else if (m_bTitleBar==true && m_BtnMinimize.m_Rect.InPoint(pEvent->Pos)==true)
		{
			if (m_BtnMinimize.m_bLButtonDown==true) OnMinimizeButtonClick();
		}

		m_BtnClose.m_bLButtonDown = m_BtnMinimize.m_bLButtonDown = false;

		if(m_bDragWidget==true){
			ReleaseCapture();
			m_bDragWidget = false;
			return true;
		}
		break;
	case MWM_MOUSEMOVE:
		if(m_bDragWidget==true){
			sp.x -= m_DragPoint.x;
			sp.y -= m_DragPoint.y;
			if(sp.x<0) sp.x = 0;
			if(sp.y<0) sp.y = 0;
			if(sp.x+m_Rect.w>MGetWorkspaceWidth()-1) sp.x = MGetWorkspaceWidth()-m_Rect.w-1;
			if(sp.y+m_Rect.h>MGetWorkspaceHeight()-1) sp.y = MGetWorkspaceHeight()-m_Rect.h-1;
			
			MPOINT p = MScreenToClient(GetParent(), sp);
			
			if (m_bMovable == true)
				SetPosition(p.x, p.y);
			
			return true;
		}
		else if(m_bTitleBar==true)
		{
			if(m_BtnClose.m_Rect.InPoint(pEvent->Pos)==true)
			{
				if(m_BtnClose.m_bMouseOver==false) m_BtnClose.m_bMouseOver = true;
			}
			else
			{
				if(m_BtnClose.m_bMouseOver==true) m_BtnClose.m_bMouseOver = false;
			}
			if(m_BtnMinimize.m_Rect.InPoint(pEvent->Pos)==true)
			{
				if(m_BtnMinimize.m_bMouseOver==false) m_BtnMinimize.m_bMouseOver = true;
			}
			else
			{
				if(m_BtnMinimize.m_bMouseOver==true) m_BtnMinimize.m_bMouseOver = false;
			}

		}
		break;
	case MWM_LBUTTONDBLCLK:
		// m_BtnClose 가 초기화가 안되어서 debug 에서는 동작을 안함. 일단 주석
		/*
		if (m_bTitleBar==true && m_BtnClose.m_Rect.InPoint(pEvent->Pos)==true)
		{
			OnCloseButtonClick();
		}
		else if (m_bTitleBar==true && m_BtnMinimize.m_Rect.InPoint(pEvent->Pos)==true)
		{
			OnMinimizeButtonClick();
		}

		// Shade
		else if(m_bTitleBar==true && TitleBarRect.InPoint(pEvent->Pos)==true){
			SetShade(!m_bShade);
			return true;
		}

		else if(WidgetRect.InPoint(pEvent->Pos)==true){
			return true;
		}
		return false;
		*/
		break;
	}
	return false;
}

void MFrame::OnShow(void)
{
	MWidget::OnShow();

	/*
	// 에디트 컨트롤에 우선순위를 두고...
	for(int i=0; i<GetChildCount(); i++){
		MWidget* pWidget = GetChild(i);
		if(strcmp(pWidget->GetClassName(), "Edit")==0){
			pWidget->SetFocus();
			return true;
		}
	}
	*/
	// Children의 첫번째 Widget에게 포커스를 준다.
	if(GetChildCount()>0) GetChild(0)->SetFocus();
}

MFrame::MFrame(const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()

	SetBounds(MRECT(FRAME_X, FRAME_Y, FRAME_W, FRAME_H));
	m_OldRect = m_Rect;

	SetFocusEnable(true);
//	Show(false);

	m_bDragWidget = false;

	m_bZOrderChangable = true;

	// Resizable
	m_bResizable = true;
	m_bMovable = true;

	m_bShade = false;
	m_bCanShade = true;

	m_bTitleBar = true;

	m_nMinWidth = 300;
	m_nMinHeight = 200;
}

MFrame::~MFrame(void)
{

}

void MFrame::OnSize(int w, int h)
{
	ResizeBtnsByAnchors(w, h);
	m_OldRect = m_Rect;
}

bool MFrame::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{

	return false;
}

void MFrame::SetShade(bool bShade)
{
	if (!m_bCanShade) return;

	if (m_bShade == bShade) return;

	if(m_bShade==false){
		m_BeforeShade = MSIZE(m_Rect.w, m_Rect.h);
		SetSize(m_Rect.w, MTITLEBAR_HEIGHT);
		m_bShade = true;
		m_bResizable = false;
	}
	else{
		SetSize(m_BeforeShade);
		m_bShade = false;
		m_bResizable = true;
	}
}

void ResizeByAnchors(MFrameBtn* pFrameBtn, MRECT parentRect, int w, int h)
{
	MRECT r = pFrameBtn->m_Rect;
	if(pFrameBtn->m_Anchor.bLeft==true && pFrameBtn->m_Anchor.bRight==true)
	{
		r.w += (w-parentRect.w);
	}
	else if(pFrameBtn->m_Anchor.bRight==true)
	{
		r.x += (w-parentRect.w);
	}
	if(pFrameBtn->m_Anchor.bTop==true && pFrameBtn->m_Anchor.bBottom==true)
	{
		r.h += (h-parentRect.h);
	}
	else if(pFrameBtn->m_Anchor.bBottom==true)
	{
		r.y += (h-parentRect.h);
	}
	pFrameBtn->m_Rect = r;
}

void MFrame::ResizeBtnsByAnchors(int w, int h)
{
	ResizeByAnchors(&m_BtnClose, m_OldRect, w, h);
	ResizeByAnchors(&m_BtnMinimize, m_OldRect, w, h);
}

void MFrame::OnCloseButtonClick()
{
	Show(false);
}
void MFrame::OnMinimizeButtonClick()
{
	SetShade(!m_bShade);
}

bool MFrame::IsFocus()
{
	MWidget* pWidget = MWidget::m_pFocusedWidget;

	while ( pWidget != NULL)
	{
		if ( pWidget == this)
			return true;

		pWidget = pWidget->GetParent();
	}

	return false;
}


} // namespace mint3