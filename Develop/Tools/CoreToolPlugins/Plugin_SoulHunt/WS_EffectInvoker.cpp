#include "stdafx.h"
#include "WS_EffectInvoker.h"

WS_EffectCharacterToolInvoker::WS_EffectCharacterToolInvoker()
: XEffectBaseInvoker()
{
	m_pActor = NULL;
	m_pEffectManager = NULL;
	m_pEffectInfoMgr = NULL;
}

WS_EffectCharacterToolInvoker::~WS_EffectCharacterToolInvoker()
{

}

void WS_EffectCharacterToolInvoker::Invoke( RActor* pActor, string& strEffectName, string& strEffectParam, string& strEffectBoneName, XEffectInfoMgr* pEffectInfoMgr, WS_EffectManager* pEffectManager, vector<stControllerEffectData>* vecOutData /*= NULL*/ )
{
	m_pEffectManager = pEffectManager;
	m_pEffectInfoMgr = pEffectInfoMgr;
	m_pActor = pActor;
	
	//XEffectInfo * pEffectInfo =  g_pMainApp->m_pEffectInfoMgr->GetEffectInfo(strEffectName);
	XEffectInfo * pEffectInfo =  m_pEffectInfoMgr->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		string strBoneName	= strEffectBoneName.empty() ? GetReserved_BoneName(strEffectName) : strEffectBoneName;
		string strParam		= strEffectParam.empty() ? GetReserved_Param(strEffectName) : strEffectParam;

		float fEffectDurationTime = pEffectInfo->m_fEffectDurationTime;

		// 이벤트 발생
		for(vector<XBaseEffectInfoItem *>::iterator itEvent = pEffectInfo->m_vecEffectInfoItem.begin(); 
			itEvent != pEffectInfo->m_vecEffectInfoItem.end(); itEvent++)
		{
			map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::iterator itFind = m_mapEffectHandlers.find((*itEvent)->eEffectType);
			if(itFind != m_mapEffectHandlers.end())
			{
				EFFECT_RESULT_DATA outData;
				EFFECT_TRANSEMISION_DATA transemisionData;
				transemisionData.pEffectInfoItem	= (*itEvent);
				transemisionData.strEventName		= strEffectName;

				itFind->second(*this, transemisionData, strBoneName, strParam, fEffectDurationTime, outData);

				if(vecOutData &&
					outData.uidEffect != MUID::ZERO)
				{
					stControllerEffectData stResultData;
					stResultData.type				= (CONTROLLER_EFFECT_TYPE)outData.type;
					stResultData.uidEffect			= outData.uidEffect;
					stResultData.strResourceName	= outData.strResourceName;
					stResultData.strBoneName		= outData.strBoneName;
					stResultData.bFollow			= outData.bFollow;
					stResultData.bTerrainAlign		= outData.bTerrainAlign;
					stResultData.bTerrainPos		= outData.bTerrainPos;

					vecOutData->push_back(stResultData);
				}
			}
		}
	}
}

string WS_EffectCharacterToolInvoker::GetReserved_Param( string& strEffectName )
{
	for(vector<RESERVED_EFFECT_DATA>::iterator it = m_vecResrvedEffectData.begin(); it != m_vecResrvedEffectData.end(); it++)
	{
		if((*it).strEffectName == strEffectName)
		{
			return (*it).strParam;
		}
	}

	return "";
}

string WS_EffectCharacterToolInvoker::GetReserved_BoneName( string& strEffectName )
{
	for(vector<RESERVED_EFFECT_DATA>::iterator it = m_vecResrvedEffectData.begin(); it != m_vecResrvedEffectData.end(); it++)
	{
		if((*it).strEffectName == strEffectName)
		{
			return (*it).strBoneName;
		}
	}

	return "";
}

