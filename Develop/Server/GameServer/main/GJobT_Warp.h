#ifndef _GJobT_Warp_h
#define _GJobT_Warp_h

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_Warp: public GInstantJob, public MMemPool<GJobT_Warp>
{
private:
	vec3 m_vecPos;
	vec3 m_vecDir;
	bool m_bRouteToMe;

protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_WARP);
	virtual string GetParamString() const override;

	GJobT_Warp(GModuleAI* pmAI, vec3 vecPos, vec3 vecDir, bool bRouteToMe);
	virtual ~GJobT_Warp();
};

#endif
