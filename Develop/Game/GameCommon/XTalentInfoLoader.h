#pragma once

#include "XTalentInfo.h"
#include "CSTalentInfoLoader.h"

class XTalentInfoMgr;

class XTalentInfoLoader : public CSTalentInfoLoader
{
protected:
	bool LoadTalent(XTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName);
	bool LoadExt(XTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName);
public:
	XTalentInfoLoader() {}
	virtual ~XTalentInfoLoader() {}
	bool Load(XTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName, const TCHAR* szExtFileName, const TCHAR* szHitFileName);


	using CSTalentInfoLoader::LoadHitInfo;
};