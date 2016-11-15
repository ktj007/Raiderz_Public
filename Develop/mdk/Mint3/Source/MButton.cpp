#include "stdafx.h"
#include "MButton.h"
#include "MButtonGroup.h"
#include "MColorTable.h"
#include "MMsgBox.h"
#include "Mint.h"
#include "MWLua.h"
#include "MMainFrame.h"
#include "MEventHandler.h"
#include "MScriptManager.h"

namespace mint3 {

#define MBUTTON_DEFAULT_ALIGNMENT_MODE	(MAM_HCENTER|MAM_VCENTER)


IMPLEMENT_LOOK(MButton, MButtonLook)


void MButton::OnMouseIn(void)
{
}

void MButton::OnMouseOut(void)
{
}


bool MButton::OnEvent(MEvent* pEvent, MListener* pListener)
{
	bool bButtonDowned = (m_bLButtonDown) || (m_bRButtonDown);
	if (MWidget::OnEvent(pEvent, pListener) == true) return true;

	MRECT r = GetInitialClientRect();
	m_ClickPos = pEvent->Pos;

	switch(pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		if(r.InPoint(pEvent->Pos)==true)
		{
			return true;
		}
		break;

	case MWM_LBUTTONUP:
		if ((bButtonDowned) && (r.InPoint(pEvent->Pos)==true)) 
		{
			if(GetType()==MBT_CHECKBTN || GetType()==MBT_PUSHBTN) 
			{
				if(m_pButtonGroup)
					SetCheck(true);
				else
					SetCheck(!m_bChecked);
			}

			return true;
		}
		break;
	case MWM_LBUTTONDBLCLK:
		if(r.InPoint(pEvent->Pos)==true)
		{
			if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
			return true;
		}
		break;

	case MWM_RBUTTONDBLCLK:
		if(r.InPoint(pEvent->Pos)==true)
		{
			if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
			return true;
		}
		break;

	case MWM_RBUTTONDOWN:
		if(r.InPoint(pEvent->Pos)==true)
		{
			return true;
		}
		break;

	case MWM_RBUTTONUP:
		if(bButtonDowned && r.InPoint(pEvent->Pos)==true && pListener!=NULL)
		{
			if(GetType()==MBT_CHECKBTN && GetType() == MBT_PUSHBTN ) m_bChecked = !m_bChecked;
		}
		return true;
		break;
	case MWM_MOUSEMOVE:
		if(r.InPoint(pEvent->Pos)==true)
		{
			if(m_bMouseOver==false)
			{
				m_bMouseOver = true;
				OnMouseIn();
				if(pListener!=NULL) pListener->OnCommand(this, MEVENT_BUTTON_IN);
			}

		}
		else{
			if(m_bMouseOver==true)
			{
				m_bMouseOver = false;
				OnMouseOut();
				if(pListener!=NULL) pListener->OnCommand(this, MEVENT_BUTTON_OUT);
			}
		}
		return false;
	case MWM_KEYDOWN:
		if(m_bEnableEnter==true && IsFocus()==true)
		{
			if(pEvent->nKey==VK_RETURN)
			{
				if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
				else OnClick(pEvent);
				return true;
			}
		}
		break;
	}
	return false;
}

void MButton::OnShow(void)
{
	MWidget::OnShow();

	m_bMouseOver = false;
}

void MButton::OnHide(void)
{
	MWidget::OnHide();

	if(m_pMsgBox) m_pMsgBox->Show(false);
}

MButton::MButton(const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener), m_TextColor(0, 255, 0)//, m_TextOffset(0, 0)
{
	m_bMouseOver = false;
	m_bShowText = true;

	m_AlignmentMode = MBUTTON_DEFAULT_ALIGNMENT_MODE;
	m_Type = MBT_NORMAL;
	m_bChecked = false;

	m_bEnableEnter = true;
	m_bHighlight = true;

	m_pIcon = NULL;

	m_nKeyAssigned = MBKA_NONE;

	SetFocusEnable(true);

	m_pMsgBox = NULL;

	m_ClickPos = MPOINT(0,0);

	m_bComboButton = false;

	m_bStretch = false;

	m_pButtonGroup = NULL;
	m_nIndexInGroup = 0;

	LOOK_IN_CONSTRUCTOR()
}

MButton::~MButton(void)
{
	// GetMainFrame의 자식으로 만들어지기 때문에 delete 하면 에러난다. 
	// 그렇지만 여기서 delete를 안해주면 계속 쓰레기가 남기 때문에 이건 나중에 어떻게든 처리해야 함. - birdkr(2007-11-29)
	//if(m_pMsgBox!=NULL) 
	//{
	//	delete m_pMsgBox;
	//	m_pMsgBox = NULL;
	//}

	if (m_pButtonGroup)
	{
		if (m_pButtonGroup->m_pPrevious == this) m_pButtonGroup->m_pPrevious = NULL;
	}

	ReleaseButtonGroup();
}


/*
void MButton::SetTextOffset(MPOINT p)
{
	m_TextOffset = p;
}
*/
void MButton::SetTextColor(MCOLOR color)
{
	m_TextColor = color;
}

MCOLOR MButton::GetTextColor(void)
{
	return m_TextColor;
}

void MButton::ShowText(bool bShow)
{
	m_bShowText = bShow;
}

bool MButton::DefaultCommand(void)
{
	if(m_pMsgBox!=NULL) m_pMsgBox->Show(true, true);
	else OnClick(NULL);
	return true;
}

MAlignmentMode MButton::GetAlignment(void)
{
	return m_AlignmentMode;
}

MAlignmentMode MButton::SetAlignment(MAlignmentMode am)
{
	MAlignmentMode temp = m_AlignmentMode;
	m_AlignmentMode = am;
	return temp;
}

void MButton::SetType(MButtonType t)
{
	m_Type = t;
}

MButtonType MButton::GetType(void)
{
	return m_Type;
}

void MButton::SetCheck(bool bCheck)
{
	if (m_bChecked == bCheck) return;

	if (m_pButtonGroup)
	{
		if(bCheck && m_pButtonGroup->m_pPrevious)
		{
			m_pButtonGroup->m_pPrevious->SetCheck(false);
		}

		m_pButtonGroup->m_pPrevious=this;
	}

	m_bChecked = bCheck;

	OnCheckChanged(m_bChecked);
}

bool MButton::GetCheck(void)
{
	return m_bChecked;
}

bool MButton::IsButtonDown(void)
{
	if(m_bLButtonDown==true && m_bMouseOver==true) return true;
	return false;
}

bool MButton::IsMouseOver(void)
{
	return m_bMouseOver;
}

void MButton::SetConfirmMessageBox(const char* szMessage)
{
	if(szMessage==NULL || szMessage[0]==0)
	{
		if(m_pMsgBox!=NULL)
		{
			delete m_pMsgBox;
			m_pMsgBox = NULL;
		}
		return;
	}

	if(m_pMsgBox==NULL) {
		m_pMsgBox = (MMsgBox*)Mint::GetInstance()->NewWidget(MINT_MSGBOX, "", Mint::GetInstance()->GetMainFrame(), this );
		m_pMsgBox->SetType(MT_OKCANCEL);
	}
	
	m_pMsgBox->SetText(szMessage);
}

bool MButton::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if (pWidget==m_pMsgBox)
	{
		if(nMsg == MEVENT_MSGBOX_OK) 
		{
			m_EventHandler.OnCommand(this, MEVENT_CLICK);
			m_EventHandler.OnCommand(this, MEVENT_BUTTON_CLICK);
		}
		return true;
	}
	return false;
}

