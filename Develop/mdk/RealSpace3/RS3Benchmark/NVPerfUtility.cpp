#include "stdafx.h"
#include "NVPerfUtility.h"
#include "MDebug.h"

bool NVPerf::Init()
{
	// NVPM_ERROR_DRIVER_MISMATCH: NVPerfAPI version and driver version do not match
	NVPMRESULT nvpmResult = NVPMInit(); 
	_ASSERT( NVPM_OK == nvpmResult );
	if( NVPM_OK != nvpmResult )
	{
		mlog( "Failed NVPMInit.\n" );
		return false;
	}

	// 카운터 모두 출력해보기
	//UINT unCountersNum;
	//nvpmResult = NVPMGetNumCounters( &unCountersNum );
	//_ASSERT( NVPM_OK == nvpmResult );
	//char counterName[MAX_PATH];
	//for( UINT i = 0; i< unCountersNum; ++i )
	//{
	//	UINT unCounterNameLen = MAX_PATH;
	//	nvpmResult = NVPMGetCounterName( i, counterName, &unCounterNameLen );
	//	_ASSERT( NVPM_OK == nvpmResult );
	//	mlog( "###Nvidia Performance Counter Type - %s\n", counterName );
	//}

	return true;
}

void NVPerf::Shutdown()
{
	// NVPM_ERROR_NOT_INITIALIZED: NVPMInit wasn't called or didn't complete sucessfully
	NVPMRESULT nvpmResult = NVPMShutdown();
	_ASSERT( NVPM_OK == nvpmResult );
}

void NVPerf::RunExperiment( NVPerfRenderCaller* pRenderCaller )
{
	_ASSERT( pRenderCaller );

	if( false == Init() )
		return;

//	NVPerfCounterSetSimpleBottleneck perfCounterSet;
	NVPerfCounterSetBottleneck perfCounterSet;
//	NVPerfCounterSet perfCounterSet;

	perfCounterSet.SetCounters();

	int nCount = 0;
	NVPMRESULT nvpmResult = NVPMBeginExperiment( &nCount );
	if( nvpmResult != NVPM_OK )
		return;

	for( int i = 0; i < nCount; ++i )
	{
		NVPMBeginPass(i);

		// Draw the scene, including Present or SwapBuffers
		pRenderCaller->CallRender();

		NVPMEndPass(i);
	}

	nvpmResult = NVPMEndExperiment();
	_ASSERT( NVPM_OK == nvpmResult );

	perfCounterSet.ReportLog();

	Shutdown();
}

///////////////////////////////////////////////////////////////////////////////

NVPerfCounter::NVPerfCounter( char* counterName ) :
 mIsAddSuccess(false)
{
	_ASSERT( counterName && strlen(counterName) > 0 );
	if( counterName && strlen(counterName) > 0 )
	{
		strcpy_s( mCounterName, MAX_PATH, counterName );
		NVPMRESULT result = NVPMAddCounterByName( mCounterName );
		_ASSERT( NVPM_OK == result );
		if( NVPM_OK == result )
			mIsAddSuccess = true;
	}
}

NVPerfCounter::~NVPerfCounter()
{
	if( mIsAddSuccess )
	{
		_ASSERT( mCounterName && strlen(mCounterName) > 0 );

		NVPMRESULT result = NVPMRemoveCounterByName( mCounterName );
		_ASSERT( NVPM_OK == result );
	}
}

