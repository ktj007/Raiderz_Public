#include "stdafx.h"
#include "MMainFrame.h"
#include "MLabel.h"
#include "MButton.h"
#include "MComboBox.h"
#include "MEdit.h"
#include "MFrame.h"
#include "MPopupMenu.h"

namespace mint3 {

char* g_szWidgetClasses[] = {
	MINT_LABEL,
	MINT_BUTTON,
	MINT_EDIT,
	MINT_LISTBOX,
	MINT_COMBOBOX,
	//	MINT_FILEBOX,
	MINT_FRAME,
	//	MINT_GROUP,
	//	MINT_POPUPMENU,
	//MINT_SCROLLBAR,
	//MINT_SLIDER,
	//MINT_TEXTAREA,
	//MINT_MSGBOX,
};

// Resource ID Map
class MStringIDMap : public map<string, int>{};
MStringIDMap	g_ResourceIDMap;


void MNewWidgetDlg::OnSize( int w, int h )
{

}

bool MNewWidgetDlg::OnCommand( MWidget* pWidget, const char* szMessage )
{
	//if(pWidget==m_pOK && IsMsg(szMessage, MBTN_CLK_MSG)==true){
	//	if(m_nMode==0){
	//		MWidget* pNew = MNewWidget(m_pWidgetType->GetText(), m_pWidgetName->GetText(), m_pTarget, m_pTarget);
	//		pNew->ResetAddedByDesignerFlag(true);
	//		pNew->ResetModifiedByDesignerFlag(true);
	//		MPOINT cp = MScreenToClient(m_pTarget, m_TargetPos);
	//		pNew->SetPosition(cp);
	//		pNew->SetSize(80, 20);
	//		pNew->SetID(MGetResourceID(m_pWidgetID->GetText()));
	//		//m_pTarget->ResetModifiedByDesignerFlag(true);
	//	}
	//	else{
	//		m_pTarget->SetText(m_pWidgetName->GetText());
	//		m_pTarget->SetID(MGetResourceID(m_pWidgetID->GetText()));
	//	}
	//	Hide();
	//	return true;
	//}
	//if(pWidget==m_pCancel && IsMsg(szMessage, MBTN_CLK_MSG)==true){
	//	Hide();
	//	return true;
	//}

	return false;
}

MNewWidgetDlg::MNewWidgetDlg( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ ) : MFrame(szName, pParent, pListener)
{
	SetSize(240, 200);

	m_pLabelWidgetType = new MLabel("Widget Type", this, this);
	m_pLabelWidgetType->SetBounds(12, 28, 76, 14);
	m_pLabelWidgetType->EnableDesignerMode(false);

	m_pWidgetType = new MComboBox(g_szWidgetClasses[0], this, this);
	m_pWidgetType->SetBounds(92, 24, 144, 20);
	for(int i=1; i<sizeof(g_szWidgetClasses)/sizeof(char*); i++){
		m_pWidgetType->Add(g_szWidgetClasses[i]);
	}
	m_pWidgetType->SetDropSize(160);
	m_pWidgetType->EnableDesignerMode(false);

	m_pLabelWidgetName = new MLabel("Widget Name", this, this);
	m_pLabelWidgetName->SetBounds(12, 54, 76, 14);
	m_pLabelWidgetName->EnableDesignerMode(false);
	m_pWidgetName = new MEdit(NULL, this, this);
	m_pWidgetName->SetBounds(92, 50, 144, 20);
	m_pWidgetName->EnableDesignerMode(false);

	m_pLabelWidgetID = new MLabel("Widget ID", this, this);
	m_pLabelWidgetID->SetBounds(12, 80, 76, 14);
	m_pLabelWidgetID->EnableDesignerMode(false);
	m_pWidgetID = new MEdit(NULL, this, this);
	m_pWidgetID->SetBounds(92, 76, 144, 20);
	m_pWidgetID->EnableDesignerMode(false);

	m_pOK = new MButton("&OK", this, this);
	m_pOK->SetBounds(72, 172, 80, 24);
	m_pOK->EnableDesignerMode(false);
	m_pCancel = new MButton("&Cancel", this, this);
	m_pCancel->SetBounds(156, 172, 80, 24);
	m_pCancel->EnableDesignerMode(false);

	m_pWidgetName->SetText("Unnamed");
	m_pWidgetID->SetText("ID_UNKNOWN");

	EnableDesignerMode(false);

	m_nMode = 0;
}

MNewWidgetDlg::~MNewWidgetDlg( void )
{
	delete m_pLabelWidgetID;
	delete m_pWidgetID;
	delete m_pLabelWidgetName;
	delete m_pWidgetName;
	delete m_pLabelWidgetType;
	delete m_pWidgetType;
	delete m_pOK;
	delete m_pCancel;
}

void MNewWidgetDlg::Set( const char* szName, const char* szID )
{
	m_pWidgetName->SetText(szName);
	m_pWidgetID->SetText(szID);
}

void MNewWidgetDlg::Set( const char* szClass, const char* szName, int nID )
{
	m_pWidgetType->SetText(szClass);
	m_pWidgetName->SetText(szName);
	m_pWidgetID->SetText("ID_UNKNOWN");
	for(MStringIDMap::iterator i=g_ResourceIDMap.begin(); i!=g_ResourceIDMap.end(); i++)
	{
		if((*i).second==nID)
		{
			m_pWidgetID->SetText((*i).first.c_str());
			break;
		}
	}
}

void MNewWidgetDlg::SetMode( int m )
{
	m_nMode = m;
	if(m==0)	// 추가
	{	
	}
	else		// 읽기
	{		
	}
}


bool MMainFrame::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs/*=NULL*/ )
{
	return false;
}

MMainFrame::MMainFrame( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ ) : MWidget(szName, pParent, pListener)
{
	m_pDesignerMenu = new MPopupMenu("Designer", this, this);
	m_pDesignerMenu->EnableDesignerMode(false);
	m_pDesignerMenu->Show(false);

	m_pNewWidget = new MNewWidgetDlg("Widget", this, this);
	m_pNewWidget->Show(false);
}

void MMainFrame::ShowDesignerMenu( MPOINT& p, MWidget* pCaller )
{
	m_pDesignerMenuCaller = pCaller;

	m_pDesignerMenu->SetPosition(p.x, p.y);
	m_pDesignerMenu->RemoveAllMenuItem();
	m_pDesignerMenu->AddMenuItem(DM_ADDWIDGET);
	if(pCaller->IsAddedByDesignerFlag()==true){
		m_pDesignerMenu->AddMenuItem(DM_REMOVEWIDGET);
		m_pDesignerMenu->AddMenuItem(DM_PROPERTY);
	}

	m_pDesignerMenu->SetZOrder(MZ_TOP);
	m_pDesignerMenu->Show();
}

void MMainFrame::HideDesignerMode( void )
{
	m_pDesignerMenu->Show(false);
	m_pNewWidget->Show(false);
}
} // namespace