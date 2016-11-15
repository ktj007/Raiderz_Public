#include "StdAfx.h"
#include "GItemInfoValidator.h"
#include "GServerSystems.h"
#include "GDef.h"
#include "GItemManager.h"
#include "GValidateLogger.h"
#include "CSSoul.h"
#include "CSStrings.h"
#include "GBuffInfo.h"

bool GItemInfoValidator::Check()
{
	if (gmgr.pItemManager == NULL)	return true;

	_VLI(FILENAME_ITEMINFO);
	for (GItemManager::GItemDataMap::iterator itor = gmgr.pItemManager->Begin(); itor != gmgr.pItemManager->End(); ++itor)
	{
		GItemData* pItemData = (*itor).second;
		int nItemID = (*itor).first;

		_ASSERT(NULL != pItemData);
		if (NULL == pItemData) continue;
		if (!pItemData->m_bValidation) continue;

		_VLE(_T(ITEM_XML_TAG_ITEM));
		_VLA(_T(ITEM_XML_ATTR_ID), nItemID);
		_VLA(_T(ITEM_XML_ATTR_NAME), pItemData->m_strName);
		_VLP;

		_VLOGGER->ValidateID(nItemID);

		CheckStackAmount(pItemData);
		CheckSoul(pItemData);
		CheckSellable(pItemData);
		CheckTier(pItemData);
		CheckUsableParam(pItemData);
		CheckGather(pItemData);
		CheckEnchant(pItemData);
		CheckArmorType(pItemData);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GItemInfoValidator::CheckStackAmount(GItemData* pItemData)
{
	if (NULL == pItemData) return;

	if (0 < pItemData->m_nMaxDurability && 1 < pItemData->m_nStackAmount)
	{
		_VLOGGER->Log(L"내구도가 있는 아이템은 스택될 수 없습니다.\n");
	}

	if (true == pItemData->m_bUnique && 1 < pItemData->m_nStackAmount)
	{
		_VLOGGER->Log(L"한개만 소유할 수 있는 아이템은 스택될 수 없습니다.\n");
	}

	if (true == pItemData->m_bClaimRequired && 1 < pItemData->m_nStackAmount)
	{
		_VLOGGER->Log(L"착용시 귀속 되는 아이템은 스택될 수 없습니다.\n");
	}
}

void GItemInfoValidator::CheckSoul( GItemData* pItemData )
{
	if (NULL == pItemData) return;

	if (ARMOR_AMULET != pItemData->m_nArmorType)
	{
		if (SOULT_NONE != pItemData->m_nSoulType)
		{
			_VLOGGER->Log(L"장비 타입이 애뮬렛인 아이템만 soul_type_T()에 영향을 받습니다.\n");
		}

		if (0 < pItemData->m_nSoulMaxQuantity)
		{
			_VLOGGER->Log(L"장비 타입이 애뮬렛인 아이템만 soul_maxquantity_T()에 영향을 받습니다.\n");
		}

		if (0 < pItemData->m_nSoulLinkedItem)
		{
			_VLOGGER->Log(L"장비 타입이 애뮬렛인 아이템만 soul_stoneitem_T()에 영향을 받습니다.\n");
		}			
	}
}

void GItemInfoValidator::CheckTier(GItemData* pItemData)
{
	if (NULL == pItemData) return;

	if (ITEM_TIER_VERY_COMMON == pItemData->m_nTier)
	{
		if (ITEMTYPE_WEAPON == pItemData->m_ItemType)
		{
			_VLOGGER->Log(L"무기 아이템의 등급은 매우 흔함으로 설정할 수 없습니다.\n");
		}
		else if (ITEMTYPE_ARMOR == pItemData->m_ItemType)
		{
			_VLOGGER->Log(L"방어구 아이템의 등급은 매우 흔함으로 설정할 수 없습니다.\n");
		}
	}
}

void GItemInfoValidator::CheckSellable(GItemData* pItemData)
{
	if (NULL == pItemData) return;

	if (false == pItemData->m_bSellable)
	{
		if (ITEM_TIER_VERY_COMMON == pItemData->m_nTier)
		{
			_VLOGGER->Log(L"판매가 불가능한 아이템의 등급은 매우 흔함으로 설정할 수 없습니다.\n");
		}
	}
	else
	{
		if (true == pItemData->m_bQuestRelated)
		{
			_VLOGGER->Log(L"퀘스트 아이템은 판매가 불가능 해야 합니다.\n");
		}
	}
}

void GItemInfoValidator::CheckUsableParam(GItemData* pItemData)
{   
	if (pItemData->m_ItemType != ITEMTYPE_USABLE)	return;

	if (pItemData->m_nUsableType != USABLE_TALENT_TRAIN &&
		pItemData->m_nUsableType != USABLE_INTERACTION &&
		pItemData->m_nUsableType != USABLE_INTERACTION_DEAD)
	{
		// 위의 타입만, 복수개의 값을 갖을 수 없다.
		if (1 < pItemData->m_vecUsableParam.size())
		{
			_VLOGGER->Log(L"해당 타입의 아이템은 usable_param_T()이 한개의 값만 가져야합니다.\n");
		}
	}
	
	if (pItemData->m_nUsableType == USABLE_TALENT_TRAIN)
	{
		// 탤런트 습득 아이템은 스택되지 않습니다.
		if (pItemData->m_nStackAmount != 1)
		{
			_VLOGGER->Log(L"UsableType이 train_talent인 아이템은 StackAmount가 반드시 1_T()이여야합니다.\n");
		}

		// usable_param에 중복값을 넣을 수 없다.
		vector<int> vecParam = pItemData->m_vecUsableParam;
		
		sort(vecParam.begin(), vecParam.end());
		vector<int>::iterator it = unique(vecParam.begin(), vecParam.end());
		vecParam.resize(it - vecParam.begin());

		if (vecParam.size() != pItemData->m_vecUsableParam.size())
		{
			_VLOGGER->Log(L"usable_param_T()이 중복된 값을 포함하고 있습니다.\n");
		}


		// 모든 talent_train 아이템은 usable_param이 겹쳐서는 안된다.
		for each(int nTalentID in pItemData->m_vecUsableParam)
		{
			map<talent_id, item_id>::iterator it;
			it = m_mapTrainTalent.find(nTalentID);

			if (it == m_mapTrainTalent.end())
			{
				m_mapTrainTalent.insert(make_pair(nTalentID, pItemData->m_nID));
				continue;
			}
		
			_VLOGGER->Log(L"해당 탤런트(%d)의 라이센스 아이템(%d)이 이미 있습니다.\n", nTalentID, pItemData->m_nID);
		}
	}
}

void GItemInfoValidator::CheckGather(GItemData* pItemData)
{
	if (pItemData->m_ItemType != ITEMTYPE_GATHER)	return;

	if (pItemData->m_nGatherType == GATHER_NONE)
	{		
		_VLOGGER->Log(L"%s이 없습니다.\n", L"gather_type");
	}

	if (pItemData->m_nGatherType == GATHER_LUMBER)
	{
		_VLOGGER->Log(L"%s가 %s인 도구는 허용되지 않습니다.\n", L"gather_type", CSStrings::GatherTypeToString(GATHER_LUMBER));
	}
}

void GItemInfoValidator::CheckEnchant(GItemData* pItemData)
{
	int nBuffID = pItemData->m_EnchantStone.nLinkedBuffID;
	if (nBuffID == INVALID_BUFF_ID)	
		return;

	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (pBuffInfo)
	{
		if (!pBuffInfo->IsEnchant())
		{
			_VLOGGER->Log(L"강화석에 연결된 버프가 강화용 버프가 아닙니다.\n");
		}
	}
	else
	{
		_VLOGGER->Log(L"강화석 연결 버프 정보를 찾을 수 없습니다.\n");
	}

	if (pItemData->m_EnchantStone.nColor == ENCHANT_NONE)
	{		
		_VLOGGER->Log(L"강화석 색상이 지정되어있지 않습니다.\n");
	}

	if (!pItemData->m_vecEquipEnchantColor.empty())
	{		
		if (ENCHANT_WHITE != pItemData->m_vecEquipEnchantColor.front())
		{
			_VLOGGER->Log(L"첫번째 강화 슬롯은 W만 지정 가능합니다.\n");
		}
		
		for (size_t i=1; i<pItemData->m_vecEquipEnchantColor.size(); ++i)
		{
			if (ENCHANT_WHITE == pItemData->m_vecEquipEnchantColor[i])
			{
				_VLOGGER->Log(L"첫번째 강화 슬롯만 W로 지정 가능합니다.\n");
			}
		}

		if (pItemData->m_vecEquipEnchantColor.size() > ENCHANT_MAX_COUNT)
		{
			_VLOGGER->Log(L"강화 슬롯 최대 갯수를 초과했습니다. (최대갯수: %d)\n", ENCHANT_MAX_COUNT);
		}
	}
}

void GItemInfoValidator::CheckArmorType(GItemData* pItemData)
{
	if (pItemData->m_ItemType != ITEMTYPE_ARMOR)	return;

	if (pItemData->m_nArmorType == ARMOR_NONE)
	{
		_VLOGGER->Log(L"아이템 타입이 아머인 아이템의 아머타입이 정해 지지 않았습니다.\n");
	}
}