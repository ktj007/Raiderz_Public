#include "stdafx.h"
#include "GGluePlayer.h"
#include "GEntityPlayer.h"
#include "GGlueActor.h"
#include "GNullGlueActor.h"
#include "GPlayerField.h"
#include "GPlayerInteraction.h"
#include "GPlayerBattleArena.h"
#include "GEntityNPC.h"
#include "GGlueNPC.h"
#include "GNullGlueNPC.h"
#include "GScriptUtil.h"
#include "GPlayerQuests.h"
#include "GGlobal.h"
#include "GPlayerSystem.h"
#include "GPlayerCutscene.h"
#include "GGlobal.h"
#include "GTestSystem.h"
#include "GPlayerFieldLazyGateway.h"
#include "GPlayerFactions.h"
#include "GFactionSystem.h"
#include "MLocale.h"
#include "GItemHolder.h"

GGluePlayer::GGluePlayer(GEntityPlayer* pOwner)
: GGlueActor(pOwner)
{
	// do nothing
}

GGluePlayer::~GGluePlayer()
{
	// do nothing
}

GEntityPlayer* GGluePlayer::GetOwnerPlayer() const
{
	return static_cast<GEntityPlayer*>(m_pOwner);
}


bool GGluePlayer::UpdateQuestVar(int nQuestID, int nVar)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerPlayer()->UpdateQuestVar(nQuestID, nVar);
}

int	GGluePlayer::GetQuestVar(int nQuestID)
{
	if (IsInvalidOwner())	return -1;
	return GetOwnerPlayer()->GetQuestVar(nQuestID);
}

bool GGluePlayer::IsCompleteQuest(int nQuestID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerPlayer()->GetQuests().IsCompleteQuest(nQuestID);
}

bool GGluePlayer::AddQuest(int nQuestID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerPlayer()->AddQuest(nQuestID);
}

void GGluePlayer::RuinQuest(int nQuestID)
{
	if (IsInvalidOwner())		return;
	GetOwnerPlayer()->RuinQuest(nQuestID);
}

bool GGluePlayer::BindSoul(uint8 nSoulBindingID)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerPlayer()->BindSoul(nSoulBindingID);
}

int GGluePlayer::GetItemQty(int nItemID)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerPlayer()->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, nItemID);
}

bool GGluePlayer::CheckCondition(int nConditionID)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerPlayer()->CheckCondition(nConditionID);
}
void GGluePlayer::SetFatigue(int nPercent)
{
	if (IsInvalidOwner())	return;
	return GetOwnerPlayer()->SetFatigue(nPercent);
}

void GGluePlayer::Tip(const char* szText)
{
	if (IsInvalidOwner())	return;	
	GetOwnerPlayer()->Tip(MLocale::ConvAnsiToUCS2(szText).c_str());
}

bool GGluePlayer::AddMoney(int nMoney)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerPlayer()->AddMoney(nMoney);
}

bool GGluePlayer::RemoveMoney(int nMoney)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerPlayer()->RemoveMoney(nMoney);
}

int GGluePlayer::GetMoney()
{
	if (IsInvalidOwner())	return false;
	return GetOwnerPlayer()->GetMoney();
}

GGlueNPC* GGluePlayer::GetInteractNPC()
{
	if (IsInvalidOwner())		return GNullGlueNPC::GetNull();

	GEntityNPC* pInteractNPC = GetOwnerPlayer()->GetInteraction().GetInteractingNPC();

	return TO_SAFE_LUAPTR(pInteractNPC);
}

bool GGluePlayer::ArenaRegister()
{
	if (IsInvalidOwner())		return false;
	return GetOwnerPlayer()->GetPlayerBattleArena().RegisterEntry();
}

bool GGluePlayer::Cutscene(int nCutsceneID)
{
	if (IsInvalidOwner())		return false;

	return GetOwnerPlayer()->GetCutscene().BeginCutscene(nCutsceneID);
}

bool GGluePlayer::PartyCutscene(int nCutsceneID)
{
	if (IsInvalidOwner())		return false;

	return GetOwnerPlayer()->GetCutscene().BeginPartyCutscene(nCutsceneID);
}

bool GGluePlayer::Gate(int nFieldID, vec3 vPos)	
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vPos);

	if (IsInvalidOwner())		return false;
	return GetOwnerPlayer()->GetPlayerField().GetLazyGateway().Gate(nFieldID, vPos, GetOwnerPlayer()->GetDir());
}

bool GGluePlayer::GateToMarker(int nFieldID, int nMarkerID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerPlayer()->GetPlayerField().GetLazyGateway().GateToMarker(nFieldID, nMarkerID);
}

bool GGluePlayer::GateToActor(GGlueActor* pActorDelegator)
{
	if (IsInvalidOwner())		return false;

	if (pActorDelegator == NULL || pActorDelegator->IsNull())
	{
		return false;
	}

	GEntityActor* pActor = pActorDelegator->GetOwnerActor();
	return GetOwnerPlayer()->GetPlayerField().GetLazyGateway().GateToActor(pActor);
}

bool GGluePlayer::GateToTrial(int nDynamicFieldGroupID, bool bSIngleEnter)
{
	if (IsInvalidOwner())	return false;

	// 트라이얼필드 입장
	if (GetOwnerPlayer()->GetPlayerField().GetLazyGateway().GateToTrial(nDynamicFieldGroupID, bSIngleEnter))
	{		
		// 싱글퀘스트이면 자신만 이동
		if (bSIngleEnter)	return true;

		// 파티퀘스트이면 주변 파티원들에게 쫒아올지 확인통보
		MUID uidStartField = GetOwnerPlayer()->GetFieldUID();
		GetOwnerPlayer()->RouteFollowMeTrialField(uidStartField, nDynamicFieldGroupID, MAX_PARTY_MEMBER_COUNT);	
		
		return true;
	}

	return false;
}

bool GGluePlayer::GateToInnRoom(void)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerPlayer()->GetPlayerField().GetLazyGateway().GateToInnRoom();
}

void GGluePlayer::AddAmity(int nFactionID, int nQuantity)
{
	if (IsInvalidOwner())	return;

	gsys.pFactionSystem->Increase(GetOwnerPlayer(), nFactionID, nQuantity);
}

void GGluePlayer::RemoveAmity(int nFactionID, int nQuantity)
{
	if (IsInvalidOwner())	return;

	gsys.pFactionSystem->Decrease(GetOwnerPlayer(), nFactionID, nQuantity);
}

void GGluePlayer::SetAmity(int nFactionID, int nQuantity)
{
	if (IsInvalidOwner())	return;

	gsys.pFactionSystem->Set(GetOwnerPlayer(), nFactionID, nQuantity);
}
