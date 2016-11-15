#ifndef _DBTEST_HELPER_QUEST_H
#define _DBTEST_HELPER_QUEST_H

struct DBT_POS
{
public :
	DBT_POS() : nFieldID(0), fX(0.0f), fY(0.0f), fZ(0.0f)
	{

	}

	int		nFieldID;
	float	fX;
	float	fY;
	float	fZ;
};

struct DBT_QUEST_OBJ_ITEM
{
	DBT_QUEST_OBJ_ITEM() : nItemID(0), nSlotID(0), nStackAmt(0), nIUID(0) {}

	int		nItemID;
	int16	nSlotID;
	int		nStackAmt;
	int64	nIUID;
};

typedef vector<DBT_QUEST_OBJ_ITEM> DBT_QUEST_OBJ_ITEM_VEC;

struct DBT_QUEST_CHAR
{
	DBT_QUEST_CHAR() : nCID(0), nLevel(0), nCharPtm(0), nSlotID(0), nQuestID(0), nXP(0), nMoney(0)
	{
	
	}
	int64					nAID;
	int64					nCID;
	int						nLevel;
	int						nCharPtm;
	uint8					nSlotID;
	int						nQuestID;
	int						nXP;
	int						nMoney;
	int						nModMoney;
	wstring					strAcceptDt;
	wstring					strExpiDt;

	DBT_QUEST_OBJ_ITEM_VEC	vObjItemInst;
	DBT_POS					Pos;
};


class DBTestHelper_Quest
{
public :
	static DBT_QUEST_CHAR	CreateQuestTestCharacter(const bool bChallanger = false);
	static DBT_QUEST_CHAR	CreateQuestObjectTestCharacter();
	static wstring			MakeObjItemToString(DBT_QUEST_OBJ_ITEM& ObjItem, const int16 nRemoveAmt);
};


#endif