#include "stdafx.h"
#include "GCmdHandlerGroup.h"

///
#include "GCmdHandler_Game.h"
#include "GCmdHandler_GM.h"
#include "GCmdHandler_NPCShop.h"
#include "GCmdHandler_Trade.h"
#include "GCmdHandler_Interaction.h"
#include "GCmdHandler_Quest.h"
#include "GCmdHandler_Loot.h"
#include "GCmdHandler_Party.h"
#include "GCmdHandler_Action.h"
#include "GCmdHandler_MSG.h"
#include "GCmdHandler_Item.h"
#include "GCmdHandler_Map.h"
#include "GCmdHandler_Training.h"
#include "GCmdHandler_Craft.h"
#include "GCmdHandler_Inn.h"
#include "GCmdHandler_Guild.h"
#include "GCmdHandler_Duel.h"
#include "GCmdHandler_Palette.h"
#include "GCmdHandler_Comm.h"
#include "GCmdHandler_Login.h"
#include "GCmdHandler_BattleArena.h"
#include "GCmdHandler_Cutscene.h"
#include "GCmdHandler_Char.h"
#include "GCmdHandler_Mail.h"
#include "GCmdHandler_QuestPVP.h"
#include "GCmdHandler_Storage.h"

///
#include "GCmdHandler_Master_Common.h"
#include "GCmdHandler_Master_Login.h"
#include "GCmdHandler_Master_Field.h"
#include "GCmdHandler_Master_Gate.h"
#include "GCmdHandler_Master_Party.h"
#include "GCmdHandler_Master_Char.h"
#include "GCmdHandler_Master_Loot.h"
#include "GCmdHandler_Master_Guild.h"
#include "GCmdHandler_Master_MoveServer.h"

#include "GCmdHandler_App_Common.h"
#include "GCmdHandler_App_Msg.h"
#include "GCmdHandler_App_Mail.h"

#include "GCmdHandler_FakeLoginServer.h"

GCmdHandlerGroup::GCmdHandlerGroup( MCommandCommunicator* pCC )
{
	m_pCommCmdHandler = new GCmdHandler_Comm(pCC);
	m_pLoginCmdHandler = new GCmdHandler_Login(pCC);
	m_pGameCmdHandler = new GCmdHandler_Game(pCC);
	m_pGMCmdHandler = new GCmdHandler_GM(pCC);
	m_pNPCShopCmdHandler = new GCmdHandler_NPCShop(pCC);	
	m_pTradeCmdHandler = new GCmdHandler_Trade(pCC);
	m_pInteractionCmdHandler = new GCmdHandler_Interaction(pCC);
	m_pQuestCmdHandler = new GCmdHandler_Quest(pCC);
	m_pLooCmdHandler = new GCmdHandler_Loot(pCC);
	m_pPartyCmdHandler = new GCmdHandler_Party(pCC);
	m_pActionCmdHandler = new GCmdHandler_Action(pCC);
	m_pMSGCmdHandler = new GCmdHandler_MSG(pCC);
	m_pItemCmdHandler = new GCmdHandler_Item(pCC);
	m_pMapCmdHandler = new GCmdHandler_Map(pCC);
	m_pTrainingCmdHandler = new GCmdHandler_Training(pCC);
	m_pCraftCmdHandler = new GCmdHandler_Craft(pCC);
	m_pInnCmdHandler = new GCmdHandler_Inn(pCC);
	m_pGuildCmdHandler = new GCmdHandler_Guild(pCC);
	m_pDuelCmdHandler = new GCmdHandler_Duel(pCC);
	m_pPalette = new GCmdHandler_Palette(pCC);
	m_pBattleArena = new GCmdHandler_BattleArena(pCC);
	m_pFakeLoginServerHandler = new GCmdHandler_FakeLoginServer(pCC);
	m_pCutscene = new GCmdHandler_Cutscene(pCC);
	m_pChar = new GCmdHandler_Char(pCC);
	m_pMail = new GCmdHandler_Mail(pCC);
	m_pQustPVP = new GCmdHandler_QuestPVP(pCC);
	m_pStorage = new GCmdHandler_Storage(pCC);
}

