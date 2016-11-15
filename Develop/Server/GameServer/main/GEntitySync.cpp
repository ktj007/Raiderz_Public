#include "stdafx.h"
#include "GEntitySync.h"
#include "GField.h"
#include "GCommandTable.h"
#include "GModuleAI.h"
#include "GCommandCenter.h"
#include "GGlobal.h"
#include "GFieldSystem.h"
#include "GConfig.h"	//todo 제거예정

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
	entity_selector->GetPlayers(vecEntities.Vector(), GetPos(), GetCellSize(), GetUID());
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

	if (true == GConfig::m_moveSectorWithoutCopySectorVector)	//todo 완료 후 gconfig.h헤더 인클루드 제거
	{
		MPoint oldSectorPt(m_sectorPt);
		m_sectorPt = ptNewSec;

		OnSectorChanged(oldSectorPt);
	}
	else
	{
		vector<GFieldGrid::Sector*> vecNewCells;
		vector<GFieldGrid::Sector*> vecOldCells;
		CalcNewCells(vecNewCells, m_sectorPt, ptNewSec);
		if (m_sectorPt != MPoint(-1, -1)) // 처음 진입한 경우라면 무시
		{
			CalcOldCells(vecOldCells, m_sectorPt, ptNewSec);
		}

		m_sectorPt = ptNewSec;
		OnSectorChanged(vecNewCells, vecOldCells);
	}
}

void GEntitySync::OnSectorChanged( const vector<GFieldGrid::Sector*>& vecNewSectors, const vector<GFieldGrid::Sector*>& vecOldSectors )
{
	GFieldGrid::EntitySelector* entity_selector = 
		m_pField->GetGrid().GetEntitySelector();

	GVectorMUID vecEntitiesIn;
	// EntityIn
	entity_selector->GetPlayers(vecEntitiesIn.Vector(), vecNewSectors, GetUID());
	if (!vecEntitiesIn.Vector().empty())
	{
		RouteInEntity(vecEntitiesIn.Vector());
	}

	// EntityOut
	GVectorMUID vecEntitiesOut;	
	entity_selector->GetPlayers(vecEntitiesOut.Vector(), vecOldSectors, GetUID());
	if (!vecEntitiesOut.Vector().empty())
	{
		RouteOutEntity(vecEntitiesOut.Vector());
	}
}

void GEntitySync::CalcNewCells( vector<GFieldGrid::Sector*>& vecNewSectors, MPoint ptOldSec, MPoint ptNewSec ) const
{
	GFieldGrid::DeltaSelector* delta_selector = 
		m_pField->GetGrid().GetDeltaSelector();

	delta_selector->GetDeltaNewSectors(vecNewSectors, ptOldSec, ptNewSec, GetCellSize());
}

void GEntitySync::CalcOldCells( vector<GFieldGrid::Sector*>& vecOldSectors, MPoint ptOldSec, MPoint ptNewSec ) const
{
	GFieldGrid::DeltaSelector* delta_selector = 
		m_pField->GetGrid().GetDeltaSelector();

	delta_selector->GetDeltaOldSectors(vecOldSectors, ptOldSec, ptNewSec, GetCellSize());
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
			gsys.pCommandCenter->PostCommand(fieldIncmd);
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
			gsys.pCommandCenter->PostCommand(fieldOutcmd);
		}
	}
}

void GEntitySync::ResetFieldCell()
{
	m_sectorPt.x = -1;
	m_sectorPt.y = -1;
}

void GEntitySync::GetNeighborSectorNPC(vector<MUID>& vecNPC) const
{
	if (!m_pField)	return;

	GFieldGrid::EntitySelector* grid_selector = m_pField->GetGrid().GetEntitySelector();

	grid_selector->GetNPCs(vecNPC, GetPos());
}

void GEntitySync::GetNeighborSectorPlayer(vector<MUID>& vecPlayer) const
{
	if (!m_pField)	return;		

	GFieldGrid::EntitySelector* grid_selector = m_pField->GetGrid().GetEntitySelector();	
	
	grid_selector->GetPlayers(vecPlayer, GetPos());
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