/************************************************************************/
/* 1. 캐릭터툴에서 씁니다.                                              */
/************************************************************************/

#ifndef _XITEM_H
#define _XITEM_H

#include "XItemData.h"
#include "CSChrInfo.h"
#include "CTransData.h"

class XItem
{
public:
	XItem();
	virtual ~XItem();

	int				m_nSlotID;					// 아이템 슬롯 ID
	XItemData*		m_pItemData;				// 아이템 파라미터들

	// 여기부턴 각 아이템 인스턴스마다 달라질 수 있는 정보들
	int				m_nAmount;					// 수량 (겹치는 아이템 대비)
	int				m_nDurability;				// 내구
	int				m_nDyedColor;				// 염색된 색
	bool			m_bClaimed;					// 귀속 되었는가?
	uint8			m_nSoulCount;				// 소울 흡수 횟수
	bool			m_bNewItem;					// 최근에 습득한 아이템인지 여부
	int				m_nEnchants[ENCHANT_MAX_COUNT];		// 인챈 목록 (첫번째는 활성화 인챈트)

	//SoulHunterZ
	int				m_nEXP;
	int				m_nNextAttuneXP;
	int				m_nAttuneLvl;

	void			Set(const MUID& id, XItemData* dat);
	void			ApplyTDItem(const TD_ITEM& tdItem);

	int				GetTalentID();				///< 아이템에 바인딩 된 탈렌트 아이디
	bool			IsUsableItem();
	bool			IsQuestRelated();
	int				GetID();
};
#endif // _XITEM_H
