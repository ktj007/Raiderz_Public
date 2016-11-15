// V_EditObjectList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VL_SceneNodeList.h"

//#include "CoreToolRS3.h"
//#include "CoreToolRS3_View.h"
#include "C_ControlCursor.h"
#include "C_XMLEditInfoFile.h"

#include "C_ControlMediator.h"
#include "VL_ToolBar.h"
#include "M_LogicObject.h"

#include "VD_WaterEditor.h"
#include "M_LogicObject_SceneNodes.h"

#include "RActorNode.h"

#include "M_Visitors.h"
#include "WS_WorkSpaceManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSceneNodeTreeCtrl, CTreeCtrlExM)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CSceneNodeTreeCtrl, CTreeCtrlExM)

void CSceneNodeTreeCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) {
	NMTVCUSTOMDRAW* pcd = (NMTVCUSTOMDRAW*)pNMHDR;
	HTREEITEM hItem;

	switch ( pcd->nmcd.dwDrawStage ) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;     
		break;
	case CDDS_ITEMPREPAINT :
		hItem = (HTREEITEM)pcd->nmcd.dwItemSpec;
		rs3::RSceneNode* pNode = (rs3::RSceneNode*)GetItemData(hItem);
		if (pNode && pNode->QueryAttribute(rs3::RSNA_DO_NOT_SAVE) ) {
			pcd->clrText = RGB(196,196,196);
		} else {
			HTREEITEM   hItem = (HTREEITEM)pcd->nmcd.dwItemSpec;

			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			{
				pcd->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);    
				pcd->clrTextBk = GetSysColor(COLOR_HIGHLIGHT);
			}
		}
		*pResult = CDRF_DODEFAULT;// do not set *pResult = CDRF_SKIPDEFAULT
		break;
	}    
} 

CObjectPropertyGridCtrl::CObjectPropertyGridCtrl() :
	m_pSelectedSceneNode_( NULL ),
	m_bFocused( false )
{
}

CObjectPropertyGridCtrl::~CObjectPropertyGridCtrl()
{
}

void CObjectPropertyGridCtrl::SetSelectedSceneNode( rs3::RSceneNode* _pSelectedNode )
{
	m_bFocused = false;

	RemoveAll();

	m_pSelectedSceneNode_ = _pSelectedNode;
	if( m_pSelectedSceneNode_ != NULL )
	{
		CControlSceneProperty* pProperty = dynamic_cast<CControlSceneProperty*>((CCoreObject*)m_pSelectedSceneNode_->GetUserData());
		CLogicObject* pLogicObject = dynamic_cast<CLogicObject*>((CCoreObject*)m_pSelectedSceneNode_->GetUserData());
		if(pLogicObject)
			pProperty = pLogicObject->GetProperty();

		if( pProperty != NULL )
		{
			// sticky 노드들은 수정 불가
			bool bSticky = m_pSelectedSceneNode_->QueryAttribute(rs3::RSNA_DO_NOT_SAVE);

			pProperty->Init( this, m_pSelectedSceneNode_, !bSticky );
		}
	}

	RedrawWindow();
}




void CObjectPropertyGridCtrl::OnPropertyChanged( CMFCPropertyGridProperty* pProp ) const
{
	if ( m_pSelectedSceneNode_ )
	{
		CMFCPropertyGridColorProperty* pPropColor = dynamic_cast<CMFCPropertyGridColorProperty*>( pProp );
		if( pPropColor )
			pPropColor->SetColor( pPropColor->GetColor() );

		CCoreToolPropertyGridCtrl::OnPropertyChanged( pProp );

		CControlSceneProperty* pProperty = dynamic_cast<CControlSceneProperty*>((CCoreObject*)m_pSelectedSceneNode_->GetUserData());
		CLogicObject* pLogicObject = dynamic_cast<CLogicObject*>((CCoreObject*)m_pSelectedSceneNode_->GetUserData());
		if(pLogicObject)
			pProperty = pLogicObject->GetProperty();

		if(!pProperty) return;

		pProperty->OnPropertyChanged( pProp, m_pSelectedSceneNode_ );
	}
}

void CObjectPropertyGridCtrl::OnChangeSelection( CMFCPropertyGridProperty* pNewSel, CMFCPropertyGridProperty* pOldSel )
{
	if( pNewSel )
		m_bFocused = true;
	else
		m_bFocused = false;
}

//check box observer
void CDlgSceneNodeList::CTreeCheckBoxObserver::ObserveCheckBox( HTREEITEM hClickedItem )
{
	_ASSERT( m_pEditObjectList );

	bool bWillCheck = (m_pEditObjectList->m_ctlObjectTree.GetCheck( hClickedItem ) == FALSE );

	// 부모가 있다면,
	if( m_pEditObjectList->m_ctlObjectTree.GetParentItem(hClickedItem) )
	{
		m_pEditObjectList->m_ctlObjectTree.SetCheck( hClickedItem, bWillCheck? TRUE : FALSE );
		rs3::RSceneNode* pNode = (rs3::RSceneNode*)m_pEditObjectList->m_ctlObjectTree.GetItemData(hClickedItem);
		if( pNode )
		{
			Process( pNode, bWillCheck );
		}
	}
	else // 부모가 없는 최상위라면
		UpdateCheckBox(bWillCheck, hClickedItem );
}

