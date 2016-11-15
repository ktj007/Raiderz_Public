// VL_ToolBar.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "VL_ToolBar.h"

#include "RSceneNodeID.h"
//#include "CoreToolRS3.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperRender.h"
#include "WS_WorkSpaceManager.h"

#include "M_ToolCmdMgr.h"
#include "M_ToolLogicObjectManager.h"
#include "M_CommandBuffer.h"
#include "M_LogicObject_SceneNodes.h"
#include "M_LogicObjectCommandBuffer.h"

#include "VR_ZoneObjectNew.h"
#include "VL_SceneNodeList.h"
#include "VL_RelaxAll.h"
#include "VL_Objects.h"

#include "RTreeSceneNode.h"
#include "RTreeCollision.h"

// CCoreToolToolBar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCoreToolToolBar, CPaneDialog)
CCoreToolToolBar::CCoreToolToolBar(CWnd* pParent /*=NULL*/)
{
	//m_eMoveAxis_ = AXIS_X;
	//m_eRotateAxis_ = AXIS_X;
	m_eMoveAxis_ = TB_AXIS_XY;
	m_eRotateAxis_ = TB_AXIS_Z;
	m_eCurrentAxis_ = TB_AXIS_X;
}

CCoreToolToolBar::~CCoreToolToolBar()
{
}

const char* CCoreToolToolBar::ROTATION_DRAG_INFO_ID = "ro_drag";

