#include "stdafx.h"
#include "McvView.h"
#include "McvConfig.h"
#include "MXml.h"
#include "MFileSystem.h"
#include "EditAnimationDlg.h"
#include "RConfiguration.h"
//#include "RMeshManager.h"
#include "RUtil.h"
#include "RAnimationBlendTable.h"
#include "RAnimationResource.h"
#include "AddAnimationDlg.h"
#include "resource.h"

#include "REffectSceneNode.h"	// 090602,OZ

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib,"winmm.lib")

using namespace rs3;

///////////////////////////////////////////////////////////////

//CMcvView* g_pView = NULL;

CMcvView* GetMainView() { return (CMcvView*)GetMainFrame()->GetActiveView(); }

CMcvMain* GetMain() { 
	if(GetMainApp())
		return GetMainApp()->GetMain(); 
	return NULL;
}

RDevice*  GetDevice()
{
	if(GetMain())
		return GetMain()->GetDevice();
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CMcvView

IMPLEMENT_DYNCREATE(CMcvView, CFormView)

BEGIN_MESSAGE_MAP(CMcvView, CFormView)
	//{{AFX_MSG_MAP(CMcvView)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_DROPFILES()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_TEX_ON_OFF, OnTexOnOff)
	ON_COMMAND(ID_VERTEX_NORMAL, OnVertexNormal)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_NORMAL, OnUpdateVertexNormal)
	ON_UPDATE_COMMAND_UI(ID_TEX_ON_OFF, OnUpdateTexOnOff)
//	ON_CBN_SELCHANGE(IDC_PARTS_TYPE, OnCbnSelchangePartsType)
//	ON_LBN_SELCHANGE(IDC_PARTS_LIST, OnLbnSelchangePartsList)
//	ON_LBN_SELCHANGE(IDC_WEAPON_LIST, OnLbnSelchangeWeaponList)
//	ON_LBN_SELCHANGE(IDC_ANIMATION_LIST, OnLbnSelchangeAnimationList)
	ON_BN_CLICKED(IDC_PLAY_PAUSE, OnBnClickedPlayPause)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
//	ON_COMMAND(ID_FILE_WOPEN, OnFileWopen)
	ON_COMMAND(ID_CAMERA_RESET, OnCameraReset)
//	ON_CBN_SELCHANGE(IDC_WEAPON_TYPE, OnCbnSelchangeWeaponType)
//	ON_CBN_SELCHANGE(IDC_WEAPON_TYPE2, OnCbnSelchangeWeaponType2)
	ON_NOTIFY(NM_THEMECHANGED, IDC_SLIDER_SPEED, OnNMThemeChangedSliderSpeed)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SPEED, OnNMReleasedcaptureSliderSpeed)
	ON_COMMAND(ID_SCALEDLG, OnScaledlg)
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_MAP_OBJ_POS, OnMapObjPos)
	ON_COMMAND(ID_GRID_ONOFF, OnGridOnoff)
	ON_COMMAND(ID_BBOX_ONOFF, OnBboxOnoff)
	ON_UPDATE_COMMAND_UI(ID_BBOX_ONOFF, OnUpdateBboxOnoff)
	ON_UPDATE_COMMAND_UI(ID_GRID_ONOFF, OnUpdateGridOnoff)
	ON_COMMAND(ID_EFFECT_RENDER, OnEffectRender)
	ON_UPDATE_COMMAND_UI(ID_EFFECT_RENDER, OnUpdateEffectRender)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_CHAR_VIEW_MODE, OnCharViewMode)
	ON_UPDATE_COMMAND_UI(ID_CHAR_VIEW_MODE, OnUpdateCharViewMode)
//	ON_COMMAND(ID_PARTSCOLOR, OnPartscolor)
//	ON_COMMAND(ID_MTRLEDITDLG, OnMtrleditdlg)
//	ON_COMMAND(ID_PARTSCOLORALL, OnPartscolorall)
//	ON_COMMAND(ID_ANIMATIONINFO, OnAnimationinfo)
//	ON_COMMAND(ID_BLEND_COLOR, OnBlendColor)
	ON_WM_PAINT()
//	ON_COMMAND(ID_CARTOON_LIGHT_ONOFF, OnCartoonLightOnoff)
//	ON_UPDATE_COMMAND_UI(ID_CARTOON_LIGHT_ONOFF, OnUpdateCartoonLightOnoff)
//	ON_COMMAND(ID_MODELINFO, OnModelinfo)
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
//	ON_CBN_SELCHANGE(IDC_COMBO_NODES, OnCbnSelchangeComboNodes)
//	ON_BN_CLICKED(IDC_BUTTON_REPLACE_MESH, OnBnClickedButtonReplaceMesh)
//	ON_LBN_DBLCLK(IDC_LIST_EVENT, OnLbnDblclkListEvent)
//	ON_BN_CLICKED(IDC_BUTTON_ADD_EVENT, OnBnClickedButtonAddEvent)
//	ON_BN_CLICKED(IDC_BUTTON_ADD_HANDLER, OnBnClickedButtonAddHandler)
//	ON_LBN_DBLCLK(IDC_LIST_HANDLER, OnLbnDblclkListHandler)
	ON_COMMAND(ID_SHOW_BONES, OnShowBones)
	ON_UPDATE_COMMAND_UI(ID_SHOW_BONES, OnUpdateShowBones)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
//	ON_LBN_DBLCLK(IDC_ANIMATION_LIST, OnLbnDblclkAnimationList)
//	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NODES, &CMcvView::OnTvnSelchangedTreeNodes)
// 	ON_BN_CLICKED(IDC_CHECK_NODE_MESH, &CMcvView::OnBnClickedCheckNode)
// 	ON_BN_CLICKED(IDC_CHECK_NODE_BONE, &CMcvView::OnBnClickedCheckNode)
// 	ON_BN_CLICKED(IDC_CHECK_NODE_DUMMY, &CMcvView::OnBnClickedCheckNode)
	ON_COMMAND(ID_SHOW_COLLISION, &CMcvView::OnShowCollision)
	ON_UPDATE_COMMAND_UI(ID_SHOW_COLLISION, &CMcvView::OnUpdateShowCollision)
	ON_COMMAND(ID_HDR_ONOFF, &CMcvView::OnHDROnOff)
	ON_UPDATE_COMMAND_UI(ID_HDR_ONOFF, &CMcvView::OnUpdateHDROnOff)
	ON_WM_SIZE()

	ON_COMMAND(ID_ANIMATION_BLEND, &CMcvView::OnAnimationBlend)
	ON_COMMAND(ID_ANIBLEND_IMMEDIATE, &CMcvView::OnAniblendImmediate)
	ON_COMMAND(ID_ANIBLEND_DELAYED, &CMcvView::OnAniblendDelayed)
	ON_COMMAND(ID_ANIBLEND_CROSSFADE, &CMcvView::OnAniblendCrossfade)
	ON_COMMAND(ID_ANIBLEND_DELETE, &CMcvView::OnAniblendDelete)
	ON_COMMAND(ID_ANIBLEND_EDIT_DURATION, &CMcvView::OnAniblendEditDuration)
	ON_COMMAND(ID_ANIBLEND_IMMEDIATE_TARGET_ALL, &CMcvView::OnAniblendImmediateTargetAll)
	ON_COMMAND(ID_ANIBLEND_DELAYED_TARGET_ALL, &CMcvView::OnAniblendDelayedTargetAll)
	ON_COMMAND(ID_ANIBLEND_CROSSFADE_TARGET_ALL, &CMcvView::OnAniblendCrossfadeTargetAll)
	ON_COMMAND(ID_ANIBLEND_DELETE_TARGET_ALL, &CMcvView::OnAniblendDeleteTargetAll)
	ON_COMMAND(ID_ALPHA2PASS, &CMcvView::OnAlpha2pass)
	ON_UPDATE_COMMAND_UI(ID_ALPHA2PASS, &CMcvView::OnUpdateAlpha2pass)
	ON_COMMAND(ID_MATERIALDECALDLG, &CMcvView::OnMaterialdecaldlg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMcvView construction/destruction

CMcvView::CMcvView() : CFormView(CMcvView::IDD)
{
//	g_pView = this;

	//{{AFX_DATA_INIT(CMcvView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

//	m_nSelectPartsType		= 0;
//	m_nSelectWeaponType		= 0;
//	m_nSelectWeaponType2	= 0;

	// option

	m_bTextureRender	= false;
	m_bVertexNormal		= false;
	m_bDrawGrid			= true;
	m_bDrawBBox			= true;
	m_bEffectModel		= false;
	m_bCharacterView	= true;
	m_bAlpha2Pass		= false;

	// dlg

	m_bScaleDlg			= false;
	m_bMtrlEditDlg		= false;
	m_bFrameInfoDlg		= false;
	m_bModelInfoDlg		= false;

	// Animation

	m_nPrevFrame		= -1;

	m_pCurrentSceneNode = 0;
}

CMcvView::~CMcvView()
{
}

void CMcvView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMcvView)
//	DDX_Control(pDX, IDC_ANIMATION_LIST, m_AnimationListBox);
	//DDX_Control(pDX, IDC_SLIDER1, m_SliderAniSpeed);
	//DDX_Text(pDX, IDC_SELMESHNAME, m_SelMeshNodeName);

	//DDX_Control(pDX, IDC_TREE_NODES, m_treeNodes);
	//DDX_Check(pDX, IDC_CHECK_NODE_MESH, m_bNodeMesh);
	//DDX_Check(pDX, IDC_CHECK_NODE_BONE, m_bNodeBone);
	//DDX_Check(pDX, IDC_CHECK_NODE_DUMMY, m_bNodeDummy);
	//DDX_Text(pDX, IDC_EDIT_NODE_INFO, m_strNodeInfo);
//	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
	//DDX_Control(pDX, IDC_STATIC_INFO, m_strFrame);
	//DDX_Control(pDX, IDC_CHECK_NODE_ALIGN_TEST, m_btnAlignTest);

	//}}AFX_DATA_MAP
}

BOOL CMcvView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////

void CMcvView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);

	Init();

	m_ScaleDlg.Create(IDD_SCALE,this);
	m_MtrlDlg.Create(IDD_MTRLEDITDLG,this);
	m_ModelInfoDlg.Create(IDD_MODEL_INFO,this);
	m_FrameInfoDlg.Create(IDD_FRAME_INFO,this);
	m_AniBlendDlg.Create(IDD_DIALOG_EDIT_ANIBLEND,this);
	m_MaterialDecalDlg.Create(IDD_MATERIALDECAL,this);

//	g_pDefaultLogger->SetCustomOutputCallback(McvLog);
}

