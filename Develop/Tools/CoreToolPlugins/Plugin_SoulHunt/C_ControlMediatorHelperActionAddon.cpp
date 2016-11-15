#include "StdAfx.h"

#include "C_ControlMediatorHelperActionAddon.h"
#include "C_ControlMediator.h"

#include "M_ToolLogicObjectManager.h"
#include "M_ToolCmdMgr.h"
#include "M_EntityCommandBuffer.h"
#include "M_PathFinder.h"

#include "VL_ToolBar.h"
#include "VR_EntityObjectList.h"
#include "VR_EntityEnvironment.h"

bool CControlMediatorHelperEntityActionImpl::OnCheckAddSelection( CEntityObject* _pEntityObject, bool _bUpdateUI )
{
	return m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->SetSelectEntityObject(_pEntityObject, TRUE);
}

void CControlMediatorHelperEntityActionImpl::OnRemoveSelection( CEntityObject* _pEntityObject, bool _bUpdateUI )
{
	if(_bUpdateUI)
		m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->SetSelectEntityObject(_pEntityObject, FALSE);
}

void CControlMediatorHelperEntityActionImpl::OnRemoveSelectionAll( bool _bUpdateUI )
{
	if(_bUpdateUI) 
		m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->SetSelectNone();
}

void CControlMediatorHelperEntityActionImpl::OnDeleteSelectedObject(ENTITY_OBJECTLIST& _list, ENTITY_OBJECTLIST& _copyList)
{
	CRollupEntityObjectList* pEntityObjectList = m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>();
	CGroupedCommandBuffer* pGroupCmdBuffer = new CGroupedCommandBuffer;

	for(ENTITY_OBJECTLIST::iterator itr = _list.begin(); itr != _list.end(); ++itr)
	{
		CEntityObject* pEntityObject = *itr;
		if ( pEntityObject->GetEditable() == false )
			continue;

		m_pControlMediator_->GetToolLogicObjectMgr()->RemoveFromManager(pEntityObject);
		pEntityObjectList->RemoveTrackingSpawnIf(pEntityObject);

		//삭제할 대상이 copyList에 있으면 copyList 초기화
		std::list<CEntityObject*>::iterator itr_finder = std::find( _copyList.begin(), _copyList.end(), pEntityObject);
		if(itr_finder != _copyList.end())
			_copyList.clear();

		CEntityDeleteCmdBuffer* pCmdBuffer = new CEntityDeleteCmdBuffer(pEntityObject);
		pGroupCmdBuffer->AddBuffer(pCmdBuffer);
	}

	CToolCmdMgr* pCmdMgr = m_pControlMediator_->GetToolCmdMgr();
	pCmdMgr->RecordCommand(pGroupCmdBuffer);
	m_pControlMediator_->GetHelperAction()->RemoveAllSelection(false);//일단 UI는 Update 하지 않는다.
	pEntityObjectList->UpdateListBox(true, true);
}