BOOL CCoreToolToolBar::OnInitDialog(){
//	CInitDialogBar::OnInitDialogBar();

	//그룹버튼 초기화
	for (int i_btn =0; i_btn < WORKBENCH_STATE::STATE_NUM; i_btn++)
	{
		if( i_btn == WORKBENCH_STATE::OBJECT_NEW )
		{
			m_btnGroup[i_btn].setType(FooButton::hotCustomPushButtonMulti);
		}
		else
		{
			m_btnGroup[i_btn].setType(FooButton::hotCheckButton);
		}
		m_btnGroup[i_btn].addToGroup(_T("top_1"));
	}

	//축 hot radio button 초기화 - 그룹버튼을 흉내낼뿐 사실은 그냥 버튼
	for (int i_btn =0; i_btn < TB_AXIS_BTN_NUM; i_btn++)
		m_btnGroupAxis_[i_btn].setType(FooButton::hotCustomRadioButton);

	m_btnGroup[WORKBENCH_STATE::TERRAIN_HEIGHT].setBitmapId(IDB_TERRAIN_HEIGHT);
	m_btnGroup[WORKBENCH_STATE::TERRAIN_AVER].setBitmapId(IDB_TERRAIN_AVER);
	m_btnGroup[WORKBENCH_STATE::TERRAIN_FLAT].setBitmapId(IDB_TERRAIN_FLAT);
	m_btnGroup[WORKBENCH_STATE::TERRAIN_SPLAT].setBitmapId(IDB_TERRAIN_SPLIT);
	m_btnGroup[WORKBENCH_STATE::TERRAIN_RELAX].setBitmapId(IDB_TERRAIN_RELAX);
	m_btnGroup[WORKBENCH_STATE::TERRAIN_GRASS].setBitmapId(IDB_TERRAIN_GRASS);

	m_btnGroup[WORKBENCH_STATE::OBJECT_NEW].setBitmapId(IDB_OBJ_NEW);
	m_btnGroup[WORKBENCH_STATE::OBJECT_SELECT].setBitmapId(IDB_OBJ_SELECT);
	m_btnGroup[WORKBENCH_STATE::OBJECT_MOVE].setBitmapId(IDB_OBJ_MOVE);
	m_btnGroup[WORKBENCH_STATE::OBJECT_ROTATE].setBitmapId(IDB_OBJ_ROTATE);
	m_btnGroup[WORKBENCH_STATE::OBJECT_SCALE].setBitmapId(IDB_OBJ_SCALE);

	m_btnGroup[WORKBENCH_STATE::ETC_PATH_SELECT].setBitmapId(IDB_ETC_PATH_FIND);
	m_btnGroup[WORKBENCH_STATE::ETC_CAMERA].setBitmapId(IDB_ETC_CAMERA);
	m_btnGroup[WORKBENCH_STATE::ETC_MAP_VIEW].setBitmapId(IDB_ETC_MAP_VIEW);
	m_btnGroup[WORKBENCH_STATE::ETC_DEV].setBitmapId(IDB_ETC_DEV);
	m_btnGroup[WORKBENCH_STATE::ETC_DECAL].setBitmapId(IDB_ETC_DEV);

	//m_btnGroup[0].check (true); //초기화는 메인프레임이

	//그룹버튼이 아닌것 초기화
	//new / open / save
	m_btnNew.setType(FooButton::hotPushButton);
	m_btnNew.setBitmapId(IDB_FILE_NEW);
	m_btnOpen.setType(FooButton::hotPushButtonMulti);
	m_btnOpen.setBitmapId(IDB_FILE_OPEN);
	m_btnSave.setType(FooButton::hotPushButton);
	m_btnSave.setBitmapId(IDB_FILE_SAVE);

	// undo
	m_btnUndo.setType(FooButton::hotPushButton);
	m_btnUndo.setBitmapId(IDB_CMD_UNDO);

	// local axis
	m_btnLocalAxis.setType(FooButton::hotCheckButton);

	// On Terrain
	m_btnOnTerrain.setType(FooButton::hotPushButton);
	m_btnOnTerrain.setBitmapId(IDB_OBJ_ON_TERRAIN);

	// Object Lock
	m_btnObjectLock.setType(FooButton::hotCheckButton);
	m_btnObjectLock.setBitmapId(IDB_OBJ_LOCK);

	// Object List
	m_btnObjectList.setType(FooButton::hotPushButton);
	m_btnObjectList.setBitmapId(IDB_OBJ_LIST);

	// Layer Manager
	m_btnLayerManager.setType(FooButton::hotPushButton);
	m_btnLayerManager.setBitmapId(IDB_LAYER_MANAGER);

	m_btnClip.setType(FooButton::hotPushButton);
	m_btnClip.setBitmapId(IDB_ETC_CLIPPING);

	// weather and light( global directional light )
	m_btnWeather.setType(FooButton::hotPushButton);
	m_btnWeather.setBitmapId(IDB_ENV_WEATHER);

	m_ctlToolTip_.Create(this);
	m_tooltipFont_.CreatePointFont(90, "굴림");
	m_ctlToolTip_.SetFont(&m_tooltipFont_);

	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_FILE_NEW), _T("New File"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_FILE_OPEN), _T("Open File(Ctrl + O)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_FILE_SAVE), _T("Save File(Ctrl + S)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_CMD_UNDO), _T("Undo (Undo-List: F1)"));

	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_HEIGH), _T("Terrain Height(H)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_AVER), _T("Terrain Average(G)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_FLAT), _T("Terrain Flat(V)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_SPLAT), _T("Texture Brush(B)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_RELAX), _T("Terrain Relax(L)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_GRASS), _T("Grass Brush(N)"));

	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_OBJ_NEW), _T("New Object"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_SELECT), _T("Select Obj(T)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_MOVE), _T("Move Obj(Q)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_ROTATE), _T("Rotate Obj(R)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_SCALE), _T("Scale Obj(E)"));

	m_ctlToolTip_.AddTool(GetDlgItem(IDC_AXIS_X), _T("Use X-Axis(F5)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_AXIS_Y), _T("Use Y-Axis(F6)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_AXIS_Z), _T("Use Z-Axis(F7)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_AXIS_XYZ), _T("Use Multi-Axis(F8)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_AXIS_LOCAL), _T("Local Axis edit"));

	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_WEATHER), _T("Weather Info"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_OBJECT_LIST), _T("Object List"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_LAYER_MANAGER), _T("Layer Manager"));

	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_OBJ_ON_TERRAIN), _T("Paste Obj on Terrain(Z)"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_LOCK), _T("Lock Selected Objs - Edit Selected Objs(C)"));

	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_PATH_FIND), _T("Pathfinding"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_ETC_MAP_VIEW), _T("Map Info"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_ETC_DEV), _T("For Devs"));
	m_ctlToolTip_.AddTool(GetDlgItem(IDC_TOP_CLIP), _T("Clip"));

	return TRUE;
}

