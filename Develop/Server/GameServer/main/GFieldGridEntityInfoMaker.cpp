#include "StdAfx.h"
#include "GFieldGridEntityInfoMaker.h"
#include "CTransData.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "GFieldSensor.h"
#include "GBuffEntity.h"
#include "GEntityNPC.h"
#include "GEntityBPart.h"


void GFieldGridEntityInfoMaker::MakeSensorInfos(GEntityPlayer* pPlayer, vector<TD_TRIGGER_SENSOR_INFO>& outvecSensorList)
{
	VALID(pPlayer);
	GField* pField = pPlayer->GetField();
	VALID(pField);

	pField->GetFieldSensor().MakeUsableSensorInfo(pField, pPlayer, outvecSensorList);
}
void GFieldGridEntityInfoMaker::MakeBuffInfos(GEntityPlayer* pPlayer, GFieldGridSectorVec& vecCells, vector<TD_UPDATE_CACHE_BUFFENTITY>& outvecMagicArearInfoList)
{
	VALID(pPlayer);
	GField* pField = pPlayer->GetField();
	VALID(pField);

	GFieldGrid::EntitySelector* grid_selector = 
		pField->GetGrid().GetEntitySelector();

	class MakeMaginAreaInfoList : public GFieldGrid::Sector::IVisitor
	{
	public:
		MakeMaginAreaInfoList(GField& field,vector<TD_UPDATE_CACHE_BUFFENTITY>& magicAreaInfos)
			: m_magicAreaInfos(magicAreaInfos)
			, m_field(field) {}
	private:
		void OnVisit(const MUID& uid)
		{
			GBuffEntity* const buff = m_field.FindBuffEntity(uid);
			if (NULL == buff) 
				return;

			TD_UPDATE_CACHE_BUFFENTITY tdUpdateCacheMagicArea;
			buff->MakeTDCacheInfo(tdUpdateCacheMagicArea);
			m_magicAreaInfos.push_back(tdUpdateCacheMagicArea);
		}
	private:
		GField&								m_field;
		vector<TD_UPDATE_CACHE_BUFFENTITY>& m_magicAreaInfos;
	};

	for each (GFieldGrid::Sector* const sector in vecCells)
	{
		grid_selector->Visit(MakeMaginAreaInfoList(*pField, outvecMagicArearInfoList), ETID_BUFF_ENTITY, sector->Position(), 0);
	}
}
void GFieldGridEntityInfoMaker::MakeNPCInfos(GEntityPlayer* pPlayer, GFieldGridSectorVec& vecCells, vector<TD_UPDATE_CACHE_NPC>& outvecNPCInfoList, vector<TD_UPDATE_CACHE_PBART>& outvecEntityBPartInfoNode)
{
	VALID(pPlayer);
	GField* pField = pPlayer->GetField();
	VALID(pField);

	GFieldGrid::EntitySelector* grid_selector = 
		pField->GetGrid().GetEntitySelector();

	class MakeEntityBPartInfoNode : public GFieldGrid::Sector::IVisitor
	{
	public:
		MakeEntityBPartInfoNode(GEntityPlayer& player,GField& field,vector<TD_UPDATE_CACHE_NPC>& NPCInfoList,vector<TD_UPDATE_CACHE_PBART>& entityBPartInfoNode)
			: m_player(player)
			, m_NPCInfoList(NPCInfoList)
			, m_entityBPartInfoNode(entityBPartInfoNode)
			, m_field(field) {}
	private:
		void OnVisit(const MUID& uid)
		{
			GEntityNPC* const npc = m_field.FindNPC(uid);
			if (NULL == npc) 
				return;

			if (npc->IsNowInvisibility())
				return;

			if (npc->IsBPart())
			{
				TD_UPDATE_CACHE_PBART td;
				GEntityBPart* const bpart = dcast<GEntityBPart*>(npc);
				VALID(NULL!=bpart);
				bpart->MakeTDCacheInfo(&m_player, td);
				m_entityBPartInfoNode.push_back(td);
			}
			else
			{
				TD_UPDATE_CACHE_NPC tdUpdateCacheNPC;
				npc->MakeTDCacheInfo(&m_player, tdUpdateCacheNPC);
				m_NPCInfoList.push_back(tdUpdateCacheNPC);
			}
		}
	private:
		GField&							m_field;
		GEntityPlayer&					m_player;
		vector<TD_UPDATE_CACHE_PBART>&	m_entityBPartInfoNode;
		vector<TD_UPDATE_CACHE_NPC>&	m_NPCInfoList;
	};

	for each (GFieldGrid::Sector* const sector in vecCells)
	{
		grid_selector->Visit(MakeEntityBPartInfoNode(*pPlayer, *pField, outvecNPCInfoList, outvecEntityBPartInfoNode), ETID_NPC, sector->Position(), 0);
	}
}

void GFieldGridEntityInfoMaker::MakePlayerInfos(GEntityPlayer* pPlayer, GFieldGridSectorVec& vecCells, vector<TD_UPDATE_CACHE_PLAYER>& outvecPlayerInfoList, vector<TD_PLAYER_FEATURE_TATTOO>& outvecPlayerFeatureTattoo)
{
	VALID(pPlayer);
	GField* pField = pPlayer->GetField();
	VALID(pField);

	GFieldGrid::EntitySelector* grid_selector = 
		pField->GetGrid().GetEntitySelector();

	class MakePlayerInfoList : public GFieldGrid::Sector::IVisitor
	{
	public:

		MakePlayerInfoList(GField& field,const MUID& finderUID,vector<TD_UPDATE_CACHE_PLAYER>& playerInfoList, vector<TD_PLAYER_FEATURE_TATTOO>& playerFeatureTattoo)
			: m_field(field)
			, m_playerInfoList(playerInfoList)
			, m_playerFeatureTattoo(playerFeatureTattoo)
			, m_finderUID(finderUID){}

	private:
		void OnVisit(const MUID& uid)
		{
			GEntityPlayer* const player = m_field.FindPlayer(uid);
			if (NULL == player) 
				return;

			if (m_finderUID == uid)
				return;

			if (player->IsNowInvisibility())
				return;

			TD_UPDATE_CACHE_PLAYER tdUpdateCachePlayer;
			TD_PLAYER_FEATURE_TATTOO tdPlayerFeatureTattoo;
			player->MakeTDCacheInfo(tdUpdateCachePlayer, tdPlayerFeatureTattoo);
			m_playerInfoList.push_back(tdUpdateCachePlayer);

			if (tdPlayerFeatureTattoo.IsValid())
			{
				m_playerFeatureTattoo.push_back(tdPlayerFeatureTattoo);
			}
		}
	private:
		GField&							m_field;
		const MUID&						m_finderUID;
		vector<TD_UPDATE_CACHE_PLAYER>& m_playerInfoList;
		vector<TD_PLAYER_FEATURE_TATTOO>& m_playerFeatureTattoo;
	};
 
	for each (GFieldGrid::Sector* const sector in vecCells)
	{
		grid_selector->Visit(MakePlayerInfoList(*pField, pPlayer->GetUID(), outvecPlayerInfoList, outvecPlayerFeatureTattoo), ETID_PLAYER, sector->Position(), 0);
	}
}