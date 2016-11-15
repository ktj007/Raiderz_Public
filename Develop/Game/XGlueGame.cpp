#include "stdafx.h"
#include "XGlueGame.h"
#include "XMyPlayer.h"
#include "XModuleActorControl.h"
#include "XModuleMotion.h"
#include "XMotionHelper.h"
#include "XModulePost.h"
#include "XModuleBuff.h"
#include "XGame.h"
#include "XWorld.h"
#include "XStrings.h"
#include "XController.h"
#include "XItemManager.h"
#include "XGameFrameManager.h"
#include "XFactionInfo.h"
#include "XFactionInfoMgr.h"
#include "XGuildMember.h"
#include "XCharFrame.h"
#include "XPost_Item.h"
#include "XPost_Palette.h"
#include "XPost_Action.h"
#include "XPost_Quest.h"
#include "XNetwork.h"
#include "XHistory.h"
#include "XTalentInfoMgr.h"
#include "XPlayerFaction.h"
#include "CSItemHelper.h"
#include "XUICustomLook.h"
#include "XUIEquippedItemSlot.h"
#include "XInteractionInfo.h"

#include "XCommonFunction.h"
#include "XPost_Trade.h"
#include "XPost_Guild.h"
#include "XPost_Char.h"
#include "XPost_Duel.h"
#include "XPost_Comm.h"

#include "XMyActionDie.h"

#include "XPost_Interaction.h"
#include "XPost_GM.h"
#include "XPost_Sensor.h"

#include "XBattleArena.h"

#include "XNaming.h"
#include "XRecipeInfo.h"
#include "XRecipeInfoMgr.h"
#include "XBaseApplication.h"
#include "XConditionsInfoMgr.h"

#include "XNonPlayer.h"
#include "CSNPCShopCalculator.h"

#include "CSCommonParser.h"

#include "XPost_Craft.h"
#include "XPost_NPCShop.h"

#include "XItem_DroperInfoMgr.h"
#include "XItem_QuestRewarderInfoMgr.h"

#include "XPost_Channel.h"

#include "XConditionsInfo.h"
#include "XConditionStringMaker.h"
#include "XPost_Loot.h"
#include "RCameraSceneNode.h"
#include "XColorTable.h"
#include "XCameraManager.h"

#include "XMMCode.h"
#include "CSNameStringChecker.h"
#include "CCommandResultTable.h"

#include "XCampaign.h"

#include "XItemHelper.h"

#include "MLocale.h"
#include "XSoundHelper.h"
#include "XDialogTextInfoMgr.h"
#include "XItem_RecipeProductInfoMgr.h"
#include "XPost_Mail.h"
#include "XRecipeNPCInfoMgr.h"
#include "XUIFieldMap.h"
#include "XRecipeNPCInfo.h"
#include "XUIMiniMap.h"
#include "XMail.h"
#include "XSoundResMgr.h"
#include "XCommonData.h"
#include "XPlayerPVP.h"
#include "XPvPAreaLogic.h"
#include "XHelp.h"
#include "CSStringFilter.h"

#include "XGlueGame_Storage.h"
#include "XGlueGame_GuildStorage.h"

void XGlueGame::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueGame>(pLua->GetState(), "_gamefunc")

		// System info
		.def( "GetWindowSize",						&XGlueGame::GetWindowSize)
		.def( "GetWindowWidth",						&XGlueGame::GetWindowWidth)
		.def( "GetWindowHeight",					&XGlueGame::GetWindowHeight)
		.def( "ShowCursor",							&XGlueGame::ShowCursor)
		.def( "IsShowCursor",						&XGlueGame::IsShowCursor)
//		.def( "SetCursor",							&XGlueGame::SetCursor)
		.def( "GetCursorPos",						&XGlueGame::GetCursorPos)
		.def( "SetHistory",							&XGlueGame::SetHistory)
		.def( "GetHistory",							&XGlueGame::GetHistory)
		.def( "GetUpdateTime",						&XGlueGame::GetUpdateTime)
		.def( "GetVirtualKey",						&XGlueGame::GetVirtualKey)
		.def( "GetVirtualKeyStr",					&XGlueGame::GetVirtualKeyStr)
//		.def( "MouseVisible",						&XGlueGame::MouseVisible)
		.def( "GetString",							&XGlueGame::GetString)
		.def( "GetFormatStr",						&XGlueGame::GetFormatStr)
		.def( "GetResultString",					&XGlueGame::GetResultString)
		.def( "GetResultSound",						&XGlueGame::GetResultSound)
		.def( "Quit",								&XGlueGame::Quit)
		.def( "ExitToCharSelection",				&XGlueGame::ExitToCharSelection)
		.def( "IsServerModeExpo",					&XGlueGame::IsServerModeExpo)
//		.def( "GetMousePosX",						&XGlueGame::GetMousePosX)			// GetCursorPos가 있는데 이런건 왜 만듦??

		// Loading info
		.def( "GetLoadingBkgrndImage",				&XGlueGame::GetLoadingBkgrndImage)
		.def( "GetLoadingFieldDesc",				&XGlueGame::GetLoadingFieldDesc)
		.def( "GetLoadingTipMsg",					&XGlueGame::GetLoadingTipMsg)
		.def( "GetLoadingProgress",					&XGlueGame::GetLoadingProgress)

		// Character info
		.def( "GetName",							&XGlueGame::GetName)
		.def( "GetRace",							&XGlueGame::GetRace)
		.def( "GetSex",								&XGlueGame::GetSex)
		.def( "GetLevel",							&XGlueGame::GetLevel)
		.def( "GetMoney",							&XGlueGame::GetMoney)
		.def( "GetHP",								&XGlueGame::GetHP)
		.def( "GetEN",								&XGlueGame::GetEN)
		.def( "GetSTA",								&XGlueGame::GetSTA)
		.def( "GetExp",								&XGlueGame::GetExp)
		.def( "GetExpRatio",						&XGlueGame::GetExpRatio)
		.def( "GetArmorPoint",						&XGlueGame::GetArmorPoint)
		.def( "GetFatigue",							&XGlueGame::GetFatigue)
		.def( "IsMyPlayerMoving",					&XGlueGame::IsMyPlayerMoving)
		.def( "GetMyPlayerState",					&XGlueGame::GetMyPlayerState)
		.def( "SetBattleStance",					&XGlueGame::SetBattleStance)
		.def( "IsBattleStance",						&XGlueGame::IsBattleStance)
		.def( "GetRecievedQuestCount",				&XGlueGame::GetRecievedQuestCount)
		.def( "GetCompletedQuestCount",				&XGlueGame::GetCompletedQuestCount)
		.def( "GetRecievedRecipeCount",				&XGlueGame::GetRecievedRecipeCount)
		.def( "GetCompletedRecipeCount",			&XGlueGame::GetCompletedRecipeCount)
		.def( "GetKilledEnemyCount",				&XGlueGame::GetKilledEnemyCount)
		.def( "GetKilledEpicEnemyCount",			&XGlueGame::GetKilledEpicEnemyCount)
		.def( "GetCharColorCount",					&XGlueGame::GetCharColorCount)
		.def( "GetCharColorID",						&XGlueGame::GetCharColorID)
		.def( "GetCharColor",						&XGlueGame::GetCharColor)
		.def( "GetWeaponSet",						&XGlueGame::GetWeaponSet)
		.def( "IsInQuestPvpArea",					&XGlueGame::IsInQuestPvpArea)
		.def( "GetPvpTeamName",						&XGlueGame::GetPvpTeamName)

		// Character account info
		.def( "GetCharacterAccountCount",			&XGlueGame::GetCharacterAccountCount)		
		.def( "GetCharacterAccountName",			&XGlueGame::GetCharacterAccountName)
		.def( "GetCharacterAccountLevel",			&XGlueGame::GetCharacterAccountLevel)
		.def( "GetCharacterAccountLocation",		&XGlueGame::GetCharacterAccountLocation)
		.def( "GetCharacterAccountMaxCount",		&XGlueGame::GetCharacterAccountMaxCount)
		.def( "SetAccountParam",					&XGlueGame::SetAccountParam)
		.def( "GetAccountParam",					&XGlueGame::GetAccountParam)
		.def( "DeleteAccountParam",					&XGlueGame::DeleteAccountParam)

		// Character status
 		.def( "GetMaxHP",							&XGlueGame::GetMaxHP)
 		.def( "GetMaxEN",							&XGlueGame::GetMaxEN)
 		.def( "GetMaxSTA",							&XGlueGame::GetMaxSTA)
		.def( "GetSTR", 							&XGlueGame::GetSTR)
		.def( "GetDEX", 							&XGlueGame::GetDEX)
		.def( "GetINT", 							&XGlueGame::GetINT)
		.def( "GetCON", 							&XGlueGame::GetCON)
		.def( "GetCHA", 							&XGlueGame::GetCHA)

		.def( "GetFR",								&XGlueGame::GetFR)
		.def( "GetCR",								&XGlueGame::GetCR)
		.def( "GetLR",								&XGlueGame::GetLR)
		.def( "GetPR",								&XGlueGame::GetPR)
		.def( "GetHR",								&XGlueGame::GetHR)
		.def( "GetUR",								&XGlueGame::GetUR)

		// Character Modifier
		.def( "GetModSTR",							&XGlueGame::GetModSTR)
		.def( "GetModDEX",							&XGlueGame::GetModDEX)
		.def( "GetModCON",							&XGlueGame::GetModCON)
		.def( "GetModINT",							&XGlueGame::GetModINT)
		.def( "GetModCHA",							&XGlueGame::GetModCHA)

		.def( "GetModHPMax",						&XGlueGame::GetModHPMax)
		.def( "GetModENMax",						&XGlueGame::GetModENMax)
		.def( "GetModSTAMax",						&XGlueGame::GetModSTAMax)
		.def( "GetModAP",							&XGlueGame::GetModAP)

		// Regist
		.def( "GetFR",								&XGlueGame::GetFR)
		.def( "GetCR",								&XGlueGame::GetCR)
		.def( "GetLR",								&XGlueGame::GetLR)
		.def( "GetPR",								&XGlueGame::GetPR)
		.def( "GetHR",								&XGlueGame::GetHR)
		.def( "GetUR",								&XGlueGame::GetUR)

		.def( "GetModFR",							&XGlueGame::GetModFR)
		.def( "GetModCR",							&XGlueGame::GetModCR)
		.def( "GetModLR",							&XGlueGame::GetModLR)
		.def( "GetModPR",							&XGlueGame::GetModPR)
		.def( "GetModHR",							&XGlueGame::GetModHR)
		.def( "GetModUR",							&XGlueGame::GetModUR)

		// Field
		.def( "GetCurrentFieldID",					&XGlueGame::GetCurrentFieldID)
//		.def( "GetFieldPosition",					&XGlueGame::GetFieldPosition)
		.def( "GetFieldListCount",					&XGlueGame::GetFieldListCount)
		.def( "GetFieldID",							&XGlueGame::GetFieldID)
		.def( "GetFieldName",						&XGlueGame::GetFieldName)
		.def( "IsDynamicField",						&XGlueGame::IsDynamicField)
		.def( "IsShowFieldMap",						&XGlueGame::IsShowFieldMap)
		.def( "IsCurrentPvPField",					&XGlueGame::IsCurrentPvPField)
		.def( "IsCurrentTutorialField",				&XGlueGame::IsCurrentTutorialField)
		.def( "GetDayTime",							&XGlueGame::GetDayTime)
		.def( "GetWeather",							&XGlueGame::GetWeather)

		// Channel
		.def( "GetCurrentChannel",					&XGlueGame::GetCurrentChannel)
		.def( "RequestChannelList",					&XGlueGame::RequestChannelList)
		.def( "GetChannelListCount",				&XGlueGame::GetChannelListCount)
		.def( "GetChannelID",						&XGlueGame::GetChannelID)
		.def( "GetChannelState",					&XGlueGame::GetChannelState)
		.def( "MoveToChannel",						&XGlueGame::MoveToChannel)
		.def( "CancelMoveToChannel",				&XGlueGame::CancelMoveToChannel)
		.def( "LeaveDynamicField",					&XGlueGame::LeaveDynamicField)
		.def( "IsChannelChangeable",				&XGlueGame::IsChannelChangeable)
		.def( "IsCombat",							&XGlueGame::IsCombat)

		// Equipped item info
		.def( "IsEquippedItem",						&XGlueGame::IsEquippedItem)
		.def( "GetEquippedItemID",					&XGlueGame::GetEquippedItemID)
		.def( "GetEquippedItemDurability",			&XGlueGame::GetEquippedItemDurability)
		.def( "GetEquippedItemRepairCost",			&XGlueGame::GetEquippedItemRepairCost)
		.def( "GetEquippedItemEnchantedStoneID",	&XGlueGame::GetEquippedItemEnchantedStoneID)
		.def( "EquipInvenItem",						&XGlueGame::EquipInvenItem)
		.def( "UnequipItem",						&XGlueGame::UnequipItem)
		.def( "ExchangeEquippedItem",				&XGlueGame::ExchangeEquippedItem)
		.def( "GetEquipItemQuantityByID",			&XGlueGame::GetEquipItemQuantityByID)
		.def( "IsEquippedItemClaimed",				&XGlueGame::IsEquippedItemClaimed)

		// Inventory info
		.def( "GetInvenItemCount",					&XGlueGame::GetInvenItemCount)
		.def( "GetInvenItemMaxCount",				&XGlueGame::GetInvenItemMaxCount)
		.def( "GetInvenItemID",						&XGlueGame::GetInvenItemID)
		.def( "GetInvenItemQuantity",				&XGlueGame::GetInvenItemQuantity)
		.def( "GetInvenItemQuantityByID",			&XGlueGame::GetInvenItemQuantityByID)
		.def( "GetInvenItemDurability",				&XGlueGame::GetInvenItemDurability)
		.def( "GetInvenItemSellCost",				&XGlueGame::GetInvenItemSellCost)
		.def( "GetInvenItemRepairCost",				&XGlueGame::GetInvenItemRepairCost)
		.def( "GetInvenItemEnchantedStoneID",		&XGlueGame::GetInvenItemEnchantedStoneID)
		.def( "IsInvenItemEnabled",					&XGlueGame::IsInvenItemEnabled)
		.def( "IsInvenItemClaimed",					&XGlueGame::IsInvenItemClaimed)
		.def( "IsInvenNewItem",						&XGlueGame::IsInvenNewItem)		
		.def( "ClearInvenNewItemFlag",				&XGlueGame::ClearInvenNewItemFlag)		
		.def( "ClearAllInvenNewItemFlag",			&XGlueGame::ClearAllInvenNewItemFlag)		
		.def( "GetInvenItemReuseTimeRemaining",		&XGlueGame::GetInvenItemReuseTimeRemaining)
		.def( "MoveInvenItem",						&XGlueGame::MoveInvenItem)
		.def( "UseInvenItem",						&XGlueGame::UseInvenItem)
		.def( "DropInvenItem",						&XGlueGame::DropInvenItem)
		.def( "SellInvenItem",						&XGlueGame::SellInvenItem)
		.def( "SellInvenVeryCommonItem",			&XGlueGame::SellInvenVeryCommonItem)

		// Storage
		.def( "GetStorageMoney",					&XGlueGameStorage::GetStorageMoney)
		.def( "GetStorageItemCount",				&XGlueGameStorage::GetStorageItemCount)
		.def( "GetStorageItemMaxCount",				&XGlueGameStorage::GetStorageItemMaxCount)
		.def( "GetStorageItemID",					&XGlueGameStorage::GetStorageItemID)
		.def( "GetStorageItemQuantity",				&XGlueGameStorage::GetStorageItemQuantity)
		.def( "DepositMoney",						&XGlueGameStorage::DepositMoney)
		.def( "WithdrawMoney",						&XGlueGameStorage::WithdrawMoney)
		.def( "DepositItem",						&XGlueGameStorage::DepositItem)
		.def( "WithdrawItem",						&XGlueGameStorage::WithdrawItem)
		.def( "MoveStorageItem",					&XGlueGameStorage::MoveStorageItem)
		.def( "DropStorageItem",					&XGlueGameStorage::DropStorageItem)

		
		// Item Info
		.def( "GetItemName",						&XGlueGame::GetItemName)
		.def( "GetItemImage",						&XGlueGame::GetItemImage)
		.def( "GetItemDesc",						&XGlueGame::GetItemDesc)
		.def( "GetItemType",						&XGlueGame::GetItemType)
		.def( "GetItemWeaponType",					&XGlueGame::GetItemWeaponType)
		.def( "GetItemArmorType",					&XGlueGame::GetItemArmorType)
		.def( "GetItemUsableType",					&XGlueGame::GetItemUsableType)
		.def( "GetItemTier",						&XGlueGame::GetItemTier)
		.def( "GetItemQuantity",					&XGlueGame::GetItemQuantity)
		.def( "GetItemDurability",					&XGlueGame::GetItemDurability)
		.def( "GetItemBuyingCost",					&XGlueGame::GetItemBuyingCost)
		.def( "GetItemStackSize",					&XGlueGame::GetItemStackSize)
		.def( "GetItemEnchantType",					&XGlueGame::GetItemEnchantType)
		.def( "GetItemEnchantLevel",				&XGlueGame::GetItemEnchantLevel)
		.def( "GetItemEnchantHoleCount",			&XGlueGame::GetItemEnchantHoleCount)
		.def( "GetItemEnchantHoleType",				&XGlueGame::GetItemEnchantHoleType)
		.def( "GetItemEnchantAgentIDRange",			&XGlueGame::GetItemEnchantAgentIDRange)
		.def( "GetItemUnchantableWeaponTypeCount",	&XGlueGame::GetItemUnchantableWeaponTypeCount)
		.def( "GetItemUnchantableWeaponType",		&XGlueGame::GetItemUnchantableWeaponType)
		.def( "GetItemUnchantableArmorSlotCount",	&XGlueGame::GetItemUnchantableArmorSlotCount)
		.def( "GetItemUnchantableArmorSlot",		&XGlueGame::GetItemUnchantableArmorSlot)
		.def( "GetItemDropperCount",				&XGlueGame::GetItemDropperCount)
		.def( "GetItemDropperName",					&XGlueGame::GetItemDropperName)
		.def( "GetItemPutUpSound",					&XGlueGame::GetItemPutUpSound)
		.def( "GetItemPutDownSound",				&XGlueGame::GetItemPutDownSound)
		.def( "GetItemMakingSound",					&XGlueGame::GetItemMakingSound)
		.def( "GetMoneyPutUpSound",					&XGlueGame::GetMoneyPutUpSound)
		.def( "GetMoneyPutDownSound",				&XGlueGame::GetMoneyPutDownSound)
		.def( "GetItemDyeColorCount",				&XGlueGame::GetItemDyeColorCount)		
		.def( "GetItemDyeColor",					&XGlueGame::GetItemDyeColor)
		.def( "IsItemUnique",						&XGlueGame::IsItemUnique)
//		.def( "IsItemRare",							&XGlueGame::IsItemRare)
		.def( "IsItemQuestRelated",					&XGlueGame::IsItemQuestRelated)
		.def( "IsItemRepairable",					&XGlueGame::IsItemRepairable)
		.def( "IsItemTradable",						&XGlueGame::IsItemTradable)
		.def( "IsItemSellable",						&XGlueGame::IsItemSellable)
		.def( "IsItemDiscardable",					&XGlueGame::IsItemDiscardable)
		.def( "IsItemReusable",						&XGlueGame::IsItemReusable)
		.def( "IsItemRequiredClaim",				&XGlueGame::IsItemRequiredClaim)
		.def( "IsItemDyeable",						&XGlueGame::IsItemDyeable)
		.def( "IsItemRecipeMaterial",				&XGlueGame::IsItemRecipeMaterial)
// 		.def( "IsItemRecipeTooltipNeed",			&XGlueGame::IsItemRecipeTooltipNeed)		
// 		.def( "IsItemRecipeWeapon",					&XGlueGame::IsItemRecipeWeapon)
// 		.def( "IsItemRecipeShield",					&XGlueGame::IsItemRecipeShield)
// 		.def( "IsItemRecipeAccessory",				&XGlueGame::IsItemRecipeAccessory)
// 		.def( "IsItemRecipeArmor",					&XGlueGame::IsItemRecipeArmor)
// 		.def( "IsItemRecipeUsable",					&XGlueGame::IsItemRecipeUsable)
// 		.def( "IsItemRecipeHousing",				&XGlueGame::IsItemRecipeHousing)
// 		.def( "IsItemRecipeEnchant",				&XGlueGame::IsItemRecipeEnchant)
		.def( "GetItemSlot",						&XGlueGame::GetItemSlot)
		.def( "GetItemMaxDurability",				&XGlueGame::GetItemMaxDurability)
		.def( "GetItemMinDamage",					&XGlueGame::GetItemMinDamage)
		.def( "GetItemMaxDamage",					&XGlueGame::GetItemMaxDamage)
		.def( "GetItemSpellPower",					&XGlueGame::GetItemSpellPower)
		.def( "GetItemArmorPoint",					&XGlueGame::GetItemArmorPoint)
		.def( "GetItemReuseTime",					&XGlueGame::GetItemReuseTime)
		.def( "GetItemEquipReqLevel",				&XGlueGame::GetItemEquipReqLevel)
		.def( "GetItemEquipReqMaxLevel",			&XGlueGame::GetItemEquipReqMaxLevel)
		.def( "GetItemEffects",						&XGlueGame::GetItemEffects)		
		.def( "GetItemReqLicense",					&XGlueGame::GetItemReqLicense)		
		.def( "HasItemReqLicense",					&XGlueGame::HasItemReqLicense)		
// 		.def( "GetItemRecipeCount",					&XGlueGame::GetItemRecipeCount)
// 		.def( "GetItemRecipeItemID",				&XGlueGame::GetItemRecipeItemID)
// 		.def( "GetItemRecipeMinLevel",				&XGlueGame::GetItemRecipeMinLevel)
// 		.def( "GetItemRecipeMaxLevel",				&XGlueGame::GetItemRecipeMaxLevel)
//		.def( "GetItemLinkedEnchantBuffId",			&XGlueGame::GetItemLinkedEnchantBuffId)
		.def( "GetItemProductCount",				&XGlueGame::GetItemProductCount)
		.def( "GetItemProductID",					&XGlueGame::GetItemProductID)
		//SoulHunterZ - Item Attunement details
		.def( "GetItemExp",							&XGlueGame::GetItemExp)
		.def( "GetNextAttuneLevelXP",				&XGlueGame::GetReqXP)
		.def( "GetItemAttunementLevel",				&XGlueGame::GetAttuneLvl)

	
		// Talent Info
		.def( "GetTalentName",						&XGlueGame::GetTalentName)
		.def( "GetTalentImage",						&XGlueGame::GetTalentImage)
		.def( "GetTalentPutUpSound",				&XGlueGame::GetTalentPutUpSound)
		.def( "GetTalentPutDownSound",				&XGlueGame::GetTalentPutDownSound)
		.def( "DoTalentRequireFocus",				&XGlueGame::DoTalentRequireFocus)
		.def( "GetTalentRank",						&XGlueGame::GetTalentRank)
		.def( "GetTalentStyle",						&XGlueGame::GetTalentStyle)
		.def( "IsTalentFocus",						&XGlueGame::IsTalentFocus)
//		.def( "GetTalentStyleString",				&XGlueGame::GetTalentStyleString)
		.def( "GetTalentCategory",					&XGlueGame::GetTalentCategory)
		.def( "GetTalentDesc",						&XGlueGame::GetTalentDesc)
		.def( "IsTalentPassiveType",				&XGlueGame::IsTalentPassiveType)
		.def( "DoTalentRequireWeaponType",			&XGlueGame::DoTalentRequireWeaponType)
		.def( "GetTalentCostHP",					&XGlueGame::GetTalentCostHP)
		.def( "GetTalentCostEN",					&XGlueGame::GetTalentCostEN)
		.def( "GetTalentCostSP",					&XGlueGame::GetTalentCostSP)
		.def( "GetTalentCastingTime",				&XGlueGame::GetTalentCastingTime)
		.def( "GetTalentCoolTime",					&XGlueGame::GetTalentCoolTime)
		.def( "GetTalentLearnReqLevel",				&XGlueGame::GetTalentLearnReqLevel)
		.def( "GetTalentLearnReqStyleTP",			&XGlueGame::GetTalentLearnReqStyleTP)

		.def( "GetTalentModSTR",					&XGlueGame::GetTalentModSTR)
		.def( "GetTalentModDEX",					&XGlueGame::GetTalentModDEX)
		.def( "GetTalentModCON",					&XGlueGame::GetTalentModCON)
		.def( "GetTalentModINT",					&XGlueGame::GetTalentModINT)
		.def( "GetTalentModCHA",					&XGlueGame::GetTalentModCHA)

		.def( "GetTalentModHPMax",					&XGlueGame::GetTalentModHPMax)
		.def( "GetTalentModENMax",					&XGlueGame::GetTalentModENMax)
		.def( "GetTalentModSTAMax",					&XGlueGame::GetTalentModSTAMax)

		// Learned talent info
		.def( "GetTalentPoint",						&XGlueGame::GetTalentPoint)
		.def( "GetSpentTalentPoint",				&XGlueGame::GetSpentTalentPoint)
		.def( "GetSpentTalentPointAll",				&XGlueGame::GetSpentTalentPointAll)
		.def( "GetLearnedTalentCount",				&XGlueGame::GetLearnedTalentCount)
		.def( "GetLearnedTalentID",					&XGlueGame::GetLearnedTalentID)
		.def( "GetLastLearnedRankedTalentID",		&XGlueGame::GetLastLearnedRankedTalentID)
		.def( "GetEmotionalTalentCount",			&XGlueGame::GetEmotionalTalentCount)
		.def( "GetEmotionalTalentID",				&XGlueGame::GetEmotionalTalentID)
		.def( "IsLearnedTalent",					&XGlueGame::IsLearnedTalent)

		// Buff info
		.def( "GetBuffCount",						&XGlueGame::GetBuffCount)
		.def( "GetBuffID",							&XGlueGame::GetBuffID)
		.def( "GetBuffImage",						&XGlueGame::GetBuffImage)
		.def( "IsDebuff",							&XGlueGame::IsDebuff)
		.def( "GetBuffName",						&XGlueGame::GetBuffName)
//		.def( "GetBuffEffects",						&XGlueGame::GetBuffEffects)		

		// Palette item info
