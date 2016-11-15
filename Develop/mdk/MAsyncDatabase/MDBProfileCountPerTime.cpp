#include "stdafx.h"
#include "MDBProfileCountPerTime.h"
#include "MDBProfileDefine.h"


// to use timeGetTime() function.
#include <MMSystem.h>
#pragma comment (lib, "winmm.lib")


namespace mdb
{
	MDBProfileCountPerTime::MDBProfileCountPerTime()
	{
		m_dwLastCountPerSecond	= 0;
		m_dwCountPerSecond		= 0;
		m_dwLastSaveCountTime	= timeGetTime();
		m_dwArrayIndex			= 0;
		m_dwMaxCountPerSecond	= 0;
		m_dwMinCountPerSecond	= 0xffffffff;

		memset(m_CountArray, 0, sizeof(m_CountArray));
	}


	MDBProfileCountPerTime::~MDBProfileCountPerTime()
	{

	}


	void MDBProfileCountPerTime::AddCount()
	{
		MDBR_DWORD_ADD_COUNT(m_dwCountPerSecond);

		CheckSaveCount();
	}


	void MDBProfileCountPerTime::CheckSaveCount()
	{
		if (IsSaveCount())
		{
			SetMinMaxCount();
			SaveCount();
			MoveNextArrayIndex();
		}
	}


	bool MDBProfileCountPerTime::IsSaveCount()
	{
		return MDB_PROFILE_MSEC < (timeGetTime() - m_dwLastSaveCountTime);
	}


	void MDBProfileCountPerTime::SaveCount()
	{
		m_CountArray[m_dwArrayIndex]	= m_dwCountPerSecond;
		m_dwLastSaveCountTime			= timeGetTime();
		m_dwLastCountPerSecond			= m_dwCountPerSecond;
		m_dwCountPerSecond				= 0;
	}


	void MDBProfileCountPerTime::MoveNextArrayIndex()
	{
		if (MDB_PROFILE_SAVE_COUNT < (++m_dwArrayIndex))
		{
			m_dwArrayIndex = 0;
		}
	}


	void MDBProfileCountPerTime::SetMinMaxCount()
	{
		if (m_dwMaxCountPerSecond < m_dwCountPerSecond)
		{
			m_dwMaxCountPerSecond = m_dwCountPerSecond;
		}
		else if(m_dwMinCountPerSecond > m_dwCountPerSecond)
		{
			m_dwMinCountPerSecond = m_dwCountPerSecond;
		}
	}


	DWORD MDBProfileCountPerTime::GetLastCount()
	{
		return m_dwLastCountPerSecond;
	}


	float MDBProfileCountPerTime::GetAvgCount()
	{
		DWORD dwSum			= 0;
		DWORD dwAddCount	= 0;

		for (int i = 0; i < MDB_PROFILE_SAVE_COUNT; ++i)
		{
			if (0 == m_CountArray[i])
			{
				continue;
			}

			dwSum += m_CountArray[i];
			++dwAddCount;
		}

		if (0 == dwAddCount)
		{
			return 0;
		}

		return dwSum / float(dwAddCount);
	}


	DWORD MDBProfileCountPerTime::GetMaxCount()
	{
		return m_dwMaxCountPerSecond;
	}


	DWORD MDBProfileCountPerTime::GetMinCount()
	{
		return m_dwMinCountPerSecond;
	}
}