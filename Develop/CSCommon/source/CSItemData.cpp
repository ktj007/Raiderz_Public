#include "stdafx.h"
#include "CSItemData.h"
#include "CSSoul.h"
#include "CSDef.h"

CSItemData::CSItemData() 
: m_nID(-1)
, m_strName(_T(""))
, m_strDesc(_T(""))
, m_nTier(ITEM_TIER_COMMON)
, m_strMeshName(_T(""))
, m_bMeshNameBySex(false)
, m_strParentName(_T(""))
, m_nItemSlot(ITEMSLOT_NONE)
, m_nSubSlot(ITEMSLOT_NONE)
, m_ItemSoundType(SOUND_TYPE_MAX)
, m_ItemType(ITEMTYPE_NONE)
, m_nDamageAttrib(DA_NONE)
, m_nStackAmount(1)
, m_nBaseBuyingPrice(0)
, m_nBaseSellingPrice(0)
, m_nMaxDurability(0)
, m_nWeaponType(WEAPON_NONE)
, m_nHousingNPC(0)
, m_nMinDamage(0)
, m_nMaxDamage(0)
, m_nMagicMinDamage(0)
, m_nMagicMaxDamage(0)
, m_nAP(0)
, m_bUseTexColor(false)
, m_bTradable(true)
, m_bSellable(true)
, m_bUnique(false)
, m_bQuestRelated(false)
, m_bClaimRequired(false)
, m_fCraftOptionRate(0.0f)
, m_nSoulType(SOULT_NONE)
, m_nSoulMaxQuantity(0)
, m_nSoulLinkedItem(0)
, m_nArmorType(ARMOR_NONE)
, m_nTexColor(0xFFFFFFFF)
, m_bValidation(true)
, m_bLeftWeaponEquipable(false)
, m_fSpellPower(1.0f)
, m_nUsableType(USABLE_NONE)
, m_bUsableConsume(false)
, m_nGatherType(GATHER_NONE)
, m_nRepairGrade(5)
, m_bRepairable(true)
, m_bDiscardable(true)
, m_nConditionsInfoID(0)
, m_bRecipeMaterial(false)
, m_bDyeable(true)
{
}

CSItemData::~CSItemData()
{
	for (vector<CSItemMaterial*>::iterator itor = m_vecMtrls.begin();
		itor != m_vecMtrls.end(); ++itor)
	{
		CSItemMaterial* pInfo = (*itor);
		if(pInfo)
			delete(pInfo);
	}

	m_vecMtrls.clear();
}

void CSItemData::Cooking()
{
	CookNodeNames();

	switch (m_ItemType)
	{
	case ITEMTYPE_ENCHANTSTONE:
		m_bUsableConsume = true; // 반드시 소모됨
		if (m_EnchantStone.nLimitLevel <= 0)
		{
			m_EnchantStone.nTier = 0;
		}
		else if (m_EnchantStone.nLimitLevel == 1)
		{
			m_EnchantStone.nTier = 1;
		}
		else 
		{
			m_EnchantStone.nTier = ((m_EnchantStone.nLimitLevel-1)/10) + 1;
		}

		break;
	}
}

void CSItemData::CookNodeNames()
{
	// 노드 이름에 'chest'만 있으면 'chest_item'도 추가

	vector<tstring> vecItemNodeNames;
	vecItemNodeNames.push_back(_T("hat"));
	vecItemNodeNames.push_back(_T("chest"));
	vecItemNodeNames.push_back(_T("legs"));
	vecItemNodeNames.push_back(_T("hands"));
	vecItemNodeNames.push_back(_T("feet"));

	tstring strNode;
	int nNodeIndex = -1;
	bool bAlreadyAdded = false;

	for (size_t i = 0; i < m_strNodeNames.size(); i++)
	{
		TCHAR _temp[1024];
		_stprintf_s(_temp, m_strNodeNames[i].c_str());
		TCHAR* itemSuffix = _tcsstr(_temp, EQUIPMENT_ITEM_NODE_NAME_ADDITIVE_SUFFIX);

		if (itemSuffix != NULL)
		{
			bAlreadyAdded = true;
			break;
		}
	}

	if (!bAlreadyAdded)
	{
		for (size_t i = 0; i < vecItemNodeNames.size(); i++)
		{
			for (size_t j = 0; j < m_strNodeNames.size(); j++)
			{
				if (!_tcsicmp(vecItemNodeNames[i].c_str(), m_strNodeNames[j].c_str()))
				{
					nNodeIndex = (int)i;
					strNode = m_strNodeNames[j];
					break;
				}
			}
			if (nNodeIndex >= 0) break;
		}

		if (!strNode.empty())
		{
			m_strNodeNames.push_back(strNode + tstring(EQUIPMENT_ITEM_NODE_NAME_ADDITIVE_SUFFIX));
		}
	}
}

bool CSItemData::IsEnchantStone() const
{
	return (m_ItemType == ITEMTYPE_ENCHANTSTONE);
}