/////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

void CMcvView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMcvView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CMcvDoc* CMcvView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMcvDoc)));
	return (CMcvDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMcvView message handlers

bool CMcvView::Init()
{
	///////////////////////////////////////
	// Parts
/*
	m_NodesCombo.AddString("eq_head");
	m_NodesCombo.AddString("eq_face");
	m_NodesCombo.AddString("eq_chest");
	m_NodesCombo.AddString("eq_hands");
	m_NodesCombo.AddString("eq_legs");
	m_NodesCombo.AddString("eq_feet");

	m_NodesCombo.SetCurSel(0);
	*/

	///////////////////////////////////////
	// Weapon Type 

	// 무기목록은 한손 양손 + 종류

//	m_WeaponTypeCombo.AddString("eq_single_sword");
//	m_WeaponTypeCombo.AddString("eq_single_gun");

//	m_WeaponTypeCombo.AddString("eq_double_sword");
//	m_WeaponTypeCombo.AddString("eq_double_gun");

//	m_WeaponTypeCombo.SetCurSel(0);

	// 장비 위치는 왼손 오른손 + 종류 

//	m_WeaponType2Combo.AddString("eq_left_sword");
//	m_WeaponType2Combo.AddString("eq_left_gun");
//	m_WeaponType2Combo.AddString("eq_right_sword");
//	m_WeaponType2Combo.AddString("eq_right_gun");

//	m_WeaponType2Combo.SetCurSel(0);

//	string	listname = config.GetEffectPath("EffectList.xml");
//	REffectMgr::GetInstance()->LoadEffectList(listname.c_str());

//	GetActor()->SetListener(&m_eventListener);

	return true;
}

//////////////////////////////////////////////////////////////////


void CMcvView::ChangeAnimationInfoString()
{
	RActor* pActor = GetActor();
	if(!pActor) return;

	RAnimation* pAniSet = pActor->GetAnimation();
	RAnimationController& animationController = pActor->GetAnimationController();

	if( pAniSet )
	{
		CString str;
		str.Format( "%d / %d frame", animationController.GetFrame(), animationController.GetMaxFrame());
		GetFrameControlBar()->m_strFrame = str;

		str.Format( "%.2f / %.2f sec", (float)animationController.GetFrame() / 4800.0f, (float)animationController.GetMaxFrame() / 4800.0f);
		GetFrameControlBar()->m_strSecond = str;

		GetFrameControlBar()->UpdateData(FALSE);
	}
}


void CMcvView::UpdateAnimationSlider()
{
	RActor* pActor = GetActor();
	if(!pActor) return;
	const RAnimationController& animationController = pActor->GetAnimationController();

	// 애니메이션 슬라이드 바
	GetFrameControlBar()->m_SliderAnimation.SetRange(0,animationController.GetMaxFrame());
	GetFrameControlBar()->m_SliderAnimation.SetPos(animationController.GetFrame());


	HTREEITEM hItem = GetAnimationSidebar()->m_cAnimationTreeCtrl.GetSelectedItem();
	if ( hItem == NULL)
		return;

	int nAniSel = GetAnimationSidebar()->m_cAnimationTreeCtrl.GetItemData( hItem);

	if ( nAniSel >= 0  &&  (int)animationController.GetFrame() > m_nPrevFrame)
	{
		RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
		if ( pAni)
		{
			for ( size_t i = 0;  i < (int)pAni->m_animationEvents.size();  i++)
			{
				string strEvent = m_ProjectType.GetPrefix();
				strEvent += "sound";
				if ( pAni->m_animationEvents[ i]->m_strEvent != strEvent)
					continue;


				int nFrame = (int)pAni->m_animationEvents[ i]->m_nFrame;

				if ( nFrame > m_nPrevFrame  &&  nFrame <= (int)animationController.GetFrame())
				{
					map< string, string>::iterator itr = m_SoundList.find( pAni->m_animationEvents[ i]->m_strParam1);

					if ( itr != m_SoundList.end())
					{
						string strFileName = m_ProjectType.GetPathSound();
						strFileName += "/";
						strFileName += (*itr).second;

						sndPlaySound( strFileName.c_str(), SND_ASYNC);
					}
				}
			}
		}
	}

	m_nPrevFrame = (int)animationController.GetFrame();
}

////////////////////////////////////////////////////////////////////////////////////

void CMcvView::Idle()
{
	if(GetMain()) {
		GetMain()->Update();
		GetMain()->Render();

// 		if(GetMain()->m_pSelectedActorNode)
// 		{
// 			if(GetControlToolBar()->m_btnAlignTest.GetCheck())
// 			{
// 				GetMain()->m_pSelectedActorNode->AddController(&m_testController);
// 				m_testController.SetTargetPosition(GetMain()->m_vLightPos);
// 			}else
// 			{
// 				GetMain()->m_pSelectedActorNode->RemoveController(&m_testController);
// 			}
// 		}



		RAnimationController& animationController = GetActor()->GetAnimationController();
		if(!animationController.GetAnimation()) return;

		if ( animationController.GetFrame() >= (animationController.GetMaxFrame() - 1))
		{
			if ( animationController.GetLoop() == false)
				PauseAnimation();
		}

		UpdateAnimationSlider();
		ChangeAnimationInfoString();
	}
}

/////////////////////////////////////////////////////////////////////////////////////

void CMcvView::OnDestroy() 
{
	CFormView::OnDestroy();
}

bool CMcvView::IsCursorInView()
{
	POINT pt;
	GetCursorPos(&pt);

//	CWnd* hWndView = GetDlgItem(IDC_PIC);
	RECT rt;
	GetWindowRect(&rt);
	if(rt.left<=pt.x && pt.x<=rt.right &&
		rt.top<=pt.y && pt.y<=rt.bottom)
		return true;

	return false;
}

#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

BOOL CMcvView::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN ) {
		if(pMsg->wParam == VK_DELETE)
		{
			CWnd* pWnd = GetFocus();
//			if(pWnd == &GetAnimationSidebar()->m_cAnimationListBox)
//				DeleteSelectedAnimation();

			if(GetMain()->m_pSelectedActorNode)
			{
				GetActor()->DeleteActorNode(GetMain()->m_pSelectedActorNode->GetNodeName().c_str());
				GetMain()->m_pSelectedActorNode = NULL;
				GetModelInfoSidebar()->m_treeNodes.DeleteAllItems();
				UpdateNodes();
				//UpdateData();
			}

			/*
			else if(pWnd == &m_lbEvents)
				DeleteSelectedEvent();
			else if(pWnd == &m_lbEventHandler)
				DeleteSelectedEventHandler();
			*/
		}

		// 나머지 키입력 무시
		return 0;
	}

	if( pMsg->message == WM_MOUSEWHEEL ) {

		if(IsCursorInView())
		{
			CTCamera* pCamera = &GetMain()->m_Camera;
			float fStep = pCamera->m_fDist * 0.002f;
			short zDelta = (short)HIWORD(pMsg->wParam);
			if(GetMain())
				GetMain()->m_fWheel += fStep * (float)zDelta * (IsKeyDown(VK_SHIFT) ? 5 : 1);
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}


void CMcvView::OnClose() 
{
	CFormView::OnClose();

	PostQuitMessage(0);
}

int CMcvView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_controlToolBar.Create(CControlToolBar::IDD,this);
	//m_controlToolBar.ShowWindow(SW_SHOW);
	return 0;
}

BOOL CMcvView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

////////////////////////////////////////////////////////////////////////////////
//
// + 에니메이션 상태별 프레임 기술파일 로딩과 관리
// + 장착할 모델 로딩 + 관리

/*
	에니상태파일은
	라인읽기 : 첫라인은 

	ani_filename = ;
	{
		idle begin_frame end_frame
		walk begin_frame end_frame
	}
*/

void CMcvView::UpdateNodes()
{
	RActor* pActor = GetActor();
	/*
	for(int i=0;i<pActor->GetActorNodeCount();i++)
	{
		RActorNode *pActorNode = pActor->GetActorNode(i);
		m_NodesCombo.AddString(pActorNode->m_pMeshNode->m_Name.c_str());
	}
	*/

	GetModelInfoSidebar()->m_treeNodes.DeleteAllItems();
	GetModelInfoSidebar()->m_treeNodes.SetIndent(0);
	vector<HTREEITEM> treeItem;
	for(int i=0;i<pActor->GetActorNodeCount();i++)
	{
		RActorNode *pActorNode = pActor->GetActorNodes()[i];
		RMeshNode *pMeshNode = pActorNode->m_pMeshNode;

		bool isBoneNode;
		if(pActor->m_pMesh->GetFileVersion() >= EXPORTER_MESH_VER10)
			isBoneNode = pActorNode->m_pMeshNode->GetFlag(RM_FLAG_BONE_MESH);
		else
		{
			isBoneNode = (
				(_strnicmp(pMeshNode->GetName(),"BIP",3)==0 ||
					_strnicmp(pMeshNode->GetName(),"BONE",3)==0 )
				&& pMeshNode->isDummyMesh());
		}

		if((!GetModelInfoSidebar()->m_bNodeMesh && pMeshNode->isRenderAble()) ||
			(!GetModelInfoSidebar()->m_bNodeDummy && pMeshNode->isDummyMesh()&& !isBoneNode) ||
//			(!m_bNodeBone && pMeshNode->GetBipId()!=RBIPID_UNKNOWN) ||
			(!GetModelInfoSidebar()->m_bNodeBone && isBoneNode)	// 나머지는 다 뼈로 본다 ?
			)
		{
			treeItem.push_back(TVI_ROOT);
			continue;
		}

		HTREEITEM hParent = TVI_ROOT;
		if(pActorNode->GetParentActorNode() != NULL)
		{
			RActorNode* pParentActorNode = pActorNode->GetParentActorNode();
			int nParentID = pParentActorNode ->m_pMeshNode->m_nID;
			if (nParentID < (int)treeItem.size())
				hParent = treeItem[nParentID];
		}

		HTREEITEM hItem = GetModelInfoSidebar()->m_treeNodes.InsertItem(pActorNode->m_pMeshNode->m_Name.c_str(),hParent );
		GetModelInfoSidebar()->m_treeNodes.SetItemData(hItem,i);	// 인덱스를 아이템 데이터로 넣는다
		treeItem.push_back(hItem);
//		m_NodesCombo.AddString(pActorNode->m_pMeshNode->m_Name.c_str());
	}


	/*
	RMeshPartsType eq_parts = (RMeshPartsType)((int)eq_parts_hair + m_nSelectPartsType);

	int cnt = m_PartsListbox.GetCount();

	// clear listbox

	for(int i=0;i<cnt;i++) {
		m_PartsListbox.DeleteString(0);
	}

	RActor* pActor = GetActor();

	RMeshNode* pMNode;

	if(pActor->m_pMesh) {

		int nodesize = pActor->m_pMesh->m_nDataNum;

		for(int k=0;k<nodesize;k++) {

			pMNode = pActor->m_pMesh->m_NodeTable[k];

			if(pMNode->m_PartsType == eq_parts)
				m_PartsListbox.AddString(pMNode->GetName());
		}
	}

	if(pActor && pActor->m_pMesh->m_pPartsMgr) {
	
		int nNodeCnt = pActor->m_pMesh->m_pPartsMgr->m_NodeList.size();

		if(	nNodeCnt ) {

			RMeshMgr* pMeshMgr = pActor->m_pMesh->m_pPartsMgr;

			RMesh* pMesh = NULL;

			for(int j=0;j<nNodeCnt;j++) {

				pMesh = pMeshMgr->GetFast(j);

				int nodesize = pMesh->m_nDataNum;

				for(int k=0;k<nodesize;k++) {

					pMNode = pMesh->m_NodeTable[k];

					if(pMNode->m_PartsType == eq_parts)
						m_PartsListbox.AddString(pMNode->GetName());
				}
			}
		}
	}
	*/
}

