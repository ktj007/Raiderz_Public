#pragma once

#include "XBuffInfo.h"
#include "VErrorMsg.h"

class VBuffInfoMgr : public XBuffInfoMgr
{
public:
	VErrorMsg						m_ErrorMsg;

public:
	VBuffInfoMgr();
	virtual ~VBuffInfoMgr();

	void							ParseBuffInfo(MXmlElement* pElement, MXml* pXml);
	void							ParseBuffEffectInfo(MXmlElement* pElement, MXml* pXml);
};