void MButton::SetButtonGroup(MButtonGroup *pGroup)
{
	m_pButtonGroup = pGroup;
	m_nIndexInGroup = pGroup->m_nCount;
	pGroup->m_nCount++;
	
	if(GetCheck()==true)
		pGroup->m_pPrevious = this;
}

void MButton::OnCheckChanged( bool bChecked )
{
	m_EventHandler.OnCommand(this, MEVENT_BUTTON_CHECK_CHANGED);
}

void MButton::SetUserData(const char* szUserData)
{
	m_strUserData = szUserData;
}

const char* MButton::GetUserData()
{
	return m_strUserData.c_str();
}

void MButton::glueSetUserData(const char* szUserData )
{
	SetUserData(szUserData);
}

const char* MButton::glueGetUserData()
{
	return GetUserData();
}

//------------------------------------------------------------------------
// Lua
void MButton::glueSetCheck(bool bCheck)
{
	SetCheck(bCheck);
}

bool MButton::glueIsCheck()
{
	// 체크 박스 경우에만 사용한다.
	if(m_Type == MBT_CHECKBTN &&
		m_bChecked)
	{
		return true;
	}

	return false;
}

bool MButton::glueIsButtonDown()
{
	return m_bChecked;
}

void MButton::glueSetConfirmMsg( const char* szMessage )
{
	SetConfirmMessageBox(szMessage);
}

void MButton::OnClick( MEvent* pEvent )
{
	if(m_pMsgBox!=NULL) 
	{
		m_pMsgBox->Show(true, true);
	}
	else
	{
		m_EventHandler.OnCommand(this, MEVENT_CLICK);
		m_EventHandler.OnCommand(this, MEVENT_BUTTON_CLICK);
	}
}