void CMcvView::ChangeWeaponListBox()
{
	// 한손 무기와 양손무기의 종류로 나눈다.
/*
	RWeaponType eq_weapon = (RWeaponType)(m_nSelectWeaponType + 1);

	int cnt = m_WeaponListBox.GetCount();

	// clear listbox

	for(int i=0;i<cnt;i++) {
		m_WeaponListBox.DeleteString(0);
	}

	m_WeaponListBox.AddString("base_parts");

	int size = m_WeaponMeshMgr.m_list.size();

	if(	size ) {

		RMeshMgr* pMeshMgr = &m_WeaponMeshMgr;
		RMesh* pMesh = NULL;

		for(int j=0;j<size;j++) {

			pMesh = pMeshMgr->GetFast(j);

			int nodesize = pMesh->m_nDataNum;

			for(int k=0;k<nodesize;k++) {

				RMeshNode* pMNode = pMesh->m_data[k];

//				if(pMNode) {
//					if(pMNode->m_WeaponType == eq_weapon) {
//						m_WeaponListBox.AddString(pMNode->m_Name);
//					}
//				}

			}
		}
	}
*/
}

////////////////////////// animation listbox 관련
void CMcvView::ChangeAnimationListBox()
{
	RActor* pActor = GetActor();
	if ( pActor == NULL  ||   pActor->m_pMesh == NULL)
		return;


	// GetPrefix
	map< string, int>  mapPrefixCount;
	RMeshAnimationMgr* pAMgr = &pActor->m_pMesh->m_AniMgr;
	for ( size_t i= 0;  i < pAMgr->m_NodeHashList.size();  i++)
	{
		if ( pAMgr->m_NodeHashList[ i] == NULL)
			continue;

		CString strPrefix = pAMgr->m_NodeHashList[ i]->GetAliasName().c_str();
		int nFind = strPrefix.Find( '_');
		if ( nFind < 2)
			continue;

		strPrefix.Delete( nFind, strPrefix.GetLength() - nFind);

		map< string, int>::iterator itr = mapPrefixCount.find( strPrefix.GetBuffer());
		if ( itr == mapPrefixCount.end())
			mapPrefixCount.insert( map< string, int>::value_type( strPrefix.GetBuffer(), 1));
		else
			(*itr).second++;
	}


	GetAnimationSidebar()->m_cAnimationTreeCtrl.DeleteAllItems();
	GetAnimationSidebar()->m_cAnimationTreeCtrl.SetIndent(0);

	map< string, HTREEITEM>  mapPrefix;
	for ( size_t i= 0;  i < pAMgr->m_NodeHashList.size();  i++)
	{
		if ( pAMgr->m_NodeHashList[ i] == NULL)
			continue;

		CString strPrefix, strName;
		strName = pAMgr->m_NodeHashList[ i]->GetAliasName().c_str();
		strPrefix = strName;

		int nFind = strPrefix.Find( '_');
		if ( nFind < 2)
		{
			HTREEITEM hItem = GetAnimationSidebar()->m_cAnimationTreeCtrl.InsertItem( strName.GetBuffer(), TVI_ROOT);
			GetAnimationSidebar()->m_cAnimationTreeCtrl.SetItemData( hItem, (int)i);
			continue;
		}

		strPrefix.Delete( nFind, strPrefix.GetLength() - nFind);
		strName.Delete( 0, strPrefix.GetLength() + 1);

		map< string, HTREEITEM>::iterator itr = mapPrefix.find( strPrefix.GetBuffer());
		if ( itr == mapPrefix.end())
		{
			map< string, int>::iterator itr_count = mapPrefixCount.find( strPrefix.GetBuffer());
			if ( itr_count != mapPrefixCount.end()  &&  (*itr_count).second > 1)
			{
				HTREEITEM hItem = GetAnimationSidebar()->m_cAnimationTreeCtrl.InsertItem( strPrefix, TVI_ROOT);
				GetAnimationSidebar()->m_cAnimationTreeCtrl.SetItemData( hItem, -1);

				mapPrefix.insert( map< string, HTREEITEM>::value_type( strPrefix.GetBuffer(), hItem));

				hItem = GetAnimationSidebar()->m_cAnimationTreeCtrl.InsertItem( strName.GetBuffer(), hItem);
				GetAnimationSidebar()->m_cAnimationTreeCtrl.SetItemData( hItem, (int)i);
			}
			else
			{
				HTREEITEM hItem = GetAnimationSidebar()->m_cAnimationTreeCtrl.InsertItem( pAMgr->m_NodeHashList[ i]->GetAliasName().c_str(), TVI_ROOT);
				GetAnimationSidebar()->m_cAnimationTreeCtrl.SetItemData( hItem, (int)i);
			}
		}
		else
		{
			HTREEITEM hItem = GetAnimationSidebar()->m_cAnimationTreeCtrl.InsertItem( strName.GetBuffer(), (*itr).second);
			GetAnimationSidebar()->m_cAnimationTreeCtrl.SetItemData( hItem, (int)i);
		}
	}


	GetAnimationSidebar()->m_cAnimationTreeCtrl.SortChildren( TVI_ROOT);
	for ( map< string, HTREEITEM>::iterator itr = mapPrefix.begin();  itr != mapPrefix.end();  itr++)
		GetAnimationSidebar()->m_cAnimationTreeCtrl.SortChildren( (*itr).second);


	m_AniBlendDlg.ResetAniBlendGrid( pAMgr, &m_ExternAni);
}

void CMcvView::SelChangeAnimationList()
{
	int sel = GetAnimationSidebar()->m_cAnimationTreeCtrl.GetItemData( GetAnimationSidebar()->m_cAnimationTreeCtrl.GetSelectedItem());
	if ( sel < 0)
	{
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_ADDANI)->EnableWindow( TRUE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_EDITANI)->EnableWindow( FALSE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_DELANI)->EnableWindow( FALSE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_ADDEVENT)->EnableWindow( FALSE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_EDITEVENT)->EnableWindow( FALSE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_DELEVENT)->EnableWindow( FALSE);

		GetAnimationSidebar()->m_cEventListBox.ResetContent();
		return;
	}
	else
	{
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_ADDANI)->EnableWindow( TRUE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_EDITANI)->EnableWindow( TRUE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_DELANI)->EnableWindow( TRUE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_ADDEVENT)->EnableWindow( TRUE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_EDITEVENT)->EnableWindow( FALSE);
		GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_DELEVENT)->EnableWindow( FALSE);
	}

	ChangeAnimationList( sel);

	GetFrameControlBar()->m_SliderAniSpeed.SetPos( 5);
	GetFrameControlBar()->GetDlgItem( IDC_STATIC_SPEED)->SetWindowText("Speed : x1.0");

	m_nPrevFrame = -1;
}

void CMcvView::DeleteSelectedAnimation()
{
	/*
   for (int i=0;i < m_AnimationListBox.GetCount();)
   {
      if(m_AnimationListBox.GetSel( i ))
	  {
		RAnimationMgr* pAMgr = GetActor()->m_pMesh->m_AniMgr;
		// TODO: 너무 많아서 disable -_-
		  m_AnimationListBox.DeleteString(i);

	  }
	  else
		  i++;
   }
   */
}

bool CMcvView::IsActorCreated()
{
	if(GetActor()->m_pMesh!=NULL)
		return true;

	return false;
}

void CMcvView::ClearActor()
{
	GetModelInfoSidebar()->m_treeNodes.DeleteAllItems();

	RActor* pActor = GetActor();
	if( pActor )
	{
		pActor->Destroy();
		if( pActor->m_pMesh )
		{
			REngine::GetSceneManager().ReleaseResource( pActor->m_pMesh );
			pActor->m_pMesh = NULL;
		}
	}

	//GetMain()->Restore();

//	GetMeshMgr()->DelAll();

	GetMain()->m_pSelectedActorNode = NULL;	
//	m_NodesCombo.ResetContent();
//	GetControlToolBar()->m_treeNodes.DeleteAllItems();
//	m_lbEvents.ResetContent();
//	m_lbEventHandler.ResetContent();
//	GetControlToolBar()->m_AnimationListBox.ResetContent();
}

/*
void CMcvView::LoadXmlFile(char* FileName)
{
	ClearActor();

	int nID = GetMeshMgr()->AddXml(FileName);//load 폴더는? xml 이 있는폴더??

	if(nID == -1) return;

	RMesh* pMesh = GetMeshMgr()->GetFast(nID);

	GetActor()->Create(pMesh);

//	RActor* pActor = NULL;
	
//	int vid = GetActorMgr()->Add(pMesh);

//	if(vid == -1) return;

//	pActor = GetActorMgr()->GetFast(vid);

	// 등록 정보에 따라 list box 채우기

	UpdateNodes();
	ChangeWeaponListBox();
	ChangeAnimationListBox();
}
*/

