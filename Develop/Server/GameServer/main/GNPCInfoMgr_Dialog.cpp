#include "stdafx.h"
#include "GNPCInfoMgr.h"

#define DIALOG_XML_TAG_MAIET					"maiet"

#define DIALOG_XML_TAG_DIALOG					"DIALOG"
#define DIALOG_XML_TAG_SAY						"SAY"
#define DIALOG_XML_TAG_SELECT					"SELECT"

#define DIALOG_XML_ATTR_ID						"ID"
#define DIALOG_XML_ATTR_TEXT					"TEXT"
#define DIALOG_XML_ATTR_EXIT					"EXIT"

void GNPCInfoMgr::LoadDialog(GNPCInfo* pNPCInfo)
{
	char szID[512];
	sprintf_s(szID, "%05d", pNPCInfo->nID);

	MXml xml;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pChild = NULL;

	if (xml.LoadFile(FILENAME_DIALOG))
	{
		pChild = docHandle.FirstChild(DIALOG_XML_TAG_MAIET).FirstChildElement().Element();

		for( pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
		{
			if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_DIALOG))
			{
				ParseDialog(pChild, &xml, pNPCInfo->m_mapDialog);
			}
			else _ASSERT(0);
		}
	}
}

void GNPCInfoMgr::ParseDialog(MXmlElement* pElement, MXml* pXml, DIALOGINFOMAP& dialogInfoMap)
{
	GDialogNode* pNewDialogInfo = NULL;
	bool bNewDialog = false;
	int nDailogID = -1;

	_Attribute(&nDailogID, pElement, DIALOG_XML_ATTR_ID);

	DIALOGINFOMAP::iterator itor = dialogInfoMap.find(nDailogID);
	if (dialogInfoMap.end() == itor)
	{
		bNewDialog = true;
		pNewDialogInfo = new GDialogNode();
	}
	else
	{
		pNewDialogInfo = (*itor).second;
	}

	pNewDialogInfo->m_vecpChildNode.clear();

	pNewDialogInfo->m_pParentNode = NULL;
	pNewDialogInfo->m_nType = DNT_INFO;
	_Attribute(&pNewDialogInfo->m_nID, pElement, DIALOG_XML_ATTR_ID);
	_Attribute(pNewDialogInfo->m_strText, pElement, DIALOG_XML_ATTR_TEXT);
	_Attribute(&pNewDialogInfo->m_nExit, pElement, DIALOG_XML_ATTR_EXIT);	

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
			ParseSay(pChild, pXml, pNewDialogInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SELECT))
			ParseSelect(pChild, pXml, pNewDialogInfo);
		else _ASSERT(0);
	}

	if (bNewDialog) dialogInfoMap.insert(DIALOGINFOMAP::value_type(pNewDialogInfo->m_nID, pNewDialogInfo));
}

void GNPCInfoMgr::ParseSay(MXmlElement* pElement, MXml* pXml, GBaseDialogNode* pDialogNode)
{
	GDialogSayNode* pDialogSayNode = new GDialogSayNode;
	pDialogSayNode->Clear();

	pDialogSayNode->m_pParentNode = pDialogNode;
	pDialogSayNode->m_nType = DNT_SAY;

	_Attribute(pDialogSayNode->m_strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SELECT))
			ParseSelect(pChild, pXml, pDialogSayNode);
		else _ASSERT(0);
	}

	pDialogNode->m_vecpChildNode.push_back(pDialogSayNode);
}


void GNPCInfoMgr::ParseSelect(MXmlElement* pElement, MXml* pXml, GBaseDialogNode* pDialogNode)
{
	GDialogSelectNode* pDialogSelectNode = new GDialogSelectNode;
	pDialogSelectNode->Clear();

	pDialogSelectNode->m_pParentNode = pDialogNode;
	pDialogSelectNode->m_nType = DNT_SELECT;

	_Attribute(pDialogSelectNode->m_strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);
	_Attribute(&pDialogSelectNode->m_nExit, pElement, DIALOG_XML_ATTR_EXIT);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
			ParseSay(pChild, pXml, pDialogSelectNode);
		else _ASSERT(0);
	}

	pDialogNode->m_vecpChildNode.push_back(pDialogSelectNode);
}