void CDlgSceneNodeList::CTreeCheckBoxObserver::UpdateCheckBox(bool _bWillCheck, HTREEITEM hTreeItem )
{
	_ASSERT( m_pEditObjectList );

	m_pEditObjectList->m_ctlObjectTree.SetCheck( hTreeItem, _bWillCheck? TRUE : FALSE );
	rs3::RSceneNode* pNode = (rs3::RSceneNode*)m_pEditObjectList->m_ctlObjectTree.GetItemData(hTreeItem);
	if( pNode )
	{
		Process(pNode, _bWillCheck );
	}

	for( HTREEITEM hChild = m_pEditObjectList->m_ctlObjectTree.GetChildItem( hTreeItem ); hChild != NULL; )
	{
		UpdateCheckBox( _bWillCheck, hChild );
		hChild = m_pEditObjectList->m_ctlObjectTree.GetNextSiblingItem(hChild);
	}
}

void CDlgSceneNodeList::CTreeCheckBoxObserver::Process(rs3::RSceneNode* pSceneNode, bool _bWillCheck )
{
	_ASSERT( pSceneNode );

	pSceneNode->SetVisible( _bWillCheck );

	CVisibleSetter visibleSetter( _bWillCheck );
	pSceneNode->Traverse( &visibleSetter );

	// 이펙트가 있으면 Play, Stop해준다
	if( _bWillCheck )
	{
		CEffectStarter effectStarter;
		pSceneNode->Traverse( &effectStarter );
	}
	else
	{
		CEffectStopper effectStopper;
		pSceneNode->Traverse( &effectStopper );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

// CEditObjectList 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSceneNodeList, CDialog)
CDlgSceneNodeList::CDlgSceneNodeList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSceneNodeList::IDD, pParent)
	, m_bHierachyView(FALSE)
	, m_pEditWater_(NULL)
	, m_bShowList_(false)
	, m_nSelectedObjectCount(0)
	, m_pPropCtrl(NULL)
{
}

CDlgSceneNodeList::~CDlgSceneNodeList()
{
}

BOOL CDlgSceneNodeList::OnInitDialog(){
	CDialog::OnInitDialog();

	RECT rect;
	/*rect.left = 9;
	rect.top = 430;
	rect.right = 319;
	rect.bottom = 630;*/

	rect.left	= 325;
	rect.top	= 27;
	rect.right	= 635;
	rect.bottom = 619;

	m_ctlObjProperty_.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rect, this, (UINT)-1);
	m_pPropCtrl = m_UI.createPropGridCtrl(&m_ctlObjProperty_);

	// create treeview control
	m_ctlObjectTree.ClearSelection();

	#pragma warning (disable : 4312 )
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE);	// 워닝 어케 없애나요 ?
	#pragma warning (default : 4312 )

	// comctl 버젼이 6.0 이상으로 올라가면 ( xp ) 다중선택이 제대로 동작안하고 2개만 선택됩니다
	// by pok. 2010. 11. 15. 트리다중선택 버그 수정했습다.
	//DWORD dwMajor, dwMinor;
	//AtlGetCommCtrlVersion(&dwMajor, &dwMinor);
	//_ASSERT( dwMajor == 5  && dwMinor == 82 );

	CImageList* pImageList = m_pControlMediator_->GetWorkSpaceMgr()->GetImageList();
	m_ctlObjectTree.SetImageList( pImageList, TVSIL_NORMAL);

	m_treeCheckBoxObserver.m_pEditObjectList = this;
	m_ctlObjectTree.RegisterCheckBoxObserver( &m_treeCheckBoxObserver );

	// create water editor dialog
	ASSERT(m_pEditWater_ == NULL);
	m_pEditWater_ = new CDlgWaterEditor;
	m_pEditWater_->Create(CDlgWaterEditor::IDD, this);

	return TRUE;
}

void CDlgSceneNodeList::OnDestroy()
{
	ASSERT(m_pEditWater_);
	m_pEditWater_->DestroyWindow();
	delete m_pEditWater_;

	if(m_pPropCtrl)
	{
		m_pPropCtrl->DestroyWindow();
		SAFE_DELETE(m_pPropCtrl);
	}

	m_ctlObjProperty_.DestroyWindow();

	// 지워주지 않으면 ( 릭이 생겨 안지워진 신노드가 있을때 ) 뻗을수 있다
	m_ctlObjectTree.DeleteAllItems();
	m_ctlObjectTree.DestroyWindow();

	CDialog::OnDestroy();
}

