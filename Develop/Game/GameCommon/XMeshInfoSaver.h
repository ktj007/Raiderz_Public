#pragma once

#include "MXml.h"
#include "CSMeshInfo.h"
#include "CSMeshInfoSaver.h"

class CSMeshInfoMgr;

class XMeshInfoSaver : public CSMeshInfoSaver
{
private:
	void SaveMeshHitEffectInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement);

public:
	virtual bool SaveMeshInfo(CSMeshInfoMgr* pMeshInfoMgr, const TCHAR* szFileName);
};