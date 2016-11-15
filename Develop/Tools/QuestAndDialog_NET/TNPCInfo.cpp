#include "stdafx.h"
#include "TNPCInfo.h"
#include "TDialogInfo.h"

#define NPC_XML_TAG_MAIET			"maiet"
#define NPC_XML_TAG_NPC				"NPC"
#define NPC_XML_TAG_COLT			"COLT"
#define NPC_XML_TAG_COMBAT			"COMBAT"
#define NPC_XML_TAG_CHECK			"CHECK"
#define NPC_XML_TAG_DEFAULT			"DEFAULT"
#define NPC_XML_TAG_ACTION			"ACTION"

#define NPC_XML_ATTR_ID				"id"
#define NPC_XML_ATTR_PARENT			"parent"
#define NPC_XML_ATTR_NAME			"Name"
#define NPC_XML_ATTR_DESC			"Desc"
#define NPC_XML_ATTR_MAX_HP			"MaxHP"
#define NPC_XML_ATTR_MAX_EN			"MaxEN"
#define NPC_XML_ATTR_MAX_STA		"MaxSTA"
#define NPC_XML_ATTR_AP				"AP"
#define NPC_XML_ATTR_RADIUS			"Radius"
#define NPC_XML_ATTR_SPEED			"Speed"
#define NPC_XML_ATTR_RUN_SPEED		"RunSpeed"
#define NPC_XML_ATTR_VALUE			"value"
#define NPC_XML_ATTR_TYPE			"type"
#define NPC_XML_ATTR_STATE			"state"
#define NPC_XML_ATTR_ATTACKABLE		"Attackable"
#define NPC_XML_ATTR_COUNTERATTACK	"CounterAttack"
#define NPC_XML_ATTR_AA				"AA"
#define NPC_XML_ATTR_SIGHTRANGE		"SightRange"
#define NPC_XML_ATTR_SIGHTANGLE		"SightAngle"
#define NPC_XML_ATTR_EARSHOT		"Earshot"
#define NPC_XML_ATTR_ROOTED			"Rooted"
#define NPC_XML_ATTR_GUARD_RATE		"guard_rate"
#define NPC_XML_ATTR_TACKLE			"tackle"
#define NPC_XML_ATTR_TACKLE_RATE	"tackle_rate"
#define NPC_XML_ATTR_RATE			"rate"
#define NPC_XML_ATTR_MAXCOUNT		"max_count"
#define NPC_XML_ATTR_AUTORUN		"auto_run"
#define NPC_XML_ATTR_PARAM1			"param1"
#define NPC_XML_ATTR_PARAM2			"param2"
#define NPC_XML_ATTR_PARAM3			"param3"
#define NPC_XML_ATTR_PARAM4			"param4"
#define NPC_XML_ATTR_OCCUPATION		"Occupation"

#define NPC_XML_VALUE_TYPE_TALENT	"talent"
#define NPC_XML_VALUE_TYPE_LUA		"lua"
#define NPC_XML_VALUE_TYPE_HP		"hp"
#define NPC_XML_VALUE_TYPE_EN		"en"
#define NPC_XML_VALUE_TYPE_SP		"sp"

void TNPCInfo::LoadDialog()
{
	LoadOnScreenDialog();
	LoadOffScreenDialog();
}

void TNPCInfo::LoadOnScreenDialog()
{
	MXml xml;

	char szID[512];
	sprintf_s(szID, "%05d", nID);

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = nullptr;

	string strPath;
	strPath += PATH_GAMESERVER;
	strPath += PATH_DIALOG;
	strPath += szID;
	strPath += FILENAME_DIALOG_ONSCREEN;

	if (xml.LoadFile(strPath.c_str()))
	{
		pElement = docHandle.FirstChild(DIALOG_XML_TAG_MAIET).FirstChildElement().Element();

		for( pElement; pElement != nullptr; pElement=pElement->NextSiblingElement() )
		{
			if (!_stricmp(pElement->Value(), DIALOG_XML_TAG_DIALOG))
			{
				ParseDialog(pElement, &xml, m_mapOnScreenDialog);
			}
		}
	}
}

