#include "stdafx.h"
#include "VTalentDataValidator.h"
#include "XTalentInfo.h"

map<TALENT_EVENT, CHECKTALENTEVENTHANDLER> VTalentDataValidator::m_CheckTalentEventHandlers;

VTalentDataValidator::VTalentDataValidator()
:VDataValidator()
{
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_NONE,					OnCheckTalentEvent_None) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_FIRE_EFFECT,			OnCheckTalentEvent_FireEffect) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_LAUNCH_PROJECTILE,	OnCheckTalentEvent_Projectile) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_EFFECT,				OnCheckTalentEvent_Effect) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_SOUND,				OnCheckTalentEvent_Sound) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_CAMERA,				OnCheckTalentEvent_Camera) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_CUSTOM,				OnCheckTalentEvent_Custom) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_DELAYED_ACT,			OnCheckTalentEvent_Delayed_Act) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_EXTRAACTIVE_1,		OnCheckTalentEvent_ExtraActive_1) );
	m_CheckTalentEventHandlers.insert( map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::value_type( TE_EXTRAACTIVE_2,		OnCheckTalentEvent_ExtraActive_2) );
}

VTalentDataValidator::~VTalentDataValidator()
{

}

void VTalentDataValidator::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_nTalentCount = vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.size();
}

void VTalentDataValidator::CheckStart( VValidatorInfoMgr* vInfoMgr )
{
	VTalentInfoMgr* pTalentInfoMgr = vInfoMgr->m_pTalentInfoMgr;
	for(map<int , CSTalentInfo* >::iterator it = pTalentInfoMgr->m_TalentInfoMgr.begin(); it != pTalentInfoMgr->m_TalentInfoMgr.end(); ++it)
	{
		XTalentInfo* pTalentInfo = pTalentInfoMgr->m_TalentInfoMgr.Get(it->second->m_nID);


		CheckTalentInfoXml();

		CheckTalentEvent(&pTalentInfoMgr->m_TalentInfoMgr, pTalentInfo, vInfoMgr);

		CheckTalentHitInfo(&pTalentInfoMgr->m_TalentInfoMgr, pTalentInfo, vInfoMgr);
	}
}

void VTalentDataValidator::CheckTalentInfoXml()
{

}

void VTalentDataValidator::CheckTalentEvent( XTalentInfoMgr* pTalentInfoMgr, XTalentInfo* pTalentInfo, VValidatorInfoMgr* vInfoMgr )
{
	string strCheckTalentError = (string)pTalentInfo->GetName() + "(" + TransToStringFromNumber(pTalentInfo->m_nID) + ") : ";

	if(pTalentInfo->m_Events.m_nRefID > 0)
	{
		pTalentInfo = pTalentInfoMgr->Get(pTalentInfo->m_Events.m_nRefID);
		if(pTalentInfo == NULL)
		{
			string strError = strCheckTalentError + "레퍼런스 (" + TransToStringFromNumber(pTalentInfo->m_Events.m_nRefID) + ") ID는 존재하지 않는 탤런트 ID입니다.";
			m_TalentEventValidatorError.Add(EMT_LINK, strError);
			return;
		}
	}

	for(vector<CSTalentEventInfo>::iterator itNormal = pTalentInfo->m_Events.m_vecEvents.begin(); itNormal != pTalentInfo->m_Events.m_vecEvents.end(); ++itNormal)
	{
		CheckEventGo(&(*itNormal), vInfoMgr, &strCheckTalentError);
	}
	
	for(vector<CSTalentEventInfo>::iterator itAct = pTalentInfo->m_Events.m_vecActEvents.begin(); itAct != pTalentInfo->m_Events.m_vecActEvents.end(); ++itAct)
	{
		CheckEventGo(&(*itAct), vInfoMgr, &strCheckTalentError);
	}
}

void VTalentDataValidator::CheckEventGo( CSTalentEventInfo * pEvent, VValidatorInfoMgr* vInfoMg, string* pStrErrorPre )
{
	map<TALENT_EVENT, CHECKTALENTEVENTHANDLER>::iterator itFind = m_CheckTalentEventHandlers.find(pEvent->m_nEvent);
	if(itFind != m_CheckTalentEventHandlers.end())
	{
		itFind->second(*this, pEvent, vInfoMg, pStrErrorPre);
	}
}

