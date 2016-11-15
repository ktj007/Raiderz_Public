#include "stdafx.h"
#include "XEffectInvoker.h"
#include "XEffectManager.h"
#include "XCameraManager.h"
#include "XGame.h"
#include "XWorld.h"
#include "XStrings.h"
#include "XCharacter.h"
#include "XNonPlayer.h"
#//include "XSoundNaming.h"
//#include "XSoundNameMaker.h"
#include "XModuleTalent.h"
#include "XModuleCollision.h"
#include "XModuleActorControl.h"


XEffectInvoker::XEffectInvoker()
:XEffectBaseInvoker()
{
	m_pOwner		= NULL;
	m_pDestObject	= NULL;
}

XEffectInvoker::~XEffectInvoker( void )
{

}

void XEffectInvoker::Invoke( XObject* pSourceObject, wstring& strEffectName, wstring& strEffectParam, wstring& strEffectBoneName, vector<EFFECT_RESULT_DATA>* vecOutData /*= NULL*/, XObject* pDestObject /*= NULL*/, EFFECT_POST_TRANSFORM* pPostData /*= NULL*/ )
{
	if(info.effect_Info == NULL)	return;

	m_pOwner = pSourceObject;
	m_pDestObject = pDestObject;

	XEffectInfo * pEffectInfo = info.effect_Info->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		wstring strBoneName	= strEffectBoneName.empty() ? GetReserved_BoneName(strEffectName) : strEffectBoneName;
		wstring strParam		= strEffectParam.empty() ? GetReserved_Param(strEffectName) : strEffectParam;

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
				transemisionData.bWaterPos			= pEffectInfo->bWaterPos;
				transemisionData.fWaterHeightMin	= pEffectInfo->fWaterHeightMin;
				transemisionData.fWaterHeightMax	= pEffectInfo->fWaterHeightMax;
				transemisionData.bCancelEnabled		= pEffectInfo->bCancelEnabled;

				if(pPostData)
				{
					transemisionData.matWorldMatrix	= pPostData->matWorldMatrix;
					transemisionData.dwColor		= pPostData->dwColor;
					transemisionData.vSize			= pPostData->vSize;
					transemisionData.vDecalSize		= pPostData->vDecalSize;
					transemisionData.bAttackByMy	= pPostData->bAttackByMy;
					transemisionData.nDamageFlags	= pPostData->nDamageFlags;
				}

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

MUID XEffectInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	MUID uidEffect(MUID::ZERO);

	if (global.emgr == NULL) return uidEffect;
	
	// 이펙트 이름
	if(!_data.strResourceName.empty() &&
		m_pOwner &&
		m_pOwner->GetActor())
	{
		if(_data.bWaterPos && CheckInWater(m_pOwner->GetPosition(), _data.fWaterHeightMin, _data.fWaterHeightMax) == false)
			return MUID::ZERO;
		
		vec3 vLocal(modelInfo.fx, modelInfo.fy, modelInfo.fz);

		// 외부에서 스케일값이 들어왔다면... 기존 스케일보다 우선권으로 처리
		if(_data.vSize != vec3::ZERO)
			modelInfo.vScale = _data.vSize;

		RActorNode * pActorNode = m_pOwner->GetActor()->GetActorNode(MLocale::ConvUTF16ToAnsi(_data.strBoneName.c_str()).c_str());
		EFFECT_CREATE_DATA effectCreateData(_data.strResourceName, _data.strBoneName, (MUID)m_pOwner->GetUID(), _data.matWorldMatrix, vLocal, modelInfo.fDurationTime, modelInfo.vScale, modelInfo.fStartVisibilityTime, modelInfo.fEndVisibilityTime, modelInfo.bLastFramePause, !modelInfo.bAlign, modelInfo.bImmediatlyDelete, modelInfo.bAutoScale, modelInfo.bFollow, modelInfo.bTerrainAlign, modelInfo.bTerrainPos, _data.bWaterPos, _data.bCancelEnabled);

		//if(vLocal != vec3::ZERO)
		//{
		//	MMatrix matTemp;
		//	matTemp.SetTranslationMatrix(vLocal);
		//	effectCreateDatat.matPostMatrix *= matTemp;
		//}

		effectCreateData.SetWaterHeight(_data.fWaterHeightMin, _data.fWaterHeightMax);

		// 이펙트가 붙일 모델이 존재하는가?
		if(CheckActorLoadingComplete_Effect_Model(effectCreateData, uidEffect) == false)
			return uidEffect;
		
		uidEffect = global.emgr->AddXEffect(effectCreateData);

		XEffect* pEffect = global.emgr->GetXEffect(uidEffect);
		if(pEffect && pEffect->IsEffectLoop())
			resultData.bLoop = true;
	}

	return uidEffect;
}

MUID XEffectInvoker::SetSound( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	if ( global.sound == NULL || _data.strResourceName.empty()) 
		return MUID::ZERO;

	if(_data.bWaterPos && CheckInWater(m_pOwner->GetPosition(), _data.fWaterHeightMin, _data.fWaterHeightMax) == false)
		return MUID::ZERO;

	if ( global.sound)
	{
		RSound* pSound = NULL;
		MUID uid = global.system->GenerateLocalUID();

		if ( _data.strBoneName.empty() == false)
			pSound = global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(_data.strResourceName.c_str()).c_str(), uid, _data.bAttackByMy ? PLAY_EMPHASIZE : 0, m_pOwner->GetUID(), MLocale::ConvUTF16ToAnsi(_data.strBoneName.c_str()).c_str());

		else if ( m_pDestObject != NULL)
			pSound = global.sound->PlayStatic3D( MLocale::ConvUTF16ToAnsi(_data.strResourceName.c_str()).c_str(), uid,_data.bAttackByMy ? PLAY_EMPHASIZE : 0, _data.matWorldMatrix.GetTranslation());

		else if ( m_pOwner != NULL)
			pSound = global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(_data.strResourceName.c_str()).c_str(), uid,_data.bAttackByMy ? PLAY_EMPHASIZE : 0, m_pOwner->GetUID());

		if(pSound == NULL)
			uid = MUID::ZERO;
		else
		{
			resultData.bLoop = pSound->GetLoop();
		}

		return uid;
	}

	return MUID::ZERO;
}

