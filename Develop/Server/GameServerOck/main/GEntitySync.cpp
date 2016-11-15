#include "stdafx.h"
#include "GEntitySync.h"
#include "GField.h"
#include "GCommandTable.h"
#include "GModuleAI.h"
#include "GCommandCenter.h"
#include "GGlobal.h"
#include "GFieldSystem.h"

GEntitySync::GEntitySync() : m_sectorPt(-1, -1)
{
	m_nType |= ET_SYNC;
	m_nTypeID = ETID_SYNC;
}

GEntitySync::~GEntitySync()
{
}

bool GEntitySync::Create(MUID& uid)
{
	ResetFieldCell();

	return GEntity::Create(uid);
}

void GEntitySync::OnEnter()
{
	GFieldGrid::Sector* sector = m_pField->GetGrid().Lookup(GetSectorPoint());
	if (NULL == sector)
	{
		//todo 이런 경우 처리는?
		return;
	}

	m_pField->GetGrid().AddEntity(*sector, m_nTypeID, m_UID, IsHuge());

	GEntity::OnEnter();
}

void GEntitySync::OnLeave()
{
	m_pField->GetGrid().RemoveEntity(GetSectorPoint(), m_nTypeID, m_UID, IsHuge());

	GEntity::OnLeave();

	RouteEnterField(false);

	ResetFieldCell();
}

bool GEntitySync::SetPos(const vec3& pos)
{
	if (GEntity::SetPos(pos) == false)
		return false;

	UpdateFieldCell();

	return true;
}

void GEntitySync::RouteEnterField(bool bEnter)
{
	GFieldGrid::EntitySelector* entity_selector = 
		m_pField->GetGrid().GetEntitySelector();

	GVectorMUID vecEntities;
	class CollectPlayer : public GFieldGrid::Sector::IVisitor
	{
	public:
		CollectPlayer(GVectorMUID& players,const MUID& except)
			: m_players(players)
			, m_except(except) {}
	private:
		void OnVisit(const MUID& uid)
		{
			if (uid == m_except)
				return;

			m_players.Vector().push_back(uid);
		}
	private:
		GVectorMUID&	m_players;
		const MUID&		m_except;
	};
	entity_selector->VisitByPos(CollectPlayer(vecEntities, GetUID()), ETID_PLAYER, GetPos(), GetCellSize());

	if (vecEntities.Vector().empty()) return;

	if (bEnter)
	{
		RouteInEntity(vecEntities.Vector());
	}
	else
	{
		RouteOutEntity(vecEntities.Vector());
	}
}

////////////////////////////////////////////////////////////////////////////////////////

#include "GEntityActor.h"
#include "GConst.h"

void GEntitySync::UpdateFieldCell()
{
	{
		if (GConst::RUN_FOR_TEST &&
			!m_pField)
		{
			if (IsActor())
			{
				mlog3("entity_sync: m_pField is null. (is_npc: %d, name = %s, uid = %I64d)\n", IsNPC(), MLocale::ConvUTF16ToAnsi(ToEntityActor(this)->GetName()).c_str(), GetUID())	;
			}

		}
	}

	PFI_BLOCK_THISFUNC(430);
	VALID(m_pField);

	MPoint ptNewSec = m_pField->GetGrid().ConvertToIdx(m_vPos);

	if (m_sectorPt == ptNewSec)
		return; // 새로 이동한 곳이 예전에 있던 섹터랑 다를 경우

	m_pField->GetGrid().MoveEntity(m_sectorPt, ptNewSec, m_nTypeID, m_UID, IsHuge());

	
	MPoint oldSectorPt(m_sectorPt);
	m_sectorPt = ptNewSec;

	OnSectorChanged(oldSectorPt);
}

void GEntitySync::OnSectorChanged(const MPoint& oldSectorPt)
{
	VALID(NULL!=m_pField);
	GFieldGrid::DeltaSelector* const delta_selector = m_pField->GetGrid().GetDeltaSelector();
	VALID(NULL!=delta_selector);

	class ProcessChangedSectorEvent : public GFieldGrid::Sector::IVisitor
	{
	protected:
		typedef void (GEntitySync::*OnChangedSectorFunc)(const MUID&,MCommand* const);

	public:
		ProcessChangedSectorEvent(GEntitySync& entitySync,OnChangedSectorFunc func,MCommand* const cmd)
			: m_entitySync(entitySync)
			, m_func(func)
			, m_cmd(cmd) {}

	private:
		void OnVisit(const MUID& uid) { (m_entitySync.*m_func)(uid, m_cmd); }

	private:
		GEntitySync&		m_entitySync;
		OnChangedSectorFunc	m_func;
		MCommand* const		m_cmd;
	};

	{
		MCommand* const fieldIncmd = CreateFieldInCmd();
		if (NULL != fieldIncmd)
		{
			delta_selector->VisitNewSectors(
				ProcessChangedSectorEvent(*this, &GEntitySync::OnVisitedNewSectorPC, fieldIncmd), ETID_PLAYER, oldSectorPt, m_sectorPt, 1
				);
			
			if (0 != fieldIncmd->GetReceiverCount())
			{
				gsys.pCommandCenter->PostCommand(fieldIncmd);
			}
			else
			{
				delete fieldIncmd;
			}
		}
	}

	if (MPoint(-1, -1) != oldSectorPt)
	{
		// 처음 진입한 경우가 아니라면
		MCommand* const fieldOutcmd = CreateFieldOutCmd();
		if (NULL != fieldOutcmd)
		{
			delta_selector->VisitOldSectors(
				ProcessChangedSectorEvent(*this, &GEntitySync::OnVisitedOldSectorPC, fieldOutcmd), ETID_PLAYER, oldSectorPt, m_sectorPt, 1
				);

			if (0 != fieldOutcmd->GetReceiverCount())
			{
				gsys.pCommandCenter->PostCommand(fieldOutcmd);
			}
			else
			{
				delete fieldOutcmd;
			}
		}
	}
}