void CCoreToolToolBar::DoDataExchange(CDataExchange* pDX)
{
// 	CInitDialogBar::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TOP_FILE_NEW,  m_btnNew);
	DDX_Control(pDX, IDC_TOP_FILE_OPEN,  m_btnOpen);
	DDX_Control(pDX, IDC_TOP_FILE_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_TOP_CMD_UNDO,  m_btnUndo);

	// action
	DDX_Control(pDX, IDC_TOP_HEIGH, m_btnGroup[WORKBENCH_STATE::TERRAIN_HEIGHT]);
	DDX_Control(pDX, IDC_TOP_AVER, m_btnGroup[WORKBENCH_STATE::TERRAIN_AVER]);
	DDX_Control(pDX, IDC_TOP_FLAT, m_btnGroup[WORKBENCH_STATE::TERRAIN_FLAT]);
	DDX_Control(pDX, IDC_TOP_SPLAT, m_btnGroup[WORKBENCH_STATE::TERRAIN_SPLAT]);
	DDX_Control(pDX, IDC_TOP_RELAX, m_btnGroup[WORKBENCH_STATE::TERRAIN_RELAX]);
	DDX_Control(pDX, IDC_TOP_GRASS, m_btnGroup[WORKBENCH_STATE::TERRAIN_GRASS]);
	
	DDX_Control(pDX, IDC_TOP_OBJ_NEW, m_btnGroup[WORKBENCH_STATE::OBJECT_NEW]);
	DDX_Control(pDX, IDC_TOP_SELECT, m_btnGroup[WORKBENCH_STATE::OBJECT_SELECT]);
	DDX_Control(pDX, IDC_TOP_MOVE, m_btnGroup[WORKBENCH_STATE::OBJECT_MOVE]);
	DDX_Control(pDX, IDC_TOP_ROTATE, m_btnGroup[WORKBENCH_STATE::OBJECT_ROTATE]);
	DDX_Control(pDX, IDC_TOP_SCALE, m_btnGroup[WORKBENCH_STATE::OBJECT_SCALE]);

	DDX_Control(pDX, IDC_TOP_PATH_FIND, m_btnGroup[WORKBENCH_STATE::ETC_PATH_SELECT]);
	DDX_Control(pDX, IDC_TOP_ETC_CAMERA, m_btnGroup[WORKBENCH_STATE::ETC_CAMERA]);
	DDX_Control(pDX, IDC_TOP_ETC_MAP_VIEW, m_btnGroup[WORKBENCH_STATE::ETC_MAP_VIEW]);
	DDX_Control(pDX, IDC_TOP_ETC_DEV, m_btnGroup[WORKBENCH_STATE::ETC_DEV]);
	DDX_Control(pDX, IDC_TOP_ETC_DECAL, m_btnGroup[WORKBENCH_STATE::ETC_DECAL]);

	// object axis
	DDX_Control(pDX, IDC_AXIS_X, m_btnGroupAxis_[TB_AXIS_X]);
	DDX_Control(pDX, IDC_AXIS_Y, m_btnGroupAxis_[TB_AXIS_Y]);
	DDX_Control(pDX, IDC_AXIS_Z, m_btnGroupAxis_[TB_AXIS_Z]);
	DDX_Control(pDX, IDC_AXIS_XYZ, m_btnGroupAxis_[TB_AXIS_COMBO]);

	DDX_Control(pDX, IDC_AXIS_LOCAL, m_btnLocalAxis);
	DDX_Control(pDX, IDC_TOP_OBJ_ON_TERRAIN, m_btnOnTerrain);
	DDX_Control(pDX, IDC_TOP_LOCK, m_btnObjectLock);

	DDX_Control(pDX, IDC_TOP_OBJECT_LIST, m_btnObjectList);
	DDX_Control(pDX, IDC_TOP_LAYER_MANAGER, m_btnLayerManager);
	DDX_Control(pDX, IDC_TOP_WEATHER,  m_btnWeather);
	DDX_Control(pDX, IDC_TOP_CLIP, m_btnClip);
}

void CCoreToolToolBar::OnUpdateCmdUI(CCmdUI* pCmdUI)
{
	//foobar의 enable은 자체적으로 처리한다.
	//그러나 OnUpdateCmdUI를 호출해 주지 않으면 foobar 자체가 비활성된다.
	//pCmdUI->Enable(TRUE);
}

