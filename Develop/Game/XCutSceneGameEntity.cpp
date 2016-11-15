#include "stdafx.h"
#include "XCutSceneGameEntity.h"
#include "XCutScenePlayer.h"
#include "XCameraManager.h"

#include "XGameState.h"
#include "XWorld.h"

#include "XEffectInvoker.h"
#include "XEffectManager.h"
#include "XGameInterface.h"

#include "XCutSceneContent.h"


//////////////////////////////////////////////////////////////////////////
RCameraSceneNode* XCutSceneGameEntity::GetRelatedCamera()
{
	return global.camera->GetCamera();
}

//////////////////////////////////////////////////////////////////////////
void XCutSceneCameraEntity::SetTransform( const MMatrix& mat, bool bUsingCollision )
{
	RCameraSceneNode* pCamera = GetRelatedCamera();
	vec3 vPos(mat._41, mat._42, mat._43), vDir(mat._21, mat._22, mat._23), vUp(mat._31, mat._32, mat._33);
	pCamera->Set(vPos, vDir, vUp);
}

//////////////////////////////////////////////////////////////////////////
XCutSceneActorEntity::XCutSceneActorEntity( const std::wstring& _rStrActorType, XObjectManager* pObjectManager )
{
	m_pObjectManager = pObjectManager;

	m_uid = global.system->GenerateLocalUID();
	m_pCutScenePlayer = new XCutScenePlayer(m_uid, _rStrActorType);
	_ASSERT(m_pCutScenePlayer);

	bool bRet = m_pCutScenePlayer->Create();
	_ASSERT(bRet);

	m_pCutScenePlayer->SerializeCutScenePlayer();
}

XCutSceneActorEntity::~XCutSceneActorEntity()
{
	m_pObjectManager->DestroyAllObject(m_uid);
}

void XCutSceneActorEntity::OnEnter()
{
	bool bRet = m_pObjectManager->Add(m_pCutScenePlayer);
	_ASSERT(bRet);
}

void XCutSceneActorEntity::SetTransform( const MMatrix& mat, bool bUsingCollision )
{
	RVector3 vPos(mat._41, mat._42, mat._43);
	RVector3 vDir(mat._21, mat._22, mat._23);
	m_pCutScenePlayer->SetTransform(vPos, vDir, bUsingCollision);
}

void XCutSceneActorEntity::SetAnimation( const std::wstring& _rAnimationName )
{
	m_pCutScenePlayer->SetAnimation(_rAnimationName);
}

void XCutSceneActorEntity::OnUseTalent( const int nTalent )
{
	m_pCutScenePlayer->OnUseTalent(nTalent);
}
	
//////////////////////////////////////////////////////////////////////////
XCutSceneEventEntity::XCutSceneEventEntity( XCutSceneContent* pRelatedCutSceneContent, MXmlElement* _pEventElement )
{
	_ASSERT(_pEventElement);
	m_pEventElement = (MXmlElement* )_pEventElement->Clone();

	m_fLetterBoxFovX = GetRelatedCamera()->GetFovX();

	m_pCutSceneContent = pRelatedCutSceneContent;
}

XCutSceneEventEntity::~XCutSceneEventEntity()
{
	delete m_pEventElement;

	if (!global.sound)
		return;

	for (std::set<std::wstring>::iterator itr = m_sounds.begin(); itr != m_sounds.end(); ++itr)
	{
		if ( global.sound)
			global.sound->RemoveSample(MLocale::ConvUTF16ToAnsi((*itr).c_str()).c_str());
	}
}

void XCutSceneEventEntity::PlaySound( const wchar_t* _pSzSoundName )
{
	if (!global.sound)
		return;

	if ( global.sound)
		global.sound->Play(MLocale::ConvUTF16ToAnsi(_pSzSoundName).c_str());
}