string WS_EffectCharacterToolInvoker::GetReserved_SoundName( string& strEffectName, string& strEffectParam, unsigned int nFlags)
{
	for(vector<RESERVED_EFFECT_DATA>::iterator it = m_vecResrvedEffectData.begin(); it != m_vecResrvedEffectData.end(); it++)
	{
		if((*it).strEffectName == strEffectName)
		{
			string strSoundResourceName = (*it).strSoundResourceName;

			// 발자국소리
			if("foot_left" == strEffectName			||
				"foot_right" == strEffectName		||
				"foot_back" == strEffectName		||
				"foot_jump_up" == strEffectName	||
				"foot_jump_down" == strEffectName	||
				"foot_draw" == strEffectName		||
				"foot_attack" == strEffectName)
			{
				strSoundResourceName = "footstep_pnt_" + strSoundResourceName;
			}
			// 스윙
			else if("swing1" == strEffectName	||
				"swing2" == strEffectName	||
				"swing3" == strEffectName	||
				"swing4" == strEffectName	||
				"weaponfall" == strEffectName)
			{
				//WEAPON_TYPE weapon_type = g_pMainApp->GetActor()->GetCurrentWeaponType();
				//if ( ( weapon_type != WEAPON_MAX ) && ( weapon_type != WEAPON_NONE ) )
				//{
				//	strSoundResourceName = string(XStrings::WeaponType(WEAPON_TYPE(weapon_type))) + "_" + strSoundResourceName;
				//}
			}
			else if ("as_up" == strEffectName ||
				"as_down" == strEffectName ||
				"as_run" == strEffectName)
			{
				//string str = GetRemoveUnderBarfirst(strEffectName);

				//ARMOR_TYPE ArmorType = ARMOR_HEAVY;
				//string EquipmentTypeStr = g_pMainApp->m_pSoundMgr.GetArmorTypeStrSound(ArmorType);
				//strSoundResourceName = "as_" + EquipmentTypeStr + "_" + str;
			}

			return strSoundResourceName;
		}
	}

	return "";
}

string WS_EffectCharacterToolInvoker::GetReserved_ModelName( string& strEffectName )
{
	return "";
}

MUID WS_EffectCharacterToolInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	MUID uidEffect(MUID::ZERO);

	// 이펙트 이름
	if(m_pActor != NULL &&
		!_data.strResourceName.empty())
	{
		RActorNode * pActorNode = m_pActor->GetActorNode(_data.strBoneName.c_str());
		if(modelInfo.bFollow)
		{
			//1-uidEffect = g_pMainApp->m_pEffectManager->AddTEffect(_data.strResourceName, m_pActor, pActorNode, matLocal, false, bAlign, bImmediatlyDelete, fDurationTime, fStartVisibilityTime, fEndVisibilityTime);
			uidEffect = m_pEffectManager->AddWSEffect(_data.strResourceName, m_pActor, pActorNode, _data.matWorldMatrix, false, false, modelInfo.bImmediatlyDelete, modelInfo.fDurationTime, modelInfo.fStartVisibilityTime, modelInfo.fEndVisibilityTime);
		}
		else
		{
			// 본을 가지고 있으면... 본의 월드를 가져온다.
			// 없으면... 캐릭터 원점의 월드를 가져온다.
			vec3 vWorldPosition = vec3::ZERO;
			vec3 vWorldDirection = vec3::ZERO;
			vec3 vWorldUp = vec3::ZERO;

			vWorldPosition = m_pActor->GetWorldPosition();
			vWorldDirection = m_pActor->GetWorldDirection();
			vWorldUp = m_pActor->GetWorldUp();
			
			if(pActorNode)
			{
				//1-m_pActor->GetAnimatedActorNodePos(_data.strBoneName.c_str(), vWorldPosition, E_TS_WORLD);
				m_pActor->GetNodePosition(const_cast<char*>(_data.strBoneName.c_str()), vWorldPosition);
			}

			//1-uidEffect = g_pMainApp->m_pEffectManager->AddTEffect(_data.strResourceName, NULL, NULL, vWorldPosition, vWorldDirection, matLocal, bTerrainAlign, false, false, bImmediatlyDelete, fDurationTime, fStartVisibilityTime, fEndVisibilityTime);
			uidEffect = m_pEffectManager->AddWSEffect(_data.strResourceName, NULL, NULL, vWorldPosition, vWorldDirection, vWorldUp, modelInfo.bTerrainAlign, false, false, modelInfo.bImmediatlyDelete, modelInfo.fDurationTime, modelInfo.fStartVisibilityTime, modelInfo.fEndVisibilityTime);
		}

		// 스케일값 적용
		//1-g_pMainApp->m_pEffectManager->SetScaleTEffect(uidEffect, bAutoScale, fScale);
		m_pEffectManager->SetScaleWSEffect(uidEffect, modelInfo.bAutoScale, modelInfo.vScale);
	}

	return uidEffect;
}