bool CDlgSceneNodeList::IsListableNode( const rs3::RSceneNode *pSceneNode )
{
	CControlSceneProperty* pProperty = dynamic_cast<CControlSceneProperty*>((CCoreObject*)pSceneNode->GetUserData());
	CLogicObject* pLogicObject = dynamic_cast<CLogicObject*>((CCoreObject*)pSceneNode->GetUserData());
	bool bListableNode = pProperty || pLogicObject;
	return bListableNode;
}

void CDlgSceneNodeList::UpdateTreeView(HTREEITEM hTreeItem, const rs3::RSceneNode *pNode)
{
	using namespace rs3;

	const RSceneNodeList &rList = pNode->GetChildren();
	for ( RSceneNodeList::const_iterator it = rList.begin(); it != rList.end(); ++it )
	{
		RSceneNode *pNode = (*it);

		if( !IsListableNode(pNode) ) continue;

		const char *pNodeID = pNode->GetNodeIDString();
		const char* name = pNode->GetNodeName().c_str();

		// Tree에 SceneNode를 넣는다. 이름이 비어있으면, <NodeID> 를 적는다.
		HTREEITEM hCurrentItem = m_ctlObjectTree.InsertItem(name, hTreeItem);
		if ( pNode->GetNodeName().empty())
		{
			CString strNodeID;
			strNodeID.Format("<%s>", pNodeID);
			m_ctlObjectTree.SetItemText(hCurrentItem, strNodeID);
		}

		int nImage = GetImageIndex( pNode );
		m_ctlObjectTree.SetItemImage(hCurrentItem, nImage, nImage);

		// tree 에 pSceneNode 포인터를 넣는다.
		m_ctlObjectTree.SetItemData(hCurrentItem,(DWORD_PTR)pNode);
		
		// set view check
		m_ctlObjectTree.SetCheck( hCurrentItem, pNode->GetVisible()? 1 : 0 );

		// bool bSticky = pNode->QueryAttribute( RSNA_DO_NOT_SAVE );
		// TODO: 색깔을 회색으로 표시해주고 싶다

		UpdateTreeView(hCurrentItem, pNode);
	}
}

void CDlgSceneNodeList::UpdateTreeViewByClass(const rs3::RSceneNode *pNode)
{
	using namespace rs3;

	const RSceneNodeList &rList = pNode->GetChildren();
	for ( RSceneNodeList::const_iterator it = rList.begin(); it != rList.end(); ++it )
	{
		const RSceneNode *pChild = (*it);

		if( !IsListableNode(pChild) ) continue;

		// by pok. 트리뷰에서 안보이게 할부분 설정
		if( ( pChild->GetNodeName() != "DummyEnvLight0" ) && stricmp(pChild->GetNodeIDString(), rs3::RSID_EFFECT) != 0 )
		{
			if (m_nodesByClass[ pChild->GetNodeIDString() ].find(pChild->GetNodeName()) != m_nodesByClass[ pChild->GetNodeIDString() ].end())
			{
				std::string strPureName( pChild->GetNodeName().substr( 0, pChild->GetNodeName().length() - 4));
				std::string strFileName( strPureName + ".xml" );

				char szBuffer[256];
				szBuffer[0] = '\0';
				int nNumber = CXMLEditInfoFile::GetReference().GetNumbering(&strFileName);
				sprintf(szBuffer,"%s_%03d", strPureName.c_str(), nNumber);

				rs3::RToolSceneManager* pToolSceneManager = m_pControlMediator_->GetToolSceneManager();
				_ASSERT(pToolSceneManager);

				rs3::RSceneNode* pSameNameSceneNode =  pToolSceneManager->GetSceneNodeByName(szBuffer);
				while ( pSameNameSceneNode != NULL )
				{
					nNumber = CXMLEditInfoFile::GetReference().GetNumbering(&strFileName);
					sprintf(szBuffer,"%s_%03d", strPureName.c_str(), nNumber);
					pSameNameSceneNode = pToolSceneManager->GetSceneNodeByName(szBuffer);
				}
				// 값을 변경하기 위해 const 때내기
				const_cast< RSceneNode* >(pChild)->SetNodeName( szBuffer );
			}
			m_nodesByClass[ pChild->GetNodeIDString() ][ pChild->GetNodeName() ] = pChild;
		}

		UpdateTreeViewByClass(pChild);
	}
}

