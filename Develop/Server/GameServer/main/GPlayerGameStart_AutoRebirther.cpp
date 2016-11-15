#include "StdAfx.h"
#include "GPlayerGameStart_AutoRebirther.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GPlayerSystem.h"
#include "GPlayerGameStart_State.h"
#include "GRebirthSystem.h"
#include "GPlayerFieldDynamic.h"
#include "GConst.h"

GPlayerGameStart_AutoRebirther::GPlayerGameStart_AutoRebirther(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{

}

void GPlayerGameStart_AutoRebirther::Rebirth(GPlayerGameStart_State& PlayerGameStartState)
{
	if (false == PlayerGameStartState.IsDead()) return;

	if (true == PlayerGameStartState.IsBattleArena())
	{
		BattleArena(PlayerGameStartState);
	}
	else if (true == PlayerGameStartState.IsDynamicField())
	{
		Dynamic(PlayerGameStartState);
	}
	else if (true == PlayerGameStartState.IsShareField())
	{
		Share(PlayerGameStartState);
	}
}

void GPlayerGameStart_AutoRebirther::BattleArena(GPlayerGameStart_State& PlayerGameStartState)
{
	if (false == PlayerGameStartState.IsFieldExist())
	{
		m_pOwner->doRebirthBySelf();
	}
}

void GPlayerGameStart_AutoRebirther::Dynamic(GPlayerGameStart_State& PlayerGameStartState)
{
	if (false == PlayerGameStartState.IsFieldExist())
	{
		m_pOwner->doRebirthBySelf();
		gsys.pRebirthSystem->ApplyRebirthRecoverRate(m_pOwner, GConst::REBIRTH_SOULBINDING_RECOVER);
		gsys.pRebirthSystem->ApplyRebirthBuff(m_pOwner, GConst::REBIRTH_SOULBINDING_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);
	}
	else if (true == PlayerGameStartState.IsAutoRebirthTimeOver())
	{
		m_pOwner->doRebirthBySelf();

		if (m_pOwner->GetPlayerField().GetFieldDynamic().GetCheckpoint().IsInvalid())
		{
			gsys.pRebirthSystem->ApplyRebirthRecoverRate(m_pOwner, GConst::REBIRTH_CHECKPOINT_RECOVER);
			gsys.pRebirthSystem->ApplyRebirthBuff(m_pOwner, GConst::REBIRTH_CHECKPOINT_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);

		}
		else
		{
			gsys.pRebirthSystem->ApplyRebirthRecoverRate(m_pOwner, GConst::REBIRTH_SOULBINDING_RECOVER);
			gsys.pRebirthSystem->ApplyRebirthBuff(m_pOwner, GConst::REBIRTH_SOULBINDING_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);
		}
	}
}

void GPlayerGameStart_AutoRebirther::Share(GPlayerGameStart_State& PlayerGameStartState)
{
	if (true == PlayerGameStartState.IsAutoRebirthTimeOver())
	{
		m_pOwner->doRebirthBySelf();
		gsys.pRebirthSystem->ApplyRebirthRecoverRate(m_pOwner, GConst::REBIRTH_SOULBINDING_RECOVER);
		gsys.pRebirthSystem->ApplyRebirthBuff(m_pOwner, GConst::REBIRTH_SOULBINDING_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);
	}
}