void XEffectInvoker::SetCamera( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo )
{
	float fLen = 0.0f;

	if (CheckPlayCameraEffect() == false) return;

	// 내 이벤트 인가?
	if(m_pOwner->GetUID() != XGetMyUID())
	{
		XObject * pObject = gg.omgr->Find(XGetMyUID());
		if(pObject)
		{
			// 반경 조사
			fLen = (m_pOwner->GetPosition() - pObject->GetPosition()).Length();
		}
	}

	if(fLen <= cameraInfo.nDistance)
	{
		// 카메라 이펙트 발생
		//mlog("카메라 이펙트 발생 %s\n", strResourceName.c_str());
		global.camera->PlayCameraEffect(_data.strResourceName.c_str(), cameraInfo.fDurationTime);
	}
}

bool XEffectInvoker::CheckPlayCameraEffect()
{
	// 나와 연관있거나 NPC에 의해 나온 카메라 효과만 플레이한다.
	if(m_pOwner->GetUID() == XGetMyUID())
	{
		return true;
	}

	if (m_pDestObject)
	{
		if (m_pDestObject->GetUID() == XGetMyUID())
		{
			return true;
		}
	}
	else
	{
		if (m_pOwner->IsNPC())
		{
			return true;
		}
	}

	return false;
}

wstring XEffectInvoker::GetReserved_Param( wstring& strEffectName )
{
	for(vector<RESERVED_EFFECT_DATA>::iterator it = m_vecResrvedEffectData.begin(); it != m_vecResrvedEffectData.end(); it++)
	{
		if((*it).strEffectName == strEffectName)
		{
			return (*it).strParam;
		}
	}

	return L"";
}

wstring XEffectInvoker::GetReserved_BoneName( wstring& strEffectName )
{
	for(vector<RESERVED_EFFECT_DATA>::iterator it = m_vecResrvedEffectData.begin(); it != m_vecResrvedEffectData.end(); it++)
	{
		if((*it).strEffectName == strEffectName)
		{
			return (*it).strBoneName;
		}
	}

	return L"";
}

wstring XEffectInvoker::GetReserved_SoundName( wstring& strEffectName, wstring& strEffectParam, unsigned int nFlags)
{
	if(m_pOwner == NULL)
		return L"";

	for(vector<RESERVED_EFFECT_DATA>::iterator it = m_vecResrvedEffectData.begin(); it != m_vecResrvedEffectData.end(); it++)
	{
		if((*it).strEffectName == strEffectName)
		{
			wstring strSoundResourceName = (*it).strSoundResourceName;

			// 발자국소리
			if(L"foot_left" == strEffectName			||
				L"foot_right" == strEffectName		||
				L"foot_back" == strEffectName		||
				L"foot_jump_up" == strEffectName	||
				L"foot_jump_down" == strEffectName	||
				L"foot_draw" == strEffectName		||
				L"foot_attack" == strEffectName)
			{
				if (gg.currentgamestate && gg.currentgamestate->GetWorld())
				{
					wstring mt;
					if(m_pOwner->GetModuleCollision())
						mt = m_pOwner->GetModuleCollision()->GetMaterialName();
					if(mt.empty())
						mt = gg.currentgamestate->GetWorld()->GetMaterialName(m_pOwner->GetPosition());
					strSoundResourceName = L"footstep_" + mt + L"_" + strSoundResourceName;
				}
			}

			// 스윙
			else if( strEffectName == L"swing1"  ||  strEffectName == L"swing2"  ||  strEffectName == L"swing3"  ||  strEffectName == L"swing4"  ||  strEffectName == L"swing5"  ||  strEffectName == L"weaponfall")
			{
				XActor* pActor = static_cast<XActor*>( m_pOwner);
				WEAPON_TYPE weapon_type = pActor->GetCurrWeaponType();
				if ( weapon_type != WEAPON_MAX  &&  weapon_type != WEAPON_NONE)
					strSoundResourceName = wstring(XStrings::WeaponType(WEAPON_TYPE(weapon_type))) + L"_" + strSoundResourceName;
			}

			else if ( m_pDestObject != NULL  &&
				(strEffectName == L"ss1"  ||  strEffectName == L"ss2"  ||  strEffectName == L"ss3"  ||  strEffectName == L"ss4"  ||  strEffectName == L"ss5" ||
				strEffectName == L"t_ss1" || strEffectName == L"t_ss2"))
			{
				XTalentInfo* pAttackTalentInfo = NULL;
				if ( m_pDestObject->GetEntityType() == ETID_PLAYER)
				{
					if ( m_pDestObject->GetModuleTalent()->IsActive())
						pAttackTalentInfo = m_pDestObject->GetModuleTalent()->GetInfo();
				}
				else if ( m_pOwner->GetEntityType() == ETID_PLAYER)
				{
					// 피격자가 플레이어라면... 
					// 임시로 넣습니다. 정리가 되는데로 다시 구현합니다.
					return L"ss_blunt_bug";
				}

				strSoundResourceName = GetHitSoundName( strEffectName, strEffectParam, m_pDestObject, m_pOwner, pAttackTalentInfo, nFlags);
			}

			// 방패로 때리기 sd_swing(Effect.xml에 기술) - 방패에 피격당하는 소리 = sd_hit, 방패로 막는 소리 = sd
			else if ( strEffectName == L"sd_hit")
			{
				if ( m_pOwner->GetEntityType() == ETID_NPC)
				{
					XNonPlayer* pNPC = AsNPC(m_pOwner);
					wstring strArmor = GetArmorTypeSoundName(pNPC->GetInfo(true)->nArmorType);

					strSoundResourceName = L"ss_blunt_" + strArmor;
				}
			}

			// TODO : 사장님과 얘기해서 Player와 NPC와의 방어 속성이름을 정해야겠습니다.
			else if(L"sd" == strEffectName)
			{
				strSoundResourceName = GetGuardSoundName(strEffectName, m_pDestObject, m_pOwner);
			}

			else if ( m_pOwner->GetEntityType() == ETID_PLAYER  &&
				(strEffectName == L"as_up"  ||  strEffectName == L"as_down"  ||  strEffectName == L"as_run"))
			{
				XPlayer* player = static_cast<XPlayer*>( m_pOwner);

				wstring ArmorMatTypeStr;
				switch ( player->GetArmorType())
				{
				case ARMOR_CLOTH :		ArmorMatTypeStr = L"clo";		break;
				case ARMOR_LIGHT :		ArmorMatTypeStr = L"lea";		break;
				case ARMOR_MEDIUM :		ArmorMatTypeStr = L"cha";		break;
				case ARMOR_HEAVY :		ArmorMatTypeStr = L"met";		break;
				default :				ArmorMatTypeStr = L"error";		break;
				}

				strSoundResourceName = L"as_" + ArmorMatTypeStr + L"_" + (*it).strSoundResourceName;
			}

			return strSoundResourceName;
		}
	}

	return L"";
}