BEGIN_MESSAGE_MAP(CCoreToolToolBar, CPaneDialog)

	//활성화 하고싶은 자원
	ON_UPDATE_COMMAND_UI(IDC_TOP_FILE_NEW, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_FILE_OPEN, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_FILE_SAVE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_CMD_UNDO,  OnUpdateCmdUI)

	ON_UPDATE_COMMAND_UI(IDC_TOP_HEIGH, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_AVER, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_FLAT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_SPLAT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_RELAX, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_GRASS, OnUpdateCmdUI)

	ON_UPDATE_COMMAND_UI(IDC_TOP_OBJ_NEW, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_SELECT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_MOVE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_ROTATE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_SCALE, OnUpdateCmdUI)

	ON_UPDATE_COMMAND_UI(IDC_AXIS_X, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_AXIS_Y, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_AXIS_Z, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_AXIS_XYZ, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_AXIS_LOCAL, OnUpdateCmdUI)

	ON_UPDATE_COMMAND_UI(IDC_TOP_WEATHER,  OnUpdateCmdUI)

	ON_UPDATE_COMMAND_UI(IDC_TOP_OBJ_ON_TERRAIN, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_LOCK, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_OBJECT_LIST, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_LAYER_MANAGER, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_CLIP, OnUpdateCmdUI)	

	ON_UPDATE_COMMAND_UI(IDC_TOP_PATH_FIND, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_ETC_MAP_VIEW, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_ETC_CAMERA, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_ETC_DEV, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_TOP_ETC_DECAL, OnUpdateCmdUI)
    
	ON_BN_CLICKED(IDC_TOP_HEIGH, OnBnClickedTopHeigh)
	ON_BN_CLICKED(IDC_TOP_AVER, OnBnClickedTopAver)
	ON_BN_CLICKED(IDC_TOP_FLAT, OnBnClickedTopFlat)
	ON_BN_CLICKED(IDC_TOP_SPLAT, OnBnClickedTopSplat)
	ON_BN_CLICKED(IDC_TOP_WEATHER, OnBnClickedTopWeather)
	ON_BN_CLICKED(IDC_TOP_OBJ_NEW, OnBnClickedTopObjNew)
	ON_BN_CLICKED(IDC_TOP_SELECT, OnBnClickedTopSelect)
	ON_BN_CLICKED(IDC_TOP_MOVE, OnBnClickedTopMove)
	ON_BN_CLICKED(IDC_TOP_ROTATE, OnBnClickedTopRotate)
	ON_BN_CLICKED(IDC_TOP_SCALE, OnBnClickedTopScale)
	ON_BN_CLICKED(IDC_TOP_PATH_FIND, OnBnClickedTopPathFind)

	ON_BN_CLICKED(IDC_TOP_GRASS, &CCoreToolToolBar::OnBnClickedTopGrass)
	ON_BN_CLICKED(IDC_AXIS_X, &CCoreToolToolBar::OnBnClickedAxisX)
	ON_BN_CLICKED(IDC_AXIS_Y, &CCoreToolToolBar::OnBnClickedAxisY)
	ON_BN_CLICKED(IDC_AXIS_Z, &CCoreToolToolBar::OnBnClickedAxisZ)
	ON_BN_CLICKED(IDC_AXIS_XYZ, &CCoreToolToolBar::OnBnClickedAxisXyz)
	ON_BN_CLICKED(IDC_AXIS_LOCAL, &CCoreToolToolBar::OnBnClickedAxisLocal)
	ON_BN_CLICKED(IDC_TOP_RELAX, &CCoreToolToolBar::OnBnClickedTopRelax)
	ON_BN_CLICKED(IDC_TOP_CMD_UNDO, &CCoreToolToolBar::OnBnClickedTopCmdUndo)
	ON_BN_CLICKED(IDC_TOP_OBJ_ON_TERRAIN, &CCoreToolToolBar::OnBnClickedTopObjOnTerrain)
	ON_BN_CLICKED(IDC_TOP_LOCK, &CCoreToolToolBar::OnBnClickedTopLock)
	ON_BN_CLICKED(IDC_TOP_FILE_NEW, &CCoreToolToolBar::OnBnClickedTopFileNew)
	ON_BN_CLICKED(IDC_TOP_FILE_OPEN, &CCoreToolToolBar::OnBnClickedTopFileOpen)
	ON_BN_CLICKED(IDC_TOP_FILE_SAVE, &CCoreToolToolBar::OnBnClickedTopFileSave)
	ON_BN_CLICKED(IDC_TOP_ETC_MAP_VIEW, &CCoreToolToolBar::OnBnClickedTopEtcMapView)
	ON_BN_CLICKED(IDC_TOP_ETC_CAMERA, &CCoreToolToolBar::OnBnClickedTopEtcCamera)
	ON_BN_CLICKED(IDC_TOP_ETC_DEV, &CCoreToolToolBar::OnBnClickedTopEtcDev)
	ON_BN_CLICKED(IDC_TOP_ETC_DECAL, &CCoreToolToolBar::OnBnClickedTopEtcDecal)
	ON_BN_CLICKED(IDC_TOP_OBJECT_LIST, &CCoreToolToolBar::OnBnClickedTopObjectList)
	ON_BN_CLICKED(IDC_TOP_LAYER_MANAGER, &CCoreToolToolBar::OnBnClickedTopLayerManager)
	ON_BN_CLICKED(IDC_TOP_CLIP, &CCoreToolToolBar::OnBnClickedTopClip)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CCoreToolToolBar::Listen(CoreMessage& _message)
{
	if(_message.nID == VIEW_MESSAGE::DEVICE_CREATED)
	{
		OnInitDialog();
	}else
	if(_message.nID == VIEW_MESSAGE::WB_MOUSE_L_UP)
	{
		m_pControlMediator_->GetHelperRender()->RemoveDragInfoText(ROTATION_DRAG_INFO_ID);
	}
}