void CDlgSceneNodeList::UpdateTreeControl()
{
	// 트리 컨트롤을 업데이트 할때 열려있던 항목 유지하기
	std::set< const char* > setClassToExpend;
	for( SCENE_CLASS_HITEM::iterator itr = m_sceneClassHItem.begin(); itr != m_sceneClassHItem.end(); ++itr )
	{
		//열린목록
		if( itr->second.second )
			setClassToExpend.insert( itr->first );
	}

	// 트리컨트롤 위치
	int nVScrollPos = m_ctlObjectTree.GetScrollPos( SB_VERT );
	int nHScrollPos = m_ctlObjectTree.GetScrollPos( SB_HORZ );

	m_nodesByClass.clear();
	m_sceneClassHItem.clear();

	// lock window for tree control update
	// by pok, 깜빡임때문에 주석처리했습니다.
	//m_ctlObjectTree.LockWindowUpdate();

	// tree control reset
	m_ctlObjectTree.DeleteAllItems();

	if(!m_bHierachyView)
	{
		// 노드를 ID 별로 모은다
		UpdateTreeViewByClass( &m_pControlMediator_->GetToolSceneManager()->GetRootSceneNode() );

		// ID 별로 모은 노드를 트리뷰 컨트롤과 연결해 준다.
		for(NODEMAPBYCLASS::iterator i = m_nodesByClass.begin(); i!=m_nodesByClass.end(); i++)
		{
			MAP_NAME_SCENENODE::iterator itr_scene = i->second.begin();
			if( itr_scene == i->second.end() )
				continue;

			const char* currentNodeIDString = i->first;

			if( !IsListableNode(itr_scene->second) ) continue;

			HTREEITEM hCurrentClassItem = m_ctlObjectTree.InsertItem(  currentNodeIDString , TVI_ROOT);

			int nImage = GetImageIndex( itr_scene->second );

			m_ctlObjectTree.SetItemImage(hCurrentClassItem, nImage, nImage);
			m_ctlObjectTree.SetCheck( hCurrentClassItem, TRUE );

			// class hitem 모으기
			bool bExpend(false); // 열린항목을 찾으며 유지하면서 모으기
			std::set< const char* >::iterator itr = setClassToExpend.find( currentNodeIDString );
			if( itr != setClassToExpend.end() )
				bExpend = true;
			m_sceneClassHItem.insert( SCENE_CLASS_HITEM::value_type( currentNodeIDString, pair< HTREEITEM, bool>(hCurrentClassItem, bExpend)) );

			// 실제 노드 넣기
			for( ; itr_scene != i->second.end(); ++itr_scene )
			{
				const rs3::RSceneNode* pNode = itr_scene->second;
				CString strNodeName;
				if ( pNode->GetNodeName().empty())
					strNodeName.Format("<%s>", pNode->GetNodeIDString() );
				else
					strNodeName = pNode->GetNodeName().c_str();

				HTREEITEM hCurrentItem = m_ctlObjectTree.InsertItem(  strNodeName , hCurrentClassItem);
				m_ctlObjectTree.SetItemImage(hCurrentItem, nImage, nImage);

				// tree 에 pSceneNode 포인터를 넣는다.
				m_ctlObjectTree.SetItemData(hCurrentItem,(DWORD_PTR)pNode);

				// set view check
				m_ctlObjectTree.SetCheck( hCurrentItem, pNode->GetVisible() ? 1 : 0 );
			}
		}
	}
	else
	{
		UpdateTreeView(TVI_ROOT, &m_pControlMediator_->GetToolSceneManager()->GetRootSceneNode());
	}

	// unlock window for tree control update
	// by pok, 깜빡임때문에 주석처리 했습니다.
	//m_ctlObjectTree.UnlockWindowUpdate();

	// 예전에 열렸던 항목 다시 열어두기
	for( SCENE_CLASS_HITEM::iterator itr = m_sceneClassHItem.begin(); itr != m_sceneClassHItem.end(); ++itr )
	{
		//열린목록
		if( itr->second.second )
			m_ctlObjectTree.Expand( itr->second.first, TVE_EXPAND );
	}
	
	// 예전 위치 복구
	m_ctlObjectTree.SetScrollPos( SB_VERT, nVScrollPos );
	m_ctlObjectTree.SetScrollPos( SB_HORZ, nHScrollPos );
}

void CDlgSceneNodeList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_OBJECT, m_ctlObjectTree);

	// object property
	DDX_Check(pDX, IDC_CHECK_HIERACHY_VIEW, m_bHierachyView);
	DDX_Text(pDX, IDC_EDIT_OBJECT_SELECT_COUNT, m_nSelectedObjectCount);
}

BEGIN_MESSAGE_MAP(CDlgSceneNodeList, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OBJECT, &CDlgSceneNodeList::OnTvnSelchangedTreeObject)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_OBJECT, &CDlgSceneNodeList::OnNMDblclkTreeObject)
	ON_BN_CLICKED(IDC_CHECK_HIERACHY_VIEW, &CDlgSceneNodeList::OnBnClickedCheckHierachyView)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_OBJECT, &CDlgSceneNodeList::OnTvnItemexpandedTreeObject)
	ON_BN_CLICKED(IDC_BUTTON_EXPAND_TOGGLE, &CDlgSceneNodeList::OnBnClickedButtonExpandToggle)
	ON_EN_CHANGE(IDC_EDIT_OBJECT_SELECT_COUNT, &CDlgSceneNodeList::OnEnChangeEditObjectSelectCount)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_EFFECT, &CDlgSceneNodeList::OnBnClickedButtonPlayEffect)
