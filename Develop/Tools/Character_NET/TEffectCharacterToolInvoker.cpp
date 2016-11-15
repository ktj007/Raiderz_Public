#include "stdafx.h"
#include "TEffectCharacterToolInvoker.h"

TEffectCharacterToolInvoker::TEffectCharacterToolInvoker()
{
	m_pActor = NULL;
}

TEffectCharacterToolInvoker::~TEffectCharacterToolInvoker()
{

}

void TEffectCharacterToolInvoker::Invoke( TCharacter* pActor, string& strEffectName, string& strEffectParam, string& strEffectBoneName, vector<EFFECT_RESULT_DATA>* vecOutData /*= NULL*/, RMatrix * pMatLocal /*= NULL*/, float fSize /*= 0.0f*/, MMatrix matWorld /*= MMatrix::IDENTITY*/ )
{
	if(g_pMainApp->m_bShowEffect == false)
		return;

	m_pActor = pActor;
	XEffectInfo * pEffectInfo =  g_pMainApp->m_pEffectInfoMgr->GetEffectInfo(strEffectName);
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
				transemisionData.matWorldMatrix		= matWorld;
				transemisionData.vSize				= vec3(fSize, fSize, 0.0f);
				transemisionData.bWaterPos			= pEffectInfo->bWaterPos;
				if(pMatLocal)
					transemisionData.matWorldMatrix = *pMatLocal;

				itFind->second(*this, transemisionData, strBoneName, strParam, fEffectDurationTime, outData);

				if(vecOutData &&
					outData.uidEffect != MUID::ZERO)
				{
					vecOutData->push_back(outData);
				}
			}
		}
	}
}

string TEffectCharacterToolInvoker::GetReserved_Param( string& strEffectName )
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

string TEffectCharacterToolInvoker::GetReserved_BoneName( string& strEffectName )
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

string TEffectCharacterToolInvoker::GetReserved_SoundName( string& strEffectName, string& strEffectParam, unsigned int nFlags)
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
				WEAPON_TYPE weapon_type = g_pMainApp->GetActor()->GetCurrentWeaponType();
				if ( ( weapon_type != WEAPON_MAX ) && ( weapon_type != WEAPON_NONE ) )
				{
					strSoundResourceName = string(XStrings::WeaponType(WEAPON_TYPE(weapon_type))) + "_" + strSoundResourceName;
				}
			}
			else if ("ss" == strEffectName)	//피격사운드
			{
				WEAPON_TYPE weapon_type = g_pMainApp->GetActor()->GetCurrentWeaponType();
				string weaponstr;
				switch (g_pMainApp->GetActor()->GetCurrentWeaponType())
				{
				case WEAPON_1H_PIERCE:	weaponstr = "slice";	break;
				case WEAPON_1H_SLASH:
				case WEAPON_TWO_HANDED:	weaponstr = "slash";	break;
				case WEAPON_1H_BLUNT:	weaponstr = "blunt";	break;
				case WEAPON_2H_BLUNT:	weaponstr = "blunt3";	break;
				case WEAPON_STAFF:		weaponstr = "blunt2";	break;
				case WEAPON_ARCHERY:	weaponstr = "pierce";	break;
				default:				weaponstr = "error";	break;
				}	
				strSoundResourceName = strEffectName + "_" + weaponstr + "_" + "flesh";
			}
			// 방패로 때리기 sd_swing - Effect.xml에 기술
			// 방패에 피격당하는 소리 sd_hit
			// 방패로 막는 소리 sd
			else if (("sd_hit" == strEffectName) )
			{
				strSoundResourceName = "ss_blunt_flesh";
			}

			// TODO : 사장님과 얘기해서 Player와 NPC와의 방어 속성이름을 정해야겠습니다.
			//else if ((pObject->GetTypeID() == ETID_PLAYER) && ("sd" == strEffectName) )
			//{
			//	XPlayer* player = static_cast<XPlayer*>(pObject);

			//	string weaponstr = global.sound->GetArmorTypeStrSound(player->GetEquipmentType());

			//	// 방패?
			//	if(player->IsEquipShield())
			//	{
			//		strSoundResourceName = strEffectName + "_sd_" + weaponstr + "_1";
			//	}
			//	else
			//	{
			//		strSoundResourceName = strEffectName + "_ws_" + weaponstr + "_1";
			//	}
			//}
			//else if ((pObject->GetTypeID() == ETID_NPC) && ("sd" == strEffectName) )
			//{
			//	// NPC는 방패가 개념이 없다.
			//	XNonPlayer* pNPC = static_cast<XNonPlayer*>(pObject);
			//	string AarmorStr = global.sound->GetArmorTypeStrSound(pNPC->GetInfo()->);

			//	strSoundResourceName = strEffectName + "_ws_" + weaponstr + "_" + AarmorStr;
			//}
			else if ("as_up" == strEffectName ||
				"as_down" == strEffectName ||
				"as_run" == strEffectName)
			{
				ARMOR_TYPE ArmorType = ARMOR_HEAVY;
				string str = GetRemoveUnderBarfirst(strEffectName);
				string EquipmentTypeStr;
				switch (ArmorType)
				{
				case ARMOR_CLOTH:	EquipmentTypeStr = "clo";		break;
				case ARMOR_LIGHT:	EquipmentTypeStr = "lea";		break;
				case ARMOR_MEDIUM:	EquipmentTypeStr = "cha";		break;
				case ARMOR_HEAVY:	EquipmentTypeStr = "met";		break;
				default:			EquipmentTypeStr = "error";		break;
				}
				strSoundResourceName = "as_" + EquipmentTypeStr + "_" + str;
			}

			return strSoundResourceName;
		}
	}

	return "";
}