BOOL CCoreToolToolBar::PreTranslateMessage(MSG* pMsg)
{
	m_ctlToolTip_.RelayEvent(pMsg);
	return CPaneDialog::PreTranslateMessage(pMsg);
}

void CCoreToolToolBar::SetAxis(TOOLBAR_AXIS _type)
{
	if(_type == TB_AXIS_BTN_NUM) //알아서 찾아서 체크하는 자동모드
	{
		WORKBENCH_STATE::STATE wbstate = m_pControlMediator_->GetCurrentWBState();
		if(wbstate == WORKBENCH_STATE::OBJECT_MOVE)
		{
			m_eCurrentAxis_ = m_eMoveAxis_;
			SetAxisButton_(m_eCurrentAxis_);
		}
		else if(wbstate == WORKBENCH_STATE::OBJECT_ROTATE)
		{
			m_eCurrentAxis_ = m_eRotateAxis_;
			SetAxisButton_(m_eCurrentAxis_);
		}
		else
		{
			for (int i_btn =0; i_btn < TB_AXIS_BTN_NUM; i_btn++)
				m_btnGroupAxis_[i_btn].check(false);
		}
	}
	else if(_type == TB_AXIS_X)
	{
		m_btnGroupAxis_[TB_AXIS_X].check(true);
		OnBnClickedAxisX();
	}
	else if(_type == TB_AXIS_Y)
	{
		m_btnGroupAxis_[TB_AXIS_Y].check(true);
		OnBnClickedAxisY();
	}
	else if(_type == TB_AXIS_Z)
	{
		m_btnGroupAxis_[TB_AXIS_Z].check(true);
		OnBnClickedAxisZ();
	}
	else //콤보
	{
		_ASSERT( _type == TB_AXIS_XY || _type == TB_AXIS_YZ || _type == TB_AXIS_XZ );
		m_btnGroupAxis_[TB_AXIS_COMBO].check(true);
		UpdateButtonAxisXyz(_type);
	}
}

AXIS_STATE CCoreToolToolBar::GetCurrentAxisToAXIS_STATE()
{
	AXIS_STATE axis = AXIS_NULL;
	switch (m_eCurrentAxis_)
	{
		case TB_AXIS_X :
		{
			axis = AXIS_X;
			break;
		}
		case TB_AXIS_Y :
		{
			axis = AXIS_Y;
			break;
		}
		case TB_AXIS_Z :
		{
			axis = AXIS_Z;
			break;
		}
		case TB_AXIS_XY :
		{
			axis = AXIS_XY;
			break;
		}
		case TB_AXIS_YZ :
		{
			axis = AXIS_YZ;
			break;
		}
		case TB_AXIS_XZ :
		{
			axis = AXIS_XZ;
			break;
		}
		case TB_AXIS_BTN_NUM :
		{
			axis = AXIS_NULL;
			break;
		}
	}
	return axis;
}

// CTopBar 메시지 처리기입니다.

void CCoreToolToolBar::OnBnClickedTopHeigh()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::TERRAIN_HEIGHT, false);
}

void CCoreToolToolBar::OnBnClickedTopAver()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::TERRAIN_AVER, false);
}

void CCoreToolToolBar::OnBnClickedTopFlat()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::TERRAIN_FLAT, false);
}

void CCoreToolToolBar::OnBnClickedTopSplat()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::TERRAIN_SPLAT, false);
}

void CCoreToolToolBar::OnBnClickedTopWeather()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerSideTab("환경");
}

void CCoreToolToolBar::OnBnClickedTopObjNew()
{
	// 일단 편집 스테이트 변경
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_NEW, false);

	if (m_btnGroup[WORKBENCH_STATE::OBJECT_NEW].isMultiClicked())
	{
		if(m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
		{
			CMenu menu;
			menu.LoadMenu( IDR_MENU_NEW_OBJECT );
			CMenu* pPopupMenu = menu.GetSubMenu (0);
			_ASSERT(pPopupMenu != NULL);

			int nResult = m_btnGroup[WORKBENCH_STATE::OBJECT_NEW].displayPopupMenu( pPopupMenu );
			switch (nResult)
			{
			case ID_NEW_MESH_ACTOR :
				m_pControlMediator_->GetViewPtr<CRollupZoneObjectNew>()->SelectNewObjectListTab( 0 );
				break;

			case ID_NEW_SPEED_TREE :
				m_pControlMediator_->GetViewPtr<CRollupZoneObjectNew>()->SelectNewObjectListTab( 1 );
				break;

			case ID_NEW_WATER_OBJECT :
				m_pControlMediator_->GetViewPtr<CRollupZoneObjectNew>()->SelectNewObjectListTab( 2 );
				m_pControlMediator_->GetViewPtr<CRollupZoneObjectNew>()->OnBnClickedButtonEtcWaterPlaneCreate();
				break;
			}
		}
		else
		{
			m_pControlMediator_->GetHelperAction()->NotifyNewEntityToolBarButtonClicked();
		}

		m_btnGroup[WORKBENCH_STATE::OBJECT_NEW].check(true);
	}
}

