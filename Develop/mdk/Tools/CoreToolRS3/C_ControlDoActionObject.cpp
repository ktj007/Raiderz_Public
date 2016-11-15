#include "StdAfx.h"
#include "C_ControlDoAction.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperResource.h"
#include "C_ControlMediatorHelperRender.h"

#include "M_ToolLogicObjectManager.h"
#include "M_ToolCmdMgr.h"
#include "M_CommandBuffer.h"
#include "M_LogicObjectCommandBuffer.h"
#include "M_ToolSceneNodeInstance.h"
#include "M_LogicObject_SceneNodes.h"

#include "C_XMLConfig.h"
#include "C_ControlCursor.h"
#include "C_StatusBarHandler.h"

#include "VL_SceneNodeList.h"
#include "VR_ZoneObjectNew.h"
#include "VL_ToolBar.h"
#include "VG_Gizmo.h"
#include "VL_Objects.h"

#include "C_XMLEditInfoFile.h"
#include "RUtil.h"

#include "ext_lib/FooButton.h"

#include "WS_WorkSpaceManager.h"
#include "Gizmo.h"
#include "MainFrm.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

using namespace rs3;

//edit 모드가 아닌 다른 액션들은 미리 액션을 행해준다.(pre-action)
const CControlDoAction::OBJECT_EDIT_TYPE CControlDoAction::GetObjectEditTypeAndPreAction(CControlMediator* _pController)
{
	static OBJECT_EDIT_TYPE edit_type = EDIT_INIT;

	BOOL isLocked = _pController->GetViewPtr<CCoreToolToolBar>()->m_btnObjectLock.isChecked();

	// 현재 커서위치에 기즈모가 있는것인지 여부체크
	VG_Gizmo* pGizmo = _pController->GetViewPtr<VG_Gizmo>();
	bool bGizmoSelected = ( pGizmo->GetEnable() && (pGizmo->GetGizmoState() != AXIS_NULL) );

	//ctrl키가 눌려있고 선택 잠금이 안되어 있다면
	if(GetAsyncKeyState(VK_LCONTROL) && isLocked == FALSE)
	{
		edit_type = OBJECT_SELECT;
	}
	//드래그일때
	else if(_pController->m_mouseInfo.mouseState.bDrag)
	{
		//shift 드래그 - 복사
		if(	GetAsyncKeyState(VK_SHIFT) )
		{
			rs3::RSceneNode* pSelectNodeAtMouse = NULL;
			CLogicObject* pLogicObject = NULL;
			//복사하려는 물체를 선택했는지 확인
			_pController->GetHelperAction()->GetWBObjectAtCurrentMouse(&pSelectNodeAtMouse, &pLogicObject);

			// logic object 카피로 변경중
			if( (pLogicObject != NULL &&
				_pController->GetHelperAction()->IsInSelectObjectList( pLogicObject) ) || //선택한물체
				bGizmoSelected || isLocked)
				edit_type = OBJECT_COPY;

			// TODO: 삭제
// 			if(_pController->m_mapInfo.m_bEditSceneObject)
// 			{
// 				//물체를 선택해서 복사하거나 잠거졌을때 잠궈진 물체만 복사
// 				if( (pSelectNodeAtMouse != NULL &&
// 					_pController->GetHelperAction()->IsInSelectObjectList(pSelectNodeAtMouse, pLogicObject) ) || //선택한물체
// 					isLocked)
// 					edit_type = OBJECT_COPY;
// 			}
		}
	}
	//마우스 버튼 다운일때
	else if(_pController->m_mouseInfo.mouseState.bLButton)
	{
		//물체가 잠겨 있다면, 무조건 편집모드
		if(isLocked == TRUE)
		{
			// 기즈모와 상관없이 기즈모의 위치를 액션플랜 포지션으로 설정
			//if(bGizmoSelected)
			{
				// action plane position 을 설정
				RVector vecPosition = pGizmo->GetGizmoPosition();
				_pController->GetHelperAction()->SetActionPlanePosAs( vecPosition );
			}
			edit_type = OBJECT_EDIT;
			return OBJECT_EDIT;
		}

		// 기즈모를 선택했다면 무조건 편집모드
		if(bGizmoSelected)
		{
			RVector vecPosition = pGizmo->GetGizmoPosition();
			_pController->GetHelperAction()->SetActionPlanePosAs( vecPosition );

			edit_type = OBJECT_EDIT;
			return OBJECT_EDIT;
		}

		//잠겨있지 않다면 현재 커서 위치에 있는 오브젝트를 얻는다
		rs3::RSceneNode* pSelectNodeAtMouse = NULL;
		CLogicObject* pLogicObject = NULL;
		_pController->GetHelperAction()->GetWBObjectAtCurrentMouse(&pSelectNodeAtMouse, &pLogicObject);

		// 현재 커서 위치에 있는 오브젝트가 없으면 선택 해제
		if(pSelectNodeAtMouse == NULL && pLogicObject == NULL && !bGizmoSelected )
		{
			_pController->GetHelperAction()->RemoveAllSelection(true);
			edit_type = OBJECT_SELECT;
		}
		else if( !GetAsyncKeyState(VK_SHIFT) ) // 드래그 복사를 위한 쉬프트가 아니라면
		{
			// 현재 커서 위치에 있는것이 이미 선택되어있는 아니라면 새로 선택 갱신
			if ( _pController->GetHelperAction()->IsInSelectObjectList( pLogicObject) == false && !bGizmoSelected)
			{
				_pController->GetHelperAction()->RemoveAllSelection(true);
				CControlDoAction::ObjectSelect(_pController);
			}

			//클릭후에 드래그 하면 바로 오브젝트 에디트가 될 수 있도록
			edit_type = OBJECT_EDIT;

			if(bGizmoSelected)
			{
				// action plane position 을 설정
				RVector vecPosition = pGizmo->GetGizmoPosition();
				_pController->GetHelperAction()->SetActionPlanePosAs( vecPosition );
			}
		}
	}

	//selection 일때 수행하기
	if(edit_type == OBJECT_SELECT)
	{
		CControlDoAction::ObjectSelect(_pController);
	}
	//copy 모드일때
	else if(edit_type == OBJECT_COPY)
	{
		//엔트리 테스트, 마우스 커맨드 엔트리일때는 복사
		if(_pController->GetToolCmdMgr()->IsMouseCmdEntryL())
		{
//			bool bCopySucceed = _pController->GetHelperAction()->CopyDeepSelectedZoneObject();
			bool bCopySucceed = _pController->GetHelperAction()->CopyDeepSelectedObject();
			//복사 실패
			if(!bCopySucceed)
			{
				//상태를 물체 편집상태로 바꿔준다.
				edit_type = OBJECT_EDIT;
			}	
			//Scale State 였다면 Move State 로 바꿔준다.
			else
			{
				if(_pController->GetCurrentWBState() == WORKBENCH_STATE::OBJECT_SCALE)
					_pController->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_MOVE, true);

				// move observer 등록
				_pController->GetHelperAction()->AddToWBMouseObserver( CControlMediatorHelperAction::WB_MOUSE_L_UP, -1);
			}
			_pController->GetToolCmdMgr()->DoneMouseCmdL();
		}
		else
			edit_type = OBJECT_EDIT;
	}
	return edit_type;
}

