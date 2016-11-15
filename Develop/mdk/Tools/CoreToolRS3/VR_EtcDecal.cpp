// V_EditEtcDev.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_EtcDecal.h"

#include "RDevice.h"
#include "RConfiguration.h"
#include "C_XMLConfig.h"
#include "VD_DlgCommonFileOpen.h"

using namespace rs3;

// CRollupEtcDecal 대화 상자입니다.
IMPLEMENT_DYNAMIC(CRollupEtcDecal, CDialog)

CRollupEtcDecal::CRollupEtcDecal(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEtcDecal::IDD, pParent)
	, m_bCreate(false)
{
	m_eDecalType = DECAL_TYPE_PROJECTION;

	m_fDecalSize_X		= 500.f;
	m_fDecalSize_Y		= 500.f;
	m_fDecalSize_Z		= 100.f;
	m_fDecalRotate_Z	= 0;

	m_strTextureFileName = "Gunz2_smoke03.dds";
}

CRollupEtcDecal::~CRollupEtcDecal()
{
}

void CRollupEtcDecal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_DECAL_SIZE_X, m_fDecalSize_X);
	DDX_Text(pDX, IDC_EDIT_DECAL_SIZE_Y, m_fDecalSize_Y);
	DDX_Text(pDX, IDC_EDIT_DECAL_SIZE_Z, m_fDecalSize_Z);
	DDX_Text(pDX, IDC_EDIT_DECAL_ROTATE_Z, m_fDecalRotate_Z);
	DDX_Text(pDX, IDC_EDIT_DECAL_TEXTURE, m_strTextureFileName);
}


BEGIN_MESSAGE_MAP(CRollupEtcDecal, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DECALTEST, &CRollupEtcDecal::OnBnClickedDecalCreate_Projection)
	ON_BN_CLICKED(IDC_BUTTON_DECALTEST2, &CRollupEtcDecal::OnBnClickedDecalClear)
	ON_BN_CLICKED(IDC_BUTTON_DECALTEST3, &CRollupEtcDecal::OnBnClickedDecalCreate_Polygon)

	ON_EN_CHANGE(IDC_EDIT_DECAL_SIZE_X, OnChangeDecalSizeX)
	ON_EN_CHANGE(IDC_EDIT_DECAL_SIZE_Y, OnChangeDecalSizeY)
	ON_EN_CHANGE(IDC_EDIT_DECAL_SIZE_Z, OnChangeDecalSizeZ)
	ON_EN_CHANGE(IDC_EDIT_DECAL_ROTATE_Z, OnChangeDecalRotateZ)

	ON_BN_CLICKED(IDC_DECAL_TEXTURE_OPEN, &CRollupEtcDecal::OnBnClickedTextureOpen)
END_MESSAGE_MAP()


// CRollupEtcDecal 메시지 처리기입니다.

void CRollupEtcDecal::Listen(CoreMessage& _message)
{
	switch( _message.nID) 
	{
		// 활성
		case VIEW_MESSAGE::ACTIVE_VIEW :
		{
			m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::ETC_DECAL);
		}
		break;

		// 존 로딩
		case VIEW_MESSAGE::SCENE_LOADED :
		{
		}
		break;
	}
}

BOOL CRollupEtcDecal::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);
	return TRUE;
}

#include "RDecalProjectionSceneNode.h"
void CRollupEtcDecal::OnBnClickedDecalCreate_Projection()
{
	m_bCreate = true;
	m_eDecalType = DECAL_TYPE_PROJECTION;
}

void CRollupEtcDecal::OnBnClickedDecalClear()
{
	m_bCreate = false;
}

void CRollupEtcDecal::OnBnClickedDecalCreate_Polygon()
{
	m_eDecalType = DECAL_TYPE_POLYGON;
	m_bCreate = true;
}

void CRollupEtcDecal::OnChangeDecalSizeX()
{
	UpdateData(TRUE);
}

void CRollupEtcDecal::OnChangeDecalSizeY()
{
	UpdateData(TRUE);
}

void CRollupEtcDecal::OnChangeDecalSizeZ()
{
	UpdateData(TRUE);

}

void CRollupEtcDecal::OnChangeDecalRotateZ()
{
	UpdateData(TRUE);

}

void CRollupEtcDecal::OnBnClickedTextureOpen()
{
	string m_strPath;
	m_strPath.clear();
	char szfull[256];
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	m_strPath = szfull;
	m_strPath += "Texture\\";

	CListViewOpenDlg dlg(m_strPath.c_str());
	//dlg.SetTitle("조각풀의 그림파일을 선택하세요");
	dlg.SetFilter("Decal Texture 파일\0*.bmp;*.tga;*.jpg;*.dds;");
	dlg.AddStyle(OFN_ALLOWMULTISELECT);

	CString strFilename;
	if ( dlg.DoModal() == IDOK ) 
	{
		POSITION pos=dlg.GetStartPosition();
		while(pos!=NULL)
		{
			// Get the dir+filename one by one and show it in message box
			CString csFile(dlg.GetNextPathName(pos));

			int right_count = csFile.GetLength() - csFile.ReverseFind('\\') -1;
			csFile = csFile.Right(right_count);

			m_strTextureFileName = csFile.GetString();
		}
		//EditAndMakeGrl_();
	}

	UpdateData(FALSE);
}