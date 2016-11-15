#include "stdafx.h"
#include "MockBuffInfoMgr.h"


MockBuffInfoMgr::MockBuffInfoMgr()
{
}

MockBuffInfoMgr::~MockBuffInfoMgr()
{
}

void MockBuffInfoMgr::CreateBuffInfo( const std::wstring& _strXmlData)
{
	SetBuffInfo( _strXmlData);
}


bool MockBuffInfoMgr::SetBuffInfo( const std::wstring& _strXmlData)
{
	USES_CONVERSION_EX;

	MXmlDocument doc;
	doc.Parse( W2A_EX(_strXmlData.c_str(), 0));
	MXmlElement* pRootElement = doc.FirstChildElement();
	if ( pRootElement == NULL)
		return false;

	ParseBuff(pRootElement, NULL);

	return true;
}
