#include "stdafx.h"
#include "C_ControlDoActionImpl.h"

#include "M_ToolTerrain.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperResource.h"
#include "C_ControlMediatorHelperRender.h"
#include "WS_WorkSpaceManager.h"

#include "M_ToolLogicObjectManager.h"
#include "M_ToolCmdMgr.h"
#include "M_EntityCommandBuffer.h"
#include "M_LogicObjectCommandBuffer.h"

#include "C_XMLConfig.h"
#include "C_ControlCursor.h"
#include "C_StatusBarHandler.h"

#include "VL_SceneNodeList.h"
#include "VR_ZoneObjectNew.h"
#include "VR_EntityObjectList.h"
#include "VR_EtcPathFind.h"

#include "M_PathFinder.h"

#include "VL_ToolBar.h"

#include "C_XMLEditInfoFile.h"
#include "RUtil.h"

#include "ext_lib/FooButton.h"

#include <string>

#pragma warning( push )
#pragma warning( disable : 4996 )

using namespace rs3;

bool CDoActionObjectNewImpl::DoPreAction( CControlMediator* _pController )
{
	static SElementAtXML* pLastSowedRadius = NULL;
	static CEntityArea* pLastSowedEntity = NULL;

	if (false == _pController->m_mapInfo.m_bSceneEditableMap)
	{
		if (_pController->GetViewPtr<CRollupEntityObjectList>()->IsCustomObjectNewMode())
		{
			// RButton 이 눌러져 있으면, Unselect 상태로 만든다.
			if ( _pController->m_mouseInfo.mouseState.bRButton || GetAsyncKeyState(VK_ESCAPE) )
			{
				_pController->m_mouseInfo.m_bPopAble = true;
				_pController->m_mouseInfo.mouseState.bRButton = false;

				_pController->GetWorkSpaceMgr()->TriggerWBState(WORKBENCH_STATE::OBJECT_SELECT, true);

				L3DCursor::GetInstance()->SetType(L3DCursor::NONE, false);
				return true;
			}

			const RVector* pMousePosVec = _pController->GetHelperAction()->GetCurrentPickingPosition();
			if( pMousePosVec == NULL)
			{
				L3DCursor::GetInstance()->SetType(L3DCursor::NONE, false);
				return true;
			}

			RVector vMousePosition = *pMousePosVec;
			RVector vPos(vMousePosition);
			vPos.z += 150.f;
			L3DCursor::GetInstance()->SetTransform(vPos, RVector(100, 100, 400));

			CToolCmdMgr* pCommandManager = _pController->GetToolCmdMgr();
			if( pCommandManager->IsMouseCmdEntryL())
			{
				pLastSowedRadius = NULL;
				CEntityArea* pEntity = NULL;
				if(_pController->GetHelperAction()->GetSelectedEntityObject() && 
					(false == _pController->GetHelperAction()->GetSelectedEntityObject()->empty()) )
				{
					pEntity = dynamic_cast<CEntityArea*>(_pController->GetHelperAction()->GetSelectedEntityObject()->front());
				}

				CEntityArea* pEntityBase = dynamic_cast<CEntityArea*>(_pController->GetViewPtr<CRollupEntityObjectList>()->GetCurSelBaseEntityObject());

				if (pEntity && pEntityBase)
				{
					SElementAtXML* pRootProperty = pEntityBase->GetRootElement();
					ASSERT(pRootProperty);

					SElementAtXML* pCylinderProperty = NULL;
					//기본 베이스 EntityProperty에서 NPC Property 가져오기
					for(SElementAtXML::ElementList::iterator itr_element = pRootProperty->m_vecChildElement.begin();
						itr_element != pRootProperty->m_vecChildElement.end(); ++itr_element)
					{
						if( (*itr_element)->m_strNodeName == "CYLINDER" )
						{
							pCylinderProperty = *itr_element;
							break;
						}
					}
					_ASSERT(pCylinderProperty);

					SElementAtXML* pCylinderElement = new SElementAtXML;
					pCylinderElement->m_strNodeName = pCylinderProperty->m_strNodeName;

					double pos_x = (double)vMousePosition.x;
					double pos_y = (double)vMousePosition.y;
					double pos_z = (double)vMousePosition.z;
					pEntity->SetTempPos(&pos_x, &pos_y, &pos_z);
					CEntityObject::CopyElement(pEntity, pCylinderProperty, pCylinderElement, false);
					pEntity->ClearTempPos();

					pEntity->GetRootElement()->m_vecChildElement.push_back(pCylinderElement);
					pCylinderElement->m_pParentElement = pEntity->GetRootElement();

					_pController->GetViewPtr<CRollupEntityObjectList>()->OnLbnSelchangeEntityList();

					pLastSowedRadius = pCylinderElement;
					pLastSowedEntity = pEntity;

					pLastSowedEntity->UpdateAABB();
				}

				pCommandManager->DoneMouseCmdL();
			}

			if( _pController->m_mouseInfo.mouseState.bDrag && pLastSowedRadius )
			{
				float f = _pController->m_mouseInfo.m_movedMouseX * 20.f;
				float fNewRadius;

				for (SElementAtXML::AttributeList::iterator itr = pLastSowedRadius->m_vecAttribute.begin();
					itr != pLastSowedRadius->m_vecAttribute.end(); ++itr)
				{
					if ((*itr)->m_strAttrName == "radius")
					{
						fNewRadius = (float)(*itr)->m_vGenericNum[0];
						fNewRadius+=f;
						if(fNewRadius > 1.f)
						{
							(*itr)->m_vGenericNum[0] = (double)fNewRadius;
						}
						pLastSowedEntity->UpdateAABB();
						break;
					}
				}

				_pController->m_mouseInfo.m_movedMouseX = 0; //moved 초기화
			}
			return true;
		}
	}

	pLastSowedRadius = NULL;
	pLastSowedEntity = NULL;
	return false;
}

