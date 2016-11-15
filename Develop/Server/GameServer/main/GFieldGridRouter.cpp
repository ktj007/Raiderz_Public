#include "stdafx.h"
#include "GFieldGridRouter.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GBuffEntity.h"
#include "GGlobal.h"
#include "CCommandTable.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GFieldSensor.h"
#include "GPacketRouteLimiterMgr.h"
#include "GAsyncTaskFactory.h"
#include "GServer.h"
#include "GFieldGridEntityInfoMaker.h"
#include "AStlUtil.h"
#include "GPartySystem.h"
#include "GHateTable.h"


void GFieldGridRouter::SendToMe(GEntityPlayer* pPlayer, const vector<GFieldGrid::Sector*>& vecNewCells)
{
	VALID(pPlayer);
	
	vector<TD_TRIGGER_SENSOR_INFO> vecMarkers;
	vector<TD_UPDATE_CACHE_BUFFENTITY> vecBuffEntityNode;
	vector<TD_UPDATE_CACHE_NPC> vecNPCInfoNode;
	vector<TD_UPDATE_CACHE_PBART> vecEntityBPartInfoNode;
	vector<TD_UPDATE_CACHE_PLAYER> vecPlayerInfoNode;
	vector<TD_PLAYER_FEATURE_TATTOO> vecPlayerFeatureTattooNode;

	GFieldGridEntityInfoMaker gridInfoMaker;
	gridInfoMaker.MakeSensorInfos(pPlayer, vecMarkers);
	gridInfoMaker.MakeBuffInfos(pPlayer, vecNewCells, vecBuffEntityNode);
	gridInfoMaker.MakeNPCInfos(pPlayer, vecNewCells, vecNPCInfoNode, vecEntityBPartInfoNode);
	gridInfoMaker.MakePlayerInfos(pPlayer, vecNewCells, vecPlayerInfoNode, vecPlayerFeatureTattooNode);

	if (vecPlayerInfoNode.empty() &&
		vecNPCInfoNode.empty() &&
		vecEntityBPartInfoNode.empty() &&
		vecMarkers.empty() &&
		vecBuffEntityNode.empty() &&
		vecPlayerFeatureTattooNode.empty()
		)
	{
		return; // 보낼 정보가 없음
	}


	// 섹터 정보를 각각 50개단위로 나눠서 보낸다.
	size_t nMaxSize = 0;
	nMaxSize = max(nMaxSize, vecPlayerInfoNode.size());
	nMaxSize = max(nMaxSize, vecNPCInfoNode.size());
	nMaxSize = max(nMaxSize, vecEntityBPartInfoNode.size());
	nMaxSize = max(nMaxSize, vecMarkers.size());
	nMaxSize = max(nMaxSize, vecBuffEntityNode.size());

	int nDivideNumber = 50;
	int nSplitCount = nMaxSize / nDivideNumber;
	if (0 < nMaxSize % nDivideNumber)
	{
		nSplitCount++;
	}

	for(int i = 0; i < nSplitCount; i++)
	{
		MCommand* pNewCommand = MakeNewCommand(MC_FIELD_SECTOR_ENTITY_INFO, 
			6,
			NewSplitParameter(vecPlayerInfoNode, i, nDivideNumber),
			NewSplitParameter(vecPlayerFeatureTattooNode, i, nDivideNumber),
			NewSplitParameter(vecNPCInfoNode, i, nDivideNumber),
			NewSplitParameter(vecEntityBPartInfoNode, i, nDivideNumber),
			NewSplitParameter(vecBuffEntityNode, i, nDivideNumber),
			NewSplitParameter(vecMarkers, i, nDivideNumber));

		pPlayer->RouteToMe(pNewCommand);
	}
}

