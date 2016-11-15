#include "StdAfx.h"
#include "GPlayerGameStart_FieldGater.h"
#include "GEntityPlayer.h"
#include "GPlayerGameStart_State.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GDynamicFieldMaster.h"
#include "GPlayerFieldShared.h"
#include "GPlayerFieldDynamic.h"
#include "GPlayerBattleArena.h"
#include "GFieldInfoMgr.h"
#include "SDef.h"
#include "GConst.h"


GPlayerGameStart_FieldGater::GPlayerGameStart_FieldGater(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{

}

bool GPlayerGameStart_FieldGater::Gate(GPlayerGameStart_State& PlayerGameStartState)
{
	if (true == PlayerGameStartState.IsFirstLogin())
	{
		return FirstLogin();
	}
	else if (true == PlayerGameStartState.IsBattleArena())
	{		
		return BattleArena(PlayerGameStartState);
	}
	else if (true == PlayerGameStartState.IsDynamicField())
	{
		return Dynamic(PlayerGameStartState);
	}
	else if (true == PlayerGameStartState.IsShareField())
	{
		return Share(PlayerGameStartState);
	}

	return false;
}

bool GPlayerGameStart_FieldGater::FirstLogin()
{
	GFieldInfo* pFirstFieldInfo = gmgr.pFieldInfoMgr->Find(GConst::GetFirstEnterFieldID());
	if (NULL == pFirstFieldInfo) return false;

	const MARKER_INFO* pFirstMarker = pFirstFieldInfo->FindMarker(FIRST_ENTER_MARKER_ID);
	if (NULL == pFirstMarker) return false;

	return m_pOwner->GetPlayerField().GateToMarker(*pFirstMarker);
}

bool GPlayerGameStart_FieldGater::BattleArena(GPlayerGameStart_State& PlayerGameStartState)
{
	if (true == PlayerGameStartState.IsFieldExist())
	{
		GGateInfo gateInfo = m_pOwner->GetPlayerField().GetGateInfo();
		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(gateInfo.uidDynamicFieldGroup);
		if (NULL == pFieldGroup) return false;

		GATE_INFO_DYNAMIC sGateInfo(pFieldGroup->GetType(), pFieldGroup->GetInfo()->nID, gateInfo.nFieldID, gateInfo.vPosition, gateInfo.vDirection);

		if (m_pOwner->GetPlayerField().GetFieldDynamic().GateRequest(&sGateInfo))
			return true;
		else
			return m_pOwner->GetPlayerField().GetFieldDynamic().GateToEnterPos(PlayerGameStartState.GetExitFieldID());
	}
	else
	{
		m_pOwner->GetPlayerField().SetDynamicFieldGroupUID(MUID::Invalid());
		m_pOwner->GetPlayerBattleArena().SetBattleArenaUID(MUID::Invalid());

		if (true == PlayerGameStartState.IsDead())
		{
			return m_pOwner->GetPlayerField().GateToSoulBidning();			
		}
		else
		{
			return m_pOwner->GetPlayerField().GetFieldDynamic().GateToEnterPos(PlayerGameStartState.GetExitFieldID());
		}
	}

	return false;
}

bool GPlayerGameStart_FieldGater::Dynamic(GPlayerGameStart_State& PlayerGameStartState)
{
	if (true == PlayerGameStartState.IsFieldExist())
	{
		if (true == PlayerGameStartState.IsDead() && 
			true == PlayerGameStartState.IsAutoRebirthTimeOver())
		{
			GPlayerField& rPlayerField = m_pOwner->GetPlayerField();
			GPlayerFieldDynamic& rPlayerFieldDynamic = rPlayerField.GetFieldDynamic();
			const MARKER_INFO& rCheckPoint = rPlayerFieldDynamic.GetCheckpoint();

			if (rCheckPoint.IsValid())
			{
				return rPlayerFieldDynamic.GateToCheckPoint();
			}
			else
			{
				return rPlayerField.GateToSoulBidning();
			}
		}
		else
		{
			GGateInfo gateInfo = m_pOwner->GetPlayerField().GetGateInfo();
			GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(gateInfo.uidDynamicFieldGroup);
			if (NULL == pFieldGroup) return false;

			GATE_INFO_DYNAMIC sGateInfo(pFieldGroup->GetType(), pFieldGroup->GetInfo()->nID, gateInfo.nFieldID, gateInfo.vPosition, gateInfo.vDirection);

			if (m_pOwner->GetPlayerField().GetFieldDynamic().GateRequest(&sGateInfo))
				return true;
			else
				return m_pOwner->GetPlayerField().GetFieldDynamic().GateToEnterPos(PlayerGameStartState.GetExitFieldID());
		}
	}
	else
	{
		m_pOwner->GetPlayerField().SetDynamicFieldGroupUID(MUID::ZERO);

		if (true == PlayerGameStartState.IsDead())
		{
			return m_pOwner->GetPlayerField().GateToSoulBidning();			
		}
		else
		{
			return m_pOwner->GetPlayerField().GetFieldDynamic().GateToEnterPos(PlayerGameStartState.GetExitFieldID());
		}	
	}

	return false;
}

bool GPlayerGameStart_FieldGater::Share(GPlayerGameStart_State& PlayerGameStartState)
{
	if (NULL == m_pOwner) return false;

	if (true == PlayerGameStartState.IsDead() && 
		true == PlayerGameStartState.IsAutoRebirthTimeOver())
	{
		return m_pOwner->GetPlayerField().GateToSoulBidning();
	}
	else
	{
		GGateInfo gateInfo = m_pOwner->GetPlayerField().GetGateInfo();
		return m_pOwner->GetPlayerField().GetFieldShared().GateRequest(gateInfo.nFieldID, gateInfo.nChannelID, gateInfo.vPosition, gateInfo.vDirection);
	}
}
