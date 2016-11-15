#include "stdafx.h"
#include "GUnitTestRunner.h"
#include "FBaseGame.h"
#include "GUTHelper.h"
#include "GTestForward.h"
#include "GItemHolder.h"
#include "GItemManager.h"

SUITE(Cmd_Equipitem)
{
	class Fixture : public FBaseGame
	{
	public:
		Fixture() 
		{
		}
		virtual ~Fixture()
		{
		}
	};

	TEST_FIXTURE4(Fixture, EquipItemReq)
	{
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_ARMOR_LIGHT);
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_ARMOR_MEDUIM);
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_ARMOR_HEAVY);

		GItem* pItem = test.item.GiveNewItem(m_pPlayer);

		for (int i=0; i<1000; ++i)
		{
			{
				AutoTestCaseTimer(); 
				m_pLink->OnRecv(MC_ITEM_EQUIP_REQ, 
					2, 
					NEW_INT(0),
					NEW_CHAR(ITEMSLOT_BODY)
					);
			}

			test.item.UnequipItem(m_pPlayer, pItem);			
		}		
	}
}