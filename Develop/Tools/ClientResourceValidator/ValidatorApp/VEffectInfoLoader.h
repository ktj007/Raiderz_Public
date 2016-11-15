#pragma once

#include "VInfoLoader.h"
#include "XTalentEffectInfoParser.h"

class VEffectInfoLoader : public XTalentEffectInfoParser, public VInfoLoader
{
private:
	MXml 							m_xmlEffect;
	MXml 							m_xmlTalentEffect;
	MXml 							m_xmlBuffEffect;

	int 							m_nEffectCount;
	int 							m_nTalentEffectCount;
	int 							m_nBuffEFfectCount;

private:
	int 							Load_Ready_Effect(string& strFileName);
	int 							Load_Ready_TalentEffect(string& strFileName);
	int 							Load_Ready_BuffEffect(string& strFileName);

protected:
	bool 							LoadEffect(BackgroundWorker^ worker, VEffectInfoMgr* pEffectInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);
	bool 							LoadTalentEffect(BackgroundWorker^ worker, VTalentInfoMgr* pTalentInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);
	bool 							LoadBuffEffect(BackgroundWorker^ worker, VBuffInfoMgr* pBuffInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);


public:
	VEffectInfoLoader();
	virtual ~VEffectInfoLoader();

	virtual void					Init();
	virtual void					Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int						GetLoadingCount() { return m_nEffectCount + m_nTalentEffectCount + m_nBuffEFfectCount; }
	virtual LOADING_STATE			GetID() { return LS_EFFECT_INFO; }
};