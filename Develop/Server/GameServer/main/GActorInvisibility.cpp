#include "StdAfx.h"
#include "GActorInvisibility.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GDef.h"
#include "GField.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GCommandCenter.h"
#include "Gconst.h"
#include "GCalculator.h"
#include "GPlayerObjectManager.h"
#include "GPartySystem.h"
#include "GParty.h"
#include "GItemHolder.h"
#include "GBuffInfo.h"
#include "GEquipmentSlot.h"

GActorInvisibility::GActorInvisibility(GEntityActor* pOwner)
: m_pOwner(pOwner)
, m_pDice(NULL)
, m_fInvisibilityPower(0)
{
	m_pOwner->AttachObserver(this);

	// 효과주기시간 설정
	m_rgrDetection.SetTickTime(GConst::MAGICAREA_EFFECTION_PERIOD);
	m_rgrDetection.Start();

	// 기본 주사위 설정
	SetDice(new GPercentDice());

}

GActorInvisibility::~GActorInvisibility(void)
{
	m_pOwner->DetachObserver(this);

	SAFE_DELETE(m_pDice);
}

void GActorInvisibility::OnShow()
{
	VALID(m_pOwner);
	GField* pField = m_pOwner->GetField();
	if (!pField)
		return;

	OnDetected(m_vecBlinders);

	m_vecBlinders.clear();
	m_vecDetectors.clear();
}

void GActorInvisibility::OnHide()
{
	VALID(m_pOwner);
	GField* pField = m_pOwner->GetField();
	if (!pField)
		return;

	m_pOwner->doMakePeace();

	m_vecBlinders.clear();
	m_vecDetectors.clear();

	vector<GEntityPlayer*> vecPlayers = GActorInvisibility::CalcSectorPlayers();

	for each (GEntityPlayer* pPlayer in vecPlayers)
	{
		m_vecBlinders.push_back(pPlayer->GetUID());
	}

	OnBlinded(m_vecBlinders);
}

bool GActorInvisibility::IsInSight(GEntityActor* pTarget)
{
	VALID_RET(pTarget, false);

	if (m_pOwner->IsPlayer())
	{
		GEntityPlayer* pOwnerPlayer = ToEntityPlayer(m_pOwner);

		MUID uidParty = pOwnerPlayer->GetPartyUID();
		GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		
		if (pParty != NULL)
		{
			for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
			{
				MUID uidMember = it->first;
				GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
				if (pMember == NULL)	continue;

				if (uidMember == pTarget->GetUID())					
				{
					// 파티원은 무조건 보임
					return true;
				}
			}
		}
	}

	return pTarget->GetPos().DistanceTo(m_pOwner->GetPos()) <= CalcDetectDistance(pTarget);
}

void GActorInvisibility::OnUpdate( float fDelta )
{
	UpdateDetection(fDelta);
}

void GActorInvisibility::UpdateDetection( float fDelta )
{
	if (m_pOwner->IsPlayer() &&
		ToEntityPlayer(m_pOwner)->GetPlayerInfo()->m_bGhost)
		return;// 고스트 모드이면 검사하지 않음

	if (!m_pOwner->IsNowInvisibility())
		return;	// 투명화가 아니면 무시

	if (!m_rgrDetection.IsReady(fDelta))
		return;

	OnTryDetect();
}

void GActorInvisibility::OnTryDetect()
{
	VALID(m_pOwner);
	GField* pField = m_pOwner->GetField();
	if (!pField)
		return; // 필드에 들어와있지 않음

	if (m_pOwner->IsDead())
		return; // 죽어있음

	vector<GEntityActor*> vecCalcBlinders;
	vector<GEntityActor*> vecCalcDetectors;
	CalcNewBlinders(vecCalcBlinders, vecCalcDetectors);

	// 효과 범위 찾기
	GVectorMUID	vecNewDetector;
	GVectorMUID vecNewBlinders;
	GVectorMUID vecOldDetector;

	vecOldDetector.Vector() = m_vecDetectors;
	m_vecDetectors.clear();

	// 새롭게 보여진다고 알릴 대상 찾기
	for each (GEntityActor* pNewDetector in vecCalcDetectors)
	{
		bool bNewDetector = true;

		for each (const MUID& uidOldDetector in vecOldDetector.Vector())
		{
			if (pNewDetector->GetUID() == uidOldDetector)
				bNewDetector = false;
		}

		if (bNewDetector)
		{
			if (CheckDetectChance(pNewDetector))
			{
				// 탐지확률 성공
				vecNewDetector.Vector().push_back(pNewDetector->GetUID());
				m_vecDetectors.push_back(pNewDetector->GetUID());
			}
			else
			{
				vecCalcBlinders.push_back(pNewDetector);
			}
		}
		else
		{
			m_vecDetectors.push_back(pNewDetector->GetUID());
		}
	}

	// 새롭게 숨겨진다고 알릴 대상 찾기
	for each (GEntityActor* pNewBlinder in vecCalcBlinders)
	{	
		bool bNewBlinder = true;
		for each (const MUID& uidOldBlinder in m_vecBlinders)
		{
			if (pNewBlinder->IsDead())
				continue;

			if (uidOldBlinder == pNewBlinder->GetUID())
			{
				bNewBlinder = false;
				break;
			}
		}

		if (bNewBlinder)
			vecNewBlinders.Vector().push_back(pNewBlinder->GetUID());
	}
	

	// 안보이던 액터들 추가
		m_vecBlinders.clear();
	for each (GEntityActor* pNewEffectee in vecCalcBlinders)
	{	
		m_vecBlinders.push_back(pNewEffectee->GetUID());
	}
	
	OnDetected(vecNewDetector.Vector());
	OnBlinded(vecNewBlinders.Vector());
}