CLogicObject* CDoActionObjectNewImpl::OnGetReferenceObjectAndSetCursor( CControlMediator* _pController, CLogicObject* _pOldReference )
{
	CEntityObject* pEntityObject = static_cast<CEntityObject*>(_pOldReference);
	CEntityObject* pTempEntityObject = _pController->GetViewPtr<CRollupEntityObjectList>()->GetCurSelBaseEntityObject();
	if( pTempEntityObject != NULL)
	{
		if(pTempEntityObject != pEntityObject || L3DCursor::NONE == L3DCursor::GetInstance()->GetType())
		{
			pEntityObject = pTempEntityObject;
			L3DCursor::GetInstance()->SetType(L3DCursor::RCONE, false);
		}

		if(pEntityObject->GetType() == CEntityObject::ENTITY_SPAWN)
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(255, 0, 0));
		else if(pEntityObject->GetType() == CEntityObject::ENTITY_MARKER)
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(0, 255, 0));
		else if(pEntityObject->GetType() == CEntityObject::ENTITY_SENSOR)
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(0, 0, 255));
		else if(pEntityObject->GetType() == CEntityObject::ENTITY_SOUND)
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(255, 255, 255));
		else if(pEntityObject->GetType() == CEntityObject::ENTITY_AREA)
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(192, 192, 192));
		else if(pEntityObject->GetType() == CEntityObject::ENTITY_ROOM)
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(192, 192, 255));

		return pEntityObject;
	}

	return NULL;
}

bool CDoActionObjectNewImpl::OnCheckValidLocatePosition( CControlMediator* _pController, rs3::RVector& _currentCandidatePos )
{
	bool bIsVaildNavigation = true;
	if ( !g_PathfinderTest.IsVailid3DPostiion(_currentCandidatePos))
	{
		RVector vOut;
		if ( !g_PathfinderTest.GetValid3DPosition(&vOut, _currentCandidatePos))
		{
			// 커서 컬러를 검은색으로 지정.
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(0, 0, 0));
			bIsVaildNavigation = false;
		}
		else
		{
			_currentCandidatePos.z = vOut.z + 1.0f;
		}
	}

	return bIsVaildNavigation;
}

