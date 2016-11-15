#pragma once

#include "XTalentInfo.h"
#include "XTalentEffectInfo.h"

//----------------------------------------------------------------------------//
// Åø Àü¿ë TalentEffectMgr Editor                                                                     
//----------------------------------------------------------------------------//
class TTalentEffectMgr
{
protected:
	CSTalentInfoMgr*					m_pTalentInfoMgr;

	map<int, XTalentEffectBase *>		m_mapEffectBase;

private:
	void								SaveTalentEffectBase(MXmlElement * pParentElement);

	void								AddTalentEffectElement(MXmlElement* pXmlElement, TTalentInfo* pTalentInfo);
	void								AddTalentHitEffectPosDirElement(MXmlElement* pXmlElement, XTalentEffectInfo* pTalentEffectInfo);
public:
	TTalentEffectMgr(CSTalentInfoMgr* pTalentInfoMgr);
	virtual ~TTalentEffectMgr();

	XTalentEffectInfo *					CreateTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, const char* chrEffectInfoName);

	bool								DeleteTalentEffectData(XTalentEffectDataMgr* pEffectDataMgr);
	bool								DeleteTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, const char* chrEffectInfoName, int& nOutTalentEffectInfoCount);
	bool								DeleteTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, TALENT_EFFECT_TYPE nType);

	bool								Save();

	
	XTalentEffectInfo *					GetTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, const char* chrEffectInfoName);
	void								GetTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, TALENT_EFFECT_TYPE nType, vector<XTalentEffectInfo *> &TalentEffectInfo);
	void								GetDefaultTalentEffectInfo(TALENT_SKILL_TYPE nSkillType, TALENT_EFFECT_TYPE nType, XTalentEffectBase * pTalentEffectBaseInfo);

	XTalentEffectBase *					GetTalentEffectBase(TALENT_EFFECT_BASE_TYPE nType);
	map<int, XTalentEffectBase *> *		GetTalentEffectBase()  { return &m_mapEffectBase; }

	bool								CheckDuplicationTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, const char* chrEffectInfoName);

};


void									SetTextEffectDirectionList(string& strBuffer, TALENT_EFFECT_DIRECTION nDirType);
TALENT_EFFECT_DIRECTION					GetEffectDirectionList(string& strBuffer);
