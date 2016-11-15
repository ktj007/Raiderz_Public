#include "StdAfx.h"
#include "GPlayerTalent.h"
#include "GTalentInfo.h"
#include "GEntityPlayer.h"
#include "GTalentInfoMgr.h"
#include "GGlobal.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"

GPlayerTalent::GPlayerTalent(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_nRemainTP(0)
{

}

void GPlayerTalent::Insert(int nTalentID, bool bConsumeTP)
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (NULL == pTalentInfo)
	{
#ifdef _DEBUG
		mlog("Invalid TalentID : %d\n", nTalentID);
#endif
		return;
	}

	Insert(pTalentInfo, bConsumeTP);
}

void GPlayerTalent::Insert(GTalentInfo* pTalentInfo, bool bConsumeTP)
{
	if (NULL == pTalentInfo) return;
	if (true == IsTrainedTalent(pTalentInfo)) return;

	if (!m_pOwner->HasPassiveTalent(pTalentInfo->m_nExtraPassive))
	{
		m_pOwner->GetItemHolder()->GetEquipment().AddModifierByTrain(pTalentInfo->m_nExtraPassive);		
	}

	if (!m_pOwner->HasPassiveTalent(pTalentInfo->m_nExtraPassive2))
	{
		m_pOwner->GetItemHolder()->GetEquipment().AddModifierByTrain(pTalentInfo->m_nExtraPassive2);
	}

	m_setTalentID.insert(pTalentInfo->m_nID);
	
	GModEffector ModEffector;
	ModEffector.AddActorModifier(m_pOwner->GetChrStatus()->ActorModifier, pTalentInfo->m_ActorModifier);
	ModEffector.AddPlayerModifier(m_pOwner->GetPlayerModifier(), pTalentInfo->m_PlayerModifier);
	ModEffector.RefreshStatus(m_pOwner);

	if (ST_PASSIVE == pTalentInfo->m_nSkillType || ST_EXTRA_PASSIVE == pTalentInfo->m_nSkillType)
	{
		m_pOwner->GainPassiveTalentEffect(pTalentInfo);			
	}

	if (true == bConsumeTP)
	{
		m_nRemainTP -= CONSUME_TP_FOR_LEARN_TALENT;
	}

	/// 채집탤런트 학습 등록
	if (TT_GATHER == pTalentInfo->m_nTalentType)
	{
		m_GatherTalentRank.Insert(pTalentInfo->m_nRank);
	}

	m_pOwner->TrimStats();
}

void GPlayerTalent::DeleteAll()
{
	SET_TALENTID tempSetTalentID = m_setTalentID;

	for each (int nTalentID in tempSetTalentID)
	{
		Delete(nTalentID);
	}

	SetTP(m_pOwner->GetLevel());

	m_pOwner->GetItemHolder()->GetEquipment().EraseModifierByUntrainAll();
}

void GPlayerTalent::Delete(int nTalentID)
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (NULL == pTalentInfo) return;

	if (false == pTalentInfo->m_bUntrainable) return;

	m_setTalentID.erase(nTalentID);

	GModEffector ModEffector;
	ModEffector.EraseActorModifier(m_pOwner->GetChrStatus()->ActorModifier, pTalentInfo->m_ActorModifier);
	ModEffector.ErasePlayerModifier(m_pOwner->GetPlayerModifier(), pTalentInfo->m_PlayerModifier);
	ModEffector.RefreshStatus(m_pOwner);


	if (ST_EXTRA_PASSIVE == pTalentInfo->m_nSkillType || ST_PASSIVE == pTalentInfo->m_nSkillType)
	{
		m_pOwner->LostPassiveTalentEffect(pTalentInfo);
	}

	/// 채집탤런트 학습 등록해제
	if (TT_GATHER == pTalentInfo->m_nTalentType)
	{
		m_GatherTalentRank.Delete(pTalentInfo->m_nRank);
	}
}

void GPlayerTalent::InsertWithLowRankTalent(int nTalentID, bool bUseTP)
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (NULL == pTalentInfo) return;

	Insert(pTalentInfo, bUseTP);

	for (int i = pTalentInfo->m_nRank; i > 0; --i)
	{
		GTalentInfo* pLowRankTalentInfo = gmgr.pTalentInfoMgr->GetByLineAndRank(pTalentInfo->m_nTalentLine, i);
		if (NULL == pLowRankTalentInfo) continue;

		Insert(pLowRankTalentInfo, bUseTP);
	}
}

