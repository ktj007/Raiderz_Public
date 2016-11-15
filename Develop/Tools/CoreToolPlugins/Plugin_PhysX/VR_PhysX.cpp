// VR_PhysX.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_PhysX.h"

#include "C_ControlMediator.h"

#include "V_MFCHelper.h"
#include "C_XMLConfig.h"


#include "WS_PhysX.h"

// CRollupPhysX 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupPhysX, CDialog)

CRollupPhysX::CRollupPhysX(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupPhysX::IDD, pParent)
{
}

CRollupPhysX::~CRollupPhysX()
{
}

void CRollupPhysX::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PHX, m_ctrlPhxListTab);
	DDX_Control(pDX, IDC_LIST_PXOBJ, m_ctrlObjList);
}


BEGIN_MESSAGE_MAP(CRollupPhysX, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PHX, &CRollupPhysX::OnTcnSelchangeTabPhx)
	ON_LBN_SELCHANGE(IDC_LIST_PXOBJ, &CRollupPhysX::OnLbnSelchangeListPxobj)
	ON_BN_CLICKED(IDC_BUTTON_BPART_1, &CRollupPhysX::OnBnClickedButtonBpart1)
	ON_BN_CLICKED(IDC_BUTTON_BPART_2, &CRollupPhysX::OnBnClickedButtonBpart2)
	ON_BN_CLICKED(IDC_BUTTON_BPART_3, &CRollupPhysX::OnBnClickedButtonBpart3)
	ON_BN_CLICKED(IDC_BUTTON_BPART_4, &CRollupPhysX::OnBnClickedButtonBpart4)
	ON_BN_CLICKED(IDC_BUTTON_PX_RELOAD, &CRollupPhysX::OnBnClickedButtonPxReload)
END_MESSAGE_MAP()


void CRollupPhysX::Listen( CoreMessage& _message )
{
	switch(_message.nID)
	{
		case VIEW_MESSAGE::DEVICE_CREATED:
		{
			// init tab
			char* szTabListName[E_TAB_NUM] =
			{
				("All"),
				("Monster"),
			};
			m_ctrlPhxListTab.SetCurFocus(0);
			TCITEM tabItem;
			tabItem.mask = TCIF_TEXT;
			for ( int i = 0; i<E_TAB_NUM; ++i )
			{
				tabItem.pszText = szTabListName[i];
				m_ctrlPhxListTab.InsertItem(i,&tabItem);
			}
		}
		break;
	}
}

void CRollupPhysX::OnTcnSelchangeTabPhx(NMHDR *pNMHDR, LRESULT *pResult)
{
	LoadPxTabList();
	*pResult = 0;
}

void CRollupPhysX::LoadPxTabList(int nForceSet)
{
	std::vector< std::string > paths;
	if (nForceSet != -1)
	{
		m_ctrlPhxListTab.SetCurSel(nForceSet);
	}

	int nTabIndex = m_ctrlPhxListTab.GetCurFocus();
	if (nTabIndex == E_ALL)
	{
		paths.push_back( CXMLConfig::GetInstance()->strDataFullPath + "Model\\MapObject\\");
		paths.push_back( CXMLConfig::GetInstance()->strDataFullPath + "Model\\Monster\\");
		paths.push_back( CXMLConfig::GetInstance()->strDataFullPath + "Model\\NPC\\");

	}
	else if (nTabIndex == E_MONSTER)
	{
		paths.push_back( CXMLConfig::GetInstance()->strDataFullPath + "Model\\Monster\\");
	}

	std::vector< std::string > strFiles;
	for (std::vector<std::string>::iterator itr = paths.begin(); itr != paths.end(); ++itr)
		CMFCHelper::GetFileFromPathWithFilter(strFiles, itr->c_str(), ".elu", true);

	m_ctrlObjList.ResetContent();
	for (std::vector< std::string >::iterator itr = strFiles.begin(); itr != strFiles.end(); ++itr)
	{
		if (itr->find(".elu.") == std::string::npos)
			m_ctrlObjList.AddString(itr->c_str());
	}
}

void CRollupPhysX::OnLbnSelchangeListPxobj()
{
	int nSelectedList = m_ctrlObjList.GetCurSel();
	if (nSelectedList < 0)
		return;

	CString strListName;
	m_ctrlObjList.GetText(m_ctrlObjList.GetCurSel(), strListName);
	CWorkSpacePhysX* pPhysXWorkSpace = m_pControlMediator_->GetCurrentWorkSpaceAs<CWorkSpacePhysX>();
	if (pPhysXWorkSpace)
	{
		pPhysXWorkSpace->LoadPxObject(strListName.GetString());
	}
}

void CRollupPhysX::OnBnClickedButtonBpart1()
{
	CWorkSpacePhysX* pPhysXWorkSpace = m_pControlMediator_->GetCurrentWorkSpaceAs<CWorkSpacePhysX>();
	if (pPhysXWorkSpace)
	{
		pPhysXWorkSpace->StartBPart(1);
	}
}

void CRollupPhysX::OnBnClickedButtonBpart2()
{
	CWorkSpacePhysX* pPhysXWorkSpace = m_pControlMediator_->GetCurrentWorkSpaceAs<CWorkSpacePhysX>();
	if (pPhysXWorkSpace)
	{
		pPhysXWorkSpace->StartBPart(2);
	}
}

void CRollupPhysX::OnBnClickedButtonBpart3()
{
	CWorkSpacePhysX* pPhysXWorkSpace = m_pControlMediator_->GetCurrentWorkSpaceAs<CWorkSpacePhysX>();
	if (pPhysXWorkSpace)
	{
		pPhysXWorkSpace->StartBPart(3);
	}
}

void CRollupPhysX::OnBnClickedButtonBpart4()
{
	CWorkSpacePhysX* pPhysXWorkSpace = m_pControlMediator_->GetCurrentWorkSpaceAs<CWorkSpacePhysX>();
	if (pPhysXWorkSpace)
	{
		pPhysXWorkSpace->StartBPart(4);
	}
}

void CRollupPhysX::OnBnClickedButtonPxReload()
{
	OnLbnSelchangeListPxobj();
}