void XCutSceneEventEntity::OnEnter()
{
	MXmlElement* pBgmElement = m_pEventElement->FirstChildElement("bgm");
	while (pBgmElement)
	{
		MXmlElement* pBgmKeyElement = pBgmElement->FirstChildElement("bgmkey");
		while (pBgmKeyElement)
		{
			MXmlElement* pSoundElement = pBgmKeyElement->FirstChildElement("SOUND");
			if (pSoundElement)
			{
				RSampleInfo _info;
					
				std::string strSoundName;
				_Attribute( strSoundName, pSoundElement, "name");

				_Attribute( _info.strFileName, pSoundElement, "filename");

				bool bBG = false;
				_Attribute(&bBG, pSoundElement, "BG", false);
				_info.nType = bBG  ?  RSD_SOUNDTYPE_BGM : RSD_SOUNDTYPE_GENERIC;

				_Attribute( &_info.bLoop, pSoundElement, "loop", false);
				_Attribute( &_info.fVolume, pSoundElement, "volume", 1.f);
				_Attribute( &_info.nPriority, pSoundElement, "priority", 128);
				_Attribute( &_info.fMinDist, pSoundElement, "distance_min", 100.0f);
				_Attribute( &_info.fMaxDist, pSoundElement, "distance_max", 1000.0f);
				_Attribute( &_info.bUnique, pSoundElement, "unique", false);

				m_sounds.insert( MLocale::ConvAnsiToUTF16( strSoundName.c_str()));

				if ( global.sound)
					global.sound->AddSample( strSoundName.c_str(), new XSample( _info));
			}
			pBgmKeyElement = pBgmKeyElement->NextSiblingElement("bgmkey");
		}
		pBgmElement = pBgmElement->NextSiblingElement("bgm");
	}
}

void XCutSceneEventEntity::ShowCaption( const wchar_t* chCaption, const wchar_t* chFontName, int nSize, bool bBold /*= false*/, bool bItalic /*= false*/ )
{
	// 폰트 체크
	wstring strFontAliasName = MakeCaptionFontName(chFontName, nSize, bBold, bItalic);
	wstring strFinalCaption = MakeCaptionScriptForTextView(chCaption, strFontAliasName);

	// 폰트 셋팅
	int nFontHeight = 10;
	MFont* pFont = MFontManager::Get(MLocale::ConvUTF16ToAnsi(strFontAliasName.c_str()).c_str());
	if(pFont == NULL ||
		(pFont && 
		pFont == MFontManager::Get(NULL)))
	{
		// 없다.
		MFontR3* pNewFont = new MFontR3;
		if(!pNewFont->Create(global.device, MLocale::ConvUTF16ToAnsi(strFontAliasName.c_str()).c_str(), MLocale::ConvUTF16ToAnsi(chFontName).c_str(), nSize, 1.0f, bBold, bItalic))
		{
			pNewFont->Destroy();
			SAFE_DELETE(pNewFont);
		}
		else
		{
			MFontManager::Add(pNewFont);
			pFont = pNewFont;
		}
	}

	if(pFont)
		nFontHeight = pFont->GetHeight();

	global.script->GetGlueCutSceneEvent().OnShowCaptionScript(MLocale::ConvUTF16ToAnsi(strFinalCaption.c_str()).c_str(), nFontHeight);
}

void XCutSceneEventEntity::HideCaption()
{
	global.script->GetGlueCutSceneEvent().OnHideCaptionScript();
}

