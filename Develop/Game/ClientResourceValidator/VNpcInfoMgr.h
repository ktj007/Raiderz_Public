#pragma once

#include "VErrorMsg.h"
#include "XNPCInfo.h"

class VNpcInfoMgr : public XNPCInfoMgr
{
public:
	VErrorMsg							m_ErrorMsg;

public:
	VNpcInfoMgr();
	virtual ~VNpcInfoMgr();

	void								ParseNPCInfo(MXmlElement* pElement, MXml* pXml);
};