END_MESSAGE_MAP()


// CEditObjectList 메시지 처리기입니다.

void CDlgSceneNodeList::Listen(CoreMessage& _message)
{
	switch( _message.nID )
	{
		case VIEW_MESSAGE::SCENE_LOADED :
		case VIEW_MESSAGE::SCENE_IMPORTED:
		{
			m_pControlMediator_->GetHelperAction()->RemoveAllSelection(true);
			UpdateTreeControl();
		}break;
		case VIEW_MESSAGE::LOGIC_LOADED :
			m_bShowList_ = false;
			ShowWindow(SW_HIDE);
			break;
		case VIEW_MESSAGE::ACTIVE_VIEW :
			if(m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
			{
				FooButton* pGroupBtn = m_pControlMediator_->GetViewPtr<CCoreToolToolBar>()->m_btnGroup;

				if(pGroupBtn[WORKBENCH_STATE::OBJECT_MOVE].isChecked())
					m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::OBJECT_MOVE);
				else if(pGroupBtn[WORKBENCH_STATE::OBJECT_NEW].isChecked())
					m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::OBJECT_NEW);
				else if(pGroupBtn[WORKBENCH_STATE::OBJECT_ROTATE].isChecked())
					m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::OBJECT_ROTATE);
				else if(pGroupBtn[WORKBENCH_STATE::OBJECT_SCALE].isChecked())
					m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::OBJECT_SCALE);
				else if(pGroupBtn[WORKBENCH_STATE::OBJECT_SELECT].isChecked())
					m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::OBJECT_SELECT);
			}
			break;
		case VIEW_MESSAGE::UNDO_OBJ_NEW_DEL:
		case VIEW_MESSAGE::SCENE_BREAK_INSTANCE:
		if(m_pControlMediator_->m_mapInfo.m_bLoaded && m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
		{
			m_pControlMediator_->GetHelperAction()->RemoveAllSelection(true);
			UpdateTreeControl();
		}
		break;

		case VIEW_MESSAGE::UNDO_MOVE:
		if(m_pControlMediator_->m_mapInfo.m_bLoaded && m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
		{
			m_pControlMediator_->GetHelperAction()->SetSelectedObjectCenterAndActionFrame();
			ObserveSelection(NULL, true, false);
		}
		break;

		case VIEW_MESSAGE::WB_MOUSE_L_UP:
		// 일단은 존일때만
		if (m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
			ObserveSelection(NULL, true, false);
		break;

		case VIEW_MESSAGE::LOGIC_SELECT:
			{
				CLogicObject_SceneNode* pLogicObjectSceneNode = dynamic_cast<CLogicObject_SceneNode*>( _message.pLogicObject );
				if( pLogicObjectSceneNode )
				{
					SetSelectNode_( TVI_ROOT, pLogicObjectSceneNode->GetModelSceneNode(), true );	// select
					UpdatePropertyUI();
					UpdateSelectCount_();
				}
			}break;
		case VIEW_MESSAGE::LOGIC_UNSELECT:
			{
				CLogicObject_SceneNode* pLogicObjectSceneNode = dynamic_cast<CLogicObject_SceneNode*>( _message.pLogicObject );
				if( pLogicObjectSceneNode )
				{
					SetSelectNode_( TVI_ROOT, pLogicObjectSceneNode->GetModelSceneNode(), false );	// unselect
					UpdatePropertyUI();
					UpdateSelectCount_();
				}
			}break;
// 		case VIEW_MESSAGE::LOGIC_SELECTION_CLEAR:
// 			m_setSelectedNodes.clear();
// 			ObserveSelection(NULL, true, true);
// 			break;
	}
}

void CDlgSceneNodeList::OnCancel()
{
	m_bShowList_ = false;
	ShowWindow(SW_HIDE);
}
void CDlgSceneNodeList::OnOK()
{
	m_bShowList_ = false;
	ShowWindow(SW_HIDE);
}

void CDlgSceneNodeList::ShowToggle()
{
	m_bShowList_ = !m_bShowList_;
	int nShowState = m_bShowList_ ? SW_SHOW : SW_HIDE;
	ShowWindow(nShowState);
}

HTREEITEM CDlgSceneNodeList::SetSelectNode_(HTREEITEM hTreeItem, rs3::RSceneNode *pSceneNode, bool bSelect )
{
	using namespace rs3;

	for ( HTREEITEM hChildItem = m_ctlObjectTree.GetChildItem(hTreeItem) ;
			hChildItem != NULL; hChildItem = m_ctlObjectTree.GetNextSiblingItem(hChildItem))
	{
		RSceneNode *pTreeSceneNode = (RSceneNode*)m_ctlObjectTree.GetItemData(hChildItem);
		if (pTreeSceneNode == pSceneNode )
		{
// 			if( bSelect )
// 				m_setSelectedNodes.insert( pSceneNode );
// 			else
// 				m_setSelectedNodes.erase( pSceneNode );

			// 이전상태와 같은경우는 UI 로 부터 선택이 시작된 경우이다. 생략해야 제대로 동작한다
			// 즉 OnTvnSelchangedTreeObject -> GetHelperAction::AddSelectection -> VIEW_MESSAGE::LOGIC_SELECT -> 여기
			bool bPreviousSelected = ( m_ctlObjectTree.GetItemState( hChildItem, TVIS_SELECTED ) & TVIS_SELECTED) != FALSE;
			if( bPreviousSelected != bSelect )
			{
				m_ctlObjectTree.SelectItemEx(hChildItem, bSelect ? TRUE : FALSE);

				// 가끔 unselect가 안되서 디버깅 해볼때.. 이거좀 병맛
//				CString strText = m_ctlObjectTree.GetItemText( hChildItem );
//				mlog( "  tree %s %d\n", (LPCSTR)strText, bSelect );
			}
			return hChildItem;
		}

		HTREEITEM hItem = SetSelectNode_(hChildItem, pSceneNode, bSelect);
		if ( hItem )
			return hItem;
	}
	return NULL;
}

void CDlgSceneNodeList::SetListLock(bool _bLock)
{
	m_ctlObjectTree.EnableWindow(_bLock ? TRUE : FALSE);
}

void CDlgSceneNodeList::ViewStateToggle(const char* _pSzObjectClassName)
{
	SCENE_CLASS_HITEM::iterator itr = m_sceneClassHItem.find( _pSzObjectClassName );
	if( itr != m_sceneClassHItem.end() )
		m_treeCheckBoxObserver.ObserveCheckBox( itr->second.first );
}

bool CDlgSceneNodeList::GetViewState(const char* _pSzObjectClassName)
{
	BOOL bCheck(FALSE);
	SCENE_CLASS_HITEM::iterator itr = m_sceneClassHItem.find( _pSzObjectClassName );
	if( itr != m_sceneClassHItem.end() )
		bCheck = m_ctlObjectTree.GetCheck( itr->second.first );

	return (bCheck == TRUE );
}

void CDlgSceneNodeList::RecheckAllSelection_(SCENEOBJECTMAP *pSelectionList)
{
	m_ctlObjectTree.ClearSelection();
	m_ctlObjectTree.SelectItem(NULL);

	for (SCENEOBJECTMAP::iterator itr = pSelectionList->begin(); itr != pSelectionList->end(); ++itr)
	{
		HTREEITEM hItem = SetSelectNode_(TVI_ROOT, itr->first);
		itr->second = (void*)hItem;
	}
}

void CDlgSceneNodeList::UpdateSelectCount_()
{
	LOGIC_OBJECTLIST *pLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();

	m_nSelectedObjectCount = 0;
	if ( pLogicList )
		m_nSelectedObjectCount = static_cast<int>(pLogicList->size());

//	m_nSelectedObjectCount = m_setSelectedNodes.size();

	UpdateData(FALSE);
}

void CDlgSceneNodeList::RemoveSelection(HTREEITEM hItem)
{
	m_ctlObjectTree.SetItemState(hItem, 0, TVIS_SELECTED);

	m_ctlObjProperty_.SetSelectedSceneNode(NULL);
	UpdateSelectCount_();
}

void CDlgSceneNodeList::UpdatePropertyUI()
{
	LOGIC_OBJECTLIST *pLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();

	if(pLogicList->empty())
	{
		m_pPropCtrl->ShowWindow(SW_HIDE);
		m_ctlObjProperty_.SetSelectedSceneNode(NULL);
		return;
	}


	// property control
	std::list<CBaseObject*> pReflectList;
	int nSelectedCnt = 0;
	for(LOGIC_OBJECTLIST::iterator itr = pLogicList->begin(); itr!=pLogicList->end(); ++itr)
	{
		//---------------------------------------------
		//Get SceneNode
		rs3::RSceneNode* pSceneNode = NULL;
		CLogicObject* pLogicObject = *itr;
		CLogicObject_SceneNode* pLogicObjectSceneNode = dynamic_cast<CLogicObject_SceneNode*>( pLogicObject );
		if(pLogicObjectSceneNode)
			pSceneNode = pLogicObjectSceneNode->GetModelSceneNode();
		//---------------------------------------------

		if( pLogicObject->GetEditable() == false ) continue;

		if( pSceneNode != NULL )
		{
//			CControlSceneProperty* pProperty = pLogicObjectSceneNode->GetProperty();
			CBaseObject* pReflectProperty =pLogicObjectSceneNode->GetReflectProperty();;


			if(pReflectProperty)
				pReflectList.push_back(pReflectProperty);
			else
			{
				++nSelectedCnt;
				if (nSelectedCnt == 1)
				{
					m_pPropCtrl->ShowWindow(SW_HIDE);
					m_ctlObjProperty_.SetSelectedSceneNode(pSceneNode);
				}
				else if (nSelectedCnt == 2)
				{
					// 다중선택 방지
					m_ctlObjProperty_.SetSelectedSceneNode(NULL);
				}
			}
		}
	}


	if(!pReflectList.empty())
	{
		m_pPropCtrl->RemoveAll();			

		m_UI.BuildUIForObjects(m_pPropCtrl, pReflectList);

		m_pPropCtrl->ShowWindow(SW_SHOW);
		m_pPropCtrl->ExpandAll(true);
	}
	
	// m_ctlObjProperty_.SetSelectedSceneNode 를 해주면 포커스를 잃는다.
	::SetFocus(m_pControlMediator_->GetWBWindowHandle());
}


void CDlgSceneNodeList::ObserveSelection(rs3::RSceneNode *pNewSelectionSceneNode, bool _bUpdateUI, bool _bSelectionClear, bool _bReCheckAllSelected)
{
	SCENEOBJECTMAP *pSceneList = m_pControlMediator_->GetHelperAction()->GetSelectedSceneObjectPtr();

	if( _bUpdateUI )
	{
		// tree control
		if( pNewSelectionSceneNode )
		{
			// 선택
			if (_bReCheckAllSelected)
			{
				RecheckAllSelection_(pSceneList);
			}
			else
			{
				HTREEITEM hItem = SetSelectNode_(TVI_ROOT, pNewSelectionSceneNode);
				(*pSceneList)[pNewSelectionSceneNode] = hItem;
			}

			//adjust HScroll
			//m_ctlObjectTree.SetScrollPos( SB_HORZ, 58 );
		}

		UpdatePropertyUI();
	}

	if( pNewSelectionSceneNode == NULL && _bSelectionClear)
	{
		m_ctlObjectTree.ClearSelection();
		m_ctlObjectTree.SelectItem(NULL);
	}

	UpdateSelectCount_();
}

void CDlgSceneNodeList::OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if (pNMTreeView->action != 0)
		return;

	if (m_ctlObjectTree.GetClickedItem() == NULL)
		return;
	
	// UI 변경없이 선택 초기화
	m_pControlMediator_->GetHelperAction()->RemoveAllSelection(false);
// 	m_setSelectedNodes.clear();

	HTREEITEM hSelectedItem = m_ctlObjectTree.GetFirstSelectedItem();
	if (hSelectedItem == NULL)
		return;

	// UI 변경은 클릭을 통해 해졌으므로 UI변경없이 선택
// 	SCENEOBJECTMAP *pSceneList = m_pControlMediator_->GetHelperAction()->GetSelectedSceneObjectPtr();

	for ( ; hSelectedItem != NULL; hSelectedItem = m_ctlObjectTree.GetNextSelectedItem(hSelectedItem))
	{
		rs3::RSceneNode *pSceneNode = (rs3::RSceneNode*)m_ctlObjectTree.GetItemData(hSelectedItem);
// 		m_setSelectedNodes.insert( pSceneNode );
		if ( pSceneNode )
		{
			// TODO: 신노드 편집기능 걷어내고 나면 없애자
			CLogicObject_SceneNode* pLogicObjectSceneNode = dynamic_cast<CLogicObject_SceneNode*>( (CCoreObject*)pSceneNode->GetUserData() );
			if(pLogicObjectSceneNode)
			{
				// TODO. by pok. 2010. 11. 19.
				// 여러번 UI가 업데이트 되는 문제.
				// 여기를 통해서 VIEW_MESSAGE::LOGIC_SELECT 메시지가 발송되고 그에 따라 중복 UI 업데이트가 이루어진다.
				m_pControlMediator_->GetHelperAction()->AddSelectection( pLogicObjectSceneNode, false);
			}
//			else
//				m_pControlMediator_->GetHelperAction()->AddSelectection(pSceneNode, NULL, false);
// 			(*pSceneList)[pSceneNode] = hSelectedItem;
		}
	}

	// selection을 한 상태이므로, 더이상 새로운 셀렉션은 없고 UI 업데이트를 통해 Property UI 업데이트
	ObserveSelection( NULL, true, false );
}