string TEffectCharacterToolInvoker::GetReserved_ModelName( string& strEffectName )
{
	return "";
}

MUID TEffectCharacterToolInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	MUID uidEffect(MUID::ZERO);

	// 이펙트 이름
	if(m_pActor != NULL &&
		!_data.strResourceName.empty())
	{
		vec3 vLocal(modelInfo.fx, modelInfo.fy, modelInfo.fz);
		RActorNode * pActorNode = m_pActor->GetActorNode(_data.strBoneName.c_str());
		if(modelInfo.bFollow)
		{
			MMatrix matFrame;
			matFrame.MakeIdentity();

			if(_data.matWorldMatrix != MMatrix::IDENTITY)
			{
				// 이 매트릭스를 local로 바꾸어야 한다.
				MMatrix matLocal = _data.matWorldMatrix;
				MMatrix matActorWorld;
				MMatrix matWorldInverse;
				matActorWorld = m_pActor->GetWorldTransform();
				m_pActor->GetActorNodeTransform(_data.strBoneName.c_str(), matActorWorld, E_TS_WORLD);

				matActorWorld.GetInverse(&matWorldInverse);
				matLocal *= matWorldInverse;

				matFrame = matLocal;
			}

			if(vLocal != vec3::ZERO)
			{
				MMatrix matLocal;
				matLocal.SetTranslationMatrix(vLocal);

				matFrame = matLocal * matFrame;
			}

			uidEffect = g_pMainApp->m_pEffectManager->AddTEffect(_data.strResourceName, m_pActor, pActorNode, matFrame, modelInfo.bLastFramePause, !modelInfo.bAlign, modelInfo.bImmediatlyDelete, _data.bWaterPos, modelInfo.fDurationTime, modelInfo.fStartVisibilityTime, modelInfo.fEndVisibilityTime);
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
				m_pActor->GetActorNodePos(_data.strBoneName.c_str(), vWorldPosition, E_TS_WORLD);
			}

			RMatrix matWorld;
			matWorld.SetLocalMatrix(vWorldPosition, vWorldDirection, vec3::AXISZ);
			matWorld *= _data.matWorldMatrix;

			if(vLocal != vec3::ZERO)
			{
				MMatrix matLocal;
				matLocal.SetTranslationMatrix(vLocal);
				matWorld = matLocal * matWorld;
			}

			vWorldPosition = matWorld.GetTranslation();
			vWorldDirection = vec3(matWorld._21, matWorld._22, matWorld._23);
			vWorldUp = vec3(matWorld._31, matWorld._32, matWorld._33);

			if(_data.bWaterPos)
				vWorldPosition.z = SWIMMING_HEIGHT_VALUE_TEST;

			uidEffect = g_pMainApp->m_pEffectManager->AddTEffect(_data.strResourceName, NULL, NULL, vWorldPosition, vWorldDirection, vWorldUp, modelInfo.bTerrainAlign, modelInfo.bTerrainPos, modelInfo.bLastFramePause, false, modelInfo.bImmediatlyDelete, _data.bWaterPos, modelInfo.fDurationTime, modelInfo.fStartVisibilityTime, modelInfo.fEndVisibilityTime);

			TEffect* pEffect = g_pMainApp->m_pEffectManager->GetTEffect(uidEffect);
			if(pEffect)
				resultData.bLoop = pEffect->IsEffectLoop();
		}

		// 스케일값 적용
		g_pMainApp->m_pEffectManager->SetScaleTEffect(uidEffect, modelInfo.bAutoScale, modelInfo.vScale);
	}

	return uidEffect;
}

