#pragma once

#include "MNetClient.h"

class XCmdHandler_Net;
class XCmdHandler_Global;
class XCmdHandler_Msg;
class XCmdHandler_Quest;
class XCmdHandler_Local;
class XCmdHandler_Action;
class XCmdHandler_Item;
class XCmdHandler_Field;
class XCmdHandler_Talent;
class XCmdHandler_Trade;
class XCmdHandler_Training;
class XCmdHandler_NPCShop;
class XCmdHandler_Party;
class XCmdHandler_Loot;
class XCmdHandler_Interaction;
class XCmdHandler_Map;
class XCmdHandler_GM;
class XCmdHandler_Fatigue;
class XCmdHandler_Craft;
class XCmdHandler_Inn;
class XCmdHandler_Guild;
class XCmdHandler_Duel;
class XCmdHandler_BattleArena;
class XCmdHandler_Palette;
class XCmdHandler_Mail;
class XCmdHandler_Soul;
class XCmdHandler_LoginServer;
class XCmdHandler_GameStart;
class XCmdHandler_Faction;
class XCmdHandler_CutScene;
class XCmdHandler_Char;
class XCmdHandler_NPCAction;
class XCmdHandler_ChatChannel;
class XCmdHandler_Enchant;
class XCmdHandler_QuestPVP;
class XCmdHandler_MoveServer;
class XCmdHandler_Storage;

/// 클라이언트의 커맨드 핸들러 모음
class XCmdHandlerGroup
{
private:
	XCmdHandler_LoginServer*	m_pLoginServerCmdHandler;
	XCmdHandler_GameStart*		m_pGameStartCmdHandler;

	XCmdHandler_Net*			m_pCmdHandler;
	XCmdHandler_Global*			m_pGlobalCmdHandler;
	XCmdHandler_Msg*			m_pMsgCmdHandler;
	XCmdHandler_Local*			m_pLocalCmdHandler;
	XCmdHandler_Quest*			m_pQuestCmdHandler;
	XCmdHandler_Action*			m_pActionCmdHandler;
	XCmdHandler_Item*			m_pItemCmdHandler;
	XCmdHandler_Field*			m_pFieldCmdHandler;
	XCmdHandler_Talent*			m_pTalentCmdHandler;
	XCmdHandler_Trade*			m_pTradeCmdHandler;
	XCmdHandler_Training*		m_pTrainingCmdHandler;
	XCmdHandler_NPCShop*		m_pNPCShopCmdHandler;
	XCmdHandler_Party*			m_pPartyCmdHandler;
	XCmdHandler_Loot*			m_pLootCmdHandler;
	XCmdHandler_Interaction*	m_pInteractionCmdHandler;
	XCmdHandler_Map*			m_pMapHandler;
	XCmdHandler_GM*				m_pGMCmdHandler;
	XCmdHandler_Fatigue*		m_pFatigueCmdHandler;
	XCmdHandler_Craft*			m_pCraftCmdHandler;
	XCmdHandler_Inn*			m_pInnCmdHandler;
	XCmdHandler_Guild*			m_pGuildCmdHandler;
	XCmdHandler_Duel*			m_pDuelCmdHandler;
	XCmdHandler_BattleArena*	m_pBattleArenaCmdHandler;
	XCmdHandler_Palette*		m_pPaletteCmdHandler;
	XCmdHandler_Mail*			m_pMailCmdHandler;
	XCmdHandler_Soul*			m_pSoulCmdHandler;
	XCmdHandler_Faction*		m_pFactionCmdHandler;
	XCmdHandler_CutScene*		m_pCutSceneCmdHandler;
	XCmdHandler_Char*			m_pCharCmdHandler;
	XCmdHandler_NPCAction*		m_pNPCActionCmdHandler;
	XCmdHandler_ChatChannel*	m_pChatChannelCmdHandler;
	XCmdHandler_Enchant*		m_pEnchantCmdHandler;
	XCmdHandler_QuestPVP*		m_pQuestPVPCmdHandler;
	XCmdHandler_MoveServer*		m_pMoveServerCmdHandler;
	XCmdHandler_Storage*		m_pStorageCmdHandler;

public:
	XCmdHandlerGroup(minet::MCommandCommunicator* pCC);
	~XCmdHandlerGroup();
};