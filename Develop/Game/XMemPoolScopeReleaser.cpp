#include "stdafx.h"
#include "XMemPoolScopeReleaser.h"
#include "XNonPlayer.h"
#include "XProjectile.h"
#include "XModuleAction.h"
#include "XModuleMotion.h"
#include "XModuleMovable.h"
#include "XModuleNetControl.h"
#include "XModuleBParts.h"
#include "XModuleBuff.h"
#include "XModuleCollision.h"
#include "XModuleEffect.h"
#include "XModuleMessageQueue.h"
#include "XModuleNonControl.h"
#include "XModuleEntity.h"
#include "XModuleTalent.h"
#include "XModuleUI.h"
#include "XModuleBackgroundWork.h"
#include "XMovableGoParam.h"
#include "XControlCmd.h"
#include "XSoulObject.h"
#include "XCharacterCaption.h"
#include "XSwordTrailEffectController.h"
#include "XMagicArea.h"
#include "XCutScenePlayer.h"
#include "XGuardTalent.h"
#include "XMiniHpBar.h"

XMemPoolScopeReleaser::~XMemPoolScopeReleaser()
{
	XNetPlayer::release();
	XProjectile::release();
	XSoulObject::release();

	XModuleAction::release();
	XModuleMotion::release();
	XModuleNetControl::release();
	XModuleTalent::release();
	XModuleBParts::release();
	XModuleBuff::release();
	XModuleCollision::release();
	XModuleEffect::release();
	XModuleMessageQueue::release();
	XModuleMovable::release();
	XModuleNonControl::release();
	XModuleUI::release();
	XModuleBackgroundWork::release();

	XItemTalent::release();
	XArcheryTalent::release();
	XMagicTalent::release();
	XMeleeTalent::release();
	XExtraActiveTalent::release();
	XExtraActiveTalent_Grapple::release();

	XMovableGoParam::release();
	XControlCmd::release();

	//XCharacterCaption::release();
	XDamageTextureCaption::release();
	XMiniHpBar::release();

	XSwordTrailEffectController::release();

	XMagicArea::release();

	XCutScenePlayer::release();

	XGuardTalent::release();
}
