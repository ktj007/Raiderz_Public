#include "stdafx.h"
#include "VSoundDataValidator.h"
#include "XTalentInfo.h"
#include "XStrings.h"
#include "MStringUtil.h"

VCheckEffectBaseInvoker::VCheckEffectBaseInvoker()
: XEffectBaseInvoker()
{
	m_vecWeaponType_SS.push_back("slash");
	m_vecWeaponType_SS.push_back("slice");
	m_vecWeaponType_SS.push_back("slash2");
	m_vecWeaponType_SS.push_back("blunt");
	m_vecWeaponType_SS.push_back("blunt2");
	m_vecWeaponType_SS.push_back("blunt3");
	m_vecWeaponType_SS.push_back("pierce");

	m_vecArmorType_SS.push_back("flesh");
	m_vecArmorType_SS.push_back("spirit");
	m_vecArmorType_SS.push_back("soul");
	m_vecArmorType_SS.push_back("iron");
	m_vecArmorType_SS.push_back("stone");
	m_vecArmorType_SS.push_back("wood");
	m_vecArmorType_SS.push_back("undead");

	m_vecFootStepMaterialName.push_back("clo");
	m_vecFootStepMaterialName.push_back("con");
	m_vecFootStepMaterialName.push_back("drt");
	m_vecFootStepMaterialName.push_back("met");
	m_vecFootStepMaterialName.push_back("pnt");
	m_vecFootStepMaterialName.push_back("snd");
	m_vecFootStepMaterialName.push_back("snw");
	m_vecFootStepMaterialName.push_back("wat");
	m_vecFootStepMaterialName.push_back("wod");

	m_vecArmorType_AS.push_back("clo");
	m_vecArmorType_AS.push_back("lea");
	m_vecArmorType_AS.push_back("cha");
	m_vecArmorType_AS.push_back("met");

	m_vecSoundType_SD.push_back("met");
	m_vecSoundType_SD.push_back("wod");
	m_vecSoundType_SD.push_back("flesh");
}

bool VCheckEffectBaseInvoker::CheckResrvedEffectData( string strSoundName )
{
	for(vector<RESERVED_EFFECT_DATA>::iterator it = m_vecResrvedEffectData.begin(); it != m_vecResrvedEffectData.end(); ++it)
	{
		// 발자국소리
		if("foot_left" == it->strEffectName			||
			"foot_right" == it->strEffectName		||
			"foot_back" == it->strEffectName		||
			"foot_jump_up" == it->strEffectName	||
			"foot_jump_down" == it->strEffectName	||
			"foot_draw" == it->strEffectName		||
			"foot_attack" == it->strEffectName)
		{
			if(CheckResrvedEffectData_FootStep(it->strSoundResourceName, strSoundName))
				return true;
		}

		if(it->strEffectName == "swing1"  ||  it->strEffectName == "swing2"  ||  it->strEffectName == "swing3"  ||  it->strEffectName == "swing4"  || it->strEffectName == "swing5"  ||  it->strEffectName == "weaponfall")
		{
			for(int i = 0; i < WEAPON_MAX; ++i)
			{
				string strSoundResourceName = string(XStrings::WeaponType(WEAPON_TYPE(i))) + "_" + it->strEffectName;
				if(strSoundName == strSoundResourceName)
					return true;
			}
		}

		if(it->strEffectName == "ss1"  ||  it->strEffectName == "ss2"  ||  it->strEffectName == "ss3"  ||  it->strEffectName == "ss4"  ||  it->strEffectName == "ss5" ||
			it->strEffectName == "t_ss1" || it->strEffectName == "t_ss2")
		{
			if(CheckResrvedEffectData_SS(it->strEffectName, strSoundName))
				return true;
		}

		if(it->strEffectName == "sd")
		{
			if(CheckResrvedEffectData_SD(strSoundName))
				return true;
		}

		if(it->strEffectName == "as_up"  ||  it->strEffectName == "as_down"  ||  it->strEffectName == "as_run")
		{
			if(CheckResrvedEffectData_AS(it->strSoundResourceName, strSoundName))
				return true;
		}

		if(strSoundName == "ss_sd" || strSoundName == "ss_punch")
			return true;

		if(it->strEffectName == strSoundName)
			return true;
	}
	
	return false;
}

bool VCheckEffectBaseInvoker::CheckResrvedEffectData_SS( string strPre, string strSoundName )
{
	for(vector<string>::iterator itWeaponType = m_vecWeaponType_SS.begin(); itWeaponType != m_vecWeaponType_SS.end(); ++itWeaponType)
	{
		for(vector<string>::iterator itArmorType = m_vecArmorType_SS.begin(); itArmorType != m_vecArmorType_SS.end(); ++itArmorType)
		{
			string strSoundResourceName = strPre + "_" + *itWeaponType + "_" + *itArmorType;
			if(strSoundResourceName == strSoundName)
				return true;
		}

		// cri
		string strSoundResourceName = "ss_" + *itWeaponType + "_" + "cri";
		if(strSoundResourceName == strSoundName)
			return true;

		// miss
		strSoundResourceName = "ss_" + *itWeaponType + "_" + "miss";
		if(strSoundResourceName == strSoundName)
			return true;
	}

	return false;
}

