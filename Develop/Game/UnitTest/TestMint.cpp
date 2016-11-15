#include "stdafx.h"

#include "XGame.h"
#include "XCmdHandler_Global.h"
#include "XFieldInfo.h"
#include "XItemManager.h"
#include "XUnitTestList.h"
#include "XCommonFunction.h"

SUITE(GlueTest)
{
/*
	struct GlueFunctionsCall : public XGlueFunctions
	{
	public:
		XGlueSystemFunction& GetSystemFunction()	{ return m_SystemFunction;}
		XGlueMyInfoFunction& GetMyInfoFunction()	{ return m_MyInfoFunction;}
		XGlueGameFunction& GetGameFunction(){ return m_GameFunction;}
	};

	void MakeTD_MyInfo(TD_MYINFO& out)
	{
		strcpy_s(out.szName, "test_madduck");
		strcpy_s(out.szSurname, "test_¹ÌÄ£¶±");

		out.nSex	= SEX_MALE;
		out.nLevel	= 34;
		out.nSilver = 999;
		out.nMaxHP	= 70;
		out.nMaxEN	= 90;
		out.nMaxSTA = 140;
		out.nXP		= 11053;
		out.nSTR	= 3;
		out.nDEX	= 1;
		out.nINT	= 1;
		out.nCHA	= 3;
		out.nCON	= 7;

		out.nCurHP	= 70;
		out.nCurEN	= 90;
		out.nCurSTA	= 140;

		out.nExpPercent	= 70;
		out.nRemainTP	= 99;

		//out.nEquipedItemID[0] = MUID(1234, 5678);
	}

	TEST(TestMyInfoGlue)
	{
		TD_MYINFO MyInfo;
		MakeTD_MyInfo(MyInfo);

		vector<TD_ITEM> vecTDItem;
		vector<int> vecTalentID;		
		vector<TD_PLAYERQUEST> vecTDPlayerQuest;
		vector<TD_GUILD> vecTDGuild;
		vector<TD_GUILD_MEMBER> vecTDGuildMember;
		TD_PALETTE tdPalette;
		vector<TD_FACTION> vecTDFaction;

		gvar.MyInfo.SetFrom_TD_MYINFO(MyInfo, vecTDItem, vecTalentID, vecTDPlayerQuest, vecTDGuild, vecTDGuildMember, tdPalette, vecTDFaction);
		gvar.MyInfo.MyUID = MUID(12,12);

		GlueFunctionsCall glue;

		if(glue.GetMyInfoFunction().GetUID() != MUID(12, 12))
		{
			CHECK(NULL);
			return;
		}

		CHECK_EQUAL(glue.GetMyInfoFunction().GetName(), "test_madduck");
	}

*/
}