void CControlMediatorHelperEntityActionImpl::OnPasteCopiedObject(ENTITY_OBJECTLIST& _list, ENTITY_OBJECTLIST& _copyList)
{
	using namespace rs3;
	if( _copyList.empty() )
		return;

	//선택한 오브젝트들의 중심과 range 비교를 위한 aabb구하기
	RVector vecObjectCenterToCopy, vecViewCenter, vecOffSetToViewCenter;
	RBoundingBox aabbOjbectToCopy;

	if( ! m_pControlMediator_->GetHelperAction()->GetSelectedObjectAABBCenter(vecObjectCenterToCopy, &aabbOjbectToCopy) )
		return;

	//화면 중심좌표 구하기, ToDo, 일단 지형에 의존적으로
	RToolTerrain* pTerrain = (RToolTerrain*) m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
	RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	RVector vecLineP(pCamera->GetPosition());
	RVector vecLineQ(pCamera->GetDirection());
	if(vecLineQ.x == 0) vecLineQ.x = 0.00001f;
	if(vecLineQ.y == 0) vecLineQ.y = 0.00001f;
	if(vecLineQ.z == 0) vecLineQ.z = 0.00001f;

	if(pTerrain && pTerrain->GetPickLine(vecViewCenter, NULL, vecLineP, vecLineP + vecLineQ * pCamera->GetFarZ()))
		vecOffSetToViewCenter = vecViewCenter - vecObjectCenterToCopy;
	else
	{
		AfxMessageBox("화면의 중심이 맵상에 있지 않습니다.");
		return;
	}

	//붙여넣기 물체 range 체크
	aabbOjbectToCopy.vmin += vecOffSetToViewCenter;
	aabbOjbectToCopy.vmax += vecOffSetToViewCenter;

	float fSafeOffSetX(0), fSafeOffSetY(0);
	if(aabbOjbectToCopy.vmin.x < 0)
	{
		fSafeOffSetX = - aabbOjbectToCopy.vmin.x;
	}
	if(aabbOjbectToCopy.vmin.y < 0)
	{
		fSafeOffSetY = - aabbOjbectToCopy.vmin.y;
	}
	if(aabbOjbectToCopy.vmax.x > m_pControlMediator_->m_mapInfo.m_width)
	{
		fSafeOffSetX = m_pControlMediator_->m_mapInfo.m_width - aabbOjbectToCopy.vmax.x;
	}
	if(aabbOjbectToCopy.vmax.y > m_pControlMediator_->m_mapInfo.m_height)
	{
		fSafeOffSetY = m_pControlMediator_->m_mapInfo.m_height - aabbOjbectToCopy.vmax.y;
	}
	vecOffSetToViewCenter.x += fSafeOffSetX;
	vecOffSetToViewCenter.y += fSafeOffSetY;

	////////////////////////////////////////////////////////
	///
	/// 실제적인 객체 복사
	RVector vOldPos, vNewPos;
	float fNewX(0), fNewY(0);
	CEntityObject* pPastedObject = NULL;

	//선택을 붙여넣기한 객체로 바꾸며 붙여넣기
	_list.clear();
	size_t selected_size = _copyList.size();

	CRollupEntityObjectList* pRollupEntityObjectList = m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>();
	CGroupedCommandBuffer* pGroupedCmd = new CGroupedCommandBuffer;
	for(std::list<CEntityObject*>::iterator itr = _copyList.begin(); itr != _copyList.end(); ++itr)
	{
		CEntityObject* pEntityObject = *itr;
		pEntityObject->GetPosition(vOldPos);

		fNewX = vOldPos.x + vecOffSetToViewCenter.x;
		fNewY = vOldPos.y + vecOffSetToViewCenter.y;
		if( pTerrain->GetPickVertical(vNewPos, NULL, fNewX, fNewY) )
			pPastedObject = pEntityObject->CloneAtWB(vNewPos);
		else
			pPastedObject = pEntityObject->CloneAtWB(RVector(fNewX, fNewY, vOldPos.z));

		m_pControlMediator_->GetToolLogicObjectMgr()->AddToManager(pPastedObject);
		_list.push_back(pPastedObject);

		if(selected_size == 1 && pPastedObject->GetType() == CEntityObject::ENTITY_SPAWN)
			pRollupEntityObjectList->ChangeTrackingSpawn( (CEntitySpawn*)pPastedObject );

		//Undo Command
		CEntityNewCmdBuffer* pCmdBuffer = new CEntityNewCmdBuffer(pPastedObject);
		pGroupedCmd->AddBuffer(pCmdBuffer);
	}
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(pGroupedCmd);

	//선택한 객체에 대한 MFC UI 업데이트 및 새로 붙여넣을것을 지금것으로 교체
	_copyList.clear();

	pRollupEntityObjectList->SetListBoxUpdateLock(true);
	pRollupEntityObjectList->UpdateListBox(false, false);
	for(std::list<CEntityObject*>::iterator itrSetSelect = _list.begin();
		itrSetSelect != _list.end(); ++itrSetSelect)
	{
		pRollupEntityObjectList->SetSelectEntityObject( *itrSetSelect, TRUE);
		_copyList.push_back( *itrSetSelect );
	}
	pRollupEntityObjectList->SetListBoxUpdateLock(false);
	pRollupEntityObjectList->OnLbnSelchangeEntityList();
}

