#pragma once

#include "VInfoLoader.h"
#include "XTalentInfo.h"
#include "XTalentInfoLoader.h"

class XTalentInfoMgr;

class VTalentInfoLoader : public XTalentInfoLoader, public VInfoLoader
{
private:
	MXml m_xmlTalent;
	MXml m_xmlTalentExt;
	MXml m_xmlTalentHitInfo;

public:
	int m_nTalentCount;
	int m_nTalentExtCount;
	int m_nTalentHitInfoCount;

protected:
	bool 									LoadTalent(BackgroundWorker^ worker, XTalentInfoMgr* pTalentInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);
	bool 									LoadExt(BackgroundWorker^ worker, XTalentInfoMgr* pTalentInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);
	bool 									LoadHitInfo(BackgroundWorker^ worker, CSTalentInfoMgr* pTalentInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	int 									Load_Ready_Talent(string& strFileName);
	int 									Load_Ready_TalentExt(string& strFileName);
	int 									Load_Ready_TalentHitInfo(string& strFileName);

public:
	VTalentInfoLoader();
	virtual ~VTalentInfoLoader() {}

	virtual void 							Init();
	virtual void 							Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetLoadingCount() { return m_nTalentCount + m_nTalentExtCount + m_nTalentHitInfoCount; }
	virtual LOADING_STATE					GetID() { return LS_TALENT_INFO; }

	using CSTalentInfoLoader::LoadHitInfo;
};