void GEntitySync::ResetFieldCell()
{
	m_sectorPt.x = -1;
	m_sectorPt.y = -1;
}

void GEntitySync::VisitNeighborSectorEntity(GFieldGrid::Sector::IVisitor& visitor,const ENTITY_TYPEID& type) const
{
	if (!m_pField)
		return;

	GFieldGrid::EntitySelector* const grid_selector = m_pField->GetGrid().GetEntitySelector();
	VALID(NULL!=grid_selector);
	grid_selector->VisitByPos(visitor, type, GetPos(), 1);
}

void GEntitySync::GetNeighborSectorNPC(vector<MUID>& vecNPC) const
{
	if (!m_pField)	return;

	GFieldGrid::EntitySelector* grid_selector = m_pField->GetGrid().GetEntitySelector();

	class CollectNPCs : public GFieldGrid::Sector::IVisitor
	{
	public:
		CollectNPCs(vector<MUID>& npcs) : m_npcs(npcs) {}
	private:
		void OnVisit(const MUID& uid)
		{
			m_npcs.push_back(uid);
		}
	private:
		vector<MUID>& m_npcs;
	};
	grid_selector->VisitByPos(CollectNPCs(vecNPC), ETID_NPC, GetPos(), 1);
}

void GEntitySync::GetNeighborSectorPlayer(vector<MUID>& vecPlayer) const
{
	if (!m_pField)	return;		

	GFieldGrid::EntitySelector* grid_selector = m_pField->GetGrid().GetEntitySelector();	
	
	GVectorMUID vecEntities;
	class CollectPlayer : public GFieldGrid::Sector::IVisitor
	{
	public:
		CollectPlayer(vector<MUID>& players) : m_players(players) {}
	private:
		void OnVisit(const MUID& uid)
		{
			m_players.push_back(uid);
		}
	private:
		vector<MUID>& m_players;
	};
	grid_selector->VisitByPos(CollectPlayer(vecPlayer), ETID_PLAYER, GetPos(), 1);
}

void GEntitySync::RouteToThisCellExceptMe(MCommand* pCommand) const
{
	RouteToThisCell(pCommand, GetUID());
}

void GEntitySync::RouteToThisCell(MCommand* pCommand, MUID uidExcept/*=0*/) const
{
	if (!pCommand)
		return;

	if (!m_pField)
	{
		SAFE_DELETE(pCommand);
		return;
	}

	GFieldGridRouter::RouteToCell(*this, *m_pField, pCommand, m_sectorPt, GetCellSize(), m_vPos.z, uidExcept);
}
void GEntitySync::RouteToField( MCommand* pCommand ) const
{
	if (!pCommand)
		return;

	if (!m_pField)
	{
		SAFE_DELETE(pCommand);
		return;
	}

	GFieldGridRouter router;
	router.RouteToField(m_pField, pCommand);
}

MCommand* GEntitySync::CreateFieldInCmd()
{
	return NULL;
}

MCommand* GEntitySync::CreateFieldOutCmd()
{
	return NULL;
}

void GEntitySync::OnVisitedNewSectorPC(const MUID& newSightedPC,MCommand* const fieldInCmd)
{
	if (NULL!=fieldInCmd)
	{
		fieldInCmd->AddReceiver(newSightedPC);
	}
}

void GEntitySync::OnVisitedOldSectorPC(const MUID& oldSIghtedPC,MCommand* const fieldOutCmd)
{
	if (NULL!=fieldOutCmd)
	{
		fieldOutCmd->AddReceiver(oldSIghtedPC);
	}
}

void GEntitySync::OnVisitedNewSectorNPC(const MUID& newSightedNPC,MCommand* const fieldInCmd)
{
}

void GEntitySync::OnVisitedOldSectorNPC(const MUID& oldSIghtedNPC,MCommand* const fieldOutCmd)
{
}

int GEntitySync::GetCellSize() const
{
	return IsHuge()?HUGE_SECTOR_SCALE:1;
}