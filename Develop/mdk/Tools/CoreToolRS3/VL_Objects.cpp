// V_EditEntityObjectList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <algorithm>
#include "MDebug.h"
#include "MXml.h"

#include "M_ToolLogicObjectManager.h"

#include "VL_Objects.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "WS_WorkSpaceManager.h"

#include "ReflectivePropertyGridCtrl.h"

#include "ext_lib/GridTree/GridBtnCell_src/GridBtnCell.h"
#include "ext_lib/GridTree/TreeColumn_src/GridTreeCell.h"

BEGIN_MESSAGE_MAP(LayerGridCtrl, CGridCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

const int FIXED_ROW_COUNT = 1;
const int COLUMN_HIDDEN_UNUSED = 1;	// 사용안하는 컬럼 userdata 를 담기위한것
const int COLUMN_HIDE = COLUMN_HIDDEN_UNUSED+1;	// visible check button 컬럼
const int COLUMN_FREEZE = COLUMN_HIDE+1;		// freeze check button 컬럼
const int COLUMN_TREE = 0;

LayerGridCtrl::LayerGridCtrl()
: m_pController( NULL )
{
}

bool LayerGridCtrl::Initialize(CControlMediator* pController)
{
	m_pController = pController;

	m_cfData = RegisterClipboardFormat(_T("LogicObjectArray"));
	return true;
}

LayerGridCtrl::~LayerGridCtrl()
{
}

void LayerGridCtrl::CutSelectedText()
{
}

COleDataSource* LayerGridCtrl::CopyTextFromGrid()
{
 	CSharedFile file;
 	CArchive ar(&file, CArchive::store);
 	UINT nSelectedCount = GetSelectedCount();
 	ar << nSelectedCount;
 
 	CCellRange selectedRange = GetSelectedCellRange();
 	for( int i = selectedRange.GetMinRow(); i <= selectedRange.GetMaxRow(); i++ )
 	{
 		if( IsCellSelected( i, 0 ) )
 		{
 			ar << i;
 		}
 	}
 	ar.Close();
 
	COleDataSource* pDataSource = NULL;

	TRY 
	{
		pDataSource = new COleDataSource;
		pDataSource->CacheGlobalData( m_cfData, file.Detach() );
	}
	CATCH_ALL(e)
	{
		delete pDataSource;
		THROW_LAST();
	}
	END_CATCH_ALL

	return pDataSource;
}

BOOL LayerGridCtrl::PasteTextToGrid(CCellID cell, COleDataObject* pDataObject)
{
	if(pDataObject->IsDataAvailable(m_cfData))
	{
		CFile* pFile = pDataObject->GetFileData(m_cfData);

		if (pFile == NULL) return TRUE;

		string strLayerName;
		CGridCellBase* pCell = GetCell(cell.row, cell.col);
		if (pCell)
		{
			CLogicObject* pLogicObject = GetLogicObject(cell.row);
			// drop 한 지점이 layer 가 아니라 오브젝트라면 해당 오브젝트의 layer를 선택
			if( pLogicObject )
				strLayerName  = pLogicObject->GetLayerName();
			else
				strLayerName = pCell->GetText();
		}

		CArchive ar(pFile, CArchive::load);
		TRY
		{
			int nCount;
			ar >> nCount;

			for(int i = 0; i < nCount; ++i)
			{
				int nRow;
				ar >> nRow;

				CLogicObject* pLogicObject = GetLogicObject(nRow);
				if(pLogicObject)
					pLogicObject->SetLayerName( strLayerName.c_str() );
			}
		}

		CATCH_ALL(e)
		{
			ar.Close();
			delete pFile;
			THROW_LAST();
		}
		END_CATCH_ALL

		ar.Close();
		delete pFile;
	}

	m_pController->BroadCastMessage( VIEW_MESSAGE::LOGIC_LAYER_CHANGED );

	return TRUE;
}

const char*	LayerGridCtrl::GetName( int nRow )
{
	CGridCellBase* pCell = GetCell( nRow, COLUMN_TREE );
	return pCell ? pCell->GetText() : NULL;
}

CLogicObject* LayerGridCtrl::GetLogicObject( int nRow )
{
	LPARAM pUserData = GetItemData( nRow, 1 );
	CLogicObject* pLogicObject = (CLogicObject*)pUserData;
	return pLogicObject;
}

void LayerGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// hack 스럽지만 원래 있던 리스트박스의 메시지인 LBN_DBLCLK 를 이용한다
	CWnd *pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
		pOwner->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), LBN_DBLCLK), (LPARAM) GetSafeHwnd());
}

void LayerGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)
	{
	}
	else
	__super::OnKeyDown( nChar, nRepCnt, nFlags );
}

