#include "stdafx.h"
#include "XCmdHandlerGroup.h"
#include "XCmdHandler_Net.h"
#include "XCmdHandler_Global.h"
#include "XCmdHandler_Quest.h"
#include "XCmdHandler_Action.h"
#include "XCmdHandler_Talent.h"
#include "XCmdHandler_Field.h"
#include "XCmdHandler_Trade.h"
#include "XCmdHandler_Training.h"
#include "XCmdHandler_NPCShop.h"
#include "XCmdHandler_Party.h"
#include "XCmdHandler_Loot.h"
#include "XCmdHandler_Interaction.h"
#include "XCmdHandler_Map.h"
#include "XCmdHandler_GM.h"
#include "XCmdHandler_Fatigue.h"
#include "XCmdHandler_Craft.h"
#include "XCmdHandler_Inn.h"
#include "XCmdHandler_Guild.h"
#include "XCmdHandler_Duel.h"
#include "XCmdHandler_Palette.h"
#include "XCmdHandler_Mail.h"
#include "XCmdHandler_Soul.h"
#include "XCmdHandler_BattleArena.h"
#include "XCmdHandler_LoginServer.h"
#include "XCmdHandler_GameStart.h"
#include "XCmdHandler_Faction.h"
#include "XCmdHandler_CutScene.h"
#include "XCmdHandler_Char.h"
#include "XCmdHandler_NPCAction.h"
#include "XCmdHandler_ChatChannel.h"
#include "XCmdHandler_Msg.h"
#include "XCmdHandler_Local.h"
#include "XCmdHandler_Item.h"
#include "XCmdHandler_Enchant.h"
#include "XCmdHandler_QuestPVP.h"
#include "XCmdHandler_MoveServer.h"
#include "XCmdHandler_Storage.h"

XCmdHandlerGroup::XCmdHandlerGroup( minet::MCommandCommunicator* pCC )
{
	m_pLoginServerCmdHandler = new XCmdHandler_LoginServer(pCC);
	m_pGameStartCmdHandler = new XCmdHandler_GameStart(pCC);

	m_pCmdHandler = new XCmdHandler_Net(pCC);
	m_pGlobalCmdHandler = new XCmdHandler_Global(pCC);
	m_pMsgCmdHandler = new XCmdHandler_Msg(pCC);
	m_pLocalCmdHandler = new XCmdHandler_Local(pCC);
	m_pQuestCmdHandler = new XCmdHandler_Quest(pCC);
	m_pActionCmdHandler = new XCmdHandler_Action(pCC);
	m_pItemCmdHandler = new XCmdHandler_Item(pCC);
	m_pTalentCmdHandler = new XCmdHandler_Talent(pCC);
	m_pFieldCmdHandler = new XCmdHandler_Field(pCC);
	m_pTradeCmdHandler = new XCmdHandler_Trade(pCC);
	m_pTrainingCmdHandler = new XCmdHandler_Training(pCC);
	m_pNPCShopCmdHandler = new XCmdHandler_NPCShop(pCC);
	m_pPartyCmdHandler = new XCmdHandler_Party(pCC);
	m_pLootCmdHandler = new XCmdHandler_Loot(pCC);	
	m_pInteractionCmdHandler = new XCmdHandler_Interaction(pCC);
	m_pMapHandler = new XCmdHandler_Map(pCC);
	m_pGMCmdHandler = new XCmdHandler_GM(pCC);
	m_pFatigueCmdHandler = new XCmdHandler_Fatigue(pCC);
	m_pCraftCmdHandler = new XCmdHandler_Craft(pCC);
	m_pInnCmdHandler = new XCmdHandler_Inn(pCC);
	m_pGuildCmdHandler = new XCmdHandler_Guild(pCC);
	m_pDuelCmdHandler = new XCmdHandler_Duel(pCC);
	m_pBattleArenaCmdHandler = new XCmdHandler_BattleArena(pCC);
	m_pPaletteCmdHandler = new XCmdHandler_Palette(pCC);
	m_pMailCmdHandler = new XCmdHandler_Mail(pCC);
	m_pSoulCmdHandler = new XCmdHandler_Soul(pCC);
	m_pFactionCmdHandler = new XCmdHandler_Faction(pCC);
	m_pCutSceneCmdHandler = new XCmdHandler_CutScene(pCC);
	m_pCharCmdHandler = new XCmdHandler_Char(pCC);
	m_pNPCActionCmdHandler = new XCmdHandler_NPCAction(pCC);
	m_pChatChannelCmdHandler = new XCmdHandler_ChatChannel(pCC);
	m_pEnchantCmdHandler = new XCmdHandler_Enchant(pCC);
	m_pQuestPVPCmdHandler = new XCmdHandler_QuestPVP(pCC);
	m_pMoveServerCmdHandler = new XCmdHandler_MoveServer(pCC);
	m_pStorageCmdHandler = new XCmdHandler_Storage(pCC);

}

XCmdHandlerGroup::~XCmdHandlerGroup()
{
	SAFE_DELETE(m_pLoginServerCmdHandler);
	SAFE_DELETE(m_pGameStartCmdHandler);

	SAFE_DELETE(m_pNPCShopCmdHandler);
	SAFE_DELETE(m_pTrainingCmdHandler);
	SAFE_DELETE(m_pTradeCmdHandler);
	SAFE_DELETE(m_pTalentCmdHandler);
	SAFE_DELETE(m_pFieldCmdHandler);
	SAFE_DELETE(m_pItemCmdHandler);
	SAFE_DELETE(m_pActionCmdHandler);
	SAFE_DELETE(m_pQuestCmdHandler);
	SAFE_DELETE(m_pLocalCmdHandler);
	SAFE_DELETE(m_pMsgCmdHandler);
	SAFE_DELETE(m_pGlobalCmdHandler);
	SAFE_DELETE(m_pCmdHandler);
	SAFE_DELETE(m_pPartyCmdHandler);
	SAFE_DELETE(m_pLootCmdHandler);
	SAFE_DELETE(m_pInteractionCmdHandler);
	SAFE_DELETE(m_pMapHandler);
	SAFE_DELETE(m_pGMCmdHandler);
	SAFE_DELETE(m_pFatigueCmdHandler);
	SAFE_DELETE(m_pCraftCmdHandler);
	SAFE_DELETE(m_pInnCmdHandler);
	SAFE_DELETE(m_pGuildCmdHandler);
	SAFE_DELETE(m_pDuelCmdHandler);
	SAFE_DELETE(m_pBattleArenaCmdHandler);
	SAFE_DELETE(m_pPaletteCmdHandler);
	SAFE_DELETE(m_pMailCmdHandler);
	SAFE_DELETE(m_pSoulCmdHandler);
	SAFE_DELETE(m_pFactionCmdHandler);
	SAFE_DELETE(m_pCutSceneCmdHandler);
	SAFE_DELETE(m_pCharCmdHandler);	
	SAFE_DELETE(m_pNPCActionCmdHandler);
	SAFE_DELETE(m_pChatChannelCmdHandler);
	SAFE_DELETE(m_pEnchantCmdHandler);
	SAFE_DELETE(m_pQuestPVPCmdHandler);
	SAFE_DELETE(m_pMoveServerCmdHandler);
	SAFE_DELETE(m_pStorageCmdHandler)
}