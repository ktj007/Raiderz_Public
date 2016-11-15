#include "stdafx.h"
#include "WS_CutSceneXCutSceneWrapper.h"
#include "WS_CutSceneObjectEntity.h"

#include "RActor.h"

#include "../../Game/GameCommon/XEffectBaseInvoker.h"

#include "WS_CutSceneObjectEntityActor.h"
#include "WS_CutSceneObjectEntityCamera.h"
#include "WS_CutSceneObjectEntityEvent.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

void CXCutSceneWrapper::RegisterCutSceneContent( XCutSceneContent* pCutSceneContent )
{
	_ASSERT(pCutSceneContent);
	m_pXCutSceneContent = pCutSceneContent;
}

void CXCutSceneWrapper::ClearCutSceneContent()
{
	delete m_pXCutSceneContent;

	m_pXCutSceneContent = NULL;
	m_pCurrentCutSceneObject = NULL;
}

int CXCutSceneWrapper::GetCutSceneObjectCount()
{
	_ASSERT(m_pXCutSceneContent);
	return (int)m_pXCutSceneContent->GetCutSceneObjects().size();
}

const std::string& CXCutSceneWrapper::GetCutSceneObjectName( int nIndex )
{
	_ASSERT(m_pXCutSceneContent);
	return m_pXCutSceneContent->GetCutSceneObjects().at(nIndex)->GetName();
}

void CXCutSceneWrapper::WriteClapperInfo( CTreeCtrl& _rTreeControl, HTREEITEM& _rNameItem, HTREEITEM& _rDurationItem )
{
	HTREEITEM hRoot = _rTreeControl.InsertItem("Clapper Board", TVI_ROOT);

	char buf[256] = {0,};

	sprintf_s(buf, "Desc : %s", m_pXCutSceneContent->GetClapper()->GetCutSceneDesc());
	_rNameItem = _rTreeControl.InsertItem(buf, hRoot);

	sprintf_s(buf, "Field : %d", m_pXCutSceneContent->GetClapper()->GetField());
	_rTreeControl.InsertItem(buf, hRoot);

	sprintf_s(buf, "Duration : %.3f", m_pXCutSceneContent->GetClapper()->GetDuration());
	_rDurationItem = _rTreeControl.InsertItem(buf, hRoot);

	_rTreeControl.Expand(hRoot, TVE_EXPAND);
}

void CXCutSceneWrapper::ResetObjectInfo( CDlgCutSceneEditor* pCutSceneEditor )
{
	_ASSERT(m_pXCutSceneContent);
	pCutSceneEditor->ClearObject();
	for (size_t i = 0; i < m_pXCutSceneContent->GetCutSceneObjects().size(); ++i)
	{
		pCutSceneEditor->AddObject( m_pXCutSceneContent->GetCutSceneObjects().at(i)->GetName(), (double)m_pXCutSceneContent->GetClapper()->GetDuration() );
	}
	pCutSceneEditor->RefreshObject();
	m_pCurrentCutSceneObject = NULL;
}

int CXCutSceneWrapper::SetCurrentCutSceneObject(int nIndex, CDlgCutSceneEditor* pCutSceneEditor, bool bTriggerShow, bool bTriggerScroll)
{
	if (NULL == m_pXCutSceneContent)
		return -1;

	// 이거는... 좀 그런데, 추가중인지, 추가완료된건지 알수 있는 방법이 없다.
	int nCurrentDoActionCnt = m_pControlMediator->GetHelperAction()->GetSelectedLogicObjectCntForDoAction();
	if (nCurrentDoActionCnt > 1)
		return -1;

	static int oldMoveKeyIdx = -1;
	static bool bResetOldMoveKeyIdx = true;
	std::pair<int, int> moveCtrlKeyPair;
	if (nIndex < 0)
	{
		LOGIC_OBJECTLIST* pLogicList =  m_pControlMediator->GetHelperAction()->GetSelectedLogicObject();
		if (1 != pLogicList->size())
		{
			if (bResetOldMoveKeyIdx)
				oldMoveKeyIdx = -1;
			//pCutSceneEditor->RememberLastControllerScrollPos();
			pCutSceneEditor->ClearSelectedController();
			return -1;
		}

		int cnt = (int)m_pXCutSceneContent->GetCutSceneObjects().size();
		for (int i = 0; i < cnt; ++i)
		{
			XCutSceneObject* pObject = m_pXCutSceneContent->GetCutSceneObjects().at(i);
			CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(pObject->GetEntity());
			if (!pEntity)
				return -1;

			moveCtrlKeyPair = pEntity->GetMoveCtrlKeyIdxPair(pLogicList->front());
			if (moveCtrlKeyPair.second > -1)
			{
				nIndex = i;
				bTriggerScroll = true;
				break;
			}
		}

		if (nIndex < 0)
			return -1;
	}

	XCutSceneObject* pOldObject = m_pCurrentCutSceneObject;
	m_pCurrentCutSceneObject = m_pXCutSceneContent->GetCutSceneObjects().at(nIndex);
	if (!m_pCurrentCutSceneObject)
		return -1;

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	if (!pEntity)
		return -1;

	int nSelectedIdx = -1;
	if (pOldObject != m_pCurrentCutSceneObject)
	{
		if (pOldObject)
		{
			CCutSceneObjectEntity* pOldEntity = (CCutSceneObjectEntity*)(pOldObject->GetEntity());
			if (pOldEntity)
			{
				pOldEntity->SelectMoveLogic(false);
			}
		}

		// set for object
		XCutSceneObject::TYPE eType = m_pCurrentCutSceneObject->GetType();
		switch (eType)
		{
		case XCutSceneObject::E_CAMERA:
			pCutSceneEditor->SelectCameraObject(nIndex, bTriggerScroll);
			break;

		case XCutSceneObject::E_ACTOR:
			pCutSceneEditor->SelectActorObject(nIndex, bTriggerScroll);
			break;

		case XCutSceneObject::E_EVENT:
			pCutSceneEditor->SelectEventObject(nIndex, bTriggerScroll);
			break;

		default:
			_ASSERT(0);
			break;
		}

		// set for controller
		pCutSceneEditor->ClearController();
		pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pCutSceneEditor, XCutSceneObjectController::E_MOVE, moveCtrlKeyPair.second, moveCtrlKeyPair.first);
		pEntity->SelectMoveLogic(true);

		nSelectedIdx = nIndex;
		oldMoveKeyIdx = moveCtrlKeyPair.second;
	}
	else if (moveCtrlKeyPair.second > -1)
	{
		if (oldMoveKeyIdx != moveCtrlKeyPair.second)
		{
			oldMoveKeyIdx = moveCtrlKeyPair.second;
			bResetOldMoveKeyIdx = false;

			pCutSceneEditor->ClearController();
			pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pCutSceneEditor, XCutSceneObjectController::E_MOVE, moveCtrlKeyPair.second, moveCtrlKeyPair.first);
			pEntity->SelectMoveLogic(true);

			bResetOldMoveKeyIdx = true;
		}
		else if (m_pControlMediator->GetHelperAction()->GetSelectedLogicObject()->empty())
		{
			oldMoveKeyIdx = -1;
		}
	}

	if (bTriggerShow)
	{
		pCutSceneEditor->ShowEditor(true);
	}

	return nSelectedIdx;
}