DROPEFFECT LayerGridCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CRect rect;
	GetClientRect(rect);

	CCellID cid = GetCellFromPt(point);
	if( IsCellFixed(cid.row, cid.col) == false )
	{
		// 맨 아래 row 일경우 위로 스크롤
		if( IsCellVisible( cid ) && ( cid.row+1>=GetRowCount() || IsCellVisible( cid.row+1, cid.col ) == false ) )
		{
			CCellID idTopLeft = GetTopleftNonFixedCell();
			SetVerticalScroll( idTopLeft.row + 1 );
		}
		// 맨 위 row 인 경우 아래로 스크롤
		if( cid.row>0 && IsCellVisible( cid ) && IsCellVisible( cid.row-1, cid.col ) == false )
		{
			CCellID idTopLeft = GetTopleftNonFixedCell();
			SetVerticalScroll( idTopLeft.row - 1 );
		}
	}

	return __super::OnDragOver( pDataObject, dwKeyState, point );
}

void LayerGridCtrl::SetVerticalScroll( int nRow )
{
	nRow = min( max( nRow, 0 ), GetRowCount() );

	// 각행의 height 가 고정이라는 가정
	int nRowHeight = GetFixedRowHeight() / GetFixedRowCount();
	int nScrollBar = nRowHeight * ( nRow - GetFixedRowCount() ) ;

	SetScrollPos32(SB_VERT, nScrollBar, TRUE);
	// SetScrollPos32 의 redraw 가 안되네요
	RedrawWindow();
}


//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CDlgObjects, CDialog)

CDlgObjects::CDlgObjects(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgObjects::IDD, pParent)
	, m_pPropCtrl(NULL)
	, m_bShowList_(false)
{
}

CDlgObjects::~CDlgObjects()
{
}

BOOL CDlgObjects::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd* pPropertyParent = GetDlgItem( IDC_STATIC_PROPERTY );
	
	//**************************** MFC ReflectiveProperty 사용 방법 **************************************************
	m_pPropCtrl =	m_UI.createPropGridCtrl( pPropertyParent );	//이 다이알로그를 부모로 가지는 프로퍼티그리드콘트롤 생성

	CReflectivePropertyGridCtrl::PROPERTY_CHANGE_HANDLER handler(this, &CDlgObjects::OnPropertyChanged );
	m_pPropCtrl->SetCallback( handler );

	SetUpGridHeader();

	// will create some cells with btns, below
	m_BtnDataBase.SetGrid( &m_gridCtrl);

	CImageList* pImageList = m_pControlMediator_->GetWorkSpaceMgr()->GetImageList();

	m_gridCtrl.SetImageList( pImageList );
	m_gridCtrl.Initialize( m_pControlMediator_ );
	m_gridCtrl.EnableDragAndDrop();
	m_gridCtrl.EnableTitleTips(FALSE);
	m_gridCtrl.SetBkColor( RGB( 255, 255, 255 ) );
	m_gridCtrl.SetGridLines( GVL_VERT );

	return TRUE;
}

void CDlgObjects::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_LAYER_OBJECT, m_gridCtrl);
}


void CDlgObjects::OnUpdateCmdUI(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

BEGIN_MESSAGE_MAP(CDlgObjects, CDialog)
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BUTTON_NEW_LAYER, &CDlgObjects::OnBnClickedButtonNewLayer)
ON_LBN_DBLCLK(IDC_LAYER_OBJECT, &CDlgObjects::OnGridDblClk)
ON_NOTIFY(	GVN_SELCHANGED,		IDC_LAYER_OBJECT, OnGridSelChanged)
ON_NOTIFY(	GVN_BEGINLABELEDIT,	IDC_LAYER_OBJECT, OnGridBeginEdit)
ON_NOTIFY(	GVN_ENDLABELEDIT,	IDC_LAYER_OBJECT, OnGridEndEdit)
ON_NOTIFY(	NM_CLICK,			IDC_LAYER_OBJECT, OnGridClick)
ON_NOTIFY(	NM_RCLICK,			IDC_LAYER_OBJECT, OnGridRClick)
ON_COMMAND(ID_LAYER_SETASDEFAULTLAYER, &CDlgObjects::OnLayerSetAsDefaultLayer)
END_MESSAGE_MAP()

void CDlgObjects::ResetLayers()
{
	m_mapLayerObjects.clear();
	m_strDefaultLayer = "";
}

