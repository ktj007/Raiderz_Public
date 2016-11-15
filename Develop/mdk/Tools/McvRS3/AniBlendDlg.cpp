// AniBlendDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <algorithm>
#include "AniBlendDlg.h"
#include "AniBlendGridCell.h"
#include "RMeshAnimationMgr.h"
#include "RAnimationBlendTable.h"
#include "AniBlendDurationDlg.h"

// CAniBlendDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAniBlendDlg, CDialog)

CAniBlendDlg::CAniBlendDlg(CWnd* pParent /*=NULL*/) :
	CDialog(CAniBlendDlg::IDD, pParent),
	m_AnimationMgr(NULL)
{
}

CAniBlendDlg::~CAniBlendDlg()
{
}

void CAniBlendDlg::ClearAniBlendGrid()
{
	if( m_AnimationMgr )
	{
		m_AnimationMgr->GetAnimationBlendTable().Clear();
		m_AnimationMgr = NULL;
	}
}

void CAniBlendDlg::ResetAniBlendGrid( RMeshAnimationMgr* animationMgr, const map< string, string>* pExternAni )
{
	ASSERT( animationMgr );

	m_AnimationMgr = animationMgr;

	RAnimationBlendTable& blendTable = m_AnimationMgr->GetAnimationBlendTable();
//	blendTable.Reset( animationMgr );

	RAnimationHashList& animationList = m_AnimationMgr->m_NodeHashList;
	const int count = animationList.size();

	std::vector<std::string> sortedList;
	sortedList.reserve( count );
	for( int i = 0; i < count; ++i )
	{
		if( animationList[i] )
		{
			sortedList.push_back( animationList[i]->GetAliasName() );
		}
	}
	std::sort( sortedList.begin(), sortedList.end() );

	m_Grid.DeleteAllItems();
	m_Grid.SetFixedRowCount();
	m_Grid.SetFixedColumnCount();
	m_Grid.SetRowCount( count + 1 );
	m_Grid.SetColumnCount( count + 1 );

	for( int i = 0; i < count; ++i )
	{
		string strName = sortedList[ i];
		if ( pExternAni  &&  (pExternAni->find( strName) != pExternAni->end()) )
			strName.insert( 0, "* ");

		int c = i + 1;
		m_Grid.SetItemText( 0, c, strName.c_str() );
		m_Grid.SetItemFormat( 0, c, m_sCellTextFormat );
		// 타겟 애니메이션 블렌딩을 한꺼번에 변경하기 위한 표식
		m_Grid.SetItemData( 0, c, 1 );

		m_Grid.SetItemText( c, 0, strName.c_str() );
		m_Grid.SetItemFormat( c, 0, m_sCellTextFormat );

		m_Grid.SetItemBkColour( c, c, RGB( 100, 100, 100 ) );
	}

	for( int i = 0; i < count; ++i )
	{
		for( int j = 0; j < count; ++j )
		{
			if( i != j )
			{
//				RANIMATIONTRANSITIONTYPE type = blendTable.GetAnimationTransType( sortedList[i].c_str(), sortedList[j].c_str() ).type;
				BLENDDATA blendData = blendTable.GetAnimationTransData( sortedList[i].c_str(), sortedList[j].c_str() );
				m_Grid.SetItemFormat( i + 1, j + 1, m_sCellTextFormat );
				RefreshGridData( blendData, i + 1, j + 1 );
			}
		}
	}

	m_Grid.AutoSize();
}

void CAniBlendDlg::ChangeAnimationBlendType( RANIMATIONTRANSITIONTYPE blendType )
{
	ASSERT( m_AnimationMgr );

	CCellID cellId = m_Grid.GetFocusCell();

	CString source = m_Grid.GetItemText( cellId.row, 0 );
	CString target = m_Grid.GetItemText( 0, cellId.col );
	
	BLENDDATA blendData = GetAnimationTransData( source, target );

	blendData.eType = blendType;
	m_AnimationMgr->GetAnimationBlendTable().SetAnimationTransData( source, target, blendType, blendData.nDuration );

	RefreshGridData( blendData, cellId.row, cellId.col );

	m_Grid.RedrawWindow();
}

