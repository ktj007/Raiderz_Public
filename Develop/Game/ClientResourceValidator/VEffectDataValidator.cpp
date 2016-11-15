#include "stdafx.h"
#include "VEffectDataValidator.h"
#include "XTalentInfo.h"

VEffectDataValidator::VEffectDataValidator()
:VDataValidator()
, m_nEffectInfoCount(0)
, m_nTalentEffectInfoCount(0)
, m_nBuffEffectInfoCount(0)
{
	m_mapEffectHandlers_validator.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::value_type( CET_MODEL,			OnEffect_Model) );
	m_mapEffectHandlers_validator.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::value_type( CET_SOUND,			OnEffect_Sound) );
	m_mapEffectHandlers_validator.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::value_type( CET_CAMERA,			OnEffect_Camera) );
	m_mapEffectHandlers_validator.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::value_type( CET_POSTEFFECT,		OnEffect_PostEffect) );
	m_mapEffectHandlers_validator.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::value_type( CET_DECALEFFECT,		OnEffect_DecalEffect) );
	m_mapEffectHandlers_validator.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::value_type( CET_ILLUMINATION,		OnEffect_IlluminationEffect) );
	m_mapEffectHandlers_validator.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::value_type( CET_LIGHTEFFECT,		OnEffect_LightEffect) );
	m_mapEffectHandlers_validator.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::value_type( CET_ALPHATESTFADE,	OnEffect_AlphaTestFadeEffect) );

}

VEffectDataValidator::~VEffectDataValidator()
{

}

void VEffectDataValidator::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_nEffectInfoCount = vInfoMgr->m_pEffectInfoMgr->m_vecEffectInfo.size();

	m_nTalentEffectInfoCount = vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.size();

	m_nBuffEffectInfoCount = vInfoMgr->m_pBuffInfoMgr->size();
}

void VEffectDataValidator::CheckStart( VValidatorInfoMgr* vInfoMgr )
{
	for(vector<XEffectInfo *>::iterator itEffectInfo = vInfoMgr->m_pEffectInfoMgr->m_vecEffectInfo.begin(); itEffectInfo != vInfoMgr->m_pEffectInfoMgr->m_vecEffectInfo.end(); ++itEffectInfo)
	{
		CheckEffectInfo(*itEffectInfo, vInfoMgr);
	}

	for(map<int , CSTalentInfo* >::iterator itTalentInfo = vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.begin(); itTalentInfo != vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.end(); ++itTalentInfo)
	{
		XTalentInfo* pTalentInfo = vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.Get(itTalentInfo->second->m_nID);

		CheckTalentEffectInfo(pTalentInfo, vInfoMgr);
	}

	for(map<int , CSBuffInfo* >::iterator itBuffInfo = vInfoMgr->m_pBuffInfoMgr->begin(); itBuffInfo != vInfoMgr->m_pBuffInfoMgr->end(); ++itBuffInfo)
	{
		XBuffInfo* pBuffInfo = vInfoMgr->m_pBuffInfoMgr->Get(itBuffInfo->second->m_nID);

		CheckBuffEffectInfo(pBuffInfo, vInfoMgr);
	}
}

void VEffectDataValidator::CheckEffectInfo( XEffectInfo * pEffectInfo, VValidatorInfoMgr* vInfoMgr )
{
	string strEffect =pEffectInfo->m_strEffectName + " : ";
	for(vector<XBaseEffectInfoItem *>::iterator itEffect = pEffectInfo->m_vecEffectInfoItem.begin(); itEffect != pEffectInfo->m_vecEffectInfoItem.end(); ++itEffect)
	{
		map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER_VALIDATOR>::iterator itFind = m_mapEffectHandlers_validator.find((*itEffect)->eEffectType);
		if(itFind != m_mapEffectHandlers_validator.end())
		{
			itFind->second(*this, pEffectInfo, *itEffect, vInfoMgr, strEffect);
		}
	}
}

void VEffectDataValidator::OnEffect_Sound(VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo,  XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError )
{
	XEffectInfoItem_Sound * pEffectInfoItem_Sound = static_cast<XEffectInfoItem_Sound*>(pEffectInfoItem);

	if(EffectInvoker.m_CheckEffectInstance.CheckReservedEffect(pEffectInfo->m_strEffectName))
		return;

	if(vInfoMgr->m_pSoundInfoMgr->GetSampleInfo(pEffectInfoItem_Sound->strResourceName) == NULL)
	{
		string strEffectError = strError + "[사운드] 에서 " + pEffectInfoItem_Sound->strResourceName + " 사운드가 sound.xml에 없습니다.";
		EffectInvoker.m_EffectInfoValidatorError.Add(EMT_LINK, strEffectError);
	}
}

void VEffectDataValidator::OnEffect_Model( VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError )
{
	XEffectInfoItem_Model * pEffectInfoItem_Model = static_cast<XEffectInfoItem_Model*>(pEffectInfoItem);

	if(vInfoMgr->m_pEffectInfoMgr->GetEffectFileInfo(pEffectInfoItem_Model->strResourceName) == NULL)
	{
		string strEffectError = strError + "[모델] 에서 " + pEffectInfoItem_Model->strResourceName + ".effect.scene.xml 파일이 없습니다.";
		EffectInvoker.m_EffectInfoValidatorError.Add(EMT_FILE, strEffectError);
	}
}

void VEffectDataValidator::OnEffect_Camera( VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError )
{
	XEffectInfoItem_Camera * pEffectInfoItem_Camera = static_cast<XEffectInfoItem_Camera*>(pEffectInfoItem);

	if(vInfoMgr->m_pCameraInfoMgr->GetCameraFileInfo(pEffectInfoItem_Camera->strResourceName) == NULL)
	{
		string strEffectError = strError + "[카메라] 에서 " + pEffectInfoItem_Camera->strResourceName + ".elu 파일이 없습니다.";
		EffectInvoker.m_EffectInfoValidatorError.Add(EMT_FILE, strEffectError);
	}
}

