#ifndef _GJobT_BreakPart_h
#define _GJobT_BreakPart_h

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_BreakPart: public GInstantJob, public MMemPool<GJobT_BreakPart>
{
private:
	uint8 m_nPartID;
	MUID m_uidOpponent;

protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_BREAKPART);
	virtual string GetParamString() const override;

	GJobT_BreakPart(GModuleAI* pmAI, uint8 nPartID, MUID uidOpponent);
	virtual ~GJobT_BreakPart();
};

#endif
