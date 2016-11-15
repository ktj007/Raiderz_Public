#pragma once

#include "MXml.h"

class CSTalentInfoMgr;
class XTalentEffectBase;
class XTalentEffectInfo;

class XTalentEffectInfoParser
{
protected:
	void								ParseTalentBase(MXmlElement* pElement, MXml* pXml, map<int, XTalentEffectBase *>& mapEffectBase);
	void								ParseTalentBaseInfo(MXmlElement* pAttElement, XTalentEffectBase * pEffectBase);
	void								ParseTalentEffect(MXmlElement* pElement, MXml* pXml, CSTalentInfoMgr* pTalentInfoMgr);
	void								ParseTalentHitEffectPosDir(MXmlElement* pElement, MXml* pXml, XTalentEffectInfo* pTalentInfo);

public:
	XTalentEffectInfoParser();
	virtual ~XTalentEffectInfoParser();

	bool								Load(const TCHAR* szFileName, CSTalentInfoMgr* pTalentInfoMgr, map<int, XTalentEffectBase *>& mapEffectBase);
};