void XCutSceneEventEntity::ShowKeyEvent( const wchar_t* chText, const float fPosX, const float fPosY, const wchar_t* chFontName, int nSize, bool bBold, bool bItalic )
{
	// 폰트 체크
	wstring strFontAliasName = MakeCaptionFontName(chFontName, nSize, bBold, bItalic);
	wstring strFinalText = L"{FONT name=\"" + strFontAliasName + L"\"}" + chText;

	// 폰트 셋팅
	MFont* pFont = MFontManager::Get(MLocale::ConvUTF16ToAnsi(strFontAliasName.c_str()).c_str());
	if(pFont == NULL ||
		(pFont && 
		pFont == MFontManager::Get(NULL)))
	{
		// 없다.
		MFontR3* pNewFont = new MFontR3;
		if(!pNewFont->Create(global.device, MLocale::ConvUTF16ToAnsi(strFontAliasName.c_str()).c_str(), MLocale::ConvUTF16ToAnsi(chFontName).c_str(), nSize, 1.0f, bBold, bItalic))
		{
			pNewFont->Destroy();
			SAFE_DELETE(pNewFont);
		}
		else
		{
			MFontManager::Add(pNewFont);
			pFont = pNewFont;
		}
	}

	global.script->GetGlueCutSceneEvent().OnShowKeyEvent(MLocale::ConvUTF16ToAnsi(strFinalText.c_str()).c_str(), fPosX, fPosY);
}

void XCutSceneEventEntity::HideKeyEvent()
{
	global.script->GetGlueCutSceneEvent().OnHideKeyEvent();
}

void XCutSceneEventEntity::SetLetterBox( bool bShow, float fRatioW, float fRatioH )
{
	if(bShow)
	{
		SetLetterBoxCameraFov(bShow);

		int w, h;
		XGetGameInterface().OPTIONS.GetScreenResolution( &w, &h);

		float fH = (float)(fRatioH * w) / fRatioW;
		int nh = (h - fH) / 2.0f;

		// 소수점 계산에 의한 오차를 0.5를 더하므로서 반올림을 함. 화면을 1픽셀정도 더 해 질수 있음
		fH += 0.5f;

		global.script->GetGlueCutSceneEvent().OnShowLetterBox(w, fH, nh);
	}
	else
	{
		global.script->GetGlueCutSceneEvent().OnHideLetterBox();
	}
}	

void XCutSceneEventEntity::SetLetterBoxCameraFov(bool bShow)
{
	float fFovX = DegreeToRadian( XCONST::CAMERA_FOV );

	// 상수로 기술된 4:3 기준 FOV 를 Y 기준으로 구한다
	float fFovY = atanf(tanf(fFovX/2.0f)/ 1.33333f )*2.0f;

	float fNewFovX = m_fLetterBoxFovX;
	if(bShow)
	{
		float fAspectRatio = float(16.0f) / float(9.0f);
		fNewFovX = atanf(tanf(fFovY/2.0f)*fAspectRatio)*2.0f;
	}

	GetRelatedCamera()->SetFov( fNewFovX );
}

wstring XCutSceneEventEntity::MakeCaptionFontName( wstring strFontName, int nSize, bool bBold, bool bItalic )
{
	wchar_t chFontAliasName[256] = {0, };
	MFont* pFont = MFontManager::Get(MLocale::ConvUTF16ToAnsi(strFontName.c_str()).c_str());
	if(pFont)
	{
		swprintf_s(chFontAliasName, L"%s", strFontName.c_str());
	}
	else
	{
		swprintf_s(chFontAliasName, L"%s_%d_%d_%d", strFontName.c_str(), nSize, bBold, bItalic);
	}

	return chFontAliasName;
}

wstring XCutSceneEventEntity::MakeCaptionScriptForTextView( wstring strStrcip, wstring strFontAliasName )
{
	wstring strTextViewText = L"{FONT name=\"" + strFontAliasName + L"\"}{ALIGN hor=\"center\" ver=\"center\"}" + strStrcip;

	return strTextViewText;
}

void XCutSceneEventEntity::SetEnvironment( int nEnvTime, int nEnvWeather, const std::wstring& _rEnvSetName, bool bFading )
{
	float fTime = GAME_ENVIRONMENT_FADING_TIME;
	if (!bFading)
	{
		fTime = 0.f;
	}

	gg.currentgamestate->GetWorld()->OnCmdSetEnv(_rEnvSetName.c_str(), (GAME_TIME_TYPE)nEnvTime, (GAME_WEATHER_TYPE)nEnvWeather, bFading, fTime);
}