//		.def( "GetPaletteItemName",					&XGlueGame::GetPaletteItemName)
//		.def( "GetPaletteItemImage",				&XGlueGame::GetPaletteItemImage)
		.def( "GetPalleteItemType",					&XGlueGame::GetPalleteItemType)
		.def( "GetPalleteItemID",					&XGlueGame::GetPalleteItemID)
		.def( "SetPaletteItem",						&XGlueGame::SetPaletteItem)
		.def( "ChangePaletteSet",					&XGlueGame::ChangePaletteSet)
		.def( "GetPaletteSetNum",					&XGlueGame::GetPaletteSetNum)
		.def( "SetWeaponBindedPaletteSet",			&XGlueGame::SetWeaponBindedPaletteSet)
		.def( "GetWeaponBindedPaletteSet",			&XGlueGame::GetWeaponBindedPaletteSet)

		// Party info
		.def( "IsPartyJoined",						&XGlueGame::IsPartyJoined)
		.def( "AmIPartyLeader",						&XGlueGame::AmIPartyLeader)
		.def( "GetPartyMemberCount",				&XGlueGame::GetPartyMemberCount)
		.def( "GetPartyMemberName",					&XGlueGame::GetPartyMemberName)
		.def( "GetPartyMemberLevel",				&XGlueGame::GetPartyMemberLevel)
		.def( "GetPartyMemberHP",					&XGlueGame::GetPartyMemberHP)
		.def( "GetPartyMemberEN",					&XGlueGame::GetPartyMemberEN)
		.def( "GetPartyMemberBuffCount",			&XGlueGame::GetPartyMemberBuffCount)
		.def( "GetPartyMemberBuffID",				&XGlueGame::GetPartyMemberBuffID)
		.def( "IsPartyMemberMe",					&XGlueGame::IsPartyMemberMe)
		.def( "IsPartyMemberDead",					&XGlueGame::IsPartyMemberDead)
		.def( "IsPartyMemberOffline",				&XGlueGame::IsPartyMemberOffline)
		.def( "IsPartyMemberDisabled",				&XGlueGame::IsPartyMemberDisabled)
		.def( "IsPartyMemberHightlight",			&XGlueGame::IsPartyMemberHightlight)	
		.def( "IsPartyLeader",						&XGlueGame::IsPartyLeader)
		.def( "DisbandPartyMember",					&XGlueGame::DisbandPartyMember)
		.def( "LeaveParty",							&XGlueGame::LeaveParty)

		// Party Setting
		.def( "GetLootingRuleCommon",				&XGlueGame::GetLootingRuleCommon)
		.def( "GetLootingRuleRare",					&XGlueGame::GetLootingRuleRare)
		.def( "GetLootingRuleRareFor",				&XGlueGame::GetLootingRuleRareFor)
		.def( "ChangePartySetting",					&XGlueGame::ChangePartySetting)
		.def( "IsAutoParty",						&XGlueGame::IsAutoParty)
		.def( "GetAutoPartyQuestID",				&XGlueGame::GetAutoPartyQuestID)
		.def( "GetAutoPartyState",					&XGlueGame::GetAutoPartyState)
		.def( "SetAutoPartyState",					&XGlueGame::SetAutoPartyState)

		// Guild info
		.def( "GetGuildName",						&XGlueGame::GetGuildName)
		.def( "GetGuildImage",						&XGlueGame::GetGuildImage)
		.def( "AmIGuildLeader",						&XGlueGame::AmIGuildLeader)
		.def( "GetGuildMemberCount",				&XGlueGame::GetGuildMemberCount)
		.def( "GetGuildMemberName",					&XGlueGame::GetGuildMemberName)
		.def( "GetGuildMemberGrade",				&XGlueGame::GetGuildMemberGrade)
		.def( "IsGuildMemberOffline",				&XGlueGame::IsGuildMemberOffline)
		.def( "CreateGuild",						&XGlueGame::CreateGuild)
		.def( "DelegateGuildLeader",				&XGlueGame::DelegateGuildLeader)
		.def( "DisbandGuildMember",					&XGlueGame::DisbandGuildMember)
		.def( "LeaveGuild",							&XGlueGame::LeaveGuild)
		.def( "DestroyGuild",						&XGlueGame::DestroyGuild)		
		.def( "IsValidateGuildName",				&XGlueGame::IsValidateGuildName)		
		.def( "GetGuildNameLengthKor",				&XGlueGame::GetGuildNameLengthKor)		
		.def( "GetGuildNameLengthEng",				&XGlueGame::GetGuildNameLengthEng)

		// Guild Storage
		.def( "GetGuildStorageMoney",				&XGlueGameGuildStorage::GetGuildStorageMoney)		
		.def( "GetGuildStorageItemCount",			&XGlueGameGuildStorage::GetGuildStorageItemCount)
		.def( "GetGuildStorageItemMaxCount",		&XGlueGameGuildStorage::GetGuildStorageItemMaxCount)
		.def( "GetGuildStorageItemID",				&XGlueGameGuildStorage::GetGuildStorageItemID)
		.def( "GetGuildStorageItemQuantity",		&XGlueGameGuildStorage::GetGuildStorageItemQuantity)
		.def( "DepositGuildMoney",					&XGlueGameGuildStorage::DepositGuildMoney)		
		.def( "WithdrawGuildMoney",					&XGlueGameGuildStorage::WithdrawGuildMoney)		
		.def( "DepositGuildItem",					&XGlueGameGuildStorage::DepositGuildItem)
		.def( "WithdrawGuildItem",					&XGlueGameGuildStorage::WithdrawGuildItem)
		.def( "MoveGuildStorageItem",				&XGlueGameGuildStorage::MoveGuildStorageItem)
		//.def( "DropGuildStorageItem",				&XGlueGameGuildStorage::DropGuildStorageItem)


		// Quest info
		.def( "GetPlayerQuestCount",				&XGlueGame::GetPlayerQuestCount)
		.def( "GetPlayerQuestID",					&XGlueGame::GetPlayerQuestID)
		.def( "GetPlayerQuestObjectiveProgress",	&XGlueGame::GetPlayerQuestObjectiveProgress)
		.def( "IsPlayerQuestFailed",				&XGlueGame::IsPlayerQuestFailed)
		.def( "IsPlayerQuestCompleted",				&XGlueGame::IsPlayerQuestCompleted)
		.def( "IsPlayerQuestObjectiveCompleted",	&XGlueGame::IsPlayerQuestObjectiveCompleted)
		.def( "IsPlayerChallengerQuest",			&XGlueGame::IsPlayerChallengerQuest)
		
		.def( "GetQuestName",						&XGlueGame::GetQuestName)
		.def( "GetQuestDesc",						&XGlueGame::GetQuestDesc)
		.def( "GetQuestCategory",					&XGlueGame::GetQuestCategory)
		.def( "GetQuestObjectiveCount",				&XGlueGame::GetQuestObjectiveCount)
		.def( "GetQuestObjectiveType",				&XGlueGame::GetQuestObjectiveType)
		.def( "GetQuestObjectiveText",				&XGlueGame::GetQuestObjectiveText)
		.def( "GetQuestObjectiveProgress",			&XGlueGame::GetQuestObjectiveProgress)
		.def( "GetQuestObjectiveIndex",				&XGlueGame::GetQuestObjectiveIndex)
		.def( "GetQuestRewardCount",				&XGlueGame::GetQuestRewardCount)
		.def( "GetQuestRewardType",					&XGlueGame::GetQuestRewardType)
		.def( "GetQuestRewardValue",				&XGlueGame::GetQuestRewardValue)
		.def( "GetQuestRewardAmount",				&XGlueGame::GetQuestRewardAmount)
		.def( "GetQuestSelectableRewardCount",		&XGlueGame::GetQuestSelectableRewardCount)
		.def( "GetQuestSelectableRewardType",		&XGlueGame::GetQuestSelectableRewardType)
		.def( "GetQuestSelectableRewardValue",		&XGlueGame::GetQuestSelectableRewardValue)
		.def( "GetQuestSelectableRewardAmount",		&XGlueGame::GetQuestSelectableRewardAmount)
		.def( "IsQuestShareable",					&XGlueGame::IsQuestShareable)
		.def( "IndicateQuest",						&XGlueGame::IndicateQuest)
		.def( "ShareQuest",							&XGlueGame::ShareQuest)
		.def( "AcceptShareQuest",					&XGlueGame::AcceptShareQuest)
		.def( "GiveUpQuest",						&XGlueGame::GiveUpQuest)
		.def( "IsAutoPartyQuest",					&XGlueGame::IsAutoPartyQuest)

		// Faction
		.def( "GetFactionCount",					&XGlueGame::GetFactionCount)
		.def( "GetFactionID",						&XGlueGame::GetFactionID)
		.def( "GetFactionName",						&XGlueGame::GetFactionName)
		.def( "GetFactionValue",					&XGlueGame::GetFactionValue)
		.def( "GetFactionDesc",						&XGlueGame::GetFactionDesc)

		// Shop
		.def( "GetShopItemCount",					&XGlueGame::GetShopItemCount)
		.def( "GetShopItemID",						&XGlueGame::GetShopItemID)
		.def( "IsBuyableShopItem",					&XGlueGame::IsBuyableShopItem)
		.def( "IsUsableShopItem",					&XGlueGame::IsUsableShopItem)
		.def( "GetBuyDiscountRatio",				&XGlueGame::GetBuyDiscountRatio)
		.def( "GetSellDiscountRatio",				&XGlueGame::GetSellDiscountRatio)
		.def( "RepairAllItem",						&XGlueGame::RepairAllItem)
		.def( "RepairInvenItem",					&XGlueGame::RepairInvenItem)
		.def( "RepairEquippedItem",					&XGlueGame::RepairEquippedItem)
		.def( "BuyItem",							&XGlueGame::BuyItem)
		.def( "IsRepairableShop",					&XGlueGame::IsRepairableShop)

		// Journal indicator
		.def( "GetJournalCount",					&XGlueGame::GetJournalCount)
		.def( "GetJournalType",						&XGlueGame::GetJournalType)
		.def( "GetJournalID",						&XGlueGame::GetJournalID)
		.def( "AddJournal",							&XGlueGame::AddJournal)
		.def( "RemoveJournal",						&XGlueGame::RemoveJournal)
		.def( "IsJournalIndicated",					&XGlueGame::IsJournalIndicated)

		// Trade
		.def( "CancelTrade",						&XGlueGame::CancelTrade)
		.def( "GetMaxTradableItemCount",			&XGlueGame::GetMaxTradableItemCount)
		.def( "GetMyTradeItemCount",				&XGlueGame::GetMyTradeItemCount)
		.def( "GetMyTradeItemID",					&XGlueGame::GetMyTradeItemID)
		.def( "GetMyTradeItemQuantity",				&XGlueGame::GetMyTradeItemQuantity)
		.def( "GetMyTradeMoney",					&XGlueGame::GetMyTradeMoney)
		.def( "GetOtherTradeItemCount",				&XGlueGame::GetOtherTradeItemCount)
		.def( "GetOtherTradeItemID",				&XGlueGame::GetOtherTradeItemID)
		.def( "GetOtherTradeItemQuantity",			&XGlueGame::GetOtherTradeItemQuantity)
		.def( "GetOtherTradeMoney",					&XGlueGame::GetOtherTradeMoney)
		.def( "PutUpTradeItem",						&XGlueGame::PutUpTradeItem)
		.def( "PutDownTradeItem",					&XGlueGame::PutDownTradeItem)
		.def( "PutUpMoney",							&XGlueGame::PutUpMoney)
		.def( "PutDownMoney",						&XGlueGame::PutDownMoney)
		.def( "ReadyToTrade",						&XGlueGame::ReadyToTrade)
		.def( "IsReadyToTrade",						&XGlueGame::IsReadyToTrade)
		.def( "IsOtherReadyToTrade",				&XGlueGame::IsOtherReadyToTrade)

		// Change campaign scene
		.def( "ChangeCampaignScene",				&XGlueGame::ChangeCampaignScene)

		// Login
		.def( "IsDirectLogin",						&XGlueGame::IsDirectLogin)
		.def( "IsAlreadyDirectLogined",				&XGlueGame::IsAlreadyDirectLogined)
		.def( "WaitDirectLogin",					&XGlueGame::WaitDirectLogin)
		.def( "GetServerCount",						&XGlueGame::GetServerCount)
		.def( "GetServerName",						&XGlueGame::GetServerName)
		.def( "GetServerAddress",					&XGlueGame::GetServerAddress)
		.def( "GetServerState",						&XGlueGame::GetServerState)
		.def( "GetServerActive",					&XGlueGame::GetServerActive)
		.def( "GetServerType",						&XGlueGame::GetServerType)
		.def( "LoginConnect",						&XGlueGame::LoginConnect)
		.def( "LoginDisconnect",					&XGlueGame::LoginDisconnect)

		// Character
		.def( "SelChangedCharacter",				&XGlueGame::SelChangedCharacter)
		.def( "SelectCharacter",					&XGlueGame::SelectCharacter)
		.def( "DeleteCharacter",					&XGlueGame::DeleteCharacter)
		.def( "CreateNewCharacter",					&XGlueGame::CreateNewCharacter)
		.def( "ChangeNewCharacterModel",			&XGlueGame::ChangeNewCharacterModel)
		.def( "IsValidateCharName",					&XGlueGame::IsValidateCharName)
		.def( "GetCharacterNameLengthKor",			&XGlueGame::GetCharacterNameLengthKor)
		.def( "GetCharacterNameLengthEng",			&XGlueGame::GetCharacterNameLengthEng)


		// Looting
		.def( "GetLootingItemCount",				&XGlueGame::GetLootingItemCount)
		.def( "GetLootingItemID",					&XGlueGame::GetLootingItemID)
		.def( "GetLootingItemType",					&XGlueGame::GetLootingItemType)
		.def( "GetLootingItemQuantity",				&XGlueGame::GetLootingItemQuantity)
// 		.def( "IsGettableLootingItem",				&XGlueGame::IsGettableLootingItem)
// 		.def( "IsRollableLootingItem",				&XGlueGame::IsRollableLootingItem)
// 		.def( "IsAssignableLootingItem",			&XGlueGame::IsMasterLootableLootingItem)
		.def( "LootItem",							&XGlueGame::LootItem)
		.def( "LootAllItems",						&XGlueGame::LootAllItems)
		.def( "RequestLootingEnd",					&XGlueGame::RequestLootingEnd)

		// Assign looting
// 		.def( "GetItemCountLootingLooter",			&XGlueGame::GetItemCountLootingLooter)
// 		.def( "GetItemIDLootingLooter",				&XGlueGame::GetItemIDLootingLooter)
// 		.def( "GetItemnQuantityLootingLooter",		&XGlueGame::GetItemnQuantityLootingLooter)
// 		.def( "ReqAssignItem",						&XGlueGame::ReqAssignItem)
		.def( "GetAssignLootingCount",				&XGlueGame::GetAssignLootingCount)
		.def( "GetAssignLootingID",					&XGlueGame::GetAssignLootingID)
		.def( "GetAssignLootingQuantity",			&XGlueGame::GetAssignLootingQuantity)
		.def( "GetLooterCount",						&XGlueGame::GetLooterCount)
		.def( "GetLooterName",						&XGlueGame::GetLooterName)
		.def( "DoAssignLooting",					&XGlueGame::DoAssignLooting)

		// Accept quest
		.def( "AcceptQuest",						&XGlueGame::AcceptQuest)
		.def( "RejectQuest",						&XGlueGame::RejectQuest)

		// Quest Reward Dialog
		.def( "AcceptQuestRewardItem",				&XGlueGame::AcceptQuestRewardItem)
		.def( "RejectQuestRewardItem",				&XGlueGame::RejectQuestRewardItem)

		// Enemy Info
		.def( "GetEnemyCount",						&XGlueGame::GetEnemyCount)
		.def( "GetEnemyID",							&XGlueGame::GetEnemyID)
		.def( "GetEnemyName",						&XGlueGame::GetEnemyName)
		.def( "GetEnemyLevel",						&XGlueGame::GetEnemyLevel)
		.def( "GetEnemyGrade",						&XGlueGame::GetEnemyGrade)
		.def( "GetEnemyHP",							&XGlueGame::GetEnemyHP)
		.def( "GetEnemyMaxHP",						&XGlueGame::GetEnemyMaxHP)
		.def( "IsShowEnemyInfo",					&XGlueGame::IsShowEnemyInfo)

		// Target Info
		.def( "IsShowTargetInfo",					&XGlueGame::IsShowTargetInfo)
		.def( "GetTargetID",						&XGlueGame::GetTargetID)
		.def( "GetTargetName",						&XGlueGame::GetTargetName)
		.def( "GetTargetLevel",						&XGlueGame::GetTargetLevel)
		.def( "GetTargetGrade",						&XGlueGame::GetTargetGrade)
		.def( "GetTargetHP",						&XGlueGame::GetTargetHP)
		.def( "GetTargetMaxHP",						&XGlueGame::GetTargetMaxHP)
		.def( "GetTargetInteractType",				&XGlueGame::GetTargetInteractType)
		.def( "GetTargetAttackableType",			&XGlueGame::GetTargetAttackableType)
		.def( "GetTargetMessage",					&XGlueGame::GetTargetMessage)
		.def( "IsLockOnTarget",						&XGlueGame::_IsLockOnTarget)
		.def( "GetDisplayKeyString",				&XGlueGame::GetDisplayKeyString)

		// Mail box
		.def( "GetMailCount",						&XGlueGame::GetMailCount)
		.def( "IsExistUnreadMail",					&XGlueGame::IsExistUnreadMail)
		.def( "IsMailBoxFull",						&XGlueGame::IsMailBoxFull)
		.def( "GetMailSender",						&XGlueGame::GetMailSender)
		.def( "GetMailTitle",						&XGlueGame::GetMailTitle)
		.def( "GetMailContent",						&XGlueGame::GetMailContent)
		.def( "GetMailRemaindDay",					&XGlueGame::GetMailRemaindDay)
		.def( "IsMailRead",							&XGlueGame::IsMailRead)
		.def( "GetMailAttachedMoney",				&XGlueGame::GetMailAttachedMoney)
		.def( "GetMailAttachedThumbItemID",			&XGlueGame::GetMailAttachedThumbItemID)
		.def( "GetMailAttachedItemCount",			&XGlueGame::GetMailAttachedItemCount)
		.def( "GetMailAttachedItemID",				&XGlueGame::GetMailAttachedItemID)
		.def( "GetMailAttachedItemQuantity",		&XGlueGame::GetMailAttachedItemQuantity)
		.def( "GetMailAttachedItemIndex",			&XGlueGame::GetMailAttachedItemIndex)
		.def( "AttachItemToSendMail",				&XGlueGame::AttachItemToSendMail)
		.def( "DetachItemToSendMail",				&XGlueGame::DetachItemToSendMail)
		.def( "ClearSendMailAttachedItems",			&XGlueGame::ClearSendMailAttachedItems)
		.def( "ReadMail",							&XGlueGame::ReadMail)
		.def( "SendMail",							&XGlueGame::SendMail)
		.def( "DeleteMail",							&XGlueGame::DeleteMail)
		.def( "TakeMailMoney",						&XGlueGame::TakeMailMoney)
		.def( "TakeMailItem",						&XGlueGame::TakeMailItem)
		.def( "GetMailErrorMsg",					&XGlueGame::GetMailErrorMsg)

		// Rebirth
		.def( "GetRebirthRemainTime",				&XGlueGame::GetRebirthRemainTime)
		.def( "RequireRebirth",						&XGlueGame::RequireRebirth)
		.def( "GetSoulBindFieldID",					&XGlueGame::GetSoulBindFieldID)
		.def( "GetCheckPointFieldID",				&XGlueGame::GetCheckPointFieldID)
		.def( "GetRebirthItemID",					&XGlueGame::GetRebirthItemID)

		// Interaction with PC
		.def( "GetInteractionPlayerName",			&XGlueGame::GetInteractionPlayerName)
		.def( "IsInteractionPlayerPartyLeader",		&XGlueGame::IsInteractionPlayerPartyLeader)
		.def( "InviteToParty",						&XGlueGame::InviteToParty)
		.def( "InviteToGuild",						&XGlueGame::InviteToGuild)
		.def( "RequestToTrade",						&XGlueGame::RequestToTrade)
		.def( "ChallengeToDuel",					&XGlueGame::ChallengeToDuel)
		.def( "ChallengeToPartyDuel",				&XGlueGame::ChallengeToPartyDuel)
		.def( "GetRequesterName",					&XGlueGame::GetRequesterName)
		.def( "ConfirmInviteToParty",				&XGlueGame::ConfirmInviteToParty)
		.def( "ConfirmInviteToGuild",				&XGlueGame::ConfirmInviteToGuild)
		.def( "ConfirmRequestToTrade",				&XGlueGame::ConfirmRequestToTrade)
		.def( "ConfirmChallengeToDuel",				&XGlueGame::ConfirmChallengeToDuel)
		
		// Interaction with NPC
		.def( "GetNpcName",							&XGlueGame::GetNpcName)
		.def( "GetNpcWelcomeMsg",					&XGlueGame::GetNpcWelcomeMsg)
		.def( "GetCountNpcAnswer",					&XGlueGame::GetCountNpcAnswer)
		.def( "GetNpcAnswer",						&XGlueGame::GetNpcAnswer)
		.def( "SelectNpcAnswer",					&XGlueGame::SelectNpcAnswer)
		.def( "RequestNpcInteractionEnd",			&XGlueGame::RequestNpcInteractionEnd)

		// Dialog with NPC
		.def( "GetNpcDialogMessage",				&XGlueGame::GetNpcDialogMessage)
		.def( "GetCountNpcDialogAnswer",			&XGlueGame::GetCountNpcDialogAnswer)
		.def( "GetNpcDialogAnswer",					&XGlueGame::GetNpcDialogAnswer)
		.def( "SelectNpcDialogAnswer",				&XGlueGame::SelectNpcDialogAnswer)

		// Message
		.def( "Narration",							&XGlueGame::Narration )

		// 투기장
		.def( "IsBattleArena",						&XGlueGame::IsBattleArena)
		.def( "GetCurrentBattleArenaScore",			&XGlueGame::GetCurrentBattleArenaScore)
		
		// Craft
		.def( "GetCraftingRecipeCount",				&XGlueGame::GetCraftingRecipeCount)
		.def( "GetCraftingRecipeID",				&XGlueGame::GetCraftingRecipeID)
 		.def( "GetCraftingItemCost",				&XGlueGame::GetCraftingItemCost)
		.def( "ObtainCraftingRecipe",				&XGlueGame::ObtainCraftingRecipe)
		.def( "MakeCraftingItem",					&XGlueGame::MakeCraftingItem)
		.def( "IsEnabledCraftingItem",				&XGlueGame::IsEnabledCraftingItem)
		.def( "IsMakableCraftingItem",				&XGlueGame::IsMakableCraftingItem)

		// Recipe
		.def( "GetRecipeCount",						&XGlueGame::GetRecipeCount)
		.def( "GetRecipeID",						&XGlueGame::GetRecipeID)
		.def( "GetRecipeItemID",					&XGlueGame::GetRecipeItemID)
		.def( "GetRecipeItemCost",					&XGlueGame::GetRecipeItemCost)
		.def( "GetRecipeItemQuantity",				&XGlueGame::GetRecipeItemQuantity)
		.def( "GetRecipeItemName",					&XGlueGame::GetRecipeItemName)		
		.def( "GetRecipeMaterialCount",				&XGlueGame::GetRecipeMaterialCount)
		.def( "GetRecipeMaterialID",				&XGlueGame::GetRecipeMaterialID)
		.def( "GetRecipeMaterialQuantity",			&XGlueGame::GetRecipeMaterialQuantity)
		.def( "GetRecipeConditionText",				&XGlueGame::GetRecipeConditionText)
		.def( "GetRecipeArtisanCount",				&XGlueGame::GetRecipeArtisanCount)		
		.def( "GetRecipeArtisanName",				&XGlueGame::GetRecipeArtisanName)
		.def( "GetRecipeArtisanFieldID",			&XGlueGame::GetRecipeArtisanFieldID)
		.def( "RemoveRecipe",						&XGlueGame::RemoveRecipe)
		.def( "IsHaveRecipe",						&XGlueGame::IsHaveRecipe)


		.def( "PlaySound",							&XGlueGame::PlaySound)
		.def( "PlayBGM",							&XGlueGame::PlayBGM)
		.def( "CheckBGM",							&XGlueGame::CheckBGM)
		.def( "ReleaseHoldMyCharacter",				&XGlueGame::ReleaseHoldMyCharacter)
		.def( "OutputGuideMsg",						&XGlueGame::OutputGuideMsg)
		.def( "ReleaseGuideMsg",					&XGlueGame::ReleaseGuideMsg)

		//날씨
		.def("ChangeWeather",						&XGlueGame::ChangeWeather)
		.def("ChangeCurrentWeather",				&XGlueGame::ChangeCurrentWeather)
		.def("ChangeCustomEnv",						&XGlueGame::ChangeCustomEnv)
		.def("ChangeCustomTimeWeather",				&XGlueGame::ChangeCustomTimeWeather)
		.def("ChangeCustomWeather",					&XGlueGame::ChangeCustomWeather)
		.def("ChangeCustomTime",					&XGlueGame::ChangeCustomTime)

		// Challanger Quest
		.def( "GetChallengerQuestCount",			&XGlueGame::GetChallengerQuestCount)
		.def( "GetChallengerQuestID",				&XGlueGame::GetChallengerQuestID)
		.def( "GetChallengerQuestEnableType",		&XGlueGame::GetChallengerQuestEnableType)
		.def( "GetChallengerQuestConditionText",	&XGlueGame::GetChallengerQuestConditionText)

		// Enchant
		.def( "OpenEnchantItem",					&XGlueGame::OpenEnchantItem)
		.def( "BeginEnchantItem",					&XGlueGame::BeginEnchantItem)
		.def( "CancelEnchantItem",					&XGlueGame::CancelEnchantItem)
		.def( "GetEnchantSuccessPercent",			&XGlueGame::GetEnchantSuccessPercent)
		.def( "DoEnchantingItem",					&XGlueGame::DoEnchantingItem)
// 		.def( "EnchantCancel",						&XGlueGame::EnchantCancel)		
// 		.def( "EnchantStart",						&XGlueGame::EnchantStart)		
// 		.def( "EnchantUseInvenItem",				&XGlueGame::EnchantUseInvenItem)
// 		.def( "EnchantSelectItem",					&XGlueGame::EnchantSelectItem)
// 		.def( "GetEnchantMaxCount",					&XGlueGame::GetEnchantMaxCount)
// 		.def( "GetEnchantItemColor",				&XGlueGame::GetEnchantItemColor)
// 		.def( "GetEnchantStoneItemColor",			&XGlueGame::GetEnchantStoneItemColor)
// 		.def( "GetEnchantItemBuffID",				&XGlueGame::GetEnchantItemBuffID)
// 		.def( "GetEnchantItemBuffItemID",			&XGlueGame::GetEnchantItemBuffItemID)
// 		.def( "GetEnchantSuccessPercent",			&XGlueGame::GetEnchantSuccessPercent)
// 		.def( "GetEnchantRequirAgentItemName",		&XGlueGame::GetEnchantRequirAgentItemName)
// 		.def( "GetEnchantRequirAgentItemID",		&XGlueGame::GetEnchantRequirAgentItemID)
// 		.def( "GetEnchantRequirLimiteLevel",		&XGlueGame::GetEnchantRequirLimiteLevel)
// 		.def( "GetEnchantAgentItemUseLevel",		&XGlueGame::GetEnchantAgentItemUseLevel)
// 		.def( "GetEnchantItemPosition",				&XGlueGame::GetEnchantItemPosition)
// 		.def( "GetEnchantAgentAutoPush",			&XGlueGame::GetEnchantAgentAutoPush)
// 		.def( "GetEnchantCount",					&XGlueGame::GetEnchantCount)
// 		.def( "CheckEnchantItemType",				&XGlueGame::CheckEnchantItemType)
// 		.def( "CheckEnchantItemColor",				&XGlueGame::CheckEnchantItemColor)
// 		.def( "CheckEnchantItemReq",				&XGlueGame::CheckEnchantItemReq)

		//SoulHunterZ - Item Attunement System
		//.def( "OpenAttunementItem"					&XGlueGame::OpenAttuneItem)
		//.def( "BeginAttunementItem"					&XGlueGame::BeginAttuneItem)
		//.def( "CancelAttunementItem"				&XGlueGame::CancelAttuneItem)
		//.def( "GetAttunementSuccessPercent"			&XGlueGame::GetAttuneSuccessPercent)
		//.def( "DoAttuneItem"						&XGlueGame::DoAttuneItem)

		// Dye
		.def( "OpenDyeItem",						&XGlueGame::OpenDyeItem)
		.def( "BeginDyeItem",						&XGlueGame::BeginDyeItem)
		.def( "CancelDyeItem",						&XGlueGame::CancelDyeItem)
		.def( "DoDyeingItem",						&XGlueGame::DoDyeingItem)