void CDlgSceneNodeList::OnNMDblclkTreeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	rs3::RToolCamera* pCamear = m_pControlMediator_->GetToolCamera();
	if( pCamear == NULL)
		return;

	if( m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject()->empty() )
		return;

	CLogicObject* pLogicObject = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject()->front();

	// 일반적인 물체 리스트 더블클릭이면 그 물체를 화면중심으로 옮긴다.
	bool bGoToObjectCenter(true);

	CLogicObject_SceneNode* pLogicObjectSceneNode = dynamic_cast<CLogicObject_SceneNode*> ( pLogicObject );
	if( pLogicObjectSceneNode )
	{
		rs3::RSceneNode* pSceneNode = pLogicObjectSceneNode->GetModelSceneNode();
		if( _stricmp( pSceneNode->GetNodeIDString(), rs3::RSID_WATERPLANE ) == 0 ) // 물 평면
		{
			_ASSERT( m_pEditWater_ );
			m_pEditWater_->ResetWaterPlane( (rs3::RWaterPlane*)(pSceneNode) );
			m_pEditWater_->ShowWindow(SW_SHOW);
			bGoToObjectCenter = false;
		}
	}

	rs3::RVector vecPos;
	if( pLogicObject->GetPosition( vecPos ) )
	if( bGoToObjectCenter )
		pCamear->SetPositionToViewCenter( vecPos, Tool3Const::CONST_WB_STATIC_OBJ_TO_CENTER );

	*pResult = 0;
}

