#pragma once

#include "M_LogicObject.h"
#include <map>
#include <vector>
#include <deque>

class XCutSceneContent;
class XCutSceneObject;
class XCutSceneObjectEntity;
class XCutSceneObjectController;

class CControlMediator;
class CDlgCutSceneEditor;
class CCutSceneObjectEntity;
class CLogicObject;

enum CUTSCENE_OBJECT_STATE
{
	CSBS_OK,
	CSBS_WARRING,
	CSBS_ERROR
};

namespace coretoolcutscene
{
	// 리소스의 메뉴순서와 동일해야합니다.
	enum E_CONTEXT_MENU_ID
	{
		E_CAMERA_OBJ = 0,
		E_MOVE_CTRL,
		E_DOF_CTRL,
		E_ACTOR_OBJ,
		E_ANI_CTRL,
		E_EVENT_OBJ,
		E_BGM_CTRL,
		E_CAPTION_CTRL,
		E_LETTERBOX_CTRL,
		E_ENV_CTRL,
		E_EFFECT_CTRL,
		E_FADE_CTRL,
		E_TALENT_CTRL
	};
}

class CXCutSceneWrapper
{
public:
									CXCutSceneWrapper() : m_pXCutSceneContent(NULL), m_pCurrentCutSceneObject(NULL) {}
									~CXCutSceneWrapper(){}

	void							RegisterCutSceneContent( XCutSceneContent* pCutSceneContent);
	void							ClearCutSceneContent();
	bool							SaveCutSceneContent( const char* pSzFileName );

	void							RenderCutSceneActors(bool bRunningCutScenePreview);
	void							RunCutSceneContent(float fElapsedTime);
	void							ResetRunning();
	void							GetRunningTime(float& _fDuration, float& _fCurrent);

	int								GetCutSceneObjectCount();
	const std::string&				GetCutSceneObjectName(int nIndex);
	CLogicObject*					GetCurrentCutSceneObjectFirstMoveKey();
	CUTSCENE_OBJECT_STATE			CheckCutSceneObjectControllerState(int nIndex);

	void							AddCutSceneObject(const char* _pSzCutSceneObjectType, const char* _pSzActorName);
	void							AddCutSceneObjectController(int nObjIndex, const char* _pSzCutSceneObjectControllerType);

	void							RemoveCutSceneObject(int nObjectIdx);
	void							RemoveCutSceneController(int nControllerIdx, CDlgCutSceneEditor* pEditor);