// 		.def( "UseDyeItem",							&XGlueGame::UseDyeItem)
// 		.def( "CancelDyeItem",						&XGlueGame::CancelDyeItem)
//		.def( "GetDyeColor",						&XGlueGame::GetDyeColor)

		// IsDirectLoginToLoginFrame
		.def( "IsDirectLoginToLoginFrame",			&XGlueGame::IsDirectLoginToLoginFrame )
		// help
		.def( "HelpTrigger",						&XGlueGame::HelpTrigger )
		.def( "HelpTriggerString",					&XGlueGame::HelpTriggerString )
		.def( "GetHelpImg",							&XGlueGame::GetHelpImg )
		.def( "GetHelpTimer",						&XGlueGame::GetHelpTimer )
		.def( "GetHelpMsg",							&XGlueGame::GetHelpMsg )
		.def( "GetEventTrigger",					&XGlueGame::GetEventTrigger )
		.def( "FinishedHelp",						&XGlueGame::FinishedHelp )
		.def( "LockCameraToNPC",					&XGlueGame::LockCameraToNPC )


		// Cross Hair
		.def( "GetCurrentCrossHairName",			&XGlueGame::GetCurrentCrossHairName)
		.def( "GetCurrentCrossHairPickName",		&XGlueGame::GetCurrentCrossHairPickName)
		.def( "GetCurrentCrossHairAwaysName",		&XGlueGame::GetCurrentCrossHairAwaysName)


		// String
		.def( "Format",								&XGlueGame::Format)
		;
}


MWLua::ret_int2 XGlueGame::GetWindowSize()
{
	int w = MGetMint()->GetWorkspaceWidth();
	int h = MGetMint()->GetWorkspaceHeight();

	return MWLua::ret_int2( w, h);
}


int XGlueGame::GetWindowWidth()
{
	return MGetMint()->GetWorkspaceWidth();
}


int XGlueGame::GetWindowHeight()
{
	return MGetMint()->GetWorkspaceHeight();
}


void XGlueGame::ShowCursor( bool bShow)
{
	if ( global.ui == NULL  ||  global.ui->IsMouseVisible() == bShow)
		return;

	gg.controller->ClearKeyBuffer( true);

	global.ui->ShowMouse( bShow);	
}

// void XGlueGame::SetCursor(const char* szCursorName)
// {
// 	if( szCursorName == NULL) return;
// 
//	MCursorSystem::Set(szCursorName);
//}

bool XGlueGame::IsShowCursor()
{
	return global.ui->IsMouseVisible();	
}

void XGlueGame::SetHistory( const char* szKey, const char* szValue )
{
	if ( global.history)	global.history->SetValue( MLocale::ConvAnsiToUTF16(szKey).c_str(), MLocale::ConvAnsiToUTF16(szValue).c_str());
}


const char* XGlueGame::GetHistory( const char* szKey)
{
	if ( global.history) 
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(global.history->GetValue( MLocale::ConvAnsiToUTF16(szKey).c_str()).c_str());
		
		return gvar.Temp.szTemp.c_str();
	}

	return "";
}


const char* XGlueGame::GetLoadingBkgrndImage( int nFieldID /*= 0*/ )
{
	static const char __NAME[ MAX_NAME_LEN] = "_img_loading_bkgrnd";
	static const char __PATH[ MAX_PATH_NAME_LEN] = "Data/Interface/game/resource/zoneillust";

	MBitmap* pImage = MBitmapManager::Get( __NAME);
	if ( pImage != NULL)	MBitmapManager::Delete( pImage->GetName());

	char _buff[ MAX_PATH_NAME_LEN];
	sprintf_s( _buff, "%s/%05d.jpg", __PATH, nFieldID);
	pImage = MGetMint()->NewBitmap( __NAME, _buff);
	if (pImage->CreateTexture() == false)
	{
		SAFE_DELETE(pImage);

		sprintf_s( _buff, "%s/%05d.jpg", __PATH, 0);
		pImage = MGetMint()->NewBitmap( __NAME, _buff);
		if (pImage->CreateTexture() == false)
		{
			SAFE_DELETE(pImage);
			return "";
		}
	}

	MBitmapManager::Add( pImage);
	return pImage->GetName();
}


const char* XGlueGame::GetLoadingFieldDesc()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(global.ui->GetFieldDesc().c_str());
	return gvar.Temp.szTemp.c_str();
}


const char* XGlueGame::GetLoadingTipMsg()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(global.ui->GetRandomTip().c_str());
	return gvar.Temp.szTemp.c_str();
}


float XGlueGame::GetLoadingProgress()
{
	if ( gg.currentgamestate  &&  gg.currentgamestate->GetWorld())
	{
		RWorldSceneNode* pWorldSceneNode = gg.currentgamestate->GetWorld()->GetWorldSceneNode();
		if ( pWorldSceneNode)		return pWorldSceneNode->GetPercent();
	}
	return 0.0f;
}


int XGlueGame::GetRace()
{
//	return (int)gvar.MyInfo.ChrInfo.nRace;				// race 값이 없음 (-1)
	return 0;
}


int XGlueGame::GetSex()
{
	return (int)gvar.MyInfo.ChrInfo.nSex;
}


bool XGlueGame::IsMyPlayerMoving()
{
	if ( gvar.Game.pMyPlayer == NULL)		return false;

	XModuleMyControl* pMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pMyControl == NULL)				return false;
	if ( pMyControl->IsMoving() == false)	return false;
	if ( pMyControl->IsAutoRun() == true)	return false;
	return true;
}

MWLua::ret_int4 XGlueGame::GetMyPlayerState()
{
	int nSwap = 1, nJump = 1, nMoving = 1, nDodge = 1;
	if ( gvar.Game.pMyPlayer != NULL)
	{
		XBuffAttribute attrBuff = gvar.Game.pMyPlayer->GetModuleBuff()->GetBuffAttribute();
		if ( attrBuff.IsMesmerizable() == true)
		{
			nDodge = 0;
		}
		//else if ( attrBuff.IsTotemable() == true)
		//{
		//	nSwap = 0;
		//	nJump = 0;
		//	nDodge = 0;
		//}
		else
		{
			//if ( attrBuff.IsDisabledMelee() == true)		nMelee = 0;
			//if ( attrBuff.IsDisabledMagic() == true)		nMagic = 0;
			if ( ( attrBuff.GetMoveSpeedRatio() - TALENT_REQUIRED_MOVE_TRUNCATED) < 0.7 /*_talent_info.m_fRequireMoveSpeed : 하드 코딩으로...*/)
				nDodge = 0;
		}
	}
	return MWLua::ret_int4( nSwap, nJump, nMoving, nDodge);
}

void XGlueGame::SetBattleStance( bool bSet)
{
	CHAR_STANCE _stance = bSet ? CS_BATTLE : CS_NORMAL;
	if ( gvar.Game.pMyPlayer != NULL  &&  gvar.Game.pMyPlayer->GetStance() != _stance)
		gvar.Game.pMyPlayer->ChangeWeapon( _stance, true);
}


bool XGlueGame::IsBattleStance()
{
	return ( gvar.Game.pMyPlayer != NULL  &&  gvar.Game.pMyPlayer->GetStance() == CS_BATTLE)  ?  true : false;
}


int XGlueGame::GetRecievedQuestCount()
{
	return gvar.MyInfo.nRecievedQuest;
}


int XGlueGame::GetCompletedQuestCount()
{
	return gvar.MyInfo.nCompletedQuest;
}


int XGlueGame::GetRecievedRecipeCount()
{
	return gvar.MyInfo.nRecievedRecipe;
}


int XGlueGame::GetCompletedRecipeCount()
{
	return gvar.MyInfo.nCompletedRecipe;
}


int XGlueGame::GetKilledEnemyCount()
{
	return gvar.MyInfo.nKilledEnemy;
}


int XGlueGame::GetKilledEpicEnemyCount()
{
	return gvar.MyInfo.nKilledEpicEnemy;
}


int XGlueGame::GetCharColorCount( const char* szPart)
{
	XColorGroup* pColorGroup = info.colorTable->GetGroup( MLocale::ConvAnsiToUTF16(szPart).c_str());
	if ( pColorGroup == NULL)		return 0;
	return (int)pColorGroup->GetSize();
}


int XGlueGame::GetCharColorID( const char* szPart, int i)
{
	XColorGroup* pColorGroup = info.colorTable->GetGroup( MLocale::ConvAnsiToUTF16(szPart).c_str());
	if ( pColorGroup == NULL)		return -1;
	return pColorGroup->GetID( i);
}


MWLua::ret_int4 XGlueGame::GetCharColor( const char* szPart, int id)
{
	XColorGroup* pColorGroup = info.colorTable->GetGroup( MLocale::ConvAnsiToUTF16(szPart).c_str());
	if ( pColorGroup == NULL)		return MWLua::ret_int4( 0, 0, 0, 0);
	MCOLOR col = pColorGroup->GetColor( id);
	return MWLua::ret_int4( col.r, col.g, col.b, col.a);
}


int XGlueGame::GetWeaponSet()
{
	return gvar.MyInfo.EquipmentSlot.GetWeaponSet();
}


bool XGlueGame::IsInQuestPvpArea()
{
	XPvPAreaLogic* pPvPAreaLogic = gg.currentgamestate->GetPvPArea();
	if ( pPvPAreaLogic != NULL  &&  pPvPAreaLogic->IsInPvPArea() == true)	return true;
	return false;
}

const char* XGlueGame::GetPvpTeamName()
{
	gvar.Temp.szTemp = "";
	if ( gg.currentgamestate  &&  gvar.Game.pMyPlayer != NULL)
	{
		XPvPAreaLogic* pPvPAreaLogic = gg.currentgamestate->GetPvPArea();
		if ( pPvPAreaLogic)
		{
			wstring strName = pPvPAreaLogic->GetTeamName( gvar.Game.pMyPlayer->GetPlayerPVP()->GetPvPAreaTeam());
			gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( strName);
		}
	}
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetCharacterAccountLocation( int i)
{	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(info.field->GetFieldName( gvar.Char.m_CharInfos[ i].nFieldID));
	return gvar.Temp.szTemp.c_str();
}


bool XGlueGame::SetAccountParam( const char* _name, const char* _type, const char* _param)
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return false;

	wstring __name =  MLocale::ConvAnsiToUTF16( _name);
	wstring __type =  MLocale::ConvAnsiToUTF16( _type);
	wstring __param = MLocale::ConvAnsiToUTF16( _param);
	return pAccount->SetParameter( __name, __type, __param);
}


const char* XGlueGame::GetAccountParam( const char* _name, const char* _type)
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return NULL;

	wstring __name =  MLocale::ConvAnsiToUTF16( _name);
	wstring __type =  MLocale::ConvAnsiToUTF16( _type);

	const wchar_t* _param = pAccount->GetParameter( __name, __type);
	if ( _param == NULL)		return NULL;

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( _param);
	return gvar.Temp.szTemp.c_str();
}


bool XGlueGame::DeleteAccountParam( const char* _name)
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return false;

	wstring __name =  MLocale::ConvAnsiToUTF16( _name);
	return pAccount->DeleteParameter( __name);
}


bool XGlueGame::IsEquippedItem( int nItemID )
{
	return gvar.MyInfo.EquipmentSlot.IsEquipItem(nItemID);
}

int XGlueGame::GetEquippedItemID( int nSlot)
{
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nSlot);
	if ( pItem == NULL)		return 0;
	return pItem->m_pItemData->m_nID;
}


int XGlueGame::GetEquippedItemDurability( int nSlot)
{
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nSlot);
	if ( pItem == NULL)		return 0;
	return pItem->m_nDurability;
}


int XGlueGame::GetEquippedItemRepairCost( int nSlot)
{
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nSlot);
	if ( pItem == NULL)		return 0;

	int nDurability = pItem->m_nDurability;
	int nMaxDurability = pItem->m_pItemData->m_nMaxDurability;
	return CSNPCShopCalculator::CalcRepairPrice( nMaxDurability - nDurability, pItem->m_pItemData->m_nRepairGrade, 0);
}


int XGlueGame::GetEquippedItemEnchantedStoneID( int nSlot, int i)
{
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nSlot);
	if ( pItem == NULL)			return 0;

	XItemData* pItemData = info.item->GetItemData( pItem->m_pItemData->m_nID);
	if ( pItemData == NULL)		return 0;

	int _index = 0, _count = -1, _size = pItemData->m_vecEquipEnchantColor.size();
	for ( _index = 0;  _index < _size;  _index++)
	{
		if ( pItemData->m_vecEquipEnchantColor[ _index] > ENCHANT_NONE)		_count++;
		if ( _count == i)		break;
	}
	if ( _count == -1)			return 0;
	return pItem->m_nEnchants[ _index];
}

void XGlueGame::EquipInvenItem( int nSlot, int nInvenIndex)
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nInvenIndex);
	if (pItem == NULL)		return;
	if (false == CSItemHelper::IsEquipableSlot((SH_ITEM_SLOT)nSlot, pItem->m_pItemData)) return;	

	if ( gvar.Game.pMyPlayer == NULL  ||  gvar.Game.pMyPlayer->IsUsableBreakablePartsWeapon())	return;
	XPostItem_EquipItem(SLOTTYPE_INVENTORY, nInvenIndex, (SH_ITEM_SLOT)nSlot);
}


void XGlueGame::UnequipItem( int nSlot)
{
	if ( gvar.Game.pMyPlayer == NULL  ||  gvar.Game.pMyPlayer->IsUsableBreakablePartsWeapon())	return;
	XPostItem_Unequip( (SH_ITEM_SLOT)nSlot);
}


void XGlueGame::ExchangeEquippedItem( int nSlot1, int nSlot2)
{
	if ( gvar.Game.pMyPlayer == NULL  ||  gvar.Game.pMyPlayer->IsUsableBreakablePartsWeapon())	return;
	XPostItem_EquipItem(SLOTTYPE_EQUIP, nSlot1, (SH_ITEM_SLOT)nSlot2);
}

int XGlueGame::GetEquipItemQuantityByID( int nID)
{
	return	gvar.MyInfo.EquipmentSlot.GetItemQuantityByID(nID);
}

bool XGlueGame::IsEquippedItemClaimed(int nSlot)
{
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nSlot);
	if ( pItem == NULL)		return false;

	return pItem->m_bClaimed;
}

int XGlueGame::GetInvenItemID( int nSlotID ) const
{
	XItem* pItem =gvar.MyInfo.Inventory.GetItem( nSlotID);
	if(pItem == NULL) return 0;

	return pItem->m_pItemData->m_nID;
}

int XGlueGame::GetInvenItemQuantity( int nSlotID ) const
{
	XItem* pItem =gvar.MyInfo.Inventory.GetItem( nSlotID);
	if(pItem == NULL) return 0;

	return pItem->m_nAmount;
}

int XGlueGame::GetInvenItemQuantityByID(int nItemID)
{
	return gvar.MyInfo.Inventory.GetItemAmountByID(nItemID);
}

int XGlueGame::GetInvenItemSellCost( int nSlotID )
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem == NULL) return 0;

	XNonPlayer* pPlayer = gg.omgr->FindNPC(XGetInteractionInfo().InteractionTargetUID);
	if (pPlayer == NULL) return 0;

	FACTION_RELATION_TYPE nFRT = gvar.MyInfo.FactionInfo.GetRelation(pPlayer->GetInfo(true)->m_nFactionLossID);

	return CSNPCShopCalculator::CalcSellPrice( pItem->m_pItemData->m_nBaseSellingPrice, gvar.Game.NPCShopInfo.GetSellMode(), nFRT);
}

int XGlueGame::GetInvenItemDurability( int nSlotID ) const
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem == NULL) return 0;
	return pItem->m_nDurability;
}

int XGlueGame::GetInvenItemRepairCost( int nSlotID )
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem == NULL) return 0;
	int nMaxDurability = pItem->m_pItemData->m_nMaxDurability;
	int nDurability = pItem->m_nDurability;
	return CSNPCShopCalculator::CalcRepairPrice( nMaxDurability-nDurability, pItem->m_pItemData->m_nRepairGrade, 0);
}


int XGlueGame::GetInvenItemEnchantedStoneID( int nItemIndex, int i)
{
	XItem* pItem = pItem = gvar.MyInfo.Inventory.GetItem( nItemIndex);
	if ( pItem == NULL)			return 0;

	XItemData* pItemData = info.item->GetItemData( pItem->m_pItemData->m_nID);
	if ( pItemData == NULL)		return 0;

	int _index = 0, _count = -1, _size = pItemData->m_vecEquipEnchantColor.size();
	for ( _index = 0;  _index < _size;  _index++)
	{
		if ( pItemData->m_vecEquipEnchantColor[ _index] > ENCHANT_NONE)		_count++;
		if ( _count == i)		break;
	}
	if ( _count == -1)			return 0;
	return pItem->m_nEnchants[ _index];
}


bool XGlueGame::IsInvenItemEnabled( int nSlotID) const
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem == NULL) return false;
	return XItemHelper::IsAvailable( pItem->m_pItemData->m_nID);
}


bool XGlueGame::IsInvenItemClaimed( int nSlotID) const
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem == NULL) return false;
	return pItem->m_bClaimed;
}


bool XGlueGame::IsInvenNewItem( int nSlotID ) const
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if (pItem == NULL) return false;

	return pItem->m_bNewItem;
}


void XGlueGame::ClearAllInvenNewItemFlag()
{
	gvar.MyInfo.Inventory.ClearAllNewItemFlag();
}


void XGlueGame::ClearInvenNewItemFlag( int nSlotID )
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if (pItem == NULL) return;

	pItem->m_bNewItem = false;
}


int XGlueGame::GetInvenItemReuseTimeRemaining(int nSlotID ) const
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if (pItem == NULL) return false;

	return XItemHelper::GetInvenItemReuseTimeRemaining(pItem->m_pItemData->m_nID);
}

void XGlueGame::MoveInvenItem( int nSrcSlotID, int nTarSlotID)
{
	if (nSrcSlotID == nTarSlotID) return;

	XItem* pSrcItem = gvar.MyInfo.Inventory.GetItem( nSrcSlotID);
	if ( pSrcItem == NULL)		return;

	int nAmount = pSrcItem->m_nAmount;

	int nTarItemID = 0;
	XItem* pTarItem = gvar.MyInfo.Inventory.GetItem( nTarSlotID);
	if ( pTarItem)
	{
		nTarItemID = pTarItem->GetID();
		if (pSrcItem->GetID() == pTarItem->GetID())
		{
			if (pTarItem->m_pItemData->m_nStackAmount < pTarItem->m_nAmount + nAmount)
			{
				nAmount = pTarItem->m_pItemData->m_nStackAmount - pTarItem->m_nAmount;
			}
		}
	}

	XPostItem_MoveItem(SLOTTYPE_INVENTORY, nSrcSlotID, pSrcItem->GetID(), SLOTTYPE_INVENTORY, nTarSlotID, nTarItemID,  nAmount);	
}

void XGlueGame::UseInvenItem( int nSlotID )
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem == NULL)					return;

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pModuleMyControl == NULL)		return;

	pModuleMyControl->DoActionUseItem( nSlotID);
}

void XGlueGame::DropInvenItem( int nSlotID )
{
	if ( gvar.Game.pMyPlayer == NULL  ||  gvar.Game.pMyPlayer->IsUsableBreakablePartsWeapon())	return;

	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem)		XPostItem_DropItem(SLOTTYPE_INVENTORY, pItem->m_nSlotID, pItem->m_nAmount);
}

void XGlueGame::SellInvenItem( int nSlotID, int nCount )
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem == NULL)		return;

	XPostNPCShop_Sell(pItem->m_nSlotID, nCount);
}

void XGlueGame::SellInvenVeryCommonItem()
{
	XPostNPCShop_SellVeryCommon();
}


int XGlueGame::GetPalleteItemType( int index)
{
	if ( gvar.MyInfo.PaletteList.m_vContains[ index].talentID > 0)		return 1;
	else if ( gvar.MyInfo.PaletteList.m_vContains[ index].itemID > 0)	return 2;
	return 0;
}


int XGlueGame::GetPalleteItemID( int index)
{
	if ( gvar.MyInfo.PaletteList.m_vContains[ index].talentID > 0)		return gvar.MyInfo.PaletteList.m_vContains[ index].talentID;
	else if ( gvar.MyInfo.PaletteList.m_vContains[ index].itemID > 0)	return gvar.MyInfo.PaletteList.m_vContains[ index].itemID;
	return 0;
}


MWLua::ret_int2 XGlueGame::SetPaletteItem( int index, int nType, int nID)
{
	MWLua::ret_int2 _RetVal( 0, 0);

	if (gvar.MyInfo.PaletteList.IsLock_PaletteSlot())
		return _RetVal; // locked 

	if ( gvar.MyInfo.PaletteList.m_vContains[ index].talentID > 0)
	{
		_RetVal.val1 = 1;
		_RetVal.val2 = gvar.MyInfo.PaletteList.m_vContains[ index].talentID;
	}
	else if ( gvar.MyInfo.PaletteList.m_vContains[ index].itemID > 0)
	{
		_RetVal.val1 = 2;
		_RetVal.val2 = gvar.MyInfo.PaletteList.m_vContains[ index].itemID;
	}


	// Remove
	if ( nType == 0  ||  nID == 0)
	{
		int nSlotNum = (int)( index / PALETTESLOT_MAX);
		int nSlotIndex = index % PALETTESLOT_MAX;
		XPostPalette_PutDown( (PALETTE_NUM)nSlotNum, (PALETTE_SLOT)nSlotIndex);
	}

	// add
	else
	{
		PALETTE_ITEM_TYPE _type;
		switch ( nType)
		{
		case 1 :	_type = PIT_TALENT;			break;
		case 2 :	_type = PIT_ITEM;			break;
		default :	_type = PIT_NONE;			break;
		}

		int nSlotNum = (int)( index / PALETTESLOT_MAX);
		int nSlotIndex = index % PALETTESLOT_MAX;
		XPostPalette_PutUp( (PALETTE_NUM)nSlotNum, (PALETTE_SLOT)nSlotIndex, _type, nID);
	}

	return _RetVal;
}


void XGlueGame::ChangePaletteSet( int nSet)
{
	if (gvar.MyInfo.PaletteList.IsLock_PaletteList())
		return; // locked 

	PALETTE_NUM _num;
	if ( nSet < 0)			_num = PALETTENUM_1;
	else if ( nSet > 4)		_num = PALETTENUM_5;
	else					_num = (PALETTE_NUM)nSet;
	gvar.MyInfo.PaletteList.SetCurPaletteListIndex( _num);
	XPostPalette_Select( _num);
}


int XGlueGame::GetPaletteSetNum()
{
	return (int)gvar.MyInfo.PaletteList.GetCurPaletteListIndex();
}


void XGlueGame::SetWeaponBindedPaletteSet( int nSet)
{
	if (gvar.MyInfo.PaletteList.IsLock_PaletteList())
		return; // locked 

	int _num = 2;
	if ( nSet == 1)			_num = 0;
	else if ( nSet == 2)	_num = 1;
	gvar.MyInfo.PaletteList.Binding( _num);
}


int XGlueGame::GetWeaponBindedPaletteSet()
{
	if ( gvar.MyInfo.PaletteList.BindingIndex( 0) == true)			return 1;
	else if ( gvar.MyInfo.PaletteList.BindingIndex( 1) == true)		return 2;
	return 0;
}


bool XGlueGame::AmIPartyLeader()
{
	return gvar.MyInfo.Party.IsLeader();
}

bool XGlueGame::IsPartyJoined()
{
	return gvar.MyInfo.Party.IsJoined();
}

const char* XGlueGame::GetPartyMemberName( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pPartyMember->m_strName.c_str());
	
	return gvar.Temp.szTemp.c_str();
}


int XGlueGame::GetPartyMemberLevel( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return 0;
	return pPartyMember->nLevel;
}


int XGlueGame::GetPartyMemberHP( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return 0;
	return pPartyMember->nHP;
}

int XGlueGame::GetPartyMemberEN( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return 0;
	return pPartyMember->nEN;
}


int XGlueGame::GetPartyMemberBuffCount( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return 0;
	return (int)pPartyMember->nBuffIDList.size();
}


int XGlueGame::GetPartyMemberBuffID( int i, int buff)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return 0;
	return pPartyMember->nBuffIDList[ buff];
}


bool XGlueGame::IsPartyMemberMe( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return false;
	return (pPartyMember->m_uid == gvar.MyInfo.MyUID ? true : false);
}


bool XGlueGame::IsPartyMemberDead( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return false;
	return pPartyMember->IsDead();
}


bool XGlueGame::IsPartyMemberOffline( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return false;
	return pPartyMember->IsOffline();
}


bool XGlueGame::IsPartyMemberDisabled( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return false;
	if ( pPartyMember->IsDead()  ||  pPartyMember->IsOffline())		return true;
	return false;
}


bool XGlueGame::IsPartyMemberHightlight( int i )
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return false;
	return pPartyMember->IsHightLight();
}


bool XGlueGame::IsPartyLeader( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return false;
	if ( pPartyMember->m_uid == gvar.MyInfo.Party.GetLeader())	return true;
	return false;
}

void XGlueGame::DisbandPartyMember( int i)
{
	XPartyMember* pPartyMember = NULL;
	if ( gvar.MyInfo.Party.GetIndex( i, &pPartyMember) == false  ||  pPartyMember == NULL)	return;
	XPostPartyRequestKick( pPartyMember->m_uid);
}


void XGlueGame::LeaveParty()
{
	XPostPartyRequestLeave();
}

int XGlueGame::GetLootingRuleCommon()
{
	PARTY_SETTING* partysetting = gvar.MyInfo.Party.GetPartySetting();
	return partysetting->m_lootingRuleData.m_nLRC;
}

int XGlueGame::GetLootingRuleRare()
{
	PARTY_SETTING* partysetting = gvar.MyInfo.Party.GetPartySetting();
	return partysetting->m_lootingRuleData.m_nLRR;
}

int XGlueGame::GetLootingRuleRareFor()
{
	PARTY_SETTING* partysetting = gvar.MyInfo.Party.GetPartySetting();
	return partysetting->m_lootingRuleData.m_nLRRF;
}

