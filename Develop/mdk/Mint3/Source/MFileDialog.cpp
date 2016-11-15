#include "stdafx.h"
#include "MFileDialog.h"
#include "MColorTable.h"
#include "MStringTable.h"
#include "Mint.h"

namespace mint3 {

#define GAP			0
#define INNERGAP	5

#define FILEDIALOG_W	300
#define FILEDIALOG_X	20
#define FILEDIALOG_H	(MGetWorkspaceHeight()-80)
#define FILEDIALOG_Y	40

#define BUTTON_W	80
#define BUTTON_H	26

#define OK_X		(r.x+r.w-BUTTON_W*2-5)
#define OK_Y		(r.y+r.h-BUTTON_H+3)
#define CANCEL_X	(r.x+r.w-BUTTON_W)
#define CANCEL_Y	(OK_Y)

/*
#define BUTTON_W	90
#define BUTTON_H	22

#define OK_X		(r.w-BUTTON_W*2-GAP-INNERGAP)
#define OK_Y		(r.y+r.h-BUTTON_H-GAP)
#define CANCEL_X	(r.w-BUTTON_W-GAP)
#define CANCEL_Y	(OK_Y)
*/

#define FILEBOX_X	(r.x)
#define FILEBOX_Y	(r.y)
#define FILEBOX_W	(r.w-GAP*2)
#define FILEBOX_H	(r.h-BUTTON_H*2-GAP*2-INNERGAP*2)

#define EDIT_X		(r.x)
#define EDIT_Y		(r.y+r.h-BUTTON_H*2-GAP-INNERGAP)
#define EDIT_W		FILEBOX_W
#define EDIT_H		BUTTON_H

bool MFileDialog::IsExistFile(const char* szFileName)
{
	FILE* fp = fopen(szFileName, "rb");
	if(fp==NULL) return false;
	fclose(fp);
	return true;
}

/*
void MFileDialog::OnDraw(MDrawContext* pDC)
{
	pDC->SetColor(MColorTable::FRAME_PLANE);
	pDC->FillRectangle(m_Rect);
	pDC->SetColor(MColorTable::FRAME_OUTLINE);
	pDC->Rectangle(m_Rect);
}
*/
MFileDialog::MFileDialog(const char* szFilter, MWidget* pParent, MListener* pListener)
: MFrame(szFilter, pParent, (pListener==NULL)?pParent:pListener)
{
	SetBounds(MRECT(FILEDIALOG_X, FILEDIALOG_Y, FILEDIALOG_W, FILEDIALOG_H));

	MRECT r = GetClientRect();

	m_pOK = new MButton(MGetString(MSID_OK), this, this);
	m_pOK->SetBounds(MRECT(OK_X, OK_Y, BUTTON_W, BUTTON_H));
	m_pOK->SetLabelAccelerator();

	m_pOK->SetAnchor(MAnchorStyle(false, true, false, true));

	m_pCancel = new MButton(MGetString(MSID_CANCEL), this, this);
	m_pCancel->SetBounds(MRECT(CANCEL_X, CANCEL_Y, BUTTON_W, BUTTON_H));
	m_pCancel->SetAccelerator(27);

	m_pCancel->SetAnchor(MAnchorStyle(false, true, false, true));


	m_pFileBox = new MFileBox(szFilter, this, this);
	m_pFileBox->SetBounds(MRECT(FILEBOX_X, FILEBOX_Y, FILEBOX_W, FILEBOX_H));
	m_pFileBox->SetAnchor(MAnchorStyle(true, true, true, true));

	m_pFileName = new MEdit(_MAX_PATH, "", this, this);
	m_pFileName->SetBounds(MRECT(EDIT_X, EDIT_Y, EDIT_W, EDIT_H));
	//m_pFileName->SetTextColor(MCOLOR(0, 255, 0));
	m_pFileName->SetAnchor(MAnchorStyle(true, true, false, true));

	m_pMsgBox = new MMsgBox(MGetString(MSID_OVERWRITE), this, this, MT_YESNO);

	m_nType = MFDT_OPEN;

	Show(false);
}

MFileDialog::~MFileDialog(void)
{
	ReleaseExclusive();
	if(m_pMsgBox!=NULL) delete m_pMsgBox;
	if(m_pOK!=NULL) delete m_pOK;
	if(m_pCancel!=NULL) delete m_pCancel;
	if(m_pFileBox!=NULL) delete m_pFileBox;
	if(m_pFileName!=NULL) delete m_pFileName;
}

const char* MFileDialog::GetFileName(void)
{
	return m_pFileName->GetText();
}

const char* MFileDialog::GetPathName(void)
{
	sprintf(m_szTempPathName, "%s\\%s", GetBaseDir(), GetFileName());
	return m_szTempPathName;
}

bool MFileDialog::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	MListener* pListener = GetListener();
	if(pListener==NULL) return false;

	if(pWidget==m_pFileBox){
		if(nMsg == MEVENT_LB_ITEM_SEL){
			m_pFileName->SetText(m_pFileBox->GetSelItemString());
		}
		else if(nMsg == MEVENT_LB_ITEM_DBLCLK) {
			m_pFileName->SetText(m_pFileBox->GetSelItemString());
			if(m_nType==MFDT_SAVE && IsExistFile(GetPathName())==true){
				m_pMsgBox->Show(true, true);
				return true;
			}
			pListener->OnCommand(this, MEVENT_FILEDIALOG_OK);
		}
		return true;
	}
	else if(pWidget==m_pOK && nMsg == MEVENT_BUTTON_CLICK){
		if(strcmp(m_pFileName->GetText(), "")!=0){
			if(m_nType==MFDT_SAVE && IsExistFile(GetPathName())==true){
				m_pMsgBox->Show(true, true);
				return true;
			}
			pListener->OnCommand(this, MEVENT_FILEDIALOG_OK);
		}
		return true;
	}
	else if(pWidget==m_pCancel && nMsg == MEVENT_BUTTON_CLICK){
		pListener->OnCommand(this, MEVENT_FILEDIALOG_CANCEL);
		return true;
	}
	else if(pWidget==m_pMsgBox){
		m_pMsgBox->Show(false);
		if(nMsg == MEVENT_MSGBOX_YES) pListener->OnCommand(this, MEVENT_FILEDIALOG_OK);
	}
	return false;
}

void MFileDialog::Refresh(const char* szFilter)
{
	m_pFileBox->Refresh(szFilter);
}

const char* MFileDialog::GetBaseDir(void)
{
	return m_pFileBox->GetBaseDir();
}

void MFileDialog::SetType(MFileDialogType fdt)
{
	m_nType = fdt;
}

} // namespace mint3