int GPlayerTalent::GetTrainedStyleTP(TALENT_STYLE nStyle)
{
	int nTPCount = 0;

	for each (int nTalentID in m_setTalentID)
	{
		GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
		if (NULL == pTalentInfo) continue;
		if (nStyle != pTalentInfo->m_nStyle) continue;
		if (true == pTalentInfo->m_bByLicense) continue;

		nTPCount++;
	}

	return nTPCount;
}

bool GPlayerTalent::IsTrainedTalent(int nTalentID)
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (NULL == pTalentInfo) return false;

	return IsTrainedTalent(pTalentInfo);
}

bool GPlayerTalent::IsTrainedTalent(GTalentInfo* pTalentInfo)
{
	if (NULL == pTalentInfo) return false;

	if (false == pTalentInfo->m_bNeedTraining) return true;
	if (m_setTalentID.end() != m_setTalentID.find(pTalentInfo->m_nID)) return true;

	return false;
}

bool GPlayerTalent::IsTrainedGatherTalent(int nRank)
{
	return m_GatherTalentRank.HasGatherTalent(nRank);
}

bool GPlayerTalent::IsTrainedTalentLine(int nTalentLine)
{
	for each (int nTalentID in m_setTalentID)
	{
		GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
		if (NULL == pTalentInfo) continue;

		if (nTalentLine == pTalentInfo->m_nTalentLine) return true;
	}

	return false;
}

bool GPlayerTalent::IsEmpty()
{
	return m_setTalentID.empty();
}

void GPlayerTalent::EnableUseItemTalent(MUID nUsedTalentItemUID)
{	
	m_nUsedTalentItemUID = nUsedTalentItemUID;
}

void GPlayerTalent::DisableUseItemTalent()
{
	m_nUsedTalentItemUID = MUID::ZERO;
}

bool GPlayerTalent::IsEnableUseItemTalent(int nTalentID)
{
	GItemHolder* pItemHolder = m_pOwner->GetItemHolder();
	GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, m_nUsedTalentItemUID);
	if (NULL == pItem) return false;

	if (USABLE_TALENT_USE != pItem->m_pItemData->m_nUsableType) return false;
	if (nTalentID != pItem->m_pItemData->m_vecUsableParam.front()) return false;

	return true;
}

MUID GPlayerTalent::GetUsedTalentItemUID()
{
	return m_nUsedTalentItemUID;
}

int GPlayerTalent::GetTotalTP()
{
	int nConsumeTP = 0;

	for each(int nTalentID in m_setTalentID)
	{
		if (IsTPConsumeTalent(nTalentID))
		{
			nConsumeTP++;
		}
	}

	return nConsumeTP + m_nRemainTP;
}

bool GPlayerTalent::IsTPConsumeTalent(int nTalentID)
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (pTalentInfo == NULL) return false;

	if (pTalentInfo->m_nStyle == TS_LICENSE)
		return false;
	if (gmgr.pTalentInfoMgr->IsFocusTalent(nTalentID))
		return false;

	return true;	
}

//////////////////////////////////////////////////////////////////////////

void GPlayerTalent::GGatherTalentRank::Insert(int nRank)
{
	if (nRank <= 0 || GATHER_TIER_COUNT < nRank) return;
	
	m_setGatherTalentRank.insert(nRank);
}

void GPlayerTalent::GGatherTalentRank::Delete(int nRank)
{
	if (nRank <= 0 || GATHER_TIER_COUNT < nRank) return;
	
	m_setGatherTalentRank.erase(nRank);
}

bool GPlayerTalent::GGatherTalentRank::HasGatherTalent(int nRank)
{
	if (nRank <= 0 || GATHER_TIER_COUNT < nRank) return false;
	if (m_setGatherTalentRank.empty()) return false;
	
	// 기본 오름차순 정렬되는 set을 이용; 마지막 요소가 가장 큰 Rank가 된다.
	set<int>::reverse_iterator iter = m_setGatherTalentRank.rbegin();
	int nLearnedMaxRank = *iter;

	if (nLearnedMaxRank >= nRank)
		return true;

	return false;
}
