#include "stdafx.h"
#include "SDBTaskTracer.h"


void SDBTaskTracer::PushTraceTaskID( SDBTASK_ID TaskID )
{
	m_mapTraceID.insert(pair<SDBTASK_ID, int>(TaskID, 0));
}

void SDBTaskTracer::PopTraceTaskID( SDBTASK_ID TaskID )
{
	m_mapTraceID.erase(TaskID);
}

bool SDBTaskTracer::IsTracingID( SDBTASK_ID TaskID )
{
	if (m_mapTraceID.end() == m_mapTraceID.find(TaskID))
		return false;

	return true;
}

void SDBTaskTracer::IncTraceCheckCount( SDBTASK_ID TaskID )
{
	map<SDBTASK_ID, size_t>::iterator it = m_mapTraceID.find(TaskID);
	if (m_mapTraceID.end() == it)
		return;

	it->second++;
}

size_t SDBTaskTracer::GetTaceCheckCount( SDBTASK_ID TaskID )
{
	map<SDBTASK_ID, size_t>::iterator it = m_mapTraceID.find(TaskID);
	if (m_mapTraceID.end() == it)
		return 0;

	return it->second;
}

size_t SDBTaskTracer::GetMaxTraceCheckCount()
{
	return 3;
}