void CDlgObjects::Listen(CoreMessage& _message)
{
	static bool bSupressUpdateUI = false;

	switch(_message.nID)
	{
		case VIEW_MESSAGE::DEVICE_CREATED:
		{
		}
		break;
		case VIEW_MESSAGE::PRE_CLOSE_DOCUMENT:
		case VIEW_MESSAGE::PRE_OPEN_DOCUMENT: bSupressUpdateUI = true; break;

		case VIEW_MESSAGE::POST_CLOSE_DOCUMENT:
		case VIEW_MESSAGE::SCENE_LOADED:
		case VIEW_MESSAGE::SCENE_IMPORTED:
		case VIEW_MESSAGE::LOGIC_LOADED:
			bSupressUpdateUI = false;
			ResetLayers();
			RefreshObjectList();
			break;

		case VIEW_MESSAGE::LOGIC_NEW:
		case VIEW_MESSAGE::LOGIC_DELETE:
			if( bSupressUpdateUI == false )
				RefreshObjectList();
			break;

		case VIEW_MESSAGE::LOGIC_CHANGED:
			OnLogicObjectChanged( _message.pLogicObject );
			break;

		case VIEW_MESSAGE::UNDO_OBJ_NEW_DEL:
		case VIEW_MESSAGE::LOGIC_LAYER_CHANGED:
		{
			RefreshObjectList();
		}break;
		case VIEW_MESSAGE::UNDO_MOVE:
			m_pControlMediator_->GetHelperAction()->SetSelectedObjectCenterAndActionFrame();
		break;

		case VIEW_MESSAGE::LOGIC_SELECT:
			SetSelectObject( _message.pLogicObject, true );
			break;
		case VIEW_MESSAGE::LOGIC_UNSELECT:
			SetSelectObject( _message.pLogicObject, false );
			break;
		case VIEW_MESSAGE::SELECTION_CLEAR:
			SetSelectNone();
			break;
	}
}

void CDlgObjects::OnCancel()
{
	m_bShowList_ = false;
	ShowWindow(SW_HIDE);
}

void CDlgObjects::OnOK()
{
	m_bShowList_ = false;
	ShowWindow(SW_HIDE);
}

void CDlgObjects::SetUpGridHeader()
{
	m_gridCtrl.SetColumnCount( 4 );
	m_gridCtrl.SetRowCount( FIXED_ROW_COUNT );
	m_gridCtrl.SetFixedRowCount( FIXED_ROW_COUNT );

	const char* szHeader[] = { "Layers", "", "Hide", "Freeze" };
	int nWidth[] = { 285, 0, 24, 24 };
	for(int i=0; i< _countof(szHeader); i++ )
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = i;
		Item.strText = szHeader[i];
		m_gridCtrl.SetItem(&Item);
		m_gridCtrl.SetColumnWidth( i, nWidth[i] );
	}
}

typedef struct
{
	CGridBtnCellBase::STRUCT_DRAWCTL DrawCtl;  // most btn props here
	int iBtnNbr;                        // which btn within cell does this define?
	const char* pszBtnText;             // text associated with pushbutton or NULL
}   STRUCT_CTL;