wstring XEffectInvoker::GetHitSoundName( wstring strPrefix, wstring strPostfix, XObject* pAttacker, XObject* pVictim, XTalentInfo* pAttackTalentInfo, unsigned int nDamageFlags)
{
	// Weapon
	WEAPON_TYPE nWeaponType = WEAPON_1H_SLASH;
	if ( pAttacker != NULL)
	{
		if ( pAttacker->GetEntityType() == ETID_PLAYER)
		{
			XPlayer* pAttackerPlayer = dynamic_cast<XPlayer*>( pAttacker);
			if ( pAttackerPlayer != NULL)
			{
				nWeaponType = pAttackerPlayer->GetCurrWeaponType();

				// 방패로 치는 소리 처리는 따로한다.
				if ( pAttackTalentInfo != NULL  &&  strPostfix.empty() == true)
				{
					if ( pAttackTalentInfo->m_WeaponReference == WR_LEFT)
					{
						if ( pAttackerPlayer->IsEquipShield() == true)		strPostfix = L"ss_sd";
						else												strPostfix = L"ss_punch";
					}
				}
			}
		}
		else if ( pAttacker->GetEntityType() == ETID_NPC)
		{
			XNonPlayer* pNPC = dynamic_cast<XNonPlayer*>( pAttacker);
			if ( pNPC != NULL  &&  pAttackTalentInfo)
			{
				switch ( pAttackTalentInfo->m_nDamageAttrib)
				{
				case DA_SLASH:		nWeaponType = WEAPON_1H_SLASH;		break;
				case DA_BLUNT:		nWeaponType = WEAPON_1H_BLUNT;		break;
				case DA_PIERCE:		nWeaponType = WEAPON_1H_PIERCE;		break;
				}
			}
		}
	}

	wstring strWeapon;
	switch ( nWeaponType)
	{
	case WEAPON_1H_SLASH :			strWeapon = L"slash";				break;
	case WEAPON_1H_PIERCE :			strWeapon = L"slice";				break;
	case WEAPON_TWO_HANDED :		strWeapon = L"slash2";				break;
	case WEAPON_1H_BLUNT :			strWeapon = L"blunt";				break;
	case WEAPON_2H_BLUNT :			strWeapon = L"blunt3";				break;
	case WEAPON_STAFF :				strWeapon = L"blunt2";				break;
	case WEAPON_ARCHERY :			strWeapon = L"pierce";				break;
	default:						strWeapon = L"error";				break;
	}


	// Armor
	ARMOR_ATTRIB nArmorType = ARMOR_ATT_NEUTRAL;
	if ( pVictim != NULL  &&  pVictim->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNPC = dynamic_cast<XNonPlayer*>( pVictim);
		if ( pNPC != NULL)		nArmorType = pNPC->GetInfo( true)->nArmorType;
	}

	wstring strArmor = GetArmorTypeSoundName(nArmorType);

	// Etc - Critical, Miss 추가 내용
	wstring strRet;
	if ( pVictim != NULL)
	{
		XModuleActorControl* pActorControl = pVictim->GetModuleActorControl();
		if ( pActorControl != NULL)
		{
			wstring strHitFlagString;
			if ( CheckBitSet( nDamageFlags, CTR_CRITICAL))		strHitFlagString = L"cri";
			else if ( CheckBitSet( nDamageFlags, CTR_MISS))		strHitFlagString = L"miss";

			if ( strHitFlagString.empty() == false)
			{
				strPrefix = L"ss";
				strRet = strPrefix + L"_" + strWeapon + L"_" + strHitFlagString;

				return strRet;
			}
		}
	}

	if ( strPostfix.size() >= 2)
	{
		// postfix 제일 마지막이 '_'로 끝나면 앞에서부터 'weapon_' 까지 교체
		if ( strPostfix[ strPostfix.size() - 1] == L'_')
			strRet = strPostfix + strArmor;

		// postfix 앞에 'ss'가 있으면 사운드 이름 통째로 교체
		else if ( (strPostfix[ 0] == L's'  ||  strPostfix[ 0] == L'S')  &&  (strPostfix[ 1] == L's'  ||  strPostfix[ 1] == L'S'))
			strRet = strPostfix;
	}
	else
	{
		strRet = strPrefix + L"_" + strWeapon + L"_" + strArmor;
		if ( strPostfix.empty() == false)	strRet = strRet + L"_" + strPostfix;
	}

	return strRet;
}

