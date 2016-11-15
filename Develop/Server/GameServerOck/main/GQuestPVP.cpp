#include "StdAfx.h"
#include "GQuestPVP.h"
#include "GQPTeamCountSender.h"
#include "GGlueQuestPVP.h"
#include "GQPEventInfo.h"
#include "GGlobal.h"
#include "GQPEventInfoMgr.h"
#include "GField.h"
#include "GMsgCommandFacade.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GPlayerPVPArea.h"
#include "CSDef_QuestPVP.h"
#include "GMoneySystem.h"
#include "GExpSystem.h"
#include "GItemAdder.h"
#include "GEntityNPC.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBTaskDataQuestPVP.h"
#include "GItemHolder.h"
#include "GItemManager.h"
#include "GNullGlueQuestPVP.h"
#include "GDBManager.h"
#include "GExpCalculator.h"

GQuestPVP::GQuestPVP(GField* pField)
: m_pField(pField), m_pQPTeamCountSender(NULL), m_pDelegator(NULL), m_nDoingEventID(0)
{
	CreateDelegator();

	m_pQPTeamCountSender = new GQPTeamCountSender(pField);
}

GQuestPVP::~GQuestPVP(void)
{
	SAFE_DELETE(m_pDelegator);
	SAFE_DELETE(m_pQPTeamCountSender);
	Reset();
}

void GQuestPVP::Update( float fDelta )
{
	m_pQPTeamCountSender->Update(fDelta);
}

void GQuestPVP::BeginEvent(int nEventID)
{
	GQPEventInfo* pInfo = gmgr.pQPEventInfoMgr->Get(nEventID);
	VALID(pInfo);
	const GQPEBeginInfo& beginInfo = pInfo->m_BeginInfo;

	for each (const wstring& strNotice in beginInfo.m_vecStrNotice)
	{
		gsys.pMsgCommandFacade->RouteNoticeMsgToField(m_pField, strNotice.c_str());
	}

	for each (GQPESpawnInfo* pSpawnInfo in beginInfo.m_vecSpawnInfo)
	{
		m_pField->Spawn(pSpawnInfo->m_nSpawnID, pSpawnInfo->m_fDelayTime, pSpawnInfo->m_fDespawnTime);		
	}	

	m_nDoingEventID = nEventID;

	MCommand* pNewCmd = MakeNewCommand(MC_QUESTPVP_BEGIN_EVENT,
		1,
		NEW_INT(nEventID));

	GFieldGridRouter router;
	router.RouteToField(m_pField, pNewCmd);
}

void GQuestPVP::EndEvent(int nEventID, QUESTPVP_TEAM nWinnerTeam)
{
	GQPEventInfo* pInfo = gmgr.pQPEventInfoMgr->Get(nEventID);
	VALID(pInfo);
	GQPEEndInfo* pEndInfo = pInfo->GetEndInfo(nWinnerTeam);
	VALID(pEndInfo);

	for each (const wstring& strNotice in pEndInfo->m_vecStrNotice)
	{
		gsys.pMsgCommandFacade->RouteNoticeMsgToField(m_pField, strNotice.c_str());
	}	

	for each (GQPESpawnInfo* pSpawnInfo in pEndInfo->m_vecSpawnInfo)
	{
		m_pField->Spawn(pSpawnInfo->m_nSpawnID, pSpawnInfo->m_fDelayTime, pSpawnInfo->m_fDespawnTime);		
	}

	for each (int nDespawnNPCID in pEndInfo->m_vecDespawnNPCID)
	{
		m_pField->DespawnNow(nDespawnNPCID, false);
	}
	
	m_nDoingEventID = 0;

	MCommand* pNewCmd = MakeNewCommand(MC_QUESTPVP_END_EVENT,
		2,
		NEW_INT(nEventID),
		NEW_INT(nWinnerTeam));

	GFieldGridRouter router;
	router.RouteToField(m_pField, pNewCmd);

	RewardEvent(pEndInfo);	
}