	void							MoveCameraToMoveKeyAndSelectIt(int nCtrlId, float fKeyValue, CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////

	// Actor Object
	void							SetActorTypeAndName(const char* _pSzObjectType, const char* _pSzActorName);
	void							GetActorTypeAndName(std::string& strType, std::string& strName);

	//////////////////////////////////////////////////////////////////////////

	// Move Controller
	void							AddMoveController(CDlgCutSceneEditor* pEditor);
	void							ChangeMoveControllerToCatmullRom(int nCtrlId, bool bCatmullRom, bool* bPrevCatmullOut = NULL);
	int								ToggleMoveControllerCollision(int nCtrlId);

	bool							AddMoveControllerKey(int nControllerIdx, float fTime, const rs3::RVector3& vPos, const rs3::RVector3& vDir, const rs3::RVector3& vUp, bool bWarp, CDlgCutSceneEditor* pEditor);
	bool							GetMoveControllerKey(int nControllerIdx, float fKeyValue, float& fTime, rs3::RVector3& vPos, rs3::RVector3& vDir, rs3::RVector3& vUp, bool& bWarp);
	void							RemoveMoveControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeMoveControllerKey(int nControllerIdx, float fKeyValue, float fTime, const rs3::RVector3& vPos, const rs3::RVector3& vDir, const rs3::RVector3& vUp, bool bWarp, CDlgCutSceneEditor* pEditor, bool bSelectKey);
	void							GetCurrentCutSceneObjectMoveKeyInfo(int nControllerIdx, std::vector<rs3::RVector3>& _locations, std::vector<float>& _times, std::vector<rs3::RVector3>* _pDirs = NULL, std::vector<rs3::RVector3>* _pUps = NULL, std::deque<bool>* _pWarps = NULL);

	//////////////////////////////////////////////////////////////////////////

	// DOF Controller
	void							AddDOFController(CDlgCutSceneEditor* pEditor);
	void							AddDOFControllerKey(int nControllerIdx, float fTime, float fFocusDist, float fFocusRange, float fCocSize, CDlgCutSceneEditor* pEditor);
	bool							GetDOFControllerKey(int nControllerIdx, float fKeyValue, float& fTime, float& fFocusDist, float& fFocusRange, float& fCocSize);
	void							RemoveDOFControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeDOFControllerKey(int nControllerIdx, float fKeyValue, float fTime, float fFocusDist, float fFocusRange, float fCocSize, CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////

	// Animation Controller
	void							AddAnimationController(CDlgCutSceneEditor* pEditor);
	void							AddAniControllerKey(int nControllerIdx, float fTime, const std::string& _rAniName, CDlgCutSceneEditor* pEditor);
	bool							GetAniControllerKey(int nControllerIdx, float fKeyValue, float& fTime, std::string& _rAniName);
	void							RemoveAniControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeAniControllerKey(int nControllerIdx, float fKeyValue, float fTime, const std::string& _rAniName, CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////
	// Talent Controller
	void							AddTalentController(CDlgCutSceneEditor* pEditor);
	void							AddTalentControllerKey(int nControllerIdx, float fTime, const int nID, CDlgCutSceneEditor* pEditor);
	bool							GetTalentControllerKey(int nControllerIdx, float fKeyValue, float& fTime, int& nID);
	void							RemoveTalentControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeTalentControllerKey(int nControllerIdx, float fKeyValue, float fTime, int nID, CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////

	// BGM Controller
	void							AddBGMController(CDlgCutSceneEditor* pEditor);
	void							AddBGMControllerKey(int nControllerIdx, float fTime, const std::string& _rFileName, float fVolume, bool bLoop, CDlgCutSceneEditor* pEditor);
	bool							GetBGMControllerKey(int nControllerIdx, float fKeyValue, float& fTime, std::string& _rFileName, float &fVolume, bool &bLoop);
	void							RemoveBGMControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeBGMControllerKey(int nControllerIdx, float fKeyValue, float fTime, const std::string& _rFileName, float fVolume, bool bLoop, CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////
	// Caption Controller
	void							AddCaptionController(CDlgCutSceneEditor* pEditor, const std::string& strFont, int nSize, bool bBold, bool bItalic);
	void							GetCaptionController(int nControllerIdx, std::string& strOutFont, int& nOutSize, bool& bOutBold, bool& bOutItalic);
	void							ChangeCaptionController(int nControllerIdx, const std::string& strFont, int nSize, bool bBold, bool bItalic, CDlgCutSceneEditor* pEditor);

	void							AddCaptionControllerKey(int nControllerIdx, float fTime, float fDurationTime, const std::string& _rScrip, CDlgCutSceneEditor* pEditor);
	bool							GetCaptionControllerKey(int nControllerIdx, float fKeyValue, float& fTime, float &fDurationTime, std::string& _rScrip);
	void							RemoveCaptionControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeCaptionControllerKey(int nControllerIdx, float fKeyValue, float fTime, float fDurationTime, const std::string& _rScrip, CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////
	// LetterBox Controller
	void							AddLetterBoxController(CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////

	// Env Controller
	void							AddEnvController(CDlgCutSceneEditor* pEditor);
	void							AddEnvControllerKey(int nControllerIdx, float fTime, int nEnvTime, int nEnvWeather, const std::string& _rSetName, CDlgCutSceneEditor* pEditor);
	bool							GetEnvControllerKey(int nControllerIdx, float fKeyValue, float& fTime, int& nEnvTime, int& nEnvWeather, std::string& _rSetName);
	void							RemoveEnvControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeEnvControllerKey(int nControllerIdx, float fKeyValue, float fTime, int nEnvTime, int nEnvWeather, const std::string& _rSetName, CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////
	// Effect Controller
	void							AddEffectController(CDlgCutSceneEditor* pEditor);
	void							AddEffectControllerKey(int nControllerIdx, float fTime, float fDurationTime, const vec3& vPos, const vec3& vDir, const vec3& vUp, const std::string& _rEffectName, const std::string& _rEffectInfo, CDlgCutSceneEditor* pEditor);
	bool							GetEffectControllerKey(int nControllerIdx, float fKeyValue, float& fTime, float& fDurationTime, vec3& vPos, vec3& vDir, vec3& vUp, std::string& _rEffectName, std::string& _rEffectInfo);
	void							RemoveEffectControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeEffectControllerKey(int nControllerIdx, float fKeyValue, float fTime, float fDurationTime, const vec3& vPos, const vec3& vDir, const vec3& vUp, const std::string& _rEffectName, const std::string& _rEffectInfo, CDlgCutSceneEditor* pEditor);

	//////////////////////////////////////////////////////////////////////////
	// Fade Controller
	void							AddFadeController(CDlgCutSceneEditor* pEditor);
	void							AddFadeControllerKey( int nControllerIdx, float fTime, float fAlpha, CDlgCutSceneEditor* pEditor );
	bool							GetFadeControllerKey( int nControllerIdx, float fKeyValue, float& fTime, float& fAlpha);
	void							RemoveFadeControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor);
	void							ChangeFadeControllerKey(int nControllerIdx, float fKeyValue, float fTime, float fAlpha, CDlgCutSceneEditor* pEditor);
	

	//////////////////////////////////////////////////////////////////////////

	void							WriteClapperInfo(CTreeCtrl& _rTreeControl, HTREEITEM& _rNameItem, HTREEITEM& _rDurationItem);
	void							ResetObjectInfo(CDlgCutSceneEditor* pCutSceneEditor);
	int								SetCurrentCutSceneObject(int nIndex, CDlgCutSceneEditor* pCutSceneEditor, bool bTriggerShow, bool bTriggerScroll);

	int								GetContextMenuIdForObject();
	int								GetContextMenuIdForController(int nIdx);
	void							SetContextMenuForController(int nType, CMenu* pMenu, bool bUseKey, int nCtrlIdx);

	void							ShowNoticeForTool(bool bShow);

	//////////////////////////////////////////////////////////////////////////

	static XCutSceneObjectEntity*	CreateCameraObjectEntity();
	static XCutSceneObjectEntity*	CreateActorObjectEntity(const std::string& _rStrType);
	static XCutSceneObjectEntity*	CreateEventObjectEntity(MXmlElement* _pEventElement, XCutSceneContent* pCutSceneContent);

	//////////////////////////////////////////////////////////////////////////
	static void InitCXCutSceneWrapper(CControlMediator* _pControlMediator) { m_pControlMediator = _pControlMediator; }

private:
	static CControlMediator*		m_pControlMediator;
	XCutSceneContent*				m_pXCutSceneContent;
	XCutSceneObject*				m_pCurrentCutSceneObject;

	template<typename T>
	T* AddController(CDlgCutSceneEditor* pEditor)
	{
		if (!m_pCurrentCutSceneObject)
			return NULL;

		T* pController = new T(m_pCurrentCutSceneObject);
		m_pCurrentCutSceneObject->AddController(pController);

		CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
		_ASSERT(pEntity);
		pEditor->ClearController();
		pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);

		return pController;
	}

	template<typename T>
	void RemoveControllerKey(int nControllerIdx, float fKeyValue, CDlgCutSceneEditor* pEditor)
	{
		if (!m_pCurrentCutSceneObject)
			return;

		T* pKeyController = dynamic_cast<T*>(m_pCurrentCutSceneObject->GetControllers().at(nControllerIdx));
		if (!pKeyController)
			return;

		pKeyController->DeleteKey( pKeyController->GetKeyIndex(fKeyValue) );
		pKeyController->SortKey();

		CCutSceneObjectEntity* pEntity = (CCutSceneObjectEntity*)(m_pCurrentCutSceneObject->GetEntity());
		_ASSERT(pEntity);
		pEditor->ClearController();
		pEntity->ResetCutSceneEditorObject(m_pCurrentCutSceneObject, pEditor);
	}
};
