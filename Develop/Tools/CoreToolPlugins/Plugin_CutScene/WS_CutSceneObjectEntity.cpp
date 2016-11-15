#include "stdafx.h"
#include "WS_CutSceneObjectEntity.h"

CCutSceneObjectEntity::CCutSceneObjectEntity(CControlMediator* pControlMediator)
: m_pControlMediator_(pControlMediator)
, m_pOwnerToolCutSceneObject_(NULL)
, m_pCutSceneEditor_(NULL)
, m_pFirstMoveKeyLogicObject_(NULL)
{
}

CCutSceneObjectEntity::~CCutSceneObjectEntity()
{
	ClearMoveLogicObject();
}

void CCutSceneObjectEntity::ResetCutSceneEditorObject(XCutSceneObject* _pObject, CDlgCutSceneEditor* pEditor, XCutSceneObjectController::TYPE _eType, int nSelectedKeyIdx, int nSelectedCtrlIdx)
{
	using namespace rs3;

	// clear selection
	m_pControlMediator_->GetHelperAction()->RemoveAllSelection(true);

	// clear cutscene logic object
	m_CutSceneController.clear();
	ClearMoveLogicObject();

	// for move gizmo offset height
	CCutSceneObjectEntity* pCutSceneEntity = dynamic_cast<CCutSceneObjectEntity*>(_pObject->GetEntity());
	_ASSERT(pCutSceneEntity);
	float fHeight = CEntityObject::SPAWN_HEIGHT + 200.f;
	RSceneNode* pSceneNode = pCutSceneEntity->GetSceneNodeToRender();
	if (pSceneNode)
	{
		fHeight += pSceneNode->GetAABB().maxz - pSceneNode->GetAABB().minz;
	}

	// selected move key object
	CLogicObject* pSelectedKeyLogicObject = NULL;

	VEC_CUTSCENE_CONTROLLERS& rCutSceneControllers = _pObject->GetControllers();
	for (VEC_CUTSCENE_CONTROLLERS::iterator itr = rCutSceneControllers.begin(); itr != rCutSceneControllers.end(); ++itr)
	{
		XCutSceneObjectController* pController = *itr;
		m_CutSceneController.push_back(pController);
		switch(pController->GetType())
		{
		case XCutSceneObjectController::E_MOVE:
			{
				CCutSceneLogicObjectMove* pCutSceneMoveObject = new CCutSceneLogicObjectMove;
				pCutSceneMoveObject->SetBaseInfo(fHeight, (_pObject->GetType() == XCutSceneObject::E_CAMERA) );

				m_mulmapMoveLogicObjects.insert( MULMAP_MOVE_LOGIC::value_type(pController, pCutSceneMoveObject) );
				m_pControlMediator_->GetToolLogicObjectMgr()->AddToManager(pCutSceneMoveObject);

				XCutSceneObjectControllerMove* pMoveController = (XCutSceneObjectControllerMove*)pController;
				int nControllerId = pEditor->AddController("move");
				int nKeyCount = pMoveController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pMoveController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				RVector3 vPos, vDir, vUp;
				float fTime;
				bool bWarp;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pMoveController->GetKeyInfo(i, fTime, vPos, vDir, vUp, bWarp);
					CLogicObject* pLogicObject = pCutSceneMoveObject->AddKey(m_pControlMediator_->GetToolLogicObjectMgr()->GetPropertyTable("movekey"), fTime, vPos, vDir);
					CDlgCutSceneEditor::E_BAR_TYPE barType = CDlgCutSceneEditor::E_INTERVAL_KEY;
					if (bWarp)
					{
						barType = CDlgCutSceneEditor::E_WARP_KEY;
					}
					pEditor->AddBarGraphValue(nControllerId, barType, fTime, fTime);
					if (NULL == m_pFirstMoveKeyLogicObject_)
					{
						m_pFirstMoveKeyLogicObject_ = pLogicObject;
					}

					if (_eType ==  XCutSceneObjectController::E_MOVE && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
						pSelectedKeyLogicObject = pLogicObject;
					}
				}

				pCutSceneMoveObject->SetXCutSceneController(pMoveController, nControllerId);
			}
			break;
		case XCutSceneObjectController::E_ANIMATION:
			{
				XCutSceneObjectControllerAnimation* pAniController = (XCutSceneObjectControllerAnimation*)pController;
				int nControllerId = pEditor->AddController("ani");
				int nKeyCount = pAniController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pAniController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				float fTime;
				std::string strAniName;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pAniController->GetKeyInfo(i, fTime, strAniName);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_ANIMATION && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		case XCutSceneObjectController::E_TALENT:
			{
				XCutSceneObjectControllerTalent* pTalentController = (XCutSceneObjectControllerTalent*)pController;
				int nControllerId = pEditor->AddController("Talent");
				int nKeyCount = pTalentController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pTalentController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				float fTime;
				int nTalentID;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pTalentController->GetKeyInfo(i, fTime, nTalentID);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_TALENT && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		case XCutSceneObjectController::E_EFFECT:
			{
				CCutSceneLogicObjectEffect* pCutSceneEffectObject = new CCutSceneLogicObjectEffect;
				pCutSceneEffectObject->SetBaseInfo(fHeight, true);

				m_pControlMediator_->GetToolLogicObjectMgr()->AddToManager(pCutSceneEffectObject);

				XCutSceneObjectControllerEffect* pEffectController = (XCutSceneObjectControllerEffect*)pController;
				int nControllerId = pEditor->AddController("effect");
				int nKeyCount = pEffectController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pEffectController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				RVector3 vPos, vDir, vUp;
				float fTime, fDurationTime;
				string strEffectName;
				string strEffectInfo;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pEffectController->GetKeyInfo(i, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo);
					CLogicObject* pLogicObject = pCutSceneEffectObject->AddKey(m_pControlMediator_->GetToolLogicObjectMgr()->GetPropertyTable("movekey"), fTime, vPos, vDir, vUp);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_EFFECT && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}

				pCutSceneEffectObject->SetXCutSceneController(pEffectController);
			}
			break;
		case XCutSceneObjectController::E_DOF:
			{
				XCutSceneObjectControllerDOF* pDOFController = (XCutSceneObjectControllerDOF*)pController;
				int nControllerId = pEditor->AddController("dof");
				int nKeyCount = pDOFController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pDOFController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				float fTime, fFocusDist, fFocusRange, fCocSize;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pDOFController->GetKeyInfo(i, fTime, fFocusDist, fFocusRange, fCocSize);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_DOF && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		case XCutSceneObjectController::E_BGM:
			{
				XCutSceneObjectControllerBGM* pBMGController = (XCutSceneObjectControllerBGM*)pController;
				int nControllerId = pEditor->AddController("bgm");
				int nKeyCount = pBMGController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pBMGController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				float fTime, fVolume;
				std::string strFileName;
				bool bLoop;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pBMGController->GetKeyInfo(i, fTime, strFileName, bLoop, fVolume);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_BGM && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		case XCutSceneObjectController::E_CAPTION:
			{
				XCutSceneObjectControllerCaption* pCaptionController = (XCutSceneObjectControllerCaption*)pController;
				int nControllerId = pEditor->AddController("caption");
				int nKeyCount = pCaptionController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pCaptionController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				float fTime, fDurationTime;
				std::string strScript;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pCaptionController->GetKeyInfo(i, fTime, fDurationTime, strScript);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_CAPTION && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		case XCutSceneObjectController::E_LETTERBOX:
			{
				XCutSceneObjectControllerLetterBox* pLetterBoxController = (XCutSceneObjectControllerLetterBox*)pController;
				int nControllerId = pEditor->AddController("letterbox");
				int nKeyCount = pLetterBoxController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pLetterBoxController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				XCutSceneObjectControllerLetterBox::LetterBoxTYPE eType;
				float fTime;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pLetterBoxController->GetKeyInfo(i, eType, fTime);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_LETTERBOX && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		case XCutSceneObjectController::E_KEYEVENT:
			{
				XCutSceneObjectControllerKeyEvent* pKeyEventController = (XCutSceneObjectControllerKeyEvent*)pController;
				int nControllerId = pEditor->AddController("keyevent");
				int nKeyCount = pKeyEventController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pKeyEventController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				float fTime, fDurationTime;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pKeyEventController->GetKeyInfo(i, fTime, fDurationTime);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_KEYEVENT && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		case XCutSceneObjectController::E_ENV:
			{
				XCutSceneObjectControllerEnv* pEnvController = (XCutSceneObjectControllerEnv*)pController;
				int nControllerId = pEditor->AddController("env");
				int nKeyCount = pEnvController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pEnvController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				float fTime;
				int nEnvTime, nEnvWeather;
				std::string strSetName;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pEnvController->GetKeyInfo(i, fTime, nEnvTime, nEnvWeather, strSetName);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_ENV && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		case XCutSceneObjectController::E_FADE:
			{
				XCutSceneObjectControllerFade* pFadeController = (XCutSceneObjectControllerFade*)pController;
				int nControllerId = pEditor->AddController("fade");
				int nKeyCount = pFadeController->GetKeyCount();

				if (nKeyCount >= 2)
				{
					float fStart, fEnd;
					pFadeController->GetRange(fStart, fEnd);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL, fStart, fEnd);
				}

				float fTime, fAlpha;
				string strEffectName;
				for (int i = 0; i < nKeyCount; ++i)
				{
					pFadeController->GetKeyInfo(i, fTime, fAlpha);
					pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_INTERVAL_KEY, fTime, fTime);

					if (_eType ==  XCutSceneObjectController::E_FADE && nSelectedKeyIdx == i && nControllerId == nSelectedCtrlIdx)
					{
						pEditor->AddBarGraphValue(nControllerId, CDlgCutSceneEditor::E_SELECTED_KEY, fTime, fTime);
					}
				}
			}
			break;
		default:
			_ASSERT(0);
			break;
		}
	}

	if (m_pControlMediator_->GetHelperAction()->GetSelectedLogicObjectCntForDoAction() > 0)
		pEditor->RememberLastControllerScrollPos(true);
	pEditor->RefreshController();

	m_pOwnerToolCutSceneObject_ = _pObject;
	m_pCutSceneEditor_ = pEditor;

	if (nSelectedKeyIdx < 0)
	{
		m_pControlMediator_->GetHelperAction()->RemoveAllSelection(true);
	}
	else if(NULL != pSelectedKeyLogicObject)
	{
		m_pControlMediator_->GetHelperAction()->AddSelectection( pSelectedKeyLogicObject, true);
	}
}

void CCutSceneObjectEntity::SelectMoveLogic(bool bOnSelect)
{
	for (MULMAP_MOVE_LOGIC::iterator itr = m_mulmapMoveLogicObjects.begin(); itr != m_mulmapMoveLogicObjects.end(); ++itr)
	{
		CLogicObject* pLoicObject = itr->second;
		pLoicObject->SetSelection(bOnSelect);
	}
}

std::pair<int, int> CCutSceneObjectEntity::GetMoveCtrlKeyIdxPair(CLogicObject* _pMoveKey)
{
	int nCtrlIdx = -1;
	int nKeyIdx = -1;
	bool bCheckMoveKey = false;
	for (MULMAP_MOVE_LOGIC::iterator itr = m_mulmapMoveLogicObjects.begin(); itr != m_mulmapMoveLogicObjects.end(); ++itr)
	{
		CCutSceneLogicObjectMove* pLoicObject = dynamic_cast<CCutSceneLogicObjectMove*>(itr->second);
		if (NULL == pLoicObject)
			continue;

		if (false == bCheckMoveKey)
		{
			if (pLoicObject->IsMoveKey(_pMoveKey))
			{
				bCheckMoveKey = true;
			}
			else
			{
				nCtrlIdx = -1;
				nKeyIdx = -1;
				break;
			}
		}

		nKeyIdx = pLoicObject->GetMoveKeyIdx(_pMoveKey);
		if (nKeyIdx > -1)
		{
			nCtrlIdx = pLoicObject->GetCtrlIdx();
			break;
		}
	}

	return std::pair<int, int>(nCtrlIdx, nKeyIdx);
}

void CCutSceneObjectEntity::ClearMoveLogicObject()
{
	for (MULMAP_MOVE_LOGIC::iterator itr = m_mulmapMoveLogicObjects.begin(); itr != m_mulmapMoveLogicObjects.end(); ++itr)
	{
		CLogicObject* pLoicObject = itr->second;
		m_pControlMediator_->GetToolLogicObjectMgr()->RemoveFromManager(pLoicObject);

		pLoicObject->DestroyLogicObject();
		delete pLoicObject;
	}
	m_mulmapMoveLogicObjects.clear();
	m_pFirstMoveKeyLogicObject_ = NULL;
}
