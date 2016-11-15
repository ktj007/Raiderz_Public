#include "StdAfx.h"
#include "XDebugCategoryFrm.h"
#include "XPlayFrame.h"
#include "XGameFrameManager.h"
#include "XDebugFPSFrm.h"

XDebugCategoryFrm::XDebugCategoryFrm( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MFrame(szName, pParent, pListener)
{
	MRECT r = GetInitialClientRect();

	m_pCloseButton = new MButton(NULL, this, this);
	m_pCloseButton->SetText("▩");
//	m_pCloseButton->SetTextColor(0xFFFF0000);
	m_pCloseButton->SetRect(r.w - 16, 20, 12, 12);
	m_pCloseButton->SetAnchor(MAnchorStyle(false, true, true, false));

	m_pCategory = new mint3::MComboBox(NULL, this, this);
	m_pCategory->SetRect(0, 0, 80, 16);
	m_pCategory->SetText("카테고리");
	m_pCategory->AddString("AI");
	m_pCategory->AddString("동기화");
	m_pCategory->SetDroppedWidth(28);

	AddMenuButtons();

	ShowTitlebar( false);
	SetRect(10, 10, 982, 45);
	SetOpacity(1.0f);
	SetText("Debug Info Category");
	SetResizable(false);
}

void XDebugCategoryFrm::AddMenuButtons()
{
	m_pFPSButton = new MButton(NULL, this, this);
	m_pFPSButton->SetText("FPS");
	m_pFPSButton->SetRect(82, 0, 80, 16);

	m_pObjListButton = new MButton(NULL, this, this);
	m_pObjListButton->SetText("Object List");
	m_pObjListButton->SetRect(164, 0, 80, 16);

	m_pHelpButton = new MButton(NULL, this, this);
	m_pHelpButton->SetText("Help");
	m_pHelpButton->SetRect(246, 0, 80, 16);

	m_pNPCMonitorButton = new MButton(NULL, this, this);
	m_pNPCMonitorButton->SetText("NPC Monitor");
	m_pNPCMonitorButton->SetRect(328, 0, 80, 16);

	m_pNPCLogButton  = new MButton(NULL, this, this);
	m_pNPCLogButton->SetText("NPC Log");
	m_pNPCLogButton->SetRect(328, 20, 80, 16);

	m_pCombatButton = new MButton(NULL, this, this);
	m_pCombatButton->SetText("Combat");
	m_pCombatButton->SetRect(410, 0, 80, 16);

	m_pRenderGraphButton = new MButton(NULL, this, this);
	m_pRenderGraphButton->SetText("Render graph");
	m_pRenderGraphButton->SetRect(492, 0, 80, 16);

	m_pSoundButton = new MButton(NULL, this, this);
	m_pSoundButton->SetText("Sound");
	m_pSoundButton->SetRect(574, 0, 80, 16);

	m_pLootButton = new MButton(NULL, this, this);
	m_pLootButton->SetText("Loot");
	m_pLootButton->SetRect(656, 0, 80, 16);

	m_pMovableButton = new MButton(NULL, this, this);
	m_pMovableButton->SetText("Movable");
	m_pMovableButton->SetRect(738, 0, 80, 16);

	m_pMLogButton = new MButton(NULL, this, this);
	m_pMLogButton->SetText("MLog");
	m_pMLogButton->SetRect(820, 0, 80, 16);
	
	m_pCLogButton = new MButton(NULL, this, this);
	m_pCLogButton->SetText("CLog");
	m_pCLogButton->SetRect(902, 0, 80, 16);

	m_pCollisionButton = new MButton(NULL, this, this);
	m_pCollisionButton->SetText("Collision");
	m_pCollisionButton->SetRect(738, 20, 80, 16);
}

XDebugCategoryFrm::~XDebugCategoryFrm(void)
{
//	ReleaseExclusive();
}

void XDebugCategoryFrm::OnDraw( MDrawContext* pDC )
{
	MFrame::OnDraw(pDC);
}

bool XDebugCategoryFrm::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs/*=NULL*/ )
{
	if (pWidget == m_pCategory)
	{
		switch(nMsg)
		{
		case MEVENT_SELCHANGE:
			{
				int index = m_pCategory->GetCurSel();
				SelChange(index);
				return true;
			}
		}
	}
		
	switch (nMsg)
	{
	case MEVENT_CLICK:
		{
				 if (pWidget == m_pCloseButton)		Hide();
			else if (pWidget == m_pFPSButton)		SelChange(2);
			else if (pWidget == m_pObjListButton)	SelChange(3);
			else if (pWidget == m_pHelpButton)		SelChange(4);
			else if (pWidget == m_pNPCMonitorButton)SelChange(5);
			else if (pWidget == m_pCombatButton)	SelChange(6);
			else if (pWidget == m_pRenderGraphButton)SelChange(7);
			else if (pWidget == m_pSoundButton)		SelChange(8);
			else if (pWidget == m_pLootButton)		SelChange(9);
			else if (pWidget == m_pMovableButton)	SelChange(10);
			else if (pWidget == m_pMLogButton)		SelChange(11);
			else if (pWidget == m_pCLogButton)		SelChange(12);
			else if (pWidget == m_pNPCLogButton)	SelChange(13);
			else if (pWidget == m_pCollisionButton) SelChange(14);
			else
				return false;

			return true;
		}
	}


	return false;
}

