#pragma once

#include "STransData_M2G.h"
#include "GCommand.h"

class GEntityPlayer;
class GPlayerGameData
{
public:
	GPlayerGameData();
	virtual ~GPlayerGameData();

	MCommand* MakeMoveGameServerCommand(GEntityPlayer* pPlayer, MUID uidGameServer, MUID uidNetClient);	
	
	bool PackFromCommand(MCommand* pCommand);	
	void PackFromEntityPlayer(GEntityPlayer* pPlayer);
	void UnpackToEntityPlayer(GEntityPlayer* pPlayer);

	bool IsPacked() 							{ return m_bPacked; }
	uint32 GetAID() 							{ return m_PlayerInfoData.nPlayerAID; }
	int32	GetCID()							{ return m_CharaInfoData.nCID; }
	wstring GetUserID()							{ return m_CharaInfoData.szName; }

	uint32	GetEnterKey()						{ return m_PlayerInfoData.nEnterKey; }

protected:
	void PackGateInfo(GEntityPlayer* pPlayer);
	void PackDynamicFieldEnterPosInfo(GEntityPlayer* pPlayer);
	void PackMovePlayerInfo(GEntityPlayer* pPlayer);
	void PackCharaInfo(GEntityPlayer* pPlayer);
	void PackItemList(GEntityPlayer* pPlayer);
	void PackTalent(GEntityPlayer* pPlayer);
	void PackPaletteList(GEntityPlayer* pPlayer);
	void PackTalentRemainTime(GEntityPlayer* pPlayer);
	void PackQuestInstance(GEntityPlayer* pPlayer);
	void PackFaction(GEntityPlayer* pPlayer);
	void PackSawnCutscene(GEntityPlayer* pPlayer);
	void PackRecipe(GEntityPlayer* pPlayer);

	void UnpackGateInfo(GEntityPlayer* pPlayer);
	void UnpackDynamicFieldEnterPosInfo(GEntityPlayer* pPlayer);
	void UnpackMovePlayerInfo(GEntityPlayer* pPlayer);
	void UnpackCharaInfo_Pre(GEntityPlayer* pPlayer);
	void UnpackCharaInfo_Post(GEntityPlayer* pPlayer);
	void UnpackItemList(GEntityPlayer* pPlayer);
	void UnpackTalent(GEntityPlayer* pPlayer);
	void UnpackPaletteList(GEntityPlayer* pPlayer);
	void UnpackTalentRemainTime(GEntityPlayer* pPlayer);
	void UnpackQuestInstance(GEntityPlayer* pPlayer);
	void UnpackFaction(GEntityPlayer* pPlayer);
	void UnpackSawnCutscene(GEntityPlayer* pPlayer);
	void UnpackRecipe(GEntityPlayer* pPlayer);

public:
	const TD_GATE_INFO*												GetGateInfo(void) const;
	const TD_DYNAMIC_FIELD_ENTER_POS_INFO*							GetDynamicFieldEnterPosInfo(void) const;
	const TD_MOVE_PLAYER_INFO*										GetMovePlayerInfo(void) const;
	const TD_PLAYER_GAME_DATA_CHARAINFO*							GetCharaInfo(void) const;
	const vector<TD_PLAYER_GAME_DATA_EQUIP>&						GetEquip(void) const;
	const vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>&				GetItemInstance(void) const;
	const vector<int>&												GetTalent(void) const;
	const vector<TD_PLAYER_GAME_DATA_PALETTE>&						GetPalette(void) const;
	const vector<pair<int, float> >&								GetTalentCollTime(void) const;
	const vector<TD_REMAIN_BUFF_TIME>&								GetBuffRemain(void) const;
	const vector<TD_PLAYER_GAME_DATA_QUEST_INSTANCE>&				GetQuestInstance(void) const;
	const vector<TD_PLAYER_GAME_DATA_QUEST_DONE_COUNT>&				GetQuestInstanceDontDoing(void) const;
	const vector<TD_PLAYER_GAME_DATA_FACTION>&						GetFactoin(void) const;
	const vector<int>&												GetSawnCutscene(void) const;
	const vector<int>&												GetRecipe(void) const;
protected:
	bool															m_bPacked;

	TD_GATE_INFO													m_GateInfo;
	TD_DYNAMIC_FIELD_ENTER_POS_INFO									m_DynamicFieldEnterPosInfo;
	TD_MOVE_PLAYER_INFO												m_PlayerInfoData;
	TD_PLAYER_GAME_DATA_CHARAINFO									m_CharaInfoData;
	vector<TD_PLAYER_GAME_DATA_EQUIP>								m_vecEquipData;
	vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>						m_vecItemInstanceData;
	vector<int>														m_vecTalentData;
	vector<TD_PLAYER_GAME_DATA_PALETTE>								m_vecPaletteData;
	vector<pair<int, float> >										m_vecTalentCollTimeSec;
	vector<TD_REMAIN_BUFF_TIME>										m_vecBuffRemaindSec;
	vector<TD_PLAYER_GAME_DATA_QUEST_INSTANCE>						m_vecQuestInstanceData;
	vector<TD_PLAYER_GAME_DATA_QUEST_DONE_COUNT>					m_vecQuestDoneCount;	
	vector<TD_PLAYER_GAME_DATA_FACTION>								m_vecFactionData;
	vector<int>														m_vecSawnCutscene;
	vector<int>														m_vecRecipe;
};