void CMcvView::LoadAniFile(char* FileName)
{
/*	if(!IsActorCreated()) return;

	string strAnimationName = MGetPureFileName(FileName);

	RAnimation* pAnimation = GetAnimationMgr()->Add(strAnimationName.c_str(),FileName);
	pAnimation->GetResource()->BuildAll();

	RActor* pActor = GetActor();

	if( pActor ) {
		pActor->SetAnimation( pAnimation->GetAliasName().c_str(), RAT_NONE );
		PlayAnimation();
	}
*/
	if(!IsActorCreated())
		return;

	CAddAnimationDlg dlg;

	dlg.SetAniName( "");
	dlg.SetFileName( FileName);
	dlg.SetLoop( true);
	dlg.SetSpeed( 1.0f);
	dlg.SetEditMode( true);

	if ( dlg.DoModal() == IDCANCEL)
		return;

	CString strAniName = dlg.GetAniName();
	CString strFname = dlg.GetFileName();
	bool bLoop = dlg.GetLoop();
	float fSpeed = dlg.GetSpeed();

	if ( strAniName.IsEmpty() == true  ||  strFname.IsEmpty() == true)
		return;

	RActor* pActor = GetMainView()->GetActor();
	if ( pActor == NULL)
		return;

	pActor->Stop();

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.Add( strAniName.GetBuffer(), strFname.GetBuffer());
	pAni->GetResource()->BuildAll();
	if(pAni)
	{
		pAni->SetAnimationLoopType( ( bLoop == true)  ? RAniLoopType_Loop : RAniLoopType_HoldLastFrame);
		pAni->SetSpeed( fSpeed);
	}


	GetMainView()->ChangeAnimationListBox();
}


// 이펙 툴의 LoadEffectFile를모방한다. - 090602, OZ
bool CMcvView::LoadXmlFile(char* filename)
{
	// What to do ?
	//DeleteEffectInstance();
	
	// no more need..
	//const char* filename= MStringToCharPointer(FileName);

	bool bLoad = LoadSceneFile(filename);

	if( !bLoad )
	{
		_ASSERT(!"엑세멜 파일 실패");
		return false;
	}

	mlog("엑세멜 파일 로드 성공\n");

	//What da ~
	//ReloadActor();

	return true;	

}


// 이펙 툴의 LoadEffectFile를모방한다. - 090602, OZ
bool CMcvView::LoadSceneFile(const char* szFileName)
{
	// What to do ?
	//DeleteEffectInstance();

	RSceneManager *pSceneManager = REngine::GetSceneManagerPtr();

	//string strFileName = REngine::GetConfig().GetPath( RP_EFFECT ) + string(szFileName) ;
	string strFileName = string(szFileName) ;
	pSceneManager->LoadWorldSceneNodeFromRootXMLFile(pSceneManager->GetCurrentRootSceneNode(true), strFileName.c_str() );
	//pSceneManager->LoadRootSceneFromXML( szFileName);

	const RSceneNodeList& topLevelNodes = pSceneManager->GetCurrentRootSceneNode(false)->GetChildren();
	// 이건 이펙트 툴에서 가져오코드라서, 이펙트 노드인지 체크하는데 MCV에서는 어카지?
	//for( RSceneNodeList::const_iterator itr = topLevelNodes.begin(); itr!=topLevelNodes.end(); ++itr)
	//{
	//	RSceneNode* pSceneNode = *itr;
	//	if( MIsDerivedFromClass(REffectSceneNode, pSceneNode) ) 
	//	{
	//		// 두개 이상있으면 에러
	//		// What to do ?
	//		_ASSERT( NULL == m_pCurrentSceneNode );
	//		m_pCurrentSceneNode = MDynamicCast(REffectSceneNode, pSceneNode);
	//	}
	//}
	//탑 레벨의것을 현재 씬 노드로 걍 지정
	// 여러 씬을 부를떄를 고려해볼까
	//_ASSERT( NULL == m_pCurrentSceneNode );
	RSceneNodeList::const_iterator itr = topLevelNodes.begin();
	m_pCurrentSceneNode = *itr;

	// What to do ?
	if( NULL == m_pCurrentSceneNode )
		return false;

	// What to do ?
	//m_strFileName = string(szFileName); 
	//UpdateCaption();

	// What to do ?
	//RefreshSceneNodeTreeView();

	// 스타트하면 뭐해먹는겨?? 프레이 컨트롤 타입을 어쩌고저쩌고 하는데...
	// 이펙트 노드에만 있네.. 스타트는...
	// 이펙트 노드는 죄다 찾아서 스타트 시켜버렷! - 090605, OZ
	for( RSceneNodeList::const_iterator itr = topLevelNodes.begin(); itr!=topLevelNodes.end(); ++itr)
	{
		RSceneNode* pSceneNode = *itr;
		if( MIsDerivedFromClass(REffectSceneNode, pSceneNode) ) 
		{
			((REffectSceneNode*)pSceneNode)->Start();
		}
	}

	// 에니메이션도 우선 죄다 찾아서 스타트 시켜버려! - 090605, OZ
	// 라이트가 딸려서 도는 지 테스트차...
	AnimateAllActor( topLevelNodes);
	

	return true;
}


bool CMcvView::AnimateAllActor( const RSceneNodeList& nodes)
{
	for( RSceneNodeList::const_iterator itr = nodes.begin(); itr!=nodes.end(); ++itr)
	{
		RSceneNode* pSceneNode = *itr;
		if( MIsDerivedFromClass( RActor, pSceneNode)) 
		{
			RActor* pActorNode = (RActor*)pSceneNode;
			if( pActorNode->SetAnimation( McvConfig::GetInstance().m_strLightAni.c_str()))
			{
//				pActorNode->ForceAnimationControlLoop( true);
				pActorNode->Play();
			}
		}

		// 모든 뎁스를 돌게 재귀적 호출
		AnimateAllActor( pSceneNode->GetChildren());
	}

	return true;
}


/*
bool CMcvView::DefaultMeshFileOpen()
{
	std::string defaultEluFilename = string( REngine::GetConfig().GetPath(RP_EFFECT) ) + "test dummy.elu";

	bool result = MeshFileOpen(defaultEluFilename.c_str());
	if( result )
		AttachEffect( GetCurrentEffectSceneNode(), m_pActor, m_pActor->GetActorNode("Dummy02") );

	return result;
}
*/


void CMcvView::LoadFile(char* FileName)
{
	GetActor()->Stop();

	GetAnimationSidebar()->m_cAnimationTreeCtrl.DeleteAllItems();
	GetAnimationSidebar()->m_cAnimationTreeCtrl.SetIndent(0);
	GetAnimationSidebar()->m_cAnimationTreeCtrl.SelectItem( NULL);
	GetAnimationSidebar()->m_cEventListBox.ResetContent();


	ClearActor();

	RMesh* pMesh = REngine::GetSceneManager().CreateResource< RMesh >(FileName);
	if ( pMesh->BuildAll() == false )
	{
		_ASSERT(!"메시 로딩 실패");
		REngine::GetSceneManager().ReleaseResource( pMesh );
		return;
	}

	GetActor()->Create(pMesh);

//	RActor* pActor = NULL;

//	int vid = GetActorMgr()->Add(pMesh);

//	if(vid == -1) return;

//	pActor = GetActorMgr()->GetFast(vid);

//	GetActor()->SetPosition(100,0,0);
	UpdateNodes();

	m_bAlpha2Pass = !GetActor()->GetAlpha2PassRendering();
	OnAlpha2pass();

	string strPath = FileName;
	strPath += ".animation.xml";
	ReadXmlInfo( strPath.c_str());


	PROJECT_TYPE type = m_ProjectType.GetProjectType();
	m_ProjectType.SetProjectType( FileName);

	if ( type != m_ProjectType.GetProjectType())
		LoadSound( m_ProjectType.GetFnameSoundXml());

	m_nPrevFrame = -1;

	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_ADDANI)->EnableWindow( FALSE);
	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_EDITANI)->EnableWindow( FALSE);
	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_DELANI)->EnableWindow( FALSE);
	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_ADDEVENT)->EnableWindow( FALSE);
	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_EDITEVENT)->EnableWindow( FALSE);
	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_DELEVENT)->EnableWindow( FALSE);
}

void CMcvView::LoadSound( const char* szFileName)
{
	m_SoundList.clear();


	MXml xml;
	if ( xml.LoadFile( szFileName) == false)
		return;

	MXmlElement* pRoot = xml.Doc()->FirstChildElement();
	for ( MXmlElement* pElement = pRoot->FirstChildElement();  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		string node = pElement->Value();

		if ( node == "SOUND")
		{
			string name;
			_Attribute( name, pElement, "name");

			string filename;
			_Attribute( filename, pElement, "filename");


			m_SoundList.insert( map< string, string>::value_type( name, filename));
		}
	}
}

bool CMcvView::ReadXmlInfo( const char* xmlFileName, bool bExtern, const char* szFilter)
{
	MXml xml;
	if ( xml.LoadFile( xmlFileName) == false)
		return false;

	if ( bExtern == false)
	{
		m_ExternXmlFile.clear();
		m_ExternAni.clear();
	}

	MXmlElement* pRoot = xml.Doc()->FirstChildElement();
	for ( MXmlElement* pElement = pRoot->FirstChildElement();  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		string node = pElement->Value();

		if ( node == "CopyFromXml")
		{
			string filename;
			_Attribute( filename, pElement, "filename");

			string filter;
			_Attribute( filter, pElement, "filter");

			string path = MGetPathFileNameFromRelativePath( xmlFileName, filename);

			m_ExternXmlFile.insert( map< string, string>::value_type( filename, filter));

			ReadXmlInfo( path.c_str(), true, filter.c_str());
		}

		else if ( node == "AddAnimation")
		{
			string name;
			_Attribute( name, pElement, "name");

			if ( strlen( szFilter) > 2)
			{
				if ( name.compare( 0, strlen( szFilter), szFilter) != 0)
					continue;

				name.erase( 0, strlen( szFilter));
			}

			if ( bExtern == true)
				m_ExternAni.insert( map< string, string>::value_type( name, xmlFileName));
		}

		else if ( node == "AnimationTransitions")
		{
		}
	}

	return true;
}