void XDebugCategoryFrm::OnShow( bool bShow)
{
	MWidget::OnShow( bShow);

	if ( bShow == true)		BringToTop();
}

void XDebugCategoryFrm::SelChange(int nIndex)
{
#ifndef _PUBLISH
	XPlayFrame* pPlayFrame = static_cast<XPlayFrame*>(global.gfmgr->GetCurrFrame());
	if (pPlayFrame)
	{
		if (nIndex == 0)		pPlayFrame->GetDashBoard()->ShowCategoryAI();
		else if (nIndex == 1)	pPlayFrame->GetDashBoard()->ShowCategorySync();
		else if (nIndex == 2)	pPlayFrame->GetDashBoard()->ShowFPS();
		else if (nIndex == 3)	pPlayFrame->GetDashBoard()->ShowObjList();
		else if (nIndex == 4)	pPlayFrame->GetDashBoard()->ShowHelp();
		else if (nIndex == 5)	pPlayFrame->GetDashBoard()->ShowNPCMonitor();
		else if (nIndex == 6)	pPlayFrame->GetDashBoard()->ShowCombat();
		else if (nIndex == 7)	pPlayFrame->GetDashBoard()->ShowRenderGraph();
		else if (nIndex == 8)	pPlayFrame->GetDashBoard()->ShowSound();
		else if (nIndex == 9)	pPlayFrame->GetDashBoard()->ShowLoot();
		else if (nIndex == 10)	pPlayFrame->GetDashBoard()->ShowMovable();
		else if (nIndex == 11)	pPlayFrame->GetDashBoard()->ShowMlog();
		else if (nIndex == 12)	pPlayFrame->GetDashBoard()->ShowClog();
		else if (nIndex == 13)	pPlayFrame->GetDashBoard()->ShowNPCLog();
		else if (nIndex == 14)	pPlayFrame->GetDashBoard()->ShowCollision();
	}
#endif
}
/*
bool XDebugCategoryFrm::OnEvent( MEvent* pEvent, MListener* pListener )
{
	MRECT WidgetRect(0, 0, m_Rect.w, m_Rect.h);
	MPOINT sp = MClientToScreen(this, pEvent->Pos);

	switch(pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		{
			if(WidgetRect.InPoint(pEvent->Pos)==true)
			{
				SetCapture();
				m_bDragWidget = true;
				MPOINT wp = MClientToScreen(GetParent(), MPOINT(m_Rect.x, m_Rect.y));
				sp.x -= wp.x;
				sp.y -= wp.y;
				m_DragPoint = sp;

				return true;
			}
		}
		break;
	case MWM_LBUTTONUP:
		{
			if(m_bDragWidget==true)
			{
				ReleaseCapture();
				m_bDragWidget = false;
				return true;
			}
		}
		break;
	case MWM_MOUSEMOVE:
		{
			if(m_bDragWidget==true)
			{
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
		}
		break;
	}

	return false;
}
*/