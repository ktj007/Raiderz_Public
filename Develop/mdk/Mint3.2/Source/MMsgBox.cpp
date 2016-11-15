#include "stdafx.h"
#include "MMsgBox.h"
#include "MStringTable.h"
#include "Mint.h"

namespace mint3 {

int charinstr(char* sz, char c)
{
	int len = strlen(sz);
	for(int i=0; i<len; i++){
		if(sz[i]==c) return i;
	}
	return -1;
}

/*
void MMsgBox::OnDrawText(MDrawContext* pDC)
{
	//MFrameDrawer* pFD = GetFrameDrawer();

	pDC->SetColor(MCOLOR(255, 255, 255));

	char* pStr = m_szMessage;
	int nLen = charinstr(pStr, '\n');
	int nY = m_Rect.y+10;
	if(nLen<0){
		//pFD->Text(pDC, MPOINT(m_Rect.x+10, m_Rect.y+10), pStr, MFDTS_NORMAL, false, &m_Rect);
		pDC->Text(m_Rect.x+10, m_Rect.y+10, pStr);
	}
	else{
		do{
			char temp[MWIDGET_TEXT_LENGTH];
			memcpy(temp, pStr, nLen);
			temp[nLen] = 0;
			//pFD->Text(pDC, MPOINT(m_Rect.x+10, nY), temp, MFDTS_NORMAL, false, &m_Rect);
			pDC->Text(m_Rect.x+10, nY, temp);
			pStr += (nLen+1);
			nLen = charinstr(pStr, '\n');
			nY += GetFont()->GetHeight();
		}while(nLen>0);
	}
}

void MMsgBox::OnDraw(MDrawContext* pDC)
{
	OnDrawText(pDC);
}
*/
#define MMSGBOX_W	400
#define MMSGBOX_X	(MGetWorkspaceWidth()/2-MMSGBOX_W/2)
#define MMSGBOX_H	140
#define MMSGBOX_Y	(MGetWorkspaceHeight()/2-MMSGBOX_H/2)

#define MMSGBOX_OK_W	80
#define MMSGBOX_OK_H	32

void MMsgBox::OnShow( bool bShow)
{
	MFrame::OnShow( bShow);

  	SetRect( MRECT(MMSGBOX_X, MMSGBOX_Y, MMSGBOX_W, MMSGBOX_H) );
    
	if(m_pOK!=NULL && m_pOK->GetShow()==true) 
		m_pOK->SetFocus();
	else if(m_pCancel!=NULL) m_pCancel->SetFocus();
}

void MMsgBox::OnSize(int w, int h)
{
 	MRECT r = GetInitialClientRect();
	m_pMessage->SetRect(MRECT(r.x+19, r.y+40, r.w-38, r.h-70));
	int nOKOffset = 0;
	if(m_nType==MT_OKCANCEL || m_nType==MT_YESNO) nOKOffset = MMSGBOX_OK_W+5;
	if(m_pOK!=NULL) m_pOK->SetRect(MRECT(r.x+r.w-MMSGBOX_OK_W-nOKOffset - 19, r.y+r.h-MMSGBOX_OK_H-20, MMSGBOX_OK_W, MMSGBOX_OK_H));
	if(m_pCancel!=NULL) m_pCancel->SetRect(MRECT(r.x+r.w-MMSGBOX_OK_W - 19, r.y+r.h-MMSGBOX_OK_H-20, MMSGBOX_OK_W, MMSGBOX_OK_H));
}

MMsgBox::MMsgBox(const char* szMessage, MWidget* pParent, MListener* pListener, MMsgBoxType nType)
: MFrame(MGetString(MSID_MESSAGE), pParent, (pListener==NULL)?pParent:pListener)
{
	m_pMessage = new MTextView( "", this, this);
	m_pMessage->SetText( szMessage);
	

	m_pOK = NULL;
	m_pCancel = NULL;

	if(nType!=MT_NOTDECIDED)
		SetType(nType);

	m_bResizable = false;

	SetRect(MRECT(MMSGBOX_X, MMSGBOX_Y, MMSGBOX_W, MMSGBOX_H));

	MMsgBox::OnSize(MMSGBOX_W, MMSGBOX_H);

	Show(false);
}

MMsgBox::~MMsgBox(void)
{
	delete m_pMessage;
	m_pMessage = NULL;

	if(m_pOK!=NULL) 
	{
		delete m_pOK;
		m_pOK = NULL;
	}
	if(m_pCancel!=NULL) 
	{
		delete m_pCancel;
		m_pCancel = NULL;
	}
	
}

void MMsgBox::SetType(MMsgBoxType nType)
{
	m_nType = nType;

	if(m_pOK) 
	{
		delete m_pOK;
		m_pOK = NULL;
	}
	if(m_pCancel) 
	{
		delete m_pCancel;
		m_pCancel = NULL;
	}

	if(nType!=MT_CANCEL)
	{
		m_pOK = (MButton*)Mint::GetInstance()->NewWidget(MINT_BUTTON, MGetString(MSID_OK), this, this );
		m_pOK->SetText(MGetString(MSID_OK));
		m_pOK->SetLabelAccelerator();
		m_pOK->SetKeyAssign( MBKA_ENTER);
	}
	if(nType==MT_YESNO) m_pOK->SetText(MGetString(MSID_YES));

	if(nType==MT_OKCANCEL || nType==MT_YESNO || nType==MT_CANCEL){
		m_pCancel = (MButton*)Mint::GetInstance()->NewWidget(MINT_BUTTON, MGetString(MSID_CANCEL), this, this );
		m_pCancel->SetText(MGetString(MSID_CANCEL));
		if(nType==MT_YESNO) m_pCancel->SetText(MGetString(MSID_NO));
		m_pCancel->SetAccelerator(27);
		m_pCancel->SetKeyAssign( MBKA_ESC);
	}
	MMsgBox::OnSize(MMSGBOX_W, MMSGBOX_H);
}

bool MMsgBox::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	MListener* pListener = GetListener();
	if(pListener==NULL) return false;
// 
// 	if(pWidget==m_pOK && nMsg == MEVENT_BUTTON_CLICK){
// 		if(m_nType==MT_YESNO) pListener->OnCommand(this, MEVENT_MSGBOX_YES);
// 		else pListener->OnCommand(this, MEVENT_MSGBOX_OK);
// 		Show(false);
// 		return true;
// 	}
// 	else if(pWidget==m_pCancel && nMsg == MEVENT_BUTTON_CLICK){
// 		if(m_nType==MT_YESNO) pListener->OnCommand(this, MEVENT_MSGBOX_NO);
// 		else pListener->OnCommand(this, MEVENT_MSGBOX_CANCEL);
// 		Show(false);
// 		return true;
// 	}

	return false;
}

void MMsgBox::SetTitle(const char* szTitle)
{
	SetText( szTitle);
}

const char* MMsgBox::GetTitle(void)
{
	return GetText();
}

void MMsgBox::SetMessage(const char* szMessage)
{
	m_pMessage->SetText(szMessage);
}

const char* MMsgBox::GetMessage(void)
{
	static char buffer[256];
	buffer[0]=0;
//	if(m_pMessage->GetText(buffer,sizeof(buffer)))
//		return buffer;
	return buffer;
}

void MMsgBox::SetText(const char* szText)
{
	SetMessage(szText);
}

const char* MMsgBox::GetText(void)
{
	return GetMessage();
}

void MMsgBox::SetOKButtonText(const char* szText)
{
	if (m_pOK) m_pOK->SetText(szText);
}

void MMsgBox::SetCancelButtonText(const char* szText)
{
	if (m_pCancel) m_pCancel->SetText(szText);
}

} // namespace mint3