void CMcvView::FileOpen(char* filename)
{
	int len = strlen(filename);

	if(len <= 0) return;

	if(strncmp(&filename[len-4],".ani",4)==0) {

		if(m_bFrameInfoDlg) {
			m_bFrameInfoDlg = false;
			OnAnimationinfo();
		}

		LoadAniFile(filename);
		ChangeAnimationListBox();
	}
	else if(strncmp(&filename[len-4],".elu",4)==0) {

		m_LastOpenFile = filename;

		LoadFile(filename);
		ChangeAnimationListBox();

		/*
		if(m_bMtrlEditDlg) {// 보이고 있는 상태라면 갱신이 필요
			m_bMtrlEditDlg = false;
			OnMtrleditdlg();
		}
		*/
	}
	// mxl 파일(씬)을 로딩하게 바꾼다 - 090604,OZ
	else if(strncmp(&filename[len-10],".scene.xml",4)==0) {

		m_LastOpenFile = filename;

		// TO DO !!!!
		LoadXmlFile(filename);

		//if(m_bMtrlEditDlg) {// 보이고 있는 상태라면 갱신이 필요
		//	m_bMtrlEditDlg = false;
		//	OnMtrleditdlg();
		//}
	}
	else if( (strncmp(&filename[len-4],".bmp",4)==0 ) || 
			 (strncmp(&filename[len-4],".png",4)==0 ) || 
			 (strncmp(&filename[len-4],".tga",4)==0 ) ||
			 (strncmp(&filename[len-4],".jpg",4)==0 ) ||
			 (strncmp(&filename[len-4],".dds",4)==0 ) ) {
		{
			return;
		}

	}
	else {
		::MessageBox(NULL,"알수없는 확장자를 가진 파일입니다~","info",MB_OK);
		return;
	}

	string strCaption = "McvRS3 - " + string(m_LastOpenFile);
	AfxGetMainWnd()->SetWindowText(strCaption.c_str());

	ValidateModel();

	GetMain()->CameraReset();
//	ChangeAnimationInfoString();

	AfxGetApp()->AddToRecentFileList(filename); // 최근 파일에 추가
}

void CMcvView::OnFileOpen()
{
	CString t_str;
	char	t_filename[256];

	CFileDialog dlg( TRUE,_T("scene.xml"),_T("*.scene.xml"),OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("Scene XML File (test.scene.xml)|*.scene.xml|Model File (test.elu) |*.elu|Animation File (test.ani)|*.ani|"));

	if( dlg.DoModal()==IDOK ) {

		t_str = dlg.GetPathName();

		lstrcpy((LPSTR)t_filename,(LPSTR)t_str.operator const char*());

		FileOpen(t_filename);
	}
}

// 무기파일 열기 elu 나 xml -> weapon mesh list 와 연결
// weapon file open

/*
void CMcvView::LoadWeaponFile(char* filename)
{
//	GetWeaponMeshMgr()->DelAll();
	GetWeaponMeshMgr()->Add(filename);

	ChangeWeaponListBox();
}

void CMcvView::LoadWeaponXmlFile(char* filename)
{

}

void CMcvView::WFileOpen(char* filename)
{
	int len = strlen(filename);

	if(len <= 0) return;

	if(strncmp(&filename[len-4],".elu",4)==0) {
		LoadWeaponFile(filename);
	}
	else if(strncmp(&filename[len-4],".xml",4)==0) {
		LoadWeaponXmlFile(filename);
	}
	else
		::MessageBox(NULL,"알수없는 확장자를 가진 파일입니다~","info",MB_OK);
}

void CMcvView::OnFileWopen()
{
	CString t_str;
	char	t_filename[256];

	CFileDialog dlg( TRUE,_T("elu"),_T("*.elu"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("Model File (test.elu) |*.elu|XML File (test.xml)|*.xml|"));

	if( dlg.DoModal()==IDOK ) {

		t_str = dlg.GetPathName();

		lstrcpy((LPSTR)t_filename,(LPSTR)t_str.operator const char*());

		WFileOpen(t_filename);
	}
}
*/

void CMcvView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 현재 셰이더 조합과 list 검색의 부담을 줄이기 위해
	// RShaderComposite를 멤버변수로 가지고 있는 경우가 생겨서 일단 작동 안하게 주석 처리합니다.
	//rs3::RActor::DestroyShader();
	//rs3::RActor::CreateCheckShader();

	char filename[256];

	int cnt = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	bool bAllAniFile = true;

	for(int i=0;i<1;i++) {
		DragQueryFile(hDropInfo,i,filename,sizeof(filename));
		int len = strlen(filename);
		if(!_strnicmp(&filename[len-4],".ani",4)==0)
			bAllAniFile = false;
	}
	if(bAllAniFile)	// 다 애니메이션 파일이면 추가한다
	{
		for(int i=0;i<cnt;i++) {
			DragQueryFile(hDropInfo,i,filename,sizeof(filename));
			FileOpen(filename);
		}
		return;
	}


	if(cnt > 1)
		::MessageBox(NULL,"한번에 하나씩~","info",MB_OK);

//	for(int i=0;i<cnt;i++) {
	for(int i=0;i<1;i++) {
		DragQueryFile(hDropInfo,i,filename,sizeof(filename));
	}

	FileOpen(filename);
}

const char* CMcvView::GetLastOpenFile()
{
	return m_LastOpenFile.c_str();
}

BOOL CMcvView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(GetMain())
		GetMain()->m_nWheel += zDelta;

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMcvView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMcvView::OnTexOnOff()
{
	m_bTextureRender = !m_bTextureRender;
}

void CMcvView::OnVertexNormal()
{
	m_bVertexNormal = !m_bVertexNormal;
}

void CMcvView::OnCartoonLightOnoff()
{
}

void CMcvView::OnUpdateVertexNormal(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)m_bVertexNormal);
}

void CMcvView::OnUpdateTexOnOff(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)!m_bTextureRender);
}

void CMcvView::OnUpdateCartoonLightOnoff(CCmdUI *pCmdUI)
{

}

///////////////////////////////////////////////////////
/*
void CMcvView::OnCbnSelchangePartsType()
{
	m_nSelectPartsType = m_NodesCombo.GetCurSel();

	ChangePartsListBox();
}

void CMcvView::OnCbnSelchangeWeaponType()
{
	m_nSelectWeaponType = m_WeaponTypeCombo.GetCurSel();

	ChangeWeaponListBox();
}
// 장비될 위치 선택
void CMcvView::OnCbnSelchangeWeaponType2()
{
	m_nSelectWeaponType2 = m_WeaponType2Combo.GetCurSel();
}
*/
//void CMcvView::OnLbnSelchangePartsList()
//{
	/*
	int sel = m_PartsListbox.GetCurSel();

	RActor* pActor = GetActor();

	RMeshPartsType _parts = (RMeshPartsType)((int)eq_parts_hair + m_nSelectPartsType);

	CString str;
	m_PartsListbox.GetText(sel,str);

	/*
	if(pActor) {

	//	base parts

		pActor->SetParts(_parts,(char*)str.operator const char*() );

	}
	*/

//}

//void CMcvView::OnLbnSelchangeWeaponList()
//{
//	int sel = m_WeaponListBox.GetCurSel();
/*
	RActor* pActor = GetActor();

	// 장비위치

//	RMeshPartsType _weapon = (RMeshPartsType)((int)eq_parts_left_blade + m_nSelectWeaponType2);
	RWeaponMotionType type = eq_ws_pistol;

	CString str;

	m_WeaponListBox.GetText(sel,str);

	if(pActor) {

		//	base weapon

		if(sel==0) {
//			pActor->SetBaseParts(_weapon);//weapon 은 따로 관리 될수도..
		}
		else {

//			RMesh* pMesh = m_WeaponMeshMgr.GetFast(0);
//			pActor->AddWeapon(type,pMesh);
		}
	}
*/
//}


void CMcvView::PlayAnimation()
{
	RActor* pActor = GetActor();

	if ( pActor->GetAnimationController().GetFrame() >= (pActor->GetAnimationController().GetMaxFrame() - 1))
	{
		SetFrame( 0);
	}

	pActor->Play();

	CWnd* pPlayButton = GetFrameControlBar()->GetDlgItem(IDC_PLAY_PAUSE);
	pPlayButton->SetWindowText("Pause");
}

void CMcvView::PauseAnimation()
{
	RActor* pActor = GetActor();
	pActor->Pause();

	CWnd* pPlayButton = GetFrameControlBar()->GetDlgItem(IDC_PLAY_PAUSE);
	pPlayButton->SetWindowText("Play");
}

void CMcvView::OnBnClickedStop()
{
	RActor* pActor = GetActor();

	CWnd* pPlayButton = GetFrameControlBar()->GetDlgItem(IDC_PLAY_PAUSE);
	pPlayButton->SetWindowText("Play");

	GetFrameControlBar()->m_SliderAnimation.SetPos( 0);
	GetFrameControlBar()->m_strFrame.Format( "0 / %d frame", pActor->GetAnimationController().GetMaxFrame());
	GetFrameControlBar()->m_strSecond.Format( "0.00 / %.2f sec", (float)pActor->GetAnimationController().GetMaxFrame() / 4800.0f);
	GetFrameControlBar()->UpdateData( false);

	pActor->Stop();

	m_nPrevFrame = -1;
}

void CMcvView::OnBnClickedPlayPause()
{
	RActor* pActor = GetActor();
	if(!pActor) return;

	switch(pActor->GetAnimationController().GetState())
	{
	case RPS_STOP : SelChangeAnimationList();break;
	case RPS_PLAY : PauseAnimation();break;
	case RPS_PAUSE: PlayAnimation();break;
	}
}

void CMcvView::SetFrame( int nFrame)
{
	RActor* pActor = GetActor();
	if ( pActor == NULL)
		return;

	pActor->SetFrame( nFrame);

	m_nPrevFrame = nFrame - 1;
}


// 카메라와 라이트 위치값 초기화 

void CMcvView::OnCameraReset()
{
	if(GetMain())
		GetMain()->CameraReset();
}

void CMcvView::OnNMThemeChangedSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void CMcvView::OnNMReleasedcaptureSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	float pos = (float)GetFrameControlBar()->m_SliderAniSpeed.GetPos() * 0.2f;

	RActor* pActor = GetActor();

	if(pActor)
		pActor->SetSpeed( pos);

	*pResult = 0;
}

void CMcvView::OnScaledlg()
{
	if(!m_bScaleDlg) {
		m_ScaleDlg.ShowWindow(SW_SHOW);
	}
	else {
		m_ScaleDlg.ShowWindow(SW_HIDE);
	}

	ToggleScaleDlg();
}

/*
void CMcvView::OnModelinfo()
{
	if(!m_bModelInfoDlg) {

		// 노드 정보와 메터리얼 정보를 담아준다..
		m_ModelInfoDlg.Begin();		
		m_ModelInfoDlg.ShowWindow(SW_SHOW);
	}
	else {
		m_ModelInfoDlg.ShowWindow(SW_HIDE);
	}

	m_bModelInfoDlg = !m_bModelInfoDlg;
}
*/

/*
void CMcvView::OnMtrleditdlg()
{
	if(!m_bMtrlEditDlg) {
		// 지금 읽은 모델이 있다면..
		// 이전값은 clear..

		m_MtrlDlg.Begin();

		m_MtrlDlg.ShowWindow(SW_SHOW);
	}
	else {

		m_MtrlDlg.ShowWindow(SW_HIDE);
	}

	m_bMtrlEditDlg = !m_bMtrlEditDlg;
}
*/

