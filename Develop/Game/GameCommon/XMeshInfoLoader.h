#pragma once

#include "CSMeshInfoLoader.h"

class XMeshInfo;

class XMeshInfoLoader : public CSMeshInfoLoader
{
private:
	void ParseMeshHitEffectInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement);

public:
	virtual bool LoadMeshInfo(CSMeshInfoMgr* pMeshInfoMgr, const TCHAR* szFileName);

	virtual void ParseMeshInfo( CSMeshInfo* pMeshInfo, MXml* pXml, MXmlElement* pElement);
};