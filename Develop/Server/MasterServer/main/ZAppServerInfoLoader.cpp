#include "StdAfx.h"
#include "ZAppServerInfoLoader.h"
#include "ZAppServerInfoManager.h"
#include "ZServerInfoLoaderDef.h"

bool ZAppServerInfoLoader::Load(ZAppServerInfoManager* mgr, const wchar_t* szFileName)
{
	USES_CONVERSION_EX;
	m_pManager = mgr;

	MXml xml;
	if (!xml.LoadFile(W2A_EX(szFileName, 0))) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(SERVER_INFO_XML_TAG_MAIET).FirstChildElement().Element();

	if (Parse(pElement) == false)
		return false;

	return true;
}

bool ZAppServerInfoLoader::LoadStream(ZAppServerInfoManager* mgr, const char* szStream)
{
	m_pManager = mgr;

	MXml xml;

	if (!xml.LoadStream(szStream)) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(SERVER_INFO_XML_TAG_MAIET).FirstChildElement().Element();

	if (Parse(pElement) == false)
		return false;

	return true;
}

bool ZAppServerInfoLoader::Parse(MXmlElement* pElement)
{
	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), SERVER_INFO_XML_TAG_APP_SERVER))
		{
			int nID=0;
			if (_Attribute(&nID, pElement, SERVER_INFO_XML_ATTR_ID))
			{
				if (m_pManager->GetInfo(nID) != NULL)
				{
					return false;
				}

				ZAppServerInfo* pNewInfo = new ZAppServerInfo();
				pNewInfo->m_nID = nID;

				if (m_pManager->Insert(pNewInfo) == false)
				{
					SAFE_DELETE(pNewInfo);
					return false;
				}
			}
		}
	}

	return true;
}