void CMcvView::OnAnimationinfo()
{
	if(!m_bFrameInfoDlg) {
		m_FrameInfoDlg.Begin();
		m_FrameInfoDlg.ShowWindow(SW_SHOW);

	}
	else {
		m_FrameInfoDlg.ShowWindow(SW_HIDE);
	}

	m_bFrameInfoDlg = !m_bFrameInfoDlg;
}


//////////////////////////////////////////////////////////////
// mouse event

float GetTimer()//원할때
{
	static DWORD time_back = timeGetTime();

	DWORD this_time = timeGetTime();

	float s = (float)(this_time - time_back)/1000.f;

	time_back = this_time;

	return s;
}

static bool bRightDrag = false;
static bool bLeftDrag = false;


void CMcvView::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pos = point - m_oldCursorPos;

	CTCamera* pCamera = &GetMain()->m_Camera;

	float t = //GetTimer() * 
		pCamera->m_fDist * 0.002f;

	if(bRightDrag) {// pan
		
		if(pos.x > 0)
			pCamera->Left((float)pos.x * t);
		else 
			pCamera->Right((float)-pos.x * t);

		if(pos.y > 0)
			pCamera->Up((float)pos.y * t);
		else 
			pCamera->Down((float)-pos.y * t);

	}
	else if(bLeftDrag) {//rot

		if(pos.x > 0)
			pCamera->CenterRotationLeft((float)pos.x  );
		else 
			pCamera->CenterRotationRight((float)-pos.x );

		if(pos.y > 0)
			pCamera->CenterRotationUp((float)-pos.y );
		else 
			pCamera->CenterRotationDown((float)pos.y );

	}

	m_oldCursorPos = point;

	CFormView::OnMouseMove(nFlags, point);
}

// 마우스캡춰 써야하나?

void CMcvView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(IsCursorInView())
	{
		SetCapture();
		bRightDrag = true;
		m_oldCursorPos = point;
	}

	CFormView::OnRButtonDown(nFlags, point);
}

void CMcvView::OnRButtonUp(UINT nFlags, CPoint point)
{
	bRightDrag = false;

	RActor* pActor = GetActor();

	RVector v;
	bool g_bPick = false;

//	RPickInfo info;

	if(pActor&&pActor->m_pMesh) {
/*
		pActor->m_pMesh->SetPickingType(pick_real_mesh);

		if(pActor->Pick ( point.x - g_win_pos_x, point.y - g_win_pos_y, &info ) ) {

//			if(info.pMNode) {
//				m_SelMeshNodeName = info.pMNode->m_Name;
//				UpdateData(FALSE);
//			}

			g_bPick = true;
		}
		else {
			m_SelMeshNodeName = "";
			UpdateData(FALSE);
			g_bPick = false;
		}
*/
	}

	CFormView::OnRButtonUp(nFlags, point);

	ReleaseCapture();
}

void CMcvView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	m_oldCursorPos = point;
	m_LButtonClickPos = point;
	bLeftDrag = true;
	CFormView::OnLButtonDown(nFlags, point);
}

void CMcvView::OnLButtonUp(UINT nFlags, CPoint point)
{
	bLeftDrag = false;
	CFormView::OnLButtonUp(nFlags, point);
	ReleaseCapture();

	// pick
	RViewport viewport = GetDevice()->GetViewport();
	RECT rt = {0, 0, viewport.nWidth, viewport.nHeight};

	RVector vCamPosP, vMouseDir, vOnMouseRayQ;
	vCamPosP = GetMain()->m_Camera.GetWorldPosition();

	//vMouseDir 구하기
	RGetScreenLine(point.x, point.y, rt,
		GetMain()->m_Camera.GetViewMatrix(),
		GetMain()->m_Camera.GetProjectionMatrix(),
		&vMouseDir);

	RPICKINFO pickInfo;
	pickInfo.inMethod = RPM_DETAIL;
	pickInfo.inPickOrigin = vCamPosP;
	pickInfo.inPickTo = vCamPosP + vMouseDir*10000.f;

	if(GetActor()->PickActorNode(pickInfo))
	{
		RActorNode* pActorNode = (RActorNode*)pickInfo.outNode;
		SelectNode(pActorNode);
	}
	else
	{
		if(m_LButtonClickPos==point)
			Unselect();
	}
}

void CMcvView::SelectNode(RActorNode* pActorNode)
{
	int nID = pActorNode->m_pMeshNode->m_nID;
	GetModelInfoSidebar()->SelectNodeByID(nID);
}

void CMcvView::Unselect()
{
	GetModelInfoSidebar()->SelectNodeByID(-1);
}


void CMcvView::OnMapObjPos()
{
	RActor* pActor = GetActor();
	
	if(pActor) {
//		m_vMapObjPos = pActor->m_vBMin + (pActor->m_vBMax - pActor->m_vBMin)/2.f;
//		m_vMapObjPos.y = pActor->m_vBMin.y;
	}
}

void CMcvView::OnGridOnoff()
{
	GridOnOff();
}

void CMcvView::OnBboxOnoff()
{
	m_bDrawBBox = !m_bDrawBBox;
}

void CMcvView::OnEffectRender()
{
	m_bEffectModel = !m_bEffectModel;
}

void CMcvView::OnCharViewMode()
{
	m_bCharacterView = !m_bCharacterView;
}

void CMcvView::OnUpdateBboxOnoff(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)m_bDrawBBox);
}

void CMcvView::OnUpdateGridOnoff(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)m_bDrawGrid);
}

void CMcvView::OnUpdateEffectRender(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)m_bEffectModel);
}

void CMcvView::OnUpdateCharViewMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)m_bCharacterView);
}

void CMcvView::OnUpdateAlpha2pass(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)m_bAlpha2Pass);
}

void CMcvView::OnHDROnOff()
{
	static bool bHDREnable = false;
	bHDREnable = !bHDREnable;
	REngine::GetSceneManager().SetHDREnable(bHDREnable);
}

void CMcvView::OnUpdateHDROnOff(CCmdUI *pCmdUI)
{
	CTCamera* pCamera = &GetMain()->m_Camera;
	
	pCmdUI->SetCheck( REngine::GetSceneManager().IsEnableHDR());
}

/*
void CMcvView::OnPartscolor()
{
	CColorDialog dlg;
	COLORREF col;

	BYTE a,r,g,b;

	if( dlg.DoModal()==IDOK ) {
		col = dlg.GetColor();

		a = (col>>24)&0xff;
		b = (col>>16)&0xff;
		g = (col>> 8)&0xff;
		r = (col    )&0xff;

		col = D3DCOLOR_ARGB(255,r,g,b);

		// 지금 선택되어있는 파츠 노드를 구한다..xml 이 있을 경우와 아닌경우? visual mesh 에서 구하기..

		RActor* pActor = GetActor();

		if(pActor) {

			int sel = m_NodesCombo.GetCurSel();

			RMeshPartsType type = eq_parts_etc;

				 if(sel==0) type = eq_parts_hair;
			else if(sel==1) type = eq_parts_face;
			else if(sel==2) type = eq_parts_chest;
			else if(sel==3) type = eq_parts_hands;
			else if(sel==4) type = eq_parts_legs;
			else if(sel==5) type = eq_parts_feet;
/*
			if(pActor->m_pTMesh) {
			
				RealSpace2::RMeshNode* pMNode = pActor->m_pTMesh[type];

				if(pMNode==NULL)//xml 이 아닌경우...
					pMNode = pActor->m_pMesh->GetMeshData(type);

				if(pMNode) {
					pMNode->SetTColor((DWORD)col);
				}

			}
*/
			
//		}
		// 장비위치
//	}
//}
//*/

/*
void CMcvView::OnPartscolorall()
{
	CColorDialog dlg;
	COLORREF col;

	BYTE a,r,g,b;

	if( dlg.DoModal()==IDOK ) {
		col = dlg.GetColor();

		a = (col>>24)&0xff;
		b = (col>>16)&0xff;
		g = (col>> 8)&0xff;
		r = (col    )&0xff;

		col = D3DCOLOR_ARGB(255,r,g,b);

		// 지금 선택되어있는 파츠 노드를 구한다..xml 이 있을 경우와 아닌경우? visual mesh 에서 구하기..

		RActor* pActor = GetActor();

		if(pActor) {

			RMeshPartsType type[6];

			type[0] = eq_parts_hair;
			type[1] = eq_parts_face;
			type[2] = eq_parts_chest;
			type[3] = eq_parts_hands;
			type[4] = eq_parts_legs;
			type[5] = eq_parts_feet;

			if(pActor->m_pMesh) {
/*
				RealSpace3::RMeshNode* pMNode = NULL;

				for(int i=0;i<pActor->m_pMesh->m_nDataNum;i++) {
					pMNode = pActor->m_pMesh->m_NodeTable[i];
//					if(!pMNode->m_isDummy&&pMNode->m_isDummyMesh&&pMNode->m_isb)
						pMNode->SetTexColor((DWORD)col);
				}

				for(int i=0;i<6;i++) {
				
					pMNode = pActor->m_pTMesh[type[i]];

					if(pMNode==NULL)
						pMNode = pActor->m_pMesh->GetMeshData(type[i]);

					if(pMNode) {
						pMNode->SetTColor((DWORD)col);
					}

				}
*/
//			}

//		}
		// 장비위치
//	}
//}
//*/

void CMcvView::OnBlendColor()
{
	CColorDialog dlg;
	COLORREF col;
	D3DCOLORVALUE color;
	BYTE a,r,g,b;

	if( dlg.DoModal()==IDOK ) {
		col = dlg.GetColor();

		a = (col>>24)&0xff;
		b = (col>>16)&0xff;
		g = (col>> 8)&0xff;
		r = (col    )&0xff;

		col = D3DCOLOR_ARGB(255,r,g,b);

		color.r = r/255.f;
		color.g = g/255.f;
		color.b = b/255.f;
		color.a = 1.f;

		RActor* pActor = GetActor();

		if(pActor) {
//			pActor->SetNPCBlendColor(color);
		}
	}	
}

void CMcvView::GridOnOff()
{
	static DWORD backuptime;

	if( backuptime + 200 < timeGetTime() ) {
		m_bDrawGrid = !m_bDrawGrid;
		backuptime = timeGetTime();
	}	
}

void CMcvView::ToggleFrameInfoDlg()	{ m_bFrameInfoDlg = !m_bFrameInfoDlg; }
void CMcvView::ToggleMrtlEditDlg()	{ m_bMtrlEditDlg = !m_bMtrlEditDlg; }
void CMcvView::ToggleScaleDlg()		{ m_bScaleDlg = !m_bScaleDlg;	}
void CMcvView::ToggleModelInfoDlg()	{ m_bModelInfoDlg = !m_bModelInfoDlg; }

