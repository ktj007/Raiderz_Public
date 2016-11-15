#include "StdAfx.h"
#include "XConfig.h"


/** class XOptionUnit **/
XOptionUnit::XOptionUnit()
{
}

XOptionUnit::XOptionUnit( const string& strValue)
{
	m_strValue = strValue;
}

void XOptionUnit::SetValue( const string& strValue)
{
	m_strValue = strValue;
}

string XOptionUnit::GetValue()
{
	return m_strValue;
}

void XOptionUnit::SetAttribute( const string& strAttrName, const string& strAttrValue)
{
	map< string, string>::iterator itr = m_AttributeList.find( strAttrName);

	if ( itr == m_AttributeList.end())
		m_AttributeList.insert( map< string, string>::value_type( strAttrName, strAttrValue));
	else
		(*itr).second = strAttrValue;
}

string XOptionUnit::GetAttribute( const string& strAttrName)
{
	map< string, string>::iterator itr = m_AttributeList.find( strAttrName);

	string str;

	if ( itr != m_AttributeList.end())
		str = (*itr).second;

	return str;
}






/** class XConfigOption **/
XConfigOption::XConfigOption()
{
}


XConfigOption::~XConfigOption()
{
	Clear();
}


void XConfigOption::Clear()
{
	m_OptionList.clear();
}


bool XConfigOption::LoadXmlFile( const wchar_t* szFileName)
{
	Clear();

	global.filesys->SetAllowAccessUncompFile( true);

	MXml xml;
	if ( xml.LoadFile( MLocale::ConvUTF16ToAnsi( szFileName).c_str()) == false)
	{
		global.filesys->SetAllowAccessUncompFile( false);
		return false;
	}

	global.filesys->SetAllowAccessUncompFile( false);

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pRoot = docHandle.FirstChild( "CONFIG").FirstChildElement().Element();
	if ( pRoot)
		_parse_xml( pRoot, &xml);

	return true;
}


bool XConfigOption::_parse_xml( MXmlElement* pParent, MXml* pXml, const char* szName)
{
	for ( MXmlElement* pChild = pParent;  pChild != NULL;  pChild = pChild->NextSiblingElement())
	{
		string strName = szName;
		if ( strName.empty() == false)		strName += ",";
		strName += pChild->ValueStr();

		string strValue;
		_Contents( strValue, pChild, pXml);

		XOptionUnit option;
		option.SetValue( strValue);


		for ( MXmlAttribute* pAttr = pChild->FirstAttribute();  pAttr != NULL;  pAttr = pAttr->Next())
			option.SetAttribute( pAttr->Name(), pAttr->Value());

		m_OptionList.insert( map< string, XOptionUnit>::value_type( strName, option));

		_parse_xml( pChild->FirstChildElement(), pXml, strName.c_str());
	}

	return true;
}


bool XConfigOption::SaveXmlFile( const wchar_t* szFileName)
{
	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild( pDec);

	MXmlElement *pRoot = new MXmlElement( "CONFIG");
	xml.LinkEndChild( pRoot);


	for ( map< string, XOptionUnit>::iterator itr = m_OptionList.begin();  itr != m_OptionList.end();  itr++)
	{
		string strName = (*itr).first;

		if ( strName.empty())
			continue;


		MXmlElement *pCurParent = pRoot;

		while ( strName.empty() == false)
		{
			int token = strName.find( ',', 0);

			if ( token == string::npos)
			{
				XOptionUnit& option = (*itr).second;

				MXmlElement* pElement = new MXmlElement( strName);
				_SetContents( pElement, MLocale::ConvAnsiToUTF8(option.GetValue().c_str()));
				pCurParent->LinkEndChild( pElement);

				for ( map< string, string>::iterator itr = option.m_AttributeList.begin();  itr != option.m_AttributeList.end();  itr++)
				{
					if ( (*itr).second.empty())
						continue;

					pElement->SetAttribute( (*itr).first, (*itr).second);
				}

				break;
			}



			string strParent;
			strParent = strName.substr( 0, token);
			strName = strName.substr( token + 1);

			MXmlElement* pParent = pCurParent->FirstChildElement( strParent);
			if ( pParent == NULL)
			{
				MXmlElement* pElement = new MXmlElement( strParent);
				pCurParent->LinkEndChild( pElement);

				pCurParent = pElement;
			}
			else
				pCurParent = pParent;
		}
	}


	return xml.SaveFile( MLocale::ConvUTF16ToAnsi( szFileName).c_str());
}


void XConfigOption::Set( const char* szName, const string& strValue)
{
	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())
	{
		m_OptionList.insert( map< string, XOptionUnit>::value_type( szName, XOptionUnit( strValue)));
		return;
	}

	(*itr).second.SetValue( strValue);
}


void XConfigOption::Set( const char* szName, bool bValue)
{
	string str = ( bValue == true) ? "TRUE" : "FALSE";

	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())
	{
		m_OptionList.insert( map< string, XOptionUnit>::value_type( szName, XOptionUnit( str)));
		return;
	}

	(*itr).second.SetValue( str);
}


void XConfigOption::Set( const char* szName, int nValue)
{
	char buff[ 256];
	sprintf_s( buff, "%d", nValue);

	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())
	{
		m_OptionList.insert( map< string, XOptionUnit>::value_type( szName, XOptionUnit( buff)));
		return;
	}

	(*itr).second.SetValue( buff);
}


void XConfigOption::Set( const char* szName, float fValue)
{
	char buff[ 256];
	sprintf_s( buff, "%.2f", fValue);

	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())
	{
		m_OptionList.insert( map< string, XOptionUnit>::value_type( szName, XOptionUnit( buff)));
		return;
	}

	(*itr).second.SetValue( buff);
}


string XConfigOption::Get( const char* szName, const string& strDefaultValue)
{
	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())		return strDefaultValue;

	return (*itr).second.GetValue();
}


bool XConfigOption::Get( const char* szName, bool bDefaultValue)
{
	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())		return bDefaultValue;

	string strVal = (*itr).second.GetValue();
	if ( strVal == "TRUE"  ||  strVal == "true")
		return true;

	return false;
}


int XConfigOption::Get( const char* szName, int nDefaultValue)
{
	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())		return nDefaultValue;

	return atoi( (*itr).second.GetValue().c_str());
}


float XConfigOption::Get( const char* szName, float fDefaultValue)
{
	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())		return fDefaultValue;

	return atof( (*itr).second.GetValue().c_str());
}


void XConfigOption::SetAttribute( const char* szName, const string& strAttrName, const string& strAttrValue)
{
	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())		return;

	(*itr).second.SetAttribute( strAttrName, strAttrValue);
}


string XConfigOption::GetAttribute( const char* szName, const string& strAttrName, const string& strDefaultValue)
{
	map< string, XOptionUnit>::iterator itr = m_OptionList.find( szName);
	if ( itr == m_OptionList.end())		return strDefaultValue;

	return (*itr).second.GetAttribute( strAttrName);
}










/** class XConfig **/

XConfig::XConfig()
{
}


void XConfig::Reset()
{
}


XConfig* XConfig::GetInstance()
{
	static XConfig	instance;
	return &instance;
}