void TNPCInfo::LoadOffScreenDialog()
{
	MXml xml;

	char szID[512];
	sprintf_s(szID, "%05d", nID);

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = nullptr;

	string strPath;
	strPath += PATH_GAMESERVER;
	strPath += PATH_DIALOG;
	strPath += szID;
	strPath += FILENAME_DIALOG_OFFSCREEN;

	if (xml.LoadFile(strPath.c_str()))
	{
		pElement = docHandle.FirstChild(DIALOG_XML_TAG_MAIET).FirstChildElement().Element();

		for( pElement; pElement != nullptr; pElement=pElement->NextSiblingElement() )
		{
			if (!_stricmp(pElement->Value(), DIALOG_XML_TAG_DIALOG))
			{
				ParseDialog(pElement, &xml, m_mapOffScreenDialog);
			}
			else _ASSERT(0);
		}
	}
}


void TNPCInfo::ParseDialog(MXmlElement* pElement, MXml* pXml, DIALOGINFOMAP& dialogInfoMap)
{
	TDialogInfo* pNewDialogInfo = nullptr;
	bool bNewDialog = false;
	int nDailogID = -1;

	_Attribute(&nDailogID, pElement, DIALOG_XML_ATTR_ID);

	DIALOGINFOMAP::iterator itor = dialogInfoMap.find(nDailogID);
	if (dialogInfoMap.end() == itor)
	{
		bNewDialog = true;
		pNewDialogInfo = new TDialogInfo();
	}
	else
	{
		pNewDialogInfo = (*itor).second;
	}

	pNewDialogInfo->m_vecpChildNode.clear();

	pNewDialogInfo->m_pParentNode = nullptr;
	pNewDialogInfo->m_nType = DNT_INFO;
	_Attribute(&pNewDialogInfo->m_nID, pElement, DIALOG_XML_ATTR_ID);

	if (bNewDialog) dialogInfoMap.insert(DIALOGINFOMAP::value_type(pNewDialogInfo->m_nID, pNewDialogInfo));

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != nullptr; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CHECK))
			ParseCheck(pChild, pXml, pNewDialogInfo, pNewDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
			ParseSay(pChild, pXml, pNewDialogInfo, pNewDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SELECT))
			ParseSelect(pChild, pXml, pNewDialogInfo, pNewDialogInfo);
		else _ASSERT(0);
	}
}

void TNPCInfo::ParseCheck(MXmlElement* pElement, MXml* pXml, TDialogNode* pDialogNode, TDialogInfo* pDialogInfo)
{
	TDialogCheckNode* pDialogCheckNode = new TDialogCheckNode;
	pDialogCheckNode->Clear();

	string strValue;
	if (_Attribute(strValue, pElement, DIALOG_XML_ATTR_TYPE, pXml))
	{
		if (strValue == DIALOG_XML_VALUE_TYPE_QUEST)
		{
			pDialogCheckNode->m_nCheckType = DCT_QUEST;
			pDialogCheckNode->m_nParam1 = -1;
			pDialogCheckNode->m_nParam2 = -1;
		}
		else if (strValue == DIALOG_XML_VALUE_TYPE_FACTION)
		{
			pDialogCheckNode->m_nCheckType = DCT_FACTION;
			pDialogCheckNode->m_nParam1 = INT_MIN;
			pDialogCheckNode->m_nParam2 = INT_MAX;
		}
		else _ASSERT(0);
	}

	pDialogCheckNode->m_pParentNode = pDialogNode;
	pDialogCheckNode->m_nType = DNT_CHECK;

	_Attribute(&pDialogCheckNode->m_nParam1, pElement, DIALOG_XML_ATTR_PARAM1);
	if (-1 == pDialogCheckNode->m_nParam1) _ASSERT(0);
	_Attribute(&pDialogCheckNode->m_nParam2, pElement, DIALOG_XML_ATTR_PARAM2);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != nullptr; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CASE_CHECKFAIL))
			ParseCase(pChild, pXml, pDialogCheckNode, DNT_CASE_CHECKFAIL, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CASE_NOEXIST))
			ParseCase(pChild, pXml, pDialogCheckNode, DNT_CASE_NOEXIST, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CASE_DOING))
			ParseCase(pChild, pXml, pDialogCheckNode, DNT_CASE_DOING, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CASE_FINAL))
			ParseCase(pChild, pXml, pDialogCheckNode, DNT_CASE_FINAL, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CASE_COMPLETED))
			ParseCase(pChild, pXml, pDialogCheckNode, DNT_CASE_COMPLETED, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CASE_DEFAULT))
			ParseCase(pChild, pXml, pDialogCheckNode, DNT_CASE_DEFAULT, pDialogInfo);
		else _ASSERT(0);
	}

	pDialogNode->m_vecpChildNode.push_back(pDialogCheckNode);
}