wstring XEffectInvoker::GetReserved_ModelName( wstring& strEffectName )
{
	return L"";
}

void XEffectInvoker::GetEffect_Data( CUSTOM_EFFECT_TYPE eEffectType, XObject* pObject, XObject* pDestObject, wstring& strEffectName, wstring& strEffectParam, wstring& strEffectBoneName, vector<CHECK_EFFECT_DATA>* vecOutData )
{
	XEffectInfo * pEffectInfo = info.effect_Info->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		wstring strBoneName	= strEffectBoneName.empty() ? GetReserved_BoneName(strEffectName) : strEffectBoneName;

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
	
bool XEffectInvoker::CheckActorLoadingComplete_Effect_Model(EFFECT_CREATE_DATA& _effectData, MUID& uidOut)
{
	// 모델에 붙여지면서 본에 붙여지는 것만 한다. 그외는 무시...

	if(m_pOwner &&
		m_pOwner->GetActor() &&
		_effectData.strBoneName.empty() == false &&
		_effectData.bFollow == true)
	{
		if(m_pOwner->GetActor()->CheckLoadingCompletedFastly() == false)
		{
			// 로딩 완료 안됨.
			uidOut = global.emgr->AddEffectWaiting(_effectData);

			return false;
		}
	}

	return true;
}

void XEffectInvoker::SetPostEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_PostEffect& postInfo )
{
	if(m_pOwner)
	{
		if(CheckUseMotionBlur(postInfo.nPostEffectType, postInfo.fPlayTime, postInfo.fScatTime, postInfo.fSpeed, postInfo.nDistance, postInfo.nScarFactorType, postInfo.nUseType) == false)
			return;

		global.emgr->SetPostEffect(postInfo.nPostEffectType, m_pOwner, postInfo.fPlayTime, postInfo.fScatTime, postInfo.fSpeed, postInfo.nScarFactorType, postInfo.nBlurType);

	}
}

bool XEffectInvoker::CheckUseMotionBlur( BYTE nPostEffecType, float fPlayTime, float fScatTime, float fSpeed, int nDistance, BYTE nScarFactorType, BYTE nUseType )
{
	// 모션 블러만 검사
	if(nPostEffecType == PEK_MOTION_BLUR)
	{
		// 적용이 되는 타입 검사
		if(nUseType == PEUT_MYSELF &&
			m_pOwner->GetUID() != XGetMyUID())
		{
			return false;
		}

		// 모션 블러만 반경 검사를 하자.
		if(nDistance > 0 &&
			m_pOwner->GetUID() != XGetMyUID())
		{
			// 거리 제한이 있다.
			XObject * pMyPlayerObject = gg.omgr->Find(XGetMyUID());
			if(pMyPlayerObject)
			{
				// 반경 조사
				float fLen = (m_pOwner->GetPosition() - pMyPlayerObject->GetPosition()).Length();

				if(fLen > nDistance)
				{
					return false;
				}
			}
		}
	}

	return true;
}

MUID XEffectInvoker::SetIlluminationEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_IlluminationEffect& illuminationInfo )
{
	MUID uidEffect(MUID::ZERO);

	// 이펙트 이름
	if(!_data.strResourceName.empty())
	{
		EFFECT_CREATE_DATA effectCreateDatat(_data.strResourceName, _data.strBoneName, (MUID)m_pOwner->GetUID(), _data.matWorldMatrix, vec3(0, 0, 0), illuminationInfo.fDurationTime, 1.0f, 0.0f, 0.0f, false, false, false, false, false, false, false, false, _data.bCancelEnabled);
		uidEffect = global.emgr->AddXSelfIllumination(effectCreateDatat, illuminationInfo.fStartTime, illuminationInfo.fStartDurationTime, illuminationInfo.fEndTime, illuminationInfo.fEndDurationTime, illuminationInfo.fMin, illuminationInfo.fMax, illuminationInfo.bLoop);
	}

	return uidEffect;
}

MUID XEffectInvoker::SetDecalEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_DecalEffect& decalInfo )
{
	MUID uidEffect(MUID::ZERO);

	EFFECT_CREATE_DATA effectCreateDatat;

	if(m_pOwner)
	{
		effectCreateDatat.uidActor		= (MUID)m_pOwner->GetUID();
		effectCreateDatat.matPostMatrix.SetLocalMatrix(m_pOwner->GetPosition(), m_pOwner->GetDirection(), vec3(vec3::AXISZ));
	}
	else
		effectCreateDatat.matPostMatrix	= _data.matWorldMatrix;


	effectCreateDatat.strEffectName			= _data.strResourceName;
	effectCreateDatat.strBoneName			= _data.strBoneName;
	effectCreateDatat.bFollow				= decalInfo.bFollow;
	effectCreateDatat.fStartVisibilityTime	= decalInfo.fStartVisibilityTime;
	effectCreateDatat.fEndVisibilityTime	= decalInfo.fEndVisibilityTime;

	float fTempSizeX = _data.vDecalSize.x;
	float fTempSizeY = _data.vDecalSize.y;
	float fTempSizeZ = _data.vDecalSize.z;

	if(decalInfo.fSizeX > 0.0f ||
		decalInfo.fSizeY > 0.0f)
	{
		fTempSizeX = decalInfo.fSizeX;
		fTempSizeY = decalInfo.fSizeY;
		fTempSizeZ = decalInfo.fSizeZ;
	}

	uidEffect = global.emgr->AddXDecalEffect(effectCreateDatat, decalInfo.nDecalEffectType, _data.dwColor, fTempSizeX, fTempSizeY, fTempSizeZ);

	return uidEffect;
}

