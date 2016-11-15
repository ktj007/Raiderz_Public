#include "StdAfx.h"
#include "GBuffEntity.h"
#include "GModuleBuff.h"
#include "GGlobal.h"
#include "GBuffInfo.h"
#include "GField.h"
#include "GGlobal.h"
#include "GServer.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GCommandCenter.h"
#include "GEntityActor.h"
#include "GActorBuffEntity.h"

GBuffEntity* GBuffEntity::Spawn( GEntityActor* pOwner, vec3 vPoint, int nIncludeBuffID, int nLimitQty )
{
	VALID_RET(pOwner, NULL);
	GField* pField = pOwner->GetField();
	VALID_RET(pField, NULL);

	GBuffEntity* pBuffEntity = new GBuffEntity(pField, vPoint, pOwner->GetUID(), nIncludeBuffID);
	if (!pBuffEntity->Create(gsys.pServer->NewUID()))
	{
		dlog("%s - Invalid BuffID(%d).\n", __FUNCTION__, nIncludeBuffID);
		return NULL;
	}

	pBuffEntity->EnterToField(pField, vPoint);

	if (nLimitQty > 0 && // 0은 갯수 제한 없음
		pOwner->GetActorBuffEntity().GetQty(nIncludeBuffID) > nLimitQty)
	{
		pOwner->GetActorBuffEntity().DeleteFrontEntity(nIncludeBuffID, nLimitQty);
	}

	return pBuffEntity;
}




GBuffEntity::GBuffEntity(GField* pField, vec3 vPoint, MUID uidOwner, int nIncludeBuffID)
: m_uidOwner(uidOwner)
, m_nIncludeBuffID(nIncludeBuffID)
, m_pModuleBuff(NULL)
{
	m_nType |= ET_BUFF_ENTITY;
	m_nTypeID = ETID_BUFF_ENTITY;

	m_pField = pField;
	m_vPos = vPoint;
}

GBuffEntity::~GBuffEntity(void)
{
	SAFE_DELETE(m_pModuleBuff);
}

void GBuffEntity::OnUpdate( float fDelta )
{
	PFC_THISFUNC;
	__super::OnUpdate(fDelta);
	
	m_pModuleBuff->Update(fDelta);

	GEntityActor* pOwner = GetField()->FindActor(m_uidOwner);
	if (!pOwner)
	{
		// 소유자가 사라짐
		DeleteMe();
		return;
	}

	if (!m_pModuleBuff->IsGained(m_nIncludeBuffID))
	{
		DeleteMe();
		return;
	}
}

bool GBuffEntity::Create( MUID& uid )
{
	GBuffInfo* pBuffInfo = 
		gmgr.pBuffInfoMgr->Get(m_nIncludeBuffID);
	VALID_RET(pBuffInfo, false);

	m_pModuleBuff = new GModuleBuff(this);	

	bool bRet = 
		__super::Create(uid); // GetModuleBuff()->GainBuff() 아래로 옮길 수 없는가?
	
	m_pModuleBuff->GainBuffForced(pBuffInfo, pBuffInfo->m_fDuration, pBuffInfo->m_fPeriod, NULL);

	if (GetOwner())
	{
		GetOwner()->OnCreateBuffEntity(GetBuffID(), GetUID());
	}

	return bRet;
}

void GBuffEntity::OnDestroy()
{
	if (GetOwner())
	{
		GetOwner()->OnDestroyBuffEntity(GetBuffID(), GetUID());
	}

	__super::OnDestroy();
}

bool GBuffEntity::HasBuff( int nBuffID )
{
	return m_pModuleBuff->IsGained(nBuffID);
}

GEntityActor* GBuffEntity::GetOwner()
{
	return GetField()->FindActor(m_uidOwner);
}

void GBuffEntity::MakeTDCacheInfo(TD_UPDATE_CACHE_BUFFENTITY& out)
{
	if (!HasBuff(m_nIncludeBuffID))
		return;

	out.UID = GetUID();
	out.vPos = GetPos();
	out.fReaminTime = m_pModuleBuff->GetBuffRemainTime(m_nIncludeBuffID);
	out.nBuffID = m_nIncludeBuffID;
}

void GBuffEntity::RouteInEntity( const vector<MUID>& vecPlayers )
{
	if (!HasBuff(m_nIncludeBuffID))
		return;

	TD_UPDATE_CACHE_BUFFENTITY infoTD;
	MakeTDCacheInfo(infoTD);

	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_IN_BUFFENTITY, 
		1, 
		NEW_SINGLE_BLOB(&infoTD, sizeof(TD_UPDATE_CACHE_BUFFENTITY)));
	pNewCmd->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GBuffEntity::RouteOutEntity( const vector<MUID>& vecPlayers )
{
	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_OUT_BUFFENTITY, 1, NEW_UID(m_UID));
	pNewCmd->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

int GBuffEntity::GetBuffID() const
{
	return m_nIncludeBuffID;
}

MCommand* GBuffEntity::CreateFieldInCmd()
{
	if (!HasBuff(m_nIncludeBuffID))
		return NULL;

	TD_UPDATE_CACHE_BUFFENTITY infoTD;
	MakeTDCacheInfo(infoTD);

	return MakeNewCommand(
		MC_FIELD_IN_BUFFENTITY, 
		1, 
		NEW_SINGLE_BLOB( &infoTD, sizeof(TD_UPDATE_CACHE_BUFFENTITY) )
	);
}

MCommand* GBuffEntity::CreateFieldOutCmd()
{
	return MakeNewCommand(MC_FIELD_OUT_BUFFENTITY, 1, NEW_UID(m_UID));
}