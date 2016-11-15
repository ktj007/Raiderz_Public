#pragma once

#include "XGlueFunction.h"

/// lua에서 사용하는 내캐릭정보 API(read 전용)
class XGlueMyInfoFunction : public XGlueFunction
{
public:
	virtual void DeclGlueFunc(MWLua* pLua);

	// 캐릭터 정보 ----
	__int64					GetUID()			{ return gvar.MyInfo.MyUID.Value; }
	/// 이름(ID)를 넘긴다.
	const char*				GetID();
	/// First Name에 해당하는 이름을 넘긴다.
	const char*				GetSurName();
	/// 레벨정보를 넘긴다.
	int						GetLevel()			{ return gvar.MyInfo.nLevel; }
	/// 실버(돈)
	int						GetSilver()			{ return gvar.MyInfo.ChrInfo.GetMoney(); }
	/// 경험치 퍼센트를 넘긴다.
	int						GetExpPercent()		{ return gvar.MyInfo.GetExpPercent(); }
	/// 경험치
	int						GetExp()			{ return gvar.MyInfo.ChrInfo.nXP; }
	/// 현재 위치의 맵 이름을 넘긴다.
	const char*				GetFieldName();
	int						GetFieldID();
	int						GetChannelID();
	bool					IsDynamicField();
	/// 플레이어 등급
	int						GetPlayerGrade()	{ return (int)gvar.MyInfo.ChrInfo.nPlayerGrade; }

	// 능력 정보 ----
	/// 완력값을 넘긴다.
	int						GetSTR()	{ return gvar.MyInfo.ChrInfo.nSTR; }
	/// 회피값을 넘긴다.
	int						GetDEX()	{ return gvar.MyInfo.ChrInfo.nDEX; }
	/// 지력값을 넘긴다.
	int						GetINT()	{ return gvar.MyInfo.ChrInfo.nINT; }
	/// Con값을 넘긴다.
	int						GetCON()	{ return gvar.MyInfo.ChrInfo.nCON; }
	/// 카리스마값을 넘긴다.
	int						GetCHA()	{ return gvar.MyInfo.ChrInfo.nCHA; }

	// MOD ----
	/// 완력의 추가값을 넘긴다.
	int						GetModSTR() { return gvar.MyInfo.PlayerModifier.nSTR.nMod; }
	/// 회피의 추가값을 넘긴다.
	int						GetModDEX() { return gvar.MyInfo.PlayerModifier.nDEX.nMod; }
	/// 지력의 추가값을 넘긴다.
	int						GetModINT() { return gvar.MyInfo.PlayerModifier.nINT.nMod; }
	// Con의 추가값을 넘긴다.
	int						GetModCON() { return gvar.MyInfo.PlayerModifier.nCON.nMod; }
	/// 카리스마의 추가값을 넘긴다.
	int						GetModCHA() { return gvar.MyInfo.PlayerModifier.nCHA.nMod; }