void CAniBlendDlg::ChangeAnimationBlendTypeAllTarget( RANIMATIONTRANSITIONTYPE blendType )
{
	ASSERT( m_AnimationMgr );

	RAnimationBlendTable& blendTable = m_AnimationMgr->GetAnimationBlendTable();

	CCellID cellId = m_Grid.GetFocusCell();
	CString target = m_Grid.GetItemText( 0, cellId.col );

	for( int i = 1; i < m_Grid.GetRowCount(); ++i )
	{
		if( i != cellId.col )
		{
			CString source = m_Grid.GetItemText( i, 0 );

			BLENDDATA blendData = GetAnimationTransData( source, target );
			blendData.eType = blendType;
			blendTable.SetAnimationTransData( source, target, blendType, blendData.nDuration );
			RefreshGridData( blendData, i, cellId.col );
		}
	}

	m_Grid.RedrawWindow();
}

void CAniBlendDlg::ChangeAnimationBlendDuration()
{
	ASSERT( m_AnimationMgr );

	CCellID cellId = m_Grid.GetFocusCell();

	CString source = m_Grid.GetItemText( cellId.row, 0 );
	CString target = m_Grid.GetItemText( 0, cellId.col );

	BLENDDATA blendData = GetAnimationTransData( source, target );

	AniBlendDurationDlg dlg;
	dlg.m_editDuration.Format( "%d", blendData.nDuration);
	dlg.DoModal();

	unsigned int nNewDuration = BLENDDATA::DEFAULT_BLEND_DURATION;
	sscanf( LPCSTR(dlg.m_editDuration), "%d", &nNewDuration);
	blendData.nDuration = nNewDuration;

	m_AnimationMgr->GetAnimationBlendTable().SetAnimationTransData( source, target, blendData.eType, blendData.nDuration );

	RefreshGridData( blendData, cellId.row, cellId.col );

	m_Grid.RedrawWindow();
}

BLENDDATA CAniBlendDlg::GetAnimationTransData( const char* source, const char* target )
{
	ASSERT( m_AnimationMgr );

	return m_AnimationMgr->GetAnimationBlendTable().GetAnimationTransData( source, target );
}

void CAniBlendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridCtrlDemoDlg)
	DDX_Control(pDX, IDC_GRID, m_Grid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAniBlendDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CAniBlendDlg 메시지 처리기입니다.

BOOL CAniBlendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_Grid.SetDefaultCellType( RUNTIME_CLASS(CAniBlendGridCell) );

	m_Grid.GetDefaultCell( FALSE, FALSE )->SetBackClr( RGB( 200, 200, 200 ) );

	m_Grid.SetFixedColumnSelection( FALSE );
	m_Grid.SetFixedRowSelection( FALSE );

	m_Grid.SetSingleRowSelection();
	m_Grid.SetSingleColSelection();

	m_Grid.SetRowResize( TRUE );
	m_Grid.SetColumnResize( TRUE );

	m_Grid.SetEditable( FALSE );
	m_Grid.EnableSelection( FALSE );

	m_Grid.SetTrackFocusCell( TRUE );
	m_Grid.SetFrameFocusCell( TRUE );

	m_Grid.EnableTitleTips( FALSE );

//	m_Grid.SetFixedBkColor( RGB( 255, 0, 0 ) );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAniBlendDlg::RefreshGridData( BLENDDATA blendData, int row, int col )
{
	struct {
		const char* szName;
		COLORREF color;
	} gridLookTable[RAT_COUNT+1] =
	{
		"None",			RGB( 200, 200, 200 ),
		"Immediate",	RGB( 233, 196, 67 ),
		"Delayed",		RGB( 228, 111, 71 ),
		"Crossfade",	RGB( 126, 205, 31 ),
		"_max", RGB(0,0,0)
	};

	_ASSERT( _stricmp(gridLookTable[RAT_COUNT].szName,"_max")==0 );

	CString strText;

	if( blendData.nDuration != BLENDDATA::DEFAULT_BLEND_DURATION )
		strText.Format ("%s,%d", gridLookTable[blendData.eType].szName, blendData.nDuration );
	else
		strText.Format ("%s", gridLookTable[blendData.eType].szName );

	m_Grid.SetItemText( row, col, strText );
	m_Grid.SetItemBkColour( row, col, gridLookTable[blendData.eType].color );
}

void CAniBlendDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( m_Grid.GetColumnCount() )
		m_Grid.MoveWindow( 0, 0, cx, cy, TRUE );
}