#include "stdafx.h"
#include "WS_CutSceneObjectEntityEvent.h"
#include "WS_CutSceneObjectEntityActor.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

CCutSceneObjectEntityEvent::CCutSceneObjectEntityEvent(CControlMediator* pControlMediator, XCutSceneContent* pCutSceneContent) : CCutSceneObjectEntity(pControlMediator)
{
	m_pXCutSceneContent = pCutSceneContent;
	m_fLetterBoxFovX = pControlMediator->GetToolCamera()->GetFovX();
}
CCutSceneObjectEntityEvent::~CCutSceneObjectEntityEvent()
{
}

void CCutSceneObjectEntityEvent::SetTransform(const MMatrix& mat, bool bUsingCollision)
{
	_ASSERT(0);
}

void CCutSceneObjectEntityEvent::SetAnimation(const std::string& _rAnimationName)
{
	_ASSERT(0);
}

rs3::RCameraSceneNode* CCutSceneObjectEntityEvent::GetRelatedCamera()
{
	return m_pControlMediator_->GetToolCamera();
}

void CCutSceneObjectEntityEvent::ShowCaption(const char* chCaption, const char* chFontName, int nSize, bool bBold, bool bItalic)
{
	// 폰트 체크
	string strFontAliasName = MakeCaptionFontName(chFontName, nSize, bBold, bItalic);
	string strFinalCaption = MakeCaptionScriptForTextView(chCaption, strFontAliasName);

	CMintManager::GetReference().ActiveCaptionTextView(true);
	CMintManager::GetReference().SetCaptionTextView(strFinalCaption.c_str(), strFontAliasName.c_str(), chFontName, nSize, bBold, bItalic);
}

void CCutSceneObjectEntityEvent::HideCaption()
{
	CMintManager::GetReference().ActiveCaptionTextView(false);
}

void CCutSceneObjectEntityEvent::SetLetterBox(bool bShow, float fRatioW, float fRatioH)
{
	SetLetterBoxCameraFov(bShow);
	CMintManager::GetReference().SetLetterBoxPaint(bShow, fRatioW, fRatioH);
}

void CCutSceneObjectEntityEvent::SetLetterBoxCameraFov(bool bShow)
{
	float fFovX = DegreeToRadian( (float)XCONST::CAMERA_FOV );

	// 상수로 기술된 4:3 기준 FOV 를 Y 기준으로 구한다
	float fFovY = atanf(tanf(fFovX/2.0f)/ 1.33333f )*2.0f;

	float fNewFovX = m_fLetterBoxFovX;
	if(bShow)
	{
		float fAspectRatio = float(16.0f) / float(9.0f);
		fNewFovX = atanf(tanf(fFovY/2.0f)*fAspectRatio)*2.0f;
	}

	if (GetRelatedCamera())
	{
		GetRelatedCamera()->SetFov( fNewFovX );
	}
}

string CCutSceneObjectEntityEvent::MakeCaptionFontName( string strFontName, int nSize, bool bBold, bool bItalic )
{
	char chFontAliasName[256] = {0, };
	sprintf(chFontAliasName, "%s_%d_%d_%d", strFontName.c_str(), nSize, bBold, bItalic);

	//MFont* pFont = MFontManager::Get(chFontAliasName);
	//if(pFont)
	//{
	//	sprintf(chFontAliasName, "%s", strFontName.c_str());
	//}
	//else
	//{
	//	sprintf(chFontAliasName, "%s_%d_%d_%d", strFontName.c_str(), nSize, bBold, bItalic);
	//}

	return chFontAliasName;
}

string CCutSceneObjectEntityEvent::MakeCaptionScriptForTextView( string strStrcip, string strFontAliasName )
{
	string strTextViewText = "{FONT name=\"" + strFontAliasName + "\"}{ALIGN hor=\"center\" ver=\"center\"}" + strStrcip;

	return strTextViewText;
}