MUID XEffectInvoker::SetLightEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_LightEffect& lightInfo )
{
	MUID uidEffect(MUID::ZERO);

	XCharacter* pActor = NULL;
	RActorNode* pActorNode = NULL;

	wstring strBoneName =_data.strBoneName;

	MMatrix matResult;
	matResult.MakeIdentity();

	bool bFollow = lightInfo.bFollow;

	if(m_pOwner)
	{
		pActor = m_pOwner->GetActor();
		RActorNode * pActorNode = m_pOwner->GetActor()->GetActorNode(MLocale::ConvUTF16ToAnsi(_data.strBoneName.c_str()).c_str());

		// 피격용 계산... 클라에서 별도로 조작
		if(lightInfo.bAttackLight && m_pDestObject)
		{
			// 위치
			vec3 vBackDir = (m_pDestObject->GetPosition() - m_pOwner->GetPosition()).Normalize();
			XMath::NormalizeZ0(vBackDir);

			vBackDir *= 10.0f;
			vec3 vPos = _data.matWorldMatrix.GetTranslation();
			vPos += vBackDir;
			matResult.SetLocalMatrix(vPos, vec3::AXISY, vec3::AXISZ);

			pActorNode = NULL;
			strBoneName = L"";

			// 범위(radius)
			vec3 vTerrainPos = vPos;
			vec3 vPickPos = vPos + (vec3(0, 0, 1) * TERRAIN_HEIGHT_COLLISION_VALUE);
			if(gg.currentgamestate->GetWorld() &&
				gg.currentgamestate->GetWorld()->PickCollision(vPos, vPickPos, &vTerrainPos, NULL, CollisionAttribute::ALL))
			{
				float fCalRaduis = (vPos - vTerrainPos).Length();

				lightInfo.fRadius = min(fCalRaduis, lightInfo.fRadius);
			}

			// 감쇠 시작 거리
			lightInfo.fAttenuationStart = lightInfo.fRadius * 0.4f;

			// 세기 조절(환경)
			if(gg.currentgamestate->GetWorld() &&
				gg.currentgamestate->GetWorld()->GetWorldEnv())
			{
				GAME_TIME_TYPE eTimeType = gg.currentgamestate->GetWorld()->GetWorldEnv()->GetCurrentTime();
				if(eTimeType == TIME_DAWN ||
					eTimeType == TIME_SUNSET)
				{
					lightInfo.fIntensity = lightInfo.fIntensity * 0.7f;
				}
				else if(eTimeType == TIME_NIGHT)
				{
					lightInfo.fIntensity = lightInfo.fIntensity * 0.5f;
				}
			}
		}
	}
	else
	{
		matResult = _data.matWorldMatrix;
		bFollow = false;
	}

	uidEffect = global.emgr->AddXLightEffect(m_pOwner, strBoneName, lightInfo.fRadius, lightInfo.fAttenuationStart, lightInfo.fIntensity, lightInfo.dwColor, lightInfo.fDurationTime, bFollow, matResult);
	return uidEffect;
}

bool XEffectInvoker::CheckInWater(vec3& vPos, float fHeightMin, float fHeightMax)
{
	vec3 vOutWater;
	vec3 vWaterTo	= vPos;
	vWaterTo.z		+= 10000.0f;
	if(gg.currentgamestate->GetWorld()->PickCollision(vPos, vWaterTo, &vOutWater, NULL, CollisionAttribute::WATER))
	{
		float fHeight = vOutWater.z - vPos.z;
		if((fHeightMin == 0.0f && fHeightMax == 0.0f) ||
			(fHeight >= fHeightMin && fHeight <= fHeightMax))
		{
			return true;
		}
	}

	return false;
}

void XEffectInvoker::SetAlphaTestFadeEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_AlphaTestFadeEffect& fadeInfo )
{	
	global.emgr->SetXNoiseFadeEffect(m_pOwner, fadeInfo.vecstrMeshNodeName, fadeInfo.fStartDurationTime, fadeInfo.fEndDurationTime, fadeInfo.dwFadeEffectColor1, fadeInfo.dwFadeEffectColor2, fadeInfo.fFadeEffectSize1, fadeInfo.fFadeEffectSize2, fadeInfo.bInverse);
}

wstring XEffectInvoker::GetGuardSoundName( wstring strPrefix, XObject* pAttacker, XObject* pVictim )
{
	wstring strSoundResourceName;

	if (pVictim && pVictim->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* player = static_cast<XPlayer*>(pVictim);

		XItemData* pLeftItemData = NULL;
		XItemData* pRightItemData = NULL;
		player->GetCurrentWeaponSetWeaponItemData(&pRightItemData, &pLeftItemData);

		wstring str = L"met";

		if(player->IsEquipShield())
		{
			if(pLeftItemData)
				str = GetSoundTypeSoundName( pLeftItemData->m_ItemSoundType);

			strSoundResourceName = strPrefix + L"_sd_" + str;
		}
		else
		{
			 if(pRightItemData)
				str = GetSoundTypeSoundName( pRightItemData->m_ItemSoundType);

			strSoundResourceName = strPrefix + L"_ws_" + str;
		}
	}
	else if (pVictim->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNPC = AsNPC(m_pOwner);

	
		CSItemData* pLeftItemData = NULL;
		CSItemData* pRightItemData = NULL;
		pNPC->GetNPCWeaponItemData(&pRightItemData, &pLeftItemData);

		wstring str = L"flesh";

		if(pNPC->IsEquipShield())
		{
			if(pLeftItemData)
				str = GetSoundTypeSoundName( pLeftItemData->m_ItemSoundType);

			strSoundResourceName = strPrefix + L"_sd_" + str;
		}
		else
		{
			if(pRightItemData)
			{
				str = GetSoundTypeSoundName( pRightItemData->m_ItemSoundType);
				strSoundResourceName = strPrefix + L"_ws_" + str;
			}
			else
			{
				// 별도의 무기 데이터가 없으면 다음으로 처리...
				strSoundResourceName = strPrefix + L"_sd_" + str;
			}
		}

	}

	return strSoundResourceName;
}

