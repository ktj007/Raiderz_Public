#ifndef _GCMD_HANDLER_ACTION_H
#define _GCMD_HANDLER_ACTION_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Action : public MCommandHandler, public MTestMemPool<GCmdHandler_Action>
{
public:
	GCmdHandler_Action(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestMove);
	DECL_CMD_HANDLER(OnRequestStop);
	DECL_CMD_HANDLER(OnRequestMoveStop);
	DECL_CMD_HANDLER(OnRequestRotate);
	DECL_CMD_HANDLER(OnRequestPositionSync);
	DECL_CMD_HANDLER(OnRequestPositionSyncWhenFinishMovingMotionFactor);

	DECL_CMD_HANDLER(OnRequestChangeStance);
	DECL_CMD_HANDLER(OnRequestAttackMelee);
	DECL_CMD_HANDLER(OnRequestAttackArchery);
	DECL_CMD_HANDLER(OnRequestAttackArcheryDetail);
	DECL_CMD_HANDLER(OnRequestAttackGun);
	DECL_CMD_HANDLER(OnRequestAttackCancel);
	DECL_CMD_HANDLER(OnRequestAttackPrepare);
	DECL_CMD_HANDLER(OnRequestJump);
	DECL_CMD_HANDLER(OnRequestFalling);
	DECL_CMD_HANDLER(OnEndFalling);
	DECL_CMD_HANDLER(OnRequestGuard);
	DECL_CMD_HANDLER(OnRequestGuardReleased);

	DECL_CMD_HANDLER(OnRequestUseTalent);
	DECL_CMD_HANDLER(OnRequestActTalent);
	DECL_CMD_HANDLER(OnRequestActTalentWithHitcapsule);
	DECL_CMD_HANDLER(OnRequestActTalentWithGround);
	DECL_CMD_HANDLER(OnRequestCancelTalent);
	DECL_CMD_HANDLER(OnRequestStandUp);
	DECL_CMD_HANDLER(OnRequestSwitchingWeaponBegin);
	DECL_CMD_HANDLER(OnRequestSwitchingWeapon);
	DECL_CMD_HANDLER(OnRequestSwim);
	DECL_CMD_HANDLER(OnRequestUseCancelItemTalent);

	DECL_CMD_HANDLER(OnRequestSit);
	DECL_CMD_HANDLER(OnRequestSitRise);
	DECL_CMD_HANDLER(OnRequestAFK);
};

#endif//_GCMD_HANDLER_ACTION_H
