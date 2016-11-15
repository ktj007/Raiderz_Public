#ifndef _DBTEST_HELPER_CRAFT_H
#define _DBTEST_HELPER_CRAFT_H


struct DBT_RECP_ITEM
{
	int		nItemID;
	int16	nSlotID;
	int16	nStackAmt;
	int16	nRecpAmt;
	int64	nIUID;
};

typedef vector<DBT_RECP_ITEM> DBT_RECP_ITEM_VEC;

struct DBT_PROD_ITEM
{
	DBT_PROD_ITEM() : nItemID(1) {}

	const int	nItemID;
	int16		nSlotID;
	int16		nStackAmt;
	uint8		nDura;
	int			nColor;
	bool		bClaimed;
	bool		bPeriodItem;
	int			nUsagePeriod;
	wstring		strExpiDt;
};

struct DBT_CRAFT_CHAR
{
	int64				nAID;
	int64				nCID;
	int					nCharPtm;
	int					nXP;
	int					nMoney;
	int					nModMoney;
	int					nActorID;
	DBT_PROD_ITEM		ProdItem;
	DBT_RECP_ITEM_VEC	vRecpItem;
};


class DBTestHelper_Craft
{
public :
	static DBT_CRAFT_CHAR	CreateCraftTestCharacter(bool bCreateEmptyItemSlot = true);
	static wstring			MakeRecpInfoToString(DBT_RECP_ITEM& ri, const int16 nRecpAmt);
};


#endif