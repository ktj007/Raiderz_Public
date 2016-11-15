#pragma once
#include "VR_EnvWeather.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperRender.h"
#include "C_ControlMediatorHelperAction.h"
#include "M_ToolLogicObjectManager.h"

#include "VD_DlgCutSceneEditor.h"
#include "WS_CutSceneXCutSceneWrapper.h"

#include "../../Game/XCutSceneContent.h"
#include "../../Game/XCutSceneObject.h"
#include "../../Game/XCutSceneObjectController.h"
#include "../../Game/XCutSceneObjectControllerEnv.h"
#include "../../Game/XCutSceneObjectControllerLetterBox.h"
#include "../../Game/XCutSceneObjectControllerCaption.h"
#include "../../Game/XCutSceneObjectControllerBGM.h"
#include "../../Game/XCutSceneObjectControllerAnimation.h"
#include "../../Game/XCutSceneObjectControllerDOF.h"
#include "../../Game/XCutSceneObjectControllerMove.h"
#include "../../Game/XCutSceneObjectControllerFade.h"
#include "../../Game/XCutSceneObjectControllerKeyEvent.h"
#include "../../Game/XCutSceneObjectControllerTalent.h"

#include "MLocale.h"

#include "C_MintManager.h"
#include "C_XMLXConst.h"

#include "VR_CutSceneObjectList.h"
#include "VR_EntityObjectList.h"

#include "WS_EffectInvoker.h"

#include "WS_CutSceneLogicObjectBase.h"
#include "WS_CutSceneLogicObjectMove.h"
#include "WS_CutSceneLogicObjectEffect.h"

#include "RCameraAnimationController.h"
#include "M_ToolCamera.h"
#include "C_XMLConfig.h"

class CCutSceneObjectEntity : public XCutSceneObjectEntity
{
public:
	CCutSceneObjectEntity(CControlMediator* pControlMediator);
	virtual ~CCutSceneObjectEntity();

	void ResetCutSceneEditorObject(XCutSceneObject* _pObject, CDlgCutSceneEditor* pEditor, XCutSceneObjectController::TYPE _eType = XCutSceneObjectController::E_TYPE_NUM, int nSelectedKeyIdx = -1, int nSelectedCtrlIdx = -1);
	void SelectMoveLogic(bool bOnSelect);
	std::pair<int, int> GetMoveCtrlKeyIdxPair(CLogicObject* _pMoveKey);

	virtual rs3::RSceneNode* GetSceneNodeToRender(){ return NULL; }
	CLogicObject* GetFirstMoveKeyLogicObject(){ return m_pFirstMoveKeyLogicObject_; }

protected:
	void ClearMoveLogicObject();

protected:
	CControlMediator*	m_pControlMediator_;
	XCutSceneObject*	m_pOwnerToolCutSceneObject_;
	CDlgCutSceneEditor*	m_pCutSceneEditor_;
	CLogicObject*		m_pFirstMoveKeyLogicObject_;

	VEC_CUTSCENE_CONTROLLERS m_CutSceneController;
	typedef std::multimap< XCutSceneObjectController*, CCutSceneLogicObjectMove* > MULMAP_MOVE_LOGIC;
	MULMAP_MOVE_LOGIC m_mulmapMoveLogicObjects;
};