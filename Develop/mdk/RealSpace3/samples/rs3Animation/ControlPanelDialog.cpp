// ontrolPanelDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ControlPanelDialog.h"


// ControlPanelDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(ControlPanelDialog, CDialog)

ControlPanelDialog::ControlPanelDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ControlPanelDialog::IDD, pParent)
{

}

ControlPanelDialog::~ControlPanelDialog()
{
}

void ControlPanelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ControlPanelDialog, CDialog)
END_MESSAGE_MAP()


// ControlPanelDialog 메시지 처리기입니다.