void CControlMediatorHelperEntityActionImpl::OnAddToLogicObjectManager( CEntityObject* _pEntityObject, int& _nCnt, int& _nIdx )
{
	_nCnt = (int)CEntityObject::ENTITY_OBJ_NUM;
	_nIdx = (int)_pEntityObject->GetType();

	RVector pos;
	if (_pEntityObject->GetPosition(pos))
		_pEntityObject->SetValid( g_PathfinderTest.IsVailid3DPostiion(pos));
}

void CControlMediatorHelperEntityActionImpl::OnRemoveFromLogicObjectManager( CEntityObject* _pEntityObject, int& _nCnt, int& _nIdx )
{
	_nCnt = (int)CEntityObject::ENTITY_OBJ_NUM;
	_nIdx = (int)_pEntityObject->GetType();
}

void CControlMediatorHelperEntityActionImpl::OnGetNavigationDirection( CEntityObject* _pEntityObject, const rs3::RVector& _vPos, rs3::RVector& _outDir )
{
	if(_pEntityObject->GetType() == CEntityObject::ENTITY_MARKER)
	{
		RVector vEntityDir;
		if(_pEntityObject->GetDirection(vEntityDir))
			_outDir = _vPos + vEntityDir;
	}
}

void CControlMediatorHelperEntityActionImpl::OnChagnedEnvWeather( const char* _pSzName )
{
	m_pControlMediator_->GetViewPtr<CRollupEntityEnvironment>()->SetEnvironmentSet(_pSzName, false);
}

void CControlMediatorHelperEntityActionImpl::OnToolBarNewEntityClicked()
{
	CMenu menu;
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		menu.LoadMenu( IDR_MENU_NEW_ENTITY );
	}

	CMenu* pPopupMenu = menu.GetSubMenu (0);
	_ASSERT(pPopupMenu != NULL);
	int nResult = m_pControlMediator_->GetViewPtr<CCoreToolToolBar>()->m_btnGroup[WORKBENCH_STATE::OBJECT_NEW].displayPopupMenu( pPopupMenu );
	switch (nResult)
	{
	case ID_NEW_SPAWN :
		m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->OnBnClickedButtonNewSpawn();
		break;

	case ID_NEW_MARKER :
		m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->OnBnClickedButtonNewMarker();
		break;

	case ID_NEW_SENSOR :
		m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->OnBnClickedButtonNewSensor();
		break;
	case ID_NEW_SOUND :
		m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->OnBnClickedButtonNewSound();
		break;
	}
}

void CControlMediatorHelperEntityActionImpl::OnToolBarEntityOnTerrainClicked()
{
	rs3::RToolSceneManager* pSceneManager = m_pControlMediator_->GetToolSceneManager();
	if( pSceneManager == NULL )
		return;

	CGroupedCommandBuffer* pGroupCmdBuffer(NULL);
	rs3::RVector vPos;
	rs3::RVector vNormal;

	ENTITY_OBJECTLIST* pSelectedEntityObject = m_pControlMediator_->GetHelperAction()->GetSelectedEntityObject();
	for(std::list<CEntityObject*>::iterator itr = pSelectedEntityObject->begin(); itr != pSelectedEntityObject->end(); ++itr)
	{
		CEntityObject* pEntityObject = *itr;
		if(	pEntityObject->GetPosition(vPos) == false)
			continue;

		rs3::RVector vOut;
		const rs3::RVector* pNewPos = pSceneManager->GetVerticalContactPos(vPos, NULL );
		if (pNewPos)
		{
			if (!g_PathfinderTest.GetValid3DPosition(&vOut,*pNewPos))
			{
				pEntityObject->SetValid(false);
				continue;
			}

			if (vPos != vOut)
			{
				CEntityMoveCmdBuffer* pCmdBuffer = new CEntityMoveCmdBuffer(pEntityObject, vPos);

				if( pGroupCmdBuffer == NULL )
					pGroupCmdBuffer = new CGroupedCommandBuffer;

				pGroupCmdBuffer->AddBuffer(pCmdBuffer);

				pEntityObject->SetPosition(vOut);
				pEntityObject->SetValid(g_PathfinderTest.IsVailid3DPostiion(vOut));
			}
		}
	}

	if( pGroupCmdBuffer != NULL )
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand( pGroupCmdBuffer );
}

void CControlMediatorHelperEntityActionImpl::OnToolBarSelectionLocked( bool _bLock )
{
	m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->SetListLock(_bLock);
}
