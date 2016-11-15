#include "stdafx.h"
#include "VItemDataValidator.h"


VItemDataValidator::VItemDataValidator()
:VDataValidator()
{
	m_nItemCount = 0;
}

VItemDataValidator::~VItemDataValidator()
{

}

void VItemDataValidator::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_nItemCount = vInfoMgr->m_pItemInfoMgr->m_ItemDataMap.size();
}

void VItemDataValidator::CheckStart( VValidatorInfoMgr* vInfoMgr )
{
	for(XItemDataMap::iterator it = vInfoMgr->m_pItemInfoMgr->m_ItemDataMap.begin(); it != vInfoMgr->m_pItemInfoMgr->m_ItemDataMap.end(); ++it)
	{
		XItemData* pItemData = it->second;

		CheckItemModel(vInfoMgr->m_pMeshInfoMgr, pItemData);
	}
}

void VItemDataValidator::CheckItemModel( VMeshInfoMgr* vMeshInfo, XItemData* pItem )
{
	// 유효성 검사 제외
	if(pItem->m_bValidation == false)
		return;

	// 0. 메쉬 이름이 있는것만 체크
	if(pItem->m_strMeshName.empty())
		return;

	// 1. 남녀공용
	if(pItem->m_bMeshNameBySex && pItem->m_ItemType != ITEMTYPE_WEAPON)
	{
		string strMeshName = "hm_" + pItem->m_strMeshName;
		if(vMeshInfo->GetMeshInfo(strMeshName) == NULL)
		{
			string strErrorMsg = (string)pItem->GetName() + " : " + strMeshName + " 메쉬가 없습니다.";
			m_ItemValidatorError.Add(EMT_FILE, strErrorMsg);
		}

		strMeshName = "hf_" + pItem->m_strMeshName;
		if(vMeshInfo->GetMeshInfo(strMeshName) == NULL)
		{
			string strErrorMsg = (string)pItem->GetName() + " : " + strMeshName + " 메쉬가 없습니다.";
			m_ItemValidatorError.Add(EMT_FILE, strErrorMsg);
		}

		return;
	}


	// 2. 메쉬이름만
	if(vMeshInfo->GetMeshInfo(pItem->m_strMeshName) == NULL)
	{
		string strErrorMsg = (string)pItem->GetName() + " : " + pItem->m_strMeshName + " 메쉬가 없습니다.";
		m_ItemValidatorError.Add(EMT_FILE, strErrorMsg);
	}
}

void VItemDataValidator::CheckItemTalent( VTalentInfoMgr* vTalentInfo, XItemData* pItem )
{
	if(pItem->m_nUsableType == USABLE_TALENT_USE)
	{
		for(vector<int>::iterator it = pItem->m_vecUsableParam.begin(); it != pItem->m_vecUsableParam.end(); ++it)
		{
			XTalentInfo* pTalentInfo = vTalentInfo->m_TalentInfoMgr.Get(*it);
			if(pTalentInfo == NULL)
			{
				char chTalentID[128] = {0, };
				sprintf_s(chTalentID, "%d", *it);

				string strErrorMsg = (string)pItem->GetName() + " : " + string(chTalentID) + " 탤런트가 없습니다.";
				m_ItemValidatorError.Add(EMT_LINK, strErrorMsg);

			}
		}
	}
}