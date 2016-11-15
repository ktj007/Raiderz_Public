#pragma once

#include "CSInnInfoMgr.h"

class XInnInfo;

#define INN_XML_ATTR_NAME		"name"

class XInnInfoMgr : public CSInnInfoMgr
{
private:
	virtual bool ParseInn(MXmlElement* pElement, MXml* pXml, CSInnInfo* pInfo);

public:
	XInnInfoMgr(void);
	virtual ~XInnInfoMgr(void);

	virtual CSInnInfo* NewInfo();	
	XInnInfo* Get(uint8 nID);
};