//////////////////////////////////////////////////////////////////////////
CControlMediator* CXCutSceneWrapper::m_pControlMediator = NULL;

XCutSceneObjectEntity* CXCutSceneWrapper::CreateCameraObjectEntity()
{
	return new CCutSceneObjectEntityCamera(m_pControlMediator);
}

XCutSceneObjectEntity* CXCutSceneWrapper::CreateActorObjectEntity( const std::string& _rStrType )
{
	return new CCutSceneObjectEntityActor(m_pControlMediator, _rStrType);
}

XCutSceneObjectEntity* CXCutSceneWrapper::CreateEventObjectEntity(MXmlElement* _pEventElement, XCutSceneContent* pCutSceneContent)
{
	return new CCutSceneObjectEntityEvent(m_pControlMediator, pCutSceneContent);
}

//////////////////////////////////////////////////////////////////////////

void CXCutSceneWrapper::RemoveCutSceneObject(int nObjectIdx)
{
	XCutSceneObject* pObjectToDelete = m_pXCutSceneContent->GetCutSceneObjects().at(nObjectIdx);
	delete pObjectToDelete;
	m_pXCutSceneContent->GetCutSceneObjects().erase( m_pXCutSceneContent->GetCutSceneObjects().begin() + nObjectIdx );
	m_pCurrentCutSceneObject = NULL;
}

