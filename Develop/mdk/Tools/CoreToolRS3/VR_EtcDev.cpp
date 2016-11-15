// V_EditEtcDev.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VR_EtcDev.h"

#include "RDevice.h"
#include "RConfiguration.h"
//#include "M_CollisionTree.h"

using namespace rs3;

// CRollupEtcDev 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupEtcDev, CDialog)

CRollupEtcDev::CRollupEtcDev(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEtcDev::IDD, pParent)
{

}

CRollupEtcDev::~CRollupEtcDev()
{
}

void CRollupEtcDev::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRollupEtcDev, CDialog)
END_MESSAGE_MAP()


// CRollupEtcDev 메시지 처리기입니다.

void CRollupEtcDev::Listen(CoreMessage& _message)
{
	switch( _message.nID) 
	{
		// 활성
		case VIEW_MESSAGE::ACTIVE_VIEW :
		{
			m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::ETC_DEV);
		}
		break;

		// 존 로딩
		case VIEW_MESSAGE::SCENE_LOADED :
		{
		}
		break;
	}
}

BOOL CRollupEtcDev::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);
	return TRUE;
}