bool CControlDoAction::IsEditableSceneNode( rs3::RSceneNode* pSceneNode )
{
	if ( pSceneNode->GetUserData() == NULL )
		return false;

	return true;
}

void CControlDoAction::UpdateDragText(CControlMediator* _pController, const char* szDragText)
{
	VG_Gizmo* pGizmo = _pController->GetViewPtr<VG_Gizmo>();
	Gizmo* pCurrentGizmo = pGizmo->GetCurrentGizmo();

	if( pCurrentGizmo == NULL ) return;

	// 2D 좌표 구한다.
	MVector2 vScreenPos = _pController->GetToolCamera()->GetScreenPosition( pCurrentGizmo->GetPosition() );
	
	vScreenPos.x -= 25;
	vScreenPos.y -= 85;

	_pController->GetHelperRender()->GetDragInfoText( CCoreToolToolBar::ROTATION_DRAG_INFO_ID,
		vScreenPos.x, vScreenPos.y) = szDragText;
}

bool CControlDoAction::MakeRotationMatrixAndInfoText(rs3::RMatrix& _rOut, CControlMediator* _pController, UINT nAxis)
{
	char szRotationInfo[256] = {0, };
	CCoreToolToolBar* pTopBar =  _pController->GetViewPtr<CCoreToolToolBar>();
	_rOut.MakeIdentity();

	float fDegree = (float)(_pController->m_mouseInfo.m_currentMouseY - _pController->m_mouseInfo.m_orgMouseY);

	VG_Gizmo* pGizmo = _pController->GetViewPtr<VG_Gizmo>();
	Gizmo* pCurrentGizmo = pGizmo->GetCurrentGizmo();

	// 2D 좌표 구한다.
	MVector2 vScreenPos = _pController->GetToolCamera()->GetScreenPosition( pCurrentGizmo->GetPosition() );

	// 드래그 방향이 반대면 반대방향으로 돌린다 ?
	if( vScreenPos.x < _pController->m_mouseInfo.m_orgMouseX )		// 오른쪽 클릭
		fDegree *= -1;

	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	int nAngleSnapSize = pMainFrame->GetAngleSnapSize();
	if( nAngleSnapSize > 0 )
		fDegree = (int)((fDegree + .5f * float(nAngleSnapSize)) / nAngleSnapSize) * nAngleSnapSize;

	const RMatrix& localFrame = _pController->GetHelperAction()->GetSelectedObjectActionFrame();
	RVector vBaseXAxis(localFrame._11, localFrame._12, localFrame._13);
	RVector vBaseYAxis(localFrame._21, localFrame._22, localFrame._23);
	RVector vBaseZAxis(localFrame._31, localFrame._32, localFrame._33);

	switch ( nAxis )
	{
	case MOVEAXIS_X :
		_rOut.SetRotationMatrixAxis(vBaseXAxis, DegreeToRadian(fDegree));
		sprintf(szRotationInfo, "Rotation X : %.2f", fDegree);
		break;
	case MOVEAXIS_Y :
		_rOut.SetRotationMatrixAxis(vBaseYAxis, DegreeToRadian(fDegree));
		sprintf(szRotationInfo, "Rotation Y : %.2f", fDegree);
		break;
	case MOVEAXIS_Z :
		_rOut.SetRotationMatrixAxis(vBaseZAxis, DegreeToRadian(fDegree));
		sprintf(szRotationInfo, "Rotation Z : %.2f", fDegree);
		break;
	default:
		return false;
	}

	UpdateDragText( _pController, szRotationInfo );

	return true;
}

using namespace rs3;

rs3::RMatrix ns_matRotate;
std::map< DWORD_PTR, DWORD_PTR > ns_mapObjRotOrg;
rs3::RVector ns_vecPosition;
std::map< DWORD_PTR, DWORD_PTR > ns_mapObjPosOrg;