void CXCutSceneWrapper::RemoveCutSceneController( int nControllerIdx, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	m_pCurrentCutSceneObject->RemoveController(nControllerIdx);

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

int CXCutSceneWrapper::GetContextMenuIdForObject()
{
	if (!m_pCurrentCutSceneObject)
		return -1;

	XCutSceneObject::TYPE eType = m_pCurrentCutSceneObject->GetType();
	switch( eType )
	{
	case XCutSceneObject::E_CAMERA:
		{
			return coretoolcutscene::E_CAMERA_OBJ;
		}
	case XCutSceneObject::E_ACTOR:
		{
			return coretoolcutscene::E_ACTOR_OBJ;
		}
	case XCutSceneObject::E_EVENT:
		{
			return coretoolcutscene::E_EVENT_OBJ;
		}
	}

	return -1;
}

int CXCutSceneWrapper::GetContextMenuIdForController(int nIdx)
{
	if (!m_pCurrentCutSceneObject)
		return -1;

	int nControllerCnt = (int)m_pCurrentCutSceneObject->GetControllers().size();
	if (nIdx < 0 || nIdx >= nControllerCnt)
		return -1;

	XCutSceneObjectController::TYPE eType = m_pCurrentCutSceneObject->GetControllers().at(nIdx)->GetType();
	switch( eType )
	{
	case XCutSceneObjectController::E_MOVE:
		{
			return coretoolcutscene::E_MOVE_CTRL;
		}
	case XCutSceneObjectController::E_DOF:
		{
			return coretoolcutscene::E_DOF_CTRL;
		}
	case XCutSceneObjectController::E_ANIMATION:
		{
			return coretoolcutscene::E_ANI_CTRL;
		}
	case XCutSceneObjectController::E_TALENT:
		{
			return coretoolcutscene::E_TALENT_CTRL;
		}
	case XCutSceneObjectController::E_BGM:
		{
			return coretoolcutscene::E_BGM_CTRL;
		}
	case XCutSceneObjectController::E_CAPTION:
		{
			return coretoolcutscene::E_CAPTION_CTRL;
		}
	case XCutSceneObjectController::E_LETTERBOX:
		{
			return coretoolcutscene::E_LETTERBOX_CTRL;
		}
	case XCutSceneObjectController::E_ENV:
		{
			return coretoolcutscene::E_ENV_CTRL;
		}
	case XCutSceneObjectController::E_EFFECT:
		{
			return coretoolcutscene::E_EFFECT_CTRL;
		}
	case XCutSceneObjectController::E_FADE:
		{
			return coretoolcutscene::E_FADE_CTRL;
		}
	}

	return -1;
}

void CXCutSceneWrapper::SetContextMenuForController( int nType, CMenu* pMenu, bool bUseKey, int nCtrlIdx )
{
	coretoolcutscene::E_CONTEXT_MENU_ID eType = (coretoolcutscene::E_CONTEXT_MENU_ID)nType;
	switch( eType )
	{
	case coretoolcutscene::E_MOVE_CTRL:
		{
			if (bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_MOVECTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_MOVECTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_MOVECTRL_DELKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_MOVECTRL_CAMHERE, MF_DISABLED | MF_GRAYED);
			}

			XCutSceneObjectControllerMove* pMoveCtrl = (XCutSceneObjectControllerMove*)m_pCurrentCutSceneObject->GetController(nCtrlIdx);
			if (pMoveCtrl->IsUsingCatmullRomInterpolation())
			{
				pMenu->CheckMenuItem(ID_INTERPOLATIONTYPE_CATMULL, MF_CHECKED);
			}
			else
			{
				pMenu->CheckMenuItem(ID_INTERPOLATIONTYPE_LINEAR, MF_CHECKED);
			}

			if (pMoveCtrl->IsUsingCollision())
			{
				pMenu->CheckMenuItem(ID_CUTSCENE_MOVECTRL_COLLISION, MF_CHECKED);
			}

			return;
		}

	case coretoolcutscene::E_DOF_CTRL:
		{
			if (bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_DOFCTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_DOFCTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_DOFCTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}

	case coretoolcutscene::E_ANI_CTRL:
		{
			if (bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_ANICTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_ANICTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_ANICTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}
	case coretoolcutscene::E_BGM_CTRL:
		{
			if (bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_BGMCTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_BGMCTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_BGMCTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}
	case coretoolcutscene::E_CAPTION_CTRL:
		{
			if (bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_CAPTIONCTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_CAPTIONCTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_CAPTIONCTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}
	case coretoolcutscene::E_LETTERBOX_CTRL:
		{
			if (bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_LETTERBOXCTRL_STARTADDKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_LETTERBOXCTRL_ENDADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_LETTERBOXCTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_LETTERBOXCTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}
	case coretoolcutscene::E_ENV_CTRL:
		{
			if (bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_ENVCTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_ENVCTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_ENVCTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}
	case coretoolcutscene::E_EFFECT_CTRL:
		{
			if(bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_EFFECTCTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_EFFECTCTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_EFFECTCTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}
	case coretoolcutscene::E_FADE_CTRL:
		{
			if(bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_FADECTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_FADECTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_FADECTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}
	case coretoolcutscene::E_TALENT_CTRL:
		{
			if (bUseKey)
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_TALENTCTRL_ADDKEY, MF_DISABLED | MF_GRAYED);
			}
			else
			{
				pMenu->EnableMenuItem(ID_CUTSCENE_TALENTCTRL_EDITKEY, MF_DISABLED | MF_GRAYED);
				pMenu->EnableMenuItem(ID_CUTSCENE_TALENTCTRL_DELKEY, MF_DISABLED | MF_GRAYED);
			}
			return;
		}
	}
}

void CXCutSceneWrapper::AddMoveController(CDlgCutSceneEditor* pEditor)
{
	AddController<XCutSceneObjectControllerMove>(pEditor);
}

void CXCutSceneWrapper::RemoveMoveControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerMove>(nControllerIdx, fKeyValue, pEditor);
}

bool CXCutSceneWrapper::AddMoveControllerKey( int nControllerIdx, float fTime, const rs3::RVector3& vPos, const rs3::RVector3& vDir, const rs3::RVector3& vUp, bool bWarp, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return false;

	if (nControllerIdx < 0 || nControllerIdx >= (int)m_pCurrentCutSceneObject->GetControllers().size())
		return false;

	XCutSceneObjectControllerMove* pMove = dynamic_cast<XCutSceneObjectControllerMove*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pMove)
		return false;

	pMove->AddKey(fTime, vPos, vDir, vUp, bWarp);
	pMove->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
	return true;
}

bool CXCutSceneWrapper::GetMoveControllerKey( int nControllerIdx, float fKeyValue, float& fTime, rs3::RVector3& vPos, rs3::RVector3& vDir, rs3::RVector3& vUp, bool& bWarp )
{
	if (!m_pCurrentCutSceneObject)
		return false;

	if (nControllerIdx < 0)
		return false;

	XCutSceneObjectControllerMove* pMove = dynamic_cast<XCutSceneObjectControllerMove*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pMove)
		return false;

	int nIndex = pMove->GetKeyIndex(fKeyValue);
	if (nIndex < 0)
		return false;

	pMove->GetKeyInfo( nIndex, fTime, vPos, vDir, vUp, bWarp);
	return true;
}

void CXCutSceneWrapper::ChangeMoveControllerKey( int nControllerIdx, float fKeyValue, float fTime, const rs3::RVector3& vPos, const rs3::RVector3& vDir, const rs3::RVector3& vUp, bool bWarp, CDlgCutSceneEditor* pEditor, bool bSelectKey )
{
	if (!m_pCurrentCutSceneObject)
		return;

	if (nControllerIdx < 0)
		return;

	XCutSceneObjectControllerMove* pMove = dynamic_cast<XCutSceneObjectControllerMove*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pMove)
		return;

	int nMoveKeyIndex = pMove->GetKeyIndex(fKeyValue);
	if (nMoveKeyIndex < 0)
		return;

	pMove->SetKeyInfo( nMoveKeyIndex, fTime, vPos, vDir, vUp, bWarp );

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();

	if (false == bSelectKey)
		nMoveKeyIndex = -1;

	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_MOVE, nMoveKeyIndex, nControllerIdx);
}

void CXCutSceneWrapper::RenderCutSceneActors(bool bRunningCutScenePreview)
{
	if (!m_pXCutSceneContent)
		return;

	rs3::RVector3 vPos;
	rs3::RVector3 vDir;

	VEC_CUTSCNEOBJ& rObjectList = m_pXCutSceneContent->GetCutSceneObjects();
	for (VEC_CUTSCNEOBJ::iterator itr = rObjectList.begin(); itr != rObjectList.end(); ++itr)
	{
		CCutSceneObjectEntity* pCutSceneObjectEntity = dynamic_cast<CCutSceneObjectEntity*>( (*itr)->GetEntity() );
		if (!pCutSceneObjectEntity)
			continue;

		rs3::RSceneNode* pSceneNodeToRender = pCutSceneObjectEntity->GetSceneNodeToRender();
		if (!pSceneNodeToRender)
			continue;

		if (!bRunningCutScenePreview)
		{
			CLogicObject* pLogicObject = pCutSceneObjectEntity->GetFirstMoveKeyLogicObject();
			if (!pLogicObject)
				continue;

			bool bRet = pLogicObject->GetPosition(vPos);
			if (!bRet)
				continue;

			bRet = pLogicObject->GetDirection(vDir);
			if (!bRet)
				continue;

			pSceneNodeToRender->SetTransform(vPos, -vDir);
			pSceneNodeToRender->UpdateTransform();
		}

		m_pControlMediator->GetHelperRender()->RegisterToInstantRenderSceneNode(pSceneNodeToRender);
	}
}

void CXCutSceneWrapper::RunCutSceneContent(float fElapsedTime)
{
	if (fElapsedTime < 0)
		fElapsedTime = (float)rs3::REngine::GetTimer().GetElapsedTime() * 0.001f;

	m_pXCutSceneContent->Run(fElapsedTime);
}

void CXCutSceneWrapper::ResetRunning()
{
	m_pXCutSceneContent->ResetRunning();
}

void CXCutSceneWrapper::GetRunningTime( float& _fDuration, float& _fCurrent )
{
	_fDuration = m_pXCutSceneContent->GetClapper()->GetDuration();
	_fCurrent = m_pXCutSceneContent->GetCurrnetRunningTime();
}

bool CXCutSceneWrapper::SaveCutSceneContent( const char* pSzFileName )
{
	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement* pRoot = new MXmlElement("maiet");
	xml.Doc()->LinkEndChild(pRoot);

	MXmlElement* pClapperElement = new MXmlElement( "clapper" );
	_SetAttribute(pClapperElement, "desc", MLocale::ConvAnsiToUTF8(m_pXCutSceneContent->GetClapper()->GetCutSceneDesc()).c_str());
	_SetAttribute(pClapperElement, "field", m_pXCutSceneContent->GetClapper()->GetField());
	_SetAttribute(pClapperElement, "duration", m_pXCutSceneContent->GetClapper()->GetDuration());
	_SetAttribute(pClapperElement, "in", "none");
	_SetAttribute(pClapperElement, "out", "none");
	pRoot->LinkEndChild(pClapperElement);

	VEC_CUTSCNEOBJ& rObjectList = m_pXCutSceneContent->GetCutSceneObjects();
	for (VEC_CUTSCNEOBJ::iterator itr = rObjectList.begin(); itr != rObjectList.end(); ++itr)
	{
		XCutSceneObject* pObject = *itr;
		MXmlElement* pObjectElement = new MXmlElement( pObject->GetTypeStr() );
		pRoot->LinkEndChild(pObjectElement);
		pObject->AddAttributeOnSave(pObjectElement);

		VEC_CUTSCENE_CONTROLLERS& rControllers = pObject->GetControllers();
		for (VEC_CUTSCENE_CONTROLLERS::iterator itr_ctrl = rControllers.begin(); itr_ctrl != rControllers.end(); ++itr_ctrl)
		{
			(*itr_ctrl)->SaveToXML(pObjectElement);
		}
	}

	return xml.SaveFile(pSzFileName);
}

void CXCutSceneWrapper::ChangeMoveControllerToCatmullRom( int nCtrlId, bool bCatmullRom, bool* bPrevCatmullOut )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerMove* pMoveCtrl = dynamic_cast<XCutSceneObjectControllerMove*>(m_pCurrentCutSceneObject->GetController(nCtrlId));
	if(!pMoveCtrl)
		return;

	if (bPrevCatmullOut)
	{
		*bPrevCatmullOut = pMoveCtrl->IsUsingCatmullRomInterpolation();
	}
	pMoveCtrl->SetUsingCatmullRomInterpolation(bCatmullRom);
}