void GActorInvisibility::OnDetected( const vector<MUID>& vecEffectees )
{
	if (vecEffectees.empty())
		return;

	m_pOwner->RouteInEntity(vecEffectees);
}

void GActorInvisibility::OnBlinded( const vector<MUID>& vecEffectees )
{
	if (vecEffectees.empty())
		return;

	VALID(m_pOwner);
	GField* pField = m_pOwner->GetField();
	VALID(pField);

	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_HIDE, 1, 
		NEW_UID(m_pOwner->GetUID()));

	for each (const MUID& each in vecEffectees)
	{
		pNewCmd->AddReceiver(each);
	}

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

vector<GEntityPlayer*> GActorInvisibility::CalcSectorPlayers()
{
	vector<GEntityPlayer*> vecPlayers;

	VALID_RET(m_pOwner, vecPlayers);
	GField* pField = m_pOwner->GetField();
	VALID_RET(pField, vecPlayers);

	GFieldGrid::EntitySelector* grid_selector = 
		pField->GetGrid().GetEntitySelector();

	GVectorMUID vecNeighborPlayers;
	grid_selector->GetPlayers(vecNeighborPlayers.Vector(), m_pOwner->GetPos());
	for each (const MUID& uid in vecNeighborPlayers.Vector())
	{
		if (m_pOwner->GetUID() == uid)
			continue;

		GEntityPlayer* pPlayer = pField->FindPlayer(uid);
		if (!pPlayer)
			continue;

		vecPlayers.push_back(pPlayer);
	}

	return vecPlayers;
}

void GActorInvisibility::CalcNewBlinders(vector<GEntityActor*>& vecBlinders, vector<GEntityActor*>& vecDetecteors)
{
	vector<GEntityPlayer*> vecPlayers = CalcSectorPlayers();

	vecDetecteors.clear();

	for each (GEntityPlayer* pPlayer in vecPlayers)
	{
		if (IsInSight(pPlayer))
			vecDetecteors.push_back(pPlayer);
		else
			vecBlinders.push_back(pPlayer);
	}
}

void GActorInvisibility::SetDice( GPercentDice* pDice )
{
	SAFE_DELETE(m_pDice);
	m_pDice = pDice;
}

float GActorInvisibility::CalcDetectChancePercent(GEntityActor* pActor)
{
	VALID_RET(m_pOwner, 0.0f);
	VALID_RET(pActor, 0.0f);

	// 스닉 명중굴림 = MAX(MIN(스닉 기본 성능 + 거리에 의한 보정치 + 갑옷에 의한 보정치 + 레벨보정치, 100), 0)
	float fTotalPercent =	m_fInvisibilityPower + 
							CalcDetectDistancePercent(pActor) + 
							CalcDetectArmorPercent() + 
							CalcDetectLevelPercent(pActor);

	// 은신 숙련도
	if (m_pOwner->IsPlayer())
	{
		fTotalPercent -= (float)ToEntityPlayer(m_pOwner)->GetPassiveValue(TEPT_BASE_SNEAK);
	}

	mmlog("%s - percent: %.1f\n", __FUNCTION__, fTotalPercent);
	fTotalPercent = max(fTotalPercent, 0.0f);
	fTotalPercent = min(fTotalPercent, 100.0f);
	return fTotalPercent;
}

float GActorInvisibility::CalcDetectDistancePercent(GEntityActor* pActor)
{
	// 거리에 의한 보정치 = (MAX(INT(거리/200), 6) - 2) * 2.5
	float fDistance = m_pOwner->GetPos().DistanceTo(pActor->GetPos());
	return (min(MMath::TruncateToInt(fDistance/200),6)-2)*2.5f;
}

int GActorInvisibility::CalcDetectLevelPercent(GEntityActor* pActor)
{
	// 레벨 보정치 = INT((스닉시전자에 대한 대상의 레벨팩터 - 5)^2 * (스닉시전자에 대한 대상의 레벨팩터 - 5) * -0.08)
	int nOwnerLevel = m_pOwner->GetLevel();
	int nTargetLevel = pActor->GetLevel();
	int nLF = GCalculator::CalcLevelFactor(nOwnerLevel, nTargetLevel);
	return MMath::TruncateToInt(MMath::Pow((float)(nLF-5),2) * (nLF-5) * -0.08f);
}

float GActorInvisibility::CalcDetectArmorPercent()
{
	// 갑옷종류	 보정치
	// Cloth	 0
	// Light	 0
	// Medium	 15
	// Heavy	 30
	if (!m_pOwner->IsPlayer())
		return 0.0f;
	
	GItemHolder* pItemHolder = ToEntityPlayer(m_pOwner)->GetItemHolder();
	switch(pItemHolder->GetEquipment().GetArmorType())
	{
	case ARMOR_MEDIUM:	return 15.0f;
	case ARMOR_HEAVY:	return 30.0f;
	}
	return 0.0f;
}

int GActorInvisibility::CalcDetectDistance(GEntityActor* pActor)
{
	return GConst::INVISIBILITY_DETECT_DISTANCE;
}

void GActorInvisibility::OnGainBuff( int nBuffID )
{
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (!pBuffInfo)
		return;

	if (pBuffInfo->IsInvisibility())
	{
		m_fInvisibilityPower = pBuffInfo->GetInvisibilityPower();
		OnHide();
	}
}

void GActorInvisibility::OnLostBuff( int nBuffID )
{
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (!pBuffInfo)
		return;

	if (pBuffInfo->IsInvisibility())
	{
		OnShow();
	}
}

bool GActorInvisibility::CheckDetectChance( GEntityActor* pActor )
{
	return m_pDice->Dice(CalcDetectChancePercent(pActor));
}

