#include "stdafx.h"
#include "GMemory.h"

#include "GEntityMgr.h"
#include "GSpawnManager.h"
#include "GItem.h"

#include "GModuleCombat.h"
#include "GModuleBuff.h"
#include "GBehaviorDead.h"
#include "GBehaviorCombat.h"
#include "GBehaviorMission.h"
#include "GBehaviorPeace.h"
#include "GBehaviorReturn.h"
#include "GBehaviorSession.h"

#include "GModuleAI.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GPlayerObject.h"

#include "GNPCAssist.h"
#include "GModuleMovable.h"
#include "GNPCPathFinder.h"
#include "GHateTable.h"
#include "GNPCInteraction.h"

#include "GJob_Talent.h"
#include "GJob_MoveToEntity.h"
#include "GJob_MoveToPos.h"
#include "GJob_FleeFromEntity.h"
#include "GJob_FleeToHome.h"
#include "GJob_Patrol.h"
#include "GJob_Roam.h"
#include "GJob_Idle.h"
#include "GJobT_Script.h"
#include "GJob_Speak.h"
#include "GJobT_Flee.h"
#include "GJobT_Yell.h"
#include "GJobT_Warp.h"
#include "GJobT_GainBuff.h"
#include "GJobT_Despawn.h"
#include "GJobT_Die.h"
#include "GJobT_Assist.h"
#include "GJobT_Attack.h"
#include "GJobT_BreakPart.h"
#include "GJobT_FaceTo.h"
#include "GJob_WaitFor.h"
#include "GJobT_NextAction.h"
#include "GJobT_Summon.h"


#include "GMeleeTalent.h"
#include "GMagicTalent.h"
#include "GArcheryTalent.h"
#include "GItemTalent.h"
#include "GExtraActiveTalent.h"
#include "GSectorAIEventInvoker.h"

#include "GTrialField.h"
#include "GInnRoomField.h"
#include "GSharedField.h"
#include "GDynamicFieldMaster.h"
#include "GDynamicFieldPolicy.h"
#include "GSharedFieldMaster.h"
#include "GPickingZTable.h"

#include "GPlayerDBCache.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GCharacterSyncingCache.h"
#include "GItemSyncingCache.h"

#include "GDBTaskHeaderList.h"

GScopeMempoolReleaser::~GScopeMempoolReleaser()
{
	GEntityMgr::release();
	GSpawnManager::release();

	GExtraActiveTalent_Grapple::release();
	GExtraActiveTalent_Swallowed::release();
	GMagicTalent::release();
	GMeleeTalent::release();
	GArcheryTalent::release();
	GItemTalent::release();


	GFieldGrid::release();
	GSectorAIEventInvoker::release();
	GBehaviorPeace::release();
	GBehaviorDead::release();
	GBehaviorCombat::release();
	GBehaviorMission::release();
	GBehaviorReturn::release();
	GBehaviorSession::release();

	GModuleCombat::release();
	GModuleBuff::release();
	GEntityPlayer::release();

	GNPCAssist::release();
	GPlayerObject_Leaf::release();

	// GEntityNPC
	GEntityNPC_Leaf::release();
	GModuleAI::release();
	GModuleMovable::release();
	GNPCPathFinder::release();
	GHateTable::release();
	GNPCInteraction::release();


	// job
	GJob_UseTalent::release();
	GJob_Guard::release();
	GJob_MoveToEntity::release();
	GJob_MoveToPos::release();
	GJob_FleeFromEntity::release();
	GJob_FleeToHome::release();
	GJob_Patrol::release();
	GJob_Roam::release();
	GJob_Idle::release();
	GJobT_Script::release();
	GJob_Speak::release();
	GJobT_Flee::release();
	GJobT_Yell::release();
	GJobT_Warp::release();
	GJobT_GainBuff::release();
	GJobT_Despawn::release();
	GJobT_Die::release();
	GJobT_Assist::release();
	GJobT_Attack::release();
	GJobT_BreakPart::release();
	GJobT_FaceTo::release();
	GJob_WaitFor::release();
	GJobT_NextAction::release();
	GJobT_Summon::release();


	GItem::release();

	// field
	GSharedFieldMLeaf::release();
	GTrialFieldMLeaf::release();
	GInnRoomFieldMLeaf::release();

	GDynamicFieldMasterMLeaf::release();
	GDynamicFieldPolicyMLeaf::release();
	GSharedFieldMasterMLeaf::release();

	GPickingZTable::Cell::release();
	GPickingZTable::HeightData::release();
	GPickingZTable::HeightData2::release();
	GPickingZTable::HeightData3::release();
	GPickingZTable::HeightData4::release();
	GPickingZTable::HeightData5::release();
	GPickingZTable::HeightData6::release();
	GPickingZTable::HeightData7::release();
	GPickingZTable::HeightData8::release();
	GPickingZTable::HeightData9::release();
	GPickingZTable::HeightData10::release();

	// db cache
	GPlayerDBCache::release();
	GCharacterDBCache_Leaf::release();
	GItemDBCache_Leaf::release();
	GCharacterSyncingCache::release();
	GItemSyncingCache::release();

	// dbtask
	GDBTaskLogin::release();
	GCharSyncQuery::release();
	GItemSyncQuery::release();
	GDBTaskCharDecMoney::release();
	GDBTaskCharIncMoney::release();
	GDBTaskCharLevelUp::release();
	GDBTaskCharUpdateXP::release();
	GDBTaskCharSerialize::release();
	GDBTaskCraftInsert::release();
	GDBTaskCraftUpdate::release();
	GDBTaskEmblemDelete::release();
	GDBTaskEmblemInsert::release();
	GDBTaskGuildDelete::release();
	GDBTaskGuildInsert::release();
	GDBTaskGuildMasterChange::release();
	GDBTaskGuildMemberDelete::release();
	GDBTaskGuildMemberInsert::release();
	GDBTaskGuildSerialize::release();
	GDBTaskItemEquip::release();
	GDBTaskItemUnequip::release();
	GDBTaskItemLootInsert::release();
	GDBTaskItemLootUpdate::release();
	GDBTaskItemDecDura::release();
	GDBTaskItemDecStackAmt::release();
	GDBTaskItemDelete::release();
	GDBTaskItemIncStackAmt::release();
	GDBTaskItemInsert::release();
	GDBTaskItemLearnTalent::release();
	GDBTaskItemMerge::release();
	GDBTaskItemSwapSlot::release();
	GDBTaskItemBuyInsert::release();
	GDBTaskItemBuyUpdate::release();
	GDBTaskItemRepair::release();
	GDBTaskItemRepairAll::release();
	GDBTaskItemSellDelete::release();
	GDBTaskItemSellUpdate::release();
	GDBTaskItemSellVeryCommon::release();
	GDBTaskItemIncStackAmt::release();
	GDBTaskQuestAccept::release();
	GDBTaskQuestDone::release();
	GDBTaskQuestGiveup::release();
	GDBTaskQuestFail::release();	
	GDBTaskQuestPvPRewardInven::release();
	GDBTaskQuestPvPRewardMail::release();
	GDBTaskTalentLearn::release();
	GDBTaskTalentResetAll::release();
	GTradeSyncingCacheList::release();
	GGameDBTaskQuery::release();
	GGameDBTaskQueryIter::release();
	GLogDBTaskQuery::release();
}