void CDlgObjects::SetUpGridRow( int nRow, const char* szName, bool bLayer, CLogicObject* pLogicObject )
{
	//////////////////////////////////////////////////////////////////////////
	// tree 컬럼 세팅
	GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_FORMAT;
	Item.nState = 0;
	if( bLayer )
	{
		Item.mask |= GVIF_STATE;
		m_gridCtrl.SetItemImage( nRow, COLUMN_TREE, 1 );
	}

	Item.row = nRow;
	Item.col = COLUMN_TREE;

	Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
	Item.strText.Format( szName );
// 	Item.nState = GVIS_READONLY;
	m_gridCtrl.SetItem(&Item);

	if( pLogicObject )
		m_gridCtrl.SetItemImage( nRow, COLUMN_TREE, pLogicObject->GetIconIndex() );

	//////////////////////////////////////////////////////////////////////////
	// hidden column for userdata, 
	// treecolumn 은 애초부터 userdata 를 singleton 으로 하나만 지원
	// button 들은 btndatabase 만들어야 해서 복잡해서 숨겨진 컬럼을 하나 만듦
	// 그런데 포럼에 보면 숨겨진 컬럼이 맨뒤에 있어도 맨 앞에 있어도 버그가 있다고 한다 -_-

	Item.mask = GVIF_PARAM;
	Item.lParam = (LPARAM)pLogicObject;
	Item.col = COLUMN_HIDDEN_UNUSED;
	m_gridCtrl.SetItem(&Item);

	//////////////////////////////////////////////////////////////////////////
	// 버튼 컬럼 세팅
	Item.row = nRow;
	STRUCT_CTL CheckButtonCtl[] = {
	//iWidth,        sState, ucIsMbrRadioGrp,            ucAlign,		ucType, iBtnNbr, pszBtnText
	{  0, DFCS_BUTTONCHECK , FALSE, CGridBtnCellBase::CTL_ALIGN_LEFT,  DFC_BUTTON, 0, NULL },  // checkbox
	{  0, DFCS_BUTTONCHECK , FALSE, CGridBtnCellBase::CTL_ALIGN_LEFT,  DFC_BUTTON, 0, NULL }  // checkbox
	};

	if( pLogicObject )
	{
		if( pLogicObject->GetVisible() == false )
			CheckButtonCtl[0].DrawCtl.sState |= DFCS_CHECKED;

		if( pLogicObject->GetEditable() == false )
			CheckButtonCtl[1].DrawCtl.sState |= DFCS_CHECKED;
	}

	for( int i=0; i< _countof(CheckButtonCtl); i++)
	{
		Item.col = COLUMN_HIDE + i;
		Item.mask = GVIF_STATE;
		Item.nState = 0;
		Item.strText = "";

		// 이미 Setup이 완료된 row 일때 중복 설정을 피한다 ( GridCtrl 내부의 ASSERT 때문에 추가 )
		CGridBtnCell* pGridBtnCellBefore = dynamic_cast<CGridBtnCell*>( m_gridCtrl.GetCell( nRow, Item.col) );
		if( pGridBtnCellBefore == NULL )
			m_gridCtrl.SetCellType( nRow, Item.col, RUNTIME_CLASS(CGridBtnCell) );

		CGridBtnCell* pGridBtnCell = dynamic_cast<CGridBtnCell*>( m_gridCtrl.GetCell( nRow, Item.col) );
 		pGridBtnCell->SetBtnDataBase( &m_BtnDataBase);

		pGridBtnCell->SetupBtns(
			CheckButtonCtl[i].iBtnNbr,        // zero-based index of image to draw
			CheckButtonCtl[i].DrawCtl.ucType, // type of frame control to draw e.g. DFC_BUTTON
			CheckButtonCtl[i].DrawCtl.sState, // like DrawFrameControl()'s nState  e.g. DFCS_BUTTONCHECK
			(CGridBtnCellBase::CTL_ALIGN)CheckButtonCtl[i].DrawCtl.ucAlign,
			// horizontal alignment of control image
			CheckButtonCtl[i].DrawCtl.iWidth,                         // fixed width of control or 0 for size-to-fit
			CheckButtonCtl[i].DrawCtl.ucIsMbrRadioGrp,  // T=btn is member of a radio group
			CheckButtonCtl[i].pszBtnText );   // Text to insert centered in button; if NULL no text
	}
}

void CDlgObjects::SetUpGridRowAsObject( int nRow, CLogicObject* pObject )
{
	SetUpGridRow( nRow, pObject->GetObjectName(), false, pObject );
}

void CDlgObjects::SetUpGridRowAsLayer( int nRow, const char* szName)
{
	SetUpGridRow( nRow, szName, true, NULL );
}

void CDlgObjects::RefreshObjectList()
{
	if( m_gridCtrl.m_hWnd == 0 ) return;

	m_gridCtrl.DeleteAllItems();

	for( LAYER_OBJECTS_MAP::iterator itrLayer = m_mapLayerObjects.begin(); itrLayer!= m_mapLayerObjects.end(); ++itrLayer)
	{
		LOGIC_OBJECTLIST& listLayerObjects = itrLayer->second;
		listLayerObjects.clear();
	}

	SetUpGridHeader();

	LOGIC_OBJECTLIST& listLogicObject = m_pControlMediator_->GetToolLogicObjectMgr()->GetNoneEntityObjectList();
	if( listLogicObject.empty() ) return;

	// 레이어별로 오브젝트 분류
	for( LOGIC_OBJECTLIST::iterator itr = listLogicObject.begin(); itr!= listLogicObject.end(); ++itr)
	{
		CLogicObject* pLogicObject = *itr;

		LAYER_OBJECTS_MAP::iterator itrLayer = m_mapLayerObjects.find( pLogicObject->GetLayerName() );
		if( itrLayer == m_mapLayerObjects.end() )
		{
			LOGIC_OBJECTLIST emptyList;
			// 없으면 만든다
			itrLayer = m_mapLayerObjects.insert( LAYER_OBJECTS_MAP::value_type( pLogicObject->GetLayerName(), emptyList ) ).first;
		}

		LOGIC_OBJECTLIST& listLayerObjects = itrLayer->second;
		listLayerObjects.push_back( pLogicObject );
	}

	m_gridCtrl.SetRowCount( listLogicObject.size() + m_mapLayerObjects.size() + FIXED_ROW_COUNT );

	// level data for tree column
	vector<unsigned char> vecLevelData;

	int row = FIXED_ROW_COUNT;
	for( LAYER_OBJECTS_MAP::iterator itrLayer = m_mapLayerObjects.begin(); itrLayer!= m_mapLayerObjects.end(); ++itrLayer)
	{
		LOGIC_OBJECTLIST& listLayerObjects = itrLayer->second;
		vecLevelData.push_back( 1 );

		SetUpGridRowAsLayer( row++, itrLayer->first.c_str() );

		for( LOGIC_OBJECTLIST::iterator itr = listLayerObjects.begin(); itr!= listLayerObjects.end(); ++itr)
		{
			CLogicObject* pLogicObject = *itr;
			vecLevelData.push_back( 2 );

			SetUpGridRowAsObject( row++, pLogicObject );
		}
	}

	if(vecLevelData.empty() == false)
	{
		m_treeColumn.TreeSetup( &m_gridCtrl, // tree acts on a column in this grid
			COLUMN_TREE,           // which column has tree
			vecLevelData.size(), // total number of rows in the
			//  tree if totally expanded
			FIXED_ROW_COUNT,       // Set fixed row count now, too
			//  Grid total rows=aiTotalRows+aiFixedRowCount
			&vecLevelData[0],// Tree Level data array --
			//  must have aiTotalRows of entries
			TRUE,        // T=show tree (not grid) lines; F=no tree lines
			FALSE);       // T=use 1st 3 images from already set image list
	}
	//  to display folder graphics
	m_treeColumn.SetTreeLineColor( RGB( 64, 64, 64) );
	m_treeColumn.TreeDisplayOutline( UCHAR_MAX);

	RedrawLayerRows();
}