void CCoreToolToolBar::OnBnClickedTopSelect()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_SELECT, false);
}

void CCoreToolToolBar::OnBnClickedTopMove()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_MOVE, false);
}

void CCoreToolToolBar::OnBnClickedTopRotate()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_ROTATE, false);
}

void CCoreToolToolBar::OnBnClickedTopScale()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_SCALE, false);
}

void CCoreToolToolBar::OnBnClickedTopGrass()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::TERRAIN_GRASS, false);
}

void CCoreToolToolBar::SetAxisButton_(TOOLBAR_AXIS _type)
{
	for (int i_btn =0; i_btn < TB_AXIS_BTN_NUM; i_btn++)
		m_btnGroupAxis_[i_btn].check(false);

	
	if(_type < TB_AXIS_BTN_NUM)
		m_btnGroupAxis_[(int)_type].check(true);
	else
	{
		if(_type == TB_AXIS_XY)
		{
			m_btnGroupAxis_[TB_AXIS_COMBO].check(true);
			m_btnGroupAxis_[TB_AXIS_COMBO].SetWindowText("XY");
		}
		else if(_type == TB_AXIS_XZ)
		{
			m_btnGroupAxis_[TB_AXIS_COMBO].check(true);
			m_btnGroupAxis_[TB_AXIS_COMBO].SetWindowText("XZ");
		}
		else if(_type == TB_AXIS_YZ)
		{
			m_btnGroupAxis_[TB_AXIS_COMBO].check(true);
			m_btnGroupAxis_[TB_AXIS_COMBO].SetWindowText("YZ");
		}
	}
}
void CCoreToolToolBar::OnBnClickedAxisX()
{
	WORKBENCH_STATE::STATE wbstate = m_pControlMediator_->GetCurrentWBState();
	if( wbstate == WORKBENCH_STATE::OBJECT_MOVE )
	{
		m_eMoveAxis_ = TB_AXIS_X;
		m_eCurrentAxis_ = TB_AXIS_X;
		SetAxisButton_(TB_AXIS_X);
	}
	else if (wbstate == WORKBENCH_STATE::OBJECT_ROTATE )
	{
		m_eRotateAxis_ = TB_AXIS_X;
		m_eCurrentAxis_ = TB_AXIS_X;
		SetAxisButton_(TB_AXIS_X);
	}
	else
		m_btnGroupAxis_[TB_AXIS_X].check(false);
}

void CCoreToolToolBar::OnBnClickedAxisY()
{
	WORKBENCH_STATE::STATE wbstate = m_pControlMediator_->GetCurrentWBState();
	if( wbstate == WORKBENCH_STATE::OBJECT_MOVE )
	{
		m_eMoveAxis_ = TB_AXIS_Y;
		m_eCurrentAxis_ = TB_AXIS_Y;
		SetAxisButton_(TB_AXIS_Y);
	}
	else if (wbstate == WORKBENCH_STATE::OBJECT_ROTATE )
	{
		m_eRotateAxis_ = TB_AXIS_Y;
		m_eCurrentAxis_ = TB_AXIS_Y;
		SetAxisButton_(TB_AXIS_Y);
	}
	else
		m_btnGroupAxis_[TB_AXIS_Y].check(false);
}

void CCoreToolToolBar::OnBnClickedAxisZ()
{
	WORKBENCH_STATE::STATE wbstate = m_pControlMediator_->GetCurrentWBState();
	if( wbstate == WORKBENCH_STATE::OBJECT_MOVE )
	{
		m_eMoveAxis_ = TB_AXIS_Z;
		m_eCurrentAxis_ = TB_AXIS_Z;
		SetAxisButton_(TB_AXIS_Z);
	}
	else if (wbstate == WORKBENCH_STATE::OBJECT_ROTATE )
	{
		m_eRotateAxis_ = TB_AXIS_Z;
		m_eCurrentAxis_ = TB_AXIS_Z;
		SetAxisButton_(TB_AXIS_Z);
	}
	else
		m_btnGroupAxis_[TB_AXIS_Z].check(false);
}

