#include "stdafx.h"
#include "MockNPCInfo.h"


bool MockNPCInfoMgr::CreateNPCFromString( const std::wstring& _strXmlData )
{
	USES_CONVERSION_EX;

	MXmlDocument doc;
	doc.Parse(W2A_EX(_strXmlData.c_str(), 0));
	MXmlElement* pRootElement = doc.FirstChildElement();
	if(pRootElement == NULL)
		return false;

	ParseNPC(pRootElement, NULL);
	ParseNPC_SubMtrl(pRootElement, NULL);
	return true;
}
