#include "StdAfx.h"
#include "ZLoginServerInfoLoader.h"
#include "ZLoginServerInfoManager.h"
#include "ZServerInfoLoaderDef.h"

bool ZLoginServerInfoLoader::Load(ZLoginServerInfoManager* mgr, const wchar_t* szFileName)
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

bool ZLoginServerInfoLoader::LoadStream(ZLoginServerInfoManager* mgr, const char* szStream)
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

bool ZLoginServerInfoLoader::Parse(MXmlElement* pElement)
{
	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), SERVER_INFO_XML_TAG_LOGIN_SERVER))
		{
			int nID=0;
			if (_Attribute(&nID, pElement, SERVER_INFO_XML_ATTR_ID))
			{
				if (m_pManager->GetInfo(nID) != NULL)
				{
					return false;
				}

				ZLoginServerInfo* pNewInfo = new ZLoginServerInfo();
				pNewInfo->m_nID = nID;

				if (m_pManager->Insert(pNewInfo) == false)
				{
					SAFE_DELETE(pNewInfo);
					return false;
				}
				
				break;	// 오직 한개의 로그인서버 정보만 읽는다.
			}
		}
	}

	return true;
}