tstring XEffectInvoker::GetArmorTypeSoundName( ARMOR_ATTRIB nArmorType )
{
	switch ( nArmorType)
	{
	case ARMOR_ATT_NEUTRAL :
	case ARMOR_ATT_ANIMAL :
	case ARMOR_ATT_DRAGON1 :
	case ARMOR_ATT_DRAGON2 :
	case ARMOR_ATT_BUG :			return L"flesh";				break;
	case ARMOR_ATT_FIRE :
	case ARMOR_ATT_WATER :
	case ARMOR_ATT_LIGHTNING :
	case ARMOR_ATT_POISON :			return L"spirit";				break;
	case ARMOR_ATT_HOLY :
	case ARMOR_ATT_UNHOLY :			return L"soul";					break;
	case ARMOR_ATT_IRON :			return L"iron";					break;
	case ARMOR_ATT_STONE :			return L"stone";				break;
	case ARMOR_ATT_WOOD :			return L"wood";					break;
	case ARMOR_ATT_UNDEAD :			return L"undead";				break;
	}

	return L"error";
}

tstring XEffectInvoker::GetSoundTypeSoundName( SOUND_TYPE nSoundType )
{
	switch(nSoundType)
	{
	case SOUND_TYPE_UNIVERSAL:
	case SOUND_TYPE_CLOTH:
	case SOUND_TYPE_LEATHER:
	case SOUND_TYPE_CHAIN:
	case SOUND_TYPE_IRON:	
	case SOUND_TYPE_CUTLERY:
	case SOUND_TYPE_PAPER:
	case SOUND_TYPE_STONE:	
	case SOUND_TYPE_LIQUID:	
	case SOUND_TYPE_JEWEL:	
	case SOUND_TYPE_ORGAN:	
	case SOUND_TYPE_HAMMER:	
	case SOUND_TYPE_COIN:			return L"met";		break;
	case SOUND_TYPE_WOOD:			return L"wod";		break;
	}

	return L"met";
}

//////////////////////////////////////////////////////////////////////////
XEffectSinglyInvoker::XEffectSinglyInvoker()
:XEffectInvoker()
{

}

XEffectSinglyInvoker::~XEffectSinglyInvoker( void )
{

}

void XEffectSinglyInvoker::Invoke( wstring& strEffectName, vec3& vPos, vec3& vDir, vec3& vUp, vector<EFFECT_RESULT_DATA>* vecOutData /*= NULL*/, EFFECT_POST_TRANSFORM* pPostData /*= NULL*/ )
{
	if(info.effect_Info == NULL)	return;

	// 어떤 대상자가 있지 않고 좌표로 만 셋팅하는 이펙트...
	XEffectInfo * pEffectInfo = info.effect_Info->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		wstring strParam		= GetReserved_Param(strEffectName);

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
				transemisionData.bWaterPos			= pEffectInfo->bWaterPos;
				transemisionData.fWaterHeightMin	= pEffectInfo->fWaterHeightMin;
				transemisionData.fWaterHeightMax	= pEffectInfo->fWaterHeightMax;
				transemisionData.bCancelEnabled		= pEffectInfo->bCancelEnabled;

				if(pPostData)
				{
					transemisionData.dwColor		= pPostData->dwColor;
					transemisionData.vSize			= pPostData->vSize;
					transemisionData.vDecalSize		= pPostData->vDecalSize;
				}

				itFind->second(*this, transemisionData, wstring(), strParam, fEffectDurationTime, outData);

				if(vecOutData &&
					outData.uidEffect != MUID::ZERO)
				{
					vecOutData->push_back(outData);
				}
			}
		}
	}
}

MUID XEffectSinglyInvoker::SetSound( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	if(_data.bWaterPos && (CheckInWater(_data.matWorldMatrix.GetTranslation(), 0.0f, 0.0f) == false))
		return MUID::ZERO;

	if ( global.sound  && _data.strResourceName.empty() == false)
	{
		MUID uid = global.system->GenerateLocalUID();

		RSound* pSound = global.sound->PlayStatic3D( MLocale::ConvUTF16ToAnsi(_data.strResourceName.c_str()).c_str(), uid, 0, _data.matWorldMatrix.GetTranslation());
		if(pSound)
		{
			resultData.bLoop = pSound->GetLoop();
			return uid;
		}
	}

	return MUID::ZERO;
}