void CDlgSceneNodeList::OnBnClickedCheckHierachyView()
{
	UpdateData(TRUE);
	
	m_pControlMediator_->GetHelperAction()->RemoveAllSelection(true);
	UpdateTreeControl();
}

void CDlgSceneNodeList::OnTvnItemexpandedTreeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	TVITEM  item;
	item.mask = TVIF_HANDLE;
	item.hItem = pNMTreeView->itemNew.hItem;
	m_ctlObjectTree.GetItem(&item);            // 아이템 정보를 알아낸다.

	for( SCENE_CLASS_HITEM::iterator itr = m_sceneClassHItem.begin(); itr != m_sceneClassHItem.end(); ++itr )
	{
		if( itr->second.first == item.hItem )
		{
			if(item.state & TVIS_EXPANDED)
				itr->second.second = true;
			else
				itr->second.second = false;
		}
	}

	*pResult = 0;
}

void CDlgSceneNodeList::ExpandTreeControlToggle()
{
	// 모든 트리컨트롤이 닫혀있다면 열어주고 아니면 닫아준다
	bool bAllTreeItemClosed(true);
	for( SCENE_CLASS_HITEM::iterator itr = m_sceneClassHItem.begin(); itr != m_sceneClassHItem.end(); ++itr )
	{
		if( itr->second.second )
			bAllTreeItemClosed = false;
	}

	for( SCENE_CLASS_HITEM::iterator itr = m_sceneClassHItem.begin(); itr != m_sceneClassHItem.end(); ++itr )
	{
		itr->second.second = bAllTreeItemClosed; // 모두 닫힌게 사실이라면 다음에 열릴것이 사실
		m_ctlObjectTree.Expand( itr->second.first, bAllTreeItemClosed ? TVE_EXPAND : TVE_COLLAPSE);
	}
}