void CControlDoAction::ObjectNew(CControlMediator* _pController)
{
	// PreAction
	CDoActionObjectNew* pObjectNewAction = _pController->GetWorkSpaceMgr()->GetDoActionObjectNew();
	if (pObjectNewAction && pObjectNewAction->DoPreAction(_pController))
		return;

	RToolSceneManager* pSceneManager = _pController->GetToolSceneManager();
	if( pSceneManager == NULL )
		return;

	static string beforeString(""); //커서에 현재 메시 붙이기
	std::string *pStrCurrentFile = NULL;
	std::string *pStrCurrentID = NULL;
	std::string *pStrCurrentPathName = NULL;

	static LONG beforeX = -1;
	static LONG beforeY = -1;

	//for logic
	static CLogicObject* pLogicReferenceObject = NULL;
	static CLogicObject* pLogicObjectCurrentSawed = NULL;
	if (false == _pController->m_mapInfo.m_bEditSceneObject && NULL == pObjectNewAction)
		return;

	//zone 일 이면서, Entity 탭이 선택되어 있지 않은 경우 : Scene 편집모드
	if (_pController->m_mapInfo.m_bEditSceneObject)
	{
		pStrCurrentFile = &_pController->GetViewPtr<CRollupZoneObjectNew>()->m_strCurrentFile;
		pStrCurrentID = &_pController->GetViewPtr<CRollupZoneObjectNew>()->m_strCurrentType;
		pStrCurrentPathName = &_pController->GetViewPtr<CRollupZoneObjectNew>()->m_strFullPathName;
	}
	//field 이거나, 혹은 zone 인데 Entity 탭이 선택된 경우
	else
	{
		_ASSERT(pObjectNewAction);
		pLogicReferenceObject = pObjectNewAction->OnGetReferenceObjectAndSetCursor(_pController, pLogicReferenceObject);
		if (pLogicReferenceObject)
			_pController->m_mouseInfo.m_bPopAble = false;
	}

	// RButton 이 눌러져 있으면, Unselect 상태로 만든다.
	if ( _pController->m_mouseInfo.mouseState.bRButton || GetAsyncKeyState(VK_ESCAPE) )
	{
		_pController->m_mouseInfo.m_bPopAble = true;
		_pController->m_mouseInfo.mouseState.bRButton = false;

		if(_pController->m_mapInfo.m_bEditSceneObject)
		{
			//선택 초기화
			beforeString = "";
			*pStrCurrentFile = "";
			*pStrCurrentID = "";

			//rollback action
			const WORKBENCH_STATE::STATE wbstate = _pController->GetPreviousWBState();
			if( wbstate == WORKBENCH_STATE::OBJECT_SELECT || wbstate == WORKBENCH_STATE::OBJECT_MOVE ||
				wbstate == WORKBENCH_STATE::OBJECT_ROTATE || wbstate == WORKBENCH_STATE::OBJECT_SCALE )
				_pController->GetWorkSpaceMgr()->TriggerWBState(wbstate, true);
			_pController->GetViewPtr<CRollupZoneObjectNew>()->NoneSelect();
		}
		else //Logic
		{
			pLogicReferenceObject = NULL;
			pLogicObjectCurrentSawed = NULL;
			_pController->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_SELECT, true);
		}

		L3DCursor::GetInstance()->SetType(L3DCursor::NONE, false);
		return;
	}

	const RVector* pMousePosVec = _pController->GetHelperAction()->GetCurrentPickingPosition();

	RVector vMousePosition = pMousePosVec ? *pMousePosVec : RVector(0,0,0);
	bool bIsVaildNavigation = true;
	if (_pController->m_mapInfo.m_bEditSceneObject)
	{
	}
	else	// field 일 때 
	{
		_ASSERT(pObjectNewAction);
		bIsVaildNavigation = pObjectNewAction->OnCheckValidLocatePosition(_pController, vMousePosition);
	}

	//현재 새로 심으려고 선택한 new 파일이 없다면 선택모드로 들어간다.
	if( _pController->m_mouseInfo.mouseState.bEvented || _pController->m_mouseInfo.mouseState.bDrag)
	{
		bool bSelectedForNewObject = true;
		if(_pController->m_mapInfo.m_bEditSceneObject)
		{
			//zone 모드일 경우, pStrCurrentFile로 현재 파일 선택여부를 확인할 수 있다.
			if(pStrCurrentFile->empty())
				bSelectedForNewObject = false;
		}
		else
		{
			//field 모드일 경우, pEntityObject 의 존재 여부로 현재 선택여부를 알 수 있다.
			if(pLogicReferenceObject == NULL)
				bSelectedForNewObject = false;
		}

		//현재 새로 심을 어떤 선택도 되어 있지 않다면,
		if(bSelectedForNewObject == false)
		{
			//다른곳을 그냥 클릭했을때는 초기화 해준다
			if ( !GetAsyncKeyState(VK_LCONTROL) && _pController->m_mouseInfo.mouseState.bEvented &&
				_pController->m_mouseInfo.mouseState.bLButton && !_pController->m_mouseInfo.mouseState.bDrag)
				_pController->GetHelperAction()->RemoveAllSelection(true);

			CControlDoAction::ObjectSelect(_pController);
			//콜백액션인 ObjectSelect 액션을 OnTime액션인 new에서 부르기 때문에 해주는 작업들
			_pController->m_mouseInfo.mouseState.bEvented = false;
			return;
		}

		//Field, Entity Object 단후에 바로 물체편집
		if( !_pController->m_mapInfo.m_bEditSceneObject && _pController->m_mouseInfo.mouseState.bDrag )
		{
			if(pLogicObjectCurrentSawed != NULL)
			{
				_ASSERT(pObjectNewAction);
				pObjectNewAction->OnEditSawedObject(_pController, pLogicObjectCurrentSawed);
				_pController->m_mouseInfo.m_movedMouseX = 0; //moved 초기화
			}
		}
	}

	//선택이 바뀌었다면 커서에 달린 물체를 바꿔준다.
	if (_pController->m_mapInfo.m_bEditSceneObject && beforeString != *pStrCurrentFile && !pStrCurrentFile->empty() )
	{
		//다음번 오른쪽 버튼은 팝업메뉴가 아니라 new 캔슬이어야 한다.
		_pController->m_mouseInfo.m_bPopAble = false;
		// 커서에 현재 오브젝트를 생성한다.
		L3DCursor::GetInstance()->SetNodeFile( pStrCurrentFile->c_str(), pStrCurrentID->c_str());
		L3DCursor::GetInstance()->SetType(L3DCursor::NODE, false);
		L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(250, 0, 250));
		beforeString = *pStrCurrentFile;
	}

	//이동일때만 마우스에 달려있는 노드 업데이트
	// View 영역 안에 있을 때에만, Mouse 좌표가 갱신되기 때문에 - 이렇게 판단할 수 있다.
	if ( _pController->m_mouseInfo.m_currentMouseX != beforeX || _pController->m_mouseInfo.m_currentMouseY != beforeY )
	{
		if(_pController->m_mapInfo.m_bEditSceneObject)
		{
			L3DCursor::GetInstance()->SetTransform( vMousePosition, RVector(1, 1, 1));
		}
		else
		{
			// 커서 포시션. 다른 포지션에 영향을 미치지 않으며 오로지 커서에만 영향을 미친다.
			RVector vPos(vMousePosition);
			vPos.z += 150.f;
			L3DCursor::GetInstance()->SetTransform(vPos, RVector(100, 100, 400));
		}
	}

	// 배경 위에 객체를 생성해 올려놓기
	CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
	if( pCommandManager->IsMouseCmdEntryL())
	{
		if(_pController->m_mapInfo.m_bEditSceneObject)
		{
			if(pSceneManager == NULL)
				return;

			RSceneNode *pCurrentNode = L3DCursor::GetInstance()->GetNode();
			if ( pCurrentNode == NULL )
				return;

			//////////////////////////////////////////////////////////////////////////
			// 복사
			RSceneNode *pNode = pCurrentNode->Clone();	// 새 복사 인스턴스 생성
			if( pNode == NULL )
				return;

			pSceneManager->AddSceneNode(pNode, true);

			//////////////////////////////////////////////////////////////////////////
			// update
			RSceneNodeUpdateVisitor visitor;
			pNode->Traverse(&visitor);

			CLogicObject* pLogicObject = pSceneManager->CreateLogicObject( pNode );
			if( !pLogicObject )
			{
				pNode->RemoveFromParent();
				delete pNode;
				return;
			}

			CDlgObjects* pDlgObjects = _pController->GetViewPtr<CDlgObjects>();
			pLogicObject->SetLayerName( pDlgObjects->GetDefaultLayerName() );

			
			// 복사후 세팅
			CControlSceneProperty* pProperty = pLogicObject->GetProperty();
			if(pProperty)
				pProperty->SetNewInstance(pNode, pCurrentNode, false);

			// 이름 다시 복사, CreateLogicObject 일때 이름이 없다가 SetNewInstance 때 이름을 붙여주기 때문에
			// LogicObject 쪽으로 전파가 안된 상태
			
			// TODO: 생성이 일반화 되고나면 이것포함 SceneNode 생성부분 전체를 없애자
			pLogicObject->SetObjectName( pNode->GetNodeName().c_str() );

			//////////////////////////////////////////////////////////////////////////
			// SceneNodeInstance 이고 As Copy 이면 Convert해준다
			RToolSceneNodeInstance* pSceneInstance = dynamic_cast<RToolSceneNodeInstance*>(pNode);
			CRollupZoneObjectNew* pNewView = _pController->GetViewPtr<CRollupZoneObjectNew>();
			if( pSceneInstance && pNewView->IsCreatingAsInstance() == false)
			{
				RSceneNodeList nodeList;

				// 여기서 addtomanager
				pSceneManager->MakeSceneNodeInstanceEditable( pSceneInstance, &nodeList );

				//////////////////////////////////////////////////////////////////////////
				//Undo Buffer
				CGroupedCommandBuffer* pGroupCmdBuffer = new CGroupedCommandBuffer;
				for(RSceneNodeList::iterator itr = nodeList.begin(); itr != nodeList.end(); ++itr )
				{
					CObjectNewCmdBuffer* pCmdBuffer = new CObjectNewCmdBuffer( pSceneManager, *itr );
					pGroupCmdBuffer->AddBuffer( pCmdBuffer );
				}

				pCommandManager->RecordCommand(pGroupCmdBuffer);

				// 방금 생성한것들을 선택
				for( RSceneNodeList::iterator itr = nodeList.begin(); itr != nodeList.end(); ++itr )
				{
					RSceneNode* pEachNode = *itr;
					CLogicObject_SceneNode* pEachLogicObject = dynamic_cast<CLogicObject_SceneNode*>((CCoreObject*)pEachNode->GetUserData());
					_pController->GetHelperAction()->AddSelectection( pEachLogicObject, true );
				}

			}else
			{
				_pController->GetToolLogicObjectMgr()->AddToManager( pLogicObject );

				//////////////////////////////////////////////////////////////////////////
				//Undo Buffer
				CObjectNewCmdBuffer* pCmdBuffer = new CObjectNewCmdBuffer(pSceneManager, pNode);
				pCommandManager->RecordCommand(pCmdBuffer);

				// 방금 생성한것을 선택
				_pController->GetHelperAction()->AddSelectection( pLogicObject, true );
			}


			//////////////////////////////////////////////////////////////////////////
			// cursor 노드를 다시 생성한다
			RSceneNode* pNewCursorNode = pCurrentNode->Clone();
			pNewCursorNode->SetUserData(pCurrentNode->GetUserData());

			// SceneNode ListBox 에 커서 노드가 나오기 때문에 지운다
			L3DCursor::GetInstance()->SetNode(NULL);

			// ListBox를 업데이트 시킨다.
			_pController->GetViewPtr<CDlgSceneNodeList>()->UpdateTreeControl();


			L3DCursor::GetInstance()->SetNode( pNewCursorNode );	// 숨겨놨던 커서 다시 생성
		}
		else	// Logic 편집 모드일 때
		{
			if(pLogicReferenceObject == NULL)
				return;

			pLogicObjectCurrentSawed = pLogicReferenceObject->Clone();
			if(pLogicObjectCurrentSawed)
			{
				//give unique name to new object
				//const char* szUniqueName = _pController->GetToolLogicObjectMgr()->GetUniqueObjectNameOf( pLogicReferenceObject->GetObjectName() );
				//pLogicObjectCurrentSawed->SetObjectName( szUniqueName );
				pLogicObjectCurrentSawed->SetObjectName( pLogicReferenceObject->GetObjectName() );

				pLogicObjectCurrentSawed->SetPosition( vMousePosition );

				CDlgObjects* pDlgObjects = _pController->GetViewPtr<CDlgObjects>();
				pLogicObjectCurrentSawed->SetLayerName( pDlgObjects->GetDefaultLayerName() );

				//For Undo
				ICommandBuffer* pNewCmdBuffer = pLogicObjectCurrentSawed->CreateCloneNewCmdBuffer();
				if(pNewCmdBuffer)
					pCommandManager->RecordCommand(pNewCmdBuffer);

				_pController->GetToolLogicObjectMgr()->AddToManager(pLogicObjectCurrentSawed);

				if(pObjectNewAction)
					pObjectNewAction->OnObjectCloned(_pController, pLogicObjectCurrentSawed);

				// 방금 생성한것을 선택
				_pController->GetHelperAction()->AddSelectection( pLogicObjectCurrentSawed, true );

				pLogicObjectCurrentSawed->OnLoadCompleted();
			}
		}
		pCommandManager->DoneMouseCmdL();
	}

	beforeX = _pController->m_mouseInfo.m_currentMouseX;
	beforeY = _pController->m_mouseInfo.m_currentMouseY;
}

