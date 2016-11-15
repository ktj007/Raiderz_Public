#include "stdafx.h"
#include "ZFixedPartyInfoManager.h"
#include "MLocale.h"
#include "ZServerInfoLoaderDef.h"

ZFixedPartyInfoManager::ZFixedPartyInfoManager()
{
}

ZFixedPartyInfoManager::~ZFixedPartyInfoManager()
{
}

bool ZFixedPartyInfoManager::Load(wstring strFileName)
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(strFileName).c_str()))
	{
		return false;
	}


	MXmlHandle doc = xml.DocHandle();
	MXmlElement* pElement = doc.FirstChild(SERVER_INFO_XML_TAG_MAIET).FirstChildElement().Element();

	if (0 == _stricmp(pElement->Value(), "FIXED_PARTY"))
	{		
		MXmlElement* pChild = pElement->FirstChildElement();
		
		while(pChild != NULL)
		{
			ParseParty(pChild, &xml);
			pChild = pChild->NextSiblingElement();
		}		
	}
		
	return true;
}

void ZFixedPartyInfoManager::ParseParty(MXmlElement* pElement, MXml* pXML)
{
	if (0 == _stricmp(pElement->Value(), "PARTY"))
	{
		int nID;
		wstring strName;

		if (!_Attribute(nID, pElement, "id"))
		{
			return;
		}

		_Attribute(strName, pElement, "name", pXML);	// ¿É¼Ç°ª

		AddPartyInfo(nID, strName);


		MXmlElement* pChild = pElement->FirstChildElement();

		while(pChild != NULL)
		{
			ParseMember(pChild, nID, pXML);
			pChild = pChild->NextSiblingElement();
		}
	}
}

void ZFixedPartyInfoManager::ParseMember(MXmlElement* pElement, int nFixedPartyID, MXml* pXML)
{
	if (0 == _stricmp(pElement->Value(), "MEMBER"))
	{
		wstring strName;
		
		if (!_Attribute(strName, pElement, "name", pXML))
		{
			return;
		}
	
		AddMemberInfo(nFixedPartyID, strName);
	}
}

void ZFixedPartyInfoManager::AddPartyInfo(int nFixedPartyID, wstring strPartyName)
{

}

void ZFixedPartyInfoManager::AddMemberInfo(int nFixedPartyID, wstring strMemberName)
{
	m_mapFixedMember.insert(make_pair(strMemberName, nFixedPartyID));
}

int ZFixedPartyInfoManager::FindFixedPartyID(wstring strName)
{
	fixedmember_map::iterator it = m_mapFixedMember.find(strName);
	
	if (it == m_mapFixedMember.end())
	{
		return INVALID_ID;		
	}

	return it->second;
}

wstring ZFixedPartyInfoManager::FindFixedPartyName(int nFixedPartyID)
{
	fixedparty_map::iterator it = m_mapFixedParty.find(nFixedPartyID);

	if (it == m_mapFixedParty.end())
	{
		return wstring();
	}

	return it->second;
}