void CDoActionObjectNewImpl::OnEditSawedObject( CControlMediator* _pController, CLogicObject* _pSawedObject )
{
	CEntityObject* pEntityObjectCurrentSawed = dynamic_cast<CEntityObject*>(_pSawedObject);
	if (NULL == pEntityObjectCurrentSawed)
		return;

	//TODO, 언두시에 다음의 행위가 언두되는것이 아니고 생성자체가 언두된다.
	switch ( pEntityObjectCurrentSawed->GetType() )
	{
	case CEntityObject::ENTITY_SPAWN :
	case CEntityObject::ENTITY_SENSOR:
		{
			float f = _pController->m_mouseInfo.m_movedMouseX * 20.f;
			float fNewRadius;
			pEntityObjectCurrentSawed->GetScale(fNewRadius);
			fNewRadius += f;
			if(fNewRadius > 1.f)
			{
				pEntityObjectCurrentSawed->SetScale(fNewRadius);
			}
		}
		break;
	case CEntityObject::ENTITY_MARKER:
		{
			float fDegree = _pController->m_mouseInfo.m_movedMouseX * 0.9f;
			if( fDegree != 0 )
			{
				RMatrix matRot(RMatrix::IDENTITY);
				matRot.SetRotationZ(DegreeToRadian(fDegree));

				RVector vDir;
				if( pEntityObjectCurrentSawed->GetDirection(vDir) )
				{
					vDir = vDir * matRot;
					vDir.Normalize();
					pEntityObjectCurrentSawed->SetDirectionUp(vDir, RVector(0,0,1) );
				}
			}
		}
		break;
	case CEntityObject::ENTITY_SOUND:	// TODO : 생성 클릭 후 바로 액션을 넣을 때
		break;
	}
}

void CDoActionObjectNewImpl::OnObjectCloned( CControlMediator* _pController, CLogicObject* _pSawedObject )
{
	CEntityObject* pNewEntityObject = dynamic_cast<CEntityObject*>(_pSawedObject);
	if (NULL == pNewEntityObject)
		return;

	pNewEntityObject->SetZoneLoad(_pController->m_mapInfo.m_bSceneEditableMap);
	pNewEntityObject->SetEditable(true);

	// custom notify
	if(pNewEntityObject->GetType() == CEntityObject::ENTITY_SPAWN)
	{
		_pController->GetViewPtr<CRollupEntityObjectList>()->ChangeTrackingSpawn( (CEntitySpawn*)pNewEntityObject );
	}
	else if (pNewEntityObject->GetType() == CEntityObject::ENTITY_AREA)
	{
		SElementAtXML* pRootElement = pNewEntityObject->GetRootElement();
		_ASSERT(pRootElement);
		_ASSERT(pRootElement->m_vecChildElement.size() == 1);
		SElementAtXML* pChildElement = pRootElement->m_vecChildElement[0];
		pChildElement->DestroyElement();
		pRootElement->m_vecChildElement.clear();

		_pController->GetViewPtr<CRollupEntityObjectList>()->SetToCustomObjectNewMode();
		L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(128, 128, 128));
	}

	//Update List
	_pController->GetHelperAction()->RemoveAllSelection(true);
	_pController->GetViewPtr<CRollupEntityObjectList>()->UpdateListBox(true, false);
	_pController->GetHelperAction()->AddSelectection( pNewEntityObject, true);
	_pController->GetViewPtr<CRollupEntityObjectList>()->OnLbnSelchangeEntityList();

	if (_pController->GetViewPtr<CRollupEntityObjectList>()->IsCustomObjectNewMode())
	{
		DoPreAction(_pController);
	}
}

void CDoActionObjectSelectImpl::OnSelectChanged( CControlMediator* _pController, bool _IsEntity )
{
	if (_IsEntity)
		_pController->GetViewPtr<CRollupEntityObjectList>()->OnLbnSelchangeEntityList();
}

bool CDoActionObjectMoveImpl::DoPreAction( CControlMediator* _pController )
{
	ENTITY_OBJECTLIST* pSelectedEntityObject = _pController->GetHelperAction()->GetSelectedEntityObject();
	bool bActionEntry = _pController->GetToolCmdMgr()->IsMouseCmdEntryL();

	if (!pSelectedEntityObject)
		return false;

	if (pSelectedEntityObject->empty())
		return false;

	if (_pController->m_mouseInfo.mouseState.bDrag)
		return false;

	if (!bActionEntry)
		return false;

	CEntityObject::TYPE nType = (*(pSelectedEntityObject->begin()))->GetType();
	if (nType == CEntityObject::ENTITY_AREA || nType == CEntityObject::ENTITY_ROOM)
	{
		CEntityArea* pEntityArea = (CEntityArea*)(*(pSelectedEntityObject->begin()));

		int nCylinderIdx = pEntityArea->m_nSubCylinderSelectedIdx;
		HTREEITEM hTreeItem = _pController->GetViewPtr<CRollupEntityObjectList>()->SetAreaCylinderElement(pEntityArea);
		int nNewCylinderIdx = pEntityArea->m_nSubCylinderSelectedIdx;
		if (nCylinderIdx != nNewCylinderIdx)
		{
			_pController->GetHelperAction()->RemoveAllSelection(true);
			CControlDoAction::ObjectSelect(_pController);
		}
	}
	return false;
}