bool CDlgSceneNodeList::HasActiveSceneEditor()
{
	if ( m_pEditWater_->IsActive() )
	{
		return true;
	}
	else
	{
		if ( m_ctlObjProperty_.IsFocused() && GetFocus() == &m_ctlObjProperty_ )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void CDlgSceneNodeList::OnBnClickedButtonExpandToggle()
{
	ExpandTreeControlToggle();
}

void CDlgSceneNodeList::ShowWaterEditor()
{
	struct WaterSelector
	{
		static bool Selector( rs3::RSceneNode* pSceneNode)
		{
			if ( _stricmp(pSceneNode->GetNodeIDString(), rs3::RSID_WATERPLANE) == 0 )
					return true;
			else
				return false;
		}
	};
	std::vector< rs3::RSceneNode* > vecWater;
	m_pControlMediator_->GetToolSceneManager()->GetRootSceneNode().SerializeToVector(WaterSelector::Selector, vecWater);
	if (vecWater.empty() == false)
	{
		_ASSERT( m_pEditWater_ );
		m_pEditWater_->ResetWaterPlane( (rs3::RWaterPlane*)(vecWater[0]) );
		m_pEditWater_->ShowWindow(SW_SHOW);
	}
}

int CDlgSceneNodeList::GetImageIndex( const rs3::RSceneNode* pNode )
{
	CLogicObject* pLogicObject = m_pControlMediator_->GetToolSceneManager()->GetLogicObjectLinkedTo( pNode );
	int nImage = pLogicObject ? pLogicObject->GetIconIndex() : 0;

	return nImage;
}

void CDlgSceneNodeList::OnEnChangeEditObjectSelectCount()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// __super::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgSceneNodeList::OnBnClickedButtonPlayEffect()
{
	LOGIC_OBJECTLIST *pLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();
	for(LOGIC_OBJECTLIST::iterator itr = pLogicList->begin(); itr!=pLogicList->end(); ++itr)
	{
		//---------------------------------------------
		//Get SceneNode
		rs3::RSceneNode* pSceneNode = NULL;
		CLogicObject* pLogicObject = *itr;
		CLogicObject_SceneNode* pLogicObjectSceneNode = dynamic_cast<CLogicObject_SceneNode*>( pLogicObject );
		if(pLogicObjectSceneNode)
			pSceneNode = pLogicObjectSceneNode->GetModelSceneNode();
		if(pSceneNode)
		{
			CEffectStarter effectStarter;
			pSceneNode->Traverse( &effectStarter );
		}
	}
}