void CCutSceneObjectEntityEvent::CreateEffect(string& strEffectName, string& strEffectInfo, vec3& pos, vec3& dir, vec3& up, vector<stControllerEffectData>* pResultData)
{
	CRollupCutSceneObjectList* pCutSceneObjecList = m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>();
	if (!pCutSceneObjecList)
		return;

	if (strEffectInfo == "fixed" || strEffectInfo == "")
	{
		WS_EffectCharacterToolSinglyInvoker effectInvoker;
		XEffectInfoMgr* pEffectInfoMgr = m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->GetEffectInfoMgr();

		effectInvoker.Invoke(strEffectName, pos, dir, up, pEffectInfoMgr, pCutSceneObjecList->GetEffectManager(), pResultData);
	}
	else if (strEffectInfo.find("npc") != std::string::npos || strEffectInfo.find("post") != std::string::npos)
	{
		string strActorName;
		string strBoneName;

		if(strEffectInfo.find("npc") != std::string::npos)
		{
			size_t npc_tok_next = strEffectInfo.find_first_not_of("npc:");
			size_t bone_tok_prev = strEffectInfo.find(",bone:");
			size_t bone_tok_next = bone_tok_prev + 6;

			strActorName = strEffectInfo.substr(npc_tok_next, bone_tok_prev - npc_tok_next).c_str();
			strBoneName = strEffectInfo.substr(bone_tok_next).c_str();
		}
		else
		{
			size_t post_tok_next = strEffectInfo.find_first_not_of("post:");
			strActorName = strEffectInfo.substr(post_tok_next).c_str();
		}
		
		_ASSERT(m_pXCutSceneContent);
		RActor* pObject = NULL;
		XCutSceneObject* pCutSceneObject = m_pXCutSceneContent->FindCutSceneObject(strActorName);

		if (pCutSceneObject && pCutSceneObject->GetEntity())
		{
			CCutSceneObjectEntityActor* pActorEntity = dynamic_cast<CCutSceneObjectEntityActor*>(pCutSceneObject->GetEntity());
			if (pActorEntity)
			{
				pObject = pActorEntity->GetXRActor();
			}
		}

		if (pObject)
		{
			WS_EffectCharacterToolInvoker effectInvoker;
			XEffectInfoMgr* pEffectInfoMgr = m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->GetEffectInfoMgr();

			effectInvoker.Invoke(pObject, strEffectName, string(""), strBoneName, pEffectInfoMgr, pCutSceneObjecList->GetEffectManager(), pResultData);
		}
	}
	else if (strEffectInfo == "camera")
	{
		rs3::RCameraAnimationController* pController = new rs3::RCameraAnimationController("00_AniController", (CXMLConfig::GetInstance()->strCameraAniPath + strEffectName + ".elu.ani").c_str(), true, false );
		pController->ResetTimer(-1);

		rs3::RToolCamera* pToolCamera = m_pControlMediator_->GetToolCamera();
		pToolCamera->AddCameraController(pController);
	}
}

void CCutSceneObjectEntityEvent::DestroyEffect(MUID uidEffect)
{
	CRollupCutSceneObjectList* pCutSceneObjecList = m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>();
	if(pCutSceneObjecList)
	{
		pCutSceneObjecList->GetEffectManager()->DeleteWSEffect(uidEffect);
	}
}

void CCutSceneObjectEntityEvent::SetEnvironment(int nEnvTime, int nEnvWeather, const std::string& _rEnvSetName, bool bFading)
{
	float fTime = 1.0f;
	if (bFading)
		fTime = 0.1f;
	m_pControlMediator_->GetViewPtr<CRollupEnvWeather>()->SetWeatherInfo(TRUE, nEnvTime, nEnvWeather, _rEnvSetName, fTime);
}

void CCutSceneObjectEntityEvent::FadeUpdate(float fFadingAlpha)
{
	CMintManager::GetReference().FadeDraw(fFadingAlpha);
}

void CCutSceneObjectEntityEvent::FadeEnd()
{
	CMintManager::GetReference().FadeEnd();
}

#pragma warning( pop )