//////////////////////////////////////////////////////////////////////////
// Look
void MButtonLook::OnDrawText(MButton* pButton, MRECT& r, MDrawContext* pDC)
{
	if( !pButton->IsVisible() ) return;
	if(pButton->m_pIcon!=NULL){
		pDC->SetBitmap(pButton->m_pIcon);
		pDC->Draw(r.x, r.y+(r.h-pButton->m_pIcon->GetHeight())/2);
		r.x+=pButton->m_pIcon->GetWidth();
		r.w-=pButton->m_pIcon->GetWidth();
	}

	pDC->SetColor(MColorTable::MBUTTON_TEXT);

//	if (pDC->BeginFont())
	{
		if(pButton->m_bHighlight==true){
			MCOLOR PrevHCol = pDC->SetHighlightColor(MColorTable::PEN_HIGHLIGHT);
			pDC->TextWithHighlight(r, pButton->GetText(), pButton->GetAlignment());
			pDC->SetHighlightColor(PrevHCol);
		}
		else{
			pDC->Text(r, pButton->GetText(), pButton->GetAlignment());
		}
//		pDC->EndFont();
	}
}

void MButtonLook::OnDownDraw(MButton* pButton, MDrawContext* pDC)
{
	MRECT r = pButton->GetInitialClientRect();
	pDC->SetColor(MColorTable::MBUTTON_DARKPLANE);
	pDC->FillRectangle(r);

	OnDrawText(pButton, pButton->GetClientRect(), pDC);
}

void MButtonLook::OnUpDraw(MButton* pButton, MDrawContext* pDC)
{
	MRECT r = pButton->GetInitialClientRect();
	pDC->SetColor(MColorTable::MBUTTON_PLANE);
	pDC->FillRectangle(r);

	OnDrawText(pButton, pButton->GetClientRect(), pDC);
}

void MButtonLook::OnOverDraw(MButton* pButton, MDrawContext* pDC)
{
	if(pButton->GetType()==MBT_CHECKBTN)
	{
		//if(pButton->GetCheck()==true) OnDownDraw(pButton, pDC);
		//else OnUpDraw(pButton, pDC);
		OnCheckBoxDraw(pButton, pDC, pButton->GetCheck());
	}
	else
	{
		MRECT r = pButton->GetInitialClientRect();
		// Over일때 투명도를 1.0으로 변경. 투명도가 있을 경우 알아보기 위해서...
		pDC->SetOpacity(1.0);
		pDC->SetColor(MColorTable::MBUTTON_PLANE);
		pDC->FillRectangle(r);

		OnDrawText(pButton, pButton->GetClientRect(), pDC);
	}
}

void MButtonLook::OnDisableDraw(MButton* pButton, MDrawContext* pDC)
{
	MRECT r = pButton->GetInitialClientRect();
	pDC->SetColor(MColorTable::MBUTTON_PLANE);
	pDC->FillRectangle(r);

	OnDrawText(pButton, pButton->GetClientRect(), pDC);
}

void MButtonLook::OnDraw(MButton* pButton, MDrawContext* pDC)
{
	switch ( pButton->GetType())
	{
		case MBT_NORMAL :
		{
			if ( pButton->IsEnable() == false)		OnDisableDraw( pButton, pDC);
			else if ( pButton->IsButtonDown())		OnDownDraw( pButton, pDC);
			else if ( pButton->IsMouseOver())		OnOverDraw( pButton, pDC);
			else									OnUpDraw( pButton, pDC);

			break;
		}

		case MBT_CHECKBTN :
		{
			if ( pButton->IsEnable() == false)		OnDisableDraw( pButton, pDC);
			else if ( pButton->GetCheck())			OnDownDraw( pButton, pDC);
			else if ( pButton->IsMouseOver())		OnOverDraw( pButton, pDC);
			else if ( pButton->IsButtonDown())		OnOverDraw( pButton, pDC);
			else									OnUpDraw( pButton, pDC);

			break;
		}

		case MBT_PUSHBTN :
		{
			if ( pButton->IsEnable() == false)		OnDisableDraw( pButton, pDC);
			else if ( pButton->GetCheck())			OnDownDraw( pButton, pDC);
			else if ( pButton->IsButtonDown())		OnDownDraw( pButton, pDC);
			else if ( pButton->IsMouseOver())		OnOverDraw( pButton, pDC);
			else									OnUpDraw( pButton, pDC);

			break;
		}
	}
}

void MButtonLook::OnCheckBoxDraw(MButton* pButton, MDrawContext* pDC, bool bPushed)
{
	// 체크박스 그리기
	MRECT r = pButton->GetInitialClientRect();
	int x = r.x + (int)(r.w*0.5) - (int)(CHECKBOX_SIZE*0.5);	// 약간의 여유분
	int y = r.y + (int)(r.h*0.5) - (int)(CHECKBOX_SIZE*0.5);

	if(bPushed)
	{
		// 체크 된것
		pDC->SetColor(255, 56, 69, 255);
		pDC->FillRectangle(MRECT(x, y, (int)CHECKBOX_SIZE, (int)CHECKBOX_SIZE));
	}
	else
	{
		pDC->SetColor(128, 128, 128, 255);
		pDC->Rectangle(x, y, (int)CHECKBOX_SIZE, (int)CHECKBOX_SIZE);
	}
}

MRECT MButtonLook::GetClientRect(MButton* pButton, MRECT& r)
{
	return r;
}

} // namespace mint3