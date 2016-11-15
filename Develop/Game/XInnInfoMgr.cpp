#include "StdAfx.h"
#include "XInnInfoMgr.h"
#include "XInnInfo.h"

XInnInfoMgr::XInnInfoMgr(void)
{
}

XInnInfoMgr::~XInnInfoMgr(void)
{
}

bool XInnInfoMgr::ParseInn(MXmlElement* pElement, MXml* pXml, CSInnInfo* pInfo)
{
	if (false == __super::ParseInn(pElement, pXml, pInfo)) return false;

	XInnInfo* pXInfo = static_cast<XInnInfo*>(pInfo);

	_Attribute(pXInfo->m_strName, pElement, INN_XML_ATTR_NAME);

	return true;
}

CSInnInfo* XInnInfoMgr::NewInfo()
{
	return new XInnInfo();
}

XInnInfo* XInnInfoMgr::Get(uint8 nID)
{
	return static_cast<XInnInfo*>(__super::Get(nID));
}