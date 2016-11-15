#pragma once

#include "NVPerfSDK.h"
#include <vector>

class NVPerfRenderCaller
{
public:
	virtual void CallRender() = 0;
};

class NVPerf
{
public:
	NVPerf() {}
	~NVPerf() {}

	void RunExperiment( NVPerfRenderCaller* pRenderCaller );

private:
	bool Init();
	void Shutdown();
};

class NVPerfCounter
{
public:
	NVPerfCounter( char* counterName );
	~NVPerfCounter();

	const char* GetCounterName() const { return mCounterName; }

	bool GetCounterValue( int objectID, UINT64* pOutValue, UINT64* pOutCycles );

private:
	char	mCounterName[MAX_PATH];
	bool	mIsAddSuccess;
};

class NVPerfCounterSet
{
public:
	NVPerfCounterSet() {}
	~NVPerfCounterSet();

	virtual	void SetCounters();
	virtual	void ReportLog();

	size_t		GetCountersNum();
	const char*	GetCounterName( size_t index );
	bool		GetCounterValue( size_t index, int objectID, UINT64* pOutValue, UINT64* pOutCycles );

protected:
	void AddCounter( char* counterName );

	std::vector<NVPerfCounter*>	mCounters;
};

class NVPerfCounterSetSimpleBottleneck : public NVPerfCounterSet
{
public:
	virtual	void SetCounters() override;
	virtual	void ReportLog() override;
};

class NVPerfCounterSetBottleneck : public NVPerfCounterSet
{
public:
	virtual	void SetCounters() override;
};