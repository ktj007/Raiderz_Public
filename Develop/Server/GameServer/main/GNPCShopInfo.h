#ifndef _G_TRADE_INFO_H_
#define _G_TRADE_INFO_H_

#include "CSItemData.h"

class GConditionsInfo;
class GItemData;
class GDialogInfo;

class GNPCShopInfo : public MTestMemPool<GNPCShopInfo>
{
public:
	int				m_nID;
	float			m_fBaseBuyMod;
	float			m_fBaseSellMod;
	int				m_nFactionID;
	bool			m_bRepairable;

	GDialogInfo*	m_pDialogInfo;

	vector<GItemData*>					m_vecNormalItem;		///< 아무나 살 수 있는 아이템
	map<GItemData*, GConditionsInfo*>	m_mapConditionsItem;	///< 컨디션을 만족하는 자만 살 수 있는 아이템

	GConditionsInfo*	GetConditionsInfo(GItemData* pItemData);
	bool				HasFaction();

public:
	GNPCShopInfo();
	~GNPCShopInfo();
};

#endif//_G_TRADE_INFO_H_
