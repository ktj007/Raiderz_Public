#ifndef _X_NPCSHOP_INFO_H_
#define _X_NPCSHOP_INFO_H_

class XNPCShopInfo
{
private:
	vector<int> m_vecBuyableUsableItemID;
	vector<int> m_vecBuyableUnusableItemID;
	vector<int> m_vecUnbuyableItemID;

	vector<int>	m_vecNPCShopItem;

	float		m_fBuyMod;
	float		m_fSellMod;
	bool		m_bRepairer;

public:
	void Clear();

	vector<int>&	GetBuyableUsableItemList()	{ return m_vecBuyableUsableItemID;	}
	vector<int>&	GetBuyableUnusableItemList(){ return m_vecBuyableUnusableItemID;}
	vector<int>&	GetUnbuyableItemList()		{ return m_vecUnbuyableItemID;		}
	
	vector<int>&	GetItemList()		{ return m_vecNPCShopItem;	}

	float&			GetBuyMode()		{ return m_fBuyMod;			}
	float&			GetSellMode()		{ return m_fSellMod;		}

	bool&			GetRepairer()		{ return m_bRepairer;		}

	size_t			GetItemCount()		{ return m_vecNPCShopItem.size();}
	int				GetItemID( int i)	{ return m_vecNPCShopItem[i];	}

	bool			IsBuyable( int nID);
	bool			IsUsable( int nID);
};

#endif //_X_NPCSHOP_INFO_H_
