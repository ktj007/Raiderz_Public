#include "stdafx.h"
#include "MResource.h"
#include "MWidget.h"
#include "Mint.h"
#include "MButton.h"
#include "MEdit.h"
#include "MWLua.h"
#include "MScriptManager.h"
#include "MWidgetManager.h"
#include "MDebug.h"

namespace mint3 {

MEventArgs		MWidget::m_EventArgs;
MDragEventArgs	MWidget::m_DragEventArgs;
MLinkedTextArgs	MWidget::m_LinkedTextArgs;

bool MIsSameWidgetClass(MWidget* pWidget, const char* szClassName)
{
	if (strcmp(pWidget->GetClassName(), szClassName) == 0) return true;
	return false;
}

bool MIsSameWidgetClass(MWidget* pWidgetA, MWidget* pWidgetB)
{
	if (strcmp(pWidgetA->GetClassName(), pWidgetB->GetClassName()) == 0) return true;
	return false;
}

MWidget* MWidget::m_pCapturedWidget = NULL;
MWidget* MWidget::m_pFocusedWidget = NULL;

static int g_nWidgetCount=0;

void MWidget::MakeLocalEvent(MEvent* pLoalEvent, const MEvent* pEvent)
{
	memcpy(pLoalEvent, pEvent, sizeof(MEvent));

	MRECT sr = GetScreenRect();
	pLoalEvent->Pos.x -= sr.x;
	pLoalEvent->Pos.y -= sr.y;
}

bool MWidget::EventResize(MEvent* pEvent)
{
	MEvent LocalEvent;
	MakeLocalEvent(&LocalEvent, pEvent);

	// Widget Resize
#define RESIZER_SIZE	3
	if(m_bResizable==true){
		MPOINT sp = pEvent->Pos;
		MWidget* pParent = GetParent();
		if(pParent==NULL) return false;

		if(pEvent->nMessage==MWM_LBUTTONDOWN)
		{
			m_nResizeSide = 0;

			// 위
			if(LocalEvent.Pos.y>=-RESIZER_SIZE && LocalEvent.Pos.y<=RESIZER_SIZE)
			{
				if(LocalEvent.Pos.x>=-RESIZER_SIZE && LocalEvent.Pos.x<=m_Rect.w+RESIZER_SIZE)
					m_nResizeSide |= 1;
			}
			// 오른쪽
			if(LocalEvent.Pos.x>=m_Rect.w-RESIZER_SIZE && LocalEvent.Pos.x<=m_Rect.w+RESIZER_SIZE)
			{
				if(LocalEvent.Pos.y>=-RESIZER_SIZE && LocalEvent.Pos.y<=m_Rect.h+RESIZER_SIZE)
					m_nResizeSide |= 2;
			}
			// 아래
			if(LocalEvent.Pos.y>=m_Rect.h-RESIZER_SIZE && LocalEvent.Pos.y<=m_Rect.h+RESIZER_SIZE)
			{
				if(LocalEvent.Pos.x>=-RESIZER_SIZE && LocalEvent.Pos.x<=m_Rect.w+RESIZER_SIZE)
					m_nResizeSide |= 4;
			}
			// 왼쪽
			if(LocalEvent.Pos.x>=-RESIZER_SIZE && LocalEvent.Pos.x<=RESIZER_SIZE)
			{
				if(LocalEvent.Pos.y>=-RESIZER_SIZE && LocalEvent.Pos.y<=m_Rect.h+RESIZER_SIZE)
					m_nResizeSide |= 8;
			}

			if(m_nResizeSide!=0){
				SetCapture();
				//
				return true;
			}
		}
		if(m_nResizeSide!=0 && pEvent->nMessage==MWM_LBUTTONUP){
			ReleaseCapture();
			m_nResizeSide = 0;
			return true;
		}
		if(pEvent->nMessage==MWM_MOUSEMOVE){
			if(m_nResizeSide!=0){
				MPOINT pp = MScreenToClient(pParent, sp);
				MPOINT tp = MScreenToClient(this, sp);
				MRECT r = m_Rect;
				if ( (m_nResizeSide & 1) /* && ( m_Anchor.bTop == false) */)
				{
					r.h += (r.y - pp.y);
					r.y = pp.y;
				}
				if ( (m_nResizeSide & 2) /* && ( m_Anchor.bRight == false) */)
				{
					r.w = tp.x;
				}
				if ( (m_nResizeSide & 4) /* && ( m_Anchor.bBottom == false) */)
				{
					r.h = tp.y;
				}
				if ( (m_nResizeSide & 8) /* && ( m_Anchor.bLeft == false) */)
				{
					r.w += (r.x - pp.x);
					r.x = pp.x;
				}


				// UI 최소 사이즈 보다 작은지 검사한다.
				ResetMinSize(r, m_Rect);

				SetPosition(r.x, r.y);

				SetSize(r.w, r.h);

				return true;
			}
		}
	}

	return false;
}


void MWidget::InsertChild(MWidget* pWidget)
{
	m_Children.push_front(pWidget);
	pWidget->m_pParent = this;
}

void MWidget::AddChild(MWidget* pWidget)
{
	m_Children.push_back(pWidget);
	pWidget->m_pParent = this;
}

void MWidget::RemoveChild(MWidget* pWidget)
{
	for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
	{
		MWidget* pCurWnd = (*itor);
		if (pCurWnd == pWidget)
		{
			pWidget->m_pParent = NULL;
			m_Children.erase(itor);
			return;
		}
	}
}

void MWidget::RemoveFromParent()
{
	if(m_pParent != NULL)
	{
		m_pParent->RemoveChild(this);
	}
}

void MWidget::AddExclusive(MWidget* pWidget)
{
	m_Exclusive.push_back(pWidget);
}

bool MWidget::RemoveExclusive(MWidget* pWidget)
{
	for (list<MWidget*>::iterator itor = m_Exclusive.begin(); itor != m_Exclusive.end(); ++itor)
	{
		MWidget* pThis = (*itor);
		if(pThis==pWidget){
			m_Exclusive.erase(itor);
			return true;
		}
	}
	return false;
}

MWidget* MWidget::GetLatestExclusive(void)
{
	if(!m_Exclusive.empty()) 
	{
		return (m_Exclusive.back());
	}
	return NULL;
}

/*
void MWidget::UseAcceleractorAndCharacter(char* szText)
{
	if(szText==NULL){
		m_nAccelerator = -1;
		return;
	}
	char szAnd[2] = {GetAndChar(szText), 0};
	_strupr(szAnd);
	m_nAccelerator = szAnd[0];
	if(m_nAccelerator==0) m_nAccelerator = -1;
}
*/

MWidget::MWidget(const char* szName, MWidget* pParent, MListener* pListener)
{
	g_nWidgetCount++;
	m_nID = g_nWidgetCount;

	if(szName!=NULL)
	{
		m_strName = szName;
	}

	// Default Region
	m_Rect.x = 0;
	m_Rect.y = 0;
	m_Rect.w = 100;
	m_Rect.h = 100;

	m_pParent = pParent;
	if(m_pParent!=NULL) m_pParent->AddChild(this);

	//m_pListener = pListener;
	SetListener(pListener);

	m_pCursor = NULL;		// NULL Cursor is Default Cursor
	m_pFont = NULL;			// NULL Font is Default Font

	m_bVisible = true;
	m_bEnable = true;
	m_bFocusEnable = false;	// Default Focus Disabled

	//EnableAccelerator(true);
	//SetLabelAccelerator();
	SetAccelerator(0);

	m_bZOrderChangable = false;
	m_bResizable = false;	// 디폴트는 리사이즈 불가능
	m_nResizeSide = 0;

	m_bClipByParent = false;

	m_fOpacity = 1.0f;


	m_bEnableDesignerMode = false;
	m_nDMDragWidget = 0;
	m_bModifiedByDesigner = false;
	m_bAddedByDesigner = false;
	//m_nID = -1;

	m_BoundsAlignment = MAM_NOTALIGN;

	/*
	m_bAnchorLeft = true;
	m_bAnchorRight = false;
	m_bAnchorTop = true;
	m_bAnchorBottom = false;
	*/

	m_nMinWidth = 10;
	m_nMinHeight = 10;

	m_bEventAcceleratorCall = false;

	m_bLButtonDown = false;
	m_bRButtonDown = false;

	m_bDeclToScript = false;

// 	m_bUseRatioToStandardScreenSize = false;
// 	m_fRatioToStandardScreenWidth = 1.0f;
// 	m_fRatioToStandardScreenHeight = 1.0f;

	if (m_strName != "")
		Mint::GetInstance()->m_pWidgetManager->AddWidget(m_strName.c_str(), this);

	m_bIterating = false;
}

MWidget::~MWidget(void)
{
	ReleaseExclusive();

	// 가지고 있는 Children도 모두 삭제
	DeleteChildren();

	if(m_pParent!=NULL) m_pParent->RemoveChild(this);
	if(MWidget::m_pCapturedWidget==this) MWidget::m_pCapturedWidget = NULL;
	if(MWidget::m_pFocusedWidget==this) MWidget::m_pFocusedWidget = NULL;

	if (m_strName != "")
        Mint::GetInstance()->m_pWidgetManager->RemoveWidget(this);

	// 루아에서 해제
	if (m_bDeclToScript)
		Mint::GetInstance()->GetScriptManager()->ReleaseWidget(m_strName.c_str());
}

void MWidget::ReleaseFromScript()
{

}

void MWidget::DeleteChildren()
{
	while (!m_Children.empty())
	{
		MWidget* pWidget = (*m_Children.begin());
		m_Children.erase(m_Children.begin());
		delete pWidget;
	}
}

void MWidget::OnRun(void)
{
}

void MWidget::OnDraw(MDrawContext* pDC)
{
	// Draw Nothing
	pDC->SetColor(MCOLOR(196, 196, 196));
	pDC->FillRectangle(GetInitialClientRect());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MWidget::OnEvent(MEvent* pEvent, MListener* pListener)
{
	switch(pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		{
			MRECT r = GetInitialClientRect();
			if(r.InPoint(pEvent->Pos)==true)
			{
				if (MIsDesignerMode())
				{
					this->EnableDesignerMode(true);
					return true;
				}

				m_bLButtonDown = true;
				SetCapture();
				OnMouseDown(pEvent);
			}
		}
		break;
	case MWM_RBUTTONDOWN:
		{
			MRECT r = GetInitialClientRect();
			if(r.InPoint(pEvent->Pos)==true)
			{
				m_bRButtonDown = true;
				SetCapture();
				OnMouseDown(pEvent);
			}
		}
		break;
	case MWM_LBUTTONUP:
		{
			MRECT r = GetInitialClientRect();
			if(m_bLButtonDown==true)
			{
				ReleaseCapture();
				m_bLButtonDown = false;
				if(r.InPoint(pEvent->Pos)==true)
				{
					OnClick(pEvent);
				}

				OnMouseUp(pEvent);
			}

		}
		break;
	case MWM_RBUTTONUP:
		{
			MRECT r = GetInitialClientRect();
			if(m_bRButtonDown==true)
			{
				ReleaseCapture();
				m_bRButtonDown = false;
				if(r.InPoint(pEvent->Pos)==true)
				{
					OnRClick(pEvent);
				}

				OnMouseUp(pEvent);
			}
		}
		break;
	case MWM_LBUTTONDBLCLK:
		{
			MRECT r = GetInitialClientRect();
			if(r.InPoint(pEvent->Pos)==true)
			{
				OnDoubleClick(pEvent);
			}
		}
		break;

	case MWM_RBUTTONDBLCLK:
		{
			MRECT r = GetInitialClientRect();
			if(r.InPoint(pEvent->Pos)==true)
			{
				OnDoubleClick(pEvent);
			}
		}
		break;
	case MWM_KEYDOWN:
		{
			OnKeyDown(pEvent);
		}
		break;
	case MWM_KEYUP:
		{
			OnKeyUp(pEvent);
		}
		break;
	};
	return false;
}

void MWidget::OnSetFocus(void)
{
	m_EventHandler.OnCommand(this, MEVENT_ENTER);
}

void MWidget::OnReleaseFocus(void)
{
	m_EventHandler.OnCommand(this, MEVENT_LEAVE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MWidget::Run(void)
{
	OnRun();

	for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
	{
		MWidget* pCurWnd = (*itor);
		pCurWnd->Run();
	}

}



MRECT _intersect( const MRECT& r, int x, int y, int w, int h)
{
	int x1 = max( r.x, x);
	int y1 = max( r.y, y);
	int x2 = min( r.x + r.w,  x + w);
	int y2 = min( r.y + r.h,  y + h);

	return MRECT( x1,  y1,  x2 - x1,  y2 - y1);
}

// 위젯별 속도 프로파일링 할때
//#define ENABLE_WIDGET_PROFILE

void MWidget::Draw(MDrawContext* pDC)
{
	if ( !m_bVisible || (m_Rect.w < 1) || (m_Rect.h < 1))
		return;

#ifdef ENABLE_WIDGET_PROFILE
	Mint::GetInstance()->BeginDrawWidget(GetClassName());
#endif


	// Get parent window clipping region, origin and opacity
	MPOINT	old_orgn = pDC->GetOrigin();
	MRECT	old_clip = pDC->GetClipRect();
	float	old_opacity = pDC->GetOpacity();

	// Get window clipping region, origin and opacity
	MPOINT	orgn( old_orgn.x + m_Rect.x,  old_orgn.y + m_Rect.y);
	MRECT	clip = _intersect( old_clip, orgn.x, orgn.y, m_Rect.w, m_Rect.h);
	if ( clip.w <= 0  ||  clip.h <= 0)
		return;
	
	MPOINT offset( old_orgn.x - old_clip.x, old_orgn.y - old_clip.y);
	MPOINT pt( m_Rect.x + offset.x, m_Rect.y + offset.y);
	if ( pt.x > old_clip.w  ||  pt.y > old_clip.h  ||  (pt.x + m_Rect.w) < 0  ||  (pt.y + m_Rect.h) < 0)
		return;


	// Set clipping region, origin and opacity
	if ( m_bClipByParent)	pDC->SetClipRect( clip);
	pDC->SetOrigin( orgn);
	pDC->SetOpacity( old_opacity * m_fOpacity);


	// Set font
	if ( m_pFont)	pDC->SetFont( m_pFont);
	else			pDC->SetFont( MFontManager::Get( NULL));


	// Draw widget
	OnDraw(pDC);


	// Draw designer mode
	if ( MIsDesignerMode()  && IsEnableDesignerMode())
	{
#define DM_CORNER_INDICATOR_SIZE 6

		// Corner Indicator
		MRECT sr(0,0,MGetWorkspaceWidth(), MGetWorkspaceHeight());
		pDC->SetClipRect(sr);
		pDC->SetColor(MCOLOR(255,255,255, 128));
		pDC->FillRectangle(m_Rect.w-DM_CORNER_INDICATOR_SIZE/2, m_Rect.h-DM_CORNER_INDICATOR_SIZE/2, DM_CORNER_INDICATOR_SIZE, DM_CORNER_INDICATOR_SIZE);

		// Bounds Info
		if ( m_nDMDragWidget != 0)
		{
			char temp[64];
			sprintf(temp, "(%d, %d, %d, %d) %s", m_Rect.x, m_Rect.y, m_Rect.w, m_Rect.h, m_strName.c_str());
			pDC->SetColor(MCOLOR(0,0,0, 255));
			pDC->Text(0,-GetFont()->GetHeight(), temp);

			pDC->SetColor(MCOLOR(255,255,255,255));
			pDC->Text(-1,-1-GetFont()->GetHeight(), temp);
		}

		// Designer에 의해 수정된 위젯은 외곽선을 따로 그려준다.
		if ( m_bAddedByDesigner == true)
		{
			pDC->SetColor(255, 0, 0, 196);
			pDC->Rectangle(0, 0, m_Rect.w, m_Rect.h);
		}
		else if ( m_bModifiedByDesigner == true)
		{
			pDC->SetColor(0, 255, 0, 196);
			pDC->Rectangle(0, 0, m_Rect.w, m_Rect.h);
		}
	}


	// Draw child widgets
	if ( m_Children.size() > 0)
	{
		for ( list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
		{
			MWidget* pCurWnd = (*itor);
			if ( pCurWnd == NULL)
				continue;

			if ( pCurWnd != GetLatestExclusive())
				pCurWnd->Draw( pDC);
		}
	}


	// Draw exclusive widget
	if ( GetLatestExclusive())
		GetLatestExclusive()->Draw( pDC);


	// Restore clipping region, origin and opacity
	if ( m_bClipByParent)	pDC->SetClipRect( old_clip);
	else					pDC->FlushFont();
	pDC->SetOrigin( old_orgn);
	pDC->SetOpacity( old_opacity);


#ifdef ENABLE_WIDGET_PROFILE
	Mint::GetInstance()->EndDrawWidget(GetClassName());
#endif

}


/*
void MWidget::Draw3D(MDrawContext3D* pDC)
{
	if(m_bVisible==true) OnDraw3D(pDC);
	else return;

	for(int i=0; i<m_Children.GetCount(); i++){
		MWidget* pCurWnd = m_Children.Get(i);
		pCurWnd->Draw3D(pDC);
	}
}
*/

void MWidget::Redraw(void)
{
	Mint::GetInstance()->Update();
}

bool MWidget::Event(MEvent* pEvent)
{
	if(m_bVisible==false) return false;	// 보이는 윈도우만 이벤트를 처리할 수 있다.

	// Look에서 정의한 Client영역이 아닌 실제 Client 영역
	//MRECT r = GetClientRect();
	MRECT r = GetRect();
	r.x = r.y = 0;

	MEvent LocalEvent;
	MakeLocalEvent(&LocalEvent, pEvent);


	// Tooltip
	if ( IsVisible()  &&  r.InPoint(LocalEvent.Pos)  && (MWidget::m_pCapturedWidget == NULL || MWidget::m_pCapturedWidget == this))
		Mint::GetInstance()->SetTooltip( this, pEvent->Pos, m_strTooltip);


	if(m_bEnable==false) return false;

	//////////////////////////////////////////////////////////////////////////
	// exclusive 일때의 처리
//	if(m_bVisibleChildren==true && GetLatestExclusive()!=NULL){
	if(GetLatestExclusive() != NULL)
	{
		if(GetLatestExclusive()->Event(pEvent) == true) 
			return true;

//		if(GetLatestExclusive() != NULL) 
		return false;	// 이벤트 핸들러에 의해 Exclusive가 풀리는 경우가 있으면 다음으로 넘어간다.
	}

	//////////////////////////////////////////////////////////////////////////
	// 클릭한 위젯을 앞으로 올려준다
	if(pEvent->nMessage == MWM_LBUTTONDOWN)
	{
		if(r.InPoint(LocalEvent.Pos) == true)
		{
			SetFocus();
 			if((m_bFocusEnable) && (m_bZOrderChangable == true)) 
				SetZOrder(MZ_TOP);	// 해당 위젯을 맨 위로 올려준다.
		}
		else	// ReleaseFocus
		{
//			ReleaseFocus();
//			OnReleaseFocus();
//			ReleaseCapture();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// zorder 가 앞쪽인 자식부터 순회
	m_bIterating = true;
	for(list<MWidget*>::reverse_iterator itor = m_Children.rbegin(); itor != m_Children.rend(); ++itor)
	{
		MWidget* pCurWnd = (*itor);
		
		// 마우스 이벤트일경우 커서가 위젯위에 있는경우에만 이벤트 전달
		// TODO: 현재 over draw 등등 몇몇 기능이 이벤트를 받아야만 동작하도록 되어있어서 
		// 주석처리해두었다.
// 		if(pEvent->IsMouseEvent() && !pCurWnd->GetScreenRect().InPoint( pEvent->Pos))
// 			continue;

		if(pCurWnd->Event(pEvent) == true) 
		{
			m_bIterating = false;
			ExecuteReservedZOrder();
			return true;
		}
	}
//	}
	m_bIterating = false;
	ExecuteReservedZOrder();

	//////////////////////////////////////////////////////////////////////////
	// Designer Mode
	if(MIsDesignerMode()==true && IsEnableDesignerMode()==true)
	{
		MPOINT sp = pEvent->Pos;
		MWidget* pParent = GetParent();
		if(pParent==NULL) return false;

		// Widget Resize
		if(pEvent->nMessage==MWM_LBUTTONDOWN)
		{
			MRECT Corner(m_Rect.w-DM_CORNER_INDICATOR_SIZE/2, m_Rect.h-DM_CORNER_INDICATOR_SIZE/2, DM_CORNER_INDICATOR_SIZE, DM_CORNER_INDICATOR_SIZE);
			if(Corner.InPoint(LocalEvent.Pos)==true)
			{
				SetCapture();
				m_nDMDragWidget = 1;
				m_DMDragPoint = sp;
				return true;
			}
		}

		// Widget Move
		if(pEvent->nMessage==MWM_LBUTTONDOWN)
		{
			if(r.InPoint(LocalEvent.Pos)==true)
			{
				SetCapture();
				m_nDMDragWidget = 2;
				MPOINT wp = MClientToScreen(pParent, MPOINT(m_Rect.x, m_Rect.y));
				sp.x -= wp.x;
				sp.y -= wp.y;
				m_DMDragPoint = sp;
				return true;
			}
		}

		if(m_nDMDragWidget!=0 && pEvent->nMessage==MWM_LBUTTONUP)
		{
			ReleaseCapture();
			m_nDMDragWidget = 0;
			return true;
		}

		if(pEvent->nMessage==MWM_MOUSEMOVE)
		{
			if(m_nDMDragWidget==1)	// Widget Resize
			{		
				if(sp.x<0) sp.x = 0;
				if(sp.y<0) sp.y = 0;
				if(sp.x>MGetWorkspaceWidth()-1) sp.x = MGetWorkspaceWidth()-1;
				if(sp.y>MGetWorkspaceHeight()-1) sp.y = MGetWorkspaceHeight()-1;

				// Snap
				int sx = sp.x%MGetDesignerModeSnapSize();
				if(sx<MGetDesignerModeSnapSize()/2) sp.x -= sx;
				else sp.x += (MGetDesignerModeSnapSize()-sx);
				int sy = sp.y%MGetDesignerModeSnapSize();
				if(sy<MGetDesignerModeSnapSize()/2) sp.y -= sy;
				else sp.y += (MGetDesignerModeSnapSize()-sy);

				MPOINT p = MScreenToClient(this, sp);

				SetSize(p.x, p.y);

				m_bModifiedByDesigner = true;

				return true;
			}
			else if(m_nDMDragWidget==2)	// Widget Move
			{		
				sp.x -= m_DMDragPoint.x;
				sp.y -= m_DMDragPoint.y;
				if(sp.x<0) sp.x = 0;
				if(sp.y<0) sp.y = 0;
				if(sp.x+m_Rect.w>MGetWorkspaceWidth()-1) sp.x = MGetWorkspaceWidth()-m_Rect.w-1;
				if(sp.y+m_Rect.h>MGetWorkspaceHeight()-1) sp.y = MGetWorkspaceHeight()-m_Rect.h-1;

				// Snap
				int sx = sp.x%MGetDesignerModeSnapSize();
				if(sx<MGetDesignerModeSnapSize()/2) sp.x -= sx;
				else sp.x += (MGetDesignerModeSnapSize()-sx);
				int sy = sp.y%MGetDesignerModeSnapSize();
				if(sy<MGetDesignerModeSnapSize()/2) sp.y -= sy;
				else sp.y += (MGetDesignerModeSnapSize()-sy);

				MPOINT p = MScreenToClient(GetParent(), sp);

				SetPosition(p.x, p.y);

				m_bModifiedByDesigner = true;

				return true;
			}
		}

		if(pEvent->nMessage==MWM_RBUTTONDOWN)
		{
			if(r.InPoint(LocalEvent.Pos)==true)
			{
				MShowDesignerMenu(pEvent->Pos, this);
				return true;
			}
		}
		else if(pEvent->nMessage==MWM_RBUTTONUP)
		{
		}

		return false;
	}

	// 위젯 리사이즈
	// 클라이언트 영역을 벗어나서 제어해야하므로, Event()에서 처리한다.
	if(EventResize(pEvent)==true) return true;

	if(pEvent->nMessage==MWM_HOTKEY){
		if(OnHotKey(pEvent->nKey)==true) return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// 각 위젯의 이벤트 처리 : OnEvent

	// Focused Event
	if(pEvent->nMessage==MWM_CHAR			|| 
	   pEvent->nMessage==MWM_KEYDOWN		|| 
	   pEvent->nMessage==MWM_IMECOMPOSE		||
       pEvent->nMessage==MWM_ACTIONKEYDOWN	|| 
	   pEvent->nMessage==MWM_ACTIONKEYUP	|| 
	   pEvent->nMessage==MWM_ACTIONPRESSED	|| 
	   pEvent->nMessage==MWM_ACTIONRELEASED  )
	{
		if (IsFocus()) {	// KEY Event는 Focus 없는 위젯에는 의미없다
			//먼저 자식에게로 전해져서 처리되면 ok 처리한 자식이 없으면 부모에게까지 간다
			if(OnEvent(&LocalEvent, GetListener())==true) 
				return true;

			// Tab Key
			if(pEvent->nMessage==MWM_KEYDOWN && pEvent->nKey==VK_TAB){
				if(OnTab(!pEvent->GetShiftState())==true) 
					return true;
			}

			/*
			// Focused Widget에게 우선권이 있으며...
			if(MWidget::m_pFocusedWidget==this){	// MWM_CHAR 메세지인 경우 현재 포커스된 윈도우에게만 메세지가 보내진다.
				if(OnEvent(&LocalEvent, GetListener())==true) 
					return true;
			}
			*/
		}
	}
	else{
		// Capture가 걸릴 경우 Capture된 윈도우에만 메세지 보내기
		if((MWidget::m_pCapturedWidget==NULL || MWidget::m_pCapturedWidget==this))	
			if(OnEvent(&LocalEvent, GetListener())==true) 
				return true;
	}

	if(pEvent->nMessage==MWM_LBUTTONUP) ReleaseCapture();

	return false;
}

bool MWidget::EventAccelerator(MEvent* pEvent)
{
	if(m_bVisible==false) return false;	// 보이는 윈도우만 이벤트를 처리할 수 있다.
	if(m_bEnable==false) return false;

	if(!(pEvent->nMessage==MWM_KEYDOWN || pEvent->nMessage==MWM_SYSKEYDOWN)) return false;

	// KEYDOWN일때 Edit에 Focus가 있을 경우 Accelerator는 무시된다. (SYSKEYDOWN만 유효하게 된다)
	if(pEvent->nMessage==MWM_KEYDOWN)
		if(MWidget::m_pFocusedWidget!=NULL)
			if(strcmp(MWidget::m_pFocusedWidget->GetClassName(), MINT_EDIT)==0) return false;

	if(GetLatestExclusive()!=NULL){
		if(GetLatestExclusive()->EventAccelerator(pEvent)==true) return true;
		if(GetLatestExclusive()!=NULL) return false;	// 이벤트 핸들러에 의해 Exclusive가 풀리는 경우가 있으면 다음으로 넘어간다.
	}

	if(GetLatestExclusive()==NULL){
		for (list<MWidget*>::reverse_iterator itor = m_Children.rbegin(); itor != m_Children.rend(); ++itor)
		{
			MWidget* pCurWnd = (*itor);
			if(pCurWnd->EventAccelerator(pEvent)==true) return true;
		}
	}

	// Accelerator
	char szKey[2] = {(char)pEvent->nKey, 0};
//	_strupr(szKey);
	if(m_nAccelerator==szKey[0]) {

		m_bEventAcceleratorCall = true;

		if(DefaultCommand()==true) {
			m_bEventAcceleratorCall = false;
			return true;
		}
		m_bEventAcceleratorCall = false;
	}

	return false;
}

bool MWidget::EventDefaultKey(MEvent* pEvent)
{
	if(m_bVisible==false) return false;	// 보이는 윈도우만 이벤트를 처리할 수 있다.
	if(m_bEnable==false) return false;

	if(!(pEvent->nMessage==MWM_KEYDOWN))return false;

	if(GetLatestExclusive()!=NULL){
		if(GetLatestExclusive()->EventDefaultKey(pEvent)==true) return true;
		if(GetLatestExclusive()!=NULL) return false;	// 이벤트 핸들러에 의해 Exclusive가 풀리는 경우가 있으면 다음으로 넘어간다.
	}

	if(GetLatestExclusive()==NULL){
		for (list<MWidget*>::reverse_iterator itor = m_Children.rbegin(); itor != m_Children.rend(); ++itor)
		{
			MWidget* pCurWnd = (*itor);
			if(pCurWnd->EventDefaultKey(pEvent)==true) return true;
		}
	}

	if(strcmp(GetClassName(), MINT_BUTTON)!=0) return false;

	MButton* pButton = (MButton*)this;
	if((pButton->m_nKeyAssigned==MBKA_ENTER && pEvent->nKey==VK_RETURN) ||
		(pButton->m_nKeyAssigned==MBKA_ESC && pEvent->nKey==VK_ESCAPE))
		if(DefaultCommand()==true) return true;

	return false;
}

void MWidget::OnEnabled(bool bEnabled)
{

}

void MWidget::OnShow(void)
{
	m_EventHandler.OnCommand(this, MEVENT_SHOW);
	m_bLButtonDown = false;
	m_bRButtonDown = false;


	for (list<MWidget*>::reverse_iterator itor = m_Children.rbegin(); itor != m_Children.rend(); ++itor)
	{
		MWidget* pCurWnd = (*itor);
		pCurWnd->OnShow();
	}
}

void MWidget::OnHide(void)
{
	m_EventHandler.OnCommand(this, MEVENT_HIDE);
	if(MWidget::m_pFocusedWidget==this) ReleaseFocus();

}

void MWidget::OnSize(int w, int h)
{

}

void MWidget::OnClick(MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent(pEvent);

	m_EventHandler.OnCommand(this, MEVENT_CLICK);
}

void MWidget::OnRClick(MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent(pEvent);

	m_EventHandler.OnCommand(this, MEVENT_RCLICK);
}

void MWidget::OnMouseDown(MEvent* pEvent)
{
	m_EventHandler.OnCommand(this, MEVENT_MOUSE_DOWN);
}

void MWidget::OnMouseUp(MEvent* pEvent)
{
	m_EventHandler.OnCommand(this, MEVENT_MOUSE_UP);
}

void MWidget::OnMouseWheel(MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent(pEvent);

	m_EventHandler.OnCommand(this, MEVENT_MOUSE_WHEEL);
}

void MWidget::OnDoubleClick(MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent(pEvent);

	m_EventHandler.OnCommand(this, MEVENT_DOUBLE_CLICK);
}

void MWidget::OnKeyDown(MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent(pEvent);

	m_EventHandler.OnCommand(this, MEVENT_KEY_DOWN);
}

void MWidget::OnKeyUp(MEvent* pEvent)
{
	m_EventHandler.OnCommand(this, MEVENT_KEY_UP);
}

void MWidget::OnItemSelected( MEvent* pEvent )
{
	MWidget::m_EventArgs.SetFromEvent(pEvent);

	m_EventHandler.OnCommand(this, MEVENT_LB_ITEM_SEL);
}

/*
void MWidget::OnReSize()
{
}
*/


bool MWidget::OnTab(bool bForward)
{
	MWidget* pParent = GetParent();
	if(pParent==NULL) return false;

	if(m_pParent->GetLatestExclusive()==this) return false;

	int nThisIndex = pParent->GetChildIndex(this);
	if(nThisIndex<0) return false;

	for(int i=0; i<pParent->GetChildCount(); i++){
		int nIndex = 0;
		if(bForward==true) nIndex = (nThisIndex+i+1) % pParent->GetChildCount();
		else nIndex = (nThisIndex+pParent->GetChildCount()-1-i) % pParent->GetChildCount();

		MWidget* pSibling = pParent->GetChild( nIndex );
		if(pSibling->IsFocusEnable()==true && pSibling!=this && pSibling->IsVisible()==true && pSibling->IsEnable()==true){
			pSibling->SetFocus();
			return true;
		}
	}

	return false;
}

bool MWidget::OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	m_DragEventArgs.Set(pSender, MEvent::GetMouseClientPos().x,
		MEvent::GetMouseClientPos().y, MEvent::GetCtrlState(), szString, szItemString);
	m_EventHandler.OnCommand(this, MEVENT_DROP);

	return false;
}

bool MWidget::OnUnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	return false;
}

void MWidget::ResizeChildrenByAnchor(int w, int h)
{
	int dw = w - m_Rect.w;
	int dh = h - m_Rect.h;


	for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
	{
		MWidget* pChild = (*itor);

//		if ( pChild->GetBoundsAlignment() != 0)
//		{
//			// Bounds Alignment에 우선순위가 있다.
//			pChild->SetBoundsAlignment(pChild->GetBoundsAlignment(), w, h);
//			continue;
//		}


		// 앵커로 위치 및 크기 변환
		MRECT r = pChild->m_Rect;

		if ( (pChild->m_Anchor.bLeft == true) && (pChild->m_Anchor.bRight == true))		r.w += dw;
		else if ( pChild->m_Anchor.bRight == true)										r.x += dw;
//		else if ( pChild->m_Anchor.bLeft == true)										;
//		else																			r.x += dw >> 1;

		if ( (pChild->m_Anchor.bTop == true) && (pChild->m_Anchor.bBottom == true))		r.h += dh;
		else if (pChild->m_Anchor.bBottom == true)										r.y += dh;
//		else if (pChild->m_Anchor.bTop == true)											;
//		else																			r.y += dh >> 1;


		// 크기와 상대 정보 설정
		pChild->SetBounds( r);
	}
}

void MWidget::Show(bool bVisible, bool bModal)
{
	if(m_bVisible==bVisible){
		if(bModal==true){
			if(m_pParent!=NULL && m_pParent->GetLatestExclusive()==this)
				return;
		}
		else return;
	}

	m_bVisible = bVisible;

	if(bVisible==true && bModal==true) SetExclusive();
	else if(bVisible==false) {
		ReleaseExclusive();
		if(MWidget::m_pFocusedWidget==this) ReleaseFocus();
	}

	if (bVisible) 
	{
		OnShow();
		//SetZOrder(MZ_TOP);	// 보이는 거라면 처음으로 보낸다.
	}
	else 
	{
		OnHide();
		//SetZOrder(MZ_BOTTOM);  // 숨기는 거라면 마지막으로 보낸다.
	}
}

void MWidget::Enable(bool bEnable)
{
	if (m_bEnable == bEnable) return;
	m_bEnable = bEnable;
	OnEnabled(bEnable);
}

bool MWidget::IsVisible(void)
{
	return m_bVisible;
}

bool MWidget::IsEnable(void)
{
	return m_bEnable;
}

void MWidget::SetResizable(bool bEnable)
{
	m_bResizable = bEnable;
}

bool MWidget::IsResizable(void)
{
	return m_bResizable;
}

void MWidget::SetListener(MListener* pListener)
{
	m_EventHandler.SetListener(pListener);
	//m_pListener = pListener;
}

MListener* MWidget::GetListener(void)
{
	return m_EventHandler.GetListener();
}

int MWidget::GetID(void)
{
	return m_nID;
}

void MWidget::SetID(int nID)
{
	m_nID = nID;
}

void MWidget::SetText(const char* szText)
{
	if(szText==NULL) return;
//	_ASSERT(strlen(szText)<MWIDGET_TEXT_LENGTH);
//	if (strlen(szText) >= MWIDGET_TEXT_LENGTH) return;

	m_strText = szText;

	SetLabelAccelerator();
	//if(m_nAcceleratorType==1) UseAcceleratorAndCharacter(szText);
}

const char* MWidget::GetText(void)
{
	return m_strText.c_str();
}

void MWidget::SetCapture(void)
{
	MWidget::m_pCapturedWidget = this;
}

void MWidget::ReleaseCapture(void)
{
	MWidget::m_pCapturedWidget = NULL;
}

void MWidget::SetFocusEnable(bool bEnable)
{
	m_bFocusEnable = bEnable;
}

bool MWidget::IsFocusEnable(void)
{
	return m_bFocusEnable;
}

void MWidget::SetFocus(void)
{
	if(m_bFocusEnable==false) return;
	/*
	if(m_pParent!=NULL){
		if(m_pParent->m_pExclusive!=NULL && m_pParent->m_pExclusive!=MWidget::m_pFocusedWidget) return;
	}
	*/
	// 자식중에 Exclusive 위젯이 있는경우,
	// 현재 Exclusive 위젯과 그 Children에게만 Focus가 갈 수 있도록 한다.
	MWidget* pExDes = FindExclusiveDescendant();
	if(pExDes!=NULL) return;
	/*
	if(MWidget::m_pFocusedWidget!=NULL){
		if(MWidget::m_pFocusedWidget->m_pParent!=NULL){
			if(MWidget::m_pFocusedWidget->m_pParent->m_pExclusive==MWidget::m_pFocusedWidget){
				return;
			}
		}
	}
	*/

	if(MWidget::m_pFocusedWidget==this) return;

	if(MWidget::m_pFocusedWidget!=NULL) MWidget::m_pFocusedWidget->OnReleaseFocus();

	MWidget::m_pFocusedWidget = this;
	OnSetFocus();
}

void MWidget::ReleaseFocus(void)
{
	if(MWidget::m_pFocusedWidget==this) OnReleaseFocus();
	MWidget::m_pFocusedWidget = NULL;
}

bool MWidget::IsFocus(void)
{
	if(MWidget::m_pFocusedWidget==this) return true;
	return false;
}

void MWidget::SetParent( MWidget* pParent )
{
	RemoveFromParent();
	m_pParent = pParent;

	if (m_pParent)
	{
		m_pParent->AddChild(this);
	}
}

MWidget* MWidget::GetParent(void)
{
	return m_pParent;
}

int MWidget::GetChildCount(void)
{
	return (int)m_Children.size();
}

MWidget* MWidget::GetChild(int i)
{
	list<MWidget*>::iterator itor = m_Children.begin();
	for (int index = 0; index < i; ++index, ++itor)
	{
		if (itor == m_Children.end()) break;
	}

	if (itor != m_Children.end()) return (*itor);
	return NULL;
}

int MWidget::GetChildIndex(MWidget* pWidget)
{
	int index = 0;
	for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
	{
		MWidget* pChild = (*itor);
		if(pChild==pWidget) return index;
		index++;
	}

	return -1;
}

void MWidget::SetExclusive(void)
{
	if(m_pParent!=NULL){
		m_pParent->AddExclusive(this);
		SetFocus();	// Exclusive로 보여질때 포커싱.
	}
}

void MWidget::ReleaseExclusive(void)
{
	if(m_pParent!=NULL)
		m_pParent->RemoveExclusive(this);
}

MCursor* MWidget::SetCursor(MCursor* pCursor)
{
	MCursor* pTemp = m_pCursor;
	m_pCursor = pCursor;
	return pTemp;
}

MCursor* MWidget::GetCursor(void)
{
	return m_pCursor;
}

MFont* MWidget::SetFont(MFont* pFont)
{
	MFont* pTemp = m_pFont;
	m_pFont = pFont;
	return pTemp;
}

MFont* MWidget::GetFont(void)
{
	if(m_pFont==NULL) return MFontManager::Get(NULL);
	else return m_pFont;
}

void MWidget::SetSize(int w, int h)
{
//	if(w<0) w = 1;
//	if(h<0) h = 1;


	// TODO: 100,100 으로 지정하면 default size 가 되어 onsize가 불리지 않음.
	if(w==m_Rect.w && h==m_Rect.h) return;

	ResizeChildrenByAnchor(w, h);

	m_Rect.w = w;
	m_Rect.h = h;

	OnSize(w, h);
}

void MWidget::SetSize(MSIZE& s)
{
	SetSize(s.w, s.h);
}


void MWidget::SetPosition(int x, int y)
{
	m_Rect.x = x;
	m_Rect.y = y;
}

void MWidget::SetPosition(MPOINT& p)
{
	SetPosition(p.x, p.y);
}

void MWidget::SetBounds(MRECT& r)
{
	SetBounds(r.x, r.y, r.w, r.h);
}

void MWidget::SetBounds(int x, int y, int w, int h)
{
	SetPosition(x, y);

//	if(w<0) w = 1;
//	if(h<0) h = 1;

	SetSize(w, h);
	/*
	bool bResized = false;
	if(w!=m_Rect.w || h!=m_Rect.h){
		m_Rect.w = w;
		m_Rect.h = h;
		bResized = true;
	}
	*/

	//if(bResized==true) OnSize(w, h);
}

MPOINT MWidget::GetPosition(void)
{
	return MPOINT(m_Rect.x, m_Rect.y);
}

MRECT MWidget::GetRect(void)
{
	return m_Rect;
}

string& MWidget::GetName()
{
	return m_strName;
}

void MWidget::GetBoundsAlignmentPosition(MPOINT* p, MAlignmentMode am, int w, int h)
{
	p->x = m_Rect.x;
	p->y = m_Rect.y;

	MSIZE s(w, h);
	if(w==-1){
		s.w = MGetWorkspaceWidth();
		if(GetParent()!=NULL) s.w = GetParent()->m_Rect.w;
	}
	if(h==-1){
		s.h = MGetWorkspaceHeight();
		if(GetParent()!=NULL) s.h = GetParent()->m_Rect.h;
	}

	if(m_BoundsAlignment&MAM_LEFT) p->x = 0;
	else if(m_BoundsAlignment&MAM_RIGHT) p->x = s.w-m_Rect.w;
	else if(m_BoundsAlignment&MAM_HCENTER) p->x = (s.w-m_Rect.w)/2;

	if(m_BoundsAlignment&MAM_LEFT) p->y = 0;
	else if(m_BoundsAlignment&MAM_BOTTOM) p->y = s.h-m_Rect.h;
	else if(m_BoundsAlignment&MAM_VCENTER) p->y = (s.h-m_Rect.h)/2;
}

void MWidget::SetBoundsAlignment(MAlignmentMode am, int w, int h)
{
	m_BoundsAlignment = am;

	MPOINT p;
	GetBoundsAlignmentPosition(&p, m_BoundsAlignment, w, h);

	// 포지션 정렬에 의해 재설정
	m_Rect.x = p.x;
	m_Rect.y = p.y;
}

MAlignmentMode MWidget::GetBoundsAlignment(void)
{
	return m_BoundsAlignment;
}


MRECT MWidget::GetScreenRect(void)
{
	if(m_pParent!=NULL){
		MRECT sr = m_pParent->GetScreenRect();
		MRECT r = m_Rect;
		r.Offset(sr.x, sr.y);
		return r;
	}

	return m_Rect;
}

void MWidget::SetTooltipMsg( const char* szToolTipMsg)
{
	m_strTooltip = szToolTipMsg;
}

const char* MWidget::GetTooltipMsg()
{
	return m_strTooltip.c_str();
}

void MWidget::SetHelpMsg( const char* szHelpMsg)
{
	m_strHelpMsg = szHelpMsg;
}

const char* MWidget::GetHelpMsg()
{
	return m_strHelpMsg.c_str();
}

void MWidget::SetAccelerator(int a)
{
	//m_nAcceleratorType = 0;	// use custom accelerator type
	if(a==0) m_nAccelerator = -1;
	else m_nAccelerator = a;
}

void MWidget::SetLabelAccelerator(void)
{
	SetAccelerator(GetLabelAccelerator());
}

char MWidget::GetLabelAccelerator(void)
{
	char szAnd[2] = {GetAndChar( GetText()), 0};
	_strupr(szAnd);
	if(szAnd[0]==0) return -1;
	return szAnd[0];
}

/*
void MWidget::SetLabelAccelerator(void)
{
	m_nAcceleratorType = 1;	// use label's & character
	UseAcceleractorAndCharacter(m_szText);
}

void MWidget::SetToolTipAccelerator(void)
{
	m_nAcceleratorType = 2;	// use tooltip's & character
	if(m_pToolTip!=NULL)
		UseAcceleractorAndCharacter(m_pToolTip->m_szText);
	else
		UseAcceleractorAndCharacter(NULL);
}

void MWidget::EnableAccelerator(bool bEnabled)
{
	m_bAcceleratorEnabled = bEnabled;
}
*/

MRECT MWidget::GetClientRect(void)
{
	return MRECT(0, 0, m_Rect.w, m_Rect.h);
}

MRECT MWidget::GetInitialClientRect(void)
{
	return MWidget::GetClientRect();
}

void MWidget::SetZOrder(MZOrder z)
{
	if(m_pParent==NULL) return;

	MWidget* pParent = m_pParent;

	// 이터레이팅 중일때는 예약
	if(pParent->m_bIterating)
	{
		ReserveZOrder(z);
		return;
	}

	//pParent->RemoveChild(this);
	RemoveFromParent();

	switch(z){
	case MZ_TOP:
		pParent->AddChild(this);
		break;
	case MZ_BOTTOM:
		pParent->InsertChild(this);
		break;
	}
}

void MWidget::ReserveZOrder(MZOrder z)
{
	if(m_pParent==NULL) return;

	ZORDERRESERVED reservedItem = { this , z  };
	m_pParent->m_reservedZOrder.push_back( reservedItem );
}

void MWidget::ExecuteReservedZOrder()
{
	_ASSERT( !m_bIterating);

	for( list< ZORDERRESERVED >::iterator itr = m_reservedZOrder.begin(); itr!= m_reservedZOrder.end(); ++itr )
	{
		MWidget* pWidget = (*itr).pWidget;
		_ASSERT( pWidget->m_pParent == this);
		_ASSERT( !pWidget->m_pParent->m_bIterating);
		(*itr).pWidget->SetZOrder( (*itr).zOrder );
	}
	m_reservedZOrder.clear();
}


MWidget* MWidget::FindExclusiveDescendant(void)
{
	if(!m_Exclusive.empty()) 
	{
		return (m_Exclusive.back());
	}

	for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
	{
		MWidget* pChild = (*itor);
		MWidget* pExDes = pChild->FindExclusiveDescendant();
		if(pExDes!=NULL) return pExDes;
	}

	//for(int i=0; i<m_Children.GetCount(); i++){
	//	MWidget* pChild = m_Children.Get(i);
	//	MWidget* pExDes = pChild->FindExclusiveDescendant();
	//	if(pExDes!=NULL) return pExDes;
	//}

	return NULL;
}

bool MWidget::IsExclusive(MWidget* pWidget)
{
	for (list<MWidget*>::iterator itor = m_Exclusive.begin(); itor != m_Exclusive.end(); ++itor)
	{
		MWidget* pThis = (*itor);
		if(pThis==pWidget) return true;
	}

	//for(int i=0; i<m_Exclusive.GetCount(); i++){
	//	MWidget* pThis = m_Exclusive.Get(i);
	//	if(pThis==pWidget) return true;
	//}
	return false;
}


MWidget* MWidget::Find(MPOINT& p)
{
	if(IsVisible()==false) return NULL;

	for (list<MWidget*>::reverse_iterator itor = m_Children.rbegin(); itor != m_Children.rend(); ++itor)
	{
		MWidget* pChild = (*itor);
		MWidget* pFind = pChild->Find(p);
		if(pFind!=NULL) return pFind;
	}

	//for(int i=0; i<m_Children.GetCount(); i++){
	//	MWidget* pChild = m_Children.Get(m_Children.GetCount()-i-1);
	//	MWidget* pFind = pChild->Find(p);
	//	if(pFind!=NULL) return pFind;
	//}

	if(GetScreenRect().InPoint(p)==true)
		return this;

	return NULL;
}

MWidget* MWidget::FindDropAble(MPOINT& p)
{
	if(IsVisible()==false) return NULL;

	for (list<MWidget*>::reverse_iterator itor = m_Children.rbegin(); itor != m_Children.rend(); ++itor)
	{
		MWidget* pChild = (*itor);
		MWidget* pFind = pChild->FindDropAble(p);
		if(pFind!=NULL) return pFind;
	}

	//for(int i=0; i<m_Children.GetCount(); i++){
	//	MWidget* pChild = m_Children.Get(m_Children.GetCount()-i-1);
	//	MWidget* pFind = pChild->FindDropAble(p);
	//	if(pFind!=NULL) return pFind;
	//}

	if(GetScreenRect().InPoint(p)==true)
		if(m_bFocusEnable==true)
			return this;

	return NULL;
}


bool MWidget::Drop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	return OnDrop(pSender, pBitmap, szString, szItemString);
}

bool MWidget::UnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	return OnUnDrop(pSender, pBitmap, szString, szItemString);
}

void MWidget::GetHierarchicalName(char* szName)
{
	// TODO: 여긴 m_szName으로 바꿔줘야 함.
	if(m_pParent!=NULL){
		m_pParent->GetHierarchicalName(szName);
		strcat(szName, "/");
		strcat(szName, GetText());
	}
	else{
		strcpy(szName, GetText());
	}
}

MWidget* MWidget::FindWidgetByHierarchicalName(const char* szName)
{
	char szHierachicalName[2048];
	GetHierarchicalName(szHierachicalName);
	if(strcmp(szName, szHierachicalName)==0){
		return this;
	}

	for(int i=0; i<GetChildCount(); i++){
		MWidget* pFind = GetChild(i)->FindWidgetByHierarchicalName(szName);
		if(pFind!=NULL) return pFind;
	}

	return NULL;
}

bool MWidget::IsMsg(const char* szMsg1, const char* szMsg2)
{
	// 대소문자를 안가린다.
	char szTmp1[256];
	char szTmp2[256];
	strcpy(szTmp1, szMsg1);
	strcpy(szTmp2, szMsg2);
	_strupr(szTmp1);
	_strupr(szTmp2);
	if(strcmp(szTmp1, szTmp2)==0) return true;
	return false;
}

void* MWidget::Query(const char* szQuery)
{
	return NULL;
}

void MWidget::SetOpacity(float fOpacity)
{
	m_fOpacity = fOpacity;
}

float MWidget::GetOpacity()
{
	return m_fOpacity;
}

void MWidget::SetAnchor(MAnchorStyle& anchor)
{
	m_Anchor = anchor;
}

int GetAndPos(const char* szText)
{
	if(szText==NULL) return -1;
	int nLen = strlen(szText);
	for(int i=0; i<nLen; i++){
		if(szText[i]=='&') return i;
	}
	return -1;
}

char GetAndChar(const char* szText)
{
	int nLen = GetAndPos(szText);
	if(nLen==-1) return 0;
	return szText[nLen+1];
}

int RemoveAnd(char* szText)
{
	if(szText==NULL) return -1;
	int nPos = GetAndPos(szText);
	if(nPos==-1) return -1;
	int nLen = strlen(szText);
	for(int i=0; i<(nLen-nPos); i++){
		szText[nPos+i] = szText[nPos+i+1];
	}
	return nPos;
}

int RemoveAnd(char* szRemovedText, const char* szText)
{
	if(szText==NULL) return -1;
	strcpy(szRemovedText, szText);
	return RemoveAnd(szRemovedText);
}

int RemoveAnd(char* szRemovedFrontText, char* cUnderLineChar, char* szRemovedBackText, const char* szText)
{
	if(szText==NULL) return -1;
	int nPos = GetAndPos(szText);
	if(nPos==-1) return -1;
	memcpy(szRemovedFrontText, szText, nPos);
	szRemovedFrontText[nPos] = 0;
	*cUnderLineChar = szText[nPos+1];
	strcpy(szRemovedBackText, szText+nPos+2);
	return nPos;
}

MPOINT MClientToScreen(MWidget* pWidget, MPOINT& p)
{
	MRECT r = pWidget->GetScreenRect();
	return MPOINT(p.x+r.x, p.y+r.y);
}

MPOINT MScreenToClient(MWidget* pWidget, MPOINT& p)
{
	MRECT r = pWidget->GetScreenRect();
	return MPOINT(p.x-r.x, p.y-r.y);
}

MRECT MClientToScreen(MWidget* pWidget, MRECT& p)
{
	MRECT r = pWidget->GetScreenRect();
	return MRECT(p.x+r.x, p.y+r.y, p.w, p.h);
}

MRECT MScreenToClient(MWidget* pWidget, MRECT& p)
{
	MRECT r = pWidget->GetScreenRect();
	return MRECT(p.x-r.x, p.y-r.y, p.w, p.h);
}

MPOINT GetCursorPosition(void)
{
	POINT p;
	GetCursorPos(&p);
	MPOINT mp;
	mp.x = p.x;
	mp.y = p.y;
	return mp;
}


void MWidget::LinkScriptEvent(string& strEventName, string& strScript)
{
	MEventMsg nMsg = FindEventMsgFromName(strEventName.c_str());
	if (nMsg != MEVENT_UNKNOWN)
	{
		m_EventHandler.SetFlagScriptEvents(nMsg);
	}

	Mint::GetInstance()->GetScriptManager()->AddWidgetEvent(m_strName, strEventName, strScript);
}


////////////////////////////////////////////////////////
void MWidget::glueSetOpacity(float fOpacity)
{
	SetOpacity(fOpacity);
}

void MWidget::glueSetSize(int nWidth, int nHeight)
{
	SetSize(nWidth, nHeight);
}

void MWidget::glueSetWidth(int nWidth)
{
	SetSize(nWidth, m_Rect.h);
}

void MWidget::glueSetText(const char* szText)
{
	if (szText == NULL) return;
	SetText(szText);
}

bool MWidget::glueIsVisible()
{
	return IsVisible();
}

bool MWidget::glueIsFocus()
{
	return IsFocus();
}

void MWidget::glueSetFocus()
{
	SetFocus();
}

int MWidget::glueGetWidth()
{
	return m_Rect.w;	
}

int MWidget::glueGetHeight()
{
	return m_Rect.h;
}

const char* MWidget::glueGetName()
{
	return m_strName.c_str();
}

const char* MWidget::glueGetText()
{
	return GetText();
}

float MWidget::glueGetOpacity()
{
	return GetOpacity();
}

MWidget* MWidget::glueGetParent()
{
	return GetParent();
}

// TODO : 함수문제있음. [2/22/2008 madduck]
MWLua::ret_int2 MWidget::glueGetPosition()
{
	MWLua::ret_int2 ret(m_Rect.x, m_Rect.y);
	return ret;
}

int MWidget::glueGetPositionX()
{
	return m_Rect.x;
}

int MWidget::glueGetPositionY()
{
	return m_Rect.y;
}

void MWidget::glueBringToTop()
{
	SetZOrder( MZ_TOP);
}

void MWidget::glueSetPosition(int x, int y)
{
	SetPosition(x, y);
}

void MWidget::DeclToSctipt()
{
	Mint::GetInstance()->GetScriptManager()->DeclWidget(GetClassName(), m_strName.c_str(), this);	// 루아 오브젝트 생성
	m_bDeclToScript = true;
}

void MWidget::ResetMinSize(MRECT& rt, const MRECT& rtOrg)
{
	if(MWIDGET_WINDOW_MIN_SIZE_W > rt.w) 
	{
		rt.x = rtOrg.x;
		rt.w = rtOrg.w;
	}
	if(MWIDGET_WINDOW_MIN_SIZE_H > rt.h) 
	{
		rt.y = rtOrg.y;
		rt.h = rtOrg.h;
	}
}

void MWidget::InitMouseState() 
{ 
	m_bLButtonDown = false;
	m_bRButtonDown = false;
}

bool MWidget::IsEnableDesignerMode(void)
{
	return m_bEnableDesignerMode;
}

void MWidget::EnableDesignerMode(bool bEnable)
{
	m_bEnableDesignerMode = bEnable;
}

bool MWidget::IsModifiedByDesigner(void)
{
	return m_bModifiedByDesigner;
}

bool MWidget::IsAddedByDesignerFlag(void)
{
	return m_bAddedByDesigner;
}

} // namespace mint3