void TNPCInfo::ParseCase(MXmlElement* pElement, MXml* pXml, TDialogNode* pDialogNode, int nDialogNodeType, TDialogInfo* pDialogInfo)
{
	TDialogCaseNode* pDialogCaseNode = new TDialogCaseNode;
	pDialogCaseNode->Clear();

	pDialogCaseNode->m_pParentNode = pDialogNode;
	pDialogCaseNode->m_nType = nDialogNodeType;

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != nullptr; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CHECK))
			ParseCheck(pChild, pXml, pDialogCaseNode, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
			ParseSay(pChild, pXml, pDialogCaseNode, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SELECT))
			ParseSelect(pChild, pXml, pDialogCaseNode, pDialogInfo);
		else _ASSERT(0);
	}

	pDialogNode->m_vecpChildNode.push_back(pDialogCaseNode);
}

void TNPCInfo::ParseSay(MXmlElement* pElement, MXml* pXml, TDialogNode* pDialogNode, TDialogInfo* pDialogInfo)
{
	TDialogSayNode* pDialogSayNode = new TDialogSayNode;
	pDialogSayNode->Clear();

	pDialogSayNode->m_pParentNode = pDialogNode;
	pDialogSayNode->m_nType = DNT_SAY;

	_Attribute(pDialogSayNode->m_strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != nullptr; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CHECK))
			ParseCheck(pChild, pXml, pDialogSayNode, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SELECT))
			ParseSelect(pChild, pXml, pDialogSayNode, pDialogInfo);
		else _ASSERT(0);
	}

	pDialogNode->m_vecpChildNode.push_back(pDialogSayNode);
}


void TNPCInfo::ParseSelect(MXmlElement* pElement, MXml* pXml, TDialogNode* pDialogNode, TDialogInfo* pDialogInfo)
{
	TDialogSelectNode* pDialogSelectNode = new TDialogSelectNode;
	pDialogSelectNode->Clear();

	pDialogSelectNode->m_pParentNode = pDialogNode;
	pDialogSelectNode->m_nType = DNT_SELECT;

	_Attribute(pDialogSelectNode->m_strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);
	_Attribute(&pDialogSelectNode->m_nExit, pElement, DIALOG_XML_ATTR_EXIT);

	pDialogInfo->m_vecChildSelectNodeExit.push_back(pDialogSelectNode->m_nExit);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != nullptr; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_CHECK))
			ParseCheck(pChild, pXml, pDialogSelectNode, pDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
			ParseSay(pChild, pXml, pDialogSelectNode, pDialogInfo);
		else _ASSERT(0);
	}

	pDialogNode->m_vecpChildNode.push_back(pDialogSelectNode);
}