void XGlueGame::ChangePartySetting(int nCommonRule, int nRareRule, int nRareRuleFor, int nLeaderIndex)
{
	PARTY_SETTING* pPartySetting = gvar.MyInfo.Party.GetPartySetting();

	// 파티명이 달라졌으면 변경 요청
	//if (0 != wcscmp(pPartySetting->m_szName, MLocale::ConvAnsiToUTF16(szPartyName).c_str()))
	//{
	//	XPostPartyChangeName(MLocale::ConvAnsiToUTF16(szPartyName).c_str());
	//}

	// 파티장이 달라졌으면 변경 요청
	XPartyMember* pMember = NULL;
	if (gvar.MyInfo.Party.GetIndex(nLeaderIndex, &pMember))	
	{
		if (pMember != NULL)
		{
			if (pPartySetting->m_uidLeader != pMember->m_uid)
			{
				XPostPartyChangeLeader(pMember->m_uid);
			}
		}
	}

	// 루팅룰이 달라졌으면 변경 요청
	LOOTING_RULE_DATA		rule;
	rule.m_nLRC = (LOOTING_RULE_COMMON)nCommonRule;
	rule.m_nLRR = (LOOTING_RULE_RARE)nRareRule;
	rule.m_nLRRF = (LOOTING_RULE_RARE_FOR)nRareRuleFor;

	if (pPartySetting->m_lootingRuleData != rule)
	{
		XPostPartyChangeLootingRule(rule);
	}
}

bool XGlueGame::IsAutoParty()
{
	PARTY_SETTING* pPartySetting = gvar.MyInfo.Party.GetPartySetting();
	return pPartySetting->m_nAutoPartyQuestID > 0 ? true : false;
}

int XGlueGame::GetAutoPartyQuestID()
{
	PARTY_SETTING* pPartySetting = gvar.MyInfo.Party.GetPartySetting();
	return pPartySetting->m_nAutoPartyQuestID;
}

int XGlueGame::GetAutoPartyState()
{
	return gvar.MyInfo.Party.GetPartyState();
}

//파티가 아닐때는 그냥 설정을 혼자 가지고 있다가.하는 걸로
void XGlueGame::SetAutoPartyState(int nState)
{
	PARTY_SETTING* pPartySetting = gvar.MyInfo.Party.GetPartySetting();
	int nQuestID = pPartySetting->m_nAutoPartyQuestID;
	gvar.MyInfo.Party.SetPartyState(nQuestID, nState);

	if (IsPartyJoined() == true && AmIPartyLeader() == true)
		XPostRequestAutoPartyChangeState(nQuestID, nState);
}

bool XGlueGame::AmIGuildLeader()
{
	for ( MAP_GUILD_MEMBER::iterator itr = gvar.MyInfo.Guild.m_mapMember.begin();  itr != gvar.MyInfo.Guild.m_mapMember.end();  itr++)
	{
		if ( (*itr).second->m_nGrade != GMG_MASTER)			continue;
		if ( (*itr).second->m_strName == gvar.MyInfo.ChrInfo.szName)		return true;
		return false;
	}
	return false;
}


const char* XGlueGame::GetGuildMemberName( int i)
{
	XGuildMember* pGuildMember = NULL;
	if ( gvar.MyInfo.Guild.GetIndex( i, &pGuildMember) == false  ||  pGuildMember== NULL)	return "";
	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pGuildMember->m_strName.c_str());

	return gvar.Temp.szTemp.c_str();
}


const char* XGlueGame::GetGuildMemberGrade( int i)
{
	XGuildMember* pGuildMember = NULL;
	if ( gvar.MyInfo.Guild.GetIndex( i, &pGuildMember) == false  ||  pGuildMember== NULL)	return "";
	switch ( pGuildMember->m_nGrade)
	{
	case GMG_MASTER :		return "master";
		// to do...
	}
	return "";
}


bool XGlueGame::IsGuildMemberOffline( int i)
{
	XGuildMember* pGuildMember = NULL;
	if ( gvar.MyInfo.Guild.GetIndex( i, &pGuildMember) == false  ||  pGuildMember== NULL)	return "";
	return (!pGuildMember->m_bOnline);
}


void XGlueGame::CreateGuild( const char* _guildname)
{
	wstring __guildname =  MLocale::ConvAnsiToUTF16( _guildname);

	XPostGuild_Create(__guildname.c_str());
}


void XGlueGame::DelegateGuildLeader( int i)
{
	XGuildMember* pGuildMember = NULL;
	if ( gvar.MyInfo.Guild.GetIndex( i, &pGuildMember) == false  ||  pGuildMember== NULL)	return;

	XPostGuild_ChangeMaster(pGuildMember->m_strName.c_str());
}


void XGlueGame::DisbandGuildMember( int i)
{
	XGuildMember* pGuildMember = NULL;
	if ( gvar.MyInfo.Guild.GetIndex( i, &pGuildMember) == false  ||  pGuildMember== NULL)	return;
	
	XPostGuild_Kick(pGuildMember->m_strName.c_str());
}


void XGlueGame::LeaveGuild()
{
	XPostGuild_Leave();
}


void XGlueGame::DestroyGuild()
{
	XPostGuild_Destroy();
}

bool XGlueGame::IsValidateGuildName( const char* szName)
{
	CSNameStringChecker checker(MLocale::ConvAnsiToUTF16(szName).c_str(), CSNameStringChecker::TYPE_GUILD);
	CCommandResultTable nResult = (CCommandResultTable)checker.Check();
	if (CR_SUCCESS == nResult)	return true;
	return false;
}

MWLua::ret_int2 XGlueGame::GetGuildNameLengthKor()
{
	return MWLua::ret_int2(CSNameStringChecker::GUILD_MIN_HANGUL, CSNameStringChecker::GUILD_MAX_HANGUL);
}

MWLua::ret_int2 XGlueGame::GetGuildNameLengthEng()
{
	return MWLua::ret_int2(CSNameStringChecker::GUILD_MIN_ENGLISH, CSNameStringChecker::GUILD_MAX_ENGLISH);
}

int XGlueGame::GetPlayerQuestID( int i)
{
	XPlayerQuest* pQuest = NULL;
	if ( gvar.MyInfo.Quests.GetIndex( i, &pQuest) == false  ||  pQuest == NULL)	return 0;
	return pQuest->m_pQuestInfo->nID;
}


int XGlueGame::GetPlayerQuestObjectiveProgress( int id, int nObjectIndex)
{
	XPlayerQuest* pQuest = gvar.MyInfo.Quests.Get( id);
	if ( pQuest == NULL)	return 0;

	XQObjectiveInfo* pQuestObjectiveInfo = NULL;
	if ( pQuest->m_pQuestInfo->GetIndex( nObjectIndex, &pQuestObjectiveInfo) == false)		return 0;

	XPlayerQObjective* pQuestObjective = pQuest->Get( pQuestObjectiveInfo->nID);
	if ( pQuestObjective == NULL)		return 0;

	return pQuestObjective->nProgress;
}


bool XGlueGame::IsPlayerQuestFailed( int id)
{
	XPlayerQuest* pQuest = gvar.MyInfo.Quests.Get( id);
	if ( pQuest == NULL)	return false;

	return pQuest->IsFail();
}


bool XGlueGame::IsPlayerQuestCompleted( int id )
{
	XPlayerQuest* pQuest = gvar.MyInfo.Quests.Get( id);
	if ( pQuest == NULL)	return false;

	return pQuest->IsComplete();
}

bool XGlueGame::IsPlayerQuestObjectiveCompleted( int id, int nObjectIndex)
{
	XPlayerQuest* pQuest = gvar.MyInfo.Quests.Get( id);
	if ( pQuest == NULL)	return false;

	XQObjectiveInfo* pQuestObjectiveInfo = NULL;
	if ( pQuest->m_pQuestInfo->GetIndex( nObjectIndex, &pQuestObjectiveInfo) == false)		return false;

	XPlayerQObjective* pQuestObjective = pQuest->Get( pQuestObjectiveInfo->nID);
	if ( pQuestObjective == NULL)		return false;

	return pQuestObjective->bComplete;
}

bool XGlueGame::IsPlayerChallengerQuest( int id)
{
	XPlayerQuest* pQuest = gvar.MyInfo.Quests.Get( id);
	if ( pQuest == NULL)	return false;

	return pQuest->bChallengerQuest;
}

const char* XGlueGame::GetQuestName( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return "";

	if (gvar.Debug.bShowID)
	{
		wchar_t szBuff[128] = {0,};
		swprintf_s(szBuff, L"%s(ID : %d) ", pQuest->GetTitle(), id);
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(szBuff);
	}
	else
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pQuest->GetTitle());
	}
	
	return gvar.Temp.szTemp.c_str();
}


const char* XGlueGame::GetQuestDesc( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return "";

	mmcode::MControlCodeTransResult result = 
		XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(pQuest->GetDesc(), XGetMyUID());
	gvar.Temp.szTemp = result.m_strOutput;

	return gvar.Temp.szTemp.c_str();
}


const char* XGlueGame::GetQuestCategory( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return "";
	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pQuest->GetCategory());
	
	return gvar.Temp.szTemp.c_str();
}


int XGlueGame::GetQuestObjectiveCount( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;
	return pQuest->GetQObejctiveInfoCount();
}


int XGlueGame::GetQuestObjectiveType( int id, int nObjectIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;

	XQObjectiveInfo* pQuestObjectiveInfo = NULL;
	if ( pQuest->GetIndex( nObjectIndex, &pQuestObjectiveInfo) == false)	return 0;
	return (int)pQuestObjectiveInfo->nType;
}


const char* XGlueGame::GetQuestObjectiveText( int id, int nObjectIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return "";

	XQObjectiveInfo* pQuestObjectiveInfo = NULL;
	if ( pQuest->GetIndex( nObjectIndex, &pQuestObjectiveInfo) == false)	return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pQuestObjectiveInfo->GetDesc());

	return gvar.Temp.szTemp.c_str();
}


int	XGlueGame::GetQuestObjectiveProgress( int id, int nObjectIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;

	XQObjectiveInfo* pQuestObjectiveInfo = NULL;
	if ( pQuest->GetIndex( nObjectIndex, &pQuestObjectiveInfo) == false)	return 0;

	return pQuestObjectiveInfo->nParam2;
}


int XGlueGame::GetQuestRewardCount( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;
	return (int)pQuest->vecReward.size();
}


int XGlueGame::GetQuestRewardType( int id, int nRewardIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;
	return (int)pQuest->vecReward[ nRewardIndex].nType;
}


int XGlueGame::GetQuestRewardValue( int id, int nRewardIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return -1;

	QUEST_REWARD_TYPE qrt = pQuest->vecReward[ nRewardIndex].nType;
	switch ( qrt)
	{
	case QRT_MONEY:		return -1;
	case QRT_XP :		return -1;
	case QRT_ITEM :		return pQuest->vecReward[ nRewardIndex].nParam1;
	case QRT_FACTION :	return pQuest->vecReward[ nRewardIndex].nParam1;
	}
	return -1;
}


int XGlueGame::GetQuestRewardAmount( int id, int nRewardIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;

	QUEST_REWARD_TYPE qrt = pQuest->vecReward[ nRewardIndex].nType;
	switch ( qrt)
	{
	case QRT_MONEY:		return pQuest->vecReward[ nRewardIndex].nParam1;
	case QRT_XP :		return pQuest->vecReward[ nRewardIndex].nParam1;
	case QRT_ITEM :		return pQuest->vecReward[ nRewardIndex].nParam2;
	case QRT_FACTION :	return pQuest->vecReward[ nRewardIndex].nParam2;
	}
	return 0;
}

int XGlueGame::GetQuestSelectableRewardCount( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;
	return (int)pQuest->vecSelectableReward.size();
}


int XGlueGame::GetQuestSelectableRewardType( int id, int nRewardIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;
	return (int)pQuest->vecSelectableReward[ nRewardIndex].nType;
}


int XGlueGame::GetQuestSelectableRewardValue( int id, int nRewardIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return -1;

	QUEST_REWARD_TYPE qrt = pQuest->vecSelectableReward[ nRewardIndex].nType;
	switch ( qrt)
	{
	case QRT_MONEY:		return -1;
	case QRT_XP :		return -1;
	case QRT_ITEM :		return pQuest->vecSelectableReward[ nRewardIndex].nParam1;
	case QRT_FACTION :	return pQuest->vecSelectableReward[ nRewardIndex].nParam1;
	}
	return -1;
}


int XGlueGame::GetQuestSelectableRewardAmount( int id, int nRewardIndex)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest == NULL)	return 0;

	QUEST_REWARD_TYPE qrt = pQuest->vecSelectableReward[ nRewardIndex].nType;
	switch ( qrt)
	{
	case QRT_MONEY:		return pQuest->vecSelectableReward[ nRewardIndex].nParam1;
	case QRT_XP :		return pQuest->vecSelectableReward[ nRewardIndex].nParam1;
	case QRT_ITEM :		return pQuest->vecSelectableReward[ nRewardIndex].nParam2;
	case QRT_FACTION :	return pQuest->vecSelectableReward[ nRewardIndex].nParam2;
	}
	return 0;
}

bool XGlueGame::IsQuestShareable( int nQuestID)
{
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return false;

	return QSHAT_NONE != pQuestInfo->nShareType;
}

void XGlueGame::IndicateQuest( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
//	if ( pQuest)	;
}


void XGlueGame::ShareQuest( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest)	XPostQuest_RequestShare( pQuest->nID);
}

void XGlueGame::AcceptShareQuest( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest)	XPostQuest_RequestShareGive( id);
}

void XGlueGame::GiveUpQuest( int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if ( pQuest)	XPostQuest_RequestCancel( pQuest->nID);
}

bool XGlueGame::IsAutoPartyQuest(int id)
{
	XQuestInfo* pQuest = info.quest->Get( id);
	if (pQuest)
		return pQuest->IsAutoParty();

	return false;
}

int XGlueGame::GetFactionCount() const
{
	return gvar.MyInfo.FactionInfo.GetSize();
}

int XGlueGame::GetFactionID( int i)
{
	XPlayerFaction* pFaction = NULL;
	if ( gvar.MyInfo.FactionInfo.GetIndex( i, &pFaction) == false)		return 0;

	return pFaction->m_nID;
}

const char* XGlueGame::GetFactionName( int nFactionID)
{
	XFactionInfo* pFactionInfo = info.faction->Get( nFactionID);
	if ( pFactionInfo == NULL)		return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pFactionInfo->GetTitle());

	return gvar.Temp.szTemp.c_str();
}


int XGlueGame::GetFactionValue( int i)
{
	XPlayerFaction* pFaction = NULL;
	if ( gvar.MyInfo.FactionInfo.GetIndex( i, &pFaction) == false)		return 0;
	return (int)pFaction->GetQuantity();
}

const char* XGlueGame::GetFactionDesc(int i)
{
	XPlayerFaction* pFaction = NULL;
	if ( gvar.MyInfo.FactionInfo.GetIndex( i, &pFaction) == false)		return "";

	XFactionInfo* pFactionInfo = info.faction->Get( pFaction->m_nID);
	if ( pFactionInfo == NULL)		return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pFactionInfo->GetDesc());

	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetShopItemCount() const
{
	return (int)gvar.Game.NPCShopInfo.GetItemCount();
}

int XGlueGame::GetShopItemID( int i) const
{
	return gvar.Game.NPCShopInfo.GetItemID( i);
}

bool XGlueGame::IsBuyableShopItem( int nID) const
{
	return gvar.Game.NPCShopInfo.IsBuyable( nID);
}

bool XGlueGame::IsUsableShopItem( int nID) const
{
	return gvar.Game.NPCShopInfo.IsUsable( nID);
}

float XGlueGame::GetBuyDiscountRatio()
{
	return gvar.Game.NPCShopInfo.GetBuyMode();
}

float XGlueGame::GetSellDiscountRatio()
{
	return gvar.Game.NPCShopInfo.GetSellMode();
}

void XGlueGame::RepairAllItem()
{
	XPostNPCShop_RepairAll();
}


void XGlueGame::RepairInvenItem( int i)
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( i);
	if ( pItem == NULL)		return;

	if ( pItem->m_pItemData != NULL &&
		pItem->m_pItemData->m_nMaxDurability != 0 &&
		pItem->m_nDurability < pItem->m_pItemData->m_nMaxDurability)
	{
		XPostNPCShop_RepairOne(SLOTTYPE_INVENTORY, pItem->m_nSlotID);
	}
}


void XGlueGame::RepairEquippedItem( int nSlot)
{
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nSlot);
	if ( pItem == NULL)		return;

	if ( pItem->m_pItemData != NULL &&
		pItem->m_pItemData->m_nMaxDurability != 0 &&
		pItem->m_nDurability < pItem->m_pItemData->m_nMaxDurability)
	{
		XPostNPCShop_RepairOne(SLOTTYPE_EQUIP, pItem->m_nSlotID);
	}
}


void XGlueGame::BuyItem( int nItemID, int nCount)
{
	if ( nItemID <= 0 || nCount <= 0)	return;
	XPostNPCShop_Buy( nItemID, nCount);
}

bool XGlueGame::IsRepairableShop()
{
	return gvar.Game.NPCShopInfo.GetRepairer();
}


int XGlueGame::GetJournalCount()
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return 0;
	return (int)pAccount->m_JournalIndicator.size();
}


int XGlueGame::GetJournalType( int i)
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return 0;
	return (int)pAccount->m_JournalIndicator[ i].first;
}


int XGlueGame::GetJournalID( int i)
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return 0;
	return (int)pAccount->m_JournalIndicator[ i].second;
}


bool XGlueGame::AddJournal( int _type, int _id)
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return false;
	return pAccount->AddJournal( _type, _id);
}


bool XGlueGame::RemoveJournal( int _type, int _id)
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return false;
	return pAccount->RemoveJournal( _type, _id);
}


bool XGlueGame::IsJournalIndicated( int _type, int _id)
{
	XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
	if ( pAccount == NULL)		return false;
	return pAccount->IsExistJournal( _type, _id);
}



void XGlueGame::ChangeCampaignScene( int nScene)
{
	XGetLoginCampaign()->ChangeScene( nScene);
}


bool XGlueGame::IsDirectLogin()
{
	return gvar.Network.bDirectLogin;
}

bool XGlueGame::IsAlreadyDirectLogined()
{
	return gvar.Network.bIsAlreadyDirectLogined;
}

void XGlueGame::WaitDirectLogin(bool bWait)
{
	gvar.Network.bWaitDirectLogin = bWait;
}


int XGlueGame::GetServerCount()
{
	return (int)gvar.Network.vecWorldInfo.size();
}


const char* XGlueGame::GetServerName( int i)
{
	wstring strName = gvar.Network.vecWorldInfo[ i].strName;
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( strName).c_str();
	return gvar.Temp.szTemp.c_str();
}


const char* XGlueGame::GetServerAddress( int i)
{
	wstring strIP = gvar.Network.vecWorldInfo[ i].strIP;
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( strIP).c_str();
	return gvar.Temp.szTemp.c_str();
}


MWLua::ret_int2 XGlueGame::GetServerState( int i)
{
	return MWLua::ret_int2( gvar.Network.vecWorldInfo[ i].nCurrentPlayerCount, gvar.Network.vecWorldInfo[ i].nMaxPlayerCount);
}


bool XGlueGame::GetServerActive( int i)
{
	// to do...
	return true;
}

int XGlueGame::GetServerType( int i)
{
	return gvar.Network.vecWorldInfo[ i].nType;
}


void XGlueGame::LoginConnect( const char* szServer, const char* szLoginID, const char* szPassword)
{	
	XCommonFunction::Connect(MLocale::ConvAnsiToUTF16(szLoginID).c_str(), MLocale::ConvAnsiToUTF16(szPassword).c_str(), MLocale::ConvAnsiToUTF16(szServer).c_str());
}


void XGlueGame::LoginDisconnect()
{
	if ( global.net)	global.net->PostDisconnect();
}


void XGlueGame::SelChangedCharacter( int i)
{
	if ( global.gfmgr == NULL  ||  global.gfmgr->GetCurrFrameID() != CHAR_FRAME_ID)		return;
	XCharFrame* pCharFrame = static_cast<XCharFrame*>(global.gfmgr->GetCurrFrame());
	if ( pCharFrame != NULL)
	{
		if ( i >= 0  &&  i < (int)gvar.Char.m_CharInfos.size())		pCharFrame->SelectChar( i);
		else														pCharFrame->DeleteCurrChar();
	}
}


void XGlueGame::SelectCharacter( int i)
{
	if ( global.gfmgr == NULL  ||  global.gfmgr->GetCurrFrameID() != CHAR_FRAME_ID)		return;
	if ( i < 0  ||  i >= (int)gvar.Char.m_CharInfos.size())								return;
	XPostChar_RequestSelMyChar( i);

	if ( global.script)
	{
		global.script->GetGlueGameEvent().OnCharacterEvent( "WORLD_ENTER", "SHOW");
	}
}


void XGlueGame::DeleteCharacter( int i)
{
	if ( global.gfmgr == NULL  ||  global.gfmgr->GetCurrFrameID() != CHAR_FRAME_ID)		return;
	if ( i < 0  ||  i >= (int)gvar.Char.m_CharInfos.size())								return;
	XPostChar_RequestDeleteCharacter( i);
}


void XGlueGame::CreateNewCharacter( const char* szName, MWLua::table t)
{
	if ( global.gfmgr == NULL  ||  global.gfmgr->GetCurrFrameID() != CHAR_FRAME_ID)		return;
	if ( szName == NULL  ||  szName[ 0] == 0)											return;

	CSNameStringChecker checker(MLocale::ConvAnsiToUTF16(szName).c_str());
	CCommandResultTable nResult = (CCommandResultTable)checker.Check();

	if (CR_SUCCESS == nResult)
	{
		XCHARACTER_CREATEINFO ci;
		ci.nStyle = TALENT_STYLE( t.get<int>("style"));
		ci.nSex = SEX( t.get<int>("sex"));
		ci.nFace = int( t.get<int>("face"));
		ci.nHair = int( t.get<int>("hair"));
		ci.nSkinColor = int( t.get<int>("skin_col"));
		ci.nHairColor = int( t.get<int>("hair_col"));
		ci.nVoice = int( t.get<int>("voice"));
		ci.nEyeColor = int( t.get<int>("eye_col"));
		ci.nMakeUp = int( t.get<int>("makeup"));
		ci.nTattoo = int( t.get<int>("tattoo"));
		ci.fTattooScale = float( t.get<float>("tattooscale"));
		ci.vTattooPos.x = float( t.get<float>("tattooposx"));
		ci.vTattooPos.y = float( t.get<float>("tattooposy"));
		ci.nEquipments = int( t.get<int>("equip"));
		ci.nEquipmentsCol = int( t.get<int>("equip_col"));

		XPostChar_RequestInsertCharacter( MLocale::ConvAnsiToUTF16(szName).c_str(), ci);
	}
	else
	{
		if ( global.script)
			global.script->GetGlueGameEvent().OnCharacterEvent( "CHARACTER", "CREATE", 0, nResult);
	}
}


void XGlueGame::ChangeNewCharacterModel( MWLua::table t)
{
	if ( global.gfmgr == NULL  ||  global.gfmgr->GetCurrFrameID() != CHAR_FRAME_ID)		return;
 	XCharFrame* pCharFrame = static_cast<XCharFrame*>( global.gfmgr->GetCurrFrame());

	if ( pCharFrame)
	{
		XCHARACTER_CREATEINFO ci;
		ci.nStyle = TALENT_STYLE( t.get<int>("style"));
		ci.nSex = SEX( t.get<int>("sex"));
		ci.nFace = int( t.get<int>("face"));
		ci.nHair = int( t.get<int>("hair"));
		ci.nSkinColor = int( t.get<int>("skin_col"));
		ci.nHairColor = int( t.get<int>("hair_col"));
		ci.nVoice = int( t.get<int>("voice"));
		ci.nEyeColor = int( t.get<int>("eye_col"));
		ci.nMakeUp = int( t.get<int>("makeup"));
		ci.nTattoo = int( t.get<int>("tattoo"));
		ci.fTattooScale = float( t.get<float>("tattooscale"));
		ci.vTattooPos.x = float( t.get<float>("tattooposx"));
		ci.vTattooPos.y = float( t.get<float>("tattooposy"));
		ci.nEquipments = int( t.get<int>("equip"));
		ci.nEquipmentsCol = int( t.get<int>("equip_col"));
		ci.bBattleStance = bool( t.get<bool>("battlestance"));

		pCharFrame->ChangeMockChar( ci);
	}
}


bool XGlueGame::IsValidateCharName( const char* szName)
{
	CSNameStringChecker checker(MLocale::ConvAnsiToUTF16(szName).c_str());
	CCommandResultTable nResult = (CCommandResultTable)checker.Check();
	if ( nResult == CR_SUCCESS)		return true;
	return false;
}

MWLua::ret_int2 XGlueGame::GetCharacterNameLengthKor()
{
	return MWLua::ret_int2(CSNameStringChecker::CHARACTER_MIN_HANGUL, CSNameStringChecker::CHARACTER_MAX_HANGUL);
}

MWLua::ret_int2 XGlueGame::GetCharacterNameLengthEng()
{
	return MWLua::ret_int2(CSNameStringChecker::CHARACTER_MIN_ENGLISH, CSNameStringChecker::CHARACTER_MAX_ENGLISH);
}

int XGlueGame::GetLootingItemID( unsigned int i) const
{
	if(gvar.MyInfo.LootingItem.GetSize() <= i) return 0;

	XLootingItem* pItem = gvar.MyInfo.LootingItem.Get( i);
	if (pItem == NULL) return 0;
	return pItem->m_nItemID;
}

int XGlueGame::GetLootingItemType( unsigned int i) const
{
	if(gvar.MyInfo.LootingItem.GetSize() <= i) return 0;

	XLootingItem* pItem = gvar.MyInfo.LootingItem.Get( i);
	if (pItem == NULL) return 0;
	return (int)pItem->m_nItemType;
}

int XGlueGame::GetLootingItemQuantity( unsigned int i) const
{
	if(gvar.MyInfo.LootingItem.GetSize() <= i) return 0;

	XLootingItem* pItem = gvar.MyInfo.LootingItem.Get( i);
	if (pItem == NULL) return 0;
	return pItem->m_nAmount;
}

// bool XGlueGame::IsGettableLootingItem(int i)
// {
// 	XDropItem* pItem = gvar.MyInfo.LootingItem.GetItem( i);
// 	if (pItem == NULL) return false;
// 
// 	return DIT_GETTABLE == pItem->m_nDropItemType;
// }
// 
// bool XGlueGame::IsRollableLootingItem(int i)
// {
// 	XDropItem* pItem = gvar.MyInfo.LootingItem.GetItem( i);
// 	if (pItem == NULL) return false;
// 
// 	return DIT_ROLLABLE == pItem->m_nDropItemType;
// }
// 
// bool XGlueGame::IsMasterLootableLootingItem(int i)
// {
// 	XDropItem* pItem = gvar.MyInfo.LootingItem.GetItem( i);
// 	if (pItem == NULL) return false;
// 
// 	return DIT_MASTERLOOTABLE == pItem->m_nDropItemType;
// }