int CXCutSceneWrapper::ToggleMoveControllerCollision( int nCtrlId )
{
	if (!m_pCurrentCutSceneObject)
		return -1;

	XCutSceneObjectControllerMove* pMoveCtrl = dynamic_cast<XCutSceneObjectControllerMove*>(m_pCurrentCutSceneObject->GetController(nCtrlId));
	if(!pMoveCtrl)
		return -1;

	bool bUsingCollision = !pMoveCtrl->IsUsingCollision();
	pMoveCtrl->SetUsingCollision(bUsingCollision);

	return (bUsingCollision? 1 : 0);
}

void CXCutSceneWrapper::AddDOFController( CDlgCutSceneEditor* pEditor )
{
	AddController<XCutSceneObjectControllerDOF>(pEditor);
}

void CXCutSceneWrapper::AddDOFControllerKey( int nControllerIdx, float fTime, float fFocusDist, float fFocusRange, float fCocSize, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerDOF* pDof = dynamic_cast<XCutSceneObjectControllerDOF*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pDof)
		return;

	pDof->AddKey(fTime, fFocusDist, fFocusRange, fCocSize);
	pDof->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

bool CXCutSceneWrapper::GetDOFControllerKey( int nControllerIdx, float fKeyValue, float& fTime, float& fFocusDist, float& fFocusRange, float& fCocSize )
{
	if (!m_pCurrentCutSceneObject)
		return false;

	if (nControllerIdx < 0)
		return false;

	XCutSceneObjectControllerDOF* pDof = dynamic_cast<XCutSceneObjectControllerDOF*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pDof)
		return false;

	int nKeyIndex = pDof->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return false;

	pDof->GetKeyInfo(nKeyIndex, fTime, fFocusDist, fFocusRange, fCocSize);
	return true;
}