void CControlDoAction::ObjectSelect(CControlMediator* _pController)
{
	// PreAction
	CDoActionObjectSelect* pObjectSelectAction = _pController->GetWorkSpaceMgr()->GetDoActionObjectSelect();
	if (pObjectSelectAction && pObjectSelectAction->DoPreAction(_pController))
		return;

	//for Zone static Object Selection
	RToolSceneManager* pManager = NULL;
	CToolLogicObjectManager* pLogicObjectManager = NULL;

	//선택 드래그 영역 같은 마우스 커서 타입도 있으므로, 매 렌더링시마다 커서 초기화
	L3DCursor::GetInstance()->SetType(L3DCursor::NONE, false);

	//Lock 확인
	BOOL isLocked = _pController->GetViewPtr<CCoreToolToolBar>()->m_btnObjectLock.isChecked();

	//////////////////////////////////////////////////////////////////////////
	//
	// 선택이전에 해주어야 할 사항
	// 일단 존 편집모드의 Static Object에 대해서만 Shift Copy를 지원한다.
	//
	if(_pController->m_mapInfo.m_bEditSceneObject)
	{
		pManager = _pController->GetToolSceneManager();

		//shitf일때는 clone 모드
		if(	GetAsyncKeyState(VK_SHIFT) && _pController->m_mouseInfo.mouseState.bLButton && _pController->GetToolCmdMgr()->IsMouseCmdEntryL())
		{
			const SCENEOBJECTMAP *pSelectList = _pController->GetHelperAction()->GetSelectedSceneObject();

			//return point
			if( pSelectList->empty() ) 
				return;

			if(!isLocked)
			{
				rs3::RSceneNode* pSelectNodeAtMouse = NULL;
				CLogicObject* pLogicObject = NULL;
				_pController->GetHelperAction()->GetWBObjectAtCurrentMouse(&pSelectNodeAtMouse, &pLogicObject);
				if( !_pController->GetHelperAction()->IsInSelectObjectList( pLogicObject) )
					return;
			}
			//Copy는 MOVE상태로 변경후 실행
			_pController->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_MOVE, true);
			return;
		}
	}
