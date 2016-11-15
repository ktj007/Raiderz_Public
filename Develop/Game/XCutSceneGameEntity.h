#ifndef _XCUT_SCENE_GAME_ENTITY_H
#define _XCUT_SCENE_GAME_ENTITY_H

#pragma once
#include "XCutSceneObject.h"

class XCutSceneContent;

//////////////////////////////////////////////////////////////////////////
class XCutSceneGameEntity : public XCutSceneObjectEntity
{
public:
	virtual rs3::RCameraSceneNode* GetRelatedCamera();
	virtual void OnEnter() = 0;
};

//////////////////////////////////////////////////////////////////////////
class XCutSceneCameraEntity : public XCutSceneGameEntity
{
public:
	virtual void OnEnter() {}
	virtual void SetTransform(const MMatrix& mat, bool bUsingCollision);
	virtual void SetAnimation(const std::wstring& _rAnimationName) {}
};

class XCutSceneActorEntity : public XCutSceneGameEntity
{
public:
	XCutSceneActorEntity(const std::wstring& _rStrActorType, XObjectManager*	pObjectManager);
	virtual ~XCutSceneActorEntity();

	virtual void OnEnter();
	virtual void SetTransform(const MMatrix& mat, bool bUsingCollision);
	virtual void SetAnimation(const std::wstring& _rAnimationName);
	virtual void OnUseTalent(const int nTalent);

	XObject* GetXObject(){ return (XObject*)m_pCutScenePlayer; }

private:
	MUID				m_uid;
	XCutScenePlayer*	m_pCutScenePlayer;
	XObjectManager*		m_pObjectManager;

};

class XCutSceneEventEntity : public XCutSceneGameEntity
{
public:
	XCutSceneEventEntity(XCutSceneContent* pRelatedCutSceneContent, MXmlElement* _pEventElement);
	virtual ~XCutSceneEventEntity();

	virtual void OnEnter();
	virtual void SetTransform(const MMatrix& mat, bool bUsingCollision) {}
	virtual void SetAnimation(const std::wstring& _rAnimationName) {}
	virtual void PlaySound(const wchar_t* _pSzSoundName);
	virtual void ShowCaption(const wchar_t* chCaption, const wchar_t* chFontName, int nSize, bool bBold = false, bool bItalic = false);
	virtual void HideCaption();
	virtual void ShowKeyEvent(const wchar_t* chText, const float fPosX, const float fPosY, const wchar_t* chFontName, int nSize, bool bBold = false, bool bItalic = false);
	virtual void HideKeyEvent();
	virtual void SetLetterBox(bool bShow, float fRatioW, float fRatioH);
	virtual void SetEnvironment(int nEnvTime, int nEnvWeather, const std::wstring& _rEnvSetName, bool bFading);
	virtual void CreateEffect(wstring& strEffectName, wstring& strEffectInfo, vec3& pos, vec3& dir, vec3& up, vector<stControllerEffectData>* pResultData);
	virtual void DestroyEffect(MUID uidEffect);
	virtual void FadeUpdate( float fFadingAlpha );
	virtual void FadeEnd();
	virtual void CutSceneSkip();

private:
	std::set<std::wstring>	m_sounds;
	MXmlElement*			m_pEventElement;
	XCutSceneContent*		m_pCutSceneContent;

	float m_fLetterBoxFovX;

	void SetLetterBoxCameraFov(bool bShow);

	wstring MakeCaptionFontName(wstring strFontName, int nSize, bool bBold, bool bItalic);
	wstring MakeCaptionScriptForTextView(wstring strStrcip, wstring strFontAliasName);
};


#endif