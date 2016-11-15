#ifndef _SDBTASK_TRACER_H
#define _SDBTASK_TRACER_H

#include <map>


#include "SDBTaskID.h"


class SDBTaskTracer
{
public :
	SDBTaskTracer() {}

	void	PushTraceTaskID(SDBTASK_ID TaskID);
	void	PopTraceTaskID(SDBTASK_ID TaskID);
	bool	IsTracingID(SDBTASK_ID TaskID);
	void	IncTraceCheckCount(SDBTASK_ID TaskID);
	size_t	GetTaceCheckCount(SDBTASK_ID TaskID);
	size_t	GetMaxTraceCheckCount();

private :
	map<SDBTASK_ID, size_t> m_mapTraceID;			
};


#endif