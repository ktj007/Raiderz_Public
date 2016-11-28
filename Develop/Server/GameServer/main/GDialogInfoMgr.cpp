#include "StdAfx.h"
#include "GDialogInfoMgr.h"
#include "GDialogInfo.h"
#include "GValidateLogger.h"
#include "GSTLUtil.h"
#include "GGlobal.h"
#include "GItemAddRemoveInfo.h"
#include "GItemAddRemoveInfoParser.h"
#include "CSDef_Dialog.h"

GDialogInfoMgr::GDialogInfoMgr()
: m_nNowTextID(0)
{
}

GDialogInfoMgr::~GDialogInfoMgr()
{
	Clear();
}

void GDialogInfoMgr::Clear()
{
	m_nNowTextID = 0;

	for_each(m_mapDialogInfo.begin(), m_mapDialogInfo.end(), DPSO());
	m_mapDialogInfo.clear();	
}

bool GDialogInfoMgr::Load(const wchar_t* szFileName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	_VLI(szFileName);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(DIALOG_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), DIALOG_XML_TAG_DIALOG))
		{
			ParseDialog(pElement, &xml);
		}
	}

	return true;
}

void GDialogInfoMgr::ParseDialog(MXmlElement* pElement, MXml* pXml)
{
	int nDialogID;
	_Attribute(&nDialogID, pElement, DIALOG_XML_ATTR_ID);

	_VLE(_T(DIALOG_XML_TAG_DIALOG));	
	_VLA(_T(DIALOG_XML_ATTR_ID), nDialogID);
	_VLP;

	GDialogInfo* pExistInfo = Get(nDialogID);
	if (NULL != pExistInfo)
	{		
		_VLOGGER->Log(_T(DIALOG_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}	

	GDialogInfo* pNewDialogInfo = new GDialogInfo();
	pNewDialogInfo->m_nID = nDialogID;
	pNewDialogInfo->m_nTextID = MakeNewTextID();
	
	pNewDialogInfo->m_strText = CSFormatString::Format(L"$$DIALOG_TEXT_{0}", FSParam(nDialogID));
	
	_Attribute(&pNewDialogInfo->m_nExit, pElement, DIALOG_XML_ATTR_EXIT);	

	MXmlElement* pChild = pElement->FirstChildElement();
	if (NULL != pChild)
	{
		if (0 == _stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
		{
			ParseSay(pChild, pXml, pNewDialogInfo);
		}
		else
		{
			_ASSERT(0);
		}
	}

	Insert(pNewDialogInfo);
}

void GDialogInfoMgr::ParseSay(MXmlElement* pElement, MXml* pXml, GDialogInfo* pDialogInfo)
{
	GDialogSayInfo* pDialogSayInfo = new GDialogSayInfo;
	pDialogSayInfo->m_nTextID = MakeNewTextID();
	_Attribute(pDialogSayInfo->m_strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);

	_VLE(_T(DIALOG_XML_TAG_SAY));
	_VLA(_T(DIALOG_XML_ATTR_TEXT), pDialogSayInfo->m_nTextID);
	_VLP;	

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SELECT))
			ParseSelect(pChild, pXml, pDialogSayInfo);
		else _ASSERT(0);
	}

	pDialogInfo->m_pDialogSayInfo = pDialogSayInfo;
}

void GDialogInfoMgr::ParseSay(MXmlElement* pElement, MXml* pXml, GDialogSelectInfo* pDialogSelectInfo)
{
	GDialogSayInfo* pDialogSayInfo = new GDialogSayInfo;
	pDialogSayInfo->m_nTextID = MakeNewTextID();
	_Attribute(pDialogSayInfo->m_strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);

	_VLE(_T(DIALOG_XML_TAG_SAY));
	_VLA(_T(DIALOG_XML_ATTR_TEXT), pDialogSayInfo->m_nTextID);
	_VLP;

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SELECT))
			ParseSelect(pChild, pXml, pDialogSayInfo);
		else _ASSERT(0);
	}

	pDialogSelectInfo->m_pDialogSayInfo = pDialogSayInfo;
}

void GDialogInfoMgr::ParseSelect(MXmlElement* pElement, MXml* pXml, GDialogSayInfo* pDialogSayInfo)
{
	GDialogSelectInfo* pDialogSelectInfo = new GDialogSelectInfo;
	pDialogSelectInfo->m_nTextID = MakeNewTextID();
	_Attribute(pDialogSelectInfo->m_strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);
	_Attribute(&pDialogSelectInfo->m_nExit, pElement, DIALOG_XML_ATTR_EXIT);

	_VLE(_T(DIALOG_XML_TAG_SELECT));
	_VLA(_T(DIALOG_XML_ATTR_TEXT), pDialogSelectInfo->m_nTextID);
	_VLA(_T(DIALOG_XML_ATTR_EXIT), pDialogSelectInfo->m_nExit);
	_VLP;

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
			ParseSay(pChild, pXml, pDialogSelectInfo);
		else if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_ITEM))
			ParseItem(pChild, pXml, pDialogSelectInfo);
		else _ASSERT(0);
	}

	pDialogSayInfo->m_vecDialogSelectInfo.push_back(pDialogSelectInfo);
}

void GDialogInfoMgr::ParseItem(MXmlElement* pElement, MXml* pXml, GDialogSelectInfo* pDialogSelectInfo)
{
	if (0 == pDialogSelectInfo->m_nExit) return;

	pDialogSelectInfo->m_pItemAddRemoveInfo = new GItemAddRemoveInfo;
	
	GItemAddRemoveInfoParser parser;
	parser.ParseItem(pElement, pXml, pDialogSelectInfo->m_pItemAddRemoveInfo);
}

void GDialogInfoMgr::Insert( GDialogInfo* pDialogInfo )
{	
	if (NULL == pDialogInfo)
	{
		_ASSERT(0);
		return;
	}

	if (NULL != Get(pDialogInfo->m_nID))
	{
		_ASSERT(0);
		return;
	}

	m_mapDialogInfo.insert(MAP_DIALOGINFO::value_type(pDialogInfo->m_nID, pDialogInfo));
}

GDialogInfo* GDialogInfoMgr::Get(int nID)
{
	if (true == m_mapDialogInfo.empty()) return NULL;

	MAP_DIALOGINFO::iterator itor = m_mapDialogInfo.find(nID);
	if (m_mapDialogInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool GDialogInfoMgr::IsExist(int nDialogID)
{
	if (NULL == Get(nDialogID)) return false;

	return true;
}

int GDialogInfoMgr::MakeNewTextID()
{
	m_nNowTextID++;

	return m_nNowTextID;
}