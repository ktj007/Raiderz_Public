#ifndef _GPROCESS_MANAGER_H
#define _GPROCESS_MANAGER_H

#include "GProcess.h"

/// 프로세스 관리자
class GProcessManager
{
private:
	vector<GProcess*>		m_vecProcesses;
public:
	GProcessManager();
	~GProcessManager();
	void Register(GProcess* pProcess);

	void Run();
};

#endif