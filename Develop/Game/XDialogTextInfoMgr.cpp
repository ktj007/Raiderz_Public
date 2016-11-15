#include "stdafx.h"
#include "XDialogTextInfoMgr.h"
#include "MLocale.h"
#include "CSDef_Dialog.h"

XDialogTextInfoMgr::XDialogTextInfoMgr()
: m_nNowTextID(0)
{

}

void XDialogTextInfoMgr::Clear()
{
	m_mapText.clear();
	m_nNowTextID = 0;
}

#include "XStrings.h"

const wchar_t* XDialogTextInfoMgr::Get( int nTextID )
{
	map<int, wstring>::iterator it = m_mapText.find(nTextID);
	if (m_mapText.end() == it) return NULL;

	return XStringResMgr::GetInstance()->GetString(it->second);

	//return it->second.c_str();
}

bool XDialogTextInfoMgr::Load( const wchar_t* szFileName )
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

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

bool XDialogTextInfoMgr::Reload( const wchar_t* szFileName )
{
	Clear();

	return Load(szFileName);
}


void XDialogTextInfoMgr::ParseDialog( MXmlElement* pElement, MXml* pXml )
{
	int nDialogID = 0;
	_Attribute(&nDialogID,	pElement, DIALOG_XML_ATTR_ID);

	wstring strText = CSFormatString::Format(_T("DIALOG_TEXT_{0}"), FSParam(nDialogID));	// 문자열테이블 키
	//_Attribute(strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);
	InsertNewText(MakeNewTextID(), strText);

	MXmlElement* pChild = pElement->FirstChildElement();
	if (NULL != pChild)
	{
		if (0 == _stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
			ParseSay(pChild, pXml);
	}
}

void XDialogTextInfoMgr::ParseSay( MXmlElement* pElement, MXml* pXml )
{
	wstring strText;
	_Attribute(strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);
	InsertNewText(MakeNewTextID(), strText);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SELECT))
			ParseSelect(pChild, pXml);
	}
}

void XDialogTextInfoMgr::ParseSelect( MXmlElement* pElement, MXml* pXml )
{
	wstring strText;
	_Attribute(strText, pElement, DIALOG_XML_ATTR_TEXT, pXml);
	InsertNewText(MakeNewTextID(), strText);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), DIALOG_XML_TAG_SAY))
			ParseSay(pChild, pXml);
	}
}

void XDialogTextInfoMgr::InsertNewText( int nTextID, wstring strText )
{
	m_mapText.insert(map<int, wstring>::value_type(nTextID, strText));
}

int XDialogTextInfoMgr::MakeNewTextID()
{
	m_nNowTextID++;

	return m_nNowTextID;
}