void CMcvView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

/*
void CMcvView::OnCbnSelchangeComboNodes()
{
	RActorNode *pNode = GetActor()->GetActorNode(m_NodesCombo.GetCurSel());
	GetMain()->m_pSelectedActorNode = pNode;
}
*/

/*
void CMcvView::OnBnClickedButtonReplaceMesh()
{
	if(!GetMain()->m_pSelectedActorNode ||
		!GetMain()->m_pSelectedActorNode->m_pMeshNode->isRenderAble()) 
	{
		MessageBox("select a mesh node", "error");
		return;
	}

	ReplaceMeshDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		GetActor()->ReplaceMeshNode(GetMain()->m_pSelectedActorNode,
			dlg.m_pMesh->GetNode(dlg.m_strMeshNodeName));
	}
}
*/

/*
bool CMcvView::EditEvent(RAnimationEvent* pEvent)
{
	EditEventDlg dlg;
	dlg.m_strEventName = pEvent->m_strEvent.c_str();
	dlg.m_strFrame.Format("%d",pEvent->m_nFrame);
	dlg.m_strParam1 = pEvent->m_strParam1.c_str();
	dlg.m_strParam2 = pEvent->m_strParam2.c_str();

	if(dlg.DoModal() == IDOK)
	{
		pEvent->m_nFrame = atoi(dlg.m_strFrame);
		pEvent->m_strEvent = (LPCSTR)dlg.m_strEventName;
		pEvent->m_strParam1 = (LPCSTR)dlg.m_strParam1;
		pEvent->m_strParam2 = (LPCSTR)dlg.m_strParam2;
		return true;
	}

	return false;
}

void CMcvView::OnLbnDblclkListEvent()
{
	RAnimation* pAnimation = GetActor()->GetAnimationFrameInfo().m_pAnimation;
	if(!pAnimation) return;

	RAnimationEvent* pEvent = pAnimation->m_animationEvents[m_lbEvents.GetCurSel()];
	if(!pEvent) return;

	if(EditEvent(pEvent))
	{
		int nSel = m_lbEvents.GetCurSel();
		m_lbEvents.DeleteString(nSel);

		CString str;
		FormatEventString(str,pEvent);
		m_lbEvents.InsertString(nSel,str);
	}
}

void CMcvView::OnBnClickedButtonAddEvent()
{
	RAnimation* pAnimation = GetActor()->GetAnimationFrameInfo().m_pAnimation;
	if(!pAnimation) return;

	RAnimationEvent* pEvent = new RAnimationEvent;
	pEvent->m_nFrame = GetActor()->GetAnimationFrameInfo().m_nFrame;
	pEvent->m_strEvent = "unknown";

	if(EditEvent(pEvent))
	{
		pAnimation->m_animationEvents.push_back(pEvent);

		CString str;
		FormatEventString(str,pEvent);
		m_lbEvents.AddString(str);
	}else
		delete pEvent;
}

bool CMcvView::EditEventHandler(AnimationEventHandler *pHandler)
{
	EditHandlerDlg dlg;
	dlg.m_strEventName = pHandler->m_strEventName.c_str();
	dlg.m_strSoundFileName = pHandler->m_strSoundFileName.c_str();
	dlg.m_strEffectName = pHandler->m_strEffectFileName.c_str();
	dlg.m_strEffectParentName = pHandler->m_strEffectParent.c_str();
	if(dlg.DoModal())
	{
		pHandler->m_strEventName = (LPCSTR)dlg.m_strEventName;
		pHandler->m_strEffectFileName = (LPCSTR)dlg.m_strEffectName;
		pHandler->m_strEffectParent = (LPCSTR)dlg.m_strEffectParentName;
		pHandler->m_strSoundFileName = (LPCSTR)dlg.m_strSoundFileName;
		return true;
	}
	return false;
}

void CMcvView::OnBnClickedButtonAddHandler()
{
	AnimationEventHandler *pHandler = new AnimationEventHandler;

	RAnimation* pAnimation = GetActor()->GetAnimationFrameInfo().m_pAnimation;
	if(m_lbEvents.GetCurSel()!=LB_ERR && pAnimation)
	{
		RAnimationEvent* pEvent = pAnimation->m_animationEvents[m_lbEvents.GetCurSel()];
		_ASSERT(pEvent);
		pHandler->m_strEventName = pEvent->m_strEvent;
	}

	if(EditEventHandler(pHandler))
	{
		pHandler->m_itrator = m_eventListener.insert( AnimationEventListener::value_type(pHandler->m_strEventName,pHandler) );
		m_lbEventHandler.AddString(pHandler->m_strEventName.c_str());
		m_lbEventHandler.SetItemData(m_lbEventHandler.GetCount()-1,(DWORD_PTR)pHandler);
	}else
		delete pHandler;
}
void CMcvView::OnLbnDblclkListHandler()
{
	AnimationEventHandler *pHandler = (AnimationEventHandler*)m_lbEventHandler.GetItemData(m_lbEventHandler.GetCurSel());
	if(!pHandler) return;

	if(EditEventHandler(pHandler))
	{
		m_eventListener.erase(pHandler->m_itrator);
		pHandler->m_itrator = m_eventListener.insert( AnimationEventListener::value_type(pHandler->m_strEventName,pHandler) );

		int nSel = m_lbEventHandler.GetCurSel();
		m_lbEventHandler.DeleteString(nSel);
		m_lbEventHandler.InsertString(nSel,pHandler->m_strEventName.c_str());
		m_lbEventHandler.SetItemData(nSel,(DWORD_PTR)pHandler);
	}
}
*/

void CMcvView::OnShowBones()
{
	GetMain()->m_bShowBones = !GetMain()->m_bShowBones;
}

void CMcvView::OnUpdateShowBones(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)GetMain()->m_bShowBones);
}

/*
void CMcvView::DeleteSelectedEvent()
{
	RAnimation* pAnimation = GetActor()->GetAnimation();

	int nSel = m_lbEvents.GetCurSel();
	if(nSel!=LB_ERR && pAnimation && nSel<(int)pAnimation->m_animationEvents.size())
	{
		delete pAnimation->m_animationEvents[nSel];
		pAnimation->m_animationEvents.erase(pAnimation->m_animationEvents.begin()+nSel);
		m_lbEvents.DeleteString(nSel);
	}
}

void CMcvView::DeleteSelectedEventHandler()
{
	int nSel = m_lbEventHandler.GetCurSel();
	if(nSel==LB_ERR) return;

	AnimationEventHandler* pHandler = (AnimationEventHandler*)m_lbEventHandler.GetItemData(nSel);
	m_lbEventHandler.DeleteString(nSel);
	m_eventListener.DeleteHandler(pHandler);
}
*/

void CMcvView::OnFileSaveAs()
{
	if(!IsActorCreated()) return;

	string fileName = m_LastOpenFile + ".animation.xml";
	CFileDialog dlg( FALSE,_T("xml"),fileName.c_str(),OFN_NOCHANGEDIR| OFN_HIDEREADONLY| OFN_OVERWRITEPROMPT,
		_T("XML File (test.xml)|*.xml|"));

	if( dlg.DoModal()==IDOK ) {
		SaveXmlFile((LPCSTR)dlg.GetPathName());
		m_LastOpenFile = (LPCSTR)dlg.GetPathName();
	}
}

void CMcvView::OnFileSave()
{
	if(!IsActorCreated()) return;

	if( m_LastOpenFile.length() > 4 && 
		m_LastOpenFile.substr(m_LastOpenFile.length()-4)!=".xml")
	{
		OnFileSaveAs();
	}
	else
	{
		SaveXmlFile(m_LastOpenFile.c_str());
	}
}

bool CMcvView::SaveXmlFile(const char* szFileName)
{
	MXml xml;
	
	MXmlDeclaration* pDeclaration = xml.CreateDeclaration( "1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild( pDeclaration);
	
	MXmlElement *pRoot = new MXmlElement( "XML");
	xml.Doc()->LinkEndChild( pRoot);

	for ( map< string, string>::iterator itr = m_ExternXmlFile.begin();  itr != m_ExternXmlFile.end();  itr++)
	{
		MXmlElement *pElement = new MXmlElement( "CopyFromXml");
		pRoot->LinkEndChild( pElement);

		_SetAttribute( pElement, "filename", (*itr).first.c_str());
		_SetAttribute( pElement, "filter", (*itr).second.c_str());
	}


	RMeshAnimationMgr* pAniMgr = &GetActor()->m_pMesh->m_AniMgr;

	if ( GetActor()->m_pMesh)
	{
		for ( size_t i = 0;  i < pAniMgr->m_NodeHashList.size();  i++) 
		{
			RAnimation* pAnimation = pAniMgr->m_NodeHashList[ i];
			if ( pAnimation == NULL)
				continue;

			if ( m_ExternAni.find( pAnimation->GetAliasName()) != m_ExternAni.end())
				continue;

			MXmlElement *pElement = new MXmlElement( "AddAnimation");
			pRoot->LinkEndChild( pElement);
			_SetAttribute( pElement, "name", pAnimation->GetAliasName());

			string strRelativePath = MGetPathFileNameFromRelativePath( pAnimation->GetResource()->GetName(), szFileName);
			_SetAttribute( pElement, "filename", strRelativePath);
			_SetAttribute( pElement, "motion_loop_type", (pAnimation->GetAnimationLoopType()==RAniLoopType_Loop) ? "loop" : "lastframe");
			_SetAttribute( pElement, "speed", pAnimation->GetSpeed());

			pAnimation->SaveEvent( pElement);
		}
	}


	if ( pAniMgr->m_AnimationBlendTable.IsValid())
	{
		MXmlElement* pAniTransElements = new MXmlElement( "AnimationTransitions");
		pRoot->LinkEndChild( pAniTransElements);

		for ( size_t i = 0;  i < pAniMgr->m_NodeHashList.size();  i++) 
		{
			for ( size_t j = 0;  j < pAniMgr->m_NodeHashList.size();  j++)
			{
				if ( i == j)
					continue;

				if ( m_ExternAni.find( pAniMgr->m_NodeHashList[ i]->GetAliasName()) != m_ExternAni.end()  &&
					m_ExternAni.find( pAniMgr->m_NodeHashList[ j]->GetAliasName()) != m_ExternAni.end())
					continue;


				BLENDDATA blend = pAniMgr->m_AnimationBlendTable.GetAnimationTransData( i, j);
				if ( pAniMgr->m_AnimationBlendTable.IsNotDefaultValue( blend) == false)
					continue;

				MXmlElement* pAniTransElement = new MXmlElement( "AnimationTransition");
				pAniTransElements->LinkEndChild( pAniTransElement);


				_SetAttribute( pAniTransElement, "source", pAniMgr->m_NodeHashList[ i]->GetAliasName());
				_SetAttribute( pAniTransElement, "target", pAniMgr->m_NodeHashList[ j]->GetAliasName());
				_SetAttribute( pAniTransElement, "duration", (int)blend.nDuration);
				_SetAttribute( pAniTransElement, "blend_type", pAniMgr->m_AnimationBlendTable.GetTransTypeString( blend.eType));
			}
		}
	}

	return xml.SaveFile( szFileName);
}

bool CMcvView::EditAnimation(RAnimation* pAnimation)
{
	EditAnimationDlg dlg;

	dlg.m_strFileName = pAnimation->GetResource()->GetName().c_str();
	dlg.m_strName = pAnimation->GetAliasName().c_str();
	dlg.m_bLoop = (pAnimation->GetAnimationLoopType()==RAniLoopType_Loop);
	if(dlg.DoModal())
	{
		pAnimation->SetAliasName((char*)(LPCSTR)(dlg.m_strName));
		pAnimation->SetAnimationLoopType( dlg.m_bLoop ? RAniLoopType_Loop : RAniLoopType_HoldLastFrame );
		return true;
	}
	return false;
}

/*
void CMcvView::OnLbnDblclkAnimationList()
{
	int sel = m_AnimationListBox.GetCurSel();
	if(sel==LB_ERR) return;

	RAnimation* pAni = GetActor()->m_pMesh->m_AniMgr.m_NodeHashList[sel];

	if( pAni ) {
		if(EditAnimation(pAni))
		{
			m_AnimationListBox.DeleteString(sel);
			m_AnimationListBox.InsertString(sel,pAni->GetName());
			m_AnimationListBox.SetCurSel(sel);
		}
	}
}
*/

void CMcvView::OnTvnSelchangedTreeNodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

// 	CString strSelected = GetControlToolBar()->m_treeNodes.GetItemText(pNMTreeView->itemNew.hItem);
// 	RActorNode *pNode = GetActor()->GetActorNode((LPCSTR)strSelected);
// 	GetMain()->m_pSelectedActorNode = pNode;

	if(pNMTreeView->itemNew.hItem)
	{
		// 중복된 노드가 있을수 있으므로 인덱스로 선택한다
		int nIndex = GetModelInfoSidebar()->m_treeNodes.GetItemData(pNMTreeView->itemNew.hItem);
		RActorNode *pNode = GetActor()->GetActorNodes()[nIndex];
		GetMain()->m_pSelectedActorNode = pNode;
	}else
		GetMain()->m_pSelectedActorNode = NULL;

	UpdateNodeInfo();
}

