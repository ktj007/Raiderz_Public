#include "stdafx.h"

#include "RCFrameInfoDlg.h"

using namespace rs3;

IMPLEMENT_DYNCREATE(CRCFrameInfoDlg, CDHtmlDialog)

CRCFrameInfoDlg::CRCFrameInfoDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CRCFrameInfoDlg::IDD, CRCFrameInfoDlg::IDH, pParent)
{
}

CRCFrameInfoDlg::~CRCFrameInfoDlg()
{
}

void CRCFrameInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NODE_LISTBOX, m_ListBox);
}

BOOL CRCFrameInfoDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BEGIN_MESSAGE_MAP(CRCFrameInfoDlg, CDHtmlDialog)
	
	ON_LBN_SELCHANGE(IDC_NODE_LISTBOX, OnLbnSelchangeNodeListbox)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CRCFrameInfoDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CRCFrameInfoDlg 메시지 처리기입니다.

HRESULT CRCFrameInfoDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	GetMainView()->ToggleFrameInfoDlg();

	OnOK();
	return S_OK;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

HRESULT CRCFrameInfoDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRCFrameInfoDlg::ClearListBox()
{
	int cnt = m_ListBox.GetCount();

	for(int i=0;i<cnt;i++) {
		m_ListBox.DeleteString(0);
	}
}

void CRCFrameInfoDlg::Begin()
{
	ClearListBox();

	m_ListBox.SetSel(0);

	RActor* pActor = GetMainView()->GetActor();

	if(pActor) {
/*
		// 원래코드 에니메이션 정보
		AniFrameInfo* pInfo = pActor->GetFrameInfo(AMode_All);

		if(pInfo) {

			RAnimation* pAni = pInfo->m_pAniSet;

			if( pAni ) {

				RAnimationFile*	pAniFile = pAni->m_pAniData;

				if( pAniFile ) {
					int cnt = pAniFile->m_nAniNodeCnt;

					RAnimationNode* pNode = NULL;

					for(int i=0;i<cnt;i++) {
						pNode = pAniFile->m_pAniNodeTable[i];

						if(pNode)
							m_ListBox.AddString( pNode->GetName() );
					}
				}
			}
		}

		RMeshNode* pMNode = NULL;

		for(int i=0;i<pActor->m_pMesh->m_nDataNum;i++)
		{
			pMNode = pActor->m_pMesh->m_NodeTable[i];

			if(pMNode)
				m_ListBox.AddString( pMNode->GetName() );
		}
*/
	}
}

void CRCFrameInfoDlg::End()
{
	
}

void CRCFrameInfoDlg::OnLbnSelchangeNodeListbox()
{
	int nCnt = m_ListBox.GetCount();

	if(nCnt==0) return;

	int nSel = m_ListBox.GetCurSel();

	char	str[1024];
//	CString str;
	m_ListBox.GetText(nSel,str);

	RActor* pActor = GetMainView()->GetActor();

//	RMeshNode* pMNode = NULL;

//	if(pActor&&pActor->m_pMesh) {
//		 pActor->m_pMesh->SetToolSelectNodeName( str );
//	}
	
}