//	else //for field // scenenode/logicobject 둘다 실행하도록 수정
	{
		pLogicObjectManager = _pController->GetToolLogicObjectMgr();
	}

	//잠겨있다면 클론은 수행하되 나머지는 그냥 빠져나가기
	if(isLocked == TRUE)
		return;

	//////////////////////////////////////////////////////////////////////////
	//
	// 드래그 / 드래그 완료
	// Ctrl Selection, Ctrl + Alt Selection, Selection 에 대한 처리
	//

	//ctrl 여부
	bool bCtrlPressd = GetAsyncKeyState(VK_LCONTROL) ? true : false;

	// 드래그 / 드래그 완료를 위한 이전 드래그 상태 체크
	static bool bBeforeLDrag = false;
	if(_pController->m_mouseInfo.mouseState.bDrag && !bBeforeLDrag)
		bBeforeLDrag = true;

	//drag - drag와 drag완료의 구분
	static CPoint pointBegin, pointEnd; //드래그 시의 드래그 포인트
	if( _pController->m_mouseInfo.mouseState.bDrag)
	{
		//POINT pointBegin, pointEnd;
		pointBegin.x = _pController->m_mouseInfo.m_orgMouseX;
		pointBegin.y = _pController->m_mouseInfo.m_orgMouseY;
		pointEnd.x = _pController->m_mouseInfo.m_currentMouseX;
		pointEnd.y = _pController->m_mouseInfo.m_currentMouseY;

		// pointBegin을 항상 좌상단에 올려놓는다.
		if ( pointBegin.x > pointEnd.x )
			std::swap(pointBegin.x, pointEnd.x);
		if ( pointBegin.y > pointEnd.y )
			std::swap(pointBegin.y, pointEnd.y);

		L3DCursor::GetInstance()->SetType( L3DCursor::BOX2D, false );
		L3DCursor::GetInstance()->SetColor( D3DCOLOR_XRGB(255, 255, 255));
		L3DCursor::GetInstance()->SetTransform(
			RVector((float)pointBegin.x,(float)pointBegin.y, 0.0f)
			,RVector((float)pointEnd.x ,(float)pointEnd.y, 0.0f));
	}

	//drag 완료
	if ( !_pController->m_mouseInfo.mouseState.bDrag && bBeforeLDrag)
	{
		bBeforeLDrag = false;

		if ( pointBegin.x == pointEnd.x || pointBegin.y == pointEnd.y )
			return;

		// 마우스의 두 점을 이용하여 프러스텀의 꼭지점을 만들고 ,
		RECT rt;
		rt.left = 0;
		rt.top = 0;
		_pController->GetWBSize(rt.right, rt.bottom);

		RDevice *pDevice = _pController->GetCurrentDevice();
		RMatrix viewMat = pDevice->GetTransform(RST_VIEW);
		RMatrix projMat = pDevice->GetTransform(RST_PROJECTION);

		RVector vCameraPos = _pController->GetToolCamera()->GetPosition();
		float fFarZ = _pController->GetToolCamera()->GetFarZ();
		float fNearZ = _pController->GetToolCamera()->GetNearZ();

		RVector vp1, vq1;
		RVector vp2, vq2;
		RVector vp3, vq3;
		RVector vp4, vq4;

		RGetScreenLine(pointEnd.x, pointEnd.y, rt, viewMat, projMat		,&vp1);
		RGetScreenLine(pointBegin.x, pointBegin.y, rt, viewMat, projMat	,&vp2);
		RGetScreenLine(pointBegin.x, pointEnd.y, rt, viewMat, projMat	,&vp3);
		RGetScreenLine(pointEnd.x, pointBegin.y, rt, viewMat, projMat	,&vp4);

		vq1 = vCameraPos + vp1*fFarZ;
		vp1 = vCameraPos + vp1*fNearZ;
		vq2 = vCameraPos + vp2*fFarZ;
		vp2 = vCameraPos + vp2*fNearZ;
		vq3 = vCameraPos + vp3*fFarZ;
		vp3 = vCameraPos + vp3*fNearZ;
		vq4 = vCameraPos + vp4*fFarZ;
		vp4 = vCameraPos + vp4*fNearZ;

		// 그것을 이용하여 Frustum 을 만든다. Plane 방향은 안쪽으로 -
		RViewFrustum kFrustum;

		RPlane nearPlane(vp2, vp1, vp3);
		RPlane farPlane(vq2, vq3, vq1);
		RPlane leftPlane(vp2, vp3, vq2);
		RPlane rightPlane(vp1, vp4, vq4);
		RPlane topPlane(vp4, vp2, vq2);
		RPlane bottomPlane(vp3, vp1, vq1);

		kFrustum.SetPlane(RV_NEAR,nearPlane);
		kFrustum.SetPlane(RV_FAR ,farPlane);
		kFrustum.SetPlane(RV_LEFT,leftPlane);
		kFrustum.SetPlane(RV_RIGHT,rightPlane);
		kFrustum.SetPlane(RV_TOP,topPlane);
		kFrustum.SetPlane(RV_BOTTOM,bottomPlane);

		//ctrl 키가 눌리지 않았다면 증가선택이 아님
		if (!bCtrlPressd)
			_pController->GetHelperAction()->RemoveAllSelection(false); //Update UI는 실제수행에서

		//ctrl + alt 이면 선택에서 빼기모드
		bool bDragRemoveMode = bCtrlPressd && GetAsyncKeyState(VK_MENU);

		//////////////////////////////////////////////////////////////////////////
		//
		// 실제 수행
		// Ctrl Selection, Ctrl + Alt Selection, Selection 에 대한 처리
		//

		ASSERT(pLogicObjectManager);
		std::vector<CLogicObject*> vecPickEntityObject;
		pLogicObjectManager->PickLogicObjectFrustumForSelection(vecPickEntityObject, kFrustum);
		int nSize = (int)vecPickEntityObject.size();
		_pController->GetHelperAction()->SetSelectedLogicObjectCntForDoAction(nSize);

		bool bOnlyEntityObject = true;
		for ( int i = 0; i < nSize;)
		{
			CLogicObject* pLogicObject = vecPickEntityObject[i];

			if(bDragRemoveMode)
				_pController->GetHelperAction()->RemoveSelection( pLogicObject, true);
			else
				_pController->GetHelperAction()->AddSelectection( pLogicObject, true);

			if (pLogicObject->IsEntityLogic() == false)
			{
				bOnlyEntityObject = false;
			}

			++i;
			_pController->GetHelperAction()->SetSelectedLogicObjectCntForDoAction(nSize - i);
		}

		if (pObjectSelectAction)
			pObjectSelectAction->OnSelectChanged(_pController, bOnlyEntityObject);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// 마우스 버튼 클릭시 - callback 액션이므로 다운은 클릭을 의미
	//
	if (!_pController->m_mouseInfo.mouseState.bDrag && _pController->m_mouseInfo.mouseState.bLButton)
	{
		const RVector *pOrigin(NULL), *pTo(NULL);
		_pController->GetHelperAction()->GetCurrentMouseLine(&pOrigin, &pTo);
		if( pOrigin && pTo)
		{
			//선택할 물체
			CLogicObject* pLogicObject = NULL;

			ASSERT(pLogicObjectManager);
			pLogicObject = pLogicObjectManager->PickLogicObjectForSelection(*pOrigin, *pTo);

			// set selected logic object count
			_pController->GetHelperAction()->SetSelectedLogicObjectCntForDoAction(1);
			if ( !bCtrlPressd )
			{
				//더하기 선택이 아니므로 선택 초기화
				_pController->GetHelperAction()->RemoveAllSelection(true);
				//더하기
				_pController->GetHelperAction()->AddSelectection( pLogicObject, true);
			}
			else
			{
				//더하기 선택
				if(_pController->GetHelperAction()->IsInSelectObjectList( pLogicObject))
					_pController->GetHelperAction()->RemoveSelection( pLogicObject, true);
				else
					_pController->GetHelperAction()->AddSelectection( pLogicObject, true);
			}

			if( !_pController->m_mapInfo.m_bEditSceneObject )
			{
				if(pLogicObject)
				{
					if (pObjectSelectAction)
						pObjectSelectAction->OnSelectChanged(_pController, pLogicObject->IsEntityLogic());
				}
			}

			// reset selected logic object count
			_pController->GetHelperAction()->SetSelectedLogicObjectCntForDoAction(0);
		}
	}
}