void XGlueGame::LootItem( unsigned int i)
{
	if(gvar.MyInfo.LootingItem.GetSize() <= i) return;

	XLootingItem* pItem = gvar.MyInfo.LootingItem.Get( i);
	if ( pItem == NULL)		return;

	vector<MUID> vecDropItemUID;
	vecDropItemUID.push_back( pItem->m_ItemUID);

	XEvent evt;
	evt.m_nID = XEVTL_LOOT_GETITEM;
	evt.m_pData = &vecDropItemUID;
	gg.omgr->PostEvent( XGetMyUID(), evt);
}

void XGlueGame::LootAllItems()
{
	vector<MUID> vecDropItemUID;
	size_t _size = gvar.MyInfo.LootingItem.GetSize();
	for ( size_t i = 0;  i < _size;  i++)
	{
		XLootingItem* pItem = gvar.MyInfo.LootingItem.Get( i);
		XItemData* pItemData = info.item->GetItemData(pItem->m_nItemID);
		if(pItemData == NULL) continue;
		if(pItemData->m_bUnique == true)
		{
			if(gvar.MyInfo.Inventory.IsHaveItem(pItem->m_nItemID))
			{
				global.ui->OnSystemMsg(L"{0} is unique, you cannot loot another one of those.", FSParam(pItemData->GetName()));
				continue;
			}
		}

		vecDropItemUID.push_back( pItem->m_ItemUID);
	}

	XEvent evt;
	evt.m_nID = XEVTL_LOOT_GETALLITEM;
	evt.m_pData = &vecDropItemUID;
	gg.omgr->PostEvent( XGetMyUID(), evt);
}

void XGlueGame::RequestLootingEnd()
{
	gvar.Game.pMyPlayer->GetModuleMyControl()->OnLootEndReady();
}

// int XGlueGame::GetItemCountLootingLooter()
// {
// 	return gvar.MyInfo.LootingItem.vecTDLootMasterLootUI.size();
// }
//
// int XGlueGame::GetItemIDLootingLooter()
// {
// 	TD_LOOT_MASTERLOOT_UI& tdMasterLoot = gvar.MyInfo.LootingItem.vecTDLootMasterLootUI.back();
// 	return tdMasterLoot.m_nItemID;
// }
//
// int XGlueGame::GetItemnQuantityLootingLooter()
// {
// 	TD_LOOT_MASTERLOOT_UI& tdMasterLoot = gvar.MyInfo.LootingItem.vecTDLootMasterLootUI.back();
// 	return tdMasterLoot.m_nItemAmount;
// }
//
// void XGlueGame::ReqAssignItem(int nIndex, bool bRandom)
// {
// 	XPartyMember* pPartyMember = NULL;
// 	if ( gvar.MyInfo.Party.GetIndex( nIndex, &pPartyMember) == false ||
// 		pPartyMember == NULL)	return;
// 
// 	TD_LOOT_MASTERLOOT_UI& tdMasterLoot = gvar.MyInfo.LootingItem.vecTDLootMasterLootUI.back();
// 	XPostLoot_MasterLootItem(gvar.MyInfo.LootingItem.uidNPC,
// 		tdMasterLoot.m_nDropItemUID, 0, bRandom);
// 
// 	gvar.MyInfo.LootingItem.vecTDLootMasterLootUI.pop_back();
// }

int XGlueGame::GetAssignLootingCount()
{
	return (int)gvar.MyInfo.AssignLootingItem.GetSize();
}


int XGlueGame::GetAssignLootingID( int nItemIndex)
{
	XLootingItem* pItem = gvar.MyInfo.AssignLootingItem.Get( nItemIndex);
	if ( pItem == NULL)			return 0;
	return pItem->m_nItemID;
}


int XGlueGame::GetAssignLootingQuantity( int nItemIndex)
{
	XLootingItem* pItem = gvar.MyInfo.AssignLootingItem.Get( nItemIndex);
	if ( pItem == NULL)			return 0;
	return pItem->m_nAmount;
}


int XGlueGame::GetLooterCount( int nItemIndex)
{
	XLootingItem* pItem = gvar.MyInfo.AssignLootingItem.Get( nItemIndex);
	if ( pItem == NULL)			return 0;
	return (int)pItem->m_vLooter.size();
}


const char* XGlueGame::GetLooterName( int nItemIndex, int nLooterIndex)
{
	XLootingItem* pItem = gvar.MyInfo.AssignLootingItem.Get( nItemIndex);
	if ( pItem == NULL)			return 0;
	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pItem->m_vLooter[ nLooterIndex].m_strName.c_str());

	return gvar.Temp.szTemp.c_str();
}


void XGlueGame::DoAssignLooting( int nItemIndex, int nWho)
{
	XLootingItem* pItem = gvar.MyInfo.AssignLootingItem.Get( nItemIndex);
	if ( pItem == NULL)			return;
	XPostLoot_MasterLootItem( pItem->m_NpcUID, pItem->m_ItemUID, static_cast<uint8>(nWho), false);
	gvar.MyInfo.AssignLootingItem.DeleteItem( nItemIndex);
}


void XGlueGame::AcceptQuest( int nID)
{
	XQuestInfo* pQuestInfo = info.quest->Get( nID);
	if ( pQuestInfo != NULL)	XPostQuest_RequestGive( nID);

	SetNPCIconQuestBegin();
}

void XGlueGame::RejectQuest()
{
	SetNPCIconQuestBegin();
}

void XGlueGame::SetNPCIconQuestBegin()
{
	XNonPlayer* pNPC = gg.omgr->FindNPC(XGetInteractionInfo().InteractionTargetUID);
	if (pNPC == NULL) return;

	pNPC->SetPreviousNPCIcon();
}

void XGlueGame::AcceptQuestRewardItem( int nQuestID, int nSelectedReward)
{
	XQuestInfo* pQuestInfo = info.quest->Get( nQuestID);
	if ( pQuestInfo == NULL) return;
	XPostQuest_RequestReward( XGetInteractionInfo().InteractionTargetUID, nQuestID, nSelectedReward);
}

void XGlueGame::RejectQuestRewardItem()
{
	XNonPlayer* pNPC = gg.omgr->FindNPC(XGetInteractionInfo().InteractionTargetUID);
	if (pNPC == NULL) return;

	pNPC->SetNPCIConState(NIS_QUEST_END);
}

const char* XGlueGame::GetInteractionPlayerName() const
{
	XNetPlayer* pNetPlayer = gg.omgr->FindNetPlayer( XGetInteractionInfo().InteractionTargetUID);
	if ( !pNetPlayer)									return NULL;
	if ( pNetPlayer->GetEntityType() != ETID_PLAYER)	return NULL;
	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pNetPlayer->GetName());

	return gvar.Temp.szTemp.c_str();
}

const bool XGlueGame::IsInteractionPlayerPartyLeader() const
{
	XNetPlayer* pNetPlayer = gg.omgr->FindNetPlayer( XGetInteractionInfo().InteractionTargetUID);
	if ( !pNetPlayer)									return NULL;
	if ( pNetPlayer->GetEntityType() != ETID_PLAYER)	return NULL;

	return pNetPlayer->IsPartyLeader();
}

void XGlueGame::InviteToParty()
{
	if ( XGetInteractionInfo().InteractionTargetUID ==  gvar.MyInfo.MyUID)
		global.ui->OnSystemMsg(L"You can't invite yourself, silly.");
	else
	{
		wstring strPlayerName = XCommonFunction::GetPlayerName( XGetInteractionInfo().InteractionTargetUID);
		global.ui->OnSystemMsg(L"{0} has been invited to the party.", FSParam(strPlayerName));
		XPostPartyRequestInvite( XGetInteractionInfo().InteractionTargetUID);
	}
}

void XGlueGame::InviteToGuild()
{
	XPostGuild_Invite_Req( XGetInteractionInfo().InteractionTargetUID);
}

void XGlueGame::RequestToTrade()
{
	if ( XGetInteractionInfo().InteractionTargetUID ==  gvar.MyInfo.MyUID)
		global.ui->OnSystemMsg(L"Can't trade with yourself, boss.");
	else
	{
		wstring strPlayerName = XCommonFunction::GetPlayerName( XGetInteractionInfo().InteractionTargetUID);
		global.ui->OnSystemMsg(L"You have requested to trade with {0}", FSParam(strPlayerName));
		XPostTrade_Start( XGetInteractionInfo().InteractionTargetUID);
	}
}

void XGlueGame::ChallengeToDuel()
{
	wstring strPlayerName = XCommonFunction::GetPlayerName( XGetInteractionInfo().InteractionTargetUID);
	global.ui->OnSystemMsg(L"You requested to duel with {0}", FSParam(strPlayerName));
	XPostDuel_Request( XGetInteractionInfo().InteractionTargetUID);
}

void XGlueGame::ChallengeToPartyDuel()
{
	wstring strPlayerName = XCommonFunction::GetPlayerName( XGetInteractionInfo().InteractionTargetUID);
	global.ui->OnSystemMsg(L"You requested to duel with {0}'s party.", FSParam(strPlayerName));
	XPostPartyDuel_Request( XGetInteractionInfo().InteractionTargetUID);
}

const char* XGlueGame::GetRequesterName() const
{
	XNetPlayer* pNetPlayer = gg.omgr->FindNetPlayer( XGetInteractionInfo().GetRequestPlayerUID());
	if ( !pNetPlayer)
	{
		_ASSERT(0);
		return NULL;
	}
	if ( pNetPlayer->GetEntityType() != ETID_PLAYER)	return NULL;

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pNetPlayer->GetName());

	return gvar.Temp.szTemp.c_str();
}

void XGlueGame::ConfirmInviteToParty( bool bAccept)
{
	XPostPartyRequestInviteQuestionRespond( bAccept);
	gvar.Game.bPartyReq = false;
}

void XGlueGame::ConfirmInviteToGuild( bool bAccept)
{
	XPostGuild_Invite_Res( bAccept);
}

void XGlueGame::ConfirmRequestToTrade( bool bAccept)
{
	XPostTrade_StartAccept(bAccept);
	gvar.Game.bTradeReq = false;
}

void XGlueGame::ConfirmChallengeToDuel( bool bAccept)
{
	XPostDuel_QuestionRespond( bAccept);
	gvar.Game.bDuelReq = false;
}

int XGlueGame::GetEnemyCount()
{
	if (_IsLockOnTarget())	return 1;

	return gvar.Game.EnemyInfo.GetEnemyCount();
}

UIID XGlueGame::GetEnemyID()
{
	if (_IsLockOnTarget())
		return _GetTargetID(XGetInteractionInfo().GetTargetUIID());

	TD_ENEMY_INFO* pInfo = gvar.Game.EnemyInfo.GetEnemyInfo(0);
	if ( pInfo == NULL)		return 0;

	XObject* pObject = gg.omgr->FindActor_UIID( pInfo->nUIID);
	if ( pObject == NULL)	return 0;

	return pInfo->nUIID;
}

const char* XGlueGame::GetEnemyName()
{
	if (_IsLockOnTarget())
		return _GetTargetName(XGetInteractionInfo().GetTargetUIID());

	TD_ENEMY_INFO* pInfo = gvar.Game.EnemyInfo.GetEnemyInfo(0);
	if ( pInfo == NULL)		return "";

	XObject* pObject = gg.omgr->FindActor_UIID( pInfo->nUIID);
	if ( pObject == NULL)	return "";
	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pObject->GetName());

	return gvar.Temp.szTemp.c_str();
}


int XGlueGame::GetEnemyLevel()
{
	if (_IsLockOnTarget())
		return _GetTargetLevel(XGetInteractionInfo().GetTargetUIID());

	TD_ENEMY_INFO* pInfo = gvar.Game.EnemyInfo.GetEnemyInfo(0);
	if ( pInfo == NULL) return 0;

	XNonPlayer* pNPC = gg.omgr->FindNPC_UIID( pInfo->nUIID);
	if (pNPC && pNPC->IsAttackable() == false)	return 0;

	return pInfo->nLevel;
}


int XGlueGame::GetEnemyGrade()
{
	if (_IsLockOnTarget())
		return _GetTargetGrade(XGetInteractionInfo().GetTargetUIID());

	TD_ENEMY_INFO* pInfo = gvar.Game.EnemyInfo.GetEnemyInfo(0);
	if ( pInfo == NULL)	return 0;

	XNonPlayer* pNPC = gg.omgr->FindNPC_UIID( pInfo->nUIID);
	if (pNPC && pNPC->GetInfo()) return pNPC->GetInfo()->nGrade;

	return 0;
}


int XGlueGame::GetEnemyHP()
{
	if (_IsLockOnTarget())
		return _GetTargetHP(XGetInteractionInfo().GetTargetUIID());

	TD_ENEMY_INFO* pInfo = gvar.Game.EnemyInfo.GetEnemyInfo(0);
	if ( pInfo == NULL) return 0;

	return pInfo->nHPPercent;	
}


int XGlueGame::GetEnemyMaxHP()
{
	if (_IsLockOnTarget())
		return _GetTargetMaxHP(XGetInteractionInfo().GetTargetUIID());

	TD_ENEMY_INFO* pInfo = gvar.Game.EnemyInfo.GetEnemyInfo(0);
	if ( pInfo == NULL) return 0;
	
	return pInfo->nMaxHP;
}

bool XGlueGame::IsShowEnemyInfo()
{
	if (_IsLockOnTarget())
		return _IsShowTargetInfo(XGetInteractionInfo().GetTargetUIID());

	TD_ENEMY_INFO* pInfo = gvar.Game.EnemyInfo.GetEnemyInfo(0);
	if ( pInfo == NULL) return false;

	XNonPlayer* pNPC = gg.omgr->FindNPC_UIID( pInfo->nUIID);
	if (pNPC)
	{
		XNPCInfo* pNPCInfo = pNPC->GetInfo();
		return pNPCInfo->bShowEnemyInfo;
	}

	return true;
}

UIID XGlueGame::GetTargetID()
{
	return _GetTargetID(XGetInteractionInfo().GetSearchUIID());
}

UIID XGlueGame::_GetTargetID(UIID uiid)
{
	TD_ENEMY_INFO* pInfo = gvar.Game.EnemyInfo.GetEnemyInfoByUIID(uiid);
	if (pInfo)
	{
		return XGetInteractionInfo().GetSearchUIID();
	}
	else if(XGetInteractionInfo().stTargetActorInfo.Find_Time(uiid))
	{
		return XGetInteractionInfo().GetSearchUIID();
	}
	else
	{
		if(XGetInteractionInfo().stTargetActorInfo.IsPostTime())
			XPostChar_TargetInfo(uiid);

		return 0;
	}
}

const char* XGlueGame::GetTargetName()
{
	return _GetTargetName( XGetInteractionInfo().GetSearchUIID());
}

const char* XGlueGame::_GetTargetName(UIID uiid)
{
	XObject* pObject = gg.omgr->FindActor_UIID(uiid);
	if ( pObject == NULL) return "";

	vec3 pos = pObject->GetPosition();
	float dis = pos.XYDistanceTo(gvar.Game.pMyPlayer->GetPosition()) / 100.f;

	wchar_t szDistance[128] = {0,};
	swprintf_s(szDistance, L"%s(%.2fm) ", pObject->GetName(), dis);

	if(pObject->GetType() == XOBJ_NET_PLAYER)
	{
		XNetPlayer* pNetPlayer = static_cast<XNetPlayer*>(pObject);
		if (pNetPlayer->IsPartyLeader() == true)
		{
			swprintf_s(szDistance, L"%s[%s](%.2fm) ", pObject->GetName(), XGetStr(L"PARTYLEADER"), dis);

		}
	}

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(szDistance);
	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetTargetLevel()
{
	return _GetTargetLevel(XGetInteractionInfo().GetSearchUIID());
}

int XGlueGame::_GetTargetLevel(UIID uiid)
{
	TargetActorInfo* pInfo = XGetInteractionInfo().stTargetActorInfo.Get(uiid);
	if (pInfo == NULL) return 0;

	return pInfo->m_nLevel;
}

bool XGlueGame::_IsLockOnTarget()
{
	return XGetInteractionInfo().stTargetActorInfo.bLockOnTarget;
}

bool XGlueGame::IsShowTargetInfo()
{
	return _IsShowTargetInfo(XGetInteractionInfo().GetSearchUIID());
}

bool XGlueGame::_IsShowTargetInfo(UIID uiid)
{
	XObject* pObject = gg.omgr->FindActor_UIID(uiid);
	if ( pObject == NULL)	return false;

	int nGrade = 0;
	if ( pObject->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNPC = pObject->AsNPC();
		XNPCInfo* pNPCInfo = pNPC->GetInfo();
		return pNPCInfo->IsShowTargetInfo();
	}
	else if (pObject->GetEntityType() == ETID_PLAYER)
	{
		return true;
	}

	return false;
}

int XGlueGame::GetTargetGrade()
{
	return _GetTargetGrade(XGetInteractionInfo().GetSearchUIID());
}

int XGlueGame::_GetTargetGrade(UIID uiid)
{
	XNonPlayer* pNPC = gg.omgr->FindNPC_UIID(uiid);
	if (pNPC && pNPC->GetInfo()) return pNPC->GetInfo()->nGrade;

	return 0;
}

int XGlueGame::GetTargetHP()
{
	return _GetTargetHP(XGetInteractionInfo().GetSearchUIID());
}

int XGlueGame::_GetTargetHP(UIID uiid)
{
	TargetActorInfo* pInfo = XGetInteractionInfo().stTargetActorInfo.Get(uiid);
	if (pInfo == NULL) return 0;

	return pInfo->m_nHPPercent;
}

int XGlueGame::GetTargetMaxHP()
{
	return _GetTargetMaxHP(XGetInteractionInfo().GetSearchUIID());
}

int XGlueGame::_GetTargetMaxHP(UIID uiid)
{
	TargetActorInfo* pInfo = XGetInteractionInfo().stTargetActorInfo.Get(uiid);
	if (pInfo == NULL) return 0;

	return pInfo->m_nMaxHP;
}

string XGlueGame::_GetDisplayKeyString(const string& strVKey)
{
	XVirtualKey nVKey = gg.controller->GetVirtualKey(strVKey);
	if ( nVKey <= VKEY_NA  ||  nVKey >= VKEY_COUNT)	return "";
	return gg.controller->GetDisplayString( nVKey);
}

enum { TARGETTYPE_NONE, TARGETTYPE_TALK, TARGETTYPE_TAKE, TARGETTYPE_QUEST, TARGETTYPE_TRADE, TARGETTYPE_TRIGGER };
int XGlueGame::GetTargetInteractType()
{
	XObject* pObject = gg.omgr->FindActor_UIID( XGetInteractionInfo().GetSearchUIID());
	if ( pObject == NULL)								return TARGETTYPE_NONE;
	if ( pObject->GetType() == XOBJ_NET_PLAYER)			return TARGETTYPE_TALK;
	else if ( pObject->GetType() == XOBJ_NPC)
	{
		XNonPlayer* pNPC = pObject->AsNPC();
		XNPCInfo* pNPCInfo = pNPC->GetInfo();
		TargetActorInfo* pTarInfo = XGetInteractionInfo().stTargetActorInfo.Get( XGetInteractionInfo().GetSearchUIID());
		if ( pTarInfo == NULL)							return TARGETTYPE_NONE;
		if ( pNPCInfo->nNpcType == NPC_TYPE_OBJECT)		return TARGETTYPE_TRIGGER;
		if ( pNPCInfo->nNpcType == NPC_TYPE_GATHER)		return TARGETTYPE_TAKE;
		if ( pNPC->IsDead() == true)
		{
			if ( pNPC->IsLootable() == true)			return TARGETTYPE_TAKE;
			else										return TARGETTYPE_NONE;
		}
		if ( pNPC->IsInteract() == true)
		{
			switch ( pNPC->GetIConState())
			{
			case NIS_TALK :								return TARGETTYPE_TALK;
			case NIS_TRIGGER :							return TARGETTYPE_TRIGGER;
			case NIS_CRAFT :							return TARGETTYPE_TALK;
			case NIS_SHOP :								return TARGETTYPE_TRADE;
			case NIS_QUEST_BEGIN :						return TARGETTYPE_QUEST;
			case NIS_QUEST_END :						return TARGETTYPE_QUEST;
			case NIS_QUEST_CONTINUE :					return TARGETTYPE_QUEST;
			default :									return TARGETTYPE_NONE;
			}
		}
	}
	else if ( pObject->GetType() == XOBJ_BPART_NPC)		return TARGETTYPE_TAKE;
	return TARGETTYPE_NONE;
}

MWLua::ret_int4 XGlueGame::GetTargetAttackableType()
{
	XObject* pObject = gg.omgr->FindActor_UIID( XGetInteractionInfo().GetSearchUIID());
	if ( pObject == NULL  ||  pObject->GetType() != XOBJ_NPC)	return MWLua::ret_int4( 0, 0, 0, 0);

	TargetActorInfo* pTarInfo = XGetInteractionInfo().stTargetActorInfo.Get( XGetInteractionInfo().GetSearchUIID());
	if ( pTarInfo == NULL)								return MWLua::ret_int4( 0, 0, 0, 0);

	XNonPlayer* pNPC = pObject->AsNPC();
	XNPCInfo* pNPCInfo = pNPC->GetInfo();
	switch ( pNPCInfo->nInitAttackable)
	{
	case NAT_NONE :
		return MWLua::ret_int4( 0, 0, 0, 0);

	case NAT_ALWAYS :
	case NAT_FACTION :
		{
			if ( pNPCInfo->nAA == NAAT_NONE)
			{
				TD_ENEMY_INFO* pEnemyInfo = gvar.Game.EnemyInfo.GetEnemyInfoByUIID( pNPC->GetUIID());
				if ( pEnemyInfo != NULL)				return MWLua::ret_int4( 1, -1, pTarInfo->m_bComabt ? 1 : 0, 0);
				else									return MWLua::ret_int4( 1, 0, pTarInfo->m_bComabt ? 1 : 0, 0);
			}
			if ( pNPCInfo->nAA == NAAT_ALWAYS)			return MWLua::ret_int4( 1, -1, pTarInfo->m_bComabt ? 1 : 0, 0);
			if ( pNPCInfo->nAA == NAAT_FACTION)
			{
				XFactionInfo* pFactionInfo = info.faction->Get( pNPCInfo->m_nFactionLossID);
				if ( pFactionInfo == NULL)				return MWLua::ret_int4( 1, 0, 0, 0);

				FACTION_RELATION_TYPE _faction = gvar.MyInfo.FactionInfo.GetRelation( pNPCInfo->m_nFactionLossID);
				switch ( _faction)
				{
				case FRT_WORST :
				case FRT_BAD :
					return MWLua::ret_int4( 1, -1, pTarInfo->m_bComabt ? 1 : 0, 0);

				case FRT_NORMAL :
				case FRT_GOOD :
				case FRT_EXCELLENT :
				case FRT_TRUST :
					return MWLua::ret_int4( 0, 1, pTarInfo->m_bComabt ? 1 : 0, 0);
				}
			}
			return MWLua::ret_int4( 0, 0, 0, 0);
		}
	}
	return MWLua::ret_int4( 0, 0, 0, 0);
}

const char* XGlueGame::GetTargetMessage()
{
	if(XGETCFG_GAME_SHOWHELPMESSAGE == false) return "";

	XObject* pObject = gg.omgr->FindActor_UIID( XGetInteractionInfo().GetSearchUIID());
	if ( pObject == NULL)	return "";

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return "";

	if (pModuleMyControl->IsHelpMessage())
	{
		string strKey = _GetDisplayKeyString("LOCKTARGET");
		if (_IsLockOnTarget())
		{
			gvar.Temp.szTemp = strKey + "키를 다시 눌러 주시대상 설정을 해제합니다.";
			return gvar.Temp.szTemp.c_str();
		}
		else
		{
			gvar.Temp.szTemp = strKey + "키를 눌러 주시대상으로 설정합니다.";
			return gvar.Temp.szTemp.c_str();
		}
	}

	if(pObject->GetType() == XOBJ_NPC)
	{
		gvar.Temp.szTemp = GetNPCTargetMessage(pObject).c_str();
	}	
	else if(pObject->GetType() == XOBJ_NET_PLAYER)
	{
		string strKey = _GetDisplayKeyString("PCINTERACTION");
		gvar.Temp.szTemp = strKey + "키를 눌러 대화를 할 수 있습니다.";
	}

	return gvar.Temp.szTemp.c_str();
}

string XGlueGame::GetNPCTargetMessage(XObject* pObject)
{ 
	wstring strMessage;

	XNonPlayer* pNPC = pObject->AsNPC();
	XNPCInfo* pNPCInfo = pNPC->GetInfo();

	TargetActorInfo* pInfo = XGetInteractionInfo().stTargetActorInfo.Get(XGetInteractionInfo().GetSearchUIID());
	if(pInfo == NULL) return "";

	string strKey = _GetDisplayKeyString("NPCINTERACTION");
	wstring wstrKey = MLocale::ConvAnsiToUTF16(strKey.c_str());

	if(pNPCInfo->nNpcType == NPC_TYPE_OBJECT)
	{
		strMessage = wstrKey + L"키를 눌러 아이템을 획득할 수 있습니다.";
	}
	else if(pNPC->IsDead())
	{
		if(pNPC->IsLootable())	strMessage = wstrKey + L"키를 눌러 전리품을 획득할 수 있습니다.";
	}
	else if(pNPC->IsInteract())
	{
		if(pInfo->m_nNPCIConType == NIS_SHOP)				strMessage = wstrKey + L"키를 눌러 이 NPC와 거래를 할 수 있습니다.";
		else if(pInfo->m_nNPCIConType == NIS_QUEST_BEGIN)	strMessage = wstrKey + L"키를 눌러 퀘스트를 받을 수 있습니다.";
		else if(pInfo->m_nNPCIConType == NIS_QUEST_END)		strMessage = wstrKey + L"키를 눌러 퀘스트를 완료할 수 있습니다.";
		else												strMessage = wstrKey + L"키를 눌러 대화를 할 수 있습니다.";
	}
	else if (pNPC->IsNPC())
	{
		if(pNPCInfo->nInitAttackable == NAT_NONE)
		{
			strMessage = L"<이 NPC는 공격할 수 없습니다.>";
		}
	}
	else if(pNPC->IsMonster())
	{
		if (pInfo && pInfo->m_bComabt)
		{
			strMessage = L"이미 다른 사람이 공격하고 있습니다. 이 NPC를 공격해도 경험치나 전리품을 획득할 수 없습니다.";
		}
		else if(pNPCInfo->nAA == NAAT_ALWAYS)
		{
			XFactionInfo* pFactionInfo = info.faction->Get(pNPCInfo->m_nFactionLossID);
			if (pFactionInfo)
			{
				strMessage = L"<" + pFactionInfo->GetTitle() + L">";
				strMessage += L"적대적 : 이 NPC는 당신을 발견했을 때 공격합니다.";
			}
			else
			{
				wstring strClan = pNPCInfo->GetClan();
				if (!strClan.empty())
				{
					strMessage = L"<" + strClan + L">";
				}
			}
		}
		else if(pNPCInfo->nAA == NAAT_NONE)
		{
			//if(pNPCInfo->bInteract == false)
			strMessage = L"<이 NPC는 공격할 수 없습니다.>";
		}
		else
		{
			XFactionInfo* pFactionInfo = info.faction->Get(pNPCInfo->m_nFactionLossID);
			if (NULL == pFactionInfo)
			{
				wstring strClan = pNPCInfo->GetClan();
				if (!strClan.empty())
				{
					strMessage = L"<" + strClan + L">";
				}
			}
			else
			{
				strMessage = L"<" + pFactionInfo->GetTitle() + L">";

				FACTION_RELATION_TYPE nFRT = gvar.MyInfo.FactionInfo.GetRelation(pNPCInfo->m_nFactionLossID);
				if(pNPCInfo->nAA == NAAT_FACTION)
				{						
					switch (nFRT)
					{						
					case FRT_WORST:		strMessage += L"매우 적대적 : 이 NPC는 당신을 발견했을 때 공격합니다.";	break;
					case FRT_BAD:		strMessage += L"적대적 : 이 NPC는 당신을 발견했을 때 공격합니다.";		break;
					}
				}

				switch (nFRT)
				{
				case FRT_NORMAL:	strMessage += L"보통";			break;
				case FRT_GOOD:		strMessage += L"우호적";		break;
				case FRT_EXCELLENT:	strMessage += L"매우 우호적";	break;
				case FRT_TRUST:		strMessage += L"신뢰";	break;
				}
			}
		}
	}


	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strMessage.c_str());
	return gvar.Temp.szTemp.c_str();
}

