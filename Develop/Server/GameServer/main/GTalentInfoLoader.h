#pragma once

#include "GTalentInfo.h"
#include "CSTalentInfoLoader.h"

class GTalentInfoMgr;

struct GTalentInfoPathParam
{
	wstring strTalentInfo;
	wstring strTalentInfoExt;
	wstring strTalentPosInfo;
	wstring strTalentHitInfo;
};

class GTalentInfoLoader : public CSTalentInfoLoader
{
public:
private:
	bool LoadTalent( GTalentInfoMgr* pTalentInfoMgr, const wchar_t* szFileName);
	bool LoadTalentExt(GTalentInfoMgr* pTalentInfoMgr, const wchar_t* szFileName);
	bool LoadPosInfo(GTalentInfoMgr* pTalentInfoMgr, const wchar_t* szFileName);
public:
	GTalentInfoLoader() {}
	virtual ~GTalentInfoLoader() {}
	bool Load(GTalentInfoMgr* pTalentInfoMgr, const GTalentInfoPathParam& pathParam);
};