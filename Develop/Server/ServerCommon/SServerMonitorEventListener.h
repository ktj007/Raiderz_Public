#pragma once

#include "ServerCommonLib.h"

class MRegulator;

class SCOMMON_API SServerMonitorEventListener
{
public:
	SServerMonitorEventListener();
	~SServerMonitorEventListener();

	bool Create(std::wstring strServerName);
	bool CheckSignal_Terminate();

private:
	HANDLE m_hServerMonitorSemaphore;
	bool m_bIsAssociatedServerMonitor;

};