/*
void XGlueGame::ManagedWindow_Pin( bool bPin, const char* szWidgetName )
{
	if (global.ui == NULL) return;
	if (global.ui->GetWindowShowManager() == NULL) return;
	if (gg.omgr == NULL) return;

	global.ui->GetWindowShowManager()->PinWindow(bPin, szWidgetName);
}
*/


// GetMailCount
int XGlueGame::GetMailCount()
{
	return gvar.MyInfo.Mailbox.GetMailCount();
}


// IsExistUnreadMail
bool XGlueGame::IsExistUnreadMail()
{
	return gvar.MyInfo.Mailbox.IsExistUnreadMail();
}


// IsMailBoxFull
bool XGlueGame::IsMailBoxFull()
{
	return gvar.MyInfo.Mailbox.IsFullMailbox();
}


// GetMailTitle
const char* XGlueGame::GetMailTitle( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)
	{
		gvar.Temp.szTemp = "";
		return gvar.Temp.szTemp.c_str();
	}

	//if (GetStringFilter()->IsValidStr(pMail->GetTitle().c_str()))
	//{
	//	int i = 0;
	//}

	wstring szMsg = GetStringFilter()->Filter(pMail->GetTitle().c_str());
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( szMsg.c_str());
	return gvar.Temp.szTemp.c_str();
}


// GetMailSender
const char* XGlueGame::GetMailSender( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)
	{
		gvar.Temp.szTemp = "";
		return gvar.Temp.szTemp.c_str();
	}

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( pMail->GetSender().c_str());
	return gvar.Temp.szTemp.c_str();
}


// GetMailContent
const char* XGlueGame::GetMailContent( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)
	{
		gvar.Temp.szTemp = "";
		return gvar.Temp.szTemp.c_str();
	}

	//if (GetStringFilter()->IsValidStr(pMail->GetContent().strText.c_str()))
	//{
	//	int i = 0;
	//}

	wstring szMsg = GetStringFilter()->Filter(pMail->GetContent().strText.c_str());
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( szMsg.c_str());
	return gvar.Temp.szTemp.c_str();
}


// GetMailRemaindDay
int XGlueGame::GetMailRemaindDay( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL) return 0;

	return pMail->GetMailRemaindDay();
}


// IsMailRead
bool XGlueGame::IsMailRead( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)	return false;

	return pMail->IsRead();
}


// GetMailAttachedItemCount
int XGlueGame::GetMailAttachedItemCount( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)	return 0;
	XMailContent& _content = pMail->GetContent();
	return (int)_content.vAppendedItems.size();
}


// GetMailAttachedItemID
int XGlueGame::GetMailAttachedItemID( int i, int item)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)	return 0;
	XMailContent& _content = pMail->GetContent();
	return (int)_content.vAppendedItems[ item].m_nItemID;
}


// GetMailAttachedItemQuantity
int XGlueGame::GetMailAttachedItemQuantity( int i, int item)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)	return 0;
	XMailContent& _content = pMail->GetContent();
	return (int)_content.vAppendedItems[ item].m_nQuantity;
}


// GetMailAttachedItemIndex
int XGlueGame::GetMailAttachedItemIndex( int i, int item)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)	return 0;
	XMailContent& _content = pMail->GetContent();
	return (int)_content.vAppendedItems[ item].m_nSlotID;
}


// GetMailAttachedMoney
int XGlueGame::GetMailAttachedMoney( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)	return 0;
	XMailContent& _content = pMail->GetContent();
	return (int)_content.nAppendedMoney;
}


// GetMailAttachedThumbItemID
int XGlueGame::GetMailAttachedThumbItemID( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if (pMail == NULL)	return 0;
	return pMail->GetThumbnailItemID();
}


// AttachItemToSendMail
void XGlueGame::AttachItemToSendMail( int i, int qty)
{
	XPostMail_PutUpItem( i, qty);
}


// DetachItemToSendMail
void XGlueGame::DetachItemToSendMail( int i, int qty)
{
	XPostMail_PutDownItem( i, qty);
}


// ClearSendMailAttachedItems
void XGlueGame::ClearSendMailAttachedItems()
{
	XPostMail_ResetItems();
}


// ReadMail
bool XGlueGame::ReadMail( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if ( pMail == NULL)		return false;
	pMail->SetRead();

	if (pMail->GetContent().bFill)	///< 편지 내용을 이미 읽었으면 바로 표시합니다.
	{
		if (global.script)		global.script->GetGlueGameEvent().OnGameEvent( "MAILBOX", "READ");
		return true;
	}

	XPostMail_ReadMail( pMail->GetUID().Value);
	return true;
}


// SendMail
bool XGlueGame::SendMail( const char* szReceiver, const char* szTitle, const char* szContent, int nMoney)
{
	if ( szReceiver == NULL  ||  szReceiver[ 0] == 0  ||  szTitle == NULL ||  szContent == NULL || nMoney < 0)
		return false;

	XPostMail_SendMail( MLocale::ConvAnsiToUTF16(szReceiver).c_str(), MLocale::ConvAnsiToUTF16(szTitle).c_str(), MLocale::ConvAnsiToUTF16(szContent).c_str(), nMoney);
	return true;
}

// DeleteMail
bool XGlueGame::DeleteMail( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if ( pMail == NULL)		return false;
	XPostMail_DeleteMail( pMail->GetUID());
	return true;
}


// TakeMailMoney
void XGlueGame::TakeMailMoney( int i)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if ( pMail == NULL)		return;
	XPostMail_TakeAppendedMoney( pMail->GetUID());
}


// TakeMailItem
void XGlueGame::TakeMailItem( int i, int item)
{
	XMail* pMail = gvar.MyInfo.Mailbox.GetMailIndex( i);
	if ( pMail == NULL)		return;
	XPostMail_TakeAppendedItem( pMail->GetUID(), item);
}


// GetMailErrorMsg
const char* XGlueGame::GetMailErrorMsg( int _errcode)
{
	gvar.Temp.szTemp = "";
	switch ( _errcode)
	{
	case CR_SUCCESS :							break;
	case CR_FAIL_MAIL_POST_SELF :				gvar.Temp.szTemp = "You really must be lonely to be sending mail to yourself.";				break;
	case CR_FAIL_MAIL_POST_LIMIT_TITLE :		gvar.Temp.szTemp = "The title is too long.";								break;
	case CR_FAIL_MAIL_POST_LIMIT_TEXT :			gvar.Temp.szTemp = "The body of your message is too long.";							break;
	case CR_FAIL_MAIL_POST_NOT_FIND_USER :		gvar.Temp.szTemp = "No such user exists.";				break;
	case CR_FAIL_MAIL_POST_NOT_SEND_GM :		gvar.Temp.szTemp = "You cannot send mail to a GM.";					break;
	case CR_FAIL_MAIL_POST_FULL_BOX :			gvar.Temp.szTemp = "That person's mailbox is full, sorry.";	break;
	case CR_FAIL_MAIL_POST_NOT_ENOUGH_POSTAGE :	gvar.Temp.szTemp = "You don't have enough money to send that message.";			break;
	default :									gvar.Temp.szTemp = "Can't send your message.";								break;
	}

	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetRebirthRemainTime()
{
	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (pModuleMyControl == NULL)	return 0;

	XMyActionDie* pActionState = dynamic_cast<XMyActionDie*>(pModuleMyControl->GetCurrentActionState());
	if (pActionState == NULL) return 0;

	return pActionState->GetRebirthRemainTime();
}


int XGlueGame::GetSoulBindFieldID()
{
	return gvar.MyInfo.nSoulbindingFieldID;
}


int XGlueGame::GetCheckPointFieldID()
{
	return gvar.MyInfo.nCheckPointFieldID;
}


int XGlueGame::GetRebirthItemID()
{
	XItem* pItem = gvar.MyInfo.Inventory.GetUsableSelfRebirthItem();
	if (NULL == pItem) return 0;

	return pItem->m_pItemData->m_nID;
}

void XGlueGame::RequireRebirth( int nSelect )
{
	switch (nSelect)
	{
	case 1:
		{
			XPostChar_Rebirth( REBT_SOULBINDING);
		}
		break;
	case 2:
		{
			// Quest PvP zone
			XPvPAreaLogic* pPvPAreaLogic = gg.currentgamestate->GetPvPArea();
			if ( pPvPAreaLogic != NULL  &&  pPvPAreaLogic->IsInPvPArea() == true)
				XPostChar_Rebirth( REBT_QPVP);

			else
				XPostChar_Rebirth( REBT_CHECKPOINT);
		}
		break;
	case 3:
		{
			if ( true == gvar.MyInfo.BattleArenaInfo.IsInBattleArena() == true)
			{
				XPostChar_Rebirth(REBT_PLACE_IN_BATTLEGROUD);
			}
			else
			{
				XItem* pItem = gvar.MyInfo.Inventory.GetUsableSelfRebirthItem();
				if ( pItem != NULL)
					gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionUseItem( pItem->m_nSlotID);
			}
		}
		break;
	case 4:
		{
			XPostChar_Rebirth( REBT_NEAR_SOULBINDING);
		}
		break;
	case 5:
		{
			if (gvar.Network.IsServerModeExpo())
			{
				XPostGMRebirth(gvar.Game.pMyPlayer->GetPosition());
			}
		}
		break;
	default:
		{
			_ASSERT(0);
		}
		break;
	}
}


int XGlueGame::GetVirtualKey( const char* szKey)
{
	XVirtualKey nVKey = VKEY_NA;
	if ( gg.controller != NULL)		nVKey = gg.controller->GetVirtualKey( szKey);
	return (int)nVKey;
}


const char* XGlueGame::GetVirtualKeyStr( int nVKey)
{
	gvar.Temp.szTemp = "";
	if ( gg.controller != NULL)		gvar.Temp.szTemp = gg.controller->GetDisplayString( (XVirtualKey)nVKey);
	return gvar.Temp.szTemp.c_str();
}


// void XGlueGame::MouseVisible( bool bVisible )
// {
// 	if (!global.ui) return;
// 
// 	if(global.ui->IsMouseVisible() == bVisible)
// 		return;
// 
// 	gg.controller->ClearKeyBuffer( true);
// 
// 	global.ui->ShowMouseForGame(bVisible); 
// }

const char* XGlueGame::GetNpcName() const
{
	gvar.Temp.szTemp = "";
	XNonPlayer* pNonPlayer = gg.omgr->FindNPC( XGetInteractionInfo().InteractionTargetUID);
	if ( pNonPlayer != NULL)	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pNonPlayer->GetName());
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetNpcWelcomeMsg() const
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.Game.NPCInteractionInfo.GetOpeningString());
	
	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetCountNpcAnswer()
{
	return (int)gvar.Game.NPCInteractionInfo.GetList().size();
}

const char* XGlueGame::GetNpcAnswer( int i)
{
	wstring strDesc;
	XStringResMgr::GetInstance()->TransKeyStr( strDesc, gvar.Game.NPCInteractionInfo.GetList()[ i].szIEText);
		
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strDesc.c_str());
	return gvar.Temp.szTemp.c_str();
}

void XGlueGame::SelectNpcAnswer( int i)
{
	int nElementID = gvar.Game.NPCInteractionInfo.GetList()[ i].nIElementID;

	if ( gvar.Game.NPCInteractionInfo.MenuType() == MENU_NPC)				XPostNPCInteraction_IE( XGetInteractionInfo().InteractionTargetUID, nElementID);
	else if ( gvar.Game.NPCInteractionInfo.MenuType() == MENU_SECTOR)		XPostSensorTriggerSelection( nElementID);
	else if ( gvar.Game.NPCInteractionInfo.MenuType() == MENU_FOLLOW_WARP)	XPosSensorTriggerFollowWarp( nElementID);
}

void XGlueGame::RequestNpcInteractionEnd()
{
	XPostNPCInteraction_End();
}

const char* XGlueGame::GetNpcDialogMessage() const
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.Game.DialogInfo.GetSayString());

	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetCountNpcDialogAnswer()
{
	return (int)gvar.Game.DialogInfo.GetList().size();
}

const char* XGlueGame::GetNpcDialogAnswer( int i)
{
	wstring strDesc;
	XStringResMgr::GetInstance()->TransKeyStr(strDesc, info.dialog_text->Get(gvar.Game.DialogInfo.GetList()[i].nTextID));

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strDesc.c_str());
	return gvar.Temp.szTemp.c_str();
}

void XGlueGame::SelectNpcDialogAnswer(int i)
{
	XPostRequestDialogSelect( i);
}


bool XGlueGame::IsBattleArena()
{
	return gvar.MyInfo.BattleArenaInfo.IsInBattleArena();
}


MWLua::ret_int2 XGlueGame::GetCurrentBattleArenaScore()
{
	return gg.currentgamestate->GetBattleArena()->GetCurrentScore();
}

///< 크래프트
int XGlueGame::GetCraftingRecipeCount()
{
	return gvar.MyInfo.CraftableRecipeInfo.GetRecipeCount();
}


int XGlueGame::GetCraftingRecipeID( int i)
{
 	XRecipeInfo* pRecipeInfo = gvar.MyInfo.CraftableRecipeInfo.GetRecipeInfoByIndex( i);
 	if ( pRecipeInfo == NULL)	return 0;
	return pRecipeInfo->m_nID;
}

int XGlueGame::GetCraftingItemCost( int i)
{
	return gvar.MyInfo.CraftableRecipeInfo.GetPriceByIndex( i);
}

bool XGlueGame::ObtainCraftingRecipe( int i)
{
	XRecipeInfo* pRecipeInfo = gvar.MyInfo.CraftableRecipeInfo.GetRecipeInfoByIndex( i);
	if ( pRecipeInfo == NULL)		return false;
	
	XPostCraft_RecipeInsert( pRecipeInfo->m_nID);
	return true;
}


bool XGlueGame::MakeCraftingItem( int i)
{
	XRecipeInfo* pRecipeInfo = gvar.MyInfo.CraftableRecipeInfo.GetRecipeInfoByIndex( i);
	if ( pRecipeInfo == NULL)		return false;

	XItemData* pItemData = info.item->GetItemData(pRecipeInfo->m_nProductItemID);
	if (pItemData == NULL) return CR_TRUE;

//	if(pItemData->m_ItemSoundType)
//		global.sound->PlayItemSound(pItemData->m_ItemSoundType, false, true);

	XPostCraft_MakeReq( pRecipeInfo->m_nID);
	return true;
}


bool XGlueGame::IsEnabledCraftingItem( int i)
{
	return gvar.MyInfo.CraftableRecipeInfo.IsCraftableItem( i);
}


bool XGlueGame::IsMakableCraftingItem( int i)
{
	if ( gvar.MyInfo.ChrInfo.GetMoney() < gvar.MyInfo.CraftableRecipeInfo.GetPriceByIndex( i))		return false;

	XRecipeInfo* pRecipeInfo = gvar.MyInfo.CraftableRecipeInfo.GetRecipeInfoByIndex( i);
	if ( pRecipeInfo == NULL)		return false;

	for each (RecipeMaterial material in pRecipeInfo->m_vecMaterial)
	{
		int nSum = gvar.MyInfo.Inventory.GetItemAmountByID(material.m_nItemID) +
			gvar.MyInfo.EquipmentSlot.GetItemQuantityByID(material.m_nItemID);

		if (material.m_nItemID > 0  &&
			nSum < material.m_nAmount)		return false;
	}
	
	return true;
}


int XGlueGame::GetRecipeCount()
{
	return gvar.MyInfo.RecipeInfo.GetCount();
}


int XGlueGame::GetRecipeID( int i)
{
	return gvar.MyInfo.RecipeInfo.GetRecipeID( i);
}


int XGlueGame::GetRecipeItemID( int nID)
{
	XRecipeInfo* pRecipeInfo = info.recipe->Get( nID);
	if ( pRecipeInfo == NULL)	return 0;
	return pRecipeInfo->m_nProductItemID;
}

int XGlueGame::GetRecipeItemCost( int nID)
{
	XRecipeInfo* pRecipeInfo = info.recipe->Get( nID);
	if ( pRecipeInfo == NULL)	return 0;
	return pRecipeInfo->m_nPrice;
}


int XGlueGame::GetRecipeItemQuantity( int nID)
{
	XRecipeInfo* pRecipeInfo = info.recipe->Get( nID);
	if ( pRecipeInfo == NULL)	return 0;
	return pRecipeInfo->m_nAmount;
}

const char* XGlueGame::GetRecipeItemName( int nID )
{
	XRecipeInfo* pRecipeInfo = info.recipe->Get( nID);
	if ( pRecipeInfo == NULL)	return NULL;

	XItemData* pItemData = info.item->GetItemData(pRecipeInfo->m_nProductItemID);
	if ( pItemData == NULL) return NULL;

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pItemData->GetName());

	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetRecipeMaterialCount( int nID)
{
	XRecipeInfo* pRecipeInfo = info.recipe->Get( nID);
	if ( pRecipeInfo == NULL)	return 0;
	return (int)pRecipeInfo->m_vecMaterial.size();
}


int XGlueGame::GetRecipeMaterialID( int nID, int nMaterialIndex)
{
	if ( nMaterialIndex >= MAX_RECIPE_MATERIAL_TYPE)	return 0;
	XRecipeInfo* pRecipeInfo = info.recipe->Get( nID);
	if ( pRecipeInfo == NULL)	return 0;
	return pRecipeInfo->m_vecMaterial[ nMaterialIndex].m_nItemID;
}


int XGlueGame::GetRecipeMaterialQuantity( int nID, int nMaterialIndex)
{
	if ( nMaterialIndex >= MAX_RECIPE_MATERIAL_TYPE)	return 0;
	XRecipeInfo* pRecipeInfo = info.recipe->Get( nID);
	if ( pRecipeInfo == NULL)	return 0;
	return pRecipeInfo->m_vecMaterial[ nMaterialIndex].m_nAmount;
}

const char* XGlueGame::GetRecipeConditionText(int nID)
{
	XRecipeInfo* pRecipeInfo = info.recipe->Get( nID);
	if ( pRecipeInfo == NULL || pRecipeInfo->m_nConditionID == 0)	return "";

	gvar.Temp.szTemp = GetConditionText(pRecipeInfo->m_nConditionID);
	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetRecipeArtisanCount( int nID)
{
	return info.recipenpc->GetNPCCount( nID);
}

const char* XGlueGame::GetRecipeArtisanName( int nID, int nIndex)
{	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( info.recipenpc->GetNPCName( nID, nIndex));
	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetRecipeArtisanFieldID( int nID, int nIndex)
{	
	return info.recipenpc->GetNPCFieldID( nID, nIndex);
}

void XGlueGame::RemoveRecipe( int nID)
{
	XPostCraft_RecipeDelete( nID);
}


bool XGlueGame::IsHaveRecipe( int nID)
{
	return gvar.MyInfo.RecipeInfo.IsExist( nID);
}


const char* XGlueGame::GetItemName( int nItemID ) const
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return "";

	if (gvar.Debug.bShowID)
	{
		wchar_t szBuff[128] = {0,};
		swprintf_s(szBuff, L"%s(ID : %d) ", pItemData->GetName(), nItemID);
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(szBuff);
	}
	else
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pItemData->GetName());
	}

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetItemImage( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return "";

	if ( pItemData->m_strIconName.empty())		return "iconUnknown";

	MBitmap* pImage = global.ui->GetIcon(pItemData->m_strIconName);
	if ( pImage == NULL)			return "iconUnknown";
	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pItemData->m_strIconName.c_str());

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetItemDesc( int nItemID ) const
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pItemData->GetDesc());

	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetItemType( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;
	return (int)pItemData->m_ItemType;
}

int XGlueGame::GetItemWeaponType( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;
	return (int)pItemData->m_nWeaponType;
}

int XGlueGame::GetItemArmorType( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;
	return (int)pItemData->m_nArmorType;
}

int XGlueGame::GetItemUsableType( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;
	return (int)pItemData->m_nUsableType;
}

int XGlueGame::GetItemTier( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;
	return (int)pItemData->m_nTier;
}

int XGlueGame::GetItemQuantity( int nItemID) const
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItemByID( nItemID);
	if ( pItem == NULL || pItem->m_pItemData == NULL) return 0;
	return pItem->m_nAmount;
}

int XGlueGame::GetItemDurability( int nItemID)
{
	XItem* pItem = gvar.MyInfo.Inventory.GetItemByID( nItemID);
	if ( pItem == NULL || pItem->m_pItemData == NULL) return 0;
	return pItem->m_nDurability;
}

int XGlueGame::GetItemBuyingCost( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if( pItemData == NULL) return 0;

	XNonPlayer* pPlayer = gg.omgr->FindNPC(XGetInteractionInfo().InteractionTargetUID);
	if (pPlayer == NULL) return 0;

	FACTION_RELATION_TYPE nFRT = gvar.MyInfo.FactionInfo.GetRelation(pPlayer->GetInfo(true)->m_nFactionLossID);

	return CSNPCShopCalculator::CalcBuyPrice(
		pItemData->m_nBaseBuyingPrice
		, gvar.Game.NPCShopInfo.GetBuyMode()
		, nFRT);
}


int XGlueGame::GetItemStackSize( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL)		return 0;
	return pItemData->m_nStackAmount;
}

int XGlueGame::GetItemEnchantType( int nItemID)
{
	for ( int i = 0;  i < 5;  i++)
	{
		if ( ENCHANT_AGENT_ITEMID[i] == nItemID)								return 3;		// Agent
	}

	XItemData* pItemData = info.item->GetItemData( nItemID);
	if (pItemData == NULL  ||  pItemData->m_EnchantStone.nLinkedBuffID == -1)	return 0;		// None
	if ( pItemData->m_EnchantStone.nColor == ENCHANT_NONE)						return 0;		// None
	else if ( pItemData->m_EnchantStone.nColor == ENCHANT_WHITE)				return 2;		// Special stone
	else																		return 1;		// Stone
	return 0;
}

int XGlueGame::GetItemEnchantLevel( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL ||  pItemData->m_EnchantStone.nLinkedBuffID == -1)	return 0;
	return pItemData->m_EnchantStone.nLimitLevel;
}

int XGlueGame::GetItemEnchantHoleCount( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL)		return 0;
	size_t _count = 0;
	size_t _size = pItemData->m_vecEquipEnchantColor.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		if ( pItemData->m_vecEquipEnchantColor[ i] > ENCHANT_NONE)		_count++;
	}
	return _count;
}

int XGlueGame::GetItemEnchantHoleType( int nItemID, int i)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL)		return 0;

	int _index = 0, _count = -1, _size = pItemData->m_vecEquipEnchantColor.size();
	for ( _index = 0;  _index < _size;  _index++)
	{
		if ( pItemData->m_vecEquipEnchantColor[ _index] > ENCHANT_NONE)		_count++;
		if ( _count == i)		break;
	}
	if ( _count == -1)			return 0;

	int _type = 0;
	switch ( pItemData->m_vecEquipEnchantColor[ _index])
	{
	case ENCHANT_NONE :		_type = 0;		break;		// None
	case ENCHANT_WHITE :	_type = 2;		break;		// Special
	default :				_type = 1;		break;		// General
	}

	return _type;
}

MWLua::ret_int2 XGlueGame::GetItemEnchantAgentIDRange( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL  ||  pItemData->m_EnchantStone.nLinkedBuffID == -1  ||  pItemData->m_EnchantStone.nLimitLevel == 0) 
		return MWLua::ret_int2( 0, 0);

	int nIndex = ( ( pItemData->m_EnchantStone.nLimitLevel - 1) / 10);
	if ( nIndex >= 5)		return MWLua::ret_int2( 0, 0);

	return MWLua::ret_int2( ENCHANT_AGENT_ITEMID[ nIndex], ENCHANT_AGENT_ITEMID[ 4]);
}

int XGlueGame::GetItemUnchantableWeaponTypeCount( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL ||
		pItemData->m_EnchantStone.nLinkedBuffID == -1) 
		return 0;

	return (int)pItemData->m_EnchantStone.vecLimitWeapons.size();
}

int XGlueGame::GetItemUnchantableWeaponType( int nItemID, int nIndex )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL ||
		pItemData->m_EnchantStone.nLinkedBuffID == -1) 
		return 0;

	if(nIndex >= (int)pItemData->m_EnchantStone.vecLimitWeapons.size())
		return 0;

	return (int)pItemData->m_EnchantStone.vecLimitWeapons[nIndex];
}

int XGlueGame::GetItemUnchantableArmorSlotCount( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL ||
		pItemData->m_EnchantStone.nLinkedBuffID == -1) 
		return 0;

	return (int)pItemData->m_EnchantStone.vecLimitEquipments.size();
}

int XGlueGame::GetItemUnchantableArmorSlot( int nItemID, int nIndex )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL ||
		pItemData->m_EnchantStone.nLinkedBuffID == -1L) 
		return 0;

	if(nIndex >= (int)pItemData->m_EnchantStone.vecLimitEquipments.size())
		return 0;

	return (int)pItemData->m_EnchantStone.vecLimitEquipments[nIndex];
}

int XGlueGame::GetItemDropperCount( int nItemID)
{
	return (int)info.item_droper->GetDropperCount( nItemID);
}

