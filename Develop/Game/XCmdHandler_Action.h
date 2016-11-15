#ifndef _XCMD_HANDLER_ACTION_H
#define _XCMD_HANDLER_ACTION_H

#include "MCommandHandler.h"
using namespace minet;

/// 액션 커맨드 처리 핸들러
class XCmdHandler_Action  : public MCommandHandler
{
private:

protected:
	DECL_CMD_HANDLER(OnMove);
	DECL_CMD_HANDLER(OnStop);
	DECL_CMD_HANDLER(OnMoveStop);
	DECL_CMD_HANDLER(OnRotate);
	DECL_CMD_HANDLER(OnChangeStance);

	DECL_CMD_HANDLER(OnPrepareAttack);
	DECL_CMD_HANDLER(OnAttackCancel);
	DECL_CMD_HANDLER(OnBreakPart);
	DECL_CMD_HANDLER(OnBreakPartRecovery);
	DECL_CMD_HANDLER(OnBreakPartEx);
		
	DECL_CMD_HANDLER(OnActTalent);
	DECL_CMD_HANDLER(OnExtraActTalent);
	DECL_CMD_HANDLER(OnActTalentWithGround);
	DECL_CMD_HANDLER(OnUseTalent);

	DECL_CMD_HANDLER(OnRequestFailTalent);
	DECL_CMD_HANDLER(OnCancelTalent);
	DECL_CMD_HANDLER(OnActSpellProjectile);
	DECL_CMD_HANDLER(OnActSpellMagicArea);
	DECL_CMD_HANDLER(OnReCastingSpell);
	DECL_CMD_HANDLER(OnEndTalentCooldown);

	DECL_CMD_HANDLER(OnTalentHit);
	DECL_CMD_HANDLER(OnBuffHit);
	DECL_CMD_HANDLER(OnHitBuffNoMotionFactor);
	DECL_CMD_HANDLER(OnHitTalentNoMotionFactor);
	DECL_CMD_HANDLER(OnHitTalentKnockbackMotionFactor);
	DECL_CMD_HANDLER(OnTalentHitDodge);
	DECL_CMD_HANDLER(OnTalentHeal);

	DECL_CMD_HANDLER(OnAttackArchery);
	DECL_CMD_HANDLER(OnAttackArcheryDetail);
	
	DECL_CMD_HANDLER(OnGuard);
	DECL_CMD_HANDLER(OnGuardFailed);
	DECL_CMD_HANDLER(OnGuardReleased);
	DECL_CMD_HANDLER(OnJump);
	DECL_CMD_HANDLER(OnFalling);
	DECL_CMD_HANDLER(OnFallingDamage);	
	DECL_CMD_HANDLER(OnDie);
	DECL_CMD_HANDLER(OnDespawn);
	DECL_CMD_HANDLER(OnGuardDefense);
	DECL_CMD_HANDLER(OnGuardDefensePartial);

	DECL_CMD_HANDLER(OnCancelNextAttack);

	DECL_CMD_HANDLER(OnStandUp);

	DECL_CMD_HANDLER(OnTestSelfMotionFactor);
	DECL_CMD_HANDLER(OnTalentDrain);

	DECL_CMD_HANDLER(OnSit);
	DECL_CMD_HANDLER(OnSitRise);
	DECL_CMD_HANDLER(OnHide);
	DECL_CMD_HANDLER(OnAFKBegin);
	DECL_CMD_HANDLER(OnAFKEnd);

	DECL_CMD_HANDLER(OnMyCombatEnd);
public:
	XCmdHandler_Action(MCommandCommunicator* pCC);
};



#endif