MUID XEffectSinglyInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	MUID uidEffect(MUID::ZERO);

	// 이펙트 이름
	if(!_data.strResourceName.empty())
	{
		vec3 vLocal(modelInfo.fx, modelInfo.fy * -1.0f, modelInfo.fz);

		MMatrix matTemp;
		matTemp.SetTranslationMatrix(vLocal);

		MMatrix matResult = _data.matWorldMatrix * matTemp;

		if(_data.bWaterPos && (CheckInWater(matResult.GetTranslation(), 0.0f, 0.0f) == false))
			return MUID::ZERO;

		// 외부에서 스케일값이 들어왔다면... 기존 스케일보다 우선권으로 처리
		if(_data.vSize != vec3::ZERO)
			modelInfo.vScale = _data.vSize;

		EFFECT_CREATE_DATA effectCreateData(_data.strResourceName, wstring(), MUID::ZERO, matResult, vLocal, modelInfo.fDurationTime, modelInfo.vScale, modelInfo.fStartVisibilityTime, modelInfo.fEndVisibilityTime, modelInfo.bLastFramePause, false, modelInfo.bImmediatlyDelete, modelInfo.bAutoScale, false, modelInfo.bTerrainAlign, modelInfo.bTerrainPos, _data.bWaterPos, _data.bCancelEnabled);
		uidEffect = global.emgr->AddXEffect(effectCreateData);

		XEffect* pEffect = global.emgr->GetXEffect(uidEffect);
		if(pEffect && pEffect->IsEffectLoop())
			resultData.bLoop = true;
	}

	return uidEffect;
}

void XEffectSinglyInvoker::SetCamera( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo )
{
	float fLen = 0.0f;
	vec3 vPos = _data.matWorldMatrix.GetTranslation();

	// 내 이벤트 인가?
	XObject * pObject = gg.omgr->Find(XGetMyUID());
	if(pObject)
	{
		// 반경 조사
		fLen = (vPos - pObject->GetPosition()).Length();
	}

	if(fLen <= cameraInfo.nDistance)
	{
		// 카메라 이펙트 발생
		//mlog("카메라 이펙트 발생 %s\n", strResourceName.c_str());
		global.camera->PlayCameraEffect(_data.strResourceName.c_str(), cameraInfo.fDurationTime);
	}
}

//////////////////////////////////////////////////////////////////////////
XEffectAdvenceInvoker::XEffectAdvenceInvoker()
:XEffectInvoker()
{

}

XEffectAdvenceInvoker::~XEffectAdvenceInvoker( void )
{

}

void XEffectAdvenceInvoker::Invoke( wstring& strEffectName )
{
	if(info.effect_Info == NULL)	return;

	// 미리 로딩하는 이펙트...
	XEffectInfo * pEffectInfo = info.effect_Info->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
		// 입력된 정보가 없으면... 기본 정보 가져오기
		wstring strParam		= GetReserved_Param(strEffectName);

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

				itFind->second(*this, transemisionData, wstring(), strParam, fEffectDurationTime, outData);
			}
		}
	}
}

MUID XEffectAdvenceInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData )
{
	// 리소스만 로딩하도록한다.
	// 이펙트 이름
	if(!_data.strResourceName.empty())
	{
		global.emgr->PreLoaderXEffect(_data.strResourceName);
	}

	return MUID::ZERO;
}

//////////////////////////////////////////////////////////////////////////
XEffectWeaponInvoker::XEffectWeaponInvoker()
:XEffectInvoker()
{

}

XEffectWeaponInvoker::~XEffectWeaponInvoker( void )
{

}

void XEffectWeaponInvoker::Invoke( XObject* pSourceObject, wstring& strEffectName, wstring& strBoneName, int nPartsSlotType, vector<EFFECT_RESULT_DATA>* vecOutData, XObject* pDestObject /*= NULL*/ )
{
	if(info.effect_Info == NULL)	
		return;

	m_pOwner = pSourceObject;
	m_pDestObject = pDestObject;

	XEffectInfo * pEffectInfo = info.effect_Info->GetEffectInfo(strEffectName);
	if(pEffectInfo)
	{
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
				transemisionData.strBoneName		= strBoneName;
				transemisionData.nPartsSlotType		= nPartsSlotType;

				if(itFind->first != CET_MODEL)
				{
					itFind->second(*this, transemisionData, wstring(), wstring(), fEffectDurationTime, outData);

					if(vecOutData &&
						outData.uidEffect != MUID::ZERO)
					{
						vecOutData->push_back(outData);
					}
				}
				else
				{
					SetWeaponEffectInvoker(transemisionData, fEffectDurationTime, vecOutData);
				}
			}
		}
	}
}

MUID XEffectWeaponInvoker::SetModel( EFFECT_TRANSEMISION_DATA& _data, bool bFollow, bool bTerrainAlign, bool bTerrainPos, bool bWaterPos, bool bAlign, bool bAutoScale, bool bImmediatlyDelete, vec3 vScale, float fDurationTime, float fStartVisibilityTime, float fEndVisibilityTime, bool bLastFramePause )
{
	MUID uidEffect(MUID::ZERO);

	if (global.emgr == NULL) return uidEffect;

	// 이펙트 이름
	if(!_data.strResourceName.empty() &&
		m_pOwner &&
		m_pOwner->GetActor())
	{
		EFFECT_CREATE_DATA effectCreateData(_data.strResourceName, _data.strBoneName, (MUID)m_pOwner->GetUID(), MMatrix::IDENTITY, vec3::ZERO, fDurationTime, vScale, fStartVisibilityTime, fEndVisibilityTime, bLastFramePause, !bAlign, bImmediatlyDelete, bAutoScale, bFollow, bTerrainAlign, bTerrainPos, bWaterPos, false);
		effectCreateData.nPartsSlotType = _data.nPartsSlotType;

		// 이펙트가 붙일 모델이 존재하는가?
		if(CheckActorLoadingComplete_Effect_Weapon(effectCreateData, uidEffect) == false)
			return uidEffect;

		uidEffect = global.emgr->AddXWeaponEffect(effectCreateData);
	}

	return uidEffect;
}