const char* XGlueGame::GetItemDropperName( int nItemID, int i)
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(info.item_droper->GetDropperName( nItemID, i));

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetItemPutUpSound( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL)		return 0;

	wstring strSound;

	if ( pItemData->m_ItemSoundType != SOUND_TYPE_MAX)							strSound = L"item_" + wstring( XStrings::SoundTypeToString( pItemData->m_ItemSoundType)) + L"_up";
	if ( global.sound->IsExistSample( MLocale::ConvUTF16ToAnsi(strSound.c_str()).c_str()) == false)	strSound = L"item_universal_up";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strSound.c_str());
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetItemPutDownSound( int nItemID)
{
	gvar.Temp.szTemp = XSoundHelper::GetItemPutDownSound(nItemID);
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetItemMakingSound( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL)		return 0;

	wstring strSound;

	if ( pItemData->m_ItemSoundType != SOUND_TYPE_MAX)
	{
		switch ( pItemData->m_ItemSoundType)
		{
		case SOUND_TYPE_LEATHER:
		case SOUND_TYPE_CLOTH:
			strSound = L"item_cloth_make";
			break;
		case SOUND_TYPE_CHAIN:
		case SOUND_TYPE_IRON:
		case SOUND_TYPE_CUTLERY:
		case SOUND_TYPE_HAMMER:
			strSound = L"item_cutlery_make";
			break;
		case SOUND_TYPE_ORGAN:
		case SOUND_TYPE_LIQUID:
			strSound = L"item_food_make";
			break;
		case SOUND_TYPE_JEWEL:
		case SOUND_TYPE_WOOD:
			strSound = L"item_" + wstring( XStrings::SoundTypeToString( pItemData->m_ItemSoundType)) + L"_make";
			break;
		}
	}

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strSound.c_str());

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetMoneyPutUpSound()
{
	gvar.Temp.szTemp = "item_coin_up";
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetMoneyPutDownSound()
{
	gvar.Temp.szTemp = "item_coin_down";
	return gvar.Temp.szTemp.c_str();
}

int XGlueGame::GetItemDyeColorCount( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if (pItemData == NULL)		return 0;

	return pItemData->m_vecDyeColor.size();
}

MWLua::ret_int4 XGlueGame::GetItemDyeColor( int nItemID, int nIndex)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if (pItemData == NULL)		return MWLua::ret_int4( 0, 0, 0, 0);

	MCOLOR _col = info.colorTable->GetColor( L"item_dyed", pItemData->m_vecDyeColor[ nIndex]);
	return MWLua::ret_int4( _col.r, _col.g, _col.b, _col.a);
}

bool XGlueGame::IsItemUnique( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;
	return pItemData->m_bUnique;
}

// bool XGlueGame::IsItemRare( int nItemID )
// {
// 	XItemData* pItemData = info.item->GetItemData(nItemID);
// 	if (pItemData == NULL) return false;
// 
// 	return ITEM_TIER_RARE <= pItemData->m_nTier;
// }

bool XGlueGame::IsItemQuestRelated( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;
	return pItemData->m_bQuestRelated;

}

bool XGlueGame::IsItemRepairable( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;

	return (pItemData->m_bRepairable ? true : false);
}

bool XGlueGame::IsItemTradable( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;

	return pItemData->m_bTradable;
}

bool XGlueGame::IsItemSellable( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;
	return pItemData->m_bSellable;
}

bool XGlueGame::IsItemDiscardable( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;
	return pItemData->m_bDiscardable;
}

bool XGlueGame::IsItemReusable( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if ( pItemData != NULL && CSItemHelper::IsReusableItem( pItemData) == true)	return true;
	return false;
}

bool XGlueGame::IsItemRequiredClaim( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;
	return pItemData->m_bClaimRequired;
}

bool XGlueGame::IsItemDyeable( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;
	return pItemData->m_bDyeable;
}

bool XGlueGame::IsItemRecipeMaterial( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return false;
	return pItemData->m_bRecipeMaterial;
}

// bool XGlueGame::IsItemRecipeTooltipNeed( int nItemID)
// {
// 	if (true == IsItemRecipeWeapon(nItemID)) return true;
// 	if (true == IsItemRecipeShield(nItemID)) return true;
// 	if (true == IsItemRecipeAccessory(nItemID)) return true;
// 	if (true == IsItemRecipeArmor(nItemID)) return true;
// 	if (true == IsItemRecipeUsable(nItemID)) return true;
// 	if (true == IsItemRecipeHousing(nItemID)) return true;
// 	if (true == IsItemRecipeEnchant(nItemID)) return true;
// 
// 	return false;
// }

// bool XGlueGame::IsItemRecipeWeapon( int nItemID )
// {
// 	return info.item_recipeproduct->IsWeaponProductMaterial(nItemID);
// }

// bool XGlueGame::IsItemRecipeShield( int nItemID )
// {
// 	return info.item_recipeproduct->IsShieldProductMaterial(nItemID);
// }

// bool XGlueGame::IsItemRecipeAccessory( int nItemID )
// {
// 	return info.item_recipeproduct->IsAccessoryProductMaterial(nItemID);
// }

// bool XGlueGame::IsItemRecipeArmor( int nItemID )
// {
// 	return info.item_recipeproduct->IsArmorProductMaterial(nItemID);
// }

// bool XGlueGame::IsItemRecipeUsable( int nItemID )
// {
// 	return info.item_recipeproduct->IsUsableProductMaterial(nItemID);
// }

// bool XGlueGame::IsItemRecipeHousing( int nItemID )
// {
// 	return info.item_recipeproduct->IsHousingProductMaterial(nItemID);
// }

// bool XGlueGame::IsItemRecipeEnchant( int nItemID )
// {
// 	return info.item_recipeproduct->IsEnchantProductMaterial(nItemID);
// }

bool XGlueGame::DoTalentRequireFocus( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return false;

	if (pTalentInfo->m_nRequireFocus != TFT_NONE) return true;
	return false;
}

int XGlueGame::GetTalentRank( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return (int)pTalentInfo->m_nRank;
}

int XGlueGame::GetTalentStyle( int nTalentID)
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;
	return (int)pTalentInfo->m_nStyle;
}

bool XGlueGame::IsTalentFocus( int nTalentID)
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if ( pTalentInfo == NULL) return false;

	if ( pTalentInfo->m_nExtraPassive == TEPT_FOCUS_USE_BERSERK  ||
		 pTalentInfo->m_nExtraPassive == TEPT_FOCUS_USE_COUNTER  ||
		 pTalentInfo->m_nExtraActive == TEAT_ADVENT)
		 return true;
	return false;
}

/*
const char* XGlueGame::GetTalentStyleString( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return "";

	wstring strStyle;
	
	switch (pTalentInfo->m_nStyle)
	{
	case TS_DEFENDER:	strStyle = XGetStr(L"TALENT_STYLE_DEFENDER");	break;
	case TS_BERSERKER:	strStyle = XGetStr(L"TALENT_STYLE_BESERKER");	break;
	case TS_ASSASSIN:	strStyle = XGetStr(L"TALENT_STYLE_ASSASSIN");	break;
	case TS_RANGER:		strStyle = XGetStr(L"TALENT_STYLE_RANGER");		break;
	case TS_SORCERER:	strStyle = XGetStr(L"TALENT_STYLE_SORCERER");	break;
	case TS_CLERIC:		strStyle = XGetStr(L"TALENT_STYLE_CLERIC");		break;
	}

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strStyle.c_str());

	return gvar.Temp.szTemp.c_str();
}
*/
const char* XGlueGame::GetTalentCategory( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return "";

	wstring strCategory;

	switch (pTalentInfo->m_nCategory)
	{
	case TC_COMMON:	strCategory = XGetStr(L"TALENT_CATEGORY_COMMON");	break;
	case TC_MELEE:	strCategory = XGetStr(L"TALENT_CATEGORY_MELEE");	break;
	case TC_RANGE:	strCategory = XGetStr(L"TALENT_CATEGORY_RANGE");	break;
	case TC_MAGIC:	strCategory = XGetStr(L"TALENT_CATEGORY_MAGIC");	break;
	}

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strCategory.c_str());

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetTalentDesc( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pTalentInfo->GetDesc());

	return gvar.Temp.szTemp.c_str();
}

bool XGlueGame::IsTalentPassiveType( int nTalentID)
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if ( pTalentInfo == NULL)	return false;
	return pTalentInfo->IsPassiveSkill();
}

const char* XGlueGame::GetString( const char* szKey )
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(XGetStr(MLocale::ConvAnsiToUTF16(szKey)));

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetFormatStr(const char* szKey, const char* szArg1, const char* szArg2, const char* szArg3, const char* szArg4, const char* szArg5)
{
	wstring pArgs[5];
	pArgs[0] = MLocale::ConvAnsiToUTF16(szArg1);
	pArgs[1] = MLocale::ConvAnsiToUTF16(szArg2);
	pArgs[2] = MLocale::ConvAnsiToUTF16(szArg3);
	pArgs[3] = MLocale::ConvAnsiToUTF16(szArg4);
	pArgs[4] = MLocale::ConvAnsiToUTF16(szArg5);

	wstring strMsg = CSFormatString::Format(XGetStr( MLocale::ConvAnsiToUTF16( szKey)), FSParam(pArgs[0], pArgs[1], pArgs[2], pArgs[3], pArgs[4]));
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strMsg);

	return gvar.Temp.szTemp.c_str();


	//string str = MLocale::ConvUTF16ToAnsi( XGetStr( MLocale::ConvAnsiToUTF16( szKey)));
	//const char* pArgs[] = { szArg1, szArg2, szArg3, szArg4, szArg5 };
	//
	//char _buff[ 4];
	//for ( int i = 0;  i < 5;  i++)
	//{
	//	if ( pArgs[ i] == NULL)			break;
	//
	//	sprintf_s( _buff, "{%d}", i);
	//	size_t _find = str.find( _buff);
	//	if ( _find == string::npos)		break;
	//
	//	str.replace( _find, 3, pArgs[ i]);
	//}
	//
	//gvar.Temp.szTemp = str;
	//return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetResultString( int nResult )
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(XCmdResultStr(nResult));

	return gvar.Temp.szTemp.c_str();
}

void XGlueGame::GetResultSound( int nResult )
{
	XCmdResultSound(nResult);
}

const char* XGlueGame::GetTalentName( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if (pTalentInfo == NULL) return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pTalentInfo->GetName());

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetTalentImage( int nTalentID)
{
	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);
	if ( pTalentInfo == NULL)	return "";

	wstring strIcon = pTalentInfo->m_szIcon;	

	if ( strIcon.empty() &&  pTalentInfo->m_nTalentLine > 0)
	{
		pTalentInfo = info.talent->Get( pTalentInfo->m_nTalentLine);
		if ( pTalentInfo != NULL)	strIcon = pTalentInfo->m_szIcon;
	}

	MBitmap* pImage = global.ui->GetIcon(strIcon);
	if ( pImage == NULL)			return "iconUnknown";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strIcon.c_str());
	return gvar.Temp.szTemp.c_str();
}


const char* XGlueGame::GetTalentPutUpSound( int nTalentID)
{
	gvar.Temp.szTemp = "item_universal_up";
	return gvar.Temp.szTemp.c_str();
}


const char* XGlueGame::GetTalentPutDownSound( int nTalentID)
{
	gvar.Temp.szTemp = "item_universal_down";
	return gvar.Temp.szTemp.c_str();
}


bool XGlueGame::DoTalentRequireWeaponType( int nTalentID, int nWeaponType )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return false;

	return pTalentInfo->m_WeaponAllowedInfo.IsAllowed(WEAPON_TYPE(nWeaponType), false);
}

int XGlueGame::GetTalentCostHP( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_nHPCost;
}

int XGlueGame::GetTalentCostEN( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_nENCost;
}

int XGlueGame::GetTalentCostSP( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_nSTACost;
}

float XGlueGame::GetTalentCastingTime( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_fCastingTime;
}

float XGlueGame::GetTalentCoolTime( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_fCoolTime;
}

int XGlueGame::GetTalentLearnReqLevel( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_nTNeedLevel;
}

int XGlueGame::GetTalentLearnReqStyleTP( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_nTNeedStyleTP;
}


int XGlueGame::GetLearnedTalentCount()
{
	return (int)gvar.MyInfo.Talent.GetTalentCount();
}


int XGlueGame::GetLearnedTalentID( int i)
{
	return gvar.MyInfo.Talent.GetTalentID( i);
}


int XGlueGame::GetLastLearnedRankedTalentID( int nTalentID)
{
	XTalentInfo* pCurInfo = info.talent->Get( nTalentID);
	if ( pCurInfo == NULL)		return 0;

	XTalentInfo* pTalentInfo = pCurInfo;
	for ( int i = nTalentID + 1;   ;  i++)
	{
		if ( gvar.MyInfo.Talent.IsLearnedTalent( i) == false)	break;

		XTalentInfo* pInfo = info.talent->Get( i);
		if ( pInfo == NULL)		break;
		if ( pCurInfo->m_nTalentLine != pInfo->m_nTalentLine)	break;

		pTalentInfo = pInfo;
	}

	return pTalentInfo->m_nID;
}


int XGlueGame::GetEmotionalTalentCount()
{
	return gvar.MyInfo.Talent.GetEmotionalTalentCount();
}


int XGlueGame::GetEmotionalTalentID( int i)
{
	return gvar.MyInfo.Talent.GetEmotionalTalentID( i);
}


bool XGlueGame::IsLearnedTalent( int nTalentID )
{
	return gvar.MyInfo.Talent.IsLearnedTalent(nTalentID);
}


int XGlueGame::GetBuffCount()
{
	return (int)gvar.MyInfo.BuffList.Size();
}


int XGlueGame::GetBuffID( int i)
{
	return gvar.MyInfo.BuffList.GetItemID( i);
}


const char* XGlueGame::GetBuffImage( int nID)
{
	XBuffInfo* pBuffInfo = info.buff->Get( nID);
	if ( pBuffInfo == NULL)		return "";

	if ( pBuffInfo->m_szIcon.empty() == true)		return "iconUnknown";


	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pBuffInfo->m_szIcon.c_str());

	if ( MBitmapManager::Get( gvar.Temp.szTemp.c_str()) == NULL)
	{
		MBitmap* pBitmap = MGetMint()->NewBitmap( gvar.Temp.szTemp.c_str(), gvar.Temp.szTemp.c_str());
		if ( pBitmap != NULL)			MBitmapManager::Add( pBitmap);
	}

	return gvar.Temp.szTemp.c_str();
}


bool XGlueGame::IsDebuff( int nID)
{
	XBuffInfo* pBuffInfo = info.buff->Get( nID);
	if ( pBuffInfo == NULL)		return false;
	return ( pBuffInfo->m_nType == BUFT_DEBUFF);
}


void XGlueGame::Quit()
{
	if (global.app)
		PostMessage(global.app->GetWindowHandle(), WM_CLOSE, 0, 0);
}


void XGlueGame::ExitToCharSelection()
{
	// to do...
	XPostComm_StartMoveToLoginServerReq();
}

bool XGlueGame::IsServerModeExpo()
{
	bool bIsServerModeExpo = gvar.Network.IsServerModeExpo();
	return bIsServerModeExpo;
}

int XGlueGame::GetItemSlot( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return ITEMSLOT_NONE;

	return (int)pItemData->m_nItemSlot;
}

int XGlueGame::GetItemMaxDurability( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return ITEMSLOT_NONE;

	return pItemData->m_nMaxDurability;

}

int XGlueGame::GetItemMinDamage( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;

	return pItemData->m_nMinDamage;
}

int XGlueGame::GetItemMaxDamage( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;

	return pItemData->m_nMaxDamage;
}

float XGlueGame::GetItemSpellPower( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 1.0f;

	return pItemData->m_fSpellPower;

}

int XGlueGame::GetItemArmorPoint( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;

	return pItemData->m_nAP;
}

int XGlueGame::GetItemReuseTime( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;

	if (pItemData->m_nUsableType == USABLE_TALENT_USE)
	{

		int nBindTalentID = pItemData->m_vecUsableParam.front();

		XTalentInfo* pTalentInfo = info.talent->Get(nBindTalentID);
		if (pTalentInfo == NULL) return 0;

		return (int)pTalentInfo->m_fCoolTime;
	}

	return 0;
}

int XGlueGame::GetItemEquipReqLevel( int nItemID )
{
	/*XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;

	int nReqMinLevel = info.cond_item->GetMinLevel_For_CT_LEVEL(pItemData->m_nConditionsInfoID);

	return nReqMinLevel;*/
	return 0;
}

int XGlueGame::GetItemEquipReqMaxLevel( int nItemID )
{
	/*XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL) return 0;

	int nReqMaxLevel = info.cond_item->GetMaxLevel_For_CT_LEVEL(pItemData->m_nConditionsInfoID);

	return nReqMaxLevel;*/
	return 0;
}

MWLua::table XGlueGame::GetItemEffects( int nItemID )
{
	MWLua::table t(global.script->GetLua()->GetState());

	XItemData* pItemData = info.item->GetItemData(nItemID);
	if ( pItemData == NULL)			return t;

	XBuffInfo * pBuffInfo = NULL;

	bool bEnchantStone = false;
	if ( pItemData->m_ItemType == ITEMTYPE_ENCHANTSTONE)
	{
		pBuffInfo = info.buff->Get( pItemData->m_EnchantStone.nLinkedBuffID);
		if ( pBuffInfo == NULL)		return t;

		bEnchantStone = true;
	}

	if ( bEnchantStone == false ? pItemData->m_PlayerModifier.IsModified() : pBuffInfo->m_PlayerModifier.IsModified())
	{
		CSPlayerModifier& modifier = bEnchantStone == false ? pItemData->m_PlayerModifier : pBuffInfo->m_PlayerModifier;

		t.set(XML_ATTR_MOD_STR, modifier.nSTR.nMod);
		t.set(XML_ATTR_MOD_DEX, modifier.nDEX.nMod);
		t.set(XML_ATTR_MOD_INT, modifier.nINT.nMod);
		t.set(XML_ATTR_MOD_CHA, modifier.nCHA.nMod);
		t.set(XML_ATTR_MOD_CON, modifier.nCON.nMod);
	}

	if ( bEnchantStone == false ? pItemData->m_ActorModifier.IsModified() : pBuffInfo->m_ActorModifier.IsModified())
	{
		CSActorModifier& modifier = bEnchantStone == false ? pItemData->m_ActorModifier : pBuffInfo->m_ActorModifier;

		t.set(XML_ATTR_MOD_HP_MAX, modifier.nHPMax.nMod);
		t.set(XML_ATTR_MOD_EN_MAX, modifier.nENMax.nMod);
		t.set(XML_ATTR_MOD_STA_MAX, modifier.nSTAMax.nMod);

		t.set(XML_ATTR_MOD_MOVE_SPEED, modifier.fMoveSpeed);
		t.set(XML_ATTR_MOD_HP_REGEN, modifier.nHPRegen.nMod);
		t.set(XML_ATTR_MOD_EN_REGEN, modifier.nENRegen.nMod);
		t.set(XML_ATTR_MOD_STA_REGEN, modifier.nSTARegen.nMod);

		t.set(XML_ATTR_MOD_AP, modifier.nAP.nMod);
		t.set(XML_ATTR_MOD_ABS, modifier.nABS.nMod);
		t.set(XML_ATTR_MOD_GUARDRATE, modifier.fGuardRate.fPer);

		t.set(XML_ATTR_MOD_CRITICAL_AMP, modifier.fCriticalAmp);

		t.set(XML_ATTR_MOD_CRITICAL_MELEE_DAMAGE_AMP, modifier.fCriticalMeleeDamageAmp);
		t.set(XML_ATTR_MOD_CRITICAL_RANGE_DAMAGE_AMP, modifier.fCriticalRangeDamageAmp);
		t.set(XML_ATTR_MOD_CRITICAL_MAGIC_DAMAGE_AMP, modifier.fCriticalMagicDamageAmp);

		t.set(XML_ATTR_MOD_MELEE_ATK_AMP, modifier.fMeleeDamageAmp);
		t.set(XML_ATTR_MOD_RANGE_ATK_AMP, modifier.fRangeDamageAmp);
		t.set(XML_ATTR_MOD_MAGIC_ATK_AMP, modifier.fMagicDamageAmp);

		t.set(XML_ATTR_MOD_DEF_SLASH_AMP, modifier.fDefenceAmp[DA_SLASH]);
		t.set(XML_ATTR_MOD_DEF_BLUNT_AMP, modifier.fDefenceAmp[DA_BLUNT]);
		t.set(XML_ATTR_MOD_DEF_PIERCE_AMP, modifier.fDefenceAmp[DA_PIERCE]);
		t.set(XML_ATTR_MOD_DEF_HOLY_AMP, modifier.fDefenceAmp[DA_HOLY]);
		t.set(XML_ATTR_MOD_DEF_UNHOLY_AMP, modifier.fDefenceAmp[DA_UNHOLY]);
		t.set(XML_ATTR_MOD_DEF_FIRE_AMP, modifier.fDefenceAmp[DA_FIRE]);
		t.set(XML_ATTR_MOD_DEF_COLD_AMP, modifier.fDefenceAmp[DA_COLD]);
		t.set(XML_ATTR_MOD_DEF_LIGHTNING_AMP, modifier.fDefenceAmp[DA_LIGHTNING]);
		t.set(XML_ATTR_MOD_DEF_POISON_AMP, modifier.fDefenceAmp[DA_POISON]);

		t.set(XML_ATTR_MOD_FR, modifier.nResistances[TRT_FR].nMod);
		t.set(XML_ATTR_MOD_CR, modifier.nResistances[TRT_CR].nMod);
		t.set(XML_ATTR_MOD_PR, modifier.nResistances[TRT_PR].nMod);
		t.set(XML_ATTR_MOD_LR, modifier.nResistances[TRT_LR].nMod);
		t.set(XML_ATTR_MOD_HR, modifier.nResistances[TRT_HR].nMod);
		t.set(XML_ATTR_MOD_UR, modifier.nResistances[TRT_UR].nMod);

		t.set(XML_ATTR_MOD_FR_RATE, modifier.nResistances[TRT_FR].fPer);
		t.set(XML_ATTR_MOD_CR_RATE, modifier.nResistances[TRT_CR].fPer);
		t.set(XML_ATTR_MOD_PR_RATE, modifier.nResistances[TRT_PR].fPer);
		t.set(XML_ATTR_MOD_LR_RATE, modifier.nResistances[TRT_LR].fPer);
		t.set(XML_ATTR_MOD_HR_RATE, modifier.nResistances[TRT_HR].fPer);
		t.set(XML_ATTR_MOD_UR_RATE, modifier.nResistances[TRT_UR].fPer);

		t.set(NPC_XML_ATTR_BEATEN_MOD_VALUE, modifier.MotionFactorValue[MF_BEATEN].nMod);
		t.set(NPC_XML_ATTR_KNOCKBACK_MOD_VALUE, modifier.MotionFactorValue[MF_KNOCKBACK].nMod);
		t.set(NPC_XML_ATTR_STUN_MOD_VALUE, modifier.MotionFactorValue[MF_STUN].nMod);
		t.set(NPC_XML_ATTR_DEFLECT_MOD_VALUE, modifier.MotionFactorValue[MF_DEFLECT].nMod);		
		t.set(NPC_XML_ATTR_THROWUP_MOD_VALUE, modifier.MotionFactorValue[MF_THROWUP].nMod);
		t.set(NPC_XML_ATTR_KNOCKDOWN_MOD_VALUE, modifier.MotionFactorValue[MF_KNOCKDOWN].nMod);
		t.set(NPC_XML_ATTR_UPPERED_MOD_VALUE, modifier.MotionFactorValue[MF_UPPERED].nMod);

		t.set(NPC_XML_ATTR_BEATEN_MOD_WEIGHT, modifier.MotionFactorWeight[MF_BEATEN].nMod);
		t.set(NPC_XML_ATTR_KNOCKBACK_MOD_WEIGHT, modifier.MotionFactorWeight[MF_KNOCKBACK].nMod);
		t.set(NPC_XML_ATTR_STUN_MOD_WEIGHT, modifier.MotionFactorWeight[MF_STUN].nMod);
		t.set(NPC_XML_ATTR_DEFLECT_MOD_WEIGHT, modifier.MotionFactorWeight[MF_DEFLECT].nMod);		
		t.set(NPC_XML_ATTR_THROWUP_MOD_WEIGHT, modifier.MotionFactorWeight[MF_THROWUP].nMod);
		t.set(NPC_XML_ATTR_KNOCKDOWN_MOD_WEIGHT, modifier.MotionFactorWeight[MF_KNOCKDOWN].nMod);
		t.set(NPC_XML_ATTR_UPPERED_MOD_WEIGHT, modifier.MotionFactorWeight[MF_UPPERED].nMod);
	}

	return t;
}

int XGlueGame::GetItemReqLicense( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL)		return true;

	/*TALENT_EXTRA_PASSIVE_TYPE _type = CSItemHelper::GetTEPTForEquip( pItemData);
	if ( _type == TEPT_NONE  ||  CSItemHelper::IsMustCheckPassiveType( _type) == false  ||  info.talent->IsGivedExtraPassive( _type) == true)
		_type = TEPT_NONE;*/
	TALENT_EXTRA_PASSIVE_TYPE _type = TEPT_NONE;
	return _type;
}

bool XGlueGame::HasItemReqLicense( int nItemID, int nLicense)
{
	return true;
	/*XItemData* pItemData = info.item->GetItemData( nItemID);
	if ( pItemData == NULL  ||  nLicense == TEPT_NONE)		return true;
	return ( gvar.MyInfo.Talent.HasExtraPassiveTalent( (TALENT_EXTRA_PASSIVE_TYPE)nLicense));*/
}


int XGlueGame::GetItemProductCount( int nItemID)
{
	return info.item_recipeproduct->GetProductCount( nItemID);
}


int XGlueGame::GetItemProductID( int nItemID, int i)
{
	return info.item_recipeproduct->GetProductItemID( nItemID, i);
}

//SoulHunterZ
int XGlueGame::GetItemExp( int nItemID, bool bEquipSlot )
{
	if(bEquipSlot)
	{
		XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nItemID );
		if(!pItem)
			return -1; //Does not exist
		else
			return pItem->m_nEXP;
	}
	else
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItem( nItemID );
		if(!pItem)
			return -1; //Does not exist
		else
			return pItem->m_nEXP;
	}
}

int XGlueGame::GetReqXP( int nItemID, bool bEquipSlot )
{
	if(bEquipSlot)
	{
		XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nItemID );
		if(!pItem)
			return -1; //Does not exist
		else
			return pItem->m_nNextAttuneXP;
	}
	else
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItem( nItemID );
		if(!pItem)
			return -1; //Does not exist
		else
			return pItem->m_nNextAttuneXP;
	}
}

int XGlueGame::GetAttuneLvl( int nItemID, bool bEquipSlot )
{
	if(bEquipSlot)
	{
		XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nItemID );
		if(!pItem)
			return -1; //Does not exist
		else
			return pItem->m_nAttuneLvl;
	}
	else
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItem( nItemID );
		if(!pItem)
			return -1; //Does not exist
		else
			return pItem->m_nAttuneLvl;
	}
}

// Field
int XGlueGame::GetCurrentFieldID()
{
	return gvar.World.GetFieldID();
}


// MWLua::ret_int2 XGlueGame::GetFieldPosition()
// {
// 	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
// 	if ( pMyPlayer == NULL)		return MWLua::ret_int2( 0, 0);
// 	return MWLua::ret_int2( pMyPlayer->GetPosition().x, pMyPlayer->GetPosition().y);
// }


int XGlueGame::GetFieldListCount()
{
	return (int)info.field->GetFieldList().size();
}


