#include "stdafx.h"
#include "GTestForward.h"
#include "GItemDurabilityUpdater.h"
#include "FBaseGame.h"
#include "GItemManager.h"
#include "GItemHolder.h"
#include "GConfig.h"
#include "GPlayerPVPArea.h"
#include "GPlayerSetterForTest.h"
#include "GEquipmentSlot.h"

SUITE(ItemDurabilityUpdater)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_nOriServerMode = GConfig::m_nServerMode;
			GConfig::m_nServerMode = SERVER_MODE_NORMAL;

			m_pNPC = test.npc.SpawnNPC(m_pField);
		}
		virtual ~Fixture()
		{
			GConfig::m_nServerMode = m_nOriServerMode;
		}

		DECLWRAPPER_ItemMgr;
		DECLWRAPPER_NPCInfoMgr;

		int						m_nOriServerMode;
		GItemDurabilityUpdater	m_ItemDurabilityUpdater;
		GEntityNPC*				m_pNPC;
	};

	// 공격시 내구도 소모
	TEST_FIXTURE(Fixture, LostDurabilityByCombat_Attack)
	{
		GItemData* pWeaponItemData = test.item.NewItemData();
		pWeaponItemData->m_nMaxDurability = 100;
		pWeaponItemData->m_ItemType = ITEMTYPE_WEAPON;
		GItem* pWeaponItem = test.item.EquipNewItem(m_pPlayer, pWeaponItemData->m_nID, ITEMSLOT_RWEAPON);

		pWeaponItem->m_nDurability = pWeaponItemData->m_nMaxDurability;				
		for (int i=0; i<10000; i++)
		{
			int nBeforeWeaponItemDuration = pWeaponItem->m_nDurability;
			m_ItemDurabilityUpdater.LostDurabilityByCombat(m_pPlayer, m_pNPC, false, false);
			CHECK(nBeforeWeaponItemDuration - MAX_DURABILITY_LOST_RATE_WEAPON <= pWeaponItem->m_nDurability);
		}				
	}

	// 피격시 내구도 소모
	TEST_FIXTURE(Fixture, LostDurabilityByCombat_Attacked)
	{
		GItemData* pEquipmentItemData = test.item.NewItemData();
		pEquipmentItemData->m_nMaxDurability = 100;
		pEquipmentItemData->m_ItemType = ITEMTYPE_ARMOR;
		GItem* pEquipmentItem = test.item.EquipNewItem(m_pPlayer, pEquipmentItemData->m_nID, ITEMSLOT_BODY);

		pEquipmentItem->m_nDurability = pEquipmentItemData->m_nMaxDurability;

		for (int i=0; i<1000; i++)
		{
			int nBeforeEquipmentItemDuration = pEquipmentItem->m_nDurability;
			m_ItemDurabilityUpdater.LostDurabilityByCombat(m_pNPC, m_pPlayer, false, false);
			CHECK(nBeforeEquipmentItemDuration - MAX_DURABILITY_LOST_RATE_EQUIPMENT <= pEquipmentItem->m_nDurability);
		}
	}

	// 크리로 피격시 내구도 소모
	TEST_FIXTURE(Fixture, LostDurabilityByCombat_AttackedByCritical)
	{
		GItemData* pEquipmentItemData = test.item.NewItemData();
		pEquipmentItemData->m_nMaxDurability = 100;
		pEquipmentItemData->m_ItemType = ITEMTYPE_ARMOR;
		GItem* pEquipmentItem = test.item.EquipNewItem(m_pPlayer, pEquipmentItemData->m_nID, ITEMSLOT_BODY);

		pEquipmentItem->m_nDurability = pEquipmentItemData->m_nMaxDurability;

		for (int i=0; i<1000; i++)
		{
			int nBeforeEquipmentItemDuration = pEquipmentItem->m_nDurability;
			m_ItemDurabilityUpdater.LostDurabilityByCombat(m_pNPC, m_pPlayer, true, false);
			CHECK(nBeforeEquipmentItemDuration - MAX_DURABILITY_LOST_RATE_EQUIPMENT_CRI <= pEquipmentItem->m_nDurability);
		}

	}

	// 방어시 내구도 소모
	TEST_FIXTURE(Fixture, LostDurabilityByCombat_Defense)
	{
		GItemData* pWeaponItemData = test.item.NewItemData();
		pWeaponItemData->m_nMaxDurability = 100;
		pWeaponItemData->m_ItemType = ITEMTYPE_WEAPON;
		GItemData* pShieldItemData = test.item.NewItemData();
		pShieldItemData->m_nMaxDurability = 100;
		pShieldItemData->m_ItemType = ITEMTYPE_ARMOR;
		pShieldItemData->m_nArmorType = ARMOR_SHIELD;


		// 무기로 방어
		GItem* pWeaponItem = test.item.EquipNewItem(m_pPlayer, pWeaponItemData->m_nID, ITEMSLOT_RWEAPON);
		pWeaponItem->m_nDurability = pWeaponItemData->m_nMaxDurability;				
		for (int i=0; i<1000; i++)
		{
			int nBeforeWeaponItemDuration = pWeaponItem->m_nDurability;
			m_ItemDurabilityUpdater.LostDurabilityByCombat(m_pNPC, m_pPlayer, false, true);
			CHECK(nBeforeWeaponItemDuration - MAX_DURABILITY_LOST_RATE_EQUIPMENT <= pWeaponItem->m_nDurability);
		}

		// 방패로 방어				
		GItem* pShieldItem = test.item.EquipNewItem(m_pPlayer, pShieldItemData->m_nID, ITEMSLOT_RWEAPON);
		pShieldItem->m_nDurability = pShieldItemData->m_nMaxDurability;				
		for (int i=0; i<1000; i++)
		{
			int nBeforeShieldItemDuration = pShieldItem->m_nDurability;
			int nBeforeWeaponItemDuration = pWeaponItem->m_nDurability;
			m_ItemDurabilityUpdater.LostDurabilityByCombat(m_pNPC, m_pPlayer, false, true);
			CHECK(nBeforeShieldItemDuration - MAX_DURABILITY_LOST_RATE_EQUIPMENT <= pShieldItem->m_nDurability);
			CHECK_EQUAL(nBeforeWeaponItemDuration, pWeaponItem->m_nDurability);
		}				
	}

	// 죽었을때 내구도 소모
	namespace LostDurabilityByDie
	{
		// PVP 필드에서 플레이어에게 죽었을때는 내구도가 달지 않는다.
		TEST_FIXTURE(Fixture, NotLostDurablityInPVPField)
		{
			test.player.SetMockPlayerPVPArea_LocateAndTeamOne(m_pPlayer);			

			GItem* pItem = test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_SLASH);			
			int nBeforeItemDurability = pItem->m_nDurability;
			
			GEntityPlayer* pKiller = test.player.NewPlayer(m_pField);
			m_pPlayer->SetKiller(pKiller->GetUID());
			m_pPlayer->doDie();

			CHECK_EQUAL(nBeforeItemDurability, pItem->m_nDurability);
		}
	
		TEST_FIXTURE(Fixture, LostDurabilityByDie)
		{
			GItem* pItem = test.item.EquipNewShield(m_pPlayer);

			int nBeforeItemDurability = pItem->m_nDurability;

			m_pPlayer->doDie();

			CHECK_EQUAL(nBeforeItemDurability - GMath::TruncateToInt(pItem->m_pItemData->m_nMaxDurability * 0.10f), pItem->m_nDurability);	
		}
	}

	
	// 내구도가 없을때의 방어력
	TEST_FIXTURE(Fixture, EquipmentArmorPoint_DurabilityZero)
	{
		GItemData* pEquipmentItemData = test.item.NewItemData();
		pEquipmentItemData->m_ItemType = ITEMTYPE_ARMOR;
		pEquipmentItemData->m_nItemSlot = ITEMSLOT_BODY;
		pEquipmentItemData->m_nMaxDurability = 100;
		pEquipmentItemData->m_nAP = 100;
		GItem* pEquipmentItem = test.item.EquipNewItem(m_pPlayer, pEquipmentItemData->m_nID, pEquipmentItemData->m_nItemSlot);

		pEquipmentItem->m_nDurability = 0;

		int nDurabilityZeroAP = m_pPlayer->GetItemHolder()->GetEquipment().GetTotalAP();
		CHECK_EQUAL(0, nDurabilityZeroAP);			
	}	

	// 방어구 착용에 필요한 탤런트가 없을때의 방어력
	TEST_FIXTURE(Fixture, EquipmentArmorPoint_NotTrainTalentForEquip)
	{
		GItemData* pEquipmentItemData = test.item.NewItemData();
		pEquipmentItemData->m_ItemType = ITEMTYPE_ARMOR;
		pEquipmentItemData->m_nItemSlot = ITEMSLOT_BODY;
		pEquipmentItemData->m_nMaxDurability = 100;
		pEquipmentItemData->m_nAP = 100;
		pEquipmentItemData->m_nArmorType = ARMOR_MEDIUM;
		GItem* pEquipmentItem = test.item.EquipNewItem(m_pPlayer, pEquipmentItemData->m_nID, pEquipmentItemData->m_nItemSlot);
		pEquipmentItem->m_nDurability = pEquipmentItemData->m_nMaxDurability;

		test.talent.UntrainAll(m_pPlayer);

		int nDurabilityZeroAP = m_pPlayer->GetItemHolder()->GetEquipment().GetTotalAP();
		CHECK_EQUAL(0, nDurabilityZeroAP);			
	}	
}