void GQuestPVP::RewardEvent(GQPEEndInfo* pEndInfo)
{
	VALID(pEndInfo);

	vector<GEntityPlayer*> vecWinnerPlayer;
	CollectTeam(pEndInfo->m_nWinnerTeam, vecWinnerPlayer);
	for each (GEntityPlayer* pWinnerPlayer in vecWinnerPlayer)
	{
		GDBT_QPER_TOINVEN dataToInven;
		GDBT_QPER_TOMAIL dataToMail;
		MakeEventRewardDBT(pWinnerPlayer, pEndInfo, dataToInven, dataToMail);

		if (dataToInven.IsValid())
		{
			 gsys.pDBManager->QuestPVPERewardInven(dataToInven);
		}
		else if (dataToMail.IsValid())
		{			
			gsys.pDBManager->QuestPVPERewardMail(dataToMail);
		}
	}
}

void GQuestPVP::MakeEventRewardDBT(GEntityPlayer* pPlayer, GQPEEndInfo* pEndInfo, GDBT_QPER_TOINVEN& dataToInven, GDBT_QPER_TOMAIL& dataToMail)
{
	VALID(pPlayer);
	VALID(pEndInfo);
	

	int nRewardItemCount = min(pEndInfo->m_vecItemInfo.size(), QPER_ITEM_COUNT);

	vector<CHECK_ADDABLE_ITEM> vecCheckAddbaleItem;
	for (int i=0; i<nRewardItemCount; i++)
	{
		GQPEItemInfo* pItemInfo = pEndInfo->m_vecItemInfo[i];
		vecCheckAddbaleItem.push_back(CHECK_ADDABLE_ITEM(pItemInfo->m_nItemID, pItemInfo->m_nAmount));		
	}

	if (true == gsys.pItemSystem->GetAdder().CheckItemAddable(pPlayer, vecCheckAddbaleItem))
	{
		vector<GDBT_QPER_ITEM> vecItem;
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();

		for (int i=0; i<nRewardItemCount; i++)
		{
			GQPEItemInfo* pItemInfo = pEndInfo->m_vecItemInfo[i];

			GDBT_QPER_ITEM dataItem;
			GItem* pItem = pItemHolder->GetMergeableItem(SLOTTYPE_INVENTORY, pItemInfo->m_nItemID, pItemInfo->m_nAmount);
			if (NULL == pItem)
			{
				dataItem = GDBT_QPER_ITEM(0, pItemInfo->m_nItemID, pItemHolder->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0), pItemInfo->m_nAmount, pItemInfo->m_nAmount, pItemInfo->m_nItemID);
			}
			else
			{
				dataItem = GDBT_QPER_ITEM(pItem->m_nIUID, pItemInfo->m_nItemID, pItem->m_nSlotID, pItem->GetAmount() + pItemInfo->m_nAmount, pItemInfo->m_nAmount, pItemInfo->m_nItemID);
			}

			vecItem.push_back(dataItem);	
		}

		int nNewLevel = GExpCalculator::CalcNewLevel(pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP+pEndInfo->m_nXP);
		int nNewExp = GExpCalculator::CalcNewExp(pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP+pEndInfo->m_nXP);

		dataToInven = GDBT_QPER_TOINVEN(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
			, nNewExp, pEndInfo->m_nXP, nNewLevel, nNewLevel - pPlayer->GetLevel()
			, pPlayer->GetMoney()+pEndInfo->m_nMoney, pEndInfo->m_nMoney, vecItem
			, m_nDoingEventID, m_pField->GetID(), pPlayer->GetPos());
	}
	else
	{		
		vector<GDBT_QPER_ITEM> vecItem;
		for (int i=0; i<nRewardItemCount; i++)
		{
			GQPEItemInfo* pItemInfo = pEndInfo->m_vecItemInfo[i];

			GDBT_QPER_ITEM dataItem(0, pItemInfo->m_nItemID, i, pItemInfo->m_nAmount + pItemInfo->m_nAmount, pItemInfo->m_nAmount, pItemInfo->m_nItemID);	
			vecItem.push_back(dataItem);
		}

		wstring strDefaultItemName = L"";
		int nDefaultItemID = 0;
		if (false == vecItem.empty())
		{
			GItemData* pItemData = gmgr.pItemManager->GetItemData(pEndInfo->m_vecItemInfo.front()->m_nItemID);
			if (NULL != pItemData)
			{
				strDefaultItemName = pItemData->m_strName;
				nDefaultItemID = pItemData->m_nID;
			}			
		}

		int nNewLevel = GExpCalculator::CalcNewLevel(pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP+pEndInfo->m_nXP);
		int nNewExp = GExpCalculator::CalcNewExp(pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP+pEndInfo->m_nXP);

		dataToMail = GDBT_QPER_TOMAIL(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
			, nNewExp, pEndInfo->m_nXP, nNewLevel, nNewLevel - pPlayer->GetLevel()
			, pPlayer->GetMoney()+pEndInfo->m_nMoney, pEndInfo->m_nMoney
			, L""
			, strDefaultItemName, MAX_MAIL_DELETE_SECOND, nDefaultItemID, vecItem
			, m_nDoingEventID, m_pField->GetID(), pPlayer->GetPos());
	}	
}