void XEffectWeaponInvoker::SetWeaponEffectInvoker( EFFECT_TRANSEMISION_DATA& _data, const float fEffectDurationTime, vector<EFFECT_RESULT_DATA>* vecOutData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_Model * pEffectInfoItem_Model = static_cast<XEffectInfoItem_Model*>(_data.pEffectInfoItem);

	wstring strModelResourceName = pEffectInfoItem_Model->strResourceName.empty() ? GetReserved_ModelName(_data.strEventName) : pEffectInfoItem_Model->strResourceName;
	float fDurationTime = (fEffectDurationTime > 0.0f) ? fEffectDurationTime : pEffectInfoItem_Model->fDurationTime;

	_data.strResourceName		= strModelResourceName;

	//////////////////////////////////////////////////////////////////////////
	if(_data.strBoneName.empty())
	{
		// 붙일 actornode 이름이 없으면 무기 이펙트 더미 전체에 붙인다.
		SetWeaponEffectAllDummyNode(_data, pEffectInfoItem_Model, vecOutData);
	}
	else
	{
		SetWeaponEffect(_data, pEffectInfoItem_Model, vecOutData);
	}
}

void XEffectWeaponInvoker::SetWeaponEffect( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model * pEffectInfoItem_Model, vector<EFFECT_RESULT_DATA>* vecOutData )
{
	MUID uidWeapon = SetModel(_data, pEffectInfoItem_Model->bFollow, pEffectInfoItem_Model->bTerrainAlign, pEffectInfoItem_Model->bTerrainPos, false, pEffectInfoItem_Model->bAlign, pEffectInfoItem_Model->bAutoScale, pEffectInfoItem_Model->bImmediatlyDelete, pEffectInfoItem_Model->vScale, pEffectInfoItem_Model->fDurationTime, pEffectInfoItem_Model->fStartVisibilityTime, pEffectInfoItem_Model->fEndVisibilityTime, pEffectInfoItem_Model->bLastFramePause);

	if(vecOutData &&
		uidWeapon != MUID::ZERO)
	{
		EFFECT_RESULT_DATA outData;
		outData.type			= _data.pEffectInfoItem->eEffectType;
		outData.uidEffect		= uidWeapon;
		outData.strResourceName	= _data.strResourceName;
		outData.strBoneName		= _data.strBoneName;
		outData.bFollow			= pEffectInfoItem_Model->bFollow;
		outData.bTerrainAlign	= pEffectInfoItem_Model->bTerrainAlign;
		outData.bTerrainPos		= pEffectInfoItem_Model->bTerrainPos;

		vecOutData->push_back(outData);
	}
}

bool XEffectWeaponInvoker::CheckActorLoadingComplete_Effect_Weapon( EFFECT_CREATE_DATA& _effectData, MUID& uidOut )
{
	// 모델에 붙여지면서 본에 붙여지는 것만 한다. 그외는 무시...

	if(m_pOwner &&
		m_pOwner->GetActor() &&
		_effectData.strBoneName.empty() == false &&
		_effectData.bFollow == true)
	{
		if(m_pOwner->GetActor()->CheckLoadingCompletedFastly() == false)
		{
			// 로딩 완료 안됨.
			uidOut = global.emgr->AddWeaponEffectWaiting(_effectData);

			return false;
		}
	}

	return true;
}

void XEffectWeaponInvoker::SetWeaponEffectAllDummyNode( EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model * pEffectInfoItem_Model, vector<EFFECT_RESULT_DATA>* vecOutData )
{
	//////////////////////////////////////////////////////////////////////////
	// 왼쪽
	if(_data.nPartsSlotType == PLAYER_PARTS_SLOT_LWEAPON)
	{
		for(int i = 0; i < WEAPON_EFFECT_DUMMY_MAX_COUNT; ++i)
		{
			TCHAR chEffectName[256] = {0,};
			wsprintf(chEffectName, L"%s%02d", WEAPON_LEFT_EFFECT_DUMMY_NAME, i);

			_data.strBoneName = chEffectName;

			SetWeaponEffect(_data, pEffectInfoItem_Model, vecOutData);
		}

		// 방패
		_data.strBoneName = SHIELD_EFFECT_DUMMY_NAME;
		SetWeaponEffect(_data, pEffectInfoItem_Model, vecOutData);

		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 오른쪽
	if(_data.nPartsSlotType == PLAYER_PARTS_SLOT_RWEAPON)
	{
		for(int i = 0; i < WEAPON_EFFECT_DUMMY_MAX_COUNT; ++i)
		{
			TCHAR chEffectName[256] = {0,};
			wsprintf(chEffectName, L"%s%02d", WEAPON_EFFECT_DUMMY_NAME, i);

			_data.strBoneName = chEffectName;

			SetWeaponEffect(_data, pEffectInfoItem_Model, vecOutData);
		}

		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 기타
}

bool XEffectDelete::Del( CUSTOM_EFFECT_TYPE eType, MUID& uidEffect )
{
	if(eType == CET_MODEL ||
		eType == CET_DECALEFFECT ||
		eType == CET_ILLUMINATION ||
		eType == CET_LIGHTEFFECT)
	{
		return global.emgr->DeleteXEffect(uidEffect);
	}
	else if(eType == CET_SOUND)
	{
		if ( global.sound)
			return global.sound->StopDynamic3D( uidEffect);
	}

	return false;
}

void XEffectDelete::DelList( vector<EFFECT_RESULT_DATA>& effectList )
{
	for(vector<EFFECT_RESULT_DATA>::iterator it = effectList.begin(); it != effectList.end(); ++it)
	{
		Del(it->type, it->uidEffect);
	}

	effectList.clear();
}
