#include "StdAfx.h"
#include "GChallengerQuestInfoValidator.h"
#include "GGlobal.h"
#include "GDef.h"
#include "GValidateLogger.h"

bool GChallengerQuestInfoValidator::Check()
{
	if (!gmgr.pChallengerQuestMgr)
		return true;

	_VLI(FILENAME_CHALLENGER_QUEST);

	size_t nTierSize = gmgr.pChallengerQuestMgr->GetTierSize();
	if (nTierSize <= 0)
	{
		_VLOGGER->Log(L"도전자퀘스트 티어가 존재하지 않습니다.\n");
		return false;
	}
	
	for (size_t i=0; i<nTierSize; ++i)
	{
		const GChallengerQuestMgr::Tier* pTier = gmgr.pChallengerQuestMgr->Get(i);
		CheckTier(i+1, pTier);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GChallengerQuestInfoValidator::CheckTier(int nTier, const GChallengerQuestMgr::Tier* pTier)
{
	VALID(pTier);
	_VLOGGER->ValidateMinusORZero(pTier->nDailyQuestCount, _T(CQ_XML_ATTR_MAX_COUNT));
	_VLOGGER->ValidateMinusORZero(pTier->nMinLevel, _T(CQ_XML_ATTR_MIN_LEVEL));
	_VLOGGER->ValidateMinusORZero(pTier->nMaxLevel, _T(CQ_XML_ATTR_MAX_LEVEL));
	_VLOGGER->ValidateBiggerThan(pTier->nMinLevel, pTier->nMaxLevel, _T(CQ_XML_ATTR_MIN_LEVEL), _T(CQ_XML_ATTR_MAX_LEVEL));
	if (pTier->vecSourceQuests.empty())
	{
		_VLOGGER->Log(L"도전자퀘스트 티어의 퀘스트 항목이 존재하지 않습니다. (tier: %d)\n", nTier);
	}
}