void GQuestPVP::CollectTeam(QUESTPVP_TEAM nTeam,vector<GEntityPlayer*>& players)
{
	const GFieldInfo* const pInfo = m_pField->GetInfo();
	if (NULL == pInfo) return;
	GFieldGrid::EntitySelector* const entity_selector = m_pField->GetGrid().GetEntitySelector();
	if (NULL == entity_selector) return;
	if (true == pInfo->m_PvPAreas.empty()) return;

	set<MUID> setPlayerUID;
	for each (const PVP_AREA_INFO& pvpAreaInfo in pInfo->m_PvPAreas)
	{
		class CollectPlayers : public GFieldGrid::Sector::IVisitor
		{
		public:
			CollectPlayers(set<MUID>& players) : m_players(players) {}
		private:
			void OnVisit(const MUID& uid) {
				m_players.insert(uid);
			}
		private:
			set<MUID>& m_players;
		};
		entity_selector->VisitIntersectWithRect(CollectPlayers(setPlayerUID), ETID_PLAYER, pvpAreaInfo.rcArea);
	}

	for each (const MUID& nPlayerUID in setPlayerUID)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nPlayerUID);
		if (NULL == pPlayer) continue;
		if (nTeam != pPlayer->GetPlayerPVPArea().GetTeam()) continue;

		players.push_back(pPlayer);
	}
}

int GQuestPVP::GetTeamCount( QUESTPVP_TEAM nTeam )
{
	vector<GEntityPlayer*> vecPlayer;
	CollectTeam(nTeam, vecPlayer);

	return vecPlayer.size();
}

void GQuestPVP::CreateDelegator()
{
	m_pDelegator = new GGlueQuestPVP(this);
}

GGlueQuestPVP* GQuestPVP::GetDelegator( void )
{
	VALID_RET(m_pDelegator, NULL);
	return m_pDelegator;
}

bool GQuestPVP::IsDoingEvent()
{
	if (0 == m_nDoingEventID) return false;

	return true;
}

void GQuestPVP::RouteToQuestPVPTeam(MCommand* pNewCmd)
{
	vector<GEntityPlayer*> vecTeamOnePlayer;
	CollectTeam(QT_ONE, vecTeamOnePlayer);
	vector<GEntityPlayer*> vecTeamTwoPlayer;
	CollectTeam(QT_TWO, vecTeamTwoPlayer);

	for each (GEntityPlayer* pPlayer in vecTeamOnePlayer)
	{		
		pPlayer->RouteToMe(pNewCmd->Clone());		
	}

	for each (GEntityPlayer* pPlayer in vecTeamTwoPlayer)
	{
		pPlayer->RouteToMe(pNewCmd->Clone());		
	}

	SAFE_DELETE(pNewCmd);
}

void GQuestPVP::EnableMarkPos( int nNPCID )
{ 
	vector<GEntityNPC*> vecNPC = m_pField->GetEntityMgr()->FindNPCByID(nNPCID);
	for each (GEntityNPC* pNPC in vecNPC)
	{
		pNPC->EnableQPMarkPos();
	}
}

void GQuestPVP::DisableMarkPos( int nNPCID )
{
	vector<GEntityNPC*> vecNPC = m_pField->GetEntityMgr()->FindNPCByID(nNPCID);
	for each (GEntityNPC* pNPC in vecNPC)
	{
		pNPC->DisableQPMarkPos();
	}
}

void GQuestPVP::Reset()
{
	
}