MUID TEffectCharacterToolInvoker::SetSound( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	if(_data.strResourceName.empty()) 
		return MUID::ZERO;

//	MUID uidSound = g_pMainApp->m_pSoundMgr.PlaySound(_data.strResourceName, _data.strBoneName);
//	return uidSound;

	//<<<rsound3
	MUID uid = g_pMainApp->m_pEffectManager->GenerateLocalUID();
	RSound* pSound = g_pMainApp->m_pSoundMgr.PlayDynamic3D(_data.strResourceName.c_str(), _data.strBoneName.c_str(), uid);
	if(pSound)
	{
		resultData.bLoop = pSound->GetLoop();
		return uid;
	}
	return MUID::ZERO;
}

void TEffectCharacterToolInvoker::SetCamera( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo )
{

}

void TEffectCharacterToolInvoker::GetEffect_Data( CUSTOM_EFFECT_TYPE eEffectType, string& strEffectName, string& strEffectParam, string& strEffectBoneName, vector<CHECK_EFFECT_DATA>* vecOutData )
{
	XEffectInfo * pEffectInfo = g_pMainApp->m_pEffectInfoMgr->GetEffectInfo(strEffectName);
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

void TEffectCharacterToolInvoker::SetPostEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_PostEffect& postInfo )
{
	if(m_pActor)
	{
		g_pMainApp->m_pEffectManager->SetPostEffect(postInfo.nPostEffectType, m_pActor, postInfo.fPlayTime, postInfo.fScatTime, postInfo.fSpeed, postInfo.nScarFactorType, postInfo.nBlurType);
	}
}

MUID TEffectCharacterToolInvoker::SetIlluminationEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_IlluminationEffect& illuminationInfo )
{
	if(m_pActor)
	{
		return g_pMainApp->m_pEffectManager->SetSelfIlluminationEffect(_data.strBoneName, illuminationInfo.fStartTime, illuminationInfo.fStartDurationTime, illuminationInfo.fEndTime, illuminationInfo.fEndDurationTime, illuminationInfo.fDurationTime, illuminationInfo.fMin, illuminationInfo.fMax, illuminationInfo.bLoop);
	}

	return MUID::ZERO;
}

