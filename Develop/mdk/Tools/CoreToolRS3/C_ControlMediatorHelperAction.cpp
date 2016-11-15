#include "StdAfx.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperResource.h"
#include "C_ControlMediatorHelperRender.h"

#include "M_ToolCmdMgr.h"
#include "M_CommandBuffer.h"
#include "M_LogicObjectCommandBuffer.h"
#include "M_ToolLogicObjectManager.h"

#include "C_XMLEditInfoFile.h"
#include "C_XMLUserConfig.h"

#include "RMeshUtil.h"
#include "RUtil.h"

#include "VL_SceneNodeList.h"
#include "VR_TerrainGrass.h"
#include "VL_ToolBar.h"
#include "VG_Gizmo.h"
#include "VL_Objects.h"

//#include "VR_EtcPathFind.h"
//#include "M_CollisionTree.h"

// 편집하기 위해 더해진 카메라 랜더링
#include "VR_EtcCamera.h"
#include "VR_EtcMapView.h"

#include "C_ControlCursor.h"
#include "C_MintManager.h"

#include "RActor.h"

#include "REffectSceneNode.h"

#include "WS_WorkSpaceManager.h"

#include <algorithm>


using namespace rs3;
CControlMediatorHelperAction::CControlMediatorHelperAction(void)
: m_bPause_(false)
, m_vecContactActionPlanePos_(0, 0, 0)
, m_matSelectedObjectActionFrame_(RMatrix::IDENTITY)
{
	m_pVecTerrainContactPos_ = NULL;
	m_pVecCurrentPickingPosition_ = NULL;

	m_bValidMouseLine_ = false;
	m_nSelectedLogicObjCntForDoAction = 0;
}

CControlMediatorHelperAction::~CControlMediatorHelperAction(void)
{
}

void CControlMediatorHelperAction::InitHelperAction(CControlMediator* _pMediator)
{
	m_pControlMediator_ = _pMediator;
}

void CControlMediatorHelperAction::ResetHelperAction()
{
	m_listSelectedSceneObject_.clear();
	m_listSelectedEntityObject_.clear();
	m_listSelectedEntityToCopy_.clear();
	m_listSelectedLogicObject_.clear();
}

void CControlMediatorHelperAction::SetPauseToggle()
{
	static bool pause = false;
	pause = !pause;
	m_bPause_ = pause;
}

void CControlMediatorHelperAction::AddSelectection( CLogicObject* _pLogicObject, bool _bUpdateUI)
{
	/*
	if(pSceneNode != NULL)
	{
		// Value 초기화 ( 없으면 추가된다. )
		m_listSelectedSceneObject_[pSceneNode] = (void*)NULL;

		// 선택변경후 처리할 것들
		SetSelectedSceneNodeCenter();
		m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->ObserveSelection(pSceneNode, _bUpdateUI, false);
	}
	*/

	if(_pLogicObject != NULL && _pLogicObject->GetEditable() )
	{
		_pLogicObject->SetSelection(true);
		if (_pLogicObject->IsEntityLogic())
		{
			CEntityObject* pEntityObject = (CEntityObject*)_pLogicObject;
			CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
			if (pEntityActionHelper)
			{
				if (pEntityActionHelper->OnCheckAddSelection(pEntityObject, _bUpdateUI))
				{
					m_listSelectedEntityObject_.remove(pEntityObject);
					m_listSelectedEntityObject_.push_back(pEntityObject);
				}
			}
		}
		else
		{
			m_listSelectedLogicObject_.remove(_pLogicObject);
			m_listSelectedLogicObject_.push_back(_pLogicObject);
		}

		// 선택변경후 처리할 것들
		SetSelectedObjectCenterAndActionFrame();

		m_pControlMediator_->BroadCastMessage( CoreMessage(VIEW_MESSAGE::LOGIC_SELECT, _pLogicObject) );
		m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::SELECTION_CHANGED );
	}
}

void CControlMediatorHelperAction::RemoveSelection( CLogicObject* _pLogicObject, bool _bUpdateUI)
{
	/*
	if(pSceneNode != NULL)
	{
		// Key 삭제
		if ( (!m_listSelectedSceneObject_.empty())
			&& m_listSelectedSceneObject_.find(pSceneNode) != m_listSelectedSceneObject_.end())
		{
			HTREEITEM hItem = (HTREEITEM)m_listSelectedSceneObject_[pSceneNode];
			m_listSelectedSceneObject_.erase(pSceneNode);

			// 선택변경후 처리할 것들
			SetSelectedSceneNodeCenter();

			// 지워진것을 빼줘야되는데, 따로 함수 만들지 않고 선택된 물체를 ReCheck하는 방식으로 해결
			CDlgSceneNodeList* pDlgSceneNodeList = m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>();
			pDlgSceneNodeList->RemoveSelection(hItem);
			pDlgSceneNodeList->ObserveSelection(NULL, _bUpdateUI, false);
		}
	}
	*/

	if(_pLogicObject != NULL)
	{
		_pLogicObject->SetSelection(false);
		if (_pLogicObject->IsEntityLogic())
		{
			CEntityObject* pEntityObject = (CEntityObject*)_pLogicObject;
			CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
			if (pEntityActionHelper)
			{
				pEntityActionHelper->OnRemoveSelection(pEntityObject, _bUpdateUI);
				m_listSelectedEntityObject_.remove(pEntityObject);
			}
		}
		else
		{
			m_listSelectedLogicObject_.remove(_pLogicObject);
		}

		// 선택변경후 처리할 것들
		SetSelectedObjectCenterAndActionFrame();

		m_pControlMediator_->BroadCastMessage( CoreMessage(VIEW_MESSAGE::LOGIC_UNSELECT, _pLogicObject) );
		m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::SELECTION_CHANGED );
	}
}

