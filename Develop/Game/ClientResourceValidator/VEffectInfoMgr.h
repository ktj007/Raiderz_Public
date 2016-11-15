#pragma once

#include "VFileName.h"
#include "XEffectInfo.h"
#include "VErrorMsg.h"

class VEffectInfoMgr : public XEffectInfoMgr
{
public:
	vector<FILENAME_DATA>		m_vecEffectFileList;

	VErrorMsg					m_ErrorMsg;

public:
	VEffectInfoMgr();
	virtual ~VEffectInfoMgr();
	
	void						LoadingFileInfo();
	void						ParseEffectInfo(MXmlElement* pElement, MXml* pXml);

	FILENAME_DATA*				GetEffectFileInfo(string& strName);
};