void CDlgObjects::RefreshProperty()
{
	m_pPropCtrl->RemoveAll();

	LOGIC_OBJECTLIST* pSelectedLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();
	
	list<CBaseObject*> m_objectSelected;
	for( LOGIC_OBJECTLIST::iterator itr = pSelectedLogicList->begin(); itr!= pSelectedLogicList->end(); ++itr )
	{
		CLogicObject* pLogicObject = *itr;

		if( pLogicObject->GetEditable() == false ) continue;

		CBaseObject* pReflectProperty = pLogicObject->GetReflectProperty();
		
		if(pReflectProperty)
			m_objectSelected.push_back( pReflectProperty );
	}

	m_UI.BuildUIForObjects(m_pPropCtrl, m_objectSelected);	// 프로퍼티그리드아이템추가
	m_pPropCtrl->ExpandAll();

}

void CDlgObjects::SetSelectNone()
{
	m_gridCtrl.SetSelectedRange( 0, 0, 0, 0, TRUE, TRUE);
	m_gridCtrl.SetSelectedRange( 0, 0, 0, 0, TRUE, FALSE); // 언셀렉트가 없나 -_-

	RefreshProperty();

}

void CDlgObjects::OnLogicObjectChanged( CLogicObject* pLogicObject )
{
	// 이름등 프로퍼티가 바뀐경우 Grid Text 에 반영
	LOGIC_OBJECTLIST listSelected;
	CCellRange selectedRange = m_gridCtrl.GetSelectedCellRange();
	for( int i = selectedRange.GetMinRow(); i <= selectedRange.GetMaxRow(); i++ )
	{
		if( m_gridCtrl.IsCellSelected( i, 0 ) )
		{
			CLogicObject* pLogicObjectSelected = m_gridCtrl.GetLogicObject( i );
			if( pLogicObjectSelected == pLogicObject )
			{
				SetUpGridRowAsObject( i, pLogicObject );
				m_gridCtrl.RedrawRow( i );
			}
		}
	}
}

void CDlgObjects::OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	/*
	CLogicObject* pLogicObject = m_gridCtrl.GetLogicObject( pItem->iRow );
	BOOL bSelected = m_gridCtrl.IsCellSelected( pItem->iRow, pItem->iColumn );

	if( bSelected )
		m_pControlMediator_->GetHelperAction()->AddSelectection( pLogicObject, true );
	else
		m_pControlMediator_->GetHelperAction()->RemoveSelection( pLogicObject, true );
	*/

	m_pPropCtrl->RemoveAll();

	LOGIC_OBJECTLIST listSelected;
	CCellRange selectedRange = m_gridCtrl.GetSelectedCellRange();
	for( int i = selectedRange.GetMinRow(); i <= selectedRange.GetMaxRow(); i++ )
	{
		if( m_gridCtrl.IsCellSelected( i, 0 ) )
		{
			CLogicObject* pLogicObject = m_gridCtrl.GetLogicObject( i );
			listSelected.push_back( pLogicObject );
		}
	}


	m_pControlMediator_->GetHelperAction()->RemoveAllSelection(false);

	// CDlgSceneNodeList::OnTvnSelchangedTreeObject 에서와 같이 중복 업데이트 문제 해결하고싶다
	for( LOGIC_OBJECTLIST::iterator itr = listSelected.begin(); itr != listSelected.end(); ++itr )
	{
		m_pControlMediator_->GetHelperAction()->AddSelectection( *itr, false);
	}

}