void CXCutSceneWrapper::RemoveDOFControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerDOF>(nControllerIdx, fKeyValue, pEditor);
}

void CXCutSceneWrapper::ChangeDOFControllerKey( int nControllerIdx, float fKeyValue, float fTime, float fFocusDist, float fFocusRange, float fCocSize, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerDOF* pDof = dynamic_cast<XCutSceneObjectControllerDOF*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pDof)
		return;

	int nKeyIndex = pDof->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return;

	pDof->SetKeyInfo( nKeyIndex, fTime, fFocusDist, fFocusRange, fCocSize);
	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_DOF, nKeyIndex, nControllerIdx);
}

void CXCutSceneWrapper::SetActorTypeAndName( const char* _pSzObjectType, const char* _pSzActorName )
{
	if (!m_pCurrentCutSceneObject)
		return;

	if (m_pCurrentCutSceneObject->GetType() != XCutSceneObject::E_ACTOR)
		return;

	XCutSceneObjectActor* pActor = (XCutSceneObjectActor*)m_pCurrentCutSceneObject;
	pActor->SetType(_pSzObjectType);
	pActor->SetName(_pSzActorName);

	CCutSceneObjectEntityActor* pActorEntity = dynamic_cast<CCutSceneObjectEntityActor*>(pActor->GetEntity());
	_ASSERT(pActorEntity);
	pActorEntity->ResetEntityActorSceneNode(_pSzObjectType);
}

void CXCutSceneWrapper::GetActorTypeAndName(std::string& strType, std::string& strName)
{
	if (!m_pCurrentCutSceneObject)
		return;

	if (m_pCurrentCutSceneObject->GetType() != XCutSceneObject::E_ACTOR)
		return;

	XCutSceneObjectActor* pActor = (XCutSceneObjectActor*)m_pCurrentCutSceneObject;
	strType = pActor->GetType();
	strName = pActor->GetName();
}

void CXCutSceneWrapper::AddAnimationController( CDlgCutSceneEditor* pEditor )
{
	AddController<XCutSceneObjectControllerAnimation>(pEditor);
}

void CXCutSceneWrapper::AddAniControllerKey( int nControllerIdx, float fTime, const std::string& _rAniName, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerAnimation* pAni = dynamic_cast<XCutSceneObjectControllerAnimation*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pAni)
		return;

	pAni->AddKey(fTime, _rAniName);
	pAni->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

bool CXCutSceneWrapper::GetAniControllerKey( int nControllerIdx, float fKeyValue, float& fTime, std::string& _rAniName )
{
	if (!m_pCurrentCutSceneObject)
		return false;

	XCutSceneObjectControllerAnimation* pAni = dynamic_cast<XCutSceneObjectControllerAnimation*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pAni)
		return false;

	int nKeyIndex = pAni->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return false;

	pAni->GetKeyInfo( nKeyIndex, fTime, _rAniName );
	return true;
}

void CXCutSceneWrapper::RemoveAniControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerAnimation>(nControllerIdx, fKeyValue, pEditor);
}

void CXCutSceneWrapper::ChangeAniControllerKey( int nControllerIdx, float fKeyValue, float fTime, const std::string& _rAniName, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerAnimation* pAni = dynamic_cast<XCutSceneObjectControllerAnimation*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pAni)
		return;

	int nKeyIndex = pAni->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return;

	pAni->SetKeyInfo( nKeyIndex, fTime, _rAniName );

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_ANIMATION, nKeyIndex, nControllerIdx);
}

void CXCutSceneWrapper::AddTalentController( CDlgCutSceneEditor* pEditor )
{
	AddController<XCutSceneObjectControllerTalent>(pEditor);
}

void CXCutSceneWrapper::AddTalentControllerKey( int nControllerIdx, float fTime, int nID, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerTalent* pTalent = dynamic_cast<XCutSceneObjectControllerTalent*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pTalent)
		return;

	pTalent->AddKey(fTime, nID);
	pTalent->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

bool CXCutSceneWrapper::GetTalentControllerKey( int nControllerIdx, float fKeyValue, float& fTime, int& nID )
{
	if (!m_pCurrentCutSceneObject)
		return false;

	XCutSceneObjectControllerTalent* pTalent = dynamic_cast<XCutSceneObjectControllerTalent*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pTalent)
		return false;

	int nKeyIndex = pTalent->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return false;

	pTalent->GetKeyInfo( nKeyIndex, fTime, nID );
	return true;
}

void CXCutSceneWrapper::RemoveTalentControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerTalent>(nControllerIdx, fKeyValue, pEditor);
}

void CXCutSceneWrapper::ChangeTalentControllerKey( int nControllerIdx, float fKeyValue, float fTime, const int nID, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerTalent* pTalent = dynamic_cast<XCutSceneObjectControllerTalent*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pTalent)
		return;

	int nKeyIndex = pTalent->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return;

	pTalent->SetKeyInfo( nKeyIndex, fTime, nID );

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_TALENT, nKeyIndex, nControllerIdx);
}

CLogicObject* CXCutSceneWrapper::GetCurrentCutSceneObjectFirstMoveKey()
{
	if (!m_pCurrentCutSceneObject)
	{
		return NULL;
	}

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	if (!pEntity)
	{
		return NULL;
	}

	return pEntity->GetFirstMoveKeyLogicObject();
}