std::map< DWORD_PTR, DWORD_PTR > ns_mapObjPosOrg;
void CDoActionObjectMoveImpl::OnMove( CControlMediator* _pController, ENTITY_OBJECTLIST* _pList, const rs3::RVector3& _moveVec, CGroupedCommandBuffer* _pCmdBufferForActionEntered )
{
	size_t selected_size = _pList->size();
	for(ENTITY_OBJECTLIST::iterator it_entity = _pList->begin(); it_entity != _pList->end(); ++it_entity)
	{
		CEntityObject* pEntityObject = *it_entity;
		if (pEntityObject->GetEditable() == false)
			continue;

		RVector vPos;
		CEntityArea* pEntityArea = NULL;
		SAttributeAtXML* pCylinderPosAttribute = NULL;

		CEntityObject::TYPE nType = pEntityObject->GetType();
		if ((nType == CEntityObject::ENTITY_AREA)
		     && selected_size == 1)
		{
			pEntityArea = static_cast<CEntityArea*>(pEntityObject);

			if (pEntityArea->m_nSubCylinderSelectedIdx > -1 &&
				(int)pEntityArea->GetRootElement()->m_vecChildElement.size() > pEntityArea->m_nSubCylinderSelectedIdx )
			{
				SElementAtXML* pCurrentCylinderElement = pEntityArea->GetRootElement()->m_vecChildElement[pEntityArea->m_nSubCylinderSelectedIdx];
				double* x = NULL;
				double* y = NULL;
				double* z = NULL;
				for (SElementAtXML::AttributeList::iterator itr = pCurrentCylinderElement->m_vecAttribute.begin(); itr != pCurrentCylinderElement->m_vecAttribute.end(); ++itr)
				{
					if ((*itr)->m_strAttrName == "Position")
					{
						pCylinderPosAttribute = *itr;

						x = &(*itr)->m_vGenericNum[0];
						y = &(*itr)->m_vGenericNum[1];
						z = &(*itr)->m_vGenericNum[2];

						break;
					}
				}
				pEntityArea->SetTempPos(x, y, z);
			}
		}

		if(pEntityObject->GetPosition(vPos) == false)
			continue;

		if(_pCmdBufferForActionEntered)
		{
			if (pEntityArea)
			{
				pEntityArea->SetMoveBufferPos(vPos);
				ns_mapObjPosOrg[ DWORD_PTR(pEntityArea) ] = DWORD_PTR( pEntityArea->GetMoveBuffer() );
			}
			else
			{
				CEntityMoveCmdBuffer* pCmdBuffer = new CEntityMoveCmdBuffer(pEntityObject, vPos);
				_pCmdBufferForActionEntered->AddBuffer(pCmdBuffer);
				ns_mapObjPosOrg[ DWORD_PTR(pEntityObject) ] = DWORD_PTR( pCmdBuffer );
			}
		}

		// 원래값에서 변한만큼만 이동한다.
		CEntityMoveCmdBuffer* pCmdBuffer = (CEntityMoveCmdBuffer*)( ns_mapObjPosOrg[ DWORD_PTR(pEntityObject) ]);
		_ASSERT( pCmdBuffer != NULL );
		if (pCmdBuffer==NULL)
			continue;

		vPos = pCmdBuffer->GetPosOrg() + _moveVec;
		pEntityObject->SetPosition(vPos);

		if (NULL == pEntityArea)
			pEntityObject->SetValid(g_PathfinderTest.IsVailid3DPostiion(vPos));

		if(selected_size == 1)
		{
			_pController->GetViewPtr<CRollupEntityObjectList>()->WriteTreeViewMessage(pEntityObject->GetPosAttribute());

			if(pEntityObject->GetType() == CEntityObject::ENTITY_SPAWN)
			{
				_pController->GetViewPtr<CRollupEntityObjectList>()->ChangeTrackingSpawn( (CEntitySpawn*)pEntityObject );
			}
			else if (pEntityArea)
			{
				pEntityArea->m_CylinderPos = vPos;

				pEntityArea->ClearTempPos();
				pEntityArea->UpdateAABB();

				_pController->GetViewPtr<CRollupEntityObjectList>()->UpdateAreaCylinderUI(pCylinderPosAttribute);
			}
		}
	}
}

