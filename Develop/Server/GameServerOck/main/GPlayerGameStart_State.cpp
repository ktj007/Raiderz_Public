#include "StdAfx.h"
#include "GPlayerGameStart_State.h"
#include "GEntityPlayer.h"
#include "GPlayerBattleArena.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GFieldInfoMgr.h"

static const int NONE_EXIT_FIELD_ID = 0;

GPlayerGameStart_State::InputParam::InputParam( GEntityPlayer* pPlayer )
: nExitFieldID(0), bBattleArenaEntered(false), uidDynamicFieldGroup(MUID::ZERO), bIsDead(false), nElapsedDeadTimeSec(0)
{
	if (pPlayer)
	{
		nExitFieldID = pPlayer->GetPlayerField().GetGateInfo().nFieldID;
		bBattleArenaEntered = pPlayer->GetPlayerBattleArena().IsEntered();
		uidDynamicFieldGroup = pPlayer->GetPlayerField().GetDynamicFieldGroupUID();		
		bIsDead = pPlayer->IsDead();
		nElapsedDeadTimeSec = pPlayer->GetPlayerInfo()->nElapsedDeadTimeSec;
	}
}

GPlayerGameStart_State::InputParam::InputParam()
: nExitFieldID(0), bBattleArenaEntered(false), uidDynamicFieldGroup(MUID::ZERO), bIsDead(false), nElapsedDeadTimeSec(0)
{

}

GPlayerGameStart_State::GPlayerGameStart_State(GEntityPlayer* pPlayer)
: m_nGameStartFieldType(GSFT_NONE)
, m_bFieldExist(false)
, m_bDead(false)
, m_bAutoRebirthTimeOver(false)
, m_nExitFieldID(0)
{
	InputParam inputParam(pPlayer);
	Setup(inputParam);
}

GPlayerGameStart_State::GPlayerGameStart_State(InputParam& inputParam)
: m_nGameStartFieldType(GSFT_NONE)
, m_bFieldExist(false)
, m_bDead(false)
, m_bAutoRebirthTimeOver(false)
{
	Setup(inputParam);
}

void GPlayerGameStart_State::Setup( InputParam& inputParam )
{
	SetExitFieldID(inputParam);
	SetFieldType(inputParam);
	SetFieldExist(inputParam);
	SetDead(inputParam);
	SetAutoRebirthTimeOver(inputParam);	
}

void GPlayerGameStart_State::SetExitFieldID( InputParam& inputParam )
{
	m_nExitFieldID = inputParam.nExitFieldID;
}

void GPlayerGameStart_State::SetFieldType(InputParam& inputParam)
{
	int nExitFieldID = inputParam.nExitFieldID;

	if (NONE_EXIT_FIELD_ID == nExitFieldID)
	{
		m_nGameStartFieldType = GSFT_FIRST_LOGIN;
	}
	else if (true == inputParam.bBattleArenaEntered)
	{		
		m_nGameStartFieldType = GSFT_BATTLE_ARENA;
	}
	else
	{
		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nExitFieldID);
		if (NULL == pFieldInfo)
		{
			m_nGameStartFieldType = GSFT_NONE;
		}
		else
		{
			if (true == pFieldInfo->m_bDynamic)
			{
				m_nGameStartFieldType = GSFT_DYNAMIC;
			}
			else
			{			
				m_nGameStartFieldType = GSFT_SHARE;
			}
		}
	}
}

void GPlayerGameStart_State::SetFieldExist( InputParam& inputParam )
{
	int nExitFieldID = inputParam.nExitFieldID;

	if (NONE_EXIT_FIELD_ID == nExitFieldID)
	{
		m_bFieldExist = false;
	}
	else if (true == inputParam.bBattleArenaEntered)
	{		
		m_bFieldExist = (NULL != gmgr.pFieldMgr->GetDynamicField(inputParam.uidDynamicFieldGroup, nExitFieldID));
	}
	else
	{
		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nExitFieldID);
		if (NULL == pFieldInfo)
		{
			m_bFieldExist = false;
		}
		else
		{
			if (true == pFieldInfo->m_bDynamic)
			{
				m_bFieldExist = (NULL != gmgr.pFieldMgr->GetDynamicField(inputParam.uidDynamicFieldGroup, nExitFieldID));
			}
			else
			{
				m_bFieldExist = true;
			}
		}
	}
}

void GPlayerGameStart_State::SetDead(InputParam& inputParam)
{
	m_bDead = inputParam.bIsDead;
}

void GPlayerGameStart_State::SetAutoRebirthTimeOver(InputParam& inputParam)
{
	m_bAutoRebirthTimeOver = GAME_AUTO_REBIRTH_CHECK_TICK < inputParam.nElapsedDeadTimeSec;
}