#include "stdafx.h"
#include "MDBProfiler.h"
#include "MDBProfileDefine.h"


// to use timeGetTime() function.
#include <MMSystem.h>
#pragma comment (lib, "winmm.lib")


namespace mdb
{
	MDBProfiler::MDBProfiler() 
	{
	}


	MDBProfiler::~MDBProfiler()
	{

	}


	MDBProfileCountPerTime& MDBProfiler::GetProfileExecute()
	{
		return m_ProfileExecute;
	}


	MDBProfileCountPerTime& MDBProfiler::GetProfileCompleted()
	{
		return m_ProfileCompleted;
	}


	MDBProfileTimeElapse& MDBProfiler::GetProfileTimeElapsed()
	{
		return m_ProfileTimeElapse;	
	}


	void MDBProfiler::AddExecute()
	{
		m_ProfileExecute.AddCount();
	}


	void MDBProfiler::AddCompleted()
	{
		m_ProfileCompleted.AddCount();
	}


	void MDBProfiler::BeginOnCompleted()
	{
		m_ProfileTimeElapse.Begin();
	}


	void MDBProfiler::EndOnCompleted()
	{
		m_ProfileTimeElapse.End();
	}
}
