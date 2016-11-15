#pragma once

#include "Profile/profile.h"

class CProfileNode;

namespace rs3 
{

class RD3DQueryManager;
class REventQuery;


class RS_API RGPUProfiler 
{
public:
	RGPUProfiler();
	~RGPUProfiler();

	void						Profile_Get_Ticks(_int64 * ticks);
	float						Profile_Get_Tick_Rate(void);

	void						Create();
	void						Destroy();
	void						Call(CProfileNode* pCurrentNode);

	REventQuery*				GetEventQuery() { return m_EventQuery; }

private:
	RD3DQueryManager*			m_pQueryManager;
	REventQuery*				m_EventQuery;
};

RS_API RGPUProfiler& GetGPUProfiler(void);

}