void CControlMediatorHelperAction::RemoveAllSelection(bool _bUpdateUI)
{
	// scene
	m_listSelectedSceneObject_.clear();


	// entity
	for (ENTITY_OBJECTLIST::iterator itr = m_listSelectedEntityObject_.begin(); itr != m_listSelectedEntityObject_.end(); ++itr)
	{
		((CLogicObject*)(*itr))->SetSelection(false);
	}
	m_listSelectedEntityObject_.clear();

	// notify to entity helper
	CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
	if (pEntityActionHelper)
	{
		pEntityActionHelper->OnRemoveSelectionAll(_bUpdateUI);
	}

	// logic
	for (LOGIC_OBJECTLIST::iterator itr = m_listSelectedLogicObject_.begin(); itr != m_listSelectedLogicObject_.end(); ++itr)
	{
		(*itr)->SetSelection(false);
	}
	m_listSelectedLogicObject_.clear();

	// 선택변경후 처리할 것들
	m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::SELECTION_CLEAR );

	SetSelectedObjectCenterAndActionFrame();
	m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->ObserveSelection(NULL, _bUpdateUI, _bUpdateUI);

	m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::SELECTION_CHANGED );
}

void CControlMediatorHelperAction::SetSelectedObjectCenterAndActionFrame()
{
	if ( m_listSelectedSceneObject_.empty() && m_listSelectedLogicObject_.empty())
		return;

	using namespace rs3;

	const float MAXIUM_VALUE = 10000000.f;

	RBoundingBox kAABB;
	kAABB.Initialize();

	// 단 하나만 선택되어있는경우는 그 위치를 중심으로 삼는다
	if( m_listSelectedSceneObject_.size() + m_listSelectedLogicObject_.size() == 1)
	{
		RVector vDir(RVector::AXISY), vUp(RVector::AXISZ);

		if( m_listSelectedSceneObject_.size() == 1 )
		{
			RSceneNode *pSceneNode = (*m_listSelectedSceneObject_.begin()).first;
			
			m_vSelectNodeSceneCenter_ = pSceneNode->GetWorldPosition();
			m_vSelectNodeCenter_ = pSceneNode->GetWorldPosition();

			if (m_pControlMediator_->GetViewPtr<CCoreToolToolBar>()->m_btnLocalAxis.isChecked())
			{
				vDir = pSceneNode->GetWorldDirection();
				vUp =  pSceneNode->GetWorldUp();
			}
		}
		else
		{
			_ASSERT( m_listSelectedLogicObject_.size() == 1 );
			CLogicObject* pLogicObject = m_listSelectedLogicObject_.front();

			RVector vecPos;
			bool bRet = pLogicObject->GetPosition(vecPos);
			_ASSERT(bRet);
			m_vSelectNodeCenter_ = vecPos;

			if (m_pControlMediator_->GetViewPtr<CCoreToolToolBar>()->m_btnLocalAxis.isChecked())
			{
				pLogicObject->GetDirection(vDir);
				pLogicObject->GetUp(vUp);
			}
		}

		m_matSelectedObjectActionFrame_.SetLocalMatrix(RVector::ZERO, vDir, vUp);
		return;
	}

	if ( !m_listSelectedSceneObject_.empty() )
	{
		SCENEOBJECTMAP::iterator it = m_listSelectedSceneObject_.begin();
		RSceneNode *pSceneNode = it->first;
		RSceneNodeUpdateVisitor visitor;
		pSceneNode->Traverse(&visitor);

		//첫번째 AABB박스
		kAABB = pSceneNode->GetAABB();

		//두번째 iterator부터 순회
		for (it++; it != m_listSelectedSceneObject_.end(); ++it )
		{
			pSceneNode = it->first;
//			pSceneNode->UpdateTransform();
//			pSceneNode->Update();
			RSceneNodeUpdateVisitor visitor;
			pSceneNode->Traverse(&visitor);
			kAABB.Add( pSceneNode->GetAABB());
		}
		
		m_vSelectNodeSceneCenter_ = kAABB.vmin + ( kAABB.vmax - kAABB.vmin )*0.5f;
		m_vSelectNodeSceneCenter_.x = min(m_vSelectNodeSceneCenter_.x, MAXIUM_VALUE);
		m_vSelectNodeSceneCenter_.y = min(m_vSelectNodeSceneCenter_.y, MAXIUM_VALUE);
		m_vSelectNodeSceneCenter_.z = min(m_vSelectNodeSceneCenter_.z, MAXIUM_VALUE);

		m_vSelectNodeSceneCenter_.x = max(m_vSelectNodeSceneCenter_.x,-MAXIUM_VALUE);
		m_vSelectNodeSceneCenter_.y = max(m_vSelectNodeSceneCenter_.y,-MAXIUM_VALUE);
		m_vSelectNodeSceneCenter_.z = max(m_vSelectNodeSceneCenter_.z,-MAXIUM_VALUE);
	}

	// logic object center
	if( !m_listSelectedLogicObject_.empty() )
	{
		// 위에서 계산한 kAABB 에 더하기 logic object 박스들..

		LOGIC_OBJECTLIST::iterator itr = m_listSelectedLogicObject_.begin();
		for(; itr != m_listSelectedLogicObject_.end(); ++itr)
		{
			CLogicObject* pLogicObject = *itr;
			kAABB.Add( pLogicObject->GetAABB());
		}
	}

	// local frame
	m_matSelectedObjectActionFrame_.MakeIdentity();

	// center
	// TODO : 더 나은 방법?
	m_vSelectNodeCenter_ = kAABB.vmin + ( kAABB.vmax - kAABB.vmin )*0.5f;

	m_vSelectNodeCenter_.x = min(m_vSelectNodeCenter_.x, MAXIUM_VALUE);
	m_vSelectNodeCenter_.y = min(m_vSelectNodeCenter_.y, MAXIUM_VALUE);
	m_vSelectNodeCenter_.z = min(m_vSelectNodeCenter_.z, MAXIUM_VALUE);

	m_vSelectNodeCenter_.x = max(m_vSelectNodeCenter_.x,-MAXIUM_VALUE);
	m_vSelectNodeCenter_.y = max(m_vSelectNodeCenter_.y,-MAXIUM_VALUE);
	m_vSelectNodeCenter_.z = max(m_vSelectNodeCenter_.z,-MAXIUM_VALUE);
}