void CDlgObjects::OnGridBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	CGridCellBase* pCell = m_gridCtrl.GetCell(pItem->iRow, pItem->iColumn);
	m_strBeforeEdit = pCell->GetText();
}

void CDlgObjects::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	CGridCellBase* pCell = m_gridCtrl.GetCell(pItem->iRow, pItem->iColumn);

	// object 이름이 바뀐경우, TODO: 현재 read-only 라 동작하지않는다
	CLogicObject* pLogicObject = m_gridCtrl.GetLogicObject( pItem->iRow );
	if(pLogicObject)
	{
		if (pCell)
		{
			pLogicObject->SetObjectName( pCell->GetText() );
			RefreshProperty();
		}
		return;
	}

	// layer 이름이 바뀐경우 해당 레이어의 모든 오브젝트의 layer 속성을 바꿔준다
	
	if( pCell == NULL ) return;

	const char* szNewLayerName = pCell->GetText();

	LAYER_OBJECTS_MAP::iterator itrNewLayer = m_mapLayerObjects.find( szNewLayerName );
	if( itrNewLayer != m_mapLayerObjects.end() )
	{
		// 이미 있다. 되돌리기
		MessageBox("Warning! Layers of the same name exist.");
		pCell->SetText( m_strBeforeEdit.c_str() );

		return;
	}

	// 원래 layer 를 찾는다
	LAYER_OBJECTS_MAP::iterator itrLayer = m_mapLayerObjects.find( m_strBeforeEdit );

	const LOGIC_OBJECTLIST& listObjects = itrLayer->second;
	m_mapLayerObjects[ szNewLayerName ] = listObjects;

	for( LOGIC_OBJECTLIST::const_iterator itr = listObjects.begin(); itr!= listObjects.end(); ++itr)
	{
		CLogicObject* pLogicObject = *itr;
		pLogicObject->SetLayerName( szNewLayerName );
	}

	m_mapLayerObjects.erase( itrLayer );
}

// hide & freeze 버튼 처리
void CDlgObjects::OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if (pItem->iRow < 0)
		return;

	int nRow = pItem->iRow;
	int nCol = pItem->iColumn;

	if( nCol != COLUMN_HIDE && nCol != COLUMN_FREEZE )
		return;

	bool bButtonState = GetButtonCheckState( nRow, nCol );

	CLogicObject* pLogicObject = m_gridCtrl.GetLogicObject( nRow );
	if( pLogicObject )
	{
		switch( nCol)
		{
		case COLUMN_HIDE: SetVisible( pLogicObject, bButtonState ); break;
		case COLUMN_FREEZE: FreezeObject( pLogicObject, bButtonState ); break;
		}

	}else
	{
		// 레이어인 경우
		const char* szLayerName = m_gridCtrl.GetName( nRow );

		switch( nCol)
		{
		case COLUMN_HIDE: HideLayer( szLayerName, bButtonState ); break;
		case COLUMN_FREEZE: FreezeLayer( szLayerName, bButtonState ); break;
		}

		// 속한 오브젝트의 UI 버튼 갱신
		// nRow 밑의 오브젝트 수 만큼 갱신, hierarchy 가 없다고 가정.. 있으면 다시 구현해야한다.
		LAYER_OBJECTS_MAP::iterator itrLayer = m_mapLayerObjects.find( szLayerName );
		if( itrLayer == m_mapLayerObjects.end() )
			return;

		const LOGIC_OBJECTLIST& listObjects = itrLayer->second;
		for( unsigned int i=0; i<listObjects.size(); i++ )
		{
			SetButtonCheckState( nRow + i + 1, nCol, bButtonState);
		}

	}
}

const int MENU_ID_BASE_MOVE_LAYER_TO = 40000;

BOOL CDlgObjects::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int nCommandID = LOWORD( wParam );

	// move layer to
	if( MENU_ID_BASE_MOVE_LAYER_TO <= nCommandID && nCommandID < MENU_ID_BASE_MOVE_LAYER_TO+(int)m_mapLayerObjects.size() )
	{
		int i = 0;
		for (LAYER_OBJECTS_MAP::iterator itr = m_mapLayerObjects.begin(); itr != m_mapLayerObjects.end(); ++itr, ++i )
		{
			if( (nCommandID-MENU_ID_BASE_MOVE_LAYER_TO) == i )
			{
				const char* szName = itr->first.c_str();
				SelectedMoveLayerTo( szName );
				RefreshObjectList();
				return TRUE;
			}
		}

	}

	return CWnd::OnCommand(wParam, lParam);
}