TDialogInfo* TNPCInfo::FindOnScreenDialogNode(int nDialogID)
{
	DIALOGINFOMAP::iterator itor = m_mapOnScreenDialog.find(nDialogID);

	if(m_mapOnScreenDialog.end() != itor)
	{
		return (*itor).second;
	}

	return nullptr;
}

TDialogInfo* TNPCInfo::FindOffScreenDialogNode(int nDialogID)
{
	DIALOGINFOMAP::iterator itor = m_mapOffScreenDialog.find(nDialogID);

	if(m_mapOffScreenDialog.end() != itor)
	{
		return (*itor).second;
	}

	return nullptr;
}

void TNPCInfo::SaveDialogToXml(MXmlElement* pXeStringsOnScreen_Maiet, MXmlElement* pXeStringsOffScreen_Maiet)
{
	SaveOnScreenDialogInfoToXml(pXeStringsOnScreen_Maiet);
	SaveOffScreenDialogInfoToXml(pXeStringsOffScreen_Maiet);
}

void TNPCInfo::SaveOnScreenDialogInfoToXml(MXmlElement* pXeStringsOnScreen_Maiet)
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement* pElementMaietDialogInfo = new MXmlElement(DIALOG_XML_TAG_MAIET);
	xml.Doc()->LinkEndChild(pElementMaietDialogInfo);

	if(false == m_mapOnScreenDialog.empty())
	{
		for (DIALOGINFOMAP::iterator itorMapOnScreenDialog = m_mapOnScreenDialog.begin(); m_mapOnScreenDialog.end() != itorMapOnScreenDialog; ++itorMapOnScreenDialog)
		{
			TDialogInfo* pDialogInfo = (*itorMapOnScreenDialog).second;
			if(nullptr == pDialogInfo) continue;

			int nCntSay = 1;
			int nCntSelect = 1;

			pDialogInfo->SaveToXml(pElementMaietDialogInfo, pXeStringsOnScreen_Maiet, nID, 0, nCntSay, nCntSelect);
		}

		char szID[512];
		sprintf_s(szID, "%05d", nID);

		string strPath;
		strPath = PATH_GAMESERVER;
		strPath += PATH_DIALOG;
		strPath += szID;
		strPath += FILENAME_DIALOG_ONSCREEN;

		xml.Doc()->SaveFile(strPath);
	}	
}

void TNPCInfo::SaveOffScreenDialogInfoToXml(MXmlElement* pXeStringsOffScreen_Maiet)
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement* pElementMaietDialogInfo = new MXmlElement(DIALOG_XML_TAG_MAIET);
	xml.Doc()->LinkEndChild(pElementMaietDialogInfo);

	if(false == m_mapOffScreenDialog.empty())
	{
		for (DIALOGINFOMAP::iterator itorMapOffScreenDialog = m_mapOffScreenDialog.begin(); m_mapOffScreenDialog.end() != itorMapOffScreenDialog; ++itorMapOffScreenDialog)
		{
			TDialogInfo* pDialogInfo = (*itorMapOffScreenDialog).second;
			if(nullptr == pDialogInfo) continue;

			int nCntSay = 1;
			int nCntSelect = 1;

			pDialogInfo->SaveToXml(pElementMaietDialogInfo, pXeStringsOffScreen_Maiet, nID, 0, nCntSay, nCntSelect);
		}

		char szID[512];
		sprintf_s(szID, "%05d", nID);

		string strPath;
		strPath = PATH_GAMESERVER;
		strPath += PATH_DIALOG;
		strPath += szID;
		strPath += FILENAME_DIALOG_OFFSCREEN;

		xml.Doc()->SaveFile(strPath);
	}
}



TNPCInfoMgr::TNPCInfoMgr()
{
}

TNPCInfoMgr::~TNPCInfoMgr()
{
	Clear();
}