bool CControlMediatorHelperAction::IsInSelectObjectList( const CLogicObject* _pLogicObject)
{
	// find function
//	if(_pSceneNode != NULL)
//		if ( m_listSelectedSceneObject_.find(const_cast<rs3::RSceneNode*>(_pSceneNode)) != m_listSelectedSceneObject_.end())
//			return true;

	if(_pLogicObject != NULL)
	{
		if (_pLogicObject->IsEntityLogic())
		{
			CEntityObject* pEntityObject = (CEntityObject*)_pLogicObject;
			int i = 0;
			for(ENTITY_OBJECTLIST::iterator itr = m_listSelectedEntityObject_.begin();
				itr != m_listSelectedEntityObject_.end(); ++itr)
			{
				if(pEntityObject == *itr)
				{
					if ( i != 0)
					{
						// 처음이 아닐 경우 제일 처음으로 넣어서, 빠르게 찾을 수 있도록 한다. - by pok
						m_listSelectedEntityObject_.erase(itr);
						m_listSelectedEntityObject_.push_front(const_cast<CEntityObject*>(pEntityObject));
					}
					return true;
				}
				++i;
			}
		}
		else
		{
			int i = 0;
			for(LOGIC_OBJECTLIST::iterator itr = m_listSelectedLogicObject_.begin();
				itr != m_listSelectedLogicObject_.end(); ++itr)
			{
				if(_pLogicObject == *itr)
				{
					if ( i != 0)
					{
						// 처음이 아닐 경우 제일 처음으로 넣어서, 빠르게 찾을 수 있도록 한다. - by pok
						m_listSelectedLogicObject_.erase(itr);
						m_listSelectedLogicObject_.push_front(const_cast<CLogicObject*>(_pLogicObject));
					}
					return true;
				}
				++i;
			}
		}
	}
	return false;
}