std::map< DWORD_PTR, DWORD_PTR > ns_mapObjRotOrg;
void CDoActionObjectRotateImpl::OnRotate( CControlMediator* _pController, ENTITY_OBJECTLIST* _pList, rs3::RMatrix& _tmForRot, UINT _nAxis, CGroupedCommandBuffer* _pCmdBufferForActionEntered )
{
	size_t selected_size = _pList->size();
	RVector vDir, vUp;
	for(ENTITY_OBJECTLIST::iterator itr = _pList->begin(); itr != _pList->end(); ++itr)
	{
		CEntityObject* pEntityObject = *itr;
		if (pEntityObject->GetEditable() == false)
			continue;

		if( pEntityObject->GetDirection(vDir) == false )
			continue;

		bool bUpEditable = pEntityObject->GetUp(vUp);
		if (bUpEditable)
		{
			// Up Editable Entity는 1개의 선택만 가능하다.
			if(1 != selected_size)
				continue;

			if (CControlDoAction::MakeRotationMatrixAndInfoText(_tmForRot, _pController, _nAxis)  == false)
				continue;

			if (_pCmdBufferForActionEntered)
			{
				CLogicRotateCmdBuffer* pCmdBuffer = new CLogicRotateCmdBuffer(pEntityObject, vDir, vUp);
				_pCmdBufferForActionEntered->AddBuffer(pCmdBuffer);

				ns_mapObjRotOrg[ DWORD_PTR(pEntityObject) ] = DWORD_PTR( pCmdBuffer );
			}

			// 원래값에서 변한만큼만 회전한다.
			CLogicRotateCmdBuffer* pCmdBuffer = (CLogicRotateCmdBuffer*)( ns_mapObjRotOrg[ DWORD_PTR(pEntityObject) ]);
			_ASSERT( pCmdBuffer != NULL );
			if (pCmdBuffer==NULL)
				continue;

			vDir = pCmdBuffer->GetRotOrgDir() * _tmForRot;
			vDir.Normalize();

			vUp = pCmdBuffer->GetRotOrgUp() * _tmForRot;
			vUp.Normalize();

			pEntityObject->SetDirectionUp(vDir, vUp);

		}
		else
		{
			if(_pCmdBufferForActionEntered)
			{
				CEntityRotateCmdBuffer* pCmdBuffer = new CEntityRotateCmdBuffer(pEntityObject, vDir);
				_pCmdBufferForActionEntered->AddBuffer(pCmdBuffer);

				ns_mapObjRotOrg[ DWORD_PTR(pEntityObject) ] = DWORD_PTR( pCmdBuffer );
			}

			// 원래값에서 변한만큼만 회전한다.
			CEntityRotateCmdBuffer* pCmdBuffer = (CEntityRotateCmdBuffer*)( ns_mapObjRotOrg[ DWORD_PTR(pEntityObject) ]);
			_ASSERT( pCmdBuffer != NULL );
			if (pCmdBuffer==NULL)
				continue;

			vDir = pCmdBuffer->GetRotOrg() * _tmForRot;
			vDir.Normalize();

			pEntityObject->SetDirectionUp(vDir, RVector(0,0,1) );
		}

		//하나의 엔티티만 선택했을경우.
		if(selected_size == 1)
		{
			CRollupEntityObjectList* pRollupEntityList = _pController->GetViewPtr<CRollupEntityObjectList>();

			pRollupEntityList->WriteTreeViewMessage(pEntityObject->GetDirAttribute());
			pRollupEntityList->WriteTreeViewMessage(pEntityObject->GetUpAttribute());

			if(pEntityObject->GetType() == CEntityObject::ENTITY_SPAWN)
				pRollupEntityList->ChangeTrackingSpawn( (CEntitySpawn*)pEntityObject );
		}
	}
}