bool VCheckEffectBaseInvoker::CheckResrvedEffectData_FootStep( string strSoundResourceName, string strSoundName )
{
	for(vector<string>::iterator itFootStep = m_vecFootStepMaterialName.begin(); itFootStep != m_vecFootStepMaterialName.end(); ++itFootStep)
	{
		string strSoundFullName = "footstep_" + *itFootStep + "_" + strSoundResourceName;
		if(strSoundFullName == strSoundName)
			return true;
	}

	return false;
}

bool VCheckEffectBaseInvoker::CheckResrvedEffectData_AS( string strSoundResourceName, string strSoundName )
{
	for(vector<string>::iterator itAs = m_vecArmorType_AS.begin(); itAs != m_vecArmorType_AS.end(); ++itAs)
	{
		string strSoundFullName = "as_" + *itAs + "_" + strSoundResourceName;
		if(strSoundFullName == strSoundName)
			return true;
	}
	
	return false;
}

bool VCheckEffectBaseInvoker::CheckResrvedEffectData_SD( string strSoundName )
{
	for(vector<string>::iterator itSD = m_vecSoundType_SD.begin(); itSD != m_vecSoundType_SD.end(); ++itSD)
	{
		string strSoundWSName = "sd_ws_" + *itSD;
		string strSoundSDName = "sd_sd_" + *itSD;

		if(strSoundWSName == strSoundName ||
			strSoundSDName == strSoundName)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
VSoundDataValidator::VSoundDataValidator()
:VDataValidator()
, m_nSoundCount(0)
{

}

VSoundDataValidator::~VSoundDataValidator()
{

}

void VSoundDataValidator::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_nSoundCount = vInfoMgr->m_pSoundInfoMgr->m_mapSoundInfo.size();
}

void VSoundDataValidator::CheckStart( VValidatorInfoMgr* vInfoMgr )
{
	for(map< string, RSampleInfo>::iterator it = vInfoMgr->m_pSoundInfoMgr->m_mapSoundInfo.begin(); it != vInfoMgr->m_pSoundInfoMgr->m_mapSoundInfo.end(); ++it)
	{
		CheckSoundInfo(vInfoMgr->m_pSoundInfoMgr, (string)it->first, &(it->second), vInfoMgr);

		CheckSoundUsable((string)it->first, vInfoMgr);
	}
}

void VSoundDataValidator::CheckSoundInfo( VSoundInfoMgr* pSoundInfoMgr, string& strName, RSampleInfo* pSoundInfo, VValidatorInfoMgr* vInfoMgr )
{
	// 공백 체크
	string strFileNameTrimBlanks = cml2::TrimBlanks(pSoundInfo->strFileName);
	if(strFileNameTrimBlanks != pSoundInfo->strFileName)
	{
		string strError = strName + " 사운드에서 filename='" + pSoundInfo->strFileName + "'에 공백이 있습니다.";
		m_SoundInfoValidatorError.Add(EMT_STRING, strError);
	}

	// 파일 체크
	if(CheckSoundFileName(pSoundInfoMgr, strFileNameTrimBlanks) == false)
	{
		string strError = strName + " 사운드에서 " + pSoundInfo->strFileName + " 파일이 없습니다.";
		m_SoundInfoValidatorError.Add(EMT_FILE, strError);
	}
}

bool VSoundDataValidator::CheckSoundFileName( VSoundInfoMgr* pSoundInfoMgr, string strFileName )
{
	for(vector<FILENAME_DATA>::iterator it = pSoundInfoMgr->m_vecSoundFileList.begin(); it != pSoundInfoMgr->m_vecSoundFileList.end(); ++it)
	{
		string strCheckSourceFileName = "../../../" + strFileName;
		string strCheckTargetFileName = it->strPath + it->strFileName;

		_strlwr((char*)strCheckSourceFileName.c_str());
		_strlwr((char*)strCheckTargetFileName.c_str());
		
		if(strCheckSourceFileName == strCheckTargetFileName)
			return true;
	}

	return false;
}

void VSoundDataValidator::CheckSoundUsable( string& strName, VValidatorInfoMgr* vInfoMgr )
{
	bool bUsed = false;
	if(bUsed == false && CheckSoundUsableFromEffectInfo(vInfoMgr->m_pEffectInfoMgr, strName))
	{
		bUsed = true;
	}

	if(bUsed == false && CheckSoundUsableFromTalentInfo(vInfoMgr->m_pTalentInfoMgr, strName))
	{
		bUsed = true;
	}

	if(bUsed == false && CheckSoundUsableFromMeshInfo(vInfoMgr->m_pMeshInfoMgr, strName))
	{
		bUsed = true;
	}

	if(bUsed == false && CheckSoundUsableFromNpcInfo(vInfoMgr->m_pNpcInfoMgr, strName))
	{
		bUsed = true;
	}

	if(bUsed == false && CheckSoundUsableFromItemInfo(vInfoMgr->m_pItemInfoMgr, strName))
	{
		bUsed = true;
	}

	if(bUsed == false && CheckSoundUsableFromFieldInfo(strName))
	{
		bUsed = true;
	}

	if(bUsed == false && CheckSoundUsableFromSystemInfo(vInfoMgr, strName))
	{
		bUsed = true;
	}


	if(bUsed == false)
	{
		string strError = strName + " 사운드가 어디에도 쓰이지 않고 있습니다.";
		m_SoundInfoValidatorError.Add(EMT_LINK, strError);
	}
}

bool VSoundDataValidator::CheckSoundUsableFromEffectInfo( VEffectInfoMgr* pEffectInfoMgr, string strSoundName )
{
	for(vector<XEffectInfo *>::iterator itEffect = pEffectInfoMgr->m_vecEffectInfo.begin(); itEffect != pEffectInfoMgr->m_vecEffectInfo.end(); ++itEffect)
	{
		for(vector<XBaseEffectInfoItem *>::iterator itItem = (*itEffect)->m_vecEffectInfoItem.begin(); itItem != (*itEffect)->m_vecEffectInfoItem.end(); ++itItem)
		{
			if((*itItem)->eEffectType == CET_SOUND && (*itItem)->strResourceName == strSoundName)
				return true;
		}
	}

	// 예약어
	if(m_CheckEffectBaseInvoker.CheckResrvedEffectData(strSoundName))
		return true;

	return false;
}

bool VSoundDataValidator::CheckSoundUsableFromTalentInfo( VTalentInfoMgr* pTalentInfoMgr, string strSoundName )
{
	for(map<int , CSTalentInfo* >::iterator itTalentInfo = pTalentInfoMgr->m_TalentInfoMgr.begin(); itTalentInfo != pTalentInfoMgr->m_TalentInfoMgr.end(); ++itTalentInfo)
	{
		XTalentInfo* pTalentInfo = static_cast<XTalentInfo*>(itTalentInfo->second);
	
		for(vector<CSTalentEventInfo> ::iterator itNor = pTalentInfo->m_Events.m_vecEvents.begin(); itNor != pTalentInfo->m_Events.m_vecEvents.end(); ++itNor)
		{
			if(itNor->m_nEvent == TE_SOUND && itNor->m_strParam1 == strSoundName)
				return true;
		}

		for(vector<CSTalentEventInfo> ::iterator itAct = pTalentInfo->m_Events.m_vecActEvents.begin(); itAct != pTalentInfo->m_Events.m_vecActEvents.end(); ++itAct)
		{
			if(itAct->m_nEvent == TE_SOUND && itAct->m_strParam1 == strSoundName)
				return true;
		}
	}

	return false;
}

bool VSoundDataValidator::CheckSoundUsableFromMeshInfo( VMeshInfoMgr* pMeshInfoMgr, string strSoundName )
{
	for(map<string, VMeshInfo*>::iterator itMesh = pMeshInfoMgr->m_mapMeshInfo.begin(); itMesh != pMeshInfoMgr->m_mapMeshInfo.end(); ++itMesh)
	{
		for(vector<ANIMATION_EVENT_INFO>::iterator itEventInfo = itMesh->second->m_AnimationInfoMgr.m_AnimationEventInfo.m_vecAnimationEvents.begin(); 
			itEventInfo != itMesh->second->m_AnimationInfoMgr.m_AnimationEventInfo.m_vecAnimationEvents.end();
			++itEventInfo)
		{
			for(vector<RAnimationEvent*>::iterator itEvent = itEventInfo->vecAnimationEvent.begin(); itEvent != itEventInfo->vecAnimationEvent.end(); ++itEvent)
			{
				string strEvent = (*itEvent)->m_strEvent;
				string strParam1 = (*itEvent)->m_strParam1;
				if((*itEvent)->m_strEvent == "sh_sound" &&
					(*itEvent)->m_strParam1 == strSoundName)
					return true;
			}
		}

		// 파츠 사운드
		CSMeshInfo* pMeshInfo = pMeshInfoMgr->m_MeshInfo.GetInfo(itMesh->first);
		if(pMeshInfo)
		{
			for(int i = 0; i < 4; ++i)
			{
				if(pMeshInfo->m_strBPartsCollisionSound[i] == strSoundName ||
					pMeshInfo->m_strBPartsStartSound[i] == strSoundName)
					return true;
			}
		}

		// 히트 사운드
		for(map<tstring, tstring>::iterator itHitSound = itMesh->second->m_AnimationInfoMgr.m_AnimationHitSoundInfo.m_mapAnimationHitSoundList.begin(); 
			itHitSound != itMesh->second->m_AnimationInfoMgr.m_AnimationHitSoundInfo.m_mapAnimationHitSoundList.end();
			++itHitSound)
		{
			if(strSoundName == itHitSound->second)
				return true;
		}
	}

	return false;
}

bool VSoundDataValidator::CheckSoundUsableFromNpcInfo( VNpcInfoMgr* pNpcInfoMgr, string strSoundName )
{
	for(map<int, CSNPCInfo*>::iterator it = pNpcInfoMgr->begin(); it != pNpcInfoMgr->end(); ++it)
	{
		XNPCInfo* pNpcInfo = static_cast<XNPCInfo*>(it->second);
		
		if(pNpcInfo->SoundInfo.CombatBGM == strSoundName ||
			pNpcInfo->SoundInfo.DyingSound == strSoundName ||
			pNpcInfo->SoundInfo.EntitySound == strSoundName ||
			pNpcInfo->SoundInfo.HitSound1 == strSoundName ||
			pNpcInfo->SoundInfo.HitSound2 == strSoundName ||
			pNpcInfo->SoundInfo.HitSound3 == strSoundName ||
			pNpcInfo->SoundInfo.Aggro == strSoundName)
			return true;
	
		if(pNpcInfo->eVoiceType > VOICE_TYPE_NONE &&
			pNpcInfo->eVoiceType < VOICE_TYPE_MAX)
		{
			for(int i = 0; i < VOICE_SAY_MAX; ++i)
			{
				string strSoundResourceName = XStrings::VoiceTypeToString( pNpcInfo->eVoiceType) + string("_") + XStrings::VoiceSayToString(i);
				if(strSoundName == strSoundResourceName)
					return true;
			}
		}
	}

	return false;
}

bool VSoundDataValidator::CheckSoundUsableFromItemInfo( VItemInfoMgr* pItemInfoMgr, string strSoundName )
{
	if(VFileName::stristr(strSoundName, "item_") == false)
		return false;

	// 기본 검사
	if("item_coin_up" == strSoundName ||
		"item_coin_down" == strSoundName)
		return true;

	// 아이템 검사
	for(XItemDataMap::iterator it = pItemInfoMgr->m_ItemDataMap.begin(); it != pItemInfoMgr->m_ItemDataMap.end(); ++it)
	{
		XItemData* pItemData = it->second;

		if(pItemData->m_ItemSoundType < 0 ||
			pItemData->m_ItemSoundType >= SOUND_TYPE_MAX)
			continue;

		string strSoundType = string(XStrings::SoundTypeToString(pItemData->m_ItemSoundType));
		string strItemSoundDown = "item_" + strSoundType + "_down";
		string strItemSoundUp = "item_" + strSoundType + "_up";
		string strItemSoundMake = "item_" + strSoundType + "_make";

		if(strItemSoundDown == strSoundName ||
			strItemSoundUp == strSoundName ||
			strItemSoundMake == strSoundName)
			return true;
	}

	return false;
}

bool VSoundDataValidator::CheckSoundUsableFromFieldInfo( string strSoundName )
{
	// 임시로...
	// 2차 클베이후 처리
	if(VFileName::stristr(strSoundName, "bgm_") ||
		VFileName::stristr(strSoundName, "part") ||
		VFileName::stristr(strSoundName, "amb_"))
		return true;

	// 서버에서 쓰이는 필드 루아
	if(VFileName::stristr(strSoundName, "scene"))
		return true;

	return false;
}

bool VSoundDataValidator::CheckSoundUsableFromSystemInfo( VValidatorInfoMgr* vInfoMgr, string strSoundName )
{
	// result sound 체크
	if(vInfoMgr->m_pSoundInfoMgr->m_ResultMsg.CheckSoundDataValidator(strSoundName))
		return true;

	// 클라이언트 루아에서 쓰이는 사운드
	if(VFileName::stristr(strSoundName, "button_") ||
		VFileName::stristr(strSoundName, "autoparty_") ||
		VFileName::stristr(strSoundName, "popup_msg_alarm") ||
		VFileName::stristr(strSoundName, "ui_up"))
		return true;

	// 클라이언트에 박혀있는 사운드
	if(VFileName::stristr(strSoundName, "quest_start") ||
		VFileName::stristr(strSoundName, "quest_finish") ||
		VFileName::stristr(strSoundName, "pc_guard") ||
		VFileName::stristr(strSoundName, "interact_round") ||
		VFileName::stristr(strSoundName, "item_food_make"))
		return true;

	return false;
}
