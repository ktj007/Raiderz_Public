#include "stdafx.h"
#include "XItemValidator.h"
#include "XItemManager.h"

bool XItemValidator::Check()
{
	_VLI(FILENAME_ITEM);

	bool ret = true;

	XItemDataMap& itemDataMap = info.item->GetItemDataMap();

	for (XItemDataMap::iterator itor = itemDataMap.begin(); itor != itemDataMap.end(); ++itor)
	{
		XItemData* pItemData = static_cast<XItemData*>((*itor).second);
		if(CheckFile(pItemData) == false) ret = false;
		if (CheckVisual(pItemData) == false) ret = false;
		if (CheckWeaponMeahFindNode(pItemData) == false) ret = false;
	}

	return ret;
}

//무기아이템의 메쉬이름과 노드 이름이 존재하는지
bool XItemValidator::CheckWeaponMeahFindNode( XItemData* pItemData )
{
	if (pItemData->m_nWeaponType != WEAPON_NONE && !pItemData->m_strMeshName.empty())
	{
		RActor* pItem = new RActorMLeaf();
		wstring dd = pItemData->m_strMeshName + L".elu";
		if (pItem->Create(MLocale::ConvUTF16ToAnsi(dd.c_str()).c_str()))
		{
			bool bRet = true;

			for (size_t nNodeNameIndex = 0; nNodeNameIndex < pItemData->m_strNodeNames.size(); nNodeNameIndex++)
			{
				RActorNode* pActorNode = pItem->GetActorNode(MLocale::ConvUTF16ToAnsi(pItemData->m_strNodeNames[nNodeNameIndex].c_str()).c_str());

				if (pActorNode == NULL)
				{
					bRet = false;

					_VLE(L"ITEM");

					_VLA(L"id", pItemData->m_nID);
					_VLA(L"mesh_name", pItemData->m_strMeshName.c_str());
					_VLA(L"node_name", pItemData->m_strNodeNames[nNodeNameIndex].c_str());

					_VLP;

					m_Logger.Log(L"무기아이템의 node 이름이 다릅니다.");
				}
			}

			SAFE_DELETE(pItem);
			return bRet;
		}

		_VLE(L"ITEM");

		_VLA(L"id", pItemData->m_nID);
		_VLA(L"mesh_name", pItemData->m_strMeshName.c_str());

		_VLP;

		m_Logger.Log(L"무기아이템 메쉬 로딩 실패");

		SAFE_DELETE(pItem);
		return false;
	}

	return true;
}

bool XItemValidator::CheckVisual( XItemData* pItemData )
{
	if (pItemData->m_bMeshNameBySex == true)
	{
		if ( (!wcsnicmp(pItemData->m_strMeshName.c_str(), L"hf_", 3)) ||
			(!wcsnicmp(pItemData->m_strMeshName.c_str(), L"hm_", 3)) )
		{
			_VLE(L"ITEM");

			_VLA(L"id", pItemData->m_nID);
			_VLA(L"mesh_name", pItemData->m_strMeshName.c_str());
			_VLA(L"mesh_name_by_sex", (pItemData->m_bMeshNameBySex) ? L"TRUE" : L"FALSE");

			_VLP;


			m_Logger.Log(L"남녀 공용 아이템은 hf_ or hm_ 접두어가 없어야 합니다\n");
			return false;
		}
	}

	return true;
}

bool XItemValidator::CheckFile( XItemData* pItemData )
{
	wstring strMeshName = pItemData->m_strMeshName;

	// 메쉬 이름이 없으면 검사를 안한다.
	if(strMeshName.empty())
		return true;;

	bool bResult = true;

	// 성별 검사
	if(pItemData->m_bMeshNameBySex)
	{
		wstring strMeshNameSex = PATH_PLAYER_FEMALE + wstring(L"hf_") + strMeshName + L".elu";
		if (!ExistFile(strMeshNameSex.c_str()))
		{
			_VLE(L"ITEM");
			_VLA(L"id", pItemData->m_nID);
			_VLA(L"mesh_name", pItemData->m_strMeshName.c_str());
			_VLA(L"mesh_name_by_sex", (pItemData->m_bMeshNameBySex) ? L"TRUE" : L"FALSE");
			_VLA(L"mesh full file name", strMeshNameSex.c_str());
			_VLP;
			m_Logger.Log(L"오타이거나 메쉬파일이 없습니다.\n");
			bResult = false;
		}

		strMeshNameSex = PATH_PLAYER_MALE + wstring(L"hm_") + strMeshName + L".elu";
		if (!ExistFile(strMeshNameSex.c_str()))
		{
			_VLE(L"ITEM");
			_VLA(L"id", pItemData->m_nID);
			_VLA(L"mesh_name", pItemData->m_strMeshName.c_str());
			_VLA(L"mesh_name_by_sex", (pItemData->m_bMeshNameBySex) ? L"TRUE" : L"FALSE");
			_VLA(L"mesh full fil name", strMeshNameSex.c_str());
			_VLP;
			m_Logger.Log(L"오타이거나 메쉬파일이 없습니다.\n");
			bResult = false;
		}
	}
	else
	{
		if(pItemData->m_strMeshPath.empty() == false)
		{
			wstring strItemFullName = pItemData->m_strMeshPath + strMeshName + L".elu";

			if (!ExistFile(strItemFullName.c_str()))
			{
				_VLE(L"ITEM");
				_VLA(L"id", pItemData->m_nID);
				_VLA(L"mesh_name", pItemData->m_strMeshName.c_str());
				_VLA(L"mesh_name_by_sex", (pItemData->m_bMeshNameBySex) ? L"TRUE" : L"FALSE");
				_VLA(L"mesh full fil name", strItemFullName.c_str());
				_VLP;
				m_Logger.Log(L"오타이거나 메쉬파일이 없습니다.\n");
				bResult = false;
			}
		}
		else
		{
			_VLE(L"ITEM");
			_VLA(L"id", pItemData->m_nID);
			_VLA(L"mesh_name", pItemData->m_strMeshName.c_str());
			_VLP;
			m_Logger.Log(L"경로(mesh_path) 내용이 없습니다.\n");
		}
	}

	return bResult;
}