void XCutSceneEventEntity::CreateEffect( wstring& strEffectName, wstring& strEffectInfo, vec3& pos, vec3& dir, vec3& up, vector<stControllerEffectData>* pResultData )
{
	vector<EFFECT_RESULT_DATA> resultData;

	std::wstring strNpcTok(L"npc:");
	std::wstring strPostTok(L"post:");
	size_t npc_tok = strEffectInfo.find(strNpcTok);
	size_t post_tok = strEffectInfo.find(strPostTok);

	if (strEffectInfo == L"fixed" || strEffectInfo == L"")
	{
		XEffectSinglyInvoker effectInvoker;
		effectInvoker.Invoke(strEffectName, pos, dir, up, &resultData);
	}
	else if (npc_tok != std::wstring::npos || post_tok != std::wstring::npos)
	{
		wstring strActorName;
		wstring strBoneName;

		if (npc_tok != std::wstring::npos)
		{
			std::wstring strBoneTok(L",bone:");
			size_t bone_tok_prev = strEffectInfo.find(strBoneTok);
			size_t bone_tok_next = bone_tok_prev + strBoneTok.length();
			size_t npc_tok_next = npc_tok + strNpcTok.length();

			strActorName = strEffectInfo.substr(npc_tok_next, bone_tok_prev - npc_tok_next).c_str();
			strBoneName = strEffectInfo.substr(bone_tok_next).c_str();
		}
		else
		{
			size_t post_tok_next = post_tok + strPostTok.length();
			strActorName = strEffectInfo.substr(post_tok_next).c_str();
		}

		_ASSERT(m_pCutSceneContent);
		XObject* pObject = NULL;
		XCutSceneObject* pCutSceneObject = m_pCutSceneContent->FindCutSceneObject(strActorName);
		if (pCutSceneObject && pCutSceneObject->GetEntity())
		{
			XCutSceneActorEntity* pActorEntity = dynamic_cast<XCutSceneActorEntity*>(pCutSceneObject->GetEntity());
			if (pActorEntity)
			{
				pObject = pActorEntity->GetXObject();
			}
		}

		if (pObject)
		{
			XEffectInvoker effectInvoker;
			effectInvoker.Invoke(pObject, strEffectName, wstring(L""), strBoneName, &resultData);
		}
	}
	else if (strEffectInfo == L"camera")
	{
		global.camera->PlayCameraEffect(strEffectName.c_str(), -1.f);
	}

	if(pResultData)
	{
		for(vector<EFFECT_RESULT_DATA>::iterator it = resultData.begin(); it != resultData.end(); ++it)
		{
			stControllerEffectData effectData;

			effectData.type				= (CONTROLLER_EFFECT_TYPE)(it->type);

			effectData.uidEffect		= it->uidEffect;

			effectData.bFollow			= it->bFollow;
			effectData.bTerrainAlign	= it->bTerrainAlign;
			effectData.bTerrainPos		= it->bTerrainPos;

			effectData.strResourceName	= it->strResourceName;
			effectData.strBoneName		= it->strBoneName;

			pResultData->push_back(effectData);
		}
	}
}

void XCutSceneEventEntity::DestroyEffect( MUID uidEffect )
{
	global.emgr->DeleteXEffect(uidEffect);
}

void XCutSceneEventEntity::FadeUpdate( float fFadingAlpha )
{
	gg.currentgamestate->FadeUpdate(fFadingAlpha);
}

void XCutSceneEventEntity::FadeEnd()
{
	gg.currentgamestate->FadeEnd();
}

void XCutSceneEventEntity::CutSceneSkip()
{
	m_pCutSceneContent->SetCutSceneSkip(true);
}