MUID WS_EffectCharacterToolInvoker::SetSound( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	if(_data.strResourceName.empty()) 
		return MUID::ZERO;

	//1-MUID uidSound = g_pMainApp->m_pSoundMgr.PlaySound(_data.strResourceName, _data.strBoneName);
	MUID uidSound = MUID::ZERO;
	return uidSound;
}

void WS_EffectCharacterToolInvoker::SetCamera( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo )
{

}

void WS_EffectCharacterToolInvoker::GetEffect_Data( CUSTOM_EFFECT_TYPE eEffectType, string& strEffectName, string& strEffectParam, string& strEffectBoneName, vector<CHECK_EFFECT_DATA>* vecOutData )
{
	//1-XEffectInfo * pEffectInfo = g_pMainApp->m_pEffectInfoMgr->GetEffectInfo(strEffectName);
	XEffectInfo * pEffectInfo = m_pEffectInfoMgr->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		string strBoneName	= strEffectBoneName.empty() ? GetReserved_BoneName(strEffectName) : strEffectBoneName;

		// 이벤트 발생
		for(vector<XBaseEffectInfoItem *>::iterator itEvent = pEffectInfo->m_vecEffectInfoItem.begin(); 
			itEvent != pEffectInfo->m_vecEffectInfoItem.end(); itEvent++)
		{
			if((*itEvent)->eEffectType == CET_MODEL &&
				eEffectType == CET_MODEL)
			{
				CHECK_EFFECT_DATA outData;

				XEffectInfoItem_Model * pEffectInfoItem_Model = static_cast<XEffectInfoItem_Model*>(*itEvent);
				outData.strEffectName = strEffectName;
				outData.strResourceName = pEffectInfoItem_Model->strResourceName.empty() ? GetReserved_ModelName(strEffectName) : pEffectInfoItem_Model->strResourceName;
				outData.strBoneName = pEffectInfoItem_Model->strModelBoneName.empty() ? strBoneName : pEffectInfoItem_Model->strModelBoneName;

				vecOutData->push_back(outData);
			}
			if((*itEvent)->eEffectType == CET_SOUND &&
				eEffectType == CET_SOUND)
			{
				CHECK_EFFECT_DATA outData;

				XEffectInfoItem_Sound * pEffectInfoItem_Sound = static_cast<XEffectInfoItem_Sound*>(*itEvent);
				outData.strEffectName = strEffectName;
				outData.strResourceName = pEffectInfoItem_Sound->strResourceName.empty() ? GetReserved_SoundName(strEffectName, strEffectParam, 0) : pEffectInfoItem_Sound->strResourceName;
				outData.strBoneName = pEffectInfoItem_Sound->strSoundBoneName.empty() ? strBoneName : pEffectInfoItem_Sound->strSoundBoneName;

				vecOutData->push_back(outData);
			}
		}
	}
}

void WS_EffectCharacterToolInvoker::SetPostEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_PostEffect& postInfo )
{
	if(m_pActor)
	{
		//1-g_pMainApp->m_pEffectManager->SetPostEffect(nPostEffecType, m_pActor, fPlayTime, fScatTime, fSpeed, nScarFactorType);
		m_pEffectManager->SetPostEffect(postInfo.nPostEffectType, m_pActor, postInfo.fPlayTime, postInfo.fScatTime, postInfo.fSpeed, postInfo.nScarFactorType);
	}
}

//////////////////////////////////////////////////////////////////////////
WS_EffectCharacterToolSinglyInvoker::WS_EffectCharacterToolSinglyInvoker()
: XEffectBaseInvoker()
{
	m_pActor = NULL;
	m_pEffectManager = NULL;
	m_pEffectInfoMgr = NULL;
}

WS_EffectCharacterToolSinglyInvoker::~WS_EffectCharacterToolSinglyInvoker()
{

}

void WS_EffectCharacterToolSinglyInvoker::Invoke( string& strEffectName, vec3& vPos, vec3& vDir, vec3& vUp, XEffectInfoMgr* pEffectInfoMgr, WS_EffectManager* pEffectManager, vector<stControllerEffectData>* vecOutData /*= NULL*/ )
{
	m_pEffectManager = pEffectManager;
	m_pEffectInfoMgr = pEffectInfoMgr;
	XEffectInfo * pEffectInfo = m_pEffectInfoMgr->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		string strParam		= GetReserved_Param(strEffectName);

		float fEffectDurationTime = pEffectInfo->m_fEffectDurationTime;

		// 이벤트 발생
		for(vector<XBaseEffectInfoItem *>::iterator itEvent = pEffectInfo->m_vecEffectInfoItem.begin(); 
			itEvent != pEffectInfo->m_vecEffectInfoItem.end(); itEvent++)
		{
			map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::iterator itFind = m_mapEffectHandlers.find((*itEvent)->eEffectType);
			if(itFind != m_mapEffectHandlers.end())
			{
				EFFECT_RESULT_DATA outData;
				EFFECT_TRANSEMISION_DATA transemisionData;
				transemisionData.pEffectInfoItem	= (*itEvent);
				transemisionData.strEventName		= strEffectName;
				transemisionData.matWorldMatrix.SetLocalMatrix(vPos, vDir, vUp);

				itFind->second(*this, transemisionData, string(""), strParam, fEffectDurationTime, outData);

				if(vecOutData &&
					outData.uidEffect != MUID::ZERO)
				{
					stControllerEffectData stResultData;
					stResultData.type				= (CONTROLLER_EFFECT_TYPE)outData.type;
					stResultData.uidEffect			= outData.uidEffect;
					stResultData.strResourceName	= outData.strResourceName;
					stResultData.strBoneName		= outData.strBoneName;
					stResultData.bFollow			= outData.bFollow;
					stResultData.bTerrainAlign		= outData.bTerrainAlign;
					stResultData.bTerrainPos		= outData.bTerrainPos;

					vecOutData->push_back(stResultData);
				}
			}
		}
	}
}

MUID WS_EffectCharacterToolSinglyInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	MUID uidEffect(MUID::ZERO);

	if(m_pEffectManager == NULL)
		return uidEffect;

	// 이펙트 이름
	if(!_data.strResourceName.empty())
	{
		vec3 vPos = vec3(_data.matWorldMatrix._41, _data.matWorldMatrix._42, _data.matWorldMatrix._43);
		vec3 vDir = vec3(_data.matWorldMatrix._21, _data.matWorldMatrix._22, _data.matWorldMatrix._23);
		vec3 vUp = vec3(_data.matWorldMatrix._31, _data.matWorldMatrix._32, _data.matWorldMatrix._33);

		uidEffect = m_pEffectManager->AddWSEffect(_data.strResourceName, vPos, vDir, vUp, modelInfo.bTerrainAlign, false, false, modelInfo.bImmediatlyDelete, modelInfo.fDurationTime, modelInfo.fStartVisibilityTime, modelInfo.fEndVisibilityTime);

		// 스케일값 적용
		m_pEffectManager->SetScaleWSEffect(uidEffect, modelInfo.bAutoScale, modelInfo.vScale);
	}

	return uidEffect;
}

MUID WS_EffectCharacterToolSinglyInvoker::SetSound( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	if(_data.strResourceName.empty()) 
		return MUID::ZERO;

//1-	MUID uidSound = g_pMainApp->m_pSoundMgr.PlaySound(_data.strResourceName, _data.strBoneName);
	MUID uidSound = MUID::ZERO;
	return uidSound;
}
