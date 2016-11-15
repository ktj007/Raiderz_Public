#pragma once

#include "XDialogInfo.h"
#include "XNPCInteractionInfo.h"
#include "XNPCShopInfo.h"
#include "XEnemyInfo.h"
#include "XTargetActorInfo.h"
#include "XSoundLootItem.h"

class XMyPlayer;
class XInteractionInfo;

typedef TD_ESSENTIAL_NPC_ICON	NPC_ICON_INFO;

struct DUEL
{
	int nTeam;
	vector<MUID> vecTeam1UIDs;
	vector<MUID> vecTeam2UIDs;

	void Clear()
	{
		nTeam = -1;
		vecTeam1UIDs.clear();
		vecTeam2UIDs.clear();
	}

	DUEL()
	{
		Clear();
	}
};

/// 게임내에서 사용하는 전역 변수
class GlobalVar_Game
{	
private:
	XInteractionInfo*	m_pInteractionInfo;

public:
	bool		bTradeReq;
	bool		bPartyReq;
	bool		bGuildReq;
	bool		bDuelReq;

	wstring		strLastWhisper;				// 마지막에 귓말 건 사람

	XDialogInfo	DialogInfo;					// 대화정보
	XNPCInteractionInfo	NPCInteractionInfo;	// 
	XNPCShopInfo NPCShopInfo;				// 상점정보

	XEnemyInfo			EnemyInfo;

	struct MY_COMBAT_STANCE_AUTO_RELEASER_DATA
	{
		bool			bReserved;
		unsigned int	nReserveTime;
		MY_COMBAT_STANCE_AUTO_RELEASER_DATA() : bReserved(false), nReserveTime(0) {}
	};
	MY_COMBAT_STANCE_AUTO_RELEASER_DATA		MyCombatStanceAutoReleaserData;

	XMyPlayer*	pMyPlayer;

	GAME_TIME_TYPE		nLocalTime;
	GAME_WEATHER_TYPE	nLocalWeather;

	bool		bGameStart;					// 게임 활성화 시작

	bool			bDisconnectByDuplicatedLogin;	// 중복접속 처리로 인해 끊긴 연결인지 여부
	wstring			strLastErrorMsg;				// 마지막 에러 메세지

	vector<TD_CHALLENGER_QUEST>	vecQuestList;

	XSoundLootItem		SoundLootItem;

	std::map<UIID, NPC_ICON_INFO>		vecNPCIcon;	// 필드내의 관련 NPC Icon 정보

	DUEL			Duel;

	GlobalVar_Game();

	~GlobalVar_Game();

	void Init_ForChangeField();

	void Init();

	XInteractionInfo& GetInteractionInfo() { return *m_pInteractionInfo; }

	bool IsCombatingNow();
};