void CXCutSceneWrapper::AddBGMController( CDlgCutSceneEditor* pEditor )
{
	std::set<int> uids;
	VEC_CUTSCENE_CONTROLLERS& ctrls = m_pCurrentCutSceneObject->GetControllers();
	for (VEC_CUTSCENE_CONTROLLERS::iterator itr = ctrls.begin(); itr != ctrls.end(); ++itr)
	{
		XCutSceneObjectController* pCtrl = *itr;
		if (pCtrl->GetType() == XCutSceneObjectController::E_BGM)
		{
			int i = static_cast<XCutSceneObjectControllerBGM*>(pCtrl)->GetUID();
			if (i<-1)
				continue;

			_ASSERT(uids.find(i) == uids.end());
			uids.insert(i);
		}
	}

	int uid = 0;
	if (!uids.empty())
	{
		int i = 0;
		while(true)
		{
			if (uids.find(i) == uids.end())
			{
				uid = i;
				break;
			}
			++i;
		}
	}

	XCutSceneObjectControllerBGM* pBgmCtrl = AddController<XCutSceneObjectControllerBGM>(pEditor);
	if (!pBgmCtrl)
		return;

	pBgmCtrl->SetUID(uid);
}

void CXCutSceneWrapper::AddBGMControllerKey( int nControllerIdx, float fTime, const std::string& _rFileName, float fVolume, bool bLoop, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerBGM* pBgm = dynamic_cast<XCutSceneObjectControllerBGM*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pBgm)
		return;

	pBgm->AddKey(fTime, _rFileName, bLoop, fVolume);
	pBgm->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

bool CXCutSceneWrapper::GetBGMControllerKey( int nControllerIdx, float fKeyValue, float& fTime, std::string& _rFileName, float &fVolume, bool &bLoop )
{
	if (!m_pCurrentCutSceneObject)
		return false;

	if (nControllerIdx < 0)
		return false;

	XCutSceneObjectControllerBGM* pBgm = dynamic_cast<XCutSceneObjectControllerBGM*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pBgm)
		return false;

	int nKeyIndex = pBgm->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return false;

	pBgm->GetKeyInfo( nKeyIndex, fTime, _rFileName, bLoop, fVolume);
	return true;
}

bool CXCutSceneWrapper::GetCaptionControllerKey( int nControllerIdx, float fKeyValue, float& fTime, float &fDurationTime, std::string& _rScrip )
{
	if (!m_pCurrentCutSceneObject)
		return false;


	if (nControllerIdx < 0)
		return false;

	XCutSceneObjectControllerCaption* pCaption = dynamic_cast<XCutSceneObjectControllerCaption*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pCaption)
		return false;

	int nKeyIndex = pCaption->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return false;

	pCaption->GetKeyInfo( nKeyIndex, fTime, fDurationTime, _rScrip);
	return true;
}

void CXCutSceneWrapper::RemoveBGMControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerBGM>(nControllerIdx, fKeyValue, pEditor);
}

void CXCutSceneWrapper::ChangeBGMControllerKey(int nControllerIdx, float fKeyValue, float fTime, const std::string& _rFileName, float fVolume, bool bLoop, CDlgCutSceneEditor* pEditor)
{
	if (!m_pCurrentCutSceneObject)
		return;

	if (nControllerIdx < 0)
		return;

	XCutSceneObjectControllerBGM* pBgm = dynamic_cast<XCutSceneObjectControllerBGM*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pBgm)
		return;

	int nKeyIndex = pBgm->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return;

	pBgm->SetKeyInfo( nKeyIndex, fTime, _rFileName, bLoop, fVolume);

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_BGM, nKeyIndex, nControllerIdx);
}

void CXCutSceneWrapper::AddCaptionController( CDlgCutSceneEditor* pEditor, const std::string& strFont, int nSize, bool bBold, bool bItalic )
{
	XCutSceneObjectControllerCaption* pCaptionCtrl = AddController<XCutSceneObjectControllerCaption>(pEditor);
	if (!pCaptionCtrl)
		return;

	pCaptionCtrl->SetFontName(strFont);
	pCaptionCtrl->SetSize(nSize);
	pCaptionCtrl->SetBold(bBold);
	pCaptionCtrl->SetItalic(bItalic);
}

void CXCutSceneWrapper::ChangeCaptionControllerKey( int nControllerIdx, float fKeyValue, float fTime, float fDurationTime, const std::string& _rScrip, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerCaption* pCaption = dynamic_cast<XCutSceneObjectControllerCaption*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pCaption)
		return;

	int nKeyIndex = pCaption->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return;

	pCaption->SetKeyInfo( nKeyIndex, fTime, fDurationTime, _rScrip);

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_CAPTION, nKeyIndex, nControllerIdx);
}

void CXCutSceneWrapper::RemoveCaptionControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerCaption>(nControllerIdx, fKeyValue, pEditor);
}

void CXCutSceneWrapper::AddCaptionControllerKey( int nControllerIdx, float fTime, float fDurationTime, const std::string& _rScrip, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerCaption* pCaption = dynamic_cast<XCutSceneObjectControllerCaption*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pCaption)
		return;

	pCaption->AddKey(fTime, fDurationTime, _rScrip);
	pCaption->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

void CXCutSceneWrapper::AddLetterBoxController( CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	// 이미 존재 하는지 체크
	VEC_CUTSCENE_CONTROLLERS& vecCutScene =	m_pCurrentCutSceneObject->GetControllers();
	for(VEC_CUTSCENE_CONTROLLERS::iterator itCutSceneControllers = vecCutScene.begin(); itCutSceneControllers != vecCutScene.end(); ++itCutSceneControllers)
	{
		if((*itCutSceneControllers)->GetType() == XCutSceneObjectController::E_LETTERBOX)
		{
			// 있다. 메세지창
			AfxMessageBox( "레터박스가 이미 등록되어 있습니다.");
			return;
		}
	}

	// 없다면 생성
	XCutSceneObjectControllerLetterBox* pLetterBoxCtrl = AddController<XCutSceneObjectControllerLetterBox>(pEditor);
	if (!pLetterBoxCtrl)
		return;

	// 키 자동 입력
	pLetterBoxCtrl->AddKey(XCutSceneObjectControllerLetterBox::LBT_START, 0.0f);
	pLetterBoxCtrl->AddKey(XCutSceneObjectControllerLetterBox::LBT_END, m_pXCutSceneContent->GetClapper()->GetDuration());
	pLetterBoxCtrl->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

void CXCutSceneWrapper::GetCaptionController( int nControllerIdx, std::string& strOutFont, int& nOutSize, bool& bOutBold, bool& bOutItalic )
{
	if (!m_pCurrentCutSceneObject)
		return;

	if (nControllerIdx < 0)
		return;

	XCutSceneObjectControllerCaption* pCaption = dynamic_cast<XCutSceneObjectControllerCaption*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pCaption)
		return;

	strOutFont	= pCaption->GetFontName();
	nOutSize	= pCaption->GetSize();
	bOutBold	= pCaption->GetBold();
	bOutItalic	= pCaption->GetItalic();
}

