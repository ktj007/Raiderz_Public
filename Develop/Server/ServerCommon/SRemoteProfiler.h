#pragma once

#include "Profile/profile.h"

class SRemoteProfiler
{
public:
	SRemoteProfiler(void);
	~SRemoteProfiler(void);

	static SRemoteProfiler& Instance();

	void SendToProfiler( HWND SourceHwnd = NULL);
private:
	MBuffer*	m_pProfileBuffer;
	MBuffer*	m_pTempBuffer1;
	MBuffer*	m_pTempBuffer2;
};