void VEffectDataValidator::OnEffect_PostEffect( VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError )
{

}

void VEffectDataValidator::OnEffect_DecalEffect( VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError )
{

}

void VEffectDataValidator::OnEffect_IlluminationEffect( VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError )
{

}

void VEffectDataValidator::OnEffect_LightEffect( VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError )
{

}

void VEffectDataValidator::OnEffect_AlphaTestFadeEffect( VEffectDataValidator& EffectInvoker, XEffectInfo * pEffectInfo, XBaseEffectInfoItem* pEffectInfoItem, VValidatorInfoMgr* vInfoMgr, string strError )
{

}

void VEffectDataValidator::CheckTalentEffectInfo( XTalentInfo* pTalentInfo, VValidatorInfoMgr* vInfoMgr )
{
	string strCheckTalentError = (string)pTalentInfo->GetName() + "(" + TransToStringFromNumber(pTalentInfo->m_nID) + ") : ";
	
	for(vector<XTalentEffectInfo *>::iterator itEffect = pTalentInfo->GetEffectDataMgr()->begin(); itEffect != pTalentInfo->GetEffectDataMgr()->end(); ++itEffect)
	{
		if((*itEffect)->m_nEffectType == HIT_EFFECT_POSDIR)
			continue;

		if((*itEffect)->m_strEffectName.empty())
		{
			string strError = strCheckTalentError + "[" + (*itEffect)->m_strTalentEffectInfoName + "] 에서 이펙트 이름이 없습니다.";
			m_TalentEffectInfoValidatorError.Add(EMT_NAME, strError);
		}
		else if(vInfoMgr->m_pEffectInfoMgr->GetEffectInfo((*itEffect)->m_strEffectName) == NULL)
		{
			string strEffecType = GetTextTalentEffectType((*itEffect)->m_nEffectType);
			
			string strError = strCheckTalentError + "[" + (*itEffect)->m_strTalentEffectInfoName + "] 에서 " + (*itEffect)->m_strEffectName + " 이펙트가 effect.xml에 없습니다.";
			m_TalentEffectInfoValidatorError.Add(EMT_LINK, strError);
		}
	}
}

std::string VEffectDataValidator::GetTextTalentEffectType( TALENT_EFFECT_TYPE nEffectType )
{
	if(nEffectType == CAST_EFFECT)
	{
		return EFFECTYPE_STRING_CAST_EFFECT;
	}
	else if(nEffectType == USE_EFFECT)
	{
		return EFFECTYPE_STRING_USE_EFFECT;
	}
	else if(nEffectType == ATTACK_EFFECT)
	{
		return EFFECTYPE_STRING_ATTACK_EFFECT;
	}
	else if(nEffectType == MISS_EFFECT)
	{
		return EFFECTYPE_STRING_MISS_EFFECT;
	}
	else if(nEffectType == SHOCK_EFFECT)
	{
		return EFFECTYPE_STRING_SHOCK_EFFECT;
	}
	else if(nEffectType == DURATION_EFFECT)
	{
		return EFFECTYPE_STRING_DURATION_EFFECT;
	}
	else if(nEffectType == DEFENSE_EFFECT)
	{
		return  EFFECTYPE_STRING_DEFENSE_EFFECT;
	}

	// 모름.
	return "";
}

void VEffectDataValidator::CheckBuffEffectInfo( XBuffInfo* pBuffInfo, VValidatorInfoMgr* vInfoMgr )
{
	string strCheckBuffError = (string)pBuffInfo->GetName() + "(" + TransToStringFromNumber(pBuffInfo->m_nID) + ") : ";

	for(vector<XBuffEffectInfo *>::iterator itEffect = pBuffInfo->m_BuffEffect.begin(); itEffect != pBuffInfo->m_BuffEffect.end(); ++itEffect)
	{
		string strEffecType = GetTextBuffEffectType((*itEffect)->m_nBuffEffectType);

		if((*itEffect)->m_strEffectName.empty())
		{
			string strError = strCheckBuffError + "[" + strEffecType + "] 에서 이펙트 이름이 없습니다.";
			m_TalentEffectInfoValidatorError.Add(EMT_NAME, strError);
		}
		else if(vInfoMgr->m_pEffectInfoMgr->GetEffectInfo((*itEffect)->m_strEffectName) == NULL)
		{
			string strError = strCheckBuffError + "[" + strEffecType + "] 에서 " + (*itEffect)->m_strEffectName + " 이펙트가 effect.xml에 없습니다.";
			m_BuffEffectInfoValidatorError.Add(EMT_LINK, strError);
		}
	}
}

std::string VEffectDataValidator::GetTextBuffEffectType( BUFF_EFFECT_TYPE nBuffType )
{
	if(nBuffType == BUFF_GAIN_EFFECT)
		return BUFF_XML_TAG_BUFF_GAIN_EFFECT;
	else if(nBuffType == BUFF_HIT_EFFECT)
		return BUFF_XML_TAG_BUFF_HIT_EFFECT;

	return "";
}

bool VEffectDataValidator::XEffectDataInvoker::CheckReservedEffect( string& strEffectName )
{
	for(vector<XEffectBaseInvoker::RESERVED_EFFECT_DATA>::iterator it = m_vecResrvedEffectData.begin(); it != m_vecResrvedEffectData.end(); ++it)
	{
		if(it->strEffectName == strEffectName)
			return true;
	}

	return false;
}