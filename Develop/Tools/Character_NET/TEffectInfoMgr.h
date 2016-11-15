#ifndef _TEFFECT_INFO_MGR_H
#define _TEFFECT_INFO_MGR_H

#include "XEffectInfo.h"

class TEffectInfoMgr : public XEffectInfoMgr
{
private:
	bool				m_bEffectFileLoadError;

protected:
	virtual	void		GetCommaStringSplitter(const tstring& strValue, vector<tstring>& vecOut);

public:
	TEffectInfoMgr();
	virtual ~TEffectInfoMgr();

	bool				ReLoad();
	bool				CheckEffectInfoModify();

	bool				GetFileLoadErrorResult() { return m_bEffectFileLoadError; }
};

#endif