RVector CControlDoAction::GetMoveVector(AXIS_STATE nAxisType, const RVector &vPosition, const rs3::RMatrix& localFrame
										, const RVector &p1, const RVector &q1, const RVector &q2)
{
	RVector vBaseXAxis(localFrame._11, localFrame._12, localFrame._13);
	RVector vBaseYAxis(localFrame._21, localFrame._22, localFrame._23);
	RVector vBaseZAxis(localFrame._31, localFrame._32, localFrame._33);

	RPlane XYPlane, YZPlane, XZPlane;
	XYPlane.SetPlane(vBaseZAxis, vPosition);
	YZPlane.SetPlane(vBaseXAxis, vPosition);
	XZPlane.SetPlane(vBaseYAxis, vPosition);

	// moveaxis_x,y,z 의 경우에는 축을 포함하는 두 평면을 사용해도 되는데
	// 평행에 가까워지면 오차가 커지므로 그중 카메라 방향에 따라 적절한 평면을 선택해준다
	RVector vecCamera = Normalize(q1-p1);

	RPlane *pPlane = NULL;
	switch ( nAxisType )
	{
	case MOVEAXIS_X :
		pPlane = fabs(vecCamera.z)>fabs(vecCamera.y) ? &XYPlane : &XZPlane;
 		break;
	case MOVEAXIS_Y :
		pPlane = fabs(vecCamera.z)>fabs(vecCamera.x) ? &XYPlane : &YZPlane;
 		break;
	case MOVEAXIS_Z :
		pPlane = fabs(vecCamera.y)>fabs(vecCamera.x) ? &XZPlane : &YZPlane;
 		break;
	case MOVEAXIS_XY:
		pPlane = &XYPlane;
		break;
	case MOVEAXIS_YZ:
		pPlane = &YZPlane;
		break;
	case MOVEAXIS_XZ:
		pPlane = &XZPlane;
		break;
	}

	RVector v1, v2;
	if ( !pPlane->Intersect(v1, p1, q1))
		return RVector(0, 0, 0);
	if ( !pPlane->Intersect(v2, p1, q2))
		return RVector(0, 0, 0);

	RVector vDirection = ( v2 - v1 );
	float t;

	switch ( nAxisType )
	{
	case MOVEAXIS_X :
		t = vDirection.DotProduct(vBaseXAxis);
		vDirection = vBaseXAxis*t;
		break;
	case MOVEAXIS_Y :
		t = vDirection.DotProduct(vBaseYAxis);
		vDirection = vBaseYAxis*t;
		break;
	case MOVEAXIS_Z :
		t = vDirection.DotProduct(vBaseZAxis);
		vDirection = vBaseZAxis*t;
		break;
	case MOVEAXIS_XY:
		vDirection.z = 0.0f;
		break;
	case MOVEAXIS_YZ:
		vDirection.x = 0.0f;
		break;
	case MOVEAXIS_XZ:
		vDirection.y = 0.0f;
		break;
	}

	return vDirection;
}

void CControlDoAction::ObjectMove(CControlMediator* _pController)
{
	// PreAction
	CDoActionObjectMove* pObjectMoveAction = _pController->GetWorkSpaceMgr()->GetDoActionObjectMove();
	if (pObjectMoveAction && pObjectMoveAction->DoPreAction(_pController))
		return;

	AXIS_STATE eAxis = _pController->GetViewPtr<CCoreToolToolBar>()->GetCurrentAxisToAXIS_STATE();
	if ( eAxis == AXIS_NULL )
		return;

	//Get Object Edit type and do pre-action
	OBJECT_EDIT_TYPE edit_type = GetObjectEditTypeAndPreAction(_pController);

	//object eidtion의 조건
	if(edit_type != OBJECT_EDIT || !_pController->m_mouseInfo.mouseState.bDrag || !_pController->m_mapInfo.m_bLoaded)
		return;

	// 움직임이 없으면 리턴
	if (_pController->m_mouseInfo.m_movedMouseX == 0 && _pController->m_mouseInfo.m_movedMouseY == 0)
		return;

// 	const SCENEOBJECTMAP* pSelectList = _pController->GetHelperAction()->GetSelectedSceneObject();
	ENTITY_OBJECTLIST* pSelectedEntityObject = _pController->GetHelperAction()->GetSelectedEntityObject();
	LOGIC_OBJECTLIST* pSelectedLogicList = _pController->GetHelperAction()->GetSelectedLogicObject();

	// 선택되어있는게 없으면 리턴
	if(/*pSelectList->empty() &&*/ pSelectedLogicList->empty() && pSelectedEntityObject->empty() )
		return;
// 	if ((_pController->m_mapInfo.m_bEditSceneObject && pSelectList->empty()) ||
// 		(!_pController->m_mapInfo.m_bEditSceneObject && (pSelectedEntityObject->empty() && pSelectedLogicList->empty()) ) ||
// 		(_pController->m_mouseInfo.m_movedMouseX == 0 && _pController->m_mouseInfo.m_movedMouseY == 0)
// 		)
// 		return;

	RVector p1, q1, p2, q2;
	// 현재 커서 위치
	LONG cx = _pController->m_mouseInfo.m_currentMouseX;
	LONG cy = _pController->m_mouseInfo.m_currentMouseY;

	// 원래 위치
	LONG ox = _pController->m_mouseInfo.m_orgMouseX;
	LONG oy = _pController->m_mouseInfo.m_orgMouseY;

	//GetCurrentMouseLine을 쓸 경우에 계산하는 중간에 바뀔수 있다.
	_pController->GetHelperAction()->GetMouseLine(cx, cy, p2, q2);			// 현재 위치
	_pController->GetHelperAction()->GetMouseLine(ox, oy, p1, q1);	// 드래그 시작지점

	// 선택된 움직임 버튼 ( x, y, z, xy, yz ... ) 에 따른 적절한 움직임을 구한다
	const RVector& vecActionPos = _pController->GetHelperAction()->GetActionPlanePos();
	const RMatrix& matLocalFrame = _pController->GetHelperAction()->GetSelectedObjectActionFrame();
	RVector vVec = GetMoveVector(eAxis, vecActionPos, matLocalFrame, p1, q1, q2);

	//실제 적용부분
	CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
	CGroupedCommandBuffer* pGroupCmdBuffer = NULL;
	bool bActionEntry = pCommandManager->IsMouseCmdEntryL();

	//그룹 커맨드 버퍼 생성
	if(bActionEntry)
	{
		pGroupCmdBuffer = new CGroupedCommandBuffer;
		ns_mapObjPosOrg.clear();
	}
	
	// Entity 경우
	if ( !pSelectedEntityObject->empty() && pObjectMoveAction)
	{
		_ASSERT(pSelectedEntityObject);
		pObjectMoveAction->OnMove(_pController, pSelectedEntityObject, vVec, pGroupCmdBuffer);
	}

	// logic object
	if(!pSelectedLogicList->empty()) // scenenode/logicobject 둘다 실행하도록 수정
	{
		int nLogicIndex = 0;
		int nSelectedNum = pSelectedLogicList->size();

		// Logic 편집 모드일 경우
		for(LOGIC_OBJECTLIST::iterator it_logic = pSelectedLogicList->begin(); it_logic != pSelectedLogicList->end(); ++it_logic, ++nLogicIndex)
		{
			CLogicObject* pLogicObject = *it_logic;
			if (pLogicObject->GetEditable() == false)
				continue;

			if(bActionEntry)
			{
	 			RVector vPosOrg;
	 			if(pLogicObject->GetPosition(vPosOrg) == false)
	 				continue;

				CLogicMoveCmdBuffer* pCmdBuffer = new CLogicMoveCmdBuffer(pLogicObject, vPosOrg);
				pGroupCmdBuffer->AddBuffer(pCmdBuffer);

				ns_mapObjPosOrg[ DWORD_PTR(pLogicObject) ] = DWORD_PTR( pCmdBuffer );
			}

			CLogicMoveCmdBuffer* pCmdBuffer = (CLogicMoveCmdBuffer*)( ns_mapObjPosOrg[ DWORD_PTR(pLogicObject) ]);

			// 복사이동(-_- Shift등으로 복사하면서 이동) 등으로
			// 액션 엔트리없이 바로 진행되었을 경우에도 고려해서 짜야한다.
			if (pCmdBuffer == NULL)
			{
				if (pGroupCmdBuffer == NULL)
				{
					pGroupCmdBuffer = new CGroupedCommandBuffer;
					ns_mapObjPosOrg.clear();

					// mouse observer 등록
					_pController->GetHelperAction()->AddToWBMouseObserver( CControlMediatorHelperAction::WB_MOUSE_L_UP, CCoreToolToolBar::GetID() );
					_pController->GetHelperAction()->AddToWBMouseObserver( CControlMediatorHelperAction::WB_MOUSE_L_UP, CDlgSceneNodeList::GetID() );
				}

				RVector vPosOrg;
				if(pLogicObject->GetPosition(vPosOrg))
				{
					pCmdBuffer = new CLogicMoveCmdBuffer(pLogicObject, vPosOrg);
					pGroupCmdBuffer->AddBuffer(pCmdBuffer);
					ns_mapObjPosOrg[ DWORD_PTR(pLogicObject) ] = DWORD_PTR( pCmdBuffer );
				}

				if ( nLogicIndex == nSelectedNum - 1)
					pCommandManager->RecordCommand(pGroupCmdBuffer);
			}

			RVector vPos = pCmdBuffer->GetPosOrg();

			vPos = vPos + vVec;

			CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
			int nSnapToGridSize = pMainFrame->GetSnapToGridSize();
			if( nSnapToGridSize > 0 )
			{

#define SIGN_OF(x) (x>0 ? 1 : -1)

				vPos.x = (int)((vPos.x + SIGN_OF(vPos.x) * .5f * float(nSnapToGridSize)) / nSnapToGridSize) * nSnapToGridSize;
				vPos.y = (int)((vPos.y + SIGN_OF(vPos.y) * .5f * float(nSnapToGridSize)) / nSnapToGridSize) * nSnapToGridSize;
				vPos.z = (int)((vPos.z + SIGN_OF(vPos.z) * .5f * float(nSnapToGridSize)) / nSnapToGridSize) * nSnapToGridSize;
			}

			char szDragInfo[256] = {0, };
			sprintf(szDragInfo, "Move %.2f , %.2f , %.2f", 
				 vPos.x - pCmdBuffer->GetPosOrg().x, 
				 vPos.y - pCmdBuffer->GetPosOrg().y,
				 vPos.z - pCmdBuffer->GetPosOrg().z );

			UpdateDragText( _pController, szDragInfo );

			pLogicObject->SetPosition(vPos);
		}
	}

	//액션 완료
	if(bActionEntry)
	{
		pCommandManager->RecordCommand(pGroupCmdBuffer);
		pCommandManager->DoneMouseCmdL();

		// mouse observer 등록
		_pController->GetHelperAction()->AddToWBMouseObserver( CControlMediatorHelperAction::WB_MOUSE_L_UP, CCoreToolToolBar::GetID() );
		_pController->GetHelperAction()->AddToWBMouseObserver(CControlMediatorHelperAction::WB_MOUSE_L_UP, -1);
	}
}