void CXCutSceneWrapper::ChangeCaptionController( int nControllerIdx, const std::string& strFont, int nSize, bool bBold, bool bItalic, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerCaption* pCaption = dynamic_cast<XCutSceneObjectControllerCaption*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pCaption)
		return;

	pCaption->SetFontName(strFont);
	pCaption->SetSize(nSize);
	pCaption->SetBold(bBold);
	pCaption->SetItalic(bItalic);
}

void CXCutSceneWrapper::ShowNoticeForTool(bool bShow)
{
	CMintManager::GetReference().ShowNoticeLabel(bShow);
}

CUTSCENE_OBJECT_STATE CXCutSceneWrapper::CheckCutSceneObjectControllerState( int nIndex )
{
	_ASSERT(m_pXCutSceneContent);
	VEC_CUTSCENE_CONTROLLERS vecControllers = m_pXCutSceneContent->GetCutSceneObjects().at(nIndex)->GetControllers();

	bool bEmpty = false;
	bool bChek = false;
	int nSumKeyCount = 0;
	for(VEC_CUTSCENE_CONTROLLERS::iterator itController = vecControllers.begin(); itController != vecControllers.end(); ++itController)
	{
		int nKeyCount = (*itController)->GetKeyCount();

		// 이동과 애니메이션에 대한것만 체크합니다.
		switch((*itController)->GetType())
		{
		case XCutSceneObjectController::E_MOVE:
		case XCutSceneObjectController::E_ANIMATION:
		case XCutSceneObjectController::E_TALENT:
			{
				if(nKeyCount == 0)
					bEmpty = true;

				nSumKeyCount += nKeyCount;

				bChek = true;
			}
			break;
		}

	}

	if(bChek)
	{
		if(nSumKeyCount == 0)
			return CSBS_ERROR;
		if(bEmpty)
			return CSBS_WARRING;
	}

	return CSBS_OK;
}

void CXCutSceneWrapper::AddCutSceneObject( const char* _pSzCutSceneObjectType, const char* _pSzActorName )
{
	if (NULL == m_pXCutSceneContent)
		return;

	XCutSceneObjectActor* pActorObject = new XCutSceneObjectActor( CreateActorObjectEntity(_pSzCutSceneObjectType), m_pXCutSceneContent->GetClapper() );
	pActorObject->SetType(_pSzCutSceneObjectType);
	pActorObject->SetName( _pSzActorName );

	m_pXCutSceneContent->AddCutSceneObject(pActorObject);
}

void CXCutSceneWrapper::AddEnvController( CDlgCutSceneEditor* pEditor )
{
	AddController<XCutSceneObjectControllerEnv>(pEditor);
}

void CXCutSceneWrapper::AddEnvControllerKey( int nControllerIdx, float fTime, int nEnvTime, int nEnvWeather, const std::string& _rSetName, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerEnv* pEnv = dynamic_cast<XCutSceneObjectControllerEnv*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pEnv)
		return;

	pEnv->AddKey(fTime, nEnvTime, nEnvWeather, _rSetName);
	pEnv->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

bool CXCutSceneWrapper::GetEnvControllerKey( int nControllerIdx, float fKeyValue, float& fTime, int& nEnvTime, int& nEnvWeather, std::string& _rSetName )
{
	if (!m_pCurrentCutSceneObject)
		return false;

	if (nControllerIdx < 0)
		return false;

	XCutSceneObjectControllerEnv* pEnv = dynamic_cast<XCutSceneObjectControllerEnv*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pEnv)
		return false;

	int nKeyIndex = pEnv->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return false;

	pEnv->GetKeyInfo( nKeyIndex, fTime, nEnvTime, nEnvWeather, _rSetName );
	return true;
}

void CXCutSceneWrapper::RemoveEnvControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerEnv>(nControllerIdx, fKeyValue, pEditor);
}

void CXCutSceneWrapper::ChangeEnvControllerKey( int nControllerIdx, float fKeyValue, float fTime, int nEnvTime, int nEnvWeather, const std::string& _rSetName, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerEnv* pEnv = dynamic_cast<XCutSceneObjectControllerEnv*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pEnv)
		return;

	int nKeyIndex = pEnv->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return;

	pEnv->SetKeyInfo( nKeyIndex, fTime, nEnvTime, nEnvWeather, _rSetName );

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_ENV, nKeyIndex, nControllerIdx);
}

void CXCutSceneWrapper::AddEffectController( CDlgCutSceneEditor* pEditor )
{
	AddController<XCutSceneObjectControllerEffect>(pEditor);
}

void CXCutSceneWrapper::AddEffectControllerKey( int nControllerIdx, float fTime, float fDurationTime, const vec3& vPos, const vec3& vDir, const vec3& vUp, const std::string& _rEffectName, const std::string& _rEffectInfo, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerEffect* pEffect = dynamic_cast<XCutSceneObjectControllerEffect*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pEffect)
		return;

	pEffect->AddKey(fTime, fDurationTime, _rEffectName, _rEffectInfo, vPos, vDir, vUp);
	pEffect->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