bool GFieldGridRouter::CollectCellReceivers( vector<MUID>& outvecEntities, const GEntitySync& sender, GField& field, MCommand* const pCommand, const MPoint& sectorPt, int nCellSize, float fBaseHeight, const MUID& uidExcept/*=MUID::Invalid()*/)
{
	VALID_RET(NULL != pCommand, false);

	GFieldGrid::EntitySelector* const entity_selector = 
		field.GetGrid().GetEntitySelector();

	GFieldGrid::SectorSelector* const sector_selector = 
		field.GetGrid().GetSectorSelector();

	GVectorMUID vecAllEntitiesInGrid;

	const bool CROWFILTERED = true;
	const bool NOT_CORWD	= false;

	if (pCommand->m_pCommandDesc &&
		pCommand->m_pCommandDesc->IsFlag(MCDT_FILTER_CROWDED) && 
		sender.IsPlayer())
	{
		class CollectPlayers : public GFieldGrid::Sector::IVisitor
		{
		public:
			CollectPlayers(vector<MUID>& players) : m_players(players) {}

		private:
			void OnVisit(const MUID& uid)
			{
				m_players.push_back(uid);
			}

		private:
			vector<MUID>& m_players;
		};

		class VisitNonCrowdedSector : public GFieldGrid::SectorSelector::IVisitor
		{
		public:
			VisitNonCrowdedSector(CollectPlayers& playerCollector,GPacketRouteLimiterMgr& pkRouteLimiterMgr,int cmdID,const MUID& senderUID)
				: m_playerCollector(playerCollector)
				, m_pkRouteLimiterMgr(pkRouteLimiterMgr)
				, m_cmdID(cmdID)
				, m_senderUID(senderUID) {}

		private:
			void OnVisit(GFieldGrid::Sector& sector)
			{
				if (false == m_pkRouteLimiterMgr.TryRoute(sector, m_cmdID, m_senderUID))
					return;

				sector.Visit(m_playerCollector, ETID_PLAYER);
			}

		private:
			CollectPlayers&			m_playerCollector;
			GPacketRouteLimiterMgr& m_pkRouteLimiterMgr;
			int						m_cmdID;
			const MUID&				m_senderUID;
		};

		VALID_RET(NULL != field.GetGrid().GetPacketRouteLimiterMgr(), true);
		sector_selector->VisitNeighbors(
			VisitNonCrowdedSector(
				CollectPlayers( vecAllEntitiesInGrid.Vector() ), *field.GetGrid().GetPacketRouteLimiterMgr(), pCommand->GetID(), pCommand->GetSenderUID()
			),
			sectorPt,
			nCellSize
		);
	}
	else
	{
		entity_selector->GetPlayers(vecAllEntitiesInGrid.Vector(), sectorPt, nCellSize);
	}

	for each (const MUID& nUID in vecAllEntitiesInGrid.Vector())
	{
		if (nUID == uidExcept)
			continue;

		// 허용 높이차 검사
		if (!CheckHeight(fBaseHeight, nUID, field))
			continue;

		outvecEntities.push_back(nUID);
	}

	return NOT_CORWD;
}

void GFieldGridRouter::CollectEssentialReceivers(bool bCrowdFiltered, vector<MUID>& outvecEntities, const GEntitySync& sender, MCommand* pCommand, const MUID& uidExcept)
{
	if (!bCrowdFiltered) return;
	if (!pCommand->m_pCommandDesc) return;
	if (!pCommand->m_pCommandDesc->IsFlag(MCDT_FILTER_CROWDED)) return;
	if (!sender.IsPlayer()) return;

	vector<MUID> vecESReceivers;
	const_cast<GEntitySync*>(&sender)->CollectEssentialReceiver(vecESReceivers);
	if (vecESReceivers.empty()) return;

	set<MUID> setEntities = VectorToSet(outvecEntities);
	for each (const MUID& uidESReceiver in vecESReceivers)
	{
		if (IsNotExist(setEntities, uidESReceiver) && uidExcept != uidESReceiver)
		{
			outvecEntities.push_back(uidESReceiver);
		}
	}
}

void GFieldGridRouter::RouteToCell(const GEntitySync& sender, GField& field, MCommand* const pCommand, const MPoint& sectorPt, int nCellSize, float fBaseHeight, const MUID& uidExcept/*=MUID::Invalid()*/)
{
	PFC_THISFUNC;

	VALID(pCommand);
	VALID(pCommand->m_pCommandDesc);

	GVectorMUID vecReceivers;
	bool bCrowdFiltered = CollectCellReceivers(vecReceivers.Vector(), sender, field, pCommand, sectorPt, nCellSize, fBaseHeight, uidExcept);
	CollectEssentialReceivers(bCrowdFiltered, vecReceivers.Vector(), sender, pCommand, uidExcept);	

	if (vecReceivers.Vector().empty())
	{
		delete pCommand;
		return;
	}

	// 바로 보냅니다
	pCommand->AddReceiver(vecReceivers.Vector());
	gsys.pCommandCenter->PostCommand(pCommand);
}

void GFieldGridRouter::RouteToField(GField* pField, MCommand* pCommand)
{
	VALID(pField);

	if (!pCommand) 
		return;

	GVectorMUID vecPlayerUIDs;
	
	pField->GetEntityMgr()->GetPlayerUIDs(vecPlayerUIDs.Vector());

	pCommand->AddReceiver(vecPlayerUIDs.Vector());
	gsys.pCommandCenter->PostCommand(pCommand);
}

bool GFieldGridRouter::CheckHeight(float fBaseHeight, const MUID& uidTargetPlayer, GField& field)
{
	const GFieldInfo* pFieldInfo = field.GetInfo();
	VALID_RET(pFieldInfo != NULL, true);

	// 라우팅 허용 높이제한이 있는 필드 처리
	if (pFieldInfo->IsSectorHeightLimitField())
	{
		GEntityPlayer* pPlayer = field.FindPlayer(uidTargetPlayer);
		
		if (pPlayer != NULL)			
		{
			vec3 vPos = pPlayer->GetPos();

			return pFieldInfo->IsAllowSectorHeightLimit(fBaseHeight - vPos.z);
		}
	}

	return true;
}