GCmdHandlerGroup::~GCmdHandlerGroup()
{
	SAFE_DELETE(m_pMapCmdHandler);
	SAFE_DELETE(m_pItemCmdHandler);
	SAFE_DELETE(m_pMSGCmdHandler);
	SAFE_DELETE(m_pActionCmdHandler);
	SAFE_DELETE(m_pPartyCmdHandler);
	SAFE_DELETE(m_pLooCmdHandler);
	SAFE_DELETE(m_pQuestCmdHandler);
	SAFE_DELETE(m_pInteractionCmdHandler);
	SAFE_DELETE(m_pTradeCmdHandler);
	SAFE_DELETE(m_pNPCShopCmdHandler);
	SAFE_DELETE(m_pGMCmdHandler);
	SAFE_DELETE(m_pGameCmdHandler);
	SAFE_DELETE(m_pCommCmdHandler);
	SAFE_DELETE(m_pLoginCmdHandler);
	SAFE_DELETE(m_pTrainingCmdHandler);
	SAFE_DELETE(m_pCraftCmdHandler);
	SAFE_DELETE(m_pInnCmdHandler);
	SAFE_DELETE(m_pGuildCmdHandler);
	SAFE_DELETE(m_pDuelCmdHandler);
	SAFE_DELETE(m_pPalette);
	SAFE_DELETE(m_pBattleArena);
	SAFE_DELETE(m_pFakeLoginServerHandler);
	SAFE_DELETE(m_pCutscene);
	SAFE_DELETE(m_pChar);
	SAFE_DELETE(m_pMail);
	SAFE_DELETE(m_pQustPVP);
	SAFE_DELETE(m_pStorage);
}

//////////////////////////////////////////////////////////////////////////

GCmdHandlerGroup_MasterNetClient::GCmdHandlerGroup_MasterNetClient( minet::MCommandCommunicator* pCC )
{
	m_pCommonCmdHandler = new GCmdHandler_Master_Common(pCC);
	m_pLoginCmdHandler = new GCmdHandler_Master_Login(pCC);
	m_pFieldCmdHandler = new GCmdHandler_Master_Field(pCC);
	m_pGateCmdHandler = new GCmdHandler_Master_Gate(pCC);	
	m_pPartyCmdHandler = new GCmdHandler_Master_Party(pCC);	
	m_pCharCmdHandler = new GCmdHandler_Master_Char(pCC);
	m_pLootCmdHandler = new GCmdHandler_Master_Loot(pCC);
	m_pGuildCmdHandler = new GCmdHandler_Master_Guild(pCC);
	m_pMoveServerCmdHandler = new GCmdHandler_Master_MoveServer(pCC);
}

GCmdHandlerGroup_MasterNetClient::~GCmdHandlerGroup_MasterNetClient()
{
	SAFE_DELETE(m_pCommonCmdHandler);	
	SAFE_DELETE(m_pLoginCmdHandler);
	SAFE_DELETE(m_pFieldCmdHandler);
	SAFE_DELETE(m_pGateCmdHandler);
	SAFE_DELETE(m_pPartyCmdHandler);	
	SAFE_DELETE(m_pCharCmdHandler);
	SAFE_DELETE(m_pLootCmdHandler);
	SAFE_DELETE(m_pGuildCmdHandler);
	SAFE_DELETE(m_pMoveServerCmdHandler);
}

//////////////////////////////////////////////////////////////////////////

GCmdHandlerGroup_AppNetClient::GCmdHandlerGroup_AppNetClient( minet::MCommandCommunicator* pCC )
{
	m_pCommonCmdHandler = new GCmdHandler_App_Common(pCC);
	m_pMsgCmdHandler = new GCmdHandler_App_Msg(pCC);
	m_pMailCmdHandler = new GCmdHandler_App_Mail(pCC);
}

GCmdHandlerGroup_AppNetClient::~GCmdHandlerGroup_AppNetClient()
{
	SAFE_DELETE(m_pCommonCmdHandler);
	SAFE_DELETE(m_pMsgCmdHandler);
	SAFE_DELETE(m_pMailCmdHandler);
}