	// 저항 정보 ----
	/// 화속성 저항력의 값을 넘긴다.
	int						GetFR()		{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_FR]; }
	/// 수속성 저항력의 값을 넘긴다.
	int						GetCR()		{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_CR]; }
	/// 풍속성 저항력의 값을 넘긴다.
	int						GetLR()		{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_LR]; }
	/// 지속성 저항력의 값을 넘긴다.
	int						GetPR()		{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_PR]; }
	/// Holy 저항력의 값을 넘긴다.
	int						GetHR()		{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_HR]; }
	/// Unholy 저항력의 값을 넘긴다.
	int						GetUR()		{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_UR]; }

	// 저항 정보 Mod값
	int						GetModFR()		{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_FR].nMod; }
	int						GetModCR()		{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_CR].nMod; }
	int						GetModLR()		{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_LR].nMod; }
	int						GetModPR()		{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_PR].nMod; }
	int						GetModHR()		{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_HR].nMod; }
	int						GetModUR()		{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_UR].nMod; }

	// 상태 정보 ----
	/// 생명력의 값을 넘긴다.
	int						GetHP()		{ return gvar.MyInfo.ChrStatus.nHP; }
	/// 정신력의 값을 넘긴다.
	int						GetEN()		{ return gvar.MyInfo.ChrStatus.nEN; }
	/// 기력의 값을 넘긴다.
	int						GetSTA()	{ return gvar.MyInfo.ChrStatus.nSTA; }
	/// 최대 생명력의 값을 넘긴다.
	int						GetMaxHP()	{ return gvar.MyInfo.GetMaxHP(); }
	/// 최대 정신력의 값을 넘긴다.
	int						GetMaxEN()	{ return gvar.MyInfo.GetMaxEN(); }
	/// 최대 기력의 값을 넘긴다.
	int						GetMaxSTA() { return gvar.MyInfo.GetMaxSTA(); }

	/// 포커스 퍼센트를 넘긴다.
	const char*				GetFocusType();
	int						GetFocusPercent()	{ return gvar.MyInfo.nBubblePercent; }

	// 인벤 정보 ----
	/// 아이템 정보를 넘긴다.
	//void					GetItemInfo(unsigned int nItemID_High, unsigned int nItemID_Low);
	/// 아이템의 이름을 넘긴다.
	const char*				GetItemName(unsigned int nItemID_High, unsigned int nItemID_Low);
	const char*				GetInventoryItemName( const char* szuid );
	int						GetInventoryItemCount();
	int						GetInventoryItemIDCount(int nItemID);

	int						GetInventoryUIDItemCount(const char* szuid);
	const char*				GetInventoryItemType( const char* szuid);
	int						GetInventoryItemID( const char* szuid );

	// 장비착용 정보 ----
	/// slot에 장착된 아이템을 넘긴다.
	const char*				GetEquippedItemName( const char* szuid );
	int						GetEquippedItemID( const char* szSlot );
	__int64					GetEquippedItemUID(const char* szSlot);
	const char*				GetEquippedItem(const char* szSlot);
	void					SetEquipItem( const char* szSlot, const char* szuid, bool bPass = false );
	void					SetUnEquipItem(  const char* szSlot);

	// 무기착용 정보
	int				GetWeaponType()	{ return gvar.MyInfo.EquipmentSlot.GetCurrWeaponType();}

	// 탤런트 정보
	int						GetTP()		{ return gvar.MyInfo.Talent.GetTP();}
	const char*				GetLearnedTalentList();
	bool					IsLearnedTalent( int nTalentID);
	int						GetTalentCooldownRemainPercent(int nTalentID);
	int						GetStyleUsedTalent(int index);

	// 아이템 정보
	bool					IsQuestItem(const char* szuid);

	// 걸린 탤런트

	// 무기 사용에 대한 처리

	// 거래
	int						GetMyTradingContainerSize();
	__int64					GetMyTradingContainerItemUID(int index );
	int						GetMyTradingSilver();

	int						GetOtherTradingContainerSize();
	__int64					GetOtherTradingContainerItemUID(int index );
	const char*				GetOtherTradingContainerItemName( const char* szuid);
	int						GetOtherTradingSilver();

	//Buff
	//const char*				GetBuffList();

	// EnemyInfo
	MWLua::table			GetEnemyInfoTable();

	//파티정보
	//const char*				GetPartyMemberBuffIDList(const char* szuid);

	//Quest
	const char*				GetQuestIDList();
	int						GetQuestProgress(int nQuestID, int nObjID );
	unsigned int			GetQuestRemainTime( int nQuestID );
	bool					IsQuestFail( int nQuestID );
	bool					IsQuestTypeTimeAttack( int nQuestID );

	float					GetFieldMyPosX();
	float					GetFieldMyPosY();

	int						GetInvenEquipItemDurability( const char* szuid);
	int						GetInvenEquipItemID( const char* szuid);
	int						GetRepairableItemCount();
	int						GetTotalRepairPrice();

	//길드
	const char*				GetGuildMemberList();
	int						GetGuildMemberGrade( const char* szName );
	const char*				GetGuildMemberFieldName( const char* szName );
	int						GetGuildMemberChannelID( const char* szName );
	bool					IsGuildMemberOnline( const char* szName );
	const char*				GetGuildName();
	bool					IsGuildMaster();

	//팔레트
	void					AddPaletteTalent( int index, const char* szID );
	void					AddPaletteItem(int index, const char* szuid);
	//const char*				GetPaletteList();
	void					UsePaletteIndex(int index);
	void					TakeAwayPalette(int nSlot);	//팔레트에서 제거

	void					ChangePalette(int nNum1, int nSlot1, int nSlot2);
	void					ChangePaletteList(int nIndex);
	int						GetPaletteListIndex();			//<<< 현재 팔레트리스트의 인덱스.
	void					WeaponBinding(int index);
	bool					PrimaryBinding();
	bool					SecondaryBinding();

	//faction

	MWLua::table			GetFactionInfoTable();
	// 기타
};