void CCoreToolToolBar::UpdateButtonAxisXyz(TOOLBAR_AXIS axis)
{
	WORKBENCH_STATE::STATE wbstate = m_pControlMediator_->GetCurrentWBState();
	if( wbstate == WORKBENCH_STATE::OBJECT_MOVE )
	{
		m_eMoveAxis_ = axis;
		m_eCurrentAxis_ = axis;
		SetAxisButton_(axis);
	}
	else
		m_btnGroupAxis_[TB_AXIS_COMBO].check(false);
}

void CCoreToolToolBar::OnBnClickedAxisXyz()
{
	CString btnText;
	TOOLBAR_AXIS axis;
	m_btnGroupAxis_[TB_AXIS_COMBO].GetWindowText(btnText);
	//예전에 선택이 안되어 있었다면
	if((int)m_eCurrentAxis_ < TB_AXIS_XY)
	{
		if(btnText.Compare("XY") == 0)
		{
			axis = TB_AXIS_XY;
		}
		else if(btnText.Compare("XZ") == 0)
		{
			axis = TB_AXIS_XZ;
		}
		else if(btnText.Compare("YZ") == 0)
		{
			axis = TB_AXIS_YZ;
		}
	}
	//선택이 되었었다면
	else
	{
		if(btnText.Compare("XY") == 0)
		{
			axis = TB_AXIS_XZ;
		}
		else if(btnText.Compare("XZ") == 0)
		{
			axis = TB_AXIS_YZ;
		}
		else if(btnText.Compare("YZ") == 0)
		{
			axis = TB_AXIS_XY;
		}
	}

	UpdateButtonAxisXyz( axis );
}

void CCoreToolToolBar::OnBnClickedTopRelax()
{
	m_pControlMediator_->GetViewPtr<CDlgRelaxAll>()->Listen( CoreMessage(VIEW_MESSAGE::ACTIVE_VIEW) );
}

void CCoreToolToolBar::OnBnClickedTopCmdUndo()
{
	m_pControlMediator_->GetToolCmdMgr()->UndoCommand();
}

// Z 키 눌러서 지형에 맞추어 주는 Scene관련 기능. 어딘가로 옮겨가는게 맞는듯, 
// 아니면 기본적으로 LogicObject 에서 지원해주는게 맞겠다
void CCoreToolToolBar::OnBnClickedTopObjOnTerrain()
{
	m_pControlMediator_->GetHelperAction()->NotifyEntityOnTerrainToolBarButtonClicked();

	// 현재 위치에서 Z 아래쪽으로 Pick 할때 가장 위에 있는 노드 혹은 터레인의 해당 위치를 알아낸다.
	rs3::RToolSceneManager* pSceneManager = m_pControlMediator_->GetToolSceneManager();
	if( pSceneManager == NULL )
		return;

	// 선택한것들이 쓸모있는것들인지
//	const SCENEOBJECTMAP *pSelectList = m_pControlMediator_->GetHelperAction()->GetSelectedSceneObject();
	LOGIC_OBJECTLIST* pSelectedLogicObject = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();
	ENTITY_OBJECTLIST* pSelectedEntityObject = m_pControlMediator_->GetHelperAction()->GetSelectedEntityObject();
	if ((m_pControlMediator_->m_mapInfo.m_bSceneEditableMap && pSelectedLogicObject->empty()) ||
		(!m_pControlMediator_->m_mapInfo.m_bSceneEditableMap && pSelectedEntityObject->empty()) )
		return;

	CGroupedCommandBuffer* pGroupCmdBuffer(NULL);
	rs3::RVector vPos;
	rs3::RVector vNormal;
	if(m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
	{
		for (LOGIC_OBJECTLIST::const_iterator it = pSelectedLogicObject->begin(); it != pSelectedLogicObject->end(); ++it )
		{
			CLogicObject* pLogicObject = *it;
			CLogicObject_SceneNode* pLogicObjectSceneNode = dynamic_cast<CLogicObject_SceneNode*> ( pLogicObject );
			if( pLogicObjectSceneNode == NULL)
				continue;

			rs3::RSceneNode *pSceneNode = pLogicObjectSceneNode->GetModelSceneNode();
			if ( pSceneNode == NULL )
				continue;

			vPos = pSceneNode->GetWorldPosition();
			const rs3::RVector* pNewPos = pSceneManager->GetVerticalContactPos( vPos, pSceneNode, &vNormal );
			if( pNewPos )
			{
				bool isPosChange = false;
				rs3::RVector changedPos;

				// 나무 오브젝트 위치 보정
				if( _stricmp( pSceneNode->GetNodeIDString(), rs3::RSID_TREE ) == 0 )
				{
					rs3::RTreeSceneNode* treeSceneNode = static_cast<rs3::RTreeSceneNode*>(pSceneNode);
					rs3::RTreeCollisionObject* treeCollCapsuleObj = treeSceneNode->GetTreeCollisionInfo( rs3::RTREECOLLISION_CAPSULE );
					if( treeCollCapsuleObj )
					{
						vNormal.z = 0.0f;
						vNormal.Normalize();
						rs3::RVector newPickingPos = *pNewPos + treeCollCapsuleObj->m_vPosition + vNormal * pSceneNode->GetScale() * treeCollCapsuleObj->m_fRadius;

						const rs3::RVector* pNewPos2 = pSceneManager->GetVerticalContactPos( newPickingPos, pSceneNode );
						if( pNewPos2 )
						{
							isPosChange = true;
							changedPos.x = pNewPos->x;
							changedPos.y = pNewPos->y;
							changedPos.z = pNewPos2->z;
						}
					}
					else
					{
						isPosChange = true;
						changedPos = *pNewPos;
					}
				}
				else
				if( vPos != *pNewPos )
				{
					isPosChange = true;
					changedPos = *pNewPos;
				}

				if( isPosChange )
				{
					CLogicMoveCmdBuffer* pCmdBuffer = new CLogicMoveCmdBuffer(pLogicObject, vPos);

					if( pGroupCmdBuffer == NULL )
						pGroupCmdBuffer = new CGroupedCommandBuffer;

					pGroupCmdBuffer->AddBuffer(pCmdBuffer);

					pLogicObject->SetPosition(changedPos);

					m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_MOVE, pLogicObject ) );
//					pSceneNode->SetPosition( changedPos );
				}
			}
		}
		m_pControlMediator_->GetHelperAction()->SetSelectedObjectCenterAndActionFrame();
		m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->ObserveSelection(NULL, true, false);

		if( pGroupCmdBuffer != NULL )
			m_pControlMediator_->GetToolCmdMgr()->RecordCommand( pGroupCmdBuffer );
	}

	return;
}