bool TNPCInfoMgr::Load(const char* szFileName)
{
	MXml xml;

	if (!xml.LoadFile(szFileName)) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(NPC_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != nullptr; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), NPC_XML_TAG_NPC))
		{
			ParseNPC(pElement, &xml);
		}
	}

	return true;
}


TNPCInfo* TNPCInfoMgr::Get(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return nullptr;
	return (TNPCInfo*)((*itor).second);
}

bool TNPCInfoMgr::Exist(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return false;
	return true;
}

void TNPCInfoMgr::ParseNPC(MXmlElement* pElement, MXml* pXml)
{
	string strName;
	string strValue;
	int id = -1;
	TNPCInfo* pNewInfo = nullptr;
	bool new_npc = false;

	_Attribute(&id,									pElement, NPC_XML_ATTR_ID);

	pNewInfo = Get(id);

	if (pNewInfo == nullptr)
	{
		new_npc = true;
		pNewInfo = new TNPCInfo();
	}

	// parent가 있으면 parent의 NPC에서 복사한다.
	int parent_id = -1;
	if (_Attribute(&parent_id, pElement, NPC_XML_ATTR_PARENT))
	{
		TNPCInfo* pParentNPC = Get(parent_id);
		if (pParentNPC)
		{
			*pNewInfo = *pParentNPC;
		}
		else parent_id = -1;	// 없으면 다시 -1
	}

	pNewInfo->nID = id;
	_Attribute(pNewInfo->strName,					pElement, NPC_XML_ATTR_NAME);
	_Attribute(&pNewInfo->nMaxHP,					pElement, NPC_XML_ATTR_MAX_HP);
	_Attribute(&pNewInfo->nMaxEN,					pElement, NPC_XML_ATTR_MAX_EN);
	_Attribute(&pNewInfo->nMaxSTA,					pElement, NPC_XML_ATTR_MAX_STA);
	_Attribute(&pNewInfo->nAP,						pElement, NPC_XML_ATTR_AP);
	_Attribute(&pNewInfo->fRadius,					pElement, NPC_XML_ATTR_RADIUS);
	_Attribute(&pNewInfo->fSpeed,					pElement, NPC_XML_ATTR_SPEED);
	_Attribute(&pNewInfo->fRunSpeed,				pElement, NPC_XML_ATTR_RUN_SPEED);
	_Attribute(&pNewInfo->bAttackable,				pElement, NPC_XML_ATTR_ATTACKABLE);
	_Attribute(&pNewInfo->bAA,						pElement, NPC_XML_ATTR_AA);
	_Attribute(&pNewInfo->nOccupation,				pElement, NPC_XML_ATTR_OCCUPATION);

	char szBuff[128];
	sprintf_s(szBuff, "NPC_%05d", pNewInfo->nID);
	pNewInfo->strNameInLua = szBuff;

	char szCopyName[512];
	strcpy_s(szCopyName, 511, pNewInfo->strName.c_str());
	_strlwr(szCopyName);
	char szSep[] = " ";
	const char* szToken = strtok(szCopyName, szSep);

	while (nullptr != szToken)
	{
		TNPCINFO_MAP_BYNAME::iterator itor = m_mapNPCInfoByName.find(szToken);
		if (m_mapNPCInfoByName.end() == itor)
		{
			m_mapNPCInfoByName.insert(TNPCINFO_MAP_BYNAME::value_type(string(szToken), vector<TNPCInfo*>()));
			TNPCINFO_MAP_BYNAME::iterator insertedItor = m_mapNPCInfoByName.find(string(szToken));
			(*insertedItor).second.push_back(pNewInfo);
		}
		else
		{
			(*itor).second.push_back(pNewInfo);
		}

		szToken = strtok(nullptr, szSep);
	}

	if (new_npc) insert(value_type(pNewInfo->nID, pNewInfo));

	pNewInfo->LoadDialog();
}