void CDoActionObjectScaleImpl::OnScale( CControlMediator* _pController, ENTITY_OBJECTLIST* _pList, float _fScaleFactor, CGroupedCommandBuffer* _pCmdBufferForActionEntered )
{
	size_t selected_size = _pList->size();
	for(ENTITY_OBJECTLIST::iterator itr = _pList->begin(); itr != _pList->end(); ++itr)
	{
		CEntityObject* pEntityObject = *itr;
		if ( pEntityObject->GetEditable() == false)
			continue;

		float fRadius;
		if( pEntityObject->GetScale(fRadius) == false ) continue;

		float fNewRadius = fRadius * ( 1.f + _fScaleFactor );
		if( fNewRadius < 0.f ) fNewRadius = 0.f;

		if(_pCmdBufferForActionEntered)
		{
			CEntityScaleCmdBuffer* pCmdBuffer = new CEntityScaleCmdBuffer(pEntityObject, fRadius);
			_pCmdBufferForActionEntered->AddBuffer(pCmdBuffer);
		}

		pEntityObject->SetScale(fNewRadius);

		if(selected_size == 1)
		{
			CRollupEntityObjectList *pView = _pController->GetViewPtr<CRollupEntityObjectList>();

			pView->WriteTreeViewMessage(pEntityObject->GetScaleAttribute());
			if(pEntityObject->GetType() == CEntityObject::ENTITY_SOUND)
				pView->WriteTreeViewMessage((static_cast<CEntitySound*>(pEntityObject))->GetDistanceMinAttribute());
			else if(pEntityObject->GetType() == CEntityObject::ENTITY_SPAWN)
				pView->ChangeTrackingSpawn( (CEntitySpawn*)pEntityObject );
		}
	}
}

void CDoActionEtcPathSelectImpl::Act( CControlMediator* _pController )
{
	CRollupEtcPathFind* pPathFind = _pController->GetViewPtr<CRollupEtcPathFind>();

	RSceneManager *pSceneManager = _pController->GetToolSceneManager();
	const RVector *pOrig = NULL;
	const RVector *pTo = NULL;

	_pController->GetHelperAction()->GetCurrentMouseLine(&pOrig,&pTo);
	if ( pOrig == NULL || pTo == NULL )
		return;

	RPICKINFO kPickInfo;
	kPickInfo.inPickOrigin = *pOrig;
	kPickInfo.inPickTo = *pTo;
	kPickInfo.inMethod = RPM_DETAIL;

	if ( !pSceneManager->PickSceneNode(kPickInfo))
		return;

	// Finder Test
	if (pPathFind->GetFinderMode() == CRollupEtcPathFind::TEST_FIND)
	{
		CRollupEtcPathFind::WB_TYPE type = pPathFind->m_eWBType;
		if(type == CRollupEtcPathFind::SEL_START)
		{
			L3DCursor::GetInstance()->SetType(L3DCursor::RCONE, false);
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(255, 0, 255));
			RVector vPos(kPickInfo.outPosition);
			L3DCursor::GetInstance()->SetTransform(vPos, RVector(1, 1, 200));

			if(_pController->GetToolCmdMgr()->IsMouseCmdEntryL())
			{
				pPathFind->SetStartPosition( vPos.x, vPos.y, vPos.z);
				pPathFind->m_eWBType = CRollupEtcPathFind::SEL_END;
				_pController->GetToolCmdMgr()->DoneMouseCmdL();
			}
		}
		else if(type == CRollupEtcPathFind::SEL_END)
		{
			L3DCursor::GetInstance()->SetType(L3DCursor::RCONE, false);
			L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(0, 0, 0));
			RVector vPos(kPickInfo.outPosition);
			L3DCursor::GetInstance()->SetTransform(vPos, RVector(1, 1, 200));

			if(_pController->GetToolCmdMgr()->IsMouseCmdEntryL())
			{
				pPathFind->SetEndPosition( vPos.x, vPos.y, vPos.z);
				pPathFind->m_eWBType = CRollupEtcPathFind::SEL_NONE;
				L3DCursor::GetInstance()->SetType(L3DCursor::NONE, false);
				_pController->GetToolCmdMgr()->DoneMouseCmdL();
			}
		}
	}
	// Finder Sow
	else
	{
		L3DCursor::GetInstance()->SetType(L3DCursor::RCONE, false);
		L3DCursor::GetInstance()->SetColor(D3DCOLOR_XRGB(255, 255, 255));
		RVector vPos(kPickInfo.outPosition);
		vPos.z += 100;
		L3DCursor::GetInstance()->SetTransform(vPos, RVector(50, 50, 200));

		if(_pController->GetToolCmdMgr()->IsMouseCmdEntryL())
		{
			pPathFind->AddSeed(kPickInfo.outPosition);
			_pController->GetToolCmdMgr()->DoneMouseCmdL();
		}
	}
}

#pragma warning( pop )