void CMcvView::AddInfo(const char *pFormat, ...)
{
	char temp[256];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	GetModelInfoSidebar()->m_strNodeInfo.Append(temp);
}

void CMcvView::UpdateNodeInfo()
{
	GetModelInfoSidebar()->m_strNodeInfo.Empty();

	RActorNode* pNode = GetMain()->m_pSelectedActorNode;
	if(pNode)
	{
		RMeshNode* pMeshNode = pNode->m_pMeshNode;

		AddInfo("name : %s\r\n",pMeshNode->m_Name.c_str());
		AddInfo("face : %d\r\n",pMeshNode->m_nFaceCnt);

		bool bPointColor = pMeshNode->m_nPointColorCnt>0;
		if(bPointColor)
			AddInfo(" - has point color\r\n");

		AddInfo("material (%d) : ",pMeshNode->m_nMtrlCnt);
		for(int i=0;i<pMeshNode->m_nMtrlCnt;i++)
		{
			RMaterial* pMtrl = pMeshNode->GetMtrl(i);
			if(pMtrl)
				AddInfo("%s ",pMtrl->GetName().c_str());
			else
				AddInfo("<invalid> ");
		}
		AddInfo("\r\n");

		AddInfo("bone ref node count : %d\r\n",pNode->GetBoneRefCount());
		AddInfo("ref bones (%d) : ",pNode->m_refBones.size());
		for(unsigned int i=0;i<pNode->m_refBones.size();i++)
		{
			if(pNode->m_refBones[i]==NULL)
				AddInfo("<unknown>");
			else
				AddInfo("%s ",pNode->m_refBones[i]->m_pMeshNode->m_Name.c_str());
		}
		AddInfo("\r\n");
	}

	GetModelInfoSidebar()->UpdateData(FALSE);
}

void CMcvView::OnBnClickedCheckNode()
{
	GetControlToolBar()->UpdateData();
	UpdateNodes();
}

void CMcvView::ValidateModel()
{
	RActor* pActor = GetActor();
	for(int i=0;i<pActor->GetActorNodeCount();i++)
	{
		RActorNode* pActorNode = pActor->GetActorNodes()[i];
		if(!pActorNode->Validate())
		{
			char buffer[256];
			sprintf(buffer,"%s node 가 잘못되었습니다. node를 선택하여 정보를 확인해 주세요.",pActorNode->m_pMeshNode->m_Name.c_str());

			MessageBox(buffer);
		}
	}
}

void CMcvView::McvLog(const char* szText, MLogLevel nLevel)
{
//	CMcvView* pView = GetMainView();

	CEdit* pEdit = (CEdit*)((CMainFrame*)AfxGetMainWnd())->GetOutputBar()->GetDlgItem(IDC_EDIT_OUTPUT);

	int len = pEdit->GetWindowTextLength();
	pEdit->SetSel(len,len);

	char buffer[8192],*pBuf = buffer;
	const char* src = szText;
	_ASSERT(strlen(szText)<sizeof(buffer));

	while(*src!=0)	// edit control 이기 때문에 \n 을 \r\n 으로 바꾼다
	{
		if(*src=='\n')
		{
			*pBuf++ = '\r';
			*pBuf++ = '\n';
		}else
			*pBuf++ = *src;
		src++;
	}
	*pBuf=0;

	pEdit->ReplaceSel(buffer,false);
	pEdit->Invalidate(FALSE);
}
void CMcvView::OnShowCollision()
{
	GetMain()->m_bShowCollision = !GetMain()->m_bShowCollision;
//	GetActor()->SetRenderCollision(m_bShowCollision);
}

void CMcvView::OnUpdateShowCollision(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((int)GetMain()->m_bShowCollision );
}

void CMcvView::Resize(int x, int y)
{
	if(!GetDevice()) return;

	REngine::GetConfig().m_nWidth = x;
	REngine::GetConfig().m_nHeight = y;
	GetDevice()->ResetDevice();
}

void CMcvView::OnAlpha2pass()
{
	m_bAlpha2Pass = !m_bAlpha2Pass;
}

void CMcvView::OnAnimationBlend()
{
	if( m_AniBlendDlg.ShowWindow( SW_SHOWNA ) )
	{
		m_AniBlendDlg.ShowWindow( SW_HIDE );
	}
	else
	{
		m_AniBlendDlg.ShowWindow( SW_SHOW );
	}
}

void CMcvView::OnAniblendImmediate()
{
	m_AniBlendDlg.ChangeAnimationBlendType( RAT_IMMEDIATE_BLEND );
}

void CMcvView::OnAniblendDelayed()
{
	m_AniBlendDlg.ChangeAnimationBlendType( RAT_DELAYED_BLEND );
}

void CMcvView::OnAniblendCrossfade()
{
	m_AniBlendDlg.ChangeAnimationBlendType( RAT_CROSS_FADE );
}

void CMcvView::OnAniblendDelete()
{
	m_AniBlendDlg.ChangeAnimationBlendType( RAT_NONE );
}

void CMcvView::OnAniblendEditDuration()
{
	m_AniBlendDlg.ChangeAnimationBlendDuration();
}

void CMcvView::OnAniblendImmediateTargetAll()
{
	m_AniBlendDlg.ChangeAnimationBlendTypeAllTarget( RAT_IMMEDIATE_BLEND );
}

void CMcvView::OnAniblendDelayedTargetAll()
{
	m_AniBlendDlg.ChangeAnimationBlendTypeAllTarget( RAT_DELAYED_BLEND );
}

void CMcvView::OnAniblendCrossfadeTargetAll()
{
	m_AniBlendDlg.ChangeAnimationBlendTypeAllTarget( RAT_CROSS_FADE );
}

void CMcvView::OnAniblendDeleteTargetAll()
{
	m_AniBlendDlg.ChangeAnimationBlendTypeAllTarget( RAT_NONE );
}

void CMcvView::ChangeAnimationList( const char* name )
{
	RActor* pActor = GetActor();
	if( pActor )
	{
		RAnimation* pAni = pActor->m_pMesh->m_AniMgr.GetAnimation( name );
		ChangeAnimationListSub( pActor, pAni );
	}
}

void CMcvView::ChangeAnimationList( int index )
{
	RActor* pActor = GetActor();
	if( pActor )
	{
		RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[index];
		ChangeAnimationListSub( pActor, pAni );
	}
}

void CMcvView::ChangeAnimationListSub( RActor* pActor, RAnimation* pAni )
{
	ASSERT( pActor );

	if( !pAni )
		return;

	RAnimation* sourceAni = pActor->GetAnimation();
	if( sourceAni )
	{
		const char* source = sourceAni->GetAliasName().c_str();
		const char* target = pAni->GetAliasName().c_str();
		BLENDDATA aniBlend = m_AniBlendDlg.GetAnimationTransData( source, target );

		pActor->SetAnimation( pAni );

		switch( aniBlend.eType )
		{
		case RAT_NONE:
			mlog( "Animation Blend None [%s]->[%s].\n", source, target );
			break;

		case RAT_IMMEDIATE_BLEND:
			mlog( "Animation Blend Immediate [%s]->[%s].\n", source, target );
			break;

		case RAT_DELAYED_BLEND:
			mlog( "Animation Blend Delayed [%s]->[%s].\n", source, target );
			break;

		case RAT_CROSS_FADE:
			mlog( "Animation Blend Cross fade [%s]->[%s].\n", source, target );
			break;
		}
	}
	else
	{
		pActor->SetAnimation(pAni);
		mlog( "Animation Blend Default [%s].\n", pAni->GetAliasName().c_str() );
	}

	PlayAnimation();

	if( m_bFrameInfoDlg )
	{
		m_bFrameInfoDlg = false;
		OnAnimationinfo();
	}


	// 이벤트 추가
	GetAnimationSidebar()->UpdateEventList();
}

void CMcvView::OnMaterialdecaldlg()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_MaterialDecalDlg.ShowWindow(SW_SHOW);
}
