/***************************************************************************************************
**
** profile.cpp
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/
#include "stdafx.h"
#include "RGPUProfiler.h"

namespace rs3 
{

inline void RGPUProfiler::Profile_Get_Ticks(_int64 * ticks)
{
	QueryPerformanceCounter((LARGE_INTEGER*)ticks);
	return;

	__asm
	{
		push eax;
		push edx;
		push ecx;
		mov ecx,ticks;
		_emit 0Fh
		_emit 31h
		mov [ecx],eax;
		mov [ecx+4],edx;
		pop ecx;
		pop edx;
		pop eax;
	}
}

inline float RGPUProfiler::Profile_Get_Tick_Rate(void)
{
	static float _CPUFrequency = -1.0f;
	
	if (_CPUFrequency == -1.0f) {
		__int64 curr_rate = 0;
		::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
		_CPUFrequency = (float)curr_rate;
	} 
	
	return _CPUFrequency;
}

RS_API RGPUProfiler& GetGPUProfiler( void )
{
	static RGPUProfiler gsGPUProfiler;
	return gsGPUProfiler;
}


RGPUProfiler::RGPUProfiler() : 
//m_pCurrentNode(NULL),
m_pQueryManager(NULL),
m_EventQuery(NULL)
{

}

RGPUProfiler::~RGPUProfiler()
{
	Destroy();
}

void RGPUProfiler::Create()
{
	m_pQueryManager = REngine::GetDevicePtr()->GetQueryManager();
	_ASSERT(m_pQueryManager);

	m_EventQuery	= REngine::GetDevicePtr()->CreateEventQuery();
	_ASSERT(m_EventQuery);	
}

void RGPUProfiler::Destroy()
{
	if( NULL != REngine::GetDevicePtr() )
		REngine::GetDevicePtr()->ReleaseEventQuery(m_EventQuery);
}

void RGPUProfiler::Call(CProfileNode* pCurrentNode)
{
	_ASSERT(pCurrentNode);

	__int64 n64_GPU_StartTime;
	Profile_Get_Ticks(&n64_GPU_StartTime);

	m_EventQuery->EndIssue();
	m_EventQuery->Data(NULL);

	__int64 fStopTime;
	Profile_Get_Ticks(&fStopTime);
	fStopTime -= n64_GPU_StartTime;

	float fCurrentTime = (float)fStopTime / Profile_Get_Tick_Rate();
	float fGPUTime = pCurrentNode->Get_Current_GPUTime();
	fGPUTime += fCurrentTime;
	pCurrentNode->Set_Current_GPUTime(fGPUTime);
}

}