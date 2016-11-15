#pragma once

#include "MCommandCommunicator.h"
#include "MCommandHandler.h"

class GCmdHandler_Login;
class GCmdHandler_Comm;
class GCmdHandler_Game;
class GCmdHandler_GM;
class GCmdHandler_NPCShop;
class GCmdHandler_Trade;
class GCmdHandler_Interaction;
class GCmdHandler_Quest;
class GCmdHandler_Loot;
class GCmdHandler_Party;
class GCmdHandler_Action;
class GCmdHandler_MSG;
class GCmdHandler_Item;
class GCmdHandler_Common;
class GCmdHandler_Map;
class GCmdHandler_Training;
class GCmdHandler_Craft;
class GCmdHandler_Inn;
class GCmdHandler_Guild;
class GCmdHandler_Duel;
class GCmdHandler_Palette;
class GCmdHandler_BattleArena;
class GCmdHandler_Cutscene;
class GCmdHandler_Char;
class GCmdHandler_Mail;
class GCmdHandler_QuestPVP;
class GCmdHandler_Storage;
class GCmdHandler_TradeMarket;

class GCmdHandler_Master_Common;
class GCmdHandler_Master_Login;
class GCmdHandler_Master_Field;
class GCmdHandler_Master_Gate;
class GCmdHandler_Master_Party;
class GCmdHandler_Master_Char;
class GCmdHandler_Master_Loot;
class GCmdHandler_Master_Guild;
class GCmdHandler_Master_MoveServer;
class GCmdHandler_App_Common;
class GCmdHandler_App_Msg;
class GCmdHandler_App_Mail;

class GCmdHandler_FakeLoginServer;


class GCmdHandlerGroup : public MTestMemPool<GCmdHandlerGroup>
{
private:
	GCmdHandler_Comm*			m_pCommCmdHandler;
	GCmdHandler_Login*			m_pLoginCmdHandler;
	GCmdHandler_Game*			m_pGameCmdHandler;
	GCmdHandler_GM*				m_pGMCmdHandler;
	GCmdHandler_NPCShop*		m_pNPCShopCmdHandler;
	GCmdHandler_Trade*			m_pTradeCmdHandler;
	GCmdHandler_Interaction*	m_pInteractionCmdHandler;
	GCmdHandler_Quest*			m_pQuestCmdHandler;
	GCmdHandler_Loot*			m_pLooCmdHandler;
	GCmdHandler_Party*			m_pPartyCmdHandler;
	GCmdHandler_Action*			m_pActionCmdHandler;
	GCmdHandler_MSG*			m_pMSGCmdHandler;
	GCmdHandler_Item*			m_pItemCmdHandler;
	GCmdHandler_Map*			m_pMapCmdHandler;
	GCmdHandler_Training*		m_pTrainingCmdHandler;
	GCmdHandler_Craft*			m_pCraftCmdHandler;
	GCmdHandler_Inn*			m_pInnCmdHandler;
	GCmdHandler_Guild*			m_pGuildCmdHandler;
	GCmdHandler_Duel*			m_pDuelCmdHandler;
	GCmdHandler_Palette*		m_pPalette;
	GCmdHandler_BattleArena*	m_pBattleArena;
	GCmdHandler_FakeLoginServer*m_pFakeLoginServerHandler;
	GCmdHandler_Cutscene*		m_pCutscene;
	GCmdHandler_Char*			m_pChar;
	GCmdHandler_Mail*			m_pMail;
	GCmdHandler_QuestPVP*		m_pQustPVP;
	GCmdHandler_Storage*		m_pStorage;
	GCmdHandler_TradeMarket*	m_pTradeMarket;

public:
	GCmdHandlerGroup(minet::MCommandCommunicator* pCC);
	~GCmdHandlerGroup();
};

//////////////////////////////////////////////////////////////////////////



/// MasterNetClient에서 사용하는 커맨드 핸들러
class GCmdHandlerGroup_MasterNetClient : public MTestMemPool<GCmdHandlerGroup_MasterNetClient>
{
private:
	GCmdHandler_Master_Common*		m_pCommonCmdHandler;
	GCmdHandler_Master_Login*		m_pLoginCmdHandler;
	GCmdHandler_Master_Field*		m_pFieldCmdHandler;
	GCmdHandler_Master_Gate*		m_pGateCmdHandler;
	GCmdHandler_Master_Party*		m_pPartyCmdHandler;	
	GCmdHandler_Master_Char*		m_pCharCmdHandler;
	GCmdHandler_Master_Loot*		m_pLootCmdHandler;
	GCmdHandler_Master_Guild*		m_pGuildCmdHandler;
	GCmdHandler_Master_MoveServer*	m_pMoveServerCmdHandler;

public:
	GCmdHandlerGroup_MasterNetClient(minet::MCommandCommunicator* pCC);
	~GCmdHandlerGroup_MasterNetClient();
};


/// AppNetClient에서 사용하는 커맨드 핸들러
class GCmdHandlerGroup_AppNetClient : public MTestMemPool<GCmdHandlerGroup_AppNetClient>
{
private:
	GCmdHandler_App_Common*		m_pCommonCmdHandler;
	GCmdHandler_App_Msg*		m_pMsgCmdHandler;
	GCmdHandler_App_Mail*		m_pMailCmdHandler;

public:
	GCmdHandlerGroup_AppNetClient(minet::MCommandCommunicator* pCC);
	~GCmdHandlerGroup_AppNetClient();
};
