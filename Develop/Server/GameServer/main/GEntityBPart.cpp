#include "stdafx.h"
#include "GEntityBPart.h"
#include "GGlobal.h"
#include "GServer.h"
#include "CCommandTable.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GEntityPlayer.h"

GEntityBPart::GEntityBPart()
: GEntityNPC()
{
	m_nType |= ET_ENTITY_BPART;
}

GEntityBPart::~GEntityBPart()
{

}

void GEntityBPart::SetBPartInfo( int nNPCID, int nBPartID, int nFlag )
{
	m_infoBPart.nNPCID = nNPCID;
	m_infoBPart.nBPartID = nBPartID;
	m_infoBPart.nFlag = nFlag;
}

void GEntityBPart::MakeTDCacheInfo(GEntityPlayer* pReqPlayer, TD_UPDATE_CACHE_PBART& out )
{
	out.uid = m_UID;
	out.nUIID = m_nUIID;
	out.nNPCID = m_infoBPart.nNPCID;
	out.nBPartID = m_infoBPart.nBPartID;
	out.nBPartFlag = m_infoBPart.nFlag;
	out.vPos = m_vPos;
	out.svDir = GetDir();
	SetBitSet(out.nStatusFlag, UNS_DEAD);

	if (pReqPlayer &&
		GetNPCLoot().GetDropList().IsViewableCID(pReqPlayer->GetCID()))
	{
		SetBitSet(out.nStatusFlag, UNS_LOOTABLE);
	}
}


void GEntityBPart::RouteInEntity(const vector<MUID>& vecPlayers)
{
	// 서버 준비 중 NPC가 추가될때는 라우팅하지 않는다.
	if (gsys.pServer == NULL || !gsys.pServer->IsCreated())	return;

	TD_UPDATE_CACHE_PBART td_npc_info;
	MakeTDCacheInfo(NULL, td_npc_info);

	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_IN_BPART, 
		1, 
		NEW_SINGLE_BLOB(&td_npc_info, sizeof(TD_UPDATE_CACHE_PBART)));
	pNewCmd->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GEntityBPart::RouteOutEntity( const vector<MUID>& vecPlayers )
{
	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_OUT_BPART, 1, NEW_USHORT(m_nUIID));
	pNewCmd->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

MCommand* GEntityBPart::CreateFieldInCmd()
{
	// 서버 준비 중 NPC가 추가될때는 라우팅하지 않는다.
	if (gsys.pServer == NULL || !gsys.pServer->IsCreated())	return NULL;

	TD_UPDATE_CACHE_PBART td_npc_info;
	MakeTDCacheInfo(NULL, td_npc_info);

	return MakeNewCommand(
		MC_FIELD_IN_BPART,
		1,
		NEW_SINGLE_BLOB( &td_npc_info, sizeof(TD_UPDATE_CACHE_PBART) )
	);
}

MCommand* GEntityBPart::CreateFieldOutCmd()
{
	return MakeNewCommand(MC_FIELD_OUT_BPART, 1, NEW_USHORT(m_nUIID));
}
