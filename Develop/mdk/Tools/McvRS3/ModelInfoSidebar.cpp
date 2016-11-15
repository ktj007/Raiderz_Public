// ModelInfoDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelInfoSidebar.h"
#include "resource.h"


// CModelInfoSidebar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CModelInfoSidebar, CDialog)

CModelInfoSidebar::CModelInfoSidebar(CWnd* pParent /*=NULL*/)
	: CDialog()
	, m_bNodeMesh(TRUE)
	, m_bNodeBone(TRUE)
	, m_bNodeDummy(TRUE)
	, m_strNodeInfo(_T(""))
{

}

CModelInfoSidebar::~CModelInfoSidebar()
{
}

LRESULT CModelInfoSidebar::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	OnInitDialog();

	return CDialog::HandleInitDialog(wParam, lParam);
}

BOOL CModelInfoSidebar::OnInitDialog() 
{
	// TODO: Add extra initialization here
	CButton* pCheck = (CButton*)GetDlgItem( IDC_CHECK_TRACE_LOC);
	if ( pCheck)
		pCheck->SetCheck( TRUE);

	UpdateData(FALSE);

	return TRUE;  
}

void CModelInfoSidebar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CModelInfoSidebar)
	DDX_Check(pDX, IDC_CHECK_NODE_MESH, m_bNodeMesh);
	DDX_Check(pDX, IDC_CHECK_NODE_BONE, m_bNodeBone);
	DDX_Check(pDX, IDC_CHECK_NODE_DUMMY, m_bNodeDummy);
	DDX_Control(pDX, IDC_TREE_NODES, m_treeNodes);
	DDX_Text(pDX, IDC_EDIT_NODE_INFO, m_strNodeInfo);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_STATIC_TM_INFO, m_nodeTMInfo);
}


BEGIN_MESSAGE_MAP(CModelInfoSidebar, CDialog)
	//{{AFX_MSG_MAP(CModelInfoSidebar)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_BN_CLICKED(IDC_CHECK_NODE_MESH, &CModelInfoSidebar::OnBnClickedCheckNode)
	ON_BN_CLICKED(IDC_CHECK_NODE_BONE, &CModelInfoSidebar::OnBnClickedCheckNode)
	ON_BN_CLICKED(IDC_CHECK_NODE_DUMMY, &CModelInfoSidebar::OnBnClickedCheckNode)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NODES, &CModelInfoSidebar::OnTvnSelchangedTreeNodes)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CModelInfoSidebar::SelectNodeByID(int nID)	// tree control 에서 node 를 선택한다
{

	for( HTREEITEM hItem = m_treeNodes.GetRootItem(); hItem!= NULL;  hItem = m_treeNodes.GetNextItem(hItem,TVGN_NEXT))
	{
		int nItemData = m_treeNodes.GetItemData(hItem);

		if(nItemData==nID)
		{

			m_treeNodes.SelectItem(hItem );
			return;
		}
	}

	m_treeNodes.SelectItem(NULL);
}

void CModelInfoSidebar::OnTvnSelchangedTreeNodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetMainView()->OnTvnSelchangedTreeNodes( pNMHDR, pResult);
}

void CModelInfoSidebar::OnBnClickedCheckNode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetMainView()->OnBnClickedCheckNode();
}

void CModelInfoSidebar::_resize( int nID, int delta, bool top_anchor)
{
	CWnd* pWnd = GetDlgItem( nID);
	if ( pWnd == NULL)
		return;

	CRect rect;
	pWnd->GetWindowRect( &rect);
	ScreenToClient( rect);

	if ( top_anchor == false)
		rect.top += delta;

	rect.bottom += delta;

	GetDlgItem( nID)->MoveWindow( rect);
}

void CModelInfoSidebar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize( nType, cx, cy);

	if ( 1)
	{
		CWnd* pWnd = GetDlgItem( IDC_GROUP3);
		if ( pWnd == NULL)	return;
		CRect rectGroup1;
		pWnd->GetWindowRect( &rectGroup1);
		ScreenToClient( rectGroup1);

		pWnd = GetDlgItem( IDC_GROUP4);
		if ( pWnd == NULL)	return;
		CRect rectGroup2;
		pWnd->GetWindowRect( &rectGroup2);
		ScreenToClient( rectGroup2);

		int dy = max( cy, 300) - rectGroup1.top - rectGroup2.bottom;

		_resize( IDC_GROUP3, dy, true);
		_resize( IDC_TREE_NODES, dy, true);

		_resize( IDC_CHECK_TRACE_LOC, dy, false);

		_resize( IDC_GROUP4, dy, false);
		_resize( IDC_EDIT_NODE_INFO, dy, false);
		_resize( IDC_STATIC_TM_INFO, dy, false);
	}
}