void TNPCInfoMgr::DeleteDialog(int nNPCID, bool bOnScreen, int nDialogID)
{
	iterator itorTNPCInfoMgr = find(nNPCID);
	TNPCInfo* pNPCInfo = (*itorTNPCInfoMgr).second;
	if (pNPCInfo == nullptr) return;

	if (true == bOnScreen)
	{
		DIALOGINFOMAP::iterator itorMapOnScreenDialog = pNPCInfo->m_mapOnScreenDialog.find(nDialogID);
		if (pNPCInfo->m_mapOnScreenDialog.end() != itorMapOnScreenDialog)
		{
			delete (*itorMapOnScreenDialog).second;
			pNPCInfo->m_mapOnScreenDialog.erase(itorMapOnScreenDialog);
		}
	}
	else
	{
		DIALOGINFOMAP::iterator itorMapOffScreenDialog = pNPCInfo->m_mapOffScreenDialog.find(nDialogID);
		if (pNPCInfo->m_mapOffScreenDialog.end() != itorMapOffScreenDialog)
		{
			delete (*itorMapOffScreenDialog).second;
			pNPCInfo->m_mapOffScreenDialog.erase(itorMapOffScreenDialog);
		}
	}
}

void TNPCInfoMgr::DeleteDialogNode(TDialogNode* pDialogNode)
{
	TDialogNode* pParentDialogNode = pDialogNode->m_pParentNode;

	for (vector<TDialogNode*>::iterator itor = pParentDialogNode->m_vecpChildNode.begin(); pParentDialogNode->m_vecpChildNode.end() != itor; ++itor)
	{				 
		if (pDialogNode == (*itor))
		{
			(*itor)->Clear();
			delete (*itor);
			pParentDialogNode->m_vecpChildNode.erase(itor);
			break;
		}
	}

	return;
}


void TNPCInfoMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		TNPCInfo* pTNPCInfo = static_cast<TNPCInfo*>((*itor).second);
		pTNPCInfo->Clear();
		delete pTNPCInfo;
	}

	m_mapNPCInfoByName.clear();

	clear();
}

void TNPCInfoMgr::FindNPCByName(const char* szNPCName, vector<int>& vecFindedNPC)
{
	char szCopyNPCName[512];
	strcpy_s(szCopyNPCName, 511, szNPCName);
	_strlwr(szCopyNPCName);
	char szSep[] = " ";
	strcat_s(szCopyNPCName, 511, szSep);

	const char* szToken = strtok(szCopyNPCName, szSep);
	while (nullptr != szToken)
	{
		TNPCINFO_MAP_BYNAME::iterator mapNPCInfoByNameItor = m_mapNPCInfoByName.find(szToken);
		if (m_mapNPCInfoByName.end() == mapNPCInfoByNameItor)
		{
			vecFindedNPC.clear();
			break;
		}
		else
		{
			if (true == vecFindedNPC.empty())
			{
				int nVecTNPCInfoSize = (*mapNPCInfoByNameItor).second.size();
				for (int j = 0; nVecTNPCInfoSize > j; ++j)
				{
					vecFindedNPC.push_back((*mapNPCInfoByNameItor).second[j]->nID);
				}

			}
			else
			{
				for (vector<int>::iterator vecFindedNPCItor = vecFindedNPC.begin(); vecFindedNPC.end() != vecFindedNPCItor; ++vecFindedNPCItor)
				{
					bool bFind = false;
					for(vector<TNPCInfo*>::iterator vecTNPCInfoItor = (*mapNPCInfoByNameItor).second.begin(); (*mapNPCInfoByNameItor).second.end() !=  vecTNPCInfoItor; ++vecTNPCInfoItor) 
					{
						int pOutResult = (*vecFindedNPCItor);
						TNPCInfo * pIn = (*vecTNPCInfoItor);
						if (pOutResult == pIn->nID)
						{
							bFind = true;
							break;
						}
					}

					if (false == bFind)
					{
						vecFindedNPCItor = vecFindedNPC.erase(vecFindedNPCItor);
						if (vecFindedNPC.end() == vecFindedNPCItor)
							break;
					}
				}
			}
		}

		szToken = strtok(nullptr, szSep);
	}
}

