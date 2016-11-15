#ifndef _GJOB_SCRIPT_H
#define _GJOB_SCRIPT_H

#include "MMemPool.h"
#include "GInstantJob.h"

class GEntityActor;
class GModuleAI;

class GJobT_Script : public GInstantJob, public MMemPool<GJobT_Script>
{
private:
	MUID			m_uidOpponent;
	wstring			m_strTable;
	wstring			m_strFunc;
protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_SCRIPT);
	virtual string GetParamString() const override;

	GJobT_Script(GModuleAI* pmAI, MUID uidOpponent, wstring strTable, wstring strFunc);
	virtual ~GJobT_Script();
};







#endif