void CCoreToolToolBar::OnBnClickedTopLock()
{
	//체크가 안되어 있다면, 선택이 가능한 것이다.
	if(m_btnObjectLock.isChecked() == false)
	{
		if(m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
			m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->SetListLock(true);
		else
			m_pControlMediator_->GetHelperAction()->NotifySelectionLockToolBarButtonClicked(true);
	}
	else
	{
		if(m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
			m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->SetListLock(false);
		else
			m_pControlMediator_->GetHelperAction()->NotifySelectionLockToolBarButtonClicked(false);
	}
}

void CCoreToolToolBar::OnBnClickedTopFileNew()
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW_0);
}

void CCoreToolToolBar::OnBnClickedTopFileOpen()
{
	if (m_btnOpen.isMultiClicked())
	{
		CRect  rectButton;
		m_btnOpen.GetWindowRect(&rectButton);

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, ID_FILE_MRU_FILE1, "최근파일");

		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, rectButton.left, rectButton.bottom, AfxGetMainWnd());
		m_btnOpen.clearMultiClick();
	}
	else
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_OPENALL);
	}
}

void CCoreToolToolBar::OnBnClickedTopFileSave()
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_SAVEALL);
}

void CCoreToolToolBar::OnBnClickedTopPathFind()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::ETC_PATH_SELECT, false);
}

void CCoreToolToolBar::OnBnClickedTopEtcMapView()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::ETC_MAP_VIEW, false);
}

void CCoreToolToolBar::OnBnClickedTopEtcDev()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::ETC_DEV, false);
}

void CCoreToolToolBar::OnBnClickedTopEtcDecal()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::ETC_DECAL, false);
}

void CCoreToolToolBar::OnBnClickedTopEtcCamera()
{
	m_pControlMediator_->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::ETC_CAMERA, false);
}

void CCoreToolToolBar::OnBnClickedTopObjectList()
{
	m_pControlMediator_->GetViewPtr< CDlgSceneNodeList >()->ShowToggle();
}

void CCoreToolToolBar::OnBnClickedTopLayerManager()
{
	m_pControlMediator_->GetViewPtr< CDlgObjects >()->ShowToggle();
}

void CCoreToolToolBar::OnBnClickedTopClip()
{
	m_pControlMediator_->GetHelperRender()->ShowToggleClipPlane();
}

void CCoreToolToolBar::SetEnableAxis( bool bEnable )
{
	for (int i = 0; i < TB_AXIS_BTN_NUM; ++i)
	{
		m_btnGroupAxis_[i].enable(bEnable);
	}
}

void CCoreToolToolBar::OnDestroy()
{
	__super::OnDestroy();

	m_ctlToolTip_.DestroyWindow();
}

void CCoreToolToolBar::OnBnClickedAxisLocal()
{
	m_pControlMediator_->GetHelperAction()->SetSelectedObjectCenterAndActionFrame();
}
