#include "stdafx.h"
#include "GNPCInfo.h"
#include "GTalentInfo.h"
#include "CSStrings.h"
#include "GConst.h"
#include "GFactionInfoMgr.h"
#include "GGlobal.h"
#include "GScriptManager.h"

GNPCInfo::GNPCInfo() : CSNPCInfo()
{
	m_pMeshInfo = NULL;

	nSightRange = 0;
	bRooted = false;
	bCounterAttack = true;	
	bCombatable = true;
	bTauntable = true;
	fStandupTime = 0.0f;	
	fChaseMaxDurationTime = -1.0f;
	bGainEXP = true;
	m_fHateExpiredTime = 0.0f;
	m_nColtLink = INVALID_ID;
	
	nAA = NAAT_NONE;
	nInitAttackable = NAT_ALWAYS,

	m_nLRotationID = 0;
	m_nRRotationID = 0;
	m_fRotationAngle = 90.0f;
	m_fRotationActAngle = 90.0f;

	m_pLootInfo = NULL;

	m_fCriticalMelee = 0.0f;
	m_fCriticalRange = 0.0f;
	m_fCriticalMagic = 0.0f;
	m_fCriticalDmgAmp = 0.0f;

	m_nMinDamage = 0;
	m_nMaxDamage = 0;
	m_fSpellPower = 1.0f;

	m_bObjectThinkable = false;

	m_nFactionGainID = 0;
	m_nFactionGainQuantity = 0;
	m_nFactionLossID = 0;
	m_nFactionLossQuantity = 0;

	m_bAutoAssist = true;
	m_fAutoAssistRange = 500.0f; // 기본값은 5미터

	m_bUseScriptTimer = false;
	m_bEnableAILod = true;
	m_isMajorLodNPC = false;

	m_bAssistAfterCombat = false;

	m_nLuaSpawnCount = 0;
	m_fKeepCorpseTime = -1.0f;
	m_fHateModifierForNPC = 0.5f;
}

GNPCInfo::~GNPCInfo()
{
	Clear();
}

void GNPCInfo::Clear()
{
	for (size_t i = 0; i < vecModes.size(); i++)
	{
		GNPCInfo* pInfo = vecModes[i];
		SAFE_DELETE(pInfo);
	}
	vecModes.clear();

	if (m_pMeshInfo) SAFE_DELETE(m_pMeshInfo);

	CSNPCInfo::Clear();
}

void GNPCInfo::ModeCopyFromParent( GNPCInfo* pParentInfo )
{
	this->nMaxHP = pParentInfo->nMaxHP;
	this->nMaxEN = pParentInfo->nMaxEN;
	this->nMaxSTA = pParentInfo->nMaxSTA;
	this->nAP = pParentInfo->nAP;
}

void GNPCInfo::InitStandupTime()
{
	if (!m_pMeshInfo)	
		return;
	
	TCHAR szAniName[512];
	wsprintf(szAniName, L"%sMF_KD3", strAniPrefix.c_str());
	fStandupTime = m_pMeshInfo->GetAnimationTime(szAniName);
}

void GNPCInfo::InitMaxChaseDurationTime()
{
	if (fChaseMaxDurationTime != -1.0f)	
		return;	// 콜트에서 최대값이 지정됐음

	fChaseMaxDurationTime = GConst::COMBAT_CHASE_MAX_DURATION_TIME;
}

void GNPCInfo::Cooking()
{
	InitStandupTime();
	InitMaxChaseDurationTime();

	// Auto Assist
	{
		// 자동지원 거리가 없으면 기능 끔
		if (m_fAutoAssistRange <= 0.0f)
			m_bAutoAssist = false;
	}


	CheckUsingScriptTimer();
}

bool GNPCInfo::HasTalent( int nTalentID )
{
	return find(m_Talents.begin(), m_Talents.end(), nTalentID) != m_Talents.end();
}

bool GNPCInfo::HasFaction()
{
	return gmgr.pFactionInfoMgr->IsExist(m_nFactionLossID);
}

bool GNPCInfo::HasQuestIE()
{
	return m_InteractionInfo.HasQuestIE();
}

bool GNPCInfo::IsBoss() const
{
	return nGrade >= 5;
}

void GNPCInfo::CheckUsingScriptTimer()
{
	if (gsys.pScriptManager && gsys.pScriptManager->GetLua())
	{
		if (gsys.pScriptManager->GetLua()->IsExistMemberFunc(MLocale::ConvUTF16ToAnsi(strNameInLua.c_str()).c_str(), "OnTimer"))
		{
			this->m_bUseScriptTimer = true;
		}
	}
}