void CControlDoAction::ObjectRotate(CControlMediator* _pController)
{
	// PreAction
	CDoActionObjectRotate* pObjectRotateAction = _pController->GetWorkSpaceMgr()->GetDoActionObjectRotate();
	if (pObjectRotateAction && pObjectRotateAction->DoPreAction(_pController))
		return;

	AXIS_STATE nAxis = _pController->GetViewPtr<CCoreToolToolBar>()->GetCurrentAxisToAXIS_STATE();
	if ( nAxis == AXIS_NULL )
		return;

	//Get Object Edit type and do pre-action
	OBJECT_EDIT_TYPE edit_type = GetObjectEditTypeAndPreAction(_pController);

	//object eidtion의 조건
	if( edit_type != OBJECT_EDIT || !_pController->m_mapInfo.m_bLoaded ||
		!_pController->m_mouseInfo.mouseState.bDrag ||
		(_pController->m_mouseInfo.m_movedMouseX == 0 && _pController->m_mouseInfo.m_movedMouseY == 0)
		)
		return;

	//Command Manager
	CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
	CGroupedCommandBuffer* pGroupCmdBuffer = NULL;

	// 실제 액션.
	bool bActionEntry = pCommandManager->IsMouseCmdEntryL();

// 	const SCENEOBJECTMAP *pSelectList = _pController->GetHelperAction()->GetSelectedSceneObject();
	ENTITY_OBJECTLIST* pSelectedEntityObject = _pController->GetHelperAction()->GetSelectedEntityObject();
	LOGIC_OBJECTLIST* pSelectedLogicObject = _pController->GetHelperAction()->GetSelectedLogicObject();

	if(bActionEntry && ( /*pSelectList->size() ||*/ pSelectedEntityObject->size() || pSelectedLogicObject->size() ) )
	{
		pGroupCmdBuffer = new CGroupedCommandBuffer;
		ns_mapObjRotOrg.clear();
	}

	// 로테이션 메트릭스 만들기
	if (MakeRotationMatrixAndInfoText(ns_matRotate, _pController, nAxis ) == false)
	{
		// delete NULL은 유효한 코드다. pGroupCmdBuffer가 NULL인지 아닌지는 상관없다.
		delete pGroupCmdBuffer;
		return;
	}

	if ( pSelectedEntityObject->size() )
	{
		if (pObjectRotateAction)
			pObjectRotateAction->OnRotate(_pController, pSelectedEntityObject, ns_matRotate, nAxis, pGroupCmdBuffer);
	}

	// Logic // scene/entity/logic 모두 실행
//		else
	if( pSelectedLogicObject->size() )
	{
		int nLogicIndex = 0;
		int nSelectedNum = pSelectedLogicObject->size();

		// 회전의 중심 - 선택한 물체의 중심을 구하기 (여러개일 때는 가운데 점을 중심으로 회전 이동)
		RVector* pPivot = _pController->GetHelperAction()->GetSelectNodeCenter();
		_ASSERT( pPivot );

		// scene 편집 모드일때는 여러개의 원점을 중심으로 , logic 인 경우에는 각각을 중심으로 회전한다

		RMatrix matTransform, matTranslation, matInverseTranslation;
		for(LOGIC_OBJECTLIST::iterator itr = pSelectedLogicObject->begin(); itr != pSelectedLogicObject->end(); ++itr, ++nLogicIndex)
		{
			CLogicObject* pLogicObject = *itr;
			if (pLogicObject->GetEditable() == false)
				continue;

			RVector vPos, vDir;
			if( pLogicObject->GetPosition(vPos) == false ||
				pLogicObject->GetDirection(vDir) == false )
				continue;

			RVector vUp(RVector::AXISZ);
			if ( pLogicObject->GetUp(vUp) )
			{
				if (MakeRotationMatrixAndInfoText(ns_matRotate, _pController, nAxis) == false)
					continue;
			}

			if(bActionEntry)
			{
				CLogicRotateCmdBuffer* pCmdBuffer = new CLogicRotateCmdBuffer(pLogicObject, vDir, vUp, vPos, *pPivot);
				pGroupCmdBuffer->AddBuffer(pCmdBuffer);

				ns_mapObjRotOrg[ DWORD_PTR(pLogicObject) ] = DWORD_PTR( pCmdBuffer );
			}

			// 원래값에서 변한만큼만 회전한다.
			CLogicRotateCmdBuffer* pCmdBuffer = (CLogicRotateCmdBuffer*)( ns_mapObjRotOrg[ DWORD_PTR(pLogicObject) ]);
			if (pCmdBuffer == NULL)
			{
				if (pGroupCmdBuffer == NULL)
				{
					pGroupCmdBuffer = new CGroupedCommandBuffer;
					ns_mapObjRotOrg.clear();

					// mouse observer 등록
					_pController->GetHelperAction()->AddToWBMouseObserver( CControlMediatorHelperAction::WB_MOUSE_L_UP, CCoreToolToolBar::GetID() );
					_pController->GetHelperAction()->AddToWBMouseObserver( CControlMediatorHelperAction::WB_MOUSE_L_UP, CDlgSceneNodeList::GetID() );
				}

				pCmdBuffer = new CLogicRotateCmdBuffer(pLogicObject, vDir, vUp, vPos, *pPivot);
				pGroupCmdBuffer->AddBuffer(pCmdBuffer);

				ns_mapObjRotOrg[ DWORD_PTR(pLogicObject) ] = DWORD_PTR( pCmdBuffer );

				if ( nLogicIndex == nSelectedNum - 1)
					pCommandManager->RecordCommand(pGroupCmdBuffer);
			}

			
			// 원래값에서 변한만큼만 회전한다.
			matTransform.SetLocalMatrix( pCmdBuffer->GetRotOrgPos(), pCmdBuffer->GetRotOrgDir(), pCmdBuffer->GetRotOrgUp() );

			matTranslation.MakeIdentity();
			matInverseTranslation.MakeIdentity();

			RVector vecPivot = pCmdBuffer->GetRotPivot();
			matTranslation.SetTranslation(- vecPivot);
			matInverseTranslation.SetTranslation( vecPivot );

			matTransform = matTransform * matTranslation * ns_matRotate * matInverseTranslation;

			RVector vecNewDir, vecNewUp, vecNewPos;
			vecNewDir = RVector(matTransform._21,matTransform._22,matTransform._23);
			vecNewUp = RVector(matTransform._31,matTransform._32,matTransform._33);
			vecNewPos = RVector(matTransform._41,matTransform._42,matTransform._43);

			pLogicObject->SetDirectionUp(vecNewDir,vecNewUp);

			// scene 편집 모드일때는 pivot 을 중심으로 회전
			pLogicObject->SetPosition( vecNewPos );
		}
	}

	if(bActionEntry)
	{
		// bActionEntry인데 잠금상태라서 WB상태가 선택이 아니라 편집이고 선택된 물체는 없는경우는
		// pGroupCmdBuffer가 NULL 상태다.
		if (pGroupCmdBuffer)
		{
			if(pGroupCmdBuffer->IsEmpty())
				delete pGroupCmdBuffer;
			else
				pCommandManager->RecordCommand(pGroupCmdBuffer);
		}

		pCommandManager->DoneMouseCmdL();

		// mouse observer 등록
		_pController->GetHelperAction()->AddToWBMouseObserver( CControlMediatorHelperAction::WB_MOUSE_L_UP, CCoreToolToolBar::GetID() );
		if (_pController->m_mapInfo.m_bEditSceneObject)
			_pController->GetHelperAction()->AddToWBMouseObserver( CControlMediatorHelperAction::WB_MOUSE_L_UP, CDlgSceneNodeList::GetID() );
	}
}

