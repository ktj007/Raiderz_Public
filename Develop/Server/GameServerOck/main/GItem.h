#ifndef _GITEM_H
#define _GITEM_H

#include "MMemPool.h"
#include "GItemData.h"
#include "CSChrInfo.h"
#include "CTransData.h"
#include "GItemPeriod.h"

class GItemManager;
class TestItemManager;
struct TD_ITEM;
struct TD_PLAYER_GAME_DATA_ITEM_INSTANCE;
class GBuffInfo;

TEST_FORWARD_FT(MailItemTaker, Fixture, TestMailItemTaker_Take_Failure_OverMaxStackAmount);
//TEST_FORWARD_FT(Enchant, FEnchant, Enchant_ActiveBuff);
//TEST_FORWARD_FT(Enchant, FEnchant, EnchantBuff);

/// 아이템 인스턴스
class GItem : public MMemPool<GItem>
{
TEST_FRIEND_FT(MailItemTaker, Fixture, TestMailItemTaker_Take_Failure_OverMaxStackAmount);
//	TEST_FRIEND_FT(Enchant, FEnchant, Enchant_ActiveBuff);
//	TEST_FRIEND_FT(Enchant, FEnchant, EnchantBuff);
private:
	friend class TestItemManager;
	friend class GItemManager;
	friend class GItemContainer;
	friend class GGuildSystemForMasterServer;	
	
public:
	~GItem();

	MUID			GetUID() const				{ return m_uidID; }
	int				GetID();

	TD_ITEM								ToTD_ITEM();	
	TD_PLAYER_GAME_DATA_ITEM_INSTANCE	ToTD_PLAYER_GAME_DATA_ITEM_INSTANCE();
	TD_UPDATE_DURABILITY				ToTD_UPDATE_DURABILITY();
	TD_ITEM_INSERT						ToTD_ITEM_INSERT();

	bool			IsEquiped() const			{ return m_bEquiped; }
	void			SetEquiped(bool val)		{ m_bEquiped = val; }

	int				GetLostedDurability();
	void			GetExpiDtForDB(wstring& strExpiDt);

	bool			IsInteractableType();
	bool			IsExpired(const int nCharPlayTimeMin);
	bool			IsFineDuration() const;
	bool			IsPeriod() const;
	
	// 강화 가능 여부
	bool			IsEnchantable(GBuffInfo* pBuffInfo) const;
	// 강화
	int8 Enchant(GBuffInfo* pBuffInfo, int nItemID, ENCHANT_COLOR nEnchantColor);
	// 강화 실패
	int8 EnchantBreak(GBuffInfo* pBuffInfo);
	// 액티브 강화버프 ID 얻기
	int				GetActiveEnchantBuffID() const;
	// 액티브 강화버프가 있는지 여부
	bool			HasActiveEnchantBuff() const;
	// 강화 횟수
	int				GetEnchantCount() const;
	
	bool			operator== (const GItem* pItem);
	bool			operator!= (const GItem* pItem);

	int8			GetEmptyEnchantSlotIdx(bool bIsActiveSlot, ENCHANT_COLOR nColor=ENCHANT_NONE);
	int				GetAmount() const;

private:
	GItem();
	void			Set(const MUID& id, GItemData* dat);

	// 액티브 강화 가능 여부
	bool			IsEnchantableActiveBuff( GBuffInfo* pBuffInfo) const;
	int8 AddEnchantSlot(bool bIsActiveSlot, int nBuffID, ENCHANT_COLOR nEnchantColor=ENCHANT_NONE);


public:
	int64			m_nIUID;					// IUID - DB의 ID
	MUID			m_uidID;					// 아이템 인스턴스의 고유 ID
	GItemData*		m_pItemData;				// 아이템 파라미터들

	// 여기부턴 각 아이템 인스턴스마다 달라질 수 있는 정보들
	bool			m_bEquiped;		
	int				m_nDurability;				// 내구

	int				m_nDyedColor;				// 염색된 색깔

	bool			m_bBind;					// 귀속 되었는지?

	/// 아이템 사용 시간.
	GItemPeriod		m_UsagePeriod;				// 사용 시간. 서버에 접속했을때만 감소된다.
	GItemPeriod		m_ExpirationPeriod;			// 사용 만료 시간. 서버에 접속해 있지 않아도 감소된다. 
	wstring			m_strExpiDt;				// 만료 일시 스트링 형식.
	
	uint8			m_nSoulCount;				// 영혼 흡수 횟수

	int				m_nEnchants[ENCHANT_MAX_COUNT];		// 인챈트 목록 (첫번째는 활성화 인챈트)
	
	SH_ITEM_SLOT_TYPE	m_nSlotType;			// 슬롯 종류
	int16				m_nSlotID;				// 슬롯 위치

private:
	int				m_nAmount;					// 수량 (겹치는 아이템 대비)
};


#endif // _GITEM_H