bool NVPerfCounter::GetCounterValue( int objectID, UINT64* pOutValue, UINT64* pOutCycles )
{
	if( mIsAddSuccess )
	{
		_ASSERT( mCounterName && strlen(mCounterName) > 0 );

		NVPMRESULT nvpmResult = NVPMGetCounterValueByName( mCounterName, objectID, pOutValue, pOutCycles );
		_ASSERT( NVPM_OK == nvpmResult );
		if( NVPM_OK == nvpmResult )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////

NVPerfCounterSet::~NVPerfCounterSet()
{
	for( size_t i = 0; i < mCounters.size(); ++i )
	{
		delete mCounters[i];
	}
	mCounters.clear();
}

void NVPerfCounterSet::AddCounter( char* counterName )
{
	NVPerfCounter* pCounter = new NVPerfCounter( counterName );
	_ASSERT( pCounter );
	mCounters.push_back( pCounter );
}

size_t NVPerfCounterSet::GetCountersNum()
{
	return mCounters.size();
}

const char* NVPerfCounterSet::GetCounterName( size_t index )
{
	_ASSERT( index < mCounters.size() );
	return mCounters[index]->GetCounterName();
}

bool NVPerfCounterSet::GetCounterValue( size_t index, int objectID, UINT64* pOutValue, UINT64* pOutCycles )
{
	_ASSERT( index < mCounters.size() );
	return mCounters[index]->GetCounterValue( objectID, pOutValue, pOutCycles );
}

void NVPerfCounterSet::SetCounters()
{
	UINT unCountersNum;
	NVPMRESULT nvpmResult = NVPMGetNumCounters( &unCountersNum );
	_ASSERT( NVPM_OK == nvpmResult );

	char counterName[MAX_PATH];
	for( UINT i = 0; i< unCountersNum; ++i )
	{
		UINT unCounterNameLen = MAX_PATH;
		nvpmResult = NVPMGetCounterName( i, counterName, &unCounterNameLen );
		_ASSERT( NVPM_OK == nvpmResult );

		AddCounter( counterName );
	}

//	AddCounter( "gpu_idle" );
}

void NVPerfCounterSet::ReportLog()
{
	UINT64 value;
	UINT64 cycles;

	mlog( "########################################################\n" );
	for( size_t i = 0; i < GetCountersNum(); ++i )
	{
		const char* pCounterName = GetCounterName(i);

		if( GetCounterValue( i, 0, &value, &cycles ) )
			mlog( "### %s - value: %d cycles: %d\n", pCounterName, value, cycles );
		else
			mlog( "### %s - Failed getting counter value.\n", pCounterName );
	}
	mlog( "########################################################\n" );
}

///////////////////////////////////////////////////////////////////////////////

void NVPerfCounterSetSimpleBottleneck::SetCounters()
{
	AddCounter( "GPU Bottleneck" );
}

void NVPerfCounterSetSimpleBottleneck::ReportLog()
{
	_ASSERT( GetCountersNum() == 1 );

	UINT64 value;
	UINT64 cycles;

	mlog( "########################################################\n" );
	if( GetCounterValue( 0, 0, &value, &cycles ) )
	{
		char buff[MAX_PATH] = "unknown";
		NVPMRESULT nvpmResult = NVPMGetGPUBottleneckName( value, buff );
		_ASSERT( NVPM_OK == nvpmResult );
		mlog( "### GPU bottleneck is %s.\n", buff );
	}
	else
	{
		mlog( "### Fail to getting GPU bottleneck.\n" );
	}
	mlog( "########################################################\n" );
}

///////////////////////////////////////////////////////////////////////////////

void NVPerfCounterSetBottleneck::SetCounters()
{
	AddCounter( "IDX Bottleneck" );
	AddCounter( "IDX SOL" );
	AddCounter( "GEOM Bottleneck" );
	AddCounter( "GEOM SOL" );
	AddCounter( "Primitive Setup Bottleneck" );
	AddCounter( "Primitive Setup SOL" );
	AddCounter( "Rasterization Bottleneck" );
	AddCounter( "Rasterization SOL" );
	AddCounter( "SHD Bottleneck" );
	AddCounter( "TEX Bottleneck" );
	AddCounter( "TEX SOL" );
	AddCounter( "ROP Bottleneck" );
	AddCounter( "ROP SOL" );
	AddCounter( "FB Bottleneck" );
	AddCounter( "FB SOL" );
}

///////////////////////////////////////////////////////////////////////////////