void CControlMediatorHelperAction::DeleteSelectedObject()
{
	CToolCmdMgr* pCmdMgr = m_pControlMediator_->GetToolCmdMgr();
	/*
	if ( !m_listSelectedSceneObject_.empty() )
	{
		CGroupedCommandBuffer* pGroupCmdBuffer = NULL;
		for( SCENEOBJECTMAP::iterator it = m_listSelectedSceneObject_.begin();
			it != m_listSelectedSceneObject_.end(); ++it )
		{
			rs3::RSceneNode* pSceneNode = it->first;

			// 삭제 불가능한것
			if( pSceneNode->QueryAttribute( RSNA_DO_NOT_SAVE )) continue;

			pSceneNode->RemoveFromParent();

			// 지움에 대한 언두가 가능한지 여부
			bool bUndoableSceneNode(true);
			if ( MIsDerivedFromClass( rs3::RActor, pSceneNode) )
			{
				rs3::RActor* pActor = static_cast< RActor* >( pSceneNode );
				if ( pActor->m_bSceneActor )
				{
					delete pActor;
					bUndoableSceneNode = false;
				}
			}
			else if (MIsExactlyClass( rs3::RDirectionalLightSceneNode, pSceneNode))
			{
				if ( AfxMessageBox("전역광원을 삭제하시겠습니까?", MB_YESNO) == IDYES )
				{
					delete pSceneNode;
					bUndoableSceneNode = false;
				}
			}

			if ( bUndoableSceneNode )
			{
				if( pGroupCmdBuffer == NULL )
					 pGroupCmdBuffer = new CGroupedCommandBuffer;

				CObjectDeleteCmdBuffer* pCmdBuffer = new CObjectDeleteCmdBuffer(pSceneNode);
				pGroupCmdBuffer->AddBuffer(pCmdBuffer);
			}
		}

		// undo 가능한 지운것이 있다면
		if(pGroupCmdBuffer)
			pCmdMgr->RecordCommand(pGroupCmdBuffer);

		RemoveAllSelection(true);
		m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->UpdateTreeControl();
	}
	*/

	// 둘다 실행 - 삭제
	{
		// entity object 삭제
		if(!m_listSelectedEntityObject_.empty())
		{
			CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
			if (pEntityActionHelper)
			{
				pEntityActionHelper->OnDeleteSelectedObject(m_listSelectedEntityObject_, m_listSelectedEntityToCopy_);
			}
		}

		// logic object 삭제
		if(!m_listSelectedLogicObject_.empty())
		{
			std::list< CLogicObject*> listObjectsToDelete;
			std::list< CLogicObject*> listObjectsToDeleteUnundoable;
			for(LOGIC_OBJECTLIST::iterator itr = m_listSelectedLogicObject_.begin(); itr != m_listSelectedLogicObject_.end(); ++itr)
			{
				CLogicObject* pObject = *itr;

				// 해당 오브젝트가 포함하는 다른 로직 오브젝트를 수집하여 함께 삭제, 
				// ex) 현재 자식 신노드 경우밖에 없다. 
				//     spot 라이트의 target 오브젝트 같은것에 쓸수 있을듯, 생성될때 쌍으로 생성되고, 단독으로 지워지지 않는경우

				std::list< CLogicObject*> listSubObjects;
				pObject->GatherLogicObjectsIncluding( listSubObjects );

				// 혹시 ( 포함관계의 오브젝트가 동시에 선택되어있어서 ) 앞에 이미 더해져있는 경우 제거
				for( list< CLogicObject*>::iterator itr = listSubObjects.begin(); itr != listSubObjects.end(); ++itr )
				{
					std::list<CLogicObject*>::iterator itr_finder = std::find( listObjectsToDelete.begin(), listObjectsToDelete.end(), *itr);
					if(itr_finder == listObjectsToDelete.end())
						listObjectsToDelete.push_back( *itr );
				}

				// 딸린것들이 다 지워진 이후, 원래것을 삭제하도록한다
				listObjectsToDelete.push_back( pObject);
			}

			CGroupedCommandBuffer* pGroupCmdBuffer = new CGroupedCommandBuffer;
			while( listObjectsToDelete.size() )
			{
				CLogicObject* pObject = listObjectsToDelete.front();
				listObjectsToDelete.pop_front();

				m_pControlMediator_->GetToolLogicObjectMgr()->RemoveFromManager(pObject);
// 
// 				//삭제할 대상이 copyList에 있으면 copyList 에서 제거
// 				std::list<CLogicObject*>::iterator itr_finder = std::find( m_listSelectedLogicObjectToCopy_.begin(), m_listSelectedLogicObjectToCopy_.end(), pObject);
// 				if(itr_finder != m_listSelectedLogicObjectToCopy_.end())
// 					m_listSelectedLogicObjectToCopy_.clear();

				ICommandBuffer* pUndoCmd =  pObject->CreateDeleteCmdBuffer();
				if( pUndoCmd )
					pGroupCmdBuffer->AddBuffer(pUndoCmd);
				else
				{
					listObjectsToDeleteUnundoable.push_back( pObject );
				}
			}

			if(pGroupCmdBuffer->IsEmpty())
				delete pGroupCmdBuffer;
			else
				pCmdMgr->RecordCommand(pGroupCmdBuffer);

			m_pControlMediator_->GetHelperAction()->RemoveAllSelection(false);//일단 UI는 Update 하지 않는다.

			// undo 가 불가능하여 object 직접 삭제
			while( listObjectsToDeleteUnundoable.size() )
			{
				delete listObjectsToDeleteUnundoable.front();
				listObjectsToDeleteUnundoable.pop_front();
			}

 			m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->UpdateTreeControl();

// 
// 			CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
// 			if (pEntityActionHelper)
// 			{
// 				pEntityActionHelper->OnDeleteSelectedObject(m_listSelectedLogicObject_, m_listSelectedLogicObjectToCopy_);
// 			}

		}
	}

	m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::SELECTION_CHANGED );
}

void CControlMediatorHelperAction::CopyShallowSelectedEntityObject()
{
	//존 편집일 경우
	if(m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
	{
	}
	else // 필드 편집의 경우
	{
		CString cstrId;
		CString cstrClipboard;
		m_listSelectedEntityToCopy_.clear();

		for(std::list<CEntityObject*>::iterator itr = m_listSelectedEntityObject_.begin();
			itr != m_listSelectedEntityObject_.end(); ++itr)
		{
			CLogicObject* pEntityObject = (CLogicObject*)(*itr);
			cstrId.Format("%d // ", pEntityObject->GetObjectID() );
			cstrClipboard = cstrClipboard + cstrId + pEntityObject->GetObjectName();
			cstrClipboard += "\r\n";
			m_listSelectedEntityToCopy_.push_back( (CEntityObject*)(pEntityObject) );
		}

		if(!cstrClipboard.IsEmpty() && ::OpenClipboard(m_pControlMediator_->GetWBWindowHandle()))
		{
			::EmptyClipboard();

			HGLOBAL clipbuffer;
			char * buffer;
			const char* stringbuffer = cstrClipboard.GetString();

			clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(stringbuffer)+1);
			buffer = (char*)GlobalLock(clipbuffer);
			strcpy(buffer, LPCSTR(stringbuffer));
			GlobalUnlock(clipbuffer);

			::SetClipboardData(CF_TEXT,clipbuffer);
			CloseClipboard();
		}
	}
}

