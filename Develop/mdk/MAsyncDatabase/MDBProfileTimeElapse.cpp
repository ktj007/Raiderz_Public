#include "stdafx.h"
#include "MDBProfileTimeElapse.h"


// to use timeGetTime() function.
#include <MMSystem.h>
#pragma comment (lib, "winmm.lib")


namespace mdb
{
	MDBProfileTimeElapse::MDBProfileTimeElapse()
	{
		m_dwBgineTime		= 0;
		m_dwEndTime			= 0;
		m_dwArrayIndex		= 0;
		m_dwMaxElapseTime	= 0;
		m_dwMinElapseTime	= 0xffffffff;

		memset(m_ElapseTimeArray, 0, sizeof(m_ElapseTimeArray));
	}


	MDBProfileTimeElapse::~MDBProfileTimeElapse()
	{

	}


	void MDBProfileTimeElapse::Begin()
	{
		m_dwBgineTime = timeGetTime();
	}


	void MDBProfileTimeElapse::End()
	{
		m_dwEndTime = timeGetTime();

		SetMinMaxElapseTime();
		SaveElapseTime();
		MoveNextArrayInexe();
	}


	DWORD MDBProfileTimeElapse::GetElapseTime()
	{
		return m_dwEndTime - m_dwBgineTime;
	}


	void MDBProfileTimeElapse::SetMinMaxElapseTime()
	{
		const DWORD dwComletedTime = GetElapseTime();

		if (m_dwMaxElapseTime < dwComletedTime)
		{
			m_dwMaxElapseTime = dwComletedTime;
		}
		else if (m_dwMinElapseTime > dwComletedTime)
		{
			m_dwMinElapseTime = dwComletedTime;
		}
	}


	void MDBProfileTimeElapse::SaveElapseTime()
	{
		m_ElapseTimeArray[m_dwArrayIndex] = GetElapseTime();
	}


	void MDBProfileTimeElapse::MoveNextArrayInexe()
	{
		if (MDB_PROFILE_SAVE_COUNT < (++m_dwArrayIndex))
		{
			m_dwArrayIndex = 0;
		}
	}


	DWORD MDBProfileTimeElapse::GetLastBeginTime()
	{
		return m_dwBgineTime;
	}


	DWORD MDBProfileTimeElapse::GetLastEndTime()
	{
		return m_dwEndTime;
	}


	float MDBProfileTimeElapse::GetAvgElapseTime()
	{
		DWORD dwSum			= 0;
		DWORD dwAddCount	= 0;

		for (int i = 0; i < MDB_PROFILE_SAVE_COUNT; ++i)
		{
			if (0 == m_ElapseTimeArray[i])
			{
				continue;
			}

			dwSum += m_ElapseTimeArray[i];
			++dwAddCount;
		}

		if (0 == dwAddCount)
		{
			return 0;
		}

		return dwSum / float(dwAddCount);
	}
}