void CDlgObjects::SelectedMoveLayerTo( const char* szLayerName )
{
	CCellRange selectedRange = m_gridCtrl.GetSelectedCellRange();
	for( int i = selectedRange.GetMinRow(); i <= selectedRange.GetMaxRow(); i++ )
	{
		if( m_gridCtrl.IsCellSelected( i, 0 ) )
		{
			CLogicObject* pLogicObject = m_gridCtrl.GetLogicObject(i);
			if(pLogicObject)
				pLogicObject->SetLayerName( szLayerName );
		}
	}
}

void CDlgObjects::OnGridRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 팝업메뉴
	CPoint ptCursor;
	GetCursorPos(&ptCursor);

	// Layer Context Menu
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if (pItem->iRow >= 0 )
	{
		m_nContextMenuRow = pItem->iRow;

		CLogicObject* pLogicObject = m_gridCtrl.GetLogicObject( pItem->iRow );
		if( pLogicObject == NULL )	// 레이어인 경우
		{
			CMenu menu;
			menu.LoadMenu( IDR_MENU_LAYER_CONTEXT );

			CMenu* layerMenu = menu.GetSubMenu(0);
			if(layerMenu)
				layerMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, ptCursor.x, ptCursor.y, this );
			return;
		}
	}

	// Object Context Menu
	CMenu menu;
	menu.CreatePopupMenu();

	// sub menu
	CMenu subMenu;
	subMenu.CreatePopupMenu();

	int i = 0;
	for (LAYER_OBJECTS_MAP::iterator itr = m_mapLayerObjects.begin(); itr != m_mapLayerObjects.end(); ++itr, ++i)
	{
		const char* szName = itr->first.c_str();
		subMenu.AppendMenu( MF_STRING, MENU_ID_BASE_MOVE_LAYER_TO + i, szName );
	}

	menu.AppendMenu(MF_STRING | MF_POPUP, (UINT)subMenu.m_hMenu, "Move Layer to");

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, ptCursor.x, ptCursor.y, this );

}

bool CDlgObjects::GetButtonCheckState(int nRow, int nCol )
{
	bool bButtonState = false;
	CGridBtnCell* pButtonCell = (CGridBtnCell*)m_gridCtrl.GetCell( nRow, nCol );
	if (pButtonCell)
	{
		int nState = pButtonCell->GetDrawCtlState( 0 );
		bButtonState = ( nState & DFCS_CHECKED ) != NULL;
	}
	return bButtonState;
}

void CDlgObjects::SetButtonCheckState(int nRow, int nCol, bool bCheck )
{
	CGridBtnCell* pButtonCell = (CGridBtnCell*)m_gridCtrl.GetCell( nRow, nCol );
	if (pButtonCell)
	{
		int nState = pButtonCell->GetDrawCtlState( 0 );
		if( bCheck )			
			nState |= DFCS_CHECKED;
		else
			nState &= ~DFCS_CHECKED;

		pButtonCell->SetDrawCtlState( 0, nState );
		m_gridCtrl.RedrawCell(nRow, nCol);
	}
}

void CDlgObjects::SetVisible( CLogicObject* pLogicObject, bool bHide )
{
	m_pControlMediator_->GetHelperAction()->SetVisible( pLogicObject, bHide == false );
}

void CDlgObjects::HideLayer( const char* szLayerName, bool bHide )
{
	LAYER_OBJECTS_MAP::iterator itrLayer = m_mapLayerObjects.find( szLayerName );
	if( itrLayer == m_mapLayerObjects.end() )
		return;

	const LOGIC_OBJECTLIST& listObjects = itrLayer->second;
	for( LOGIC_OBJECTLIST::const_iterator itr = listObjects.begin(); itr!= listObjects.end(); ++itr)
	{
		CLogicObject* pLogicObject = *itr;
		m_pControlMediator_->GetHelperAction()->SetVisible( pLogicObject, bHide == false );
	}
}

void CDlgObjects::FreezeObject( CLogicObject* pLogicObject, bool bFreeze )
{
	pLogicObject->SetEditable( !bFreeze );
}

void CDlgObjects::FreezeLayer( const char* szLayerName, bool bFreeze )
{
	LAYER_OBJECTS_MAP::iterator itrLayer = m_mapLayerObjects.find( szLayerName );
	if( itrLayer == m_mapLayerObjects.end() )
		return;

	const LOGIC_OBJECTLIST& listObjects = itrLayer->second;
	for( LOGIC_OBJECTLIST::const_iterator itr = listObjects.begin(); itr!= listObjects.end(); ++itr)
	{
		CLogicObject* pLogicObject = *itr;
		pLogicObject->SetEditable( !bFreeze );
	}
}

