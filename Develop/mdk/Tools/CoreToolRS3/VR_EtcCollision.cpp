// VR_EtcCollision.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_EtcCollision.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperResource.h"
#include "C_XMLConfig.h"
#include "C_StatusBarHandler.h"

#include "RRenderHelper.h"

#include "RCollisionTreeBuilder.h"
#include "M_CollisionTree.h"

// CRollupEtcCollision 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupEtcCollision, CDialog)

CRollupEtcCollision::CRollupEtcCollision(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEtcCollision::IDD, pParent)
{
}

CRollupEtcCollision::~CRollupEtcCollision()
{
}

void CRollupEtcCollision::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_CL2_RENDER, m_ctlCl2ViewBtn);
}

BEGIN_MESSAGE_MAP(CRollupEtcCollision, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_CL2_RENDER, &CRollupEtcCollision::OnBnClickedCl2ViewBtn)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_CL2_AS, &CRollupEtcCollision::OnBnClickedButtonCreateCl2As)
END_MESSAGE_MAP()


// CRollupEtcCollision 메시지 처리기입니다.

void CRollupEtcCollision::Listen(CoreMessage& _message)
{
// 	switch( _message.nID )
// 	{
// 	}
}

void CRollupEtcCollision::OnCancel()
{
}

void CRollupEtcCollision::OnOK()
{
}

void CRollupEtcCollision::Cl2ViewToggle()
{
	if ( m_pControlMediator_ )
	{
		if ( m_ctlCl2ViewBtn.GetCheck())
			m_ctlCl2ViewBtn.SetCheck(0);
		else
			m_ctlCl2ViewBtn.SetCheck(1);

		OnBnClickedCl2ViewBtn();
	}
}

void CRollupEtcCollision::SetCollisionFileName( const string& strFileName )
{
	m_strCollisionFileName = strFileName;
}

void CRollupEtcCollision::OnBnClickedCl2ViewBtn()
{
	if ( m_pControlMediator_ )
	{
		if ( m_ctlCl2ViewBtn.GetCheck())
		{
			m_collisionTree.SetRender(true);
			m_collisionTree.Load( m_strCollisionFileName.c_str() );
		}
		else
		{
			m_collisionTree.SetRender(false);
			m_collisionTree.Destroy();
		}
	}
}

void CRollupEtcCollision::OnBnClickedButtonCreateCl2As()
{
	CFileDialog dlgFile(FALSE, "cl2", "_name");
	dlgFile.m_pOFN->lpstrTitle = "Save cl2 File";
	dlgFile.m_pOFN->lpstrInitialDir= CXMLConfig::GetInstance()->strDataFullPath.c_str();
	if(dlgFile.DoModal()==IDOK)
	{
		CString strFileName = dlgFile.GetPathName();
		CreateCl2( strFileName.GetString() );
	}
}

bool CRollupEtcCollision::CreateCl2( const char* _pSzFileName )
{
	using namespace rs3;

	RCollisionTreeBuilder colTreeBuilder;

	SceneNodeCollisionCollector collector(&colTreeBuilder);

	// 이게 맞나 ?
	const rs3::RSceneNode* pSceneNode = &m_pControlMediator_->GetToolSceneManager()->GetRootSceneNode();

	pSceneNode->ExclusionTraverseRefMesh(&collector);
	int num = colTreeBuilder.GetNumOfTreeToBuild();

	// elu가 없어서 충돌정보 기록에 실패한 파일리스트를 한번에 출력한다
	if( colTreeBuilder.m_vecFailedList.size() )
	{
		CString strMessage;
		strMessage.Format("%d개의 파일이 이전파일입니다. 익스포터버그로 충돌정보를 만들수 없습니다. 다시 익스포트 부탁해요.\n", colTreeBuilder.m_vecFailedList.size());

		for( size_t i = 0; i < colTreeBuilder.m_vecFailedList.size(); ++i )
		{
			string strFailedName = colTreeBuilder.m_vecFailedList[i] + "\n";
			strMessage += CString(strFailedName.c_str());
		}

		::MessageBox(NULL,strMessage.GetBuffer(),"I'm sorry.",MB_OK);
		colTreeBuilder.m_vecFailedList.clear();
	}

	CString strMessage;
	strMessage.Format("%d개의 폴리곤에서 충돌파일을 생성합니다.", num);
	CStatusBarHandler::GetInstance()->ShowStatusBarMessage(strMessage);

	colTreeBuilder.BuildTree();
	if (false == colTreeBuilder.Save(_pSzFileName))
		return false;

	SetCollisionFileName(_pSzFileName);
	return true;
}
