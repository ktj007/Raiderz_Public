#include "StdAfx.h"
#include "GBuffInfoValidator.h"
#include "GValidateLogger.h"
#include "GServerSystems.h"
#include "GBuffInfo.h"
#include "GTalentInfoMgr.h"
#include "GDef.h"
#include "CSBuffInfoDef.h"

bool GBuffInfoValidator::Check()
{
	if (!gmgr.pBuffInfoMgr)	
		return true;

	_VLI(FILENAME_BUFFINFO);
	for each (pair<int,CSBuffInfo*> pair in *gmgr.pBuffInfoMgr)
	{
		GBuffInfo* pBuffInfo = static_cast<GBuffInfo*>(pair.second);
		if (!pBuffInfo)
			continue;

		_VLE(_T(BUFF_XML_TAG_BUFF));
		_VLA(_T(TALENT_XML_ATTR_ID), pBuffInfo->m_nID);
		_VLP;

		CheckID(pBuffInfo);
		CheckEnchant(pBuffInfo);
		CheckTiming(pBuffInfo);
		CheckUseTalent(pBuffInfo);
		CheckUpkeep(pBuffInfo);
		CheckRequirement(pBuffInfo);
		CheckTrigger(pBuffInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GBuffInfoValidator::CheckUseTalent( GBuffInfo* pBuff )
{
	if (!pBuff->HasUseTalent())
		return;

	if (!gmgr.pTalentInfoMgr->Find(pBuff->m_nTalentID))
	{
		_VLOGGER->Log(PERCENT_D IS_INVALID_TYPE, pBuff->m_nTalentID);
	}
}

void GBuffInfoValidator::CheckID(GBuffInfo* pBuff)
{
	_VLOGGER->ValidateID(pBuff->m_nID);

	if (pBuff->m_nID <= 1000)
	{
		if (pBuff->m_nType != BUFT_DEBUFF)
		{
			_VLOGGER->Log(L"1000번 이하 ID의 %s은 DEBUF만 허용됩니다.\n", _T(BUFF_XML_ATTR_TYPE));
		}

		if (!pBuff->m_bCancelable)
		{
			_VLOGGER->Log(L"1000번 이하 ID의 %s은 true만 허용됩니다.\n", _T(BUFF_XML_ATTR_CANCELABLE));
		}
	}
}

void GBuffInfoValidator::CheckEnchant(GBuffInfo* pBuff)
{
	_VLOGGER->ValidateID(pBuff->m_nID);

	if (!pBuff->IsEnchant())
		return;


}

void GBuffInfoValidator::CheckUpkeep(GBuffInfo* pBuff)
{
	if (pBuff->m_fDuration != BUFF_DURATION_INFINITY)
	{
		_VLOGGER->ValidateMinus(pBuff->m_fDuration, _T(BUFF_XML_ATTR_DURATION));
	}
	_VLOGGER->ValidateMinus(pBuff->m_fPeriod, _T(BUFF_XML_ATTR_PERIOD));
}

void GBuffInfoValidator::CheckRequirement(GBuffInfo* pBuff)
{
	for each(int nBuffID in pBuff->m_RequireBuff.vecIncludeBuffID)
	{
		_VLOGGER->ValidateBuffID(nBuffID, _T(BUFF_XML_ATTR_INCLUDE));
	}

	for each(int nBuffID in pBuff->m_RequireBuff.vecExcludeBuffID)
	{
		_VLOGGER->ValidateBuffID(nBuffID, _T(BUFF_XML_ATTR_EXCLUDE));
	}
}

void GBuffInfoValidator::CheckTiming(GBuffInfo* pBuff)
{
	if (pBuff->m_EffectInfo.m_nPoint == CSEffectInfo::POINT_SELF &&
		pBuff->m_EffectInfo.m_nRelation == CSEffectInfo::RELATION_ENEMY &&
		pBuff->m_EffectInfo.m_fRadius == 0.0f)
	{
		_VLOGGER->Log(L"EffectPoint가 self고 EffectTarget이 enemy면서 EffectRange가 0이면 타겟을 지정할 수 없습니다.\n");
	}
}

void GBuffInfoValidator::CheckTrigger(GBuffInfo* pBuff)
{
	if (pBuff->m_TriggeredBuff.nBuffID != INVALID_BUFF_ID)
	{
		_VLOGGER->ValidateBuffID(pBuff->m_TriggeredBuff.nBuffID, _T(BUFF_XML_ATTR_TRIGGEREDBUFF_ID));
		_VLOGGER->ValidateMinus(pBuff->m_TriggeredBuff.infoEffect.m_fRadius, _T("TriggeredBuffEffectRange"));
	}

	if (pBuff->m_TriggeredBuff.infoEffect.m_nPoint == CSEffectInfo::POINT_SELF &&
		pBuff->m_TriggeredBuff.infoEffect.m_nRelation == CSEffectInfo::RELATION_ENEMY &&
		pBuff->m_TriggeredBuff.infoEffect.m_fRadius == 0.0f)
	{
		_VLOGGER->Log(L"TriggeredEffectPoint가 self고 TriggeredEffectTarget이 enemy면서 TriggeredEffectRange가 0이면 타겟을 지정할 수 없습니다.\n");
	}
}

void GBuffInfoValidator::CheckReleaseCondition(GBuffInfo* pBuff)
{
	switch(pBuff->m_Condition.infoRelease.nType)
	{
	case TC_USE_TALENT:
		_VLOGGER->ValidateMinMax(_T(BUFF_XML_ATTR_RELEASE_PARAM), pBuff->m_Condition.infoRelease.nParam, 0, 1);
		break;
	case TC_DAMAGE:
		_VLOGGER->ValidateMinus(pBuff->m_Condition.infoRelease.nParam, _T(BUFF_XML_ATTR_RELEASE_PARAM));
		break;
	}
}
