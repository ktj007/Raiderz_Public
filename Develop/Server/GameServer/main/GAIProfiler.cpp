#include "stdafx.h"
#include "GAIProfiler.h"
#include "GConfig.h"

#include "GAIStatistics.h"

GAIProfiler g_aiProfiler;

// cml2/profile.h
inline void Profile_Get_Ticks(_int64 * ticks)
{
	__asm
	{
		push edx;
		push ecx;
		mov ecx,ticks;
		_emit 0Fh
			_emit 31h
			mov [ecx],eax;
		mov [ecx+4],edx;
		pop ecx;
		pop edx;
	}
}

inline float Profile_Get_Tick_Rate(void)
{
	static float _CPUFrequency = -1.0f;

	if (_CPUFrequency == -1.0f) {
		__int64 curr_rate = 0;
		::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
		_CPUFrequency = (float)curr_rate;
	} 

	return _CPUFrequency;
}

GAIProfiler::ProfileUpdater::ProfileUpdater(int nFrameCount)
: m_nFrameCount(nFrameCount)
{
	// do nothing
}

template <typename T>
void GAIProfiler::ProfileUpdater::operator() (T& pairProfile) const
{
	CountProfile& profile = pairProfile.second;

	profile.m_nMaxUpdateCount = std::max<int>(profile.m_nMaxUpdateCount, profile.m_nCurrentUpdateCount);
	profile.m_nTotalUpdateCount += profile.m_nCurrentUpdateCount;
	profile.m_nAverageUpdateCount = profile.m_nTotalUpdateCount / m_nFrameCount;
	profile.m_nCurrentUpdateCount = 0;

	profile.m_fMaxUpdateTime = std::max<float>(profile.m_fMaxUpdateTime, profile.m_fCurrentUpdateTime);	
	profile.m_fTotalUpdateTime += profile.m_fCurrentUpdateTime;
	profile.m_fAverageUpdateTime = profile.m_fTotalUpdateTime / m_nFrameCount;
	profile.m_fCurrentUpdateTime = 0;
}


GAIProfiler::GAIProfiler()
: m_nFrameCount(0)
, m_isEnable(false)
{
	// do nothing
}

GAIProfiler::~GAIProfiler()
{
	// do nothing
}

void GAIProfiler::Update(void)
{
	if (!m_isEnable)	return;


	m_nFrameCount++;
	for_each(m_mapCountProfile.begin(), m_mapCountProfile.end(), ProfileUpdater(m_nFrameCount));

	for(int i = 0; i < APT_ID_TALENT_OFFSET; i++)
	{
		CountProfile& profile = m_aCountProfile[i];

		profile.m_nMaxUpdateCount = std::max<int>(profile.m_nMaxUpdateCount, profile.m_nCurrentUpdateCount);
		profile.m_nTotalUpdateCount += profile.m_nCurrentUpdateCount;
		profile.m_nAverageUpdateCount = profile.m_nTotalUpdateCount / m_nFrameCount;
		profile.m_nCurrentUpdateCount = 0;

		profile.m_fMaxUpdateTime = std::max<float>(profile.m_fMaxUpdateTime, profile.m_fCurrentUpdateTime);	
		profile.m_fTotalUpdateTime += profile.m_fCurrentUpdateTime;
		profile.m_fAverageUpdateTime = profile.m_fTotalUpdateTime / m_nFrameCount;
		profile.m_fCurrentUpdateTime = 0;
	}
}

void GAIProfiler::Begin(int nAiProfilingId, wstring strName)
{
	if (!m_isEnable)	return;


	if (nAiProfilingId < APT_ID_TALENT_OFFSET)
	{
		m_aCountProfile[nAiProfilingId].m_strName = strName;
		Profile_Get_Ticks(&m_aCountProfile[nAiProfilingId].m_llTick);
	}
	else
	{
		map<int, CountProfile>::iterator it = m_mapCountProfile.find(nAiProfilingId);

		if (it == m_mapCountProfile.end())
		{
			CountProfile profile;
			profile.m_strName = strName;
			Profile_Get_Ticks(&profile.m_llTick);

			m_mapCountProfile.insert(make_pair(nAiProfilingId, profile));
		}
		else
		{
			it->second.m_strName = strName;
			Profile_Get_Ticks(&it->second.m_llTick);
		}
	}
}

void GAIProfiler::End(int nAiProfilingId)
{
	if (!m_isEnable)	return;


	if (nAiProfilingId < APT_ID_TALENT_OFFSET)
	{
		m_aCountProfile[nAiProfilingId].m_nCurrentUpdateCount++;

		int64 llTick;
		Profile_Get_Ticks(&llTick);
		llTick -= m_aCountProfile[nAiProfilingId].m_llTick;
		m_aCountProfile[nAiProfilingId].m_fCurrentUpdateTime += (float)llTick / Profile_Get_Tick_Rate();

	}
	else
	{
		map<int, CountProfile>::iterator it = m_mapCountProfile.find(nAiProfilingId);

		if (it != m_mapCountProfile.end())
		{		
			it->second.m_nCurrentUpdateCount++;

			int64 llTick;
			Profile_Get_Ticks(&llTick);
			llTick -= it->second.m_llTick;
			it->second.m_fCurrentUpdateTime += (float)llTick / Profile_Get_Tick_Rate();
		}
	}
}

int GAIProfiler::GetFrameCount(void)
{
	return m_nFrameCount;
}

const map<int, CountProfile>* GAIProfiler::GetProfileMap(void)
{
	for(int i = 0; i < APT_ID_TALENT_OFFSET; i++)
	{
		if (m_aCountProfile[i].m_nTotalUpdateCount == 0)
		{
			continue;
		}

		m_mapCountProfile.insert(make_pair(i, m_aCountProfile[i]));
	}

	return &m_mapCountProfile;
}

void GAIProfiler::SetEnable(bool bEnable)
{
	m_isEnable = bEnable;
}
