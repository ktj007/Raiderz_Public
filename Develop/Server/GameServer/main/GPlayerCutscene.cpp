#include "StdAfx.h"
#include "GPlayerCutscene.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GPlayerDoing.h"
#include "GGlobal.h"
#include "GParty.h"
#include "GPartySystem.h"
#include "GCommand.h"
#include "GTDMaker.h"
#include "CTransData.h"
#include "GNPCIconSelector.h"
#include "CSConditionInfo.h"
#include "GDBManager.h"
#include "GCutsceneInfoMgr.h"
#include "GEnvManager.h"
#include "CCommandTable.h"
#include "GFieldGridEntityInfoMaker.h"
#include "GScriptGroup_Field.h"
#include "GClientCharRouter.h"
#include "GPlayerNPCIconSender.h"
#include "GClientFieldRouter.h"


GPlayerCutscene::GPlayerCutscene(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
}

GPlayerCutscene::~GPlayerCutscene()
{
}

void GPlayerCutscene::AssignSawnCutscene(std::vector<int>& vecSawnCutscene)
{
	for (int i = 0; i < (int)vecSawnCutscene.size(); ++i)
	{
		m_setSawnCutscene.insert(vecSawnCutscene[i]);;
	}
}

bool GPlayerCutscene::IsSawn(int nCutsceneID)
{
	SET_SAWN_CUTSCENE::iterator iter = m_setSawnCutscene.find(nCutsceneID);
	if (iter == m_setSawnCutscene.end())
	{
		return false;
	}
	return true;
}

bool GPlayerCutscene::BeginCutscene(int nCutsceneID)
{
	if (m_pOwner->GetDoing().IsNowCutscening()) return false;

	bool bRepeatable = true;

	GCutsceneInfo* pCutsceneInfo = gmgr.pCutsceneInfoMgr->Find(nCutsceneID);
	if (pCutsceneInfo != NULL)
		bRepeatable = pCutsceneInfo->bRepeatable;

	if (false == bRepeatable)
	{
		if (IsSawn(nCutsceneID)) return false;
		InsertSawnCutscene(nCutsceneID);
	}

	m_pOwner->GetDoing().ChangeDoing(CD_CUTSCENING);

	MCommand* pNewCmdMe = MakeNewCommand(MC_CUTSCENE_BEGIN_ME, 
		1, 
		NEW_INT(nCutsceneID));
	m_pOwner->RouteToMe(pNewCmdMe);

	MCommand* pNewCmdOther = MakeNewCommand(MC_CUTSCENE_BEGIN_OTHER, 
		1, 
		NEW_UID(m_pOwner->GetUID()));
	m_pOwner->RouteToThisCellExceptMe(pNewCmdOther);

	return true;
}
bool GPlayerCutscene::BeginPartyCutscene(int nCutsceneID)
{
	GParty* pParty = gsys.pPartySystem->FindParty(m_pOwner->GetPartyUID());
	if (pParty == NULL)
	{
		BeginCutscene(nCutsceneID);
	}
	else
	{
		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;
			GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
			if (pMember == NULL)	continue;

			pMember->GetCutscene().BeginCutscene(nCutsceneID);
		}
	}

	return true;
}
bool GPlayerCutscene::EndCutscene(int nCutsceneID)
{
	if (false == m_pOwner->GetDoing().IsNowCutscening()) return false;

	GField* pField = m_pOwner->GetField();
	if (!pField)
		return false;

	m_pOwner->GetDoing().ChangeDoing(CD_NORMAL);

	/// 컷씬 종료 응답
	MCommand* pNewCmdMe = MakeNewCommand(MC_CUTSCENE_END_ME, 1, NEW_INT(nCutsceneID));
	m_pOwner->RouteToMe(pNewCmdMe);

	/// 컷씬 종료 주변에 라우팅
	MCommand* pNewCmdOther = MakeNewCommand(MC_CUTSCENE_END_OTHER, 1, NEW_UID(m_pOwner->GetUID()));
	m_pOwner->RouteToThisCellExceptMe(pNewCmdOther);


	//////////////////////////////////////////////////////////////////////////
	// 정보 전달
	/// 1. 시간 정보
	GTDMaker tdMaker;
	vector<TD_BUFF_REMAIN_TIME> vecTDBuffRemainTime;
	tdMaker.MakeTD_BUFF_REMAIN_TIME(m_pOwner, vecTDBuffRemainTime);

	vector<TD_TALENT_REMAIN_COOLTIME> vecTDTalentRemainCoolTime;
	tdMaker.MakeTD_TALENT_REMAIN_COOLTIME(m_pOwner, vecTDTalentRemainCoolTime);

	float fAutoRebirthRemainTime = m_pOwner->GetPlayerInfo()->CalcAutoRebirthRemainTime();
	
	GClientCharRouter routerChar;
	routerChar.CharMyTimeInfo(m_pOwner->GetUID(), vecTDBuffRemainTime, vecTDTalentRemainCoolTime, fAutoRebirthRemainTime);


	/// 2. 섹터 정보들 (Player, NPC, Sensor, BuffEntity)
	vector<GFieldGrid::Sector*> vecCells;
	class CollectSectors : public GFieldGrid::SectorSelector::IVisitor
	{
	public:
		CollectSectors(vector<GFieldGrid::Sector*>& sectors) : m_sectors(sectors) {}

	private:
		void OnVisit(GFieldGrid::Sector& sector)
		{
			m_sectors.push_back(&sector);
		}

	private:
		vector<GFieldGrid::Sector*>& m_sectors;
	};
	pField->GetGrid().GetSectorSelector()->VisitNeighborsByPos(CollectSectors(vecCells), m_pOwner->GetPos(), 1);

	GFieldGridRouter sector_router;
	sector_router.SendToMe(m_pOwner, vecCells);


	/// 3. NPC Icon 정보들
	GVectorMUID vecUIDNPC;
	m_pOwner->GetNeighborSectorNPC(vecUIDNPC.Vector());
	m_pOwner->GetNPCIconSender().RemoveNPCIconData(vecUIDNPC.Vector());
	m_pOwner->GetNPCIconSender().SendByPlayerIn(vecUIDNPC.Vector(), true);


	/// 4. 시간, 날씨 정보
	GClientFieldRouter routerField;
	GAME_TIME_TYPE nTimeType = pField->GetCurrentTime();
	GAME_WEATHER_TYPE nWeatherType = pField->GetCurrentWeather();		
	routerField.ChangeTimeWeather(m_pOwner->GetUID(), nTimeType, nWeatherType);


	GetScriptField()->OnEndCutscene(pField->GetDelegator(), m_pOwner->GetDelegator(), nCutsceneID);

	return true;
}


void GPlayerCutscene::InsertSawnCutscene(int nCutsceneID)
{
	if (IsSawn(nCutsceneID)) return;

	m_setSawnCutscene.insert(nCutsceneID);

	gsys.pDBManager->CutsceneSawnInsert(m_pOwner->GetAID(), m_pOwner->GetUID(), m_pOwner->GetCID(), nCutsceneID);
}