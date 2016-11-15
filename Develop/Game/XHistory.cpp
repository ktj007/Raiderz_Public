#include "stdafx.h"
#include "XHistory.h"
#include "MLocale.h"

#define HISTORY_XML_TAG_HISTORY		"History"

bool XHistory::Load( const wchar_t* szFileName )
{
	global.filesys->SetAllowAccessUncompFile( true);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str()))
	{
		global.filesys->SetAllowAccessUncompFile( false);
		return false;
	}

	global.filesys->SetAllowAccessUncompFile( false);

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(HISTORY_XML_TAG_HISTORY).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		wstring strKey = MLocale::ConvAnsiToUTF16(pElement->Value());
		
		string strValue;
		_Contents(strValue, pElement, "", &xml);

		m_Values.insert(map<wstring, wstring>::value_type(strKey, MLocale::ConvAnsiToUTF16(strValue.c_str())));
	}

	return true;
}

bool XHistory::Save( const wchar_t* szFileName )
{
	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement* pRootElement = new MXmlElement(HISTORY_XML_TAG_HISTORY);
	xml.Doc()->LinkEndChild(pRootElement);

	for (map<wstring, wstring>::iterator itor = m_Values.begin(); itor != m_Values.end(); ++itor)
	{
		wstring strKey = (*itor).first;
		wstring strValue = (*itor).second;

		MXmlElement* pElement = new MXmlElement(MLocale::ConvUTF16ToAnsi(strKey.c_str()));
		pRootElement->LinkEndChild(pElement);

		std::string strUTF8Value = MLocale::ConvUTF16ToUTF8( strValue.c_str() );
		_SetContents(pElement, strUTF8Value.c_str() );
	}

	return xml.SaveFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str());
}

wstring XHistory::GetValue( const wchar_t* szKey )
{
	map<wstring, wstring>::iterator itor = m_Values.find(wstring(szKey));
	if (itor == m_Values.end()) return L"";
	return (*itor).second;
}

int XHistory::GetValueNum( const wchar_t* szKey )
{
	wstring strValue = GetValue(szKey);
	int ret = _wtoi(strValue.c_str());
	return ret;
}

void XHistory::SetValue( const wchar_t* szKey, const wchar_t* szValue )
{
	map<wstring, wstring>::iterator itor = m_Values.find(wstring(szKey));
	if (itor == m_Values.end())
	{
		m_Values.insert(map<wstring, wstring>::value_type(wstring(szKey), wstring(szValue)));
	}
	else
	{
		(*itor).second = wstring(szValue);
	}

}

void XHistory::SetValue( const wchar_t* szKey, int nValue )
{
	wchar_t szValue[64];
	_itow_s(nValue, szValue, 10);

	map<wstring, wstring>::iterator itor = m_Values.find(wstring(szKey));
	if (itor == m_Values.end())
	{
		m_Values.insert(map<wstring, wstring>::value_type(wstring(szKey), wstring(szValue)));
	}
	else
	{
		(*itor).second = wstring(szValue);
	}
}