void CControlMediatorHelperAction::PasteCopiedEntityObject()
{
	//씬 편집일 경우
	if(m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
	{
	}
	else
	{
		CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
		if (pEntityActionHelper)
		{
			pEntityActionHelper->OnPasteCopiedObject(m_listSelectedEntityObject_, m_listSelectedEntityToCopy_);
		}
	}
}

bool CControlMediatorHelperAction::GetSelectedObjectAABBCenter(rs3::RVector& _refOut, rs3::RBoundingBox* _pGetAABB,
	bool _bPickedPriority, const rs3::RVector* _p, const rs3::RVector* _q )
{

	RBoundingBox kAABB;
	RPICKINFO_MAP mapPickInfo;
	if( m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
	{
		if( _bPickedPriority && _p && _q &&
			m_pControlMediator_->GetToolSceneManager()->Pick(*_p, *_q, mapPickInfo, true) )
		{
			kAABB = mapPickInfo.begin()->second.outNode->GetAABB();
			_refOut = kAABB.GetCenter();
			if( _pGetAABB )
				*_pGetAABB = kAABB;
			return true;
		}
		else
		{
			rs3::RVector* pVec = GetSelectSceneNodeCenter();
			if(pVec)
			{
				_refOut = *pVec;
				if( _pGetAABB )
					*_pGetAABB = kAABB;
				return true;
			}
			else
				return false;
		}
	}
	else
	{
		//pick priority 가 있는 경우
		if( _bPickedPriority && _p && _q)
		{
			CLogicObject* pLogicObject = m_pControlMediator_->GetToolLogicObjectMgr()->PickLogicObjectForSelection( *_p, *_q );
			if(pLogicObject)
			{
				pLogicObject->GetPosition(_refOut);
				if(_pGetAABB)
					*_pGetAABB = pLogicObject->GetAABB();
				return true;
			}
		}

		//pick priority 가 없는 경우
		size_t selected_size =  m_listSelectedEntityObject_.size();
		if( selected_size == 0 )
			return false;

		std::list<CEntityObject*>::iterator it = m_listSelectedEntityObject_.begin();
		CLogicObject* pEntityObject = (CLogicObject*)(*it);
		if(selected_size == 1)
		{
			pEntityObject->GetPosition(_refOut);
			if( _pGetAABB )
				*_pGetAABB = pEntityObject->GetAABB();
			return true;
		}
		else
		{
			//첫번째 AABB박스
			kAABB = pEntityObject->GetAABB();

			//두번째 iterator부터 순회
			for (it++; it != m_listSelectedEntityObject_.end(); ++it )
			{
				pEntityObject = (CLogicObject*)(*it);
				kAABB.Add( pEntityObject->GetAABB());
			}
			_refOut = kAABB.GetCenter();
			if( _pGetAABB )
				*_pGetAABB = kAABB;
			return true;
		}
	}
}

void CControlMediatorHelperAction::GetWBObjectAtCurrentMouse(rs3::RSceneNode** _ppSceneNode, CLogicObject** _ppLogicObject) const
{
	const rs3::RVector *pOrigin(NULL), *pTo(NULL);
	GetCurrentMouseLine(&pOrigin, &pTo);
	if(pOrigin && pTo)
	{
		if(m_pControlMediator_->m_mapInfo.m_bEditSceneObject)
		{
			RPICKINFO_MAP mapPickInfo;
			// 그 라인에 물체가 걸릴때만 true
			if( m_pControlMediator_->GetToolSceneManager()->Pick(*pOrigin, *pTo, mapPickInfo, true) )
				*_ppSceneNode = mapPickInfo.begin()->second.outNode;
		}
//		else // 둘다 실행하도록 수정
		{
			*_ppLogicObject = m_pControlMediator_->GetToolLogicObjectMgr()->PickLogicObjectForSelection(*pOrigin, *pTo);
		}
	}
}

bool CControlMediatorHelperAction::CopyDeepSelectedObject()
{
	if (m_listSelectedLogicObject_.empty())
		return false;

	LOGIC_OBJECTLIST copyedObjectList;
// 	RToolSceneManager* pSceneManager = m_pControlMediator_->GetToolSceneManager();

	CGroupedCommandBuffer* pGroupedCommandBuffer = new CGroupedCommandBuffer;
	for ( LOGIC_OBJECTLIST::iterator it = m_listSelectedLogicObject_.begin(); it != m_listSelectedLogicObject_.end(); ++it )
	{
		CLogicObject *pFromObject = *it;

		CLogicObject *pToObject = pFromObject->Clone();
		if ( pToObject == NULL )
			continue;

		// give unique name to new object
		const char* szUniqueName = m_pControlMediator_->GetToolLogicObjectMgr()->GetUniqueObjectNameOf( pFromObject->GetObjectName() );
		pToObject->SetObjectName( szUniqueName );

		CDlgObjects* pDlgObjects = m_pControlMediator_->GetViewPtr<CDlgObjects>();
		pToObject->SetLayerName( pDlgObjects->GetDefaultLayerName() );

		m_pControlMediator_->GetToolLogicObjectMgr()->AddToManager(pToObject);

		copyedObjectList.push_back(pToObject);

		//undo buffer
		ICommandBuffer* pUndoCmd =  pToObject->CreateCloneNewCmdBuffer();
		if( pUndoCmd)
			pGroupedCommandBuffer->AddBuffer( pUndoCmd );
	}
	// 그룹 액션을 끝
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(pGroupedCommandBuffer);

	// selection and ui update
	RemoveAllSelection(true);
	m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->UpdateTreeControl();

	for ( LOGIC_OBJECTLIST::iterator it = copyedObjectList.begin(); it != copyedObjectList.end(); ++it )
		AddSelectection( *it, true);

	return true;
}

// TODO: scene 편집 제거후 삭제
// bool CControlMediatorHelperAction::CopyDeepSelectedZoneObject()
// {
// 	if (m_listSelectedSceneObject_.empty())
// 		return false;
// 
// 	rs3::RSceneNodeList copyedNodeList;
// 	RToolSceneManager* pSceneManager = m_pControlMediator_->GetToolSceneManager();
// 
// 	CGroupedCommandBuffer* pGroupedCommandBuffer = new CGroupedCommandBuffer;
// 	for ( SCENEOBJECTMAP::iterator it = m_listSelectedSceneObject_.begin(); it != m_listSelectedSceneObject_.end(); ++it )
// 	{
// 		rs3::RSceneNode *pFromNode = it->first;
// 
// 		// by pok. 직접적인 복사가 수행되지 않아야될것들 적어주기
// 		if ( MIsDerivedFromClass( rs3::RLightSceneNode, pFromNode) )
// 			continue;
// 		if ( MIsDerivedFromClass( rs3::REffectSceneNode, pFromNode ) )
// 			continue;
// 
// 		RSceneNode* pToNode = pFromNode->Clone();
// 		if ( pToNode == NULL )
// 			continue;
// 
// 		pToNode->SetProperty(pFromNode->GetProperty());
// 		CControlSceneProperty* pProperty = dynamic_cast<CControlSceneProperty*>((CCoreObject*)pToNode->GetProperty());
// 		pProperty->SetNewInstance(pToNode, pFromNode, true);
// 
// 		pSceneManager->AddSceneNode(pToNode, true);
// 		copyedNodeList.push_back(pToNode);
// 
// 		//undo buffer
// 		CObjectNewCmdBuffer* pCmdBuffer = new CObjectNewCmdBuffer(pToNode);
// 		pGroupedCommandBuffer->AddBuffer(pCmdBuffer);
// 	}
// 	// 그룹 액션을 끝
// 	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(pGroupedCommandBuffer);
// 
// 	// selection and ui update
// 	RemoveAllSelection(true);
// 	m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->UpdateTreeControl();
// 
// 	for ( rs3::RSceneNodeList::iterator it = copyedNodeList.begin(); it != copyedNodeList.end(); ++it )
// 		AddSelectection(*it, NULL, true);
// 
// 	return true;
// }

void CControlMediatorHelperAction::NotifyWBMouseAction( WB_MOUSE_ACTION _eAction )
{
	size_t nNum = m_vViewIDToObserveMouseAction_[_eAction].size();
	if( nNum != 0 )
	{
		VIEW_MESSAGE::ID message = _eAction == WB_MOUSE_L_DOWN? VIEW_MESSAGE::WB_MOUSE_L_DOWN : VIEW_MESSAGE::WB_MOUSE_L_UP;
		for( size_t i = 0; i < nNum; ++i)
		{
			int nId = m_vViewIDToObserveMouseAction_[_eAction][i];
			if (nId == -1)
				m_pControlMediator_->GetWorkSpaceMgr()->RoutingWBObjectEditMouseMessage(message);
			else
				m_pControlMediator_->CastMessageToView( nId , message );
		}
		m_vViewIDToObserveMouseAction_[_eAction].clear();
	}
}

void CControlMediatorHelperAction::GetMovedCursorPosFromCenter(int& _x, int& _y)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(m_pControlMediator_->m_wbhWnd_, &pt);
	_x = pt.x - m_pControlMediator_->m_wbWidth_/2;
	_y = -pt.y + m_pControlMediator_->m_wbHeight_/2;
}

bool CControlMediatorHelperAction::GetMouseLine(const LONG sx, const LONG sy, RVector& origin, RVector& to ) const
{
	if ( m_pControlMediator_->m_pCurrentCamera_ == NULL )
		return false;

	RECT rt = {0, 0, m_pControlMediator_->m_wbWidth_, m_pControlMediator_->m_wbHeight_};
	RVector vCamPosP, vMouseDir, vOnMouseRayQ;
	vCamPosP = m_pControlMediator_->m_pCurrentCamera_->GetPosition();

	//vMouseDir 구하기
	RGetScreenLine(sx, sy, rt,
		m_pControlMediator_->m_pCurrentCamera_->GetViewMatrix(),
		m_pControlMediator_->m_pCurrentCamera_->GetProjectionMatrix(),
		&vMouseDir);

	const RPlane *pPlane = &m_pControlMediator_->m_pCurrentCamera_->GetViewFrustum().GetPlanes()[RV_FAR];
	if ( !MMath::IsIntersectionPlane(vOnMouseRayQ, vCamPosP, vMouseDir, *pPlane)) //vOnMouseRayQ
		return false;

	to = vOnMouseRayQ;
	origin = vCamPosP;

	return true;
}


void CControlMediatorHelperAction::PickingCameraPlane( RVector& _vTerrainContactPos, RVector& _vMapPickPos, bool& _bRet )
{
	const RPlane *pPlane = &m_pControlMediator_->m_pCurrentCamera_->GetViewFrustum().GetPlanes()[RV_NEAR];
	D3DXPLANE pDxPlane = *pPlane;

	float dist = D3DXPlaneDotCoord( &pDxPlane, &D3DXVECTOR3(_vMapPickPos) );

	if( dist > 0 )
	{
		m_pVecCurrentPickingPosition_ = &_vMapPickPos;
		_bRet = true;
	}
	else
	{
		if( _bRet )
			m_pVecCurrentPickingPosition_ = &_vTerrainContactPos;
	}
}


bool CControlMediatorHelperAction::SetMouseLineAndPickingPosition()
{
	static RVector vTerrainContactPos(0, 0, 0);
	static RPICKINFO_MAP mapPickInfo;

	m_pVecTerrainContactPos_ = NULL;
	m_pVecCurrentPickingPosition_ = NULL;
	mapPickInfo.clear();

	// Set Current Mouse Line ---
	// 아래 사항이 참이든 거짓이든, 마우스 라인은 움직일때마다 구해진다.
	if ( !GetMouseLine(
		m_pControlMediator_->m_mouseInfo.m_currentMouseX,
		m_pControlMediator_->m_mouseInfo.m_currentMouseY,
		m_vecMouseLineOrg_, m_vecMouseLineTo_ )
		)
	{
		m_bValidMouseLine_ = false;
		return false;
	}
	m_bValidMouseLine_ = true;

	// 카메라 모드 테스트 ---
	if(m_pControlMediator_->m_bNavigation_)
		return false;

	if (m_pControlMediator_->GetHelperRender()->IsClipPlane())
	{
		MPlane plane[2];
		plane[0].SetPlane( vec3::AXISZ, vec3(0.f, 0.f, m_pControlMediator_->m_fClipNearZ));
		plane[1].SetPlane(-vec3::AXISZ, vec3(0.f, 0.f, m_pControlMediator_->m_fClipFarZ));

		rs3::RVector newMouseLineOrg;
		if(plane[0].Intersect(newMouseLineOrg, m_vecMouseLineOrg_, m_vecMouseLineTo_))
		{
			m_vecMouseLineTo_ = newMouseLineOrg;
		}
		if(plane[1].Intersect(newMouseLineOrg, m_vecMouseLineOrg_, m_vecMouseLineTo_))
		{
			m_vecMouseLineOrg_ = newMouseLineOrg;
		}
	}

	bool bRet(false);

	// 지형 Contact Position
	RToolTerrain* pTerrain = (RToolTerrain*) m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
	if( pTerrain )
	{
		if(pTerrain->GetPickLine(vTerrainContactPos, NULL, m_vecMouseLineOrg_, m_vecMouseLineTo_))
		{
			m_pVecTerrainContactPos_ = &vTerrainContactPos; //지역변수지면 Static이라 괜찮아요.
			bRet = true;
		}
	}


	if ( m_pControlMediator_->m_mouseInfo.getIgnoreMouseMovedPickingRActor() )
	{
		if( bRet )
			m_pVecCurrentPickingPosition_ = &vTerrainContactPos;
	}
	else
	{
		if( m_pControlMediator_->GetToolSceneManager()->Pick(m_vecMouseLineOrg_, m_vecMouseLineTo_, mapPickInfo, false) )
		{
			PickingCameraPlane( vTerrainContactPos,  mapPickInfo.begin()->second.outPosition, bRet );
		}
		// 물체 피킹은 실패했지만 지형피킹은 성공했을경우
		else if( bRet )
		{
			m_pVecCurrentPickingPosition_ = &vTerrainContactPos;
		}
	}

	return bRet;
}

void CControlMediatorHelperAction::SetActionPlaneAndContactPos()
{
	if ( !GetMouseLine(
		m_pControlMediator_->m_mouseInfo.m_currentMouseX,
		m_pControlMediator_->m_mouseInfo.m_currentMouseY,
		m_vecMouseLineOrg_, m_vecMouseLineTo_ )
		)
	{
		m_bValidMouseLine_ = false;
		return;
	}
	m_bValidMouseLine_ = true;
	if( !m_pControlMediator_->m_mapInfo.m_bLoaded )
		return;

	RVector vDir = -m_pControlMediator_->GetToolCamera()->GetDirection();
	SetMouseLineAndPickingPosition();
	if (m_pVecCurrentPickingPosition_)
	{
		m_actionPlane_.SetPlane( vDir, *m_pVecCurrentPickingPosition_ );
	}
	else
	{
		RVector aabbCenter;
		if(!GetSelectedObjectAABBCenter(aabbCenter, NULL, true, &m_vecMouseLineOrg_, &m_vecMouseLineTo_))
			aabbCenter.Set(0, 0, 0);

		m_actionPlane_.SetPlane( vDir, aabbCenter );
		if(m_actionPlane_.Intersect( m_vecContactActionPlanePos_, m_vecMouseLineOrg_, m_vecMouseLineTo_))
		{
			m_actionPlane_.SetPlane( vDir, m_vecContactActionPlanePos_ );
			m_pVecCurrentPickingPosition_ = &m_vecContactActionPlanePos_;
		}
	}
}

void CControlMediatorHelperAction::SetActionPlanePosAs( const rs3::RVector& vecPosition )
{
	m_vecContactActionPlanePos_ = vecPosition;
}

void CControlMediatorHelperAction::SetNavigationModeToggle(bool _bXYNavi)
{
	if(m_pControlMediator_->m_mapInfo.m_bLoaded == false)
		return;

	bool bNaviModeRequested = !m_pControlMediator_->m_bNavigation_;
	if(bNaviModeRequested)
	{
		m_pControlMediator_->m_mouseInfo.setIgnoreMouseMovedPickingRActor( false );

		SetMouseLineAndPickingPosition();
		m_pControlMediator_->m_bNavigation_ = true;

		RVector vPos(0);
		if(m_pVecCurrentPickingPosition_ != NULL)
			vPos = *m_pVecCurrentPickingPosition_;

		if (_bXYNavi)
		{
			vPos.z += m_pControlMediator_->m_pCurrentCamera_->GetPosition().z;
			m_pControlMediator_->m_pCurrentCamera_->Set(vPos, -RVector3::AXISZ, RVector3::AXISY);
		}
		else
		{
			vPos.z += Tool3Const::CONST_WB_NAVI_HEIGHT;
			RVector vTarget(vPos);

			//방향을 정해 더하기
			RVector vMoveDir(m_pControlMediator_->m_pCurrentCamera_->GetDirection());
			vMoveDir.z = 0;
			vMoveDir.Normalize();
			vTarget += vMoveDir;

			//필드일때, 마커이면 마커의 방향으로 보기
			if(m_pControlMediator_->m_mapInfo.m_bSceneEditableMap == false)
			{
				RSceneNode* pSceneNode = NULL;
				CLogicObject* pLogicObject = NULL;
				GetWBObjectAtCurrentMouse(&pSceneNode, &pLogicObject);
				if(pLogicObject != NULL)
				{
					if (pLogicObject->IsEntityLogic())
					{
						CEntityObject* pEntityObject = (CEntityObject*)pLogicObject;
						CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
						if (pEntityActionHelper)
						{
							pEntityActionHelper->OnGetNavigationDirection(pEntityObject, vPos, vTarget);
						}
					}
				}
			}

			m_pControlMediator_->m_pCurrentCamera_->SetPosAndTarget(vPos, vTarget, true);
		}

		SetCursorToScreenCenter();

		//커서 상태 청소
		L3DCursor::GetInstance()->SetType(L3DCursor::NONE, false);
		//커서 감추기
		ShowCursor(FALSE);
	}
	else
	{
		ShowCursor(TRUE);
		m_pControlMediator_->m_bNavigation_ = false;
	}
}

void CControlMediatorHelperAction::SetCursorToScreenCenter()
{
	CPoint pt(m_pControlMediator_->m_wbWidth_/2, m_pControlMediator_->m_wbHeight_/2);
	ClientToScreen(m_pControlMediator_->m_wbhWnd_, &pt);
	SetFocus(m_pControlMediator_->m_wbhWnd_);
	SetCursorPos(pt.x, pt.y);
}

void CControlMediatorHelperAction::NotifyNewEntityToolBarButtonClicked()
{
	CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
	if (pEntityActionHelper)
	{
		pEntityActionHelper->OnToolBarNewEntityClicked();
	}
}

void CControlMediatorHelperAction::NotifyEntityOnTerrainToolBarButtonClicked()
{
	CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
	if (pEntityActionHelper)
	{
		pEntityActionHelper->OnToolBarEntityOnTerrainClicked();
	}
}

void CControlMediatorHelperAction::NotifySelectionLockToolBarButtonClicked(bool _bLock)
{
	CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
	if (pEntityActionHelper)
	{
		pEntityActionHelper->OnToolBarSelectionLocked(_bLock);
	}
}

void CControlMediatorHelperAction::OnLButtonDown()
{
	SetMouseLineAndPickingPosition();

	VG_Gizmo* pGizmo = m_pControlMediator_->GetViewPtr<VG_Gizmo>();
	if(pGizmo)
	{
		if( pGizmo->GetGizmoState() != AXIS_NULL )
		{
			UINT nAxis = m_pControlMediator_->GetViewPtr<CCoreToolToolBar>()->GetCurrentAxis();
			m_nOldAxis = nAxis;

			CCoreToolToolBar::TOOLBAR_AXIS eNewAxis;
			AXIS_STATE eGizmoState = pGizmo->GetGizmoState();
			switch( eGizmoState )
			{
			case AXIS_X :
			case AXIS_Y :
			case AXIS_Z : eNewAxis = (CCoreToolToolBar::TOOLBAR_AXIS)(CCoreToolToolBar::TB_AXIS_X + (eGizmoState-AXIS_X));break;

			case AXIS_XY :
			case AXIS_YZ :
			case AXIS_XZ : eNewAxis = (CCoreToolToolBar::TOOLBAR_AXIS)(CCoreToolToolBar::TB_AXIS_XY + (eGizmoState-AXIS_XY));break;
			}

			m_pControlMediator_->GetViewPtr<CCoreToolToolBar>()->SetAxis( eNewAxis );
		}
		pGizmo->SetLockState(true);
	}
}

void CControlMediatorHelperAction::OnLButtonUp()
{
	VG_Gizmo* pGizmo = m_pControlMediator_->GetViewPtr<VG_Gizmo>();
	if(pGizmo)
	{
		if( pGizmo->GetGizmoState() != AXIS_NULL )
		{
			m_pControlMediator_->GetViewPtr<CCoreToolToolBar>()->SetAxis( (CCoreToolToolBar::TOOLBAR_AXIS)m_nOldAxis );
			m_nOldAxis = 0;
		}
		pGizmo->SetLockState(false);
	}
}

RVector* CControlMediatorHelperAction::GetSelectNodeCenter()
{
	if( m_listSelectedSceneObject_.empty() && m_listSelectedLogicObject_.empty() )
		return NULL;

	return &m_vSelectNodeCenter_;
}

void CControlMediatorHelperAction::SetVisible( CLogicObject* pLogicObject, bool bVisible )
{
	pLogicObject->SetVisible( bVisible );

	if(bVisible)
		m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_SHOW, pLogicObject ) );
	else
		m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_HIDE, pLogicObject ) );

}

void CControlMediatorHelperAction::SetEditable( CLogicObject* pLogicObject, bool bEditable )
{
	pLogicObject->SetEditable( bEditable );
}

void CControlMediatorHelperAction::UpdateSelectedObjects()
{
	for(LOGIC_OBJECTLIST::iterator itr = m_listSelectedLogicObject_.begin(); itr != m_listSelectedLogicObject_.end(); ++itr)
	{
		CLogicObject* pObj = (*itr);
		if( pObj->IsNeedUpdate() )
		{
			pObj->Update();
			m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_MOVE, pObj ) );
		}
	}
}