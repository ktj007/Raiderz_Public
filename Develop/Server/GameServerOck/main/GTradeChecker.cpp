#include "StdAfx.h"
#include "GTradeChecker.h"
#include "GEntityPlayer.h"
#include "GPlayerTrade.h"
#include "CCommandResultTable.h"
#include "GGlobal.h"
#include "GInteractionSystem.h"
#include "GTradeSystem.h"
#include "GTradeCanceler.h"
#include "GRelationChecker.h"

bool GTradeChecker::CancelInvalidTrade( GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer )
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == CheckTrader(pReqPlayer, pTarPlayer) ||
		false == CheckDistance(pReqPlayer, pTarPlayer) ||
		false == CheckEnemy(pReqPlayer, pTarPlayer))
	{
		gsys.pTradeSystem->GetCanceler().Cancel(pReqPlayer);
		return false;
	}

	return true;
}

bool GTradeChecker::CheckTrader(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == pReqPlayer->GetPlayerTrade().IsTrader(pTarPlayer->GetUID()) || 
		false == pTarPlayer->GetPlayerTrade().IsTrader(pReqPlayer->GetUID()))
	{
		return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_NOT_TRADING);
	}

	return true;
}

bool GTradeChecker::CheckDistance(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == gsys.pInteractionSystem->IsInteractableDistance(pReqPlayer, pTarPlayer))
	{
		return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_TARGET_DISTANCE);
	}

	return true;
}

bool GTradeChecker::CheckEnemy(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	GRelationChecker relationChecker;
	return !relationChecker.IsEnemy(pReqPlayer, pTarPlayer);
}