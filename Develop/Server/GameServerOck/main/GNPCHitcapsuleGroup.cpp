#include "StdAfx.h"
#include "GNPCHitcapsuleGroup.h"
#include "GDef.h"
#include "GEntityNPC.h"
#include "GCommand.h"
#include "CCommandTable.h"


GNPCHitcapsuleGroup::GNPCHitcapsuleGroup( GEntityNPC* pOwner )
: m_pOwner(pOwner)
{

}
GNPCHitcapsuleGroup::~GNPCHitcapsuleGroup(void)
{
}

void GNPCHitcapsuleGroup::Init( GNPCInfo* pNPCInfo )
{
	VALID(pNPCInfo);

	m_HitCapsuleIndexMgr.Clear();
	m_HitCapsuleIndexMgr.InsertNPCModeGroupIndex(pNPCInfo->nMode, pNPCInfo->nHitTestIndex);	// 기본 모드
	for each (GNPCInfo* each in pNPCInfo->vecModes)
	{
		DCHECK(each);
		m_HitCapsuleIndexMgr.InsertNPCModeGroupIndex(each->nMode, each->nHitTestIndex);	// 추가 모드
	}
}

bool GNPCHitcapsuleGroup::ChangeCapsuleGroup_ByTrigger( int8 val )
{
	if (!IsValidCapsuleGroupID(val))
		return false;

	m_HitCapsuleIndexMgr.Change_ByTrigger(val);

	RouteChangeCapsule(val);

	return true;
}

bool GNPCHitcapsuleGroup::ChangeCapsuleGroup_ByTalent( int8 val )
{
	if (!IsValidCapsuleGroupID(val))
		return false;

	m_HitCapsuleIndexMgr.Change_ByTalent(val);

	return true;
}

void GNPCHitcapsuleGroup::ChangeCapsuleGroup_ByTalentComplete()
{
	m_HitCapsuleIndexMgr.Change_ByTalentComplete();
}

void GNPCHitcapsuleGroup::ChangeCapsuleGroup_ByMode(NPC_MODE nMode)
{
	m_HitCapsuleIndexMgr.ChangeNPCMode(nMode);
}

int8 GNPCHitcapsuleGroup::GetCapsuleGroupIndex()
{
	return m_HitCapsuleIndexMgr.GetGroupIndex();
}

bool GNPCHitcapsuleGroup::IsValidCapsuleGroupID( int8 val ) const
{
	VALID_RET(m_pOwner, false);
	VALID_RET(m_pOwner->GetMeshInfo(), false);

	if (val == INVALID_HITCAPSULEGROUP_ID)
		return true;

	return m_pOwner->GetMeshInfo()->HasHitGroup(val);
}

void GNPCHitcapsuleGroup::RouteChangeCapsule(int8 nHitCapsuleGroup)
{
	VALID(m_pOwner);

	MCommand* pNewCommand = MakeNewCommand(MC_NPC_CHANGE_CAPSULE, 0, 
		2, 
		NEW_UID(m_pOwner->GetUID()),
		NEW_CHAR(nHitCapsuleGroup));

	m_pOwner->RouteToThisCell(pNewCommand);
}