bool CXCutSceneWrapper::GetEffectControllerKey( int nControllerIdx, float fKeyValue, float& fTime, float& fDurationTime, vec3& vPos, vec3& vDir, vec3& vUp, std::string& _rEffectName, std::string& _rEffectInfo )
{
	if (!m_pCurrentCutSceneObject)	return false;
	if (nControllerIdx < 0)			return false;

	XCutSceneObjectControllerEffect* pEffect = dynamic_cast<XCutSceneObjectControllerEffect*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pEffect)	return false;

	int nKeyIndex = pEffect->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)	return false;

	pEffect->GetKeyInfo( nKeyIndex, fTime, fDurationTime, vPos, vDir, vUp, _rEffectName, _rEffectInfo);
	return true;
}

void CXCutSceneWrapper::RemoveEffectControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerEffect>(nControllerIdx, fKeyValue, pEditor);
}

void CXCutSceneWrapper::ChangeEffectControllerKey( int nControllerIdx, float fKeyValue, float fTime, float fDurationTime, const vec3& vPos, const vec3& vDir, const vec3& vUp, const std::string& _rEffectName, const std::string& _rEffectInfo, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerEffect* pEffect = dynamic_cast<XCutSceneObjectControllerEffect*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pEffect)
		return;

	int nKeyIndex = pEffect->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return;

	pEffect->SetKeyInfo( nKeyIndex, fTime, fDurationTime, vPos, vDir, vUp, _rEffectName, _rEffectInfo );

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_EFFECT, nKeyIndex, nControllerIdx);
}

void CXCutSceneWrapper::MoveCameraToMoveKeyAndSelectIt( int nCtrlId, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerMove* pMoveCtrl = dynamic_cast<XCutSceneObjectControllerMove*>(m_pCurrentCutSceneObject->GetController(nCtrlId));
	if(!pMoveCtrl)
		return;

	int nKeyIdx = pMoveCtrl->GetKeyIndex(fKeyValue);
	if(nKeyIdx < 0)
		return;

	float fTime;
	MVector3 vPos, vDir, vUp;
	bool bWarp;

	pMoveCtrl->GetKeyInfo(nKeyIdx, fTime, vPos, vDir, vUp, bWarp);
	m_pControlMediator->GetToolCamera()->SetPositionToViewCenter(vPos, Tool3Const::CONST_WB_STATIC_OBJ_TO_CENTER);

	pEditor->RememberLastControllerScrollPos();
	m_pControlMediator->GetHelperAction()->RemoveAllSelection(true);
	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_MOVE, nKeyIdx, nCtrlId);
	pEditor->SetLastControllerScrollPos();
}

void CXCutSceneWrapper::GetCurrentCutSceneObjectMoveKeyInfo( int nControllerIdx, std::vector<rs3::RVector3>& _locations, std::vector<float>& _times, std::vector<rs3::RVector3>* _pDirs, std::vector<rs3::RVector3>* _pUps, std::deque<bool>* _pWarps )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerMove* pMoveCtrl = dynamic_cast<XCutSceneObjectControllerMove*>(m_pCurrentCutSceneObject->GetController(nControllerIdx));
	if(!pMoveCtrl)
		return;

	int nCnt = pMoveCtrl->GetKeyCount();
	float fTime;
	bool bIsWarp;
	rs3::RVector3 vPos, vDir, vUp;
	for (int i = 0; i < nCnt; ++i)
	{
		pMoveCtrl->GetKeyInfo(i, fTime, vPos, vDir, vUp, bIsWarp);
		_locations.push_back(vPos);
		_times.push_back(fTime);

		if (_pDirs)
		{
			_pDirs->push_back(vDir);
		}

		if (_pUps)
		{
			_pUps->push_back(vUp);
		}

		if (_pWarps)
		{
			_pWarps->push_back(bIsWarp);
		}
	}
}

void CXCutSceneWrapper::AddFadeController( CDlgCutSceneEditor* pEditor )
{
	AddController<XCutSceneObjectControllerFade>(pEditor);
}

void CXCutSceneWrapper::AddFadeControllerKey( int nControllerIdx, float fTime, float fAlpha, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)	return;

	XCutSceneObjectControllerFade* pFade = dynamic_cast<XCutSceneObjectControllerFade*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pFade)
		return;

	pFade->AddKey(fTime, fAlpha);
	pFade->SortKey();

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
}

bool CXCutSceneWrapper::GetFadeControllerKey( int nControllerIdx, float fKeyValue, float& fTime, float& fAlpha)
{
	if (!m_pCurrentCutSceneObject)	return false;
	if (nControllerIdx < 0)			return false;

	XCutSceneObjectControllerFade* pFade = dynamic_cast<XCutSceneObjectControllerFade*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pFade)	return false;

	int nKeyIndex = pFade->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)	return false;

	pFade->GetKeyInfo( nKeyIndex, fTime, fAlpha);
	return true;
}

void CXCutSceneWrapper::RemoveFadeControllerKey( int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor )
{
	RemoveControllerKey<XCutSceneObjectControllerFade>(nControllerIdx, fKeyValue, pEditor);
}

void CXCutSceneWrapper::ChangeFadeControllerKey( int nControllerIdx, float fKeyValue, float fTime, float fAlpha, CDlgCutSceneEditor* pEditor )
{
	if (!m_pCurrentCutSceneObject)
		return;

	XCutSceneObjectControllerFade* pFade = dynamic_cast<XCutSceneObjectControllerFade*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
	if (!pFade)
		return;

	int nKeyIndex = pFade->GetKeyIndex(fKeyValue);
	if (nKeyIndex < 0)
		return;

	pFade->SetKeyInfo( nKeyIndex, fTime, fAlpha);

	CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
	_ASSERT(pEntity);
	pEditor->ClearController();
	pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor, XCutSceneObjectController::E_FADE, nKeyIndex, nControllerIdx);
}

#pragma warning( pop )