void VTalentDataValidator::OnCheckTalentEvent_None( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{

}

void VTalentDataValidator::OnCheckTalentEvent_FireEffect( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{
	if(pEvent->m_strParam1.empty())
	{
		string strError = *pStrErrorPre + "[발사] 이벤트에서 이펙트 이름이 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_NAME, strError);
	}
	else if(vInfoMgr->m_pEffectInfoMgr->GetEffectInfo(pEvent->m_strParam1) == NULL)
	{
		string strError = *pStrErrorPre + "[발사] 이벤트에서 " + pEvent->m_strParam1 + " 이펙트가 effect.xml에 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_LINK, strError);
	}
}

void VTalentDataValidator::OnCheckTalentEvent_Projectile( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{
	if(pEvent->m_ProjectileInfo.m_strEffectName.empty() == false &&
		vInfoMgr->m_pEffectInfoMgr->GetEffectInfo(pEvent->m_ProjectileInfo.m_strEffectName) == NULL)
	{
		string strError = *pStrErrorPre + "[발사체] 이벤트에서 [발사체] 이펙트 " + pEvent->m_ProjectileInfo.m_strEffectName + " 이펙트가 effect.xml에 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_LINK, strError);
	}

	if(pEvent->m_ProjectileInfo.m_strFireEffectName.empty() == false &&
		vInfoMgr->m_pEffectInfoMgr->GetEffectInfo(pEvent->m_ProjectileInfo.m_strFireEffectName) == NULL)
	{
		string strError = *pStrErrorPre + "[발사체] 이벤트에서 [발사] 이펙트 " + pEvent->m_ProjectileInfo.m_strFireEffectName + " 이펙트가 effect.xml에 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_LINK, strError);
	}

	if(pEvent->m_ProjectileInfo.m_strAttackEffectName.empty() == false &&
		vInfoMgr->m_pEffectInfoMgr->GetEffectInfo(pEvent->m_ProjectileInfo.m_strAttackEffectName) == NULL)
	{
		string strError = *pStrErrorPre + "[발사체] 이벤트에서 [피격] 이펙트 " + pEvent->m_ProjectileInfo.m_strAttackEffectName + " 이펙트가 effect.xml에 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_LINK, strError);
	}

	if(pEvent->m_ProjectileInfo.m_strDefenceEffectName.empty() == false &&
		vInfoMgr->m_pEffectInfoMgr->GetEffectInfo(pEvent->m_ProjectileInfo.m_strDefenceEffectName) == NULL)
	{
		string strError = *pStrErrorPre + "[발사체] 이벤트에서 [방어] 이펙트 " + pEvent->m_ProjectileInfo.m_strDefenceEffectName + " 이펙트가 effect.xml에 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_LINK, strError);
	}

	if(pEvent->m_ProjectileInfo.m_strHitGroundEffectName.empty() == false &&
		vInfoMgr->m_pEffectInfoMgr->GetEffectInfo(pEvent->m_ProjectileInfo.m_strHitGroundEffectName) == NULL)
	{
		string strError = *pStrErrorPre + "[발사체] 이벤트에서 [지형피격] 이펙트 " + pEvent->m_ProjectileInfo.m_strHitGroundEffectName + " 이펙트가 effect.xml에 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_LINK, strError);
	}
}

void VTalentDataValidator::OnCheckTalentEvent_Effect( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{
	if(pEvent->m_strParam1.empty())
	{
		string strError = *pStrErrorPre + "[이펙트] 이벤트에서 이펙트 이름이 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_NAME, strError);
	}
	else if(vInfoMgr->m_pEffectInfoMgr->GetEffectFileInfo(pEvent->m_strParam1) == NULL)
	{
		string strError = *pStrErrorPre + "[이펙트] 이벤트에서 " + pEvent->m_strParam1 + ".effect.scene.xml 파일이 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_FILE, strError);
	}
}

void VTalentDataValidator::OnCheckTalentEvent_Sound( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{
	if(pEvent->m_strParam1.empty())
	{
		string strError = *pStrErrorPre + "[사운드] 이벤트에서 사운드 이름이 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_NAME, strError);
	}
	else if(vInfoMgr->m_pSoundInfoMgr->GetSampleInfo(pEvent->m_strParam1) == NULL)
	{
		string strError = *pStrErrorPre + "[사운드] 이벤트에서 " + pEvent->m_strParam1 + " 사운드가 sound.xml에 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_LINK, strError);
	}
}

void VTalentDataValidator::OnCheckTalentEvent_Camera( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{
	if(pEvent->m_strParam1.empty())
	{
		string strError = *pStrErrorPre + "[카메라] 이벤트에서 카메라 이름이 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_NAME, strError);
	}
	else if(vInfoMgr->m_pCameraInfoMgr->GetCameraFileInfo(pEvent->m_strParam1) == NULL)
	{
		string strError = *pStrErrorPre + "[카메라] 이벤트에서 " + pEvent->m_strParam1 + ".elu 파일이 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_FILE, strError);
	}
}

void VTalentDataValidator::OnCheckTalentEvent_Custom( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{
	if(pEvent->m_strCustomEvent.empty())
	{
		string strError = *pStrErrorPre + "[sh_custom] 이벤트에서 이펙트 이름이 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_NAME, strError);
	}
	else if(vInfoMgr->m_pEffectInfoMgr->GetEffectInfo(pEvent->m_strCustomEvent) == NULL)
	{
		string strError = *pStrErrorPre + "[sh_custom] 이벤트에서 " + pEvent->m_strCustomEvent + " 이펙트가 effect.xml에 없습니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_LINK, strError);
	}
}

void VTalentDataValidator::OnCheckTalentEvent_Delayed_Act( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{
	if(pEvent->m_fTime1 <= 0.0f)
	{
		string strError = *pStrErrorPre + "[시간차] 이벤트에서 시간 설정이 0 또는 0이하값입니다.";
		talentEventValidator.m_TalentEventValidatorError.Add(EMT_NAME, strError);
	}
}

void VTalentDataValidator::OnCheckTalentEvent_ExtraActive_1( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{

}

void VTalentDataValidator::OnCheckTalentEvent_ExtraActive_2( VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre )
{

}

void VTalentDataValidator::CheckTalentHitInfo( XTalentInfoMgr* pTalentInfoMgr, XTalentInfo* pTalentInfo, VValidatorInfoMgr* vInfoMgr )
{
	string strCheckTalentError = (string)pTalentInfo->GetName() + "(" + TransToStringFromNumber(pTalentInfo->m_nID) + ") 탤런트 ";

	if( pTalentInfo->m_HitInfo.m_nReferenceID > 0)
	{
		pTalentInfo = pTalentInfoMgr->Get(pTalentInfo->m_HitInfo.m_nReferenceID);
		if(pTalentInfo == NULL)
		{
			string strError = strCheckTalentError + "레퍼런스 (" + TransToStringFromNumber(pTalentInfo->m_HitInfo.m_nReferenceID) + ") ID는 존재하지 않는 탤런트 ID입니다.";
			m_TalentHitInfoValidatorError.Add(EMT_LINK, strError);
			return;
		}
	}	
}