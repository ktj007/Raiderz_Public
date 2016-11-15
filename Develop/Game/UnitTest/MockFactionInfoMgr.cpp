#include "StdAfx.h"
#include "MockFactionInfoMgr.h"


bool MockFactionInfoMgr::CreateFactionFromString( const std::wstring& _strXmlData )
{
	USES_CONVERSION_EX;

	MXmlDocument doc;
	doc.Parse(W2A_EX(_strXmlData.c_str(), 0));
	MXmlElement* pRootElement = doc.FirstChildElement();
	if(pRootElement == NULL)
		return false;

	ParseFaction(pRootElement, NULL);
	return true;
}