bool TNPCInfoMgr::AddDialog(int nNPCID, bool bOnScreen, int nDialogID)
{
	TNPCInfo* pNPCInfo = nullptr;
	pNPCInfo = Get(nNPCID);

	if (pNPCInfo == nullptr) return false;

	if (true == bOnScreen)
	{
		DIALOGINFOMAP::iterator itor = pNPCInfo->m_mapOnScreenDialog.find(nDialogID);
		if (pNPCInfo->m_mapOnScreenDialog.end() !=itor) return false;

		TDialogInfo* pDialogInfo = new TDialogInfo();
		pDialogInfo->m_nID = nDialogID;
		pDialogInfo->m_nType = DNT_INFO;
		pNPCInfo->m_mapOnScreenDialog.insert(DIALOGINFOMAP::value_type(nDialogID, pDialogInfo));
	}
	else
	{
		DIALOGINFOMAP::iterator itor = pNPCInfo->m_mapOffScreenDialog.find(nDialogID);
		if (pNPCInfo->m_mapOffScreenDialog.end() !=itor) return false;

		TDialogInfo* pDialogInfo = new TDialogInfo();
		pDialogInfo->m_nID = nDialogID;
		pDialogInfo->m_nType = DNT_INFO;
		pNPCInfo->m_mapOffScreenDialog.insert(DIALOGINFOMAP::value_type(nDialogID, pDialogInfo));
	}

	return true;
}

void TNPCInfoMgr::SaveDialogToXml()
{
	MXml xStringsOnScreen;
	MXml xStringsOffScreen;;

	MXmlDeclaration* pXdStringsOnScreen = xStringsOnScreen.CreateDeclaration("1.0", "euc-kr", "");
	xStringsOnScreen.Doc()->LinkEndChild(pXdStringsOnScreen);
	MXmlElement* pXeStringsOnScreen_Maiet = new MXmlElement("maiet");
	xStringsOnScreen.Doc()->LinkEndChild(pXeStringsOnScreen_Maiet);

	MXmlDeclaration* pXdStringsOffScreen = xStringsOffScreen.CreateDeclaration("1.0", "euc-kr", "");
	xStringsOffScreen.Doc()->LinkEndChild(pXdStringsOffScreen);
	MXmlElement* pXeStringsOffScreen_Maiet = new MXmlElement("maiet");
	xStringsOffScreen.Doc()->LinkEndChild(pXeStringsOffScreen_Maiet);

	for(iterator itor = begin(); end() != itor; ++itor)
	{
		TNPCInfo* pNPCInfo = (*itor).second;
		if (nullptr == pNPCInfo) continue;

		pNPCInfo->SaveDialogToXml(pXeStringsOnScreen_Maiet, pXeStringsOffScreen_Maiet);
	}

	string strPath;
	strPath = PATH_GAMECLIENT;
	strPath += PATH_STRINGS;
	strPath += FILENAME_STRINGS_ONSCREEN;
	xStringsOnScreen.Doc()->SaveFile(strPath);

	strPath = PATH_GAMESERVER;
	strPath += PATH_STRINGS;
	strPath += FILENAME_STRINGS_ONSCREEN;
	xStringsOnScreen.Doc()->SaveFile(strPath);

	strPath = PATH_GAMECLIENT;
	strPath += PATH_STRINGS;
	strPath += FILENAME_STRINGS_OFFSCREEN;
	xStringsOffScreen.Doc()->SaveFile(strPath);

	strPath = PATH_GAMESERVER;
	strPath += PATH_STRINGS;
	strPath += FILENAME_STRINGS_OFFSCREEN;
	xStringsOffScreen.Doc()->SaveFile(strPath);
}