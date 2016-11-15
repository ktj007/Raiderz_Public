#include "stdafx.h"
#include "MPopupMenu.h"
#include "MColorTable.h"

namespace mint3 {

IMPLEMENT_LOOK(MPopupMenu, MPopupMenuLook)


//// MPopupMenuLook ////
MPopupMenuLook::MPopupMenuLook(void)
{
	m_SelectedPlaneColor = MColorTable::MLIST_SELECTEDPLANE;
	m_SelectedTextColor = MColorTable::MLIST_SELECTEDTEXT;
	m_UnfocusedSelectedPlaneColor = MColorTable::DARK;
}

void MPopupMenuLook::OnFrameDraw(MPopupMenu* pPopupMenu, MDrawContext* pDC)
{
	MRECT r = pPopupMenu->GetInitialClientRect();
	pDC->SetColor(MColorTable::MPOPUP_PLANE);
	pDC->FillRectangle(r);
	pDC->SetColor(MColorTable::FRAME_OUTLINE);
	pDC->Rectangle(r);
}

void MPopupMenuLook::OnDraw(MPopupMenu* pPopupMenu, MDrawContext* pDC)
{
	OnFrameDraw(pPopupMenu, pDC);

	if(pPopupMenu->GetPopupMenuType()==MPMT_VERTICAL){
		pDC->SetColor(MColorTable::FRAME_OUTLINE);
		pDC->Rectangle(pPopupMenu->GetInitialClientRect());
	}
}

MRECT MPopupMenuLook::GetClientRect(MPopupMenu* pPopupMenu, MRECT& r)
{
	if(pPopupMenu->GetPopupMenuType()==MPMT_VERTICAL)
		return MRECT(1, 1, r.w-2, r.h-2);
	else
		return pPopupMenu->GetInitialClientRect();
}

//// MMenuItem ////
void MMenuItem::OnDrawMenuItem(MDrawContext* pDC, bool bSelected)
{
	MRECT r = GetClientRect();	
	pDC->SetColor(bSelected==true? MColorTable::MPOPUP_SELECTEDPLANE : MColorTable::MPOPUP_PLANE);
	pDC->FillRectangle(r);
	pDC->SetColor(bSelected==true? MColorTable::MPOPUP_SELECTEDTEX : MColorTable::MPOPUP_TEXT);
	pDC->Text(r, GetText(), MAM_LEFT);
}

void MMenuItem::OnDraw(MDrawContext* pDC)
{
//	pDC->BeginFont();
		OnDrawMenuItem(pDC, IsSelected());
//	pDC->EndFont();
}

bool MMenuItem::OnEvent(MEvent* pEvent, MListener* pListener)
{
	MRECT r = GetClientRect();
	switch(pEvent->nMessage){
	case MWM_MOUSEMOVE:
	case MWM_LBUTTONDOWN:
	case MWM_RBUTTONDOWN:
	case MWM_LBUTTONDBLCLK:
		if(r.InPoint(pEvent->Pos)==true){
			MPopupMenu* pPopupMenu = (MPopupMenu *)GetParent();
			pPopupMenu->Select(this);
			return true;
		}
		//else Select(false);
		break;
	case MWM_LBUTTONUP:
		if(r.InPoint(pEvent->Pos)==true){
			//m_bSelected = true;
			MPopupMenu* pPopupMenu = (MPopupMenu *)GetParent();
			if(GetSubMenu()!=NULL) pPopupMenu->Select(this);
			return true;
		}
		break;
	case MWM_RBUTTONUP:
		break;
	}
	return false;
}

MMenuItem::MMenuItem(const char* szName) : MWidget(szName, NULL, NULL)
{
	m_bSelected = false;
//	m_Anchors.m_bRight=true;
}

MMenuItem::~MMenuItem(void)
{
	while(!m_Children.empty())
	{
		MWidget* pWidget = (*m_Children.begin());
		delete pWidget;
	}
}

bool MMenuItem::IsSelected(void)
{
	return m_bSelected;
}

#define MENUITEM_MARGIN_X	20
#define MENUITEM_MARGIN_Y	4
int MMenuItem::GetWidth(void)
{
/*	MFont* pFont = GetFont();
	const char* szText = GetText();

	return pFont->GetWidth(szText) + MENUITEM_MARGIN_X;*/
	return GetClientRect().w;
}

int MMenuItem::GetHeight(void)
{
	MFont* pFont = GetFont();
	return pFont->GetHeight() + MENUITEM_MARGIN_Y;
	return GetClientRect().h;
}
/*
void MMenuItem::SetSubMenu(MPopupMenu* pMenu)
{
	m_pSubMenu = pMenu;
}
*/
MPopupMenu* MMenuItem::CreateSubMenu(void)
{
	if(!m_Children.empty()) return (MPopupMenu *)(*m_Children.begin());
	MPopupMenu* pSubMenu = new MPopupMenu("SubMenu", this, GetParent());
	return pSubMenu;
}

MPopupMenu* MMenuItem::GetSubMenu(void)
{
	if(!m_Children.empty()) return (MPopupMenu *)(*m_Children.begin());
	return NULL;
}

void MMenuItem::Select(bool bSelect)
{
	//if(m_bSelected==bSelect) return;	// Sub Menu를 보여주기 위해 주석 처리..

	m_bSelected = bSelect;

	MRECT r = GetClientRect();

	if(m_bSelected==true){
		MPopupMenu* pSubMenu = GetSubMenu();
		if(pSubMenu!=NULL && pSubMenu->IsVisible()==false){
			MPopupMenu* pPopupMenu = (MPopupMenu *)GetParent();
			//if(pPopupMenu->GetType()==MPMT_VERTICAL) pSubMenu->Show(r.x+r.w, r.y, true);
			//else pSubMenu->Show(r.x, r.y+r.h, true);

			if(pPopupMenu->GetType()==MPMT_VERTICAL) pSubMenu->Show(r.x+r.w, r.y, true);
			else pSubMenu->Show(r.x, r.y+r.h, true);
		}
	}
	else{
		MPopupMenu* pSubMenu = GetSubMenu();
		if(pSubMenu!=NULL) pSubMenu->Show(false);
	}
}

//// MPopupMenu ////
bool MPopupMenu::OnEvent(MEvent* pEvent, MListener* pListener)
{
	MRECT r = GetClientRect();
	switch(pEvent->nMessage){
	case MWM_LBUTTONDOWN:
	case MWM_LBUTTONUP:
	case MWM_LBUTTONDBLCLK:
	case MWM_RBUTTONDOWN:
	//case MWM_RBUTTONUP:
	case MWM_RBUTTONDBLCLK:
	case MWM_MBUTTONDOWN:
	case MWM_MBUTTONUP:
	case MWM_MBUTTONDBLCLK:
		if(r.InPoint(pEvent->Pos)==false){
			if(m_nPopupMenuType==MPMT_VERTICAL) Show(false);
			else Select((MMenuItem *)NULL);
		}
		break;
	}
	return false;
}

MPopupMenu::MPopupMenu(const char* szName, MWidget* pParent, MListener* pListener, MPopupMenuTypes t) : MWidget(szName, pParent, pListener)
{
	m_nPopupMenuType = t;

	LOOK_IN_CONSTRUCTOR()
}

MPopupMenu::~MPopupMenu(void)
{
	while(!m_Children.empty()){
		MWidget* pWidget = (*m_Children.begin());
		delete pWidget;
	}
}

MMenuItem* MPopupMenu::AddMenuItem(const char* szMenuName)
{
	MMenuItem* pNewItem = new MMenuItem();
	pNewItem->SetText(szMenuName);
	AddMenuItem(pNewItem);
	return pNewItem;
}

void MPopupMenu::AddMenuItem(MMenuItem* pMenuItem)
{
	AddChild(pMenuItem);
	pMenuItem->SetListener(this);

	MRECT cr = GetClientRect();
	MRECT ir = GetInitialClientRect();
	if(m_nPopupMenuType==MPMT_VERTICAL){
		int y = 0;
		int nWidth = 0;
		for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
		{
			MMenuItem* pMenuItem = (MMenuItem *)(*itor);
			pMenuItem->SetPosition(cr.x, cr.y+y);
			nWidth = max(nWidth, pMenuItem->GetWidth());
			y += pMenuItem->GetHeight();
		}
		for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
		{
			MMenuItem* pMenuItem = (MMenuItem *)(*itor);
			pMenuItem->SetSize(nWidth, pMenuItem->GetHeight());
		}

		SetSize(nWidth+ir.w-cr.w-1, y+ir.h-cr.h-1);
	}
	else{
		int x = 0;
		int nHeight = 0;
		for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
		{
			MMenuItem* pMenuItem = (MMenuItem *)(*itor);
			pMenuItem->SetPosition(cr.x+x, cr.y);
			nHeight = max(nHeight, pMenuItem->GetHeight());
			x += pMenuItem->GetWidth();
		}
		for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
		{
			MMenuItem* pMenuItem = (MMenuItem *)(*itor);
			pMenuItem->SetSize(pMenuItem->GetWidth(), nHeight);
		}

		SetSize(x+ir.w-cr.w-1, nHeight+ir.h-cr.h-1);
	}
}

void MPopupMenu::RemoveMenuItem(MMenuItem* pMenuItem)
{
	delete pMenuItem;
}

void MPopupMenu::RemoveAllMenuItem(void)
{
	while(!m_Children.empty()){
		MWidget* pChild = (*m_Children.begin());
		delete pChild;
	}
}

void MPopupMenu::Show(int x, int y, bool bVisible)
{
	MWidget::Show(bVisible);
	if(bVisible==true) SetPosition(x, y);
}

void MPopupMenu::Show(bool bVisible)
{
	Select((MMenuItem *)NULL);
	MWidget::Show(bVisible);
	//Show(0, 0, bVisible);
}

void MPopupMenu::SetType(MPopupMenuTypes t)
{
	m_nPopupMenuType = t;
}

MPopupMenuTypes MPopupMenu::GetType(void)
{
	return m_nPopupMenuType;
}

void MPopupMenu::Select(int idx)
{
	int index = 0;
	for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
	{
		MMenuItem* pMenuItem = (MMenuItem *)(*itor);
		if(index==idx) pMenuItem->Select(true);
		else pMenuItem->Select(false);

		index++;
	}
}

void MPopupMenu::Select(MMenuItem* pMenuItem)
{
	for (list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
	{
		MMenuItem* pThisMenuItem = (MMenuItem *)(*itor);
		if(pThisMenuItem==pMenuItem) pThisMenuItem->Select(true);
		else pThisMenuItem->Select(false);
	}
}

bool MPopupMenu::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	//if(GetType()==MPMT_VERTICAL) Show(false);
	//else Select((MMenuItem*)NULL);

	MListener* pListener = GetListener();
	if(pListener!=NULL) pListener->OnCommand(this, nMsg, szArgs);
	return true;
}


} // namespace mint3