int XGlueGame::GetFieldID( int i)
{
	CSFieldList& vField = info.field->GetFieldList();

	map< int, CSFieldFileInfo*>::iterator itr = vField.begin();

	int _size = (int)vField.size();
	for ( int index = 0;  index < _size;  index++, itr++)
	{
		if ( index != i)	continue;

		return (*itr).second->nFieldID;
	}
	return 0;
}


const char* XGlueGame::GetFieldName( int nID)
{
	gvar.Temp.szTemp.clear();
	CSFieldFileInfo* pFieldInfo = info.field->GetFieldList().Find( nID);
	if ( pFieldInfo)		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(XGetStr(pFieldInfo->strName));
	return gvar.Temp.szTemp.c_str();
}


bool XGlueGame::IsDynamicField( int nID)
{
	CSFieldFileInfo* pFieldInfo = info.field->GetFieldList().Find( nID);
	if ( pFieldInfo)		return pFieldInfo->bDynamic;
	return false;
}

bool XGlueGame::IsShowFieldMap( int nID)
{
	CSFieldFileInfo* pFieldInfo = info.field->GetFieldList().Find( nID);
	if (pFieldInfo)			return pFieldInfo->bShowMap;
	return false;
}

bool XGlueGame::IsCurrentPvPField()
{
	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pFieldInfo)	return pFieldInfo->m_PvPAreas.empty() ? false : true;
	return false;
}

int XGlueGame::GetDayTime()
{
	if (gvar.Game.nLocalTime != TIME_INVALID)
	{
		return gvar.Game.nLocalTime;
	}
	return gvar.World.nGameTime;
}


int XGlueGame::GetWeather()
{
	if (gvar.Game.nLocalWeather != WEATHER_INVALID)
	{
		return gvar.Game.nLocalWeather;
	}
	return gvar.World.nWeather;
}

// void XGlueGame::ShowCraftNPCPos(int nRecipeID, int nIndex)
// {
// 	XRecipeNPCInfo* pNPCInfo = info.recipenpc->Get(nRecipeID);
// 	if (pNPCInfo == NULL) return;
// 
// 	int nFieldID = pNPCInfo->m_vecFieldNPC[nIndex].first;
// 	int nNPCID = pNPCInfo->m_vecFieldNPC[nIndex].second;
// 	
// 	XUIFieldMap* pFieldmap = (XUIFieldMap*)MGetMint()->FindWidget( "mapFieldMap");
// 	if (NULL == pFieldmap) return;	
// 
// 	pFieldmap->UpdateInfo(nFieldID);
// 	pFieldmap->ShowCraftNPCPos(nNPCID);	
// 
// 	XUIMiniMap* pMinimap = (XUIMiniMap*)MGetMint()->FindWidget("mapMiniMap");
// 	if (NULL == pMinimap) return;
// 	pMinimap->ShowCraftNPCPos(nFieldID, nNPCID);
// 
// 	gvar.Game.pMyPlayer->GetModuleMyControl()->SetRecipeInfo(nFieldID, nNPCID);
// }

// Channel
int XGlueGame::GetCurrentChannel()
{
	return gvar.World.nChannelID;
}


void XGlueGame::RequestChannelList()
{
	XPostChannel_GetChannelList();
}


int XGlueGame::GetChannelListCount()
{
	return (int)gvar.World.ChannelList.size();
}


int XGlueGame::GetChannelID( int i)
{
	return gvar.World.ChannelList[ i].first;
}


int XGlueGame::GetChannelState( int i)
{
	return gvar.World.ChannelList[ i].second;
}


void XGlueGame::MoveToChannel( int nID)
{
	XPostChannel_ChangeChannelPrepare( nID);
}


void XGlueGame::CancelMoveToChannel()
{
	gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionIdleWithSpecialState();

	XPostChannel_ChangeChannelCancel();
}

void XGlueGame::LeaveDynamicField()
{
	gvar.Game.pMyPlayer->GetModuleMyControl()->StopChallengerQuestExitTimer();

	XPostChannel_LeaveDynamicField();
}

bool XGlueGame::IsChannelChangeable()
{
	if ( gvar.Game.pMyPlayer == NULL)	return false;

	if ( XGetInteractionInfo().m_bNPCInteractionState == true)
		return false;

	if ( gvar.Game.pMyPlayer->IsDead() == true)
		return false;

	return true;
}

bool XGlueGame::IsCombat()
{
	return gvar.Game.IsCombatingNow();
}

// Trade
void XGlueGame::CancelTrade()
{
	XPostTrade_Cancel();
}

int XGlueGame::GetMaxTradableItemCount()
{
	return MAX_TRADE_ITEM_SPACE;
}

int XGlueGame::GetMyTradeItemCount()
{
	return (int)gvar.MyInfo.TradeContents.m_MyTradeContainer.GetSize();
}

int XGlueGame::GetMyTradeItemID( int i)
{
	XItem* pItem = gvar.MyInfo.TradeContents.m_MyTradeContainer.GetItem( i);
	if ( pItem == NULL)		return 0;
	return pItem->m_pItemData->m_nID;
}

int XGlueGame::GetMyTradeItemQuantity( int i)
{
	XItem* pItem = gvar.MyInfo.TradeContents.m_MyTradeContainer.GetItem( i);
	if ( pItem == NULL)		return 0;
	return pItem->m_nAmount;
}

int XGlueGame::GetMyTradeMoney()
{
	return gvar.MyInfo.TradeContents.m_nMySilver;
}

int XGlueGame::GetOtherTradeItemCount()
{
	return (int)gvar.MyInfo.TradeContents.m_OtherTradeContainer.GetSize();
}

int XGlueGame::GetOtherTradeItemID( int i)
{
	XItem* pItem = gvar.MyInfo.TradeContents.m_OtherTradeContainer.GetItem( i);
	if ( pItem == NULL)		return 0;
	return pItem->m_pItemData->m_nID;
}

int XGlueGame::GetOtherTradeItemQuantity( int i)
{
	XItem* pItem = gvar.MyInfo.TradeContents.m_OtherTradeContainer.GetItem( i);
	if ( pItem == NULL)		return 0;
	return pItem->m_nAmount;
}

int XGlueGame::GetOtherTradeMoney()
{
	return gvar.MyInfo.TradeContents.m_nOtherSilver;
}

void XGlueGame::PutUpTradeItem( int i, int nQuantity)
{
	if ( gvar.MyInfo.TradeContents.m_MyTradeContainer.GetSize() >= MAX_TRADE_ITEM_SPACE)
	{
		global.ui->OnSystemMsg(L"거래창에 올릴 수 있는 최대 아이템의 종류는 {0}개 입니다.", FSParam(MAX_TRADE_ITEM_SPACE));
		return;
	}

	XItem* pItem = gvar.MyInfo.Inventory.GetItem( i);
	if ( pItem == NULL)									return;
	if ( pItem->m_bClaimed == true)		return;

	int nPutUpItemQuantity = nQuantity;
	int nTradeItemQuantity = 0;

	XItem* pTradeItem = NULL;
	size_t _size = gvar.MyInfo.TradeContents.m_MyTradeContainer.GetSize();
	for ( size_t _index = 0;  _index < _size;  _index++)
	{
		XItem* _item = gvar.MyInfo.TradeContents.m_MyTradeContainer.GetItem( _index);
		if ( _item->m_nSlotID == pItem->m_nSlotID)
		{
			pTradeItem = _item;
			break;
		}
	}

	if ( NULL != pTradeItem)
		nTradeItemQuantity = pTradeItem->m_nAmount;

	if ( nPutUpItemQuantity > pItem->m_nAmount - nTradeItemQuantity)
		nPutUpItemQuantity = pItem->m_nAmount - nTradeItemQuantity;

	if ( nPutUpItemQuantity <= 0)
		return;

	XPostTrade_PutUpItem( pItem->m_nSlotID, nPutUpItemQuantity);
}

void XGlueGame::PutDownTradeItem( int i)
{
	XItem* pItem = gvar.MyInfo.TradeContents.m_MyTradeContainer.GetItem( i);
	if ( pItem == NULL)		return;

	XPostTrade_PutDownItem(pItem->m_nSlotID, pItem->m_nAmount);
}

void XGlueGame::PutUpMoney( int nSilver)
{
	XPostTrade_PutUpSilver( nSilver);
}

void XGlueGame::PutDownMoney( int nSilver)
{
	XPostTrade_PutUpSilver( 0);
}

void XGlueGame::ReadyToTrade( bool bReady)
{
	if ( bReady == true)
	{
		XPostTrade_Confirm();
	}
}

bool XGlueGame::IsReadyToTrade()
{
	return gvar.MyInfo.TradeContents.m_bMyConfirm;
}

bool XGlueGame::IsOtherReadyToTrade()
{
	return gvar.MyInfo.TradeContents.m_bOtherConfirm;
}


void XGlueGame::PlaySound(const char* szSoundName)
{
	if (szSoundName == NULL) return;

	if ( global.sound)
		global.sound->Play( szSoundName);
}

void XGlueGame::PlayBGM( const char* szSoundName )
{
	if (szSoundName == NULL) return;

	if ( global.sound)
		global.sound->SetDefalutBGM( szSoundName);

	//global.sound->Play( szSoundName, "bgm_battle");
}

void XGlueGame::ReleaseHoldMyCharacter()
{
	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return;
	if( ACTION_STATE_HOLD != pModuleMyControl->GetCurrentActionStateID()) return;

	XEvent msg;
	msg.m_nID = XEVTL_MYACTION_RELEASE_ACTION_HOLD;
	gg.omgr->PostEvent(gvar.MyInfo.MyUID, msg);
}

void XGlueGame::ChangeWeather(int nWeather)
{
	if ((gg.currentgamestate)
		&& (gg.currentgamestate->GetWorld())
		&& (gvar.Game.nLocalWeather == WEATHER_INVALID))
	{
		gg.currentgamestate->GetWorld()->OnCmdSetWeather((GAME_WEATHER_TYPE)nWeather,
			true, GAME_ENVIRONMENT_FADING_TIME);

		gvar.Game.nLocalWeather = (GAME_WEATHER_TYPE)nWeather;

		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "DAYTIME");
		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "WEATHER");
	}
}

void XGlueGame::ChangeCurrentWeather()
{
	if ((gg.currentgamestate)
		&& (gg.currentgamestate->GetWorld()))
	{
		gg.currentgamestate->GetWorld()->OnCmdSetEnv(L"default",
			(GAME_TIME_TYPE)gvar.World.nGameTime, (GAME_WEATHER_TYPE)gvar.World.nWeather,
			true, GAME_ENVIRONMENT_FADING_TIME);

		gvar.Game.nLocalWeather = WEATHER_INVALID;
		gvar.Game.nLocalTime	= TIME_INVALID;

		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "DAYTIME");
		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "WEATHER");
	}
}

void XGlueGame::ChangeCustomEnv( const char* szEnvname)
{
	if ((gg.currentgamestate)
		&& (gg.currentgamestate->GetWorld()))
	{
		gg.currentgamestate->GetWorld()->OnCmdSetEnv(MLocale::ConvAnsiToUTF16(szEnvname).c_str(),
			(GAME_TIME_TYPE)gvar.World.nGameTime, (GAME_WEATHER_TYPE)gvar.World.nWeather,
			true, GAME_ENVIRONMENT_FADING_TIME);
	}
}

void XGlueGame::ChangeCustomTimeWeather( const char* szEnvname, int nTime, int nWeather)
{
	if ((gg.currentgamestate)
		&& (gg.currentgamestate->GetWorld()))
	{
		if(gg.currentgamestate->GetWorld()->IsEnvSet(szEnvname) == false)	return;

		gg.currentgamestate->GetWorld()->OnCmdSetEnv(MLocale::ConvAnsiToUTF16(szEnvname).c_str(),
			(GAME_TIME_TYPE)nTime, (GAME_WEATHER_TYPE)nWeather,
			true, GAME_ENVIRONMENT_FADING_TIME);

		gvar.Game.nLocalWeather = (GAME_WEATHER_TYPE)nWeather;
		gvar.Game.nLocalTime	= (GAME_TIME_TYPE)nTime;

		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "DAYTIME");
		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "WEATHER");
	}
}

void XGlueGame::ChangeCustomWeather( const char* szEnvname, int nWeather)
{
	if ((gg.currentgamestate)
		&& (gg.currentgamestate->GetWorld())
		&& (gvar.Game.nLocalWeather == WEATHER_INVALID))
	{
		gg.currentgamestate->GetWorld()->OnCmdSetEnv(MLocale::ConvAnsiToUTF16(szEnvname).c_str(),
			(GAME_TIME_TYPE)gvar.World.nGameTime, (GAME_WEATHER_TYPE)nWeather,
			true, GAME_ENVIRONMENT_FADING_TIME);

		gvar.Game.nLocalWeather = (GAME_WEATHER_TYPE)nWeather;

		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "DAYTIME");
		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "WEATHER");
	}
}

void XGlueGame::ChangeCustomTime( const char* szEnvname, int nTime)
{
	if ((gg.currentgamestate)
		&& (gg.currentgamestate->GetWorld())
		&& (gvar.Game.nLocalTime == TIME_INVALID))
	{
		gg.currentgamestate->GetWorld()->OnCmdSetEnv(MLocale::ConvAnsiToUTF16(szEnvname).c_str(),
			(GAME_TIME_TYPE)nTime, (GAME_WEATHER_TYPE)gvar.World.nWeather,
			true, GAME_ENVIRONMENT_FADING_TIME);

		gvar.Game.nLocalTime = (GAME_TIME_TYPE)nTime;

		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "DAYTIME");
		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "WEATHER");
	}
}

int XGlueGame::GetChallengerQuestCount()
{
	return gvar.Game.vecQuestList.size();
}

int XGlueGame::GetChallengerQuestID( int nIndex )
{
	if(gvar.Game.vecQuestList.size() <= (size_t)nIndex)	return 0;

	return gvar.Game.vecQuestList[nIndex].nQuestID;
}

int XGlueGame::GetChallengerQuestEnableType( int nIndex )
{
	if(gvar.Game.vecQuestList.size() <= (size_t)nIndex)	return CQUT_FAIL_QUEST_NOT_TODAY;

	return gvar.Game.vecQuestList[nIndex].nUnableType;
}

void XGlueGame::CheckBGM()
{
//	global.sound->m_BGMMgr.CheckEnvBGM();			<<<rsound3
}

int XGlueGame::GetTalentModSTR( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_PlayerModifier.nSTR.nMod;
}

int XGlueGame::GetTalentModDEX( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_PlayerModifier.nDEX.nMod;
}

int XGlueGame::GetTalentModCON( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_PlayerModifier.nCON.nMod;
}

int XGlueGame::GetTalentModINT( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_PlayerModifier.nINT.nMod;
}

int XGlueGame::GetTalentModCHA( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_PlayerModifier.nCHA.nMod;
}

int XGlueGame::GetTalentModHPMax( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_ActorModifier.nHPMax.nMod;
}

int XGlueGame::GetTalentModENMax( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_ActorModifier.nENMax.nMod;
}

int XGlueGame::GetTalentModSTAMax( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return 0;

	return pTalentInfo->m_ActorModifier.nSTAMax.nMod;
}

int XGlueGame::GetSTR()
{
	CSPlayerModifier PlayerModifier;
	PlayerModifier += gvar.MyInfo.PlayerModifier;
	PlayerModifier -= gvar.MyInfo.PlayerModifier_ExceptTalent;
	return PlayerModifier.nSTR.Calc(gvar.MyInfo.ChrInfo.nSTR);
}

int XGlueGame::GetDEX()
{
	CSPlayerModifier PlayerModifier;
	PlayerModifier += gvar.MyInfo.PlayerModifier;
	PlayerModifier -= gvar.MyInfo.PlayerModifier_ExceptTalent;
	return PlayerModifier.nDEX.Calc(gvar.MyInfo.ChrInfo.nDEX);
}

int XGlueGame::GetINT()
{
	CSPlayerModifier PlayerModifier;
	PlayerModifier += gvar.MyInfo.PlayerModifier;
	PlayerModifier -= gvar.MyInfo.PlayerModifier_ExceptTalent;
	return PlayerModifier.nINT.Calc(gvar.MyInfo.ChrInfo.nINT);
}

int XGlueGame::GetCON()
{
	CSPlayerModifier PlayerModifier;
	PlayerModifier += gvar.MyInfo.PlayerModifier;
	PlayerModifier -= gvar.MyInfo.PlayerModifier_ExceptTalent;
	return PlayerModifier.nCON.Calc(gvar.MyInfo.ChrInfo.nCON);
}

int XGlueGame::GetCHA()
{
	CSPlayerModifier PlayerModifier;
	PlayerModifier += gvar.MyInfo.PlayerModifier;
	PlayerModifier -= gvar.MyInfo.PlayerModifier_ExceptTalent;
	return PlayerModifier.nCHA.Calc(gvar.MyInfo.ChrInfo.nCHA);
}

string XGlueGame::GetConditionText( int nID )
{
	XConditionsInfo* pConditionsInfo = info.cond->Get(nID);
	if (pConditionsInfo == NULL) return "";

	wstring strcondition;

	XConditionStringMaker ConditionStringMaker;
	ConditionStringMaker.Init();
	ConditionStringMaker.Check(pConditionsInfo->m_pConditionComposite, strcondition);

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strcondition.c_str());

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetChallengerQuestConditionText(int nID)
{
	XQuestInfo* pQuest = info.quest->Get( nID);
	if ( pQuest == NULL)	return "";

	gvar.Temp.szTemp = GetConditionText(pQuest->nRequiredConditionIDForChallengerQuest);
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetName()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.MyInfo.ChrInfo.szName);

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetCharacterAccountName(int i)
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.Char.m_CharInfos[ i].szName);

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetGuildName() const
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.MyInfo.Guild.m_strName.c_str());

	return gvar.Temp.szTemp.c_str();
}

void XGlueGame::OutputGuideMsg( int nGuideMsg )
{
	if (global.ui)
	{
		global.ui->OnGuideMsg( XGuideMsgEnum(nGuideMsg));
	}
}

void XGlueGame::ReleaseGuideMsg( int nGuideMsg )
{
	if (global.ui)
	{
		global.ui->ReleaseGuideMsg( XGuideMsgEnum(nGuideMsg));
	}
}

const char* XGlueGame::GetDisplayKeyString( const char* szVKey )
{
	string strVKey = szVKey;

	gvar.Temp.szTemp = _GetDisplayKeyString(strVKey);

	return gvar.Temp.szTemp.c_str();

}

// OpenEnchantItem
bool XGlueGame::OpenEnchantItem( int nStoneIndex)
{
	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pModuleMyControl == NULL)	return false;
	return pModuleMyControl->DoActionEnchantUseItem( nStoneIndex);
}


// BeginEnchantItem
bool XGlueGame::BeginEnchantItem( int nItemType, int nItemIndex)
{
	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();
	if ( pMyPlayer == NULL)		return false;

	XItem* pItem = NULL;
	if ( nItemType == 1)		pItem = gvar.MyInfo.Inventory.GetItem( nItemIndex);
	else if ( nItemType == 2)	pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nItemIndex);
	else						return false;

	if ( pItem == NULL)			return false;

	ENCHANT_DATA enchant_data( nItemIndex, -1, ( nItemType == 2) ? true : false);
	XEvent evt;
	evt.m_nID = XEVTL_ENCHANT_ITEM_SELECT;
	evt.m_pData = &enchant_data;
	pMyPlayer->Event( evt);
	return true;
}


// CancelEnchantItem
void XGlueGame::CancelEnchantItem()
{
	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();
	if ( pMyPlayer == NULL)		return;

	XEvent evt;
	evt.m_nID = XEVTL_ENCHANT_CANCEL;
	pMyPlayer->Event( evt);
}


// GetEnchantSuccessPercent
float XGlueGame::GetEnchantSuccessPercent( int nStoneID)
{
	XItemData* pItemData = info.item->GetItemData( nStoneID);
	if ( pItemData == NULL  ||  pItemData->m_EnchantStone.nLinkedBuffID == -1)	return 0.0f;
	return pItemData->m_EnchantStone.fSuccessRate;
}


// DoEnchantingItem
void XGlueGame::DoEnchantingItem( int nItemType, int nItemIndex, int nAgentIndex)
{
 	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();
 	if ( pMyPlayer == NULL)		return;
 
	ENCHANT_DATA enchant_data( nItemIndex, nAgentIndex, ( nItemType == 2) ? true : false);
	XEvent evt;
	evt.m_nID = XEVTL_ENCHANT_CHECK_REQ;
	evt.m_pData = &enchant_data;
	pMyPlayer->Event( evt);
}

const char* XGlueGame::GetBuffName( int nID )
{
	XBuffInfo* pBuffInfo = info.buff->Get( nID);
	if ( pBuffInfo == NULL)		return "";

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pBuffInfo->GetName());

	return gvar.Temp.szTemp.c_str();
}

bool XGlueGame::OpenDyeItem( int nDyeItemIndex)
{
	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pModuleMyControl == NULL  ||  pModuleMyControl->GetCurrentActionStateID() == ACTION_STATE_DYE)
		return false;

	DYE_DATA dye_data;
	dye_data.m_nDyeItemIndex = nDyeItemIndex;

	if ( pModuleMyControl->DoActionDye(&dye_data) == false)
	{
		return false;
	}

	return true;
}


bool XGlueGame::BeginDyeItem( int nItemType, int nItemIndex, int nDyeItemIndex)
{
	DYE_DATA dye_data(nItemType, nItemIndex, nDyeItemIndex);

	XEvent msg;
	msg.m_nID = XEVTL_DYE_ITEM_SELECT;
	msg.m_pData = &dye_data;
	gg.omgr->PostEvent( gvar.MyInfo.MyUID, msg);

	return true;
}

void XGlueGame::CancelDyeItem()
{
	XEvent msg;
	msg.m_nID = XEVTL_DYE_CANCEL;
	gg.omgr->PostEvent( gvar.MyInfo.MyUID, msg);
}

void XGlueGame::DoDyeingItem( int nItemType, int nItemIndex, int nDyeItemIndex)
{
	DYE_DATA dye_data(nItemType, nItemIndex, nDyeItemIndex);

	XEvent msg;
	msg.m_nID = XEVTL_DYE_START_ITEM;
	msg.m_pData = &dye_data;
	gg.omgr->PostEvent( gvar.MyInfo.MyUID, msg);
}

bool XGlueGame::IsDirectLoginToLoginFrame()
{
	if( false == gvar.Network.bDirectLogin )
		return false;

	if( LOGIN_FRAME_ID != global.gfmgr->GetPrevFrameID() )
		return false;

	return true;
}

void XGlueGame::HelpTrigger( int nHelpID )
{
	if( NULL == global.ui )
		return ;

	global.ui->GetHelpMgr()->OnHelpTrigger( nHelpID );
}

void XGlueGame::HelpTriggerString( const char* szEventString )
{
	if( NULL == global.ui )
		return ;

	wstring strHelpEvent = MLocale::ConvAnsiToUTF16(szEventString);
	global.ui->GetHelpMgr()->OnHelpTriggerString( strHelpEvent );
}

const char* XGlueGame::GetHelpImg( int nHelpID, int nImgIndex )
{
	if( NULL == global.ui )
		return NULL;

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( global.ui->GetHelpMgr()->GetHelpImg( nHelpID, nImgIndex ) );
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetHelpMsg( int nHelpID )
{
	if( NULL == global.ui )
		return NULL;

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( global.ui->GetHelpMgr()->GetHelpMsg( nHelpID ) );
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetEventTrigger( int nHelpID )
{
	if( NULL == global.ui )
		return NULL;

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi( global.ui->GetHelpMgr()->GetEventTrigger( nHelpID ) );
	return gvar.Temp.szTemp.c_str();
}

float XGlueGame::GetHelpTimer( int nHelpID ) 
{
	if( NULL == global.ui )
		return 0.0f;

	return global.ui->GetHelpMgr()->GetHelpTimer( nHelpID );
}

void XGlueGame::FinishedHelp( int nHelpID )
{
	if( NULL == global.ui )
		return ;

	global.ui->GetHelpMgr()->FinishedHelp( nHelpID );
}

const char* XGlueGame::GetCurrentCrossHairName()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(global.ui->GetCureentCrossHairName());

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetCurrentCrossHairPickName()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(global.ui->GetCureentCrossHairPickName());

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::GetCurrentCrossHairAwaysName()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(global.ui->GetCurrentCrossHairAwaysName());

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGame::Format(const char* format, const char* param0, const char* param1, const char* param2, const char* param3, const char* param4, const char* param5, const char* param6)
{
	wstring strFormat;
	XStringResMgr::GetInstance()->TransKeyStr(strFormat, MLocale::ConvAnsiToTCHAR(format).c_str());	
	

	string strParams;
	string strSeparator = "|";
	
	if (param0 != NULL)
	{
		strParams += param0;

		if (param1 != NULL)	strParams += strSeparator + param1;
		if (param2 != NULL)	strParams += strSeparator + param2;
		if (param3 != NULL)	strParams += strSeparator + param3;
		if (param4 != NULL)	strParams += strSeparator + param4;
		if (param5 != NULL)	strParams += strSeparator + param5;
		if (param6 != NULL)	strParams += strSeparator + param6;
	}

	wstring strResult = CSFormatString::Format(strFormat, FSParam(MLocale::ConvAnsiToTCHAR(strParams.c_str())));
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strResult);
	return gvar.Temp.szTemp.c_str();
}

void XGlueGame::LockCameraToNPC( int nNPCID, const char* szBoneName, float fTimeSec )
{
	if (!gg.omgr) return;

	XNonPlayer* pTarNPC = gg.omgr->FindNPC_ID(nNPCID);
	if (!pTarNPC) return;

	XMyPlayer* pMyPlayer = XGetMyPlayer();
	if (!pMyPlayer) return;

	wstring strBoneName = MLocale::ConvAnsiToUTF16(szBoneName);

	// 카메라 락 발생
	XModuleMyControl* pModuleMyControl = pMyPlayer->GetModuleMyControl();
	pModuleMyControl->DoCameraLock((MUID)pTarNPC->GetUID(), strBoneName, fTimeSec);

	//gvar.Debug.uidCameraLock = GetOwner()->GetUID();
	//gvar.Debug.strCameraLockBoneName = pEvent->m_strParam1;

}

bool XGlueGame::IsCurrentTutorialField()
{
	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if ( pFieldInfo)	return pFieldInfo->m_bTutorial;
	return false;
}

void XGlueGame::Narration( const char* szMsg )
{
	wstring strMsg =  MLocale::ConvAnsiToUTF16( szMsg);
	wstring strMessage = XStringResMgr::GetInstance()->TransKeyStrWithParam(strMsg);

	MUID uidOwner = MUID::ZERO;
	mmcode::MControlCodeTransResult result = XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(strMessage.c_str(), XGetMyUID());
	result.Proc(&uidOwner);

	wstring strText = MLocale::ConvAnsiToUTF16(result.m_strOutput.c_str());

	if (global.ui)
		global.ui->OnPresentationUpper(strText);
}

int XGlueGame::GetQuestObjectiveIndex( int nQuestID, int nObjectiveID )
{
	XQuestInfo* pQuest = info.quest->Get( nQuestID);
	if ( pQuest == NULL)	return 0;

	return pQuest->GetObjectiveIndex(nObjectiveID);

}

