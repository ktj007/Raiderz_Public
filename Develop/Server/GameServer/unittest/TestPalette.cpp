#include "stdafx.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GPaletteSystem.h"
#include "GPalette.h"
#include "GPaletteItem.h"
#include "CCommandTable.h"
#include "MockLink.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"

SUITE(Palette)
{
	struct FPalette : public FBaseMockLink
	{
		FPalette()
		{
			GUTHelper_Field::NewMockFieldAndPlayer(m_pField, m_pPlayer);
		}

		~FPalette()
		{
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
		}

		GUTHelper		m_Helper;

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;	
	};

	namespace MakeTD_Palette
	{
		// 팔레트가 비어 있는 경우
		TEST_FIXTURE(FPalette, PaletteIsEmpty)
		{
			TD_PALETTE tdPalette;
			gsys.pPaletteSystem->MakeTD_PALETTE(m_pPlayer, tdPalette);

			CHECK_EQUAL(true, tdPalette.IsEmpty());
		}

		TEST_FIXTURE(FPalette, Success)
		{
			GItem* pItem = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID);

			TD_PALETTE tdPalette;
			gsys.pPaletteSystem->MakeTD_PALETTE(m_pPlayer, tdPalette);
			
			CHECK_EQUAL(false, tdPalette.IsEmpty());
			uint8 nIndex = gsys.pPaletteSystem->NumAndSlotToIndex(PALETTENUM_1, PALETTESLOT_1);
			CHECK(PIT_ITEM == tdPalette.paletteItems[nIndex].nType);
			CHECK_EQUAL(pItem->m_pItemData->m_nID, tdPalette.paletteItems[nIndex].nItemIDorTalentID);
		}
	}

	namespace Select
	{
		// 잘못된 팔레트 숫자
		TEST_FIXTURE(FPalette, InvalidNum)
		{
			CHECK_EQUAL(false, gsys.pPaletteSystem->Select(m_pPlayer, PALETTENUM_MAX));
		}
		
		TEST_FIXTURE(FPalette, Success)
		{
			m_pPlayer->GetPalette().SelectPalette(PALETTENUM_1);

			CHECK_EQUAL(true, gsys.pPaletteSystem->Select(m_pPlayer, PALETTENUM_2));
			CHECK(PALETTENUM_2 == m_pPlayer->GetPalette().GetSelectedPalette());
		}
	}

	namespace SetPrimary
	{
		// 잘못된 팔레트 숫자
		TEST_FIXTURE(FPalette, InvalidNum)
		{
			CHECK_EQUAL(false, gsys.pPaletteSystem->SetPrimary(m_pPlayer, (PALETTE_NUM) 100));
		}

		namespace Success
		{
			// 보조 장비에 세팅된 팔레트를 주장비에 세팅하려는 경우			
			TEST_FIXTURE(FPalette, SecondaryNum)
			{
				m_pPlayer->GetPalette().SetSecondary(PALETTENUM_1);

				MockLink* pLink = NewLink(m_pPlayer);
				CHECK_EQUAL(true, gsys.pPaletteSystem->SetPrimary(m_pPlayer, PALETTENUM_1));
				CHECK(PALETTENUM_1 == m_pPlayer->GetPalette().GetPrimary());
				CHECK(PALETTENUM_MAX == m_pPlayer->GetPalette().GetSecondary());

				CHECK(pLink->GetCommandCount() == 1);
				CHECK(pLink->GetCommand(0).GetID() == MC_PALETTE_SET_PRIMARY);
				CHECK(PALETTENUM_1 == (PALETTE_NUM) pLink->GetParam<unsigned char>(0, 0));
			}

			TEST_FIXTURE(FPalette, Normal)
			{
				MockLink* pLink = NewLink(m_pPlayer);
				CHECK_EQUAL(true, gsys.pPaletteSystem->SetPrimary(m_pPlayer, PALETTENUM_1));
				CHECK(PALETTENUM_1 == m_pPlayer->GetPalette().GetPrimary());
				CHECK(PALETTENUM_MAX == m_pPlayer->GetPalette().GetSecondary());

				CHECK(pLink->GetCommandCount() == 1);
				CHECK(pLink->GetCommand(0).GetID() == MC_PALETTE_SET_PRIMARY);
				CHECK(PALETTENUM_1 == (PALETTE_NUM) pLink->GetParam<unsigned char>(0, 0));
			}
		}
	}

	namespace SetSecondary
	{
		// 잘못된 팔레트 숫자
		TEST_FIXTURE(FPalette, InvalidNum)
		{
			CHECK_EQUAL(false, gsys.pPaletteSystem->SetSecondary(m_pPlayer, (PALETTE_NUM) 100));
		}

		namespace Success
		{
			// 주장비에 세팅된 팔레트를 보조 장비에 세팅하려는 경우			
			TEST_FIXTURE(FPalette, PrimaryNum)
			{
				m_pPlayer->GetPalette().SetPrimary(PALETTENUM_1);

				MockLink* pLink = NewLink(m_pPlayer);
				CHECK_EQUAL(true, gsys.pPaletteSystem->SetSecondary(m_pPlayer, PALETTENUM_1));
				CHECK(PALETTENUM_1 == m_pPlayer->GetPalette().GetSecondary());
				CHECK(PALETTENUM_MAX == m_pPlayer->GetPalette().GetPrimary());

				CHECK(pLink->GetCommandCount() == 1);
				CHECK(pLink->GetCommand(0).GetID() == MC_PALETTE_SET_SECONDARY);
				CHECK(PALETTENUM_1 == (PALETTE_NUM) pLink->GetParam<unsigned char>(0, 0));
			}

			TEST_FIXTURE(FPalette, Normal)
			{
				MockLink* pLink = NewLink(m_pPlayer);
				CHECK_EQUAL(true, gsys.pPaletteSystem->SetSecondary(m_pPlayer, PALETTENUM_1));
				CHECK(PALETTENUM_1 == m_pPlayer->GetPalette().GetSecondary());
				CHECK(PALETTENUM_MAX == m_pPlayer->GetPalette().GetPrimary());

				CHECK(pLink->GetCommandCount() == 1);
				CHECK(pLink->GetCommand(0).GetID() == MC_PALETTE_SET_SECONDARY);
				CHECK(PALETTENUM_1 == (PALETTE_NUM) pLink->GetParam<unsigned char>(0, 0));
			}
		}
	}

	namespace PutUp
	{
		// 가지고 있지 않은 아이템
		TEST_FIXTURE(FPalette, NotHaveItem)
		{
			GItemData* pItemData = m_Helper.NewItemData();

			CHECK_EQUAL(false, gsys.pPaletteSystem->PutUp(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItemData->m_nID));
		}

		// 아이템 타입이 Usable이 아닌 경우
		TEST_FIXTURE(FPalette, NotUsableItem)
		{
			GItem* pItem = m_Helper.GiveNewItem(m_pPlayer);
			pItem->m_pItemData->m_ItemType = ITEMTYPE_WEAPON;

			CHECK_EQUAL(false, gsys.pPaletteSystem->PutUp(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID));
		}

		// 배우지 않은 탤런트
		TEST_FIXTURE(FPalette, NotLearnedTalent)
		{
			GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
			pTalentInfo->m_bNeedTraining = true;

			CHECK_EQUAL(false, gsys.pPaletteSystem->PutUp(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PIT_TALENT, pTalentInfo->m_nID));
		}

		// 패시브 탤런트
		TEST_FIXTURE(FPalette, PassiveTalent)
		{
			GTalentInfo* pTalentInfo = m_Helper.LearnNewTalent(m_pPlayer);
			pTalentInfo->m_nSkillType = ST_PASSIVE;

			CHECK_EQUAL(false, gsys.pPaletteSystem->PutUp(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PIT_TALENT, pTalentInfo->m_nID));
		}

		// 잘못된 팔레트 숫자
		TEST_FIXTURE(FPalette, InvalidNum)
		{
			GItemData* pItemData = m_Helper.NewItemData();

			CHECK_EQUAL(false, gsys.pPaletteSystem->PutUp(m_pPlayer, PALETTENUM_MAX, PALETTESLOT_1, PIT_ITEM, pItemData->m_nID));
		}

		// 잘못된 팔레트 숫자
		TEST_FIXTURE(FPalette, InvalidSlot)
		{
			GItemData* pItemData = m_Helper.NewItemData();

			CHECK_EQUAL(false, gsys.pPaletteSystem->PutUp(m_pPlayer, PALETTENUM_1, PALETTESLOT_MAX, PIT_ITEM, pItemData->m_nID));
		}

		namespace Success
		{
			// 해당 슬롯이 비어 있는 경우
			TEST_FIXTURE(FPalette, SlotIsEmpty)
			{
				GTalentInfo* pTalentInfo = m_Helper.LearnNewTalent(m_pPlayer);

				MockLink* pLink = NewLink(m_pPlayer);
				CHECK_EQUAL(true, gsys.pPaletteSystem->PutUp(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PIT_TALENT, pTalentInfo->m_nID));
				CHECK_EQUAL(pTalentInfo->m_nID, m_pPlayer->GetPalette().GetPaletteItem(PALETTENUM_1, PALETTESLOT_1)->m_nItemIDorTalentID);
				CHECK(PIT_TALENT == m_pPlayer->GetPalette().GetPaletteItem(PALETTENUM_1, PALETTESLOT_1)->m_nType);

				CHECK(pLink->GetCommandCount() == 1);
				CHECK(pLink->GetCommand(0).GetID() == MC_PALETTE_PUTUP);
				CHECK(PALETTENUM_1 == (PALETTE_NUM) pLink->GetParam<unsigned char>(0, 0));
				CHECK(PALETTESLOT_1 == (PALETTE_SLOT) pLink->GetParam<unsigned char>(0, 1));
				CHECK(PIT_TALENT == (PALETTE_ITEM_TYPE) pLink->GetParam<unsigned char>(0, 2));
				CHECK_EQUAL(pTalentInfo->m_nID, pLink->GetParam<int>(0, 3));
			}

			// 이미 해당 슬롯이 비어 있지 않은 경우
			TEST_FIXTURE(FPalette, SlotIsNotEmpty)
			{
				GItem* pItem = m_Helper.GiveNewItem(m_pPlayer);
				pItem->m_pItemData->m_ItemType = ITEMTYPE_WEAPON;
				GTalentInfo* pTalentInfo = m_Helper.LearnNewTalent(m_pPlayer);
				m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID);

				MockLink* pLink = NewLink(m_pPlayer);
				CHECK_EQUAL(true, gsys.pPaletteSystem->PutUp(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PIT_TALENT, pTalentInfo->m_nID));
				CHECK_EQUAL(pTalentInfo->m_nID, m_pPlayer->GetPalette().GetPaletteItem(PALETTENUM_1, PALETTESLOT_1)->m_nItemIDorTalentID);
				CHECK(PIT_TALENT == m_pPlayer->GetPalette().GetPaletteItem(PALETTENUM_1, PALETTESLOT_1)->m_nType);

				CHECK(pLink->GetCommandCount() == 1);
				CHECK(pLink->GetCommand(0).GetID() == MC_PALETTE_PUTUP);
				CHECK(PALETTENUM_1 == (PALETTE_NUM) pLink->GetParam<unsigned char>(0, 0));
				CHECK(PALETTESLOT_1 == (PALETTE_SLOT) pLink->GetParam<unsigned char>(0, 1));
				CHECK(PIT_TALENT == (PALETTE_ITEM_TYPE) pLink->GetParam<unsigned char>(0, 2));
				CHECK_EQUAL(pTalentInfo->m_nID, pLink->GetParam<int>(0, 3));
			}			
		}		
	}

	namespace PutDown
	{
		// 해당 슬롯이 비어 있는 경우
		TEST_FIXTURE(FPalette, SlotIsEmpty)
		{
			CHECK_EQUAL(false, gsys.pPaletteSystem->PutDown(m_pPlayer, PALETTENUM_1, PALETTESLOT_1));
		}		

		// 팔레트가 잠긴 경우
		TEST_FIXTURE(FPalette, Locked)
		{
			m_pPlayer->GetPalette().Lock();
			GItem* pItem = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID);
			CHECK_EQUAL(false, gsys.pPaletteSystem->PutDown(m_pPlayer, PALETTENUM_1, PALETTESLOT_1));

			m_pPlayer->GetPalette().Unlock();
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID);
			CHECK_EQUAL(true, gsys.pPaletteSystem->PutDown(m_pPlayer, PALETTENUM_1, PALETTESLOT_1));
		}	

		// 잘못된 팔레트 숫자
		TEST_FIXTURE(FPalette, InvalidNum)
		{
			GItem* pItem = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID);

			CHECK_EQUAL(false, gsys.pPaletteSystem->PutDown(m_pPlayer, PALETTENUM_MAX, PALETTESLOT_1));
		}

		// 잘못된 슬롯
		TEST_FIXTURE(FPalette, InvalidSlot)
		{
			GItem* pItem = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID);

			CHECK_EQUAL(false, gsys.pPaletteSystem->PutDown(m_pPlayer, PALETTENUM_1, PALETTESLOT_MAX));
		}

		TEST_FIXTURE(FPalette, Success)
		{
			GItem* pItem = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID);

			MockLink* pLink = NewLink(m_pPlayer);
			CHECK_EQUAL(true, gsys.pPaletteSystem->PutDown(m_pPlayer, PALETTENUM_1, PALETTESLOT_1));

			CHECK(pLink->GetCommandCount() == 1);
			CHECK(pLink->GetCommand(0).GetID() == MC_PALETTE_PUTDOWN);

			vector<unsigned char> vecPaletteNum;
			vector<unsigned char> vecPaletteSlot;
			pLink->GetCommand(0).GetBlob(vecPaletteNum, 0);
			pLink->GetCommand(0).GetBlob(vecPaletteSlot, 1);
			CHECK(PALETTENUM_1 == vecPaletteNum[0]);
			CHECK(PALETTESLOT_1 == vecPaletteSlot[0]);
		}
	}

	namespace Change
	{
		// 바꾸려는 슬롯이 같은 경우
		TEST_FIXTURE(FPalette, ChangeEqualSlot)
		{
			GItem* pItem = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem->m_pItemData->m_nID);

			CHECK_EQUAL(false, gsys.pPaletteSystem->Change(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PALETTENUM_1, PALETTESLOT_1));
		}

		// 바꾸려는 슬롯이 둘다 비어 있는 경우
		TEST_FIXTURE(FPalette, ChangeEmptySlot)
		{
			CHECK_EQUAL(false, gsys.pPaletteSystem->Change(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PALETTENUM_1, PALETTESLOT_2));
		}

		// 잘못된 팔레트 숫자
		TEST_FIXTURE(FPalette, InvalidNum)
		{
			GItem* pItem1 = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem1->m_pItemData->m_nID);
			GItem* pItem2 = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_2, PIT_ITEM, pItem2->m_pItemData->m_nID);

			CHECK_EQUAL(false, gsys.pPaletteSystem->Change(m_pPlayer, PALETTENUM_MAX, PALETTESLOT_1, PALETTENUM_1, PALETTESLOT_2));
		}

		// 잘못된 슬롯
		TEST_FIXTURE(FPalette, InvalidSlot)
		{
			GItem* pItem1 = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem1->m_pItemData->m_nID);
			GItem* pItem2 = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_2, PIT_ITEM, pItem2->m_pItemData->m_nID);

			CHECK_EQUAL(false, gsys.pPaletteSystem->Change(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PALETTENUM_1, PALETTESLOT_MAX));
		}

		TEST_FIXTURE(FPalette, Success)
		{
			GItem* pItem1 = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, pItem1->m_pItemData->m_nID);
			GItem* pItem2 = m_Helper.GiveNewItem(m_pPlayer);
			m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_2, PIT_ITEM, pItem2->m_pItemData->m_nID);

			MockLink* pLink = NewLink(m_pPlayer);
			CHECK_EQUAL(true, gsys.pPaletteSystem->Change(m_pPlayer, PALETTENUM_1, PALETTESLOT_1, PALETTENUM_1, PALETTESLOT_2));

			CHECK(pLink->GetCommandCount() == 1);
			CHECK(pLink->GetCommand(0).GetID() == MC_PALETTE_CHANGE);
			CHECK(PALETTENUM_1 == (PALETTE_NUM) pLink->GetParam<unsigned char>(0, 0));
			CHECK(PALETTESLOT_1 == (PALETTE_SLOT) pLink->GetParam<unsigned char>(0, 1));
			CHECK(PALETTENUM_1 == (PALETTE_NUM) pLink->GetParam<unsigned char>(0, 2));
			CHECK(PALETTESLOT_2 == (PALETTE_SLOT) pLink->GetParam<unsigned char>(0, 3));
		}
	}	
}