MUID TEffectCharacterToolInvoker::SetDecalEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_DecalEffect& decalInfo )
{
	MUID uidEffect(MUID::ZERO);

	vec3 vPos;

	if(decalInfo.bFollow == false)
	{
		if(m_pActor)
		{
			RActorNode * pActorNode = m_pActor->GetActorNode(_data.strBoneName.c_str());
			MMatrix matTempWorld;
			matTempWorld.MakeIdentity();
			if(pActorNode)
				matTempWorld.SetLocalMatrix(pActorNode->GetWorldPosition(), pActorNode->GetWorldDirection(), pActorNode->GetWorldUp());
			else
				matTempWorld.SetLocalMatrix(m_pActor->GetWorldPosition(), m_pActor->GetWorldDirection(), m_pActor->GetWorldUp());

			if(_data.matWorldMatrix != MMatrix::IDENTITY)
			{
				MMatrix matDecalWorld = _data.matWorldMatrix * matTempWorld;
				vPos = matDecalWorld.GetTranslation();
			}
			else
			{
				vPos = matTempWorld.GetTranslation();
			}
		}
		else
			vPos = _data.matWorldMatrix.GetTranslation();
	}

	float fTempSize = _data.vSize.x;

	if(decalInfo.fSizeX > 0.0f ||
		decalInfo.fSizeY > 0.0f)
	{
		fTempSize = decalInfo.fSizeX;
	}

	uidEffect = g_pMainApp->m_pEffectManager->AddXDecalEffect(_data.strResourceName, _data.strBoneName, vPos, decalInfo.bFollow, decalInfo.nDecalEffectType, _data.dwColor, fTempSize);

	return uidEffect;
}

void TEffectCharacterToolInvoker::SetAlphaTestFadeEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_AlphaTestFadeEffect& fadeInfo )
{
	if(m_pActor)
	{
		g_pMainApp->m_pEffectManager->SetNoiseEffect(fadeInfo.vecstrMeshNodeName, fadeInfo.fStartDurationTime, fadeInfo.fEndDurationTime, fadeInfo.dwFadeEffectColor1, fadeInfo.dwFadeEffectColor2, fadeInfo.fFadeEffectSize1, fadeInfo.fFadeEffectSize2, fadeInfo.bInverse);
	}
}

//////////////////////////////////////////////////////////////////////////
TEffectCharacterToolWeaponInvoker::TEffectCharacterToolWeaponInvoker()
{
	m_pActor = NULL;
}

TEffectCharacterToolWeaponInvoker::~TEffectCharacterToolWeaponInvoker()
{

}

void TEffectCharacterToolWeaponInvoker::Invoke( TCharacter* pActor, string& strEffectName, string& strEffectBoneName, vector<EFFECT_RESULT_DATA>* vecOutData /*= NULL*/ )
{
	if(g_pMainApp->m_bShowEffect == false)
		return;

	m_pActor = pActor;
	XEffectInfo * pEffectInfo =  g_pMainApp->m_pEffectInfoMgr->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		string strBoneName	= strEffectBoneName;

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

				itFind->second(*this, transemisionData, strBoneName, string(""), 0.0f, outData);

				if(vecOutData &&
					outData.uidEffect != MUID::ZERO)
				{
					vecOutData->push_back(outData);
				}
			}
		}
	}
}

MUID TEffectCharacterToolWeaponInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	MUID uidEffect(MUID::ZERO);

	// 이펙트 이름
	if(!_data.strResourceName.empty())
	{
		g_pMainApp->m_pEffectManager->SetWeaponEffect(m_pActor, _data.strResourceName, _data.strBoneName);

		// 스케일값 적용
		g_pMainApp->m_pEffectManager->SetScaleTEffect(uidEffect, modelInfo.bAutoScale, modelInfo.vScale);
	}

	return uidEffect;
}

MUID TEffectCharacterToolWeaponInvoker::SetSound( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	if(_data.strResourceName.empty()) 
		return MUID::ZERO;

	MUID uid = g_pMainApp->m_pEffectManager->GenerateLocalUID();
	RSound* pSound = g_pMainApp->m_pSoundMgr.PlayDynamic3D( _data.strResourceName.c_str(), _data.strBoneName.c_str(), uid);
	if(pSound)
	{
		resultData.bLoop = pSound->GetLoop();
		return uid;
	}
	return MUID::ZERO;
}