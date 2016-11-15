// AniBlendDurationDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AniBlendDurationDlg.h"


// AniBlendDurationDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(AniBlendDurationDlg, CDialog)

AniBlendDurationDlg::AniBlendDurationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AniBlendDurationDlg::IDD, pParent)
	, m_editDuration(_T(""))
{

	EnableAutomation();

}

AniBlendDurationDlg::~AniBlendDurationDlg()
{
}

void AniBlendDurationDlg::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면
	// OnFinalRelease가 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void AniBlendDurationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ANIBLEND_DURATION, m_editDuration);
}


BEGIN_MESSAGE_MAP(AniBlendDurationDlg, CDialog)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(AniBlendDurationDlg, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IAniBlendDurationDlg에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다. 
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {90B0A9E5-67FA-4356-A2CC-0E69FD563DE7}
static const IID IID_IAniBlendDurationDlg =
{ 0x90B0A9E5, 0x67FA, 0x4356, { 0xA2, 0xCC, 0xE, 0x69, 0xFD, 0x56, 0x3D, 0xE7 } };

BEGIN_INTERFACE_MAP(AniBlendDurationDlg, CDialog)
	INTERFACE_PART(AniBlendDurationDlg, IID_IAniBlendDurationDlg, Dispatch)
END_INTERFACE_MAP()


// AniBlendDurationDlg 메시지 처리기입니다.