bool CDlgObjects::SetSelectObject(CLogicObject* _pLogicObject, BOOL _bFlag)
{
	for( int i = FIXED_ROW_COUNT; i < m_gridCtrl.GetRowCount(); i++ )
	{
		if( _pLogicObject == m_gridCtrl.GetLogicObject(i) )
		{
			UINT nState = m_gridCtrl.GetItemState( i, 0 );
			if( _bFlag )
				nState |= GVIS_SELECTED;
			else
				nState &= ~GVIS_SELECTED;

			m_gridCtrl.SetItemState( i, 0, nState );

			if( m_gridCtrl.IsCellVisible( i, 0 ) )
				m_gridCtrl.RedrawRow( i );
			else
			{
				m_gridCtrl.SetVerticalScroll( max( 0, i - 10 ) );
			}

			RefreshProperty();

			return true;
		}
	}

	return false;
}

void CDlgObjects::OnGridDblClk()
{
	rs3::RToolCamera* pCamear = m_pControlMediator_->GetToolCamera();
	if( pCamear == NULL)
		return;

	if(m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject()->size() == 0) return;

	CLogicObject* pLogicObject = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject()->front();
	if( pLogicObject == NULL )
		return;

	rs3::RVector vecSelected;
	if (pLogicObject->GetPosition(vecSelected) == false )
		return;

	pCamear->SetPositionToViewCenter( vecSelected, Tool3Const::CONST_WB_ENTITY_OBJ_TO_CENTER );
}

void CDlgObjects::OnPropertyChanged()
{
	LOGIC_OBJECTLIST* pSelectedLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();

	for(LOGIC_OBJECTLIST::iterator it_logic = pSelectedLogicList->begin(); it_logic != pSelectedLogicList->end(); ++it_logic)
	{
		CLogicObject* pLogicObject = *it_logic;
		pLogicObject->OnPropertyChangedFromUI();
		m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_CHANGED, pLogicObject ) );
	}
}

// TODO: 중복코드 제거
void CDlgObjects::ShowToggle()
{
	m_bShowList_ = !m_bShowList_;
	int nShowState = m_bShowList_ ? SW_SHOW : SW_HIDE;
	ShowWindow(nShowState);
}

const char* CDlgObjects::GetUniqueLayerName()
{
	// CToolLogicObjectManager::GetUniqueObjectName 의 copy&paste, 어떻게 중복을 피할수 있을까..?

	const char* szPrefix = "Layer";

	static char szNewName[256];

	int nNumber = 1;

	for (LAYER_OBJECTS_MAP::iterator itr = m_mapLayerObjects.begin(); itr != m_mapLayerObjects.end(); ++itr)
	{
		const char* szName = itr->first.c_str();
		// 접두어가 맞으면
		if( strncmp( szName, szPrefix, strlen(szPrefix) ) == 0 )
		{
			int nThisNumber = atoi( szName + strlen(szPrefix) );
			if( nThisNumber >= nNumber )
				nNumber = nThisNumber +1;
		}
	}

	sprintf( szNewName, "%s%d", szPrefix, nNumber );
	return szNewName;
}

void CDlgObjects::OnBnClickedButtonNewLayer()
{
	int nRowCount = m_gridCtrl.GetRowCount();
	m_gridCtrl.SetRowCount( nRowCount + 1 );

	const char* szNewLayerName = GetUniqueLayerName();
	SetUpGridRow( nRowCount, szNewLayerName, true, NULL );

	LOGIC_OBJECTLIST emptyList;
	m_mapLayerObjects.insert( LAYER_OBJECTS_MAP::value_type( szNewLayerName, emptyList ) );

	m_gridCtrl.SetVerticalScroll( nRowCount );
}

void CDlgObjects::OnDestroy()
{
	if(m_pPropCtrl)
	{
		m_pPropCtrl->DestroyWindow();
		SAFE_DELETE(m_pPropCtrl);
	}

	m_gridCtrl.DeleteAllItems();
	m_gridCtrl.DestroyWindow();

	__super::OnDestroy();
}

const char* CDlgObjects::GetDefaultLayerName()
{
	return m_strDefaultLayer.c_str();
}

void CDlgObjects::RedrawLayerRows()
{
	CCellRange visibleCellRange = m_gridCtrl.GetVisibleNonFixedCellRange();
	for( int i = visibleCellRange.GetMinRow(); i <= visibleCellRange.GetMaxRow(); i++ )
	{
		if( m_gridCtrl.GetLogicObject(i) == NULL )
		{
			// 디폴트 레이어인 경우
			if( strcmp( m_gridCtrl.GetName(i), GetDefaultLayerName() ) == 0)
				m_gridCtrl.SetItemImage( i, COLUMN_TREE, 2 );
			else
				m_gridCtrl.SetItemImage( i, COLUMN_TREE, 1 );

			m_gridCtrl.RedrawCell( i, COLUMN_TREE );
		}
	}
}

void CDlgObjects::OnLayerSetAsDefaultLayer()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_strDefaultLayer = m_gridCtrl.GetName( m_nContextMenuRow );

	RedrawLayerRows();
}