void CControlDoAction::ObjectScale(CControlMediator* _pController)
{
	CDoActionObjectScale* pObjectScaleAction = _pController->GetWorkSpaceMgr()->GetDoActionObjectScale();
	if (pObjectScaleAction && pObjectScaleAction->DoPreAction(_pController))
		return;

	//Get Object Edit type and do pre-action
	OBJECT_EDIT_TYPE edit_type = GetObjectEditTypeAndPreAction(_pController);

	//object eidtion의 조건
	if(edit_type != OBJECT_EDIT || !_pController->m_mouseInfo.mouseState.bDrag || !_pController->m_mapInfo.m_bLoaded)
		return;

	CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
	bool bActionEntry = pCommandManager->IsMouseCmdEntryL();

	{
		//scale adjust factor
		float f;
		static int nCumulFactor = 0;
		nCumulFactor += abs(_pController->m_mouseInfo.m_movedMouseY);
		if( nCumulFactor > 100 )
			f = (-_pController->m_mouseInfo.m_movedMouseY) * 0.03f;
		else
			f = (-_pController->m_mouseInfo.m_movedMouseY) * 0.01f;
		if(f == 0) return;

		ENTITY_OBJECTLIST* pSelectedEntityObject = _pController->GetHelperAction()->GetSelectedEntityObject();
		if(pSelectedEntityObject->size())
		{
			CGroupedCommandBuffer* pGroupCmdBuffer = NULL;
			if(bActionEntry)
				pGroupCmdBuffer = new CGroupedCommandBuffer;

			if (pObjectScaleAction)
				pObjectScaleAction->OnScale(_pController, pSelectedEntityObject, f, pGroupCmdBuffer);

			if(bActionEntry)
			{
				if(pGroupCmdBuffer->IsEmpty())
					delete pGroupCmdBuffer;
				else
					pCommandManager->RecordCommand(pGroupCmdBuffer);

				pCommandManager->DoneMouseCmdL();
			}
		}

		LOGIC_OBJECTLIST* pSelectedLogicObject = _pController->GetHelperAction()->GetSelectedLogicObject();
		if(pSelectedLogicObject->size())
		{
			CGroupedCommandBuffer* pGroupCmdBuffer = NULL;
			if(bActionEntry)
				pGroupCmdBuffer = new CGroupedCommandBuffer;

			size_t selected_size = pSelectedLogicObject->size();
			for(LOGIC_OBJECTLIST::iterator itr = pSelectedLogicObject->begin(); itr != pSelectedLogicObject->end(); ++itr)
			{
				CLogicObject* pLogicObject = *itr;
				if ( pLogicObject->GetEditable() == false)
					continue;

				float fRadius;
				if( pLogicObject->GetScale(fRadius) == false ) continue;

				float fNewRadius = fRadius * (1.0f + f);
				if( fNewRadius < 0.f ) fNewRadius = 0.f;

				pLogicObject->SetScale( fNewRadius );

				if(bActionEntry)
				{
					CLogicScaleCmdBuffer* pCmdBuffer = new CLogicScaleCmdBuffer(pLogicObject, fRadius);
					pGroupCmdBuffer->AddBuffer(pCmdBuffer);
				}
			}

			if(bActionEntry)
			{
				if(pGroupCmdBuffer->IsEmpty())
					delete pGroupCmdBuffer;
				else
					pCommandManager->RecordCommand(pGroupCmdBuffer);

				pCommandManager->DoneMouseCmdL();
			}
		}

	}

	if(bActionEntry)
	{
		// observer 등록
		_pController->GetHelperAction()->AddToWBMouseObserver(CControlMediatorHelperAction::WB_MOUSE_L_UP, -1);

	}
}

#pragma warning( pop )
