#ifndef _TBUFF_INFO_H
#define _TBUFF_INFO_H

#include "XBuffInfo.h"

class TBuffInfoMgr : public XBuffInfoMgr
{
private:
	bool					m_bBuffEffectFileLoadError;

private:
	bool					ReLoad();

public:
	TBuffInfoMgr();
	virtual ~TBuffInfoMgr();

	void					Init();

	XBuffInfo *				GetBuffInfo(int nBuffID);
	XBuffEffectData *		GetBuffEffect(int nBuffID);
	XBuffEffectInfo *		GetBuffEffectInfo(int nBuffID, BUFF_EFFECT_TYPE nType, int nIndex);
	XEnchantBuffEffectData*	GetEnchantBuffEffectData(int nBuffID);
	void					GetBuffInfoList(vector<XBuffInfo*>& vecBuffInfo);

	bool					GetFileLoadErrorResult() { return m_bBuffEffectFileLoadError; }

	void					GetEnchantBuffIDList(vector<int>& vecEnchantBuffID);

	bool					CheckTalentInfoModify();
};

#endif