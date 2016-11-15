#include "stdafx.h"
#include "XMsgBox.h"

#define TRANSIENT_TIME	200

// void XMsgBox::Show(bool bVisible, bool bModal)
// {
// 	DWORD elapsed=(timeGetTime()-m_nShowTime);
// 
// 	if(m_bNextVisible==m_bVisible && m_bVisible==bVisible && elapsed>TRANSIENT_TIME)
// 		return;
// 
// 	// 상태가 변경되는 중간에 다른방향으로 가야할경우
// 	if(m_bNextVisible!=bVisible){
// 		if(elapsed<TRANSIENT_TIME)
// 			m_nShowTime=timeGetTime()-(TRANSIENT_TIME-elapsed);
// 		else
// 			m_nShowTime=timeGetTime();
// 	}
// 	m_bNextVisible = bVisible;
// 
// 	MMsgBox::Show(bVisible,bModal);
// 	m_bVisible = true;
// 
// 	Enable(bVisible);
// }
// 
// #define XMSGBOX_W	400
// #define XMSGBOX_X	(MGetWorkspaceWidth()/2-XMSGBOX_W/2)
// #define XMSGBOX_H	110
// #define XMSGBOX_Y	(MGetWorkspaceHeight()/2-XMSGBOX_H/2)
// 
// void XMsgBox::OnShow()
// {
// 	if(m_pOK!=NULL && m_pOK->IsVisible()==true) 
// 		m_pOK->SetFocus();
// 	else if(m_pCancel!=NULL) m_pCancel->SetFocus();
// 
// 	SetPosition(XMSGBOX_X,XMSGBOX_Y);
// }
// 
// void XMsgBox::OnDraw(MDrawContext* pDC)
// {
// 	// fade 기능은 Mint단에서 전체적으로 지원해줘야 함. 지금은 안되니 일단 여기다 둔다. - birdkr(2007-06-27)
// 	float fOpacity = 0;
// 	if(m_bNextVisible==false){	// Hide
// 		fOpacity = 1.0f-min(float(timeGetTime()-m_nShowTime)/(float)TRANSIENT_TIME, 1.0f);
// 		if(fOpacity==0.0f) {
// 			m_bVisible = false;
// 			m_bExclusive = false;
// 		}
// 		SetOpacity(fOpacity);
// 	}
// 	else{	// Show
// 		fOpacity = min(float(timeGetTime()-m_nShowTime)/(float)TRANSIENT_TIME, 1.0f);
// 		SetOpacity(fOpacity);
// 	}
// 
// 	MMsgBox::OnDraw(pDC);
// }

XMsgBox::XMsgBox(const char* szMessage, MWidget* pParent, MListener* pListener, MMsgBoxType nType)
: MMsgBox(szMessage,pParent,pListener,nType)
{
// 	m_bCanShade = false;
// 	m_bNextVisible = false;
// 	m_nShowTime = timeGetTime()-TRANSIENT_TIME*2;
// 	SetOpacity(0.0f);
// 	m_pCustomListener = NULL;
}

XMsgBox::~XMsgBox(void)
{
}

// void XMsgBox::SetText(const char* szText)
// {
// 	MMsgBox::SetText(szText);
// 
// 	int nLineCount = MMGetLineCount(m_pMessage->GetFont(),szText,XMSGBOX_W);
// 
// 	SetBounds(MRECT(XMSGBOX_X, XMSGBOX_Y, XMSGBOX_W, XMSGBOX_H + nLineCount*m_pMessage->GetFont()->GetHeight()));
// }
