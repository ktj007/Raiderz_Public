#pragma once

#include "CSItemData.h" // for ITEMSLOT_NONE
#include "CSChrInfo.h"
#include "CSWorldInfo.h"
#include "CSBasicAttributeCalculator.h"
#include "XMyInventory.h"
#include "XEquipmentSlot.h"
#include "XMyTalent.h"
#include "XTradeContents.h"
#include "XParty.h"
#include "XPaletteContainer.h"
#include "XPlayerInfo.h"
#include "XPlayerQuest.h"
#include "XRepairItemContainer.h"
#include "XGuild.h"
#include "XBattleArenaInfo.h"
#include "XSystem.h"
#include "XPlayerFaction.h"
#include "XMyLootingItemContainer.h"
#include "XCraftableRecipeInfo.h"
#include "XMyRecipeInfo.h"
#include "XMailbox.h"
#include "XMyStorage.h"
#include "XMyGuildStorage.h"
#include "XBuffContainer.h"

/// 전역적으로 사용하는 내 정보
class GlobalVar_MyInfo
{
private:
	UIID			m_nUIID;			///< 내 Indexed ID
	float			m_fExpPercent;	///< 경험치 퍼센트율

public:
	MUID			MyUID;			///< 내 UID
	wstring			strLoginID;		///< 내 계정 ID
	PLAYER_INFO		ChrInfo;		///< 내 캐릭터 정보
	CHR_STATUS		ChrStatus;		///< 내 캐릭터 상태
	CSPlayerModifier PlayerModifier;	///< 내 캐릭터 모디파이어
	
	CSActorModifier	 ActorModifier_ExceptTalent;
	CSPlayerModifier PlayerModifier_ExceptTalent;

	int				nLevel;			///< 레벨

	// 카운트 변수
	int				nRecievedQuest;
	int				nCompletedQuest;
	int				nRecievedRecipe;
	int				nCompletedRecipe;
	int				nKilledEnemy;
	int				nKilledEpicEnemy;

	// 퀘스트
	XPlayerQuests				Quests;				///< 퀘스트
	XMyRecipeInfo				RecipeInfo;			///< 레시피

	// Party
	XParty			Party;

	// 아이템 관련
	XMyEquipmentSlot	EquipmentSlot;		///< 아이템 장비 슬롯
	XMyInventory		Inventory;			///< 아이템 인벤토리
	XMyStorage			Storage;			///< 보관함
	XMyGuildStorage		GuildStorage;		///< 길드 보관함
	XTradeContents		TradeContents;		///< 거래정보

	XMyLootingItemContainer		LootingItem;			///< 루팅아이템리스트
	XMyLootingItemContainer		AssignLootingItem;		///< 배분 대기용 루팅아이템리스트

	// 스킬 관련
	XMyTalent				Talent;
	XMyBuffContainer		BuffList;			///< 버프효과리스트

	XCraftableRecipeInfo	CraftableRecipeInfo;	///< 제작가능한 레시피 리스트

	//팔레트
	XPaletteManager		PaletteList;

	// 편지함
	XMailbox				Mailbox;

	XPlayerFactions			FactionInfo;

	bool					bGod;			///< 갓모드인지 여부

	FATIGUE_TYPE	nFatigueType;

	XGuild			Guild;	///< 길드

	XBattleArenaInfo	BattleArenaInfo;	///< 투기장

	//map<TALENT_FOCUS_TYPE, int> BubblePercent;	///< 버블
	TALENT_FOCUS_TYPE	nFocusType;
	int					nBubblePercent;

	unsigned int		nDieTime;					///< 죽은시간
	float				fAutoRebirthRemainCoolTime; ///< 부활까지 남은 시간

	int					nSoulbindingFieldID;
	int					nCheckPointFieldID;

	GlobalVar_MyInfo();
	~GlobalVar_MyInfo();

	void	Clear();

	int		GetHP()	const		{ return ChrStatus.nHP; }
	int		GetEN()	const		{ return ChrStatus.nEN; }
	int		GetSTA() const		{ return ChrStatus.nSTA; }
	int		GetMaxHP() const	{ return max(1, ChrStatus.ActorModifier.nHPMax.Calc(CSBasicAttributeCalculator::CalcMaxHP(nLevel, GetCON()))); }
	int		GetMaxEN() const	{ return max(1, ChrStatus.ActorModifier.nENMax.Calc(CSBasicAttributeCalculator::CalcMaxEN(nLevel, GetINT()))); }
	int		GetMaxSTA() const	{ return max(1, ChrStatus.ActorModifier.nSTAMax.Calc(CSBasicAttributeCalculator::CalcMaxSP(nLevel, GetDEX()))); }
	int		GetSTR() const		{ return PlayerModifier.nSTR.Calc(ChrInfo.nSTR); }
	int		GetDEX() const		{ return PlayerModifier.nDEX.Calc(ChrInfo.nDEX); }
	int		GetINT() const		{ return PlayerModifier.nINT.Calc(ChrInfo.nINT); }
	int		GetCHA() const		{ return PlayerModifier.nCHA.Calc(ChrInfo.nCHA); }
	int		GetCON() const		{ return PlayerModifier.nCON.Calc(ChrInfo.nCON); }

	int		GetModAP()
	{
		int ap = EquipmentSlot.GetAP();
		return ActorModifier_ExceptTalent.nAP.Calc(ap) - ap;
	}

	float	GetExpPercent() const { return m_fExpPercent; }
	void	SetExpPercent(float fPercent)		{ m_fExpPercent = fPercent; }

	void SetHP(int nHP)
	{
#ifndef _PUBLISH
		if (this->bGod)
		{
			ChrStatus.nHP = ChrInfo.nMaxHP;
			return;
		}
#endif
		ChrStatus.nHP = max(0, min(nHP, GetMaxHP()));
	}
	void SetEN(int nEN)
	{
#ifndef _PUBLISH
		if (this->bGod)
		{
			ChrStatus.nEN = ChrInfo.nMaxEN;
			return;
		}
#endif
		ChrStatus.nEN = max(0, min(nEN, GetMaxEN()));
	}
	void SetSTA(int nSTA)
	{
#ifndef _PUBLISH
		if (this->bGod)
		{
			ChrStatus.nSTA = ChrInfo.nMaxSTA;
			return;
		}
#endif
		ChrStatus.nSTA = max(0, min(nSTA, GetMaxSTA()));

	}

	void SubChrStatus(XTalentInfo* pTalentInfo);

	void Assign(const TD_MYINFO& td_myinfo);
	void ResetByMoveGameServer();

	TD_MYINFO AsTransDataMyInfo();

	void SetFrom_TD_MYINFO(
		const TD_MYINFO& TDMyInfo, 
		const vector<TD_ITEM>& vecTDInventory, 
		const vector<TD_ITEM>& vecTDEquipment,
		const vector<int>& vecTalent, 
		const vector<TD_PLAYERQUEST>& vecTDPlayerQuest,
		const TD_PALETTE& TDPalette, 
		const vector<TD_FACTION>& vecTDFaction, 
		const vector<int>& vecRecipeID);

	bool BuffExist(int nID)
	{
		return BuffList.FindID(nID);
	}

	void SetUIID(UIID nUIID) { m_nUIID = nUIID; }
	UIID GetUIID() { return m_nUIID; }
};
