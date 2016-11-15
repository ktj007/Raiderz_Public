#pragma once
#include "WS_CutSceneObjectEntity.h"

class CCutSceneObjectEntityEvent : public CCutSceneObjectEntity
{
private:
	float m_fLetterBoxFovX;

public:
	CCutSceneObjectEntityEvent(CControlMediator* pControlMediator, XCutSceneContent* pCutSceneContent);
	virtual ~CCutSceneObjectEntityEvent();
	
	virtual void SetTransform(const MMatrix& mat, bool bUsingCollision);
	virtual void SetAnimation(const std::string& _rAnimationName);
	virtual rs3::RCameraSceneNode* GetRelatedCamera();
	virtual void ShowCaption(const char* chCaption, const char* chFontName, int nSize, bool bBold = false, bool bItalic = false);
	virtual void HideCaption();
	virtual void SetLetterBox(bool bShow, float fRatioW, float fRatioH);
	virtual void CreateEffect(string& strEffectName, string& strEffectInfo, vec3& pos, vec3& dir, vec3& up, vector<stControllerEffectData>* pResultData);
	virtual void DestroyEffect(MUID uidEffect);
	virtual void SetEnvironment(int nEnvTime, int nEnvWeather, const std::string& _rEnvSetName, bool bFading);
	virtual void FadeUpdate(float fFadingAlpha);
	virtual void FadeEnd();

	void SetLetterBoxCameraFov(bool bShow);
	string MakeCaptionFontName( string strFontName, int nSize, bool bBold, bool bItalic );
	string MakeCaptionScriptForTextView( string strStrcip, string strFontAliasName );

protected:
	XCutSceneContent* m_pXCutSceneContent;
};