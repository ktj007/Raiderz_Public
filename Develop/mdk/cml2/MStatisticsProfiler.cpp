#include "stdafx.h"
#include "MStatisticsProfiler.h"
#include <vector>
#include "MDebug.h"
#include "MMath.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// MStatisticsProfiler //////////
MStatisticsProfiler*	g_pDefaultStatisticsProfiler = NULL;

// _DO_NOT_USE_PROFILER 디파인이나 _PUBLISH 디파인이 걸려있으면 프로파일링을 하지 않는다.
#if (!defined(_USE_PROFILER)) || (defined(_PUBLISH))

DWORD MStatisticsProfiler::GetNowTime() { return 0; }
MStatisticsProfiler::~MStatisticsProfiler() {}
void MStatisticsProfiler::Init(bool bEnabled, bool bRecord) {}
void MStatisticsProfiler::Reset() {}
void MStatisticsProfiler::BeginProfile(int nIndex, const char* szName){}
void MStatisticsProfiler::EndProfile(int nIndex){}
void MStatisticsProfiler::AddLineForAnalysis( int nIndex ) {}
void MStatisticsProfiler::FinalAnalysis(const char* filename){}
void MStatisticsProfiler::PutLine( FILE * file ) {}
void MStatisticsProfiler::ResetSampling() {}
void MStatisticsProfiler::SaveSampling( const char* filename ) {}

DWORD MStatisticsProfiler::GetTotalTimeByIndex(int nIndex) { return 0; }
float MStatisticsProfiler::GetFrameByIndex(int nIndex) { return 0.0f; }
const char* MStatisticsProfiler::GetName(int nIndex) { return ""; }

#else

#pragma comment(lib, "winmm.lib")

DWORD MStatisticsProfiler::GetNowTime()
{
	return timeGetTime();
}

MStatisticsProfiler::~MStatisticsProfiler()
{
	if( m_bRecordSampling )
	{
		for(int i=0;i<MAX_PROFILE_COUNT;i++)
		{
			if (m_ProfileItems[i].arrayElapsedTimeData)
			{
				delete[] m_ProfileItems[i].arrayElapsedTimeData;
				m_ProfileItems[i].arrayElapsedTimeData = NULL;
			}
		}
		m_bRecordSampling = false;
	}
}

void MStatisticsProfiler::Init(bool bEnabled, bool bRecord)
{
	m_bEnabled = bEnabled;
	m_bRecordSampling = bRecord;

	Reset();
}

void MStatisticsProfiler::Reset()
{
	for(int i=0;i<MAX_PROFILE_COUNT;i++)
	{
		m_ProfileItems[i].szName[0]=0;
		m_ProfileItems[i].dwTotalTime= UINT_MAX;
		m_ProfileItems[i].dwMinTime = UINT_MAX;
		m_ProfileItems[i].dwMaxTime = 0;
		m_ProfileItems[i].dwCalledCount=0;

		if( m_bRecordSampling )
		{
			m_ProfileItems[i].arrayElapsedTimeData = new DWORD[ MAX_RECORD_SAMPLING ];
			m_ProfileItems[i].nElapsedTimeDataIndex = 0;
			m_ProfileItems[i].nElapsedTimeStandardDeviation = 0;
		}
		else
			m_ProfileItems[i].arrayElapsedTimeData = NULL;
	}

	m_dwEnableTime=GetNowTime();
	m_nMaxElapsedTimeDataIndex = 0;
}

void MStatisticsProfiler::BeginProfile(int nIndex, const char* szName)
{
	if (!m_bEnabled) return;
	if (nIndex<0 || nIndex>=MAX_PROFILE_COUNT)
	{
		_ASSERTE(false && "프로파일링 Index 오류");
	}

	if(m_ProfileItems[nIndex].szName[0]==0)
	{
		m_ProfileItems[nIndex].nIndex = nIndex;
		strcpy_s(m_ProfileItems[nIndex].szName,szName);
	}

	m_ProfileItems[nIndex].dwStartTime=GetNowTime();
	m_ProfileItems[nIndex].dwCalledCount++;
}

void MStatisticsProfiler::EndProfile(int nIndex)
{
	if (!m_bEnabled || nIndex<0 || nIndex>=MAX_PROFILE_COUNT) return;

	DWORD nNowTime = GetNowTime();
	DWORD nElapsedTime = nNowTime-m_ProfileItems[nIndex].dwStartTime;

	if (m_ProfileItems[nIndex].dwTotalTime == UINT_MAX) 
		m_ProfileItems[nIndex].dwTotalTime = 0;

	m_ProfileItems[nIndex].dwTotalTime+= nElapsedTime;

	if (nElapsedTime < m_ProfileItems[nIndex].dwMinTime) m_ProfileItems[nIndex].dwMinTime = nElapsedTime;
	if (nElapsedTime > m_ProfileItems[nIndex].dwMaxTime) m_ProfileItems[nIndex].dwMaxTime = nElapsedTime;

	if( m_bRecordSampling )
	{
		int& count = m_ProfileItems[ nIndex ].nElapsedTimeDataIndex;
		if( count < MAX_RECORD_SAMPLING )
		{
			_ASSERT( m_ProfileItems[ nIndex ].arrayElapsedTimeData != NULL && "레코드 가능하게 초기화 했는지 알아보세요");
			m_ProfileItems[ nIndex ].arrayElapsedTimeData[ count ] = nElapsedTime;
			count += 1;
		}
		m_nMaxElapsedTimeDataIndex = max(m_nMaxElapsedTimeDataIndex, count);
	}
}

void MStatisticsProfiler::AddLineForAnalysis( int nIndex )
{
	if(m_ProfileItems[nIndex].szName[0]==0)
	{
		m_setLineWhenAnalysis.insert(nIndex);
	}
}

bool SortProfileDataByTotalTimePercentage(const MStatisticsProfiler::MPROFILEITEM& c1, const MStatisticsProfiler::MPROFILEITEM& c2)
{
	return (c1.dwTotalTime > c2.dwTotalTime);
}

#include <algorithm>

void MStatisticsProfiler::FinalAnalysis(const char* filename)
{
	SaveSampling(filename);

	DWORD dwTotalTime = GetNowTime()-m_dwEnableTime;

	FILE *file=fopen(filename,"w+");

	double fTotalTime = (float)dwTotalTime*0.001f;

	fprintf(file," total time = %6.3f seconds \n",fTotalTime);

	PutLine(file);
	fputs("  ID  |       Total        |   Count (cps)          |       Average         |         Min        |         Max        |    SD    | Scope\n", file);
	PutLine(file);

	std::sort(m_ProfileItems, m_ProfileItems+MAX_PROFILE_COUNT, SortProfileDataByTotalTimePercentage);

	for(int i=0;i<MAX_PROFILE_COUNT;i++)
	{
		const MPROFILEITEM& item = m_ProfileItems[i];
		int nIndex = item.nIndex;
		if (m_setLineWhenAnalysis.find(nIndex) != m_setLineWhenAnalysis.end())
		{
			PutLine(file);
		}


		if (item.dwTotalTime==UINT_MAX) continue;

		double fItemTotalTime = item.dwTotalTime / 1000.0;
		double fTotalTimePercent = min(item.dwTotalTime * 100/(float)dwTotalTime, 100);
		DWORD nCount = item.dwCalledCount;
		double fCPS = (nCount / fItemTotalTime);
		double fCountPerTotalTime = (nCount / fTotalTime);
		double fAverageTime = (item.dwTotalTime/(double)nCount)/1000.0;
		double fMinTime = item.dwMinTime/1000.0f;
		double fMinTimePercent = min(item.dwMinTime*100/(double)dwTotalTime, 100);
		double fMaxTime = item.dwMaxTime/1000.0f;
		double fMaxTimePercent = min(item.dwMaxTime*100/(double)dwTotalTime, 100);
		double fStandardDeviation = (double)item.nElapsedTimeStandardDeviation;

		fprintf(file, " %4d | %8.3f (%6.2f%%) | %8d (%7.1f)   | %8.6f (%8.0f)    | %8.6f (%6.2f%%) | %8.6f (%6.2f%%) | %8.3f | %s\n",
			nIndex,
			fItemTotalTime,
			fTotalTimePercent,
			nCount,
			fCountPerTotalTime,
			fAverageTime,
			fCPS,
			fMinTime,
			fMinTimePercent,
			fMaxTime,
			fMaxTimePercent,
			fStandardDeviation,
			item.szName);
	}

	fputs("\nGenerated by MStatisticsProfiler ", file);
	fputs("ⓒ 1998-2010, MAIET entertainment, Inc. all rights reserved.\n", file);

	fclose(file);

	return;


}

void MStatisticsProfiler::PutLine( FILE * file )
{
	fputs("----------------------------------------------------------------------------------------------------------------------------------------\n", file);
}

void MStatisticsProfiler::ResetSampling()
{
	if ( !m_bRecordSampling ) return;

	for(int i=0;i<MAX_PROFILE_COUNT;i++)
	{
		memset(m_ProfileItems[i].arrayElapsedTimeData, 0, sizeof(DWORD) * MAX_RECORD_SAMPLING);
		m_ProfileItems[i].nElapsedTimeDataIndex = 0;
	}
	m_nMaxElapsedTimeDataIndex = 0;
}

void MStatisticsProfiler::SaveSampling( const char* filename )
{
	if( m_bRecordSampling )
	{
		char szSamplingFileName[_MAX_PATH];
		sprintf_s(szSamplingFileName, "%s.csv", filename);
		FILE* file=fopen(szSamplingFileName,"w");
		if (file == NULL) return;

		std::vector<MPROFILEITEM*> vecProfileItem;
		for(int i=0;i<MAX_PROFILE_COUNT;++i)
		{
			if (m_ProfileItems[i].dwTotalTime==UINT_MAX) continue;
			vecProfileItem.push_back(&m_ProfileItems[i]);

			fprintf(file, "\"(%d)%s\",", i, m_ProfileItems[i].szName);
		}

		fprintf(file, "\n");

		// 표준 편차 계산
		for (size_t i = 0; i < vecProfileItem.size(); i++)
		{
			MPROFILEITEM* pItem = vecProfileItem[i];
			int nSamplingDataCount = pItem->nElapsedTimeDataIndex;
			if (nSamplingDataCount == 0) continue;

			double nAvg = 0;
			for (int j = 0; j < nSamplingDataCount; j++)
			{
				nAvg += pItem->arrayElapsedTimeData[j];
			}
			nAvg /= nSamplingDataCount;

			double sum = 0;
			for (int j = 0; j < nSamplingDataCount; j++)
			{
				sum += ((pItem->arrayElapsedTimeData[j] - nAvg) * (pItem->arrayElapsedTimeData[j] - nAvg));
				sum = sum / nSamplingDataCount;
			}
			pItem->nElapsedTimeStandardDeviation = sqrt(sum);
		}

		fprintf(file, "표준편차(값이 클수록 비균일적임)\n");
		for (size_t i = 0; i < vecProfileItem.size(); i++)
		{
			MPROFILEITEM* pItem = vecProfileItem[i];
			fprintf(file, "%.3f,", (float)pItem->nElapsedTimeStandardDeviation);
		}
		fprintf(file, "\n\n");

		for (int i = 0; i < MAX_RECORD_SAMPLING; i++)
		{
			if (i >= m_nMaxElapsedTimeDataIndex) break;

			for (size_t j = 0; j < vecProfileItem.size(); j++)
			{
				MPROFILEITEM* pItem = vecProfileItem[j];

				if (i < pItem->nElapsedTimeDataIndex)
				{
					fprintf(file, "%u,", pItem->arrayElapsedTimeData[i]);
				}
				else
				{
					fprintf(file, ",");
				}
			}
			fprintf(file, "\n");
		}
		fclose(file);
	}
}

DWORD MStatisticsProfiler::GetTotalTimeByIndex(int nIndex)
{
	return m_ProfileItems[nIndex].dwTotalTime;
}

float MStatisticsProfiler::GetFrameByIndex(int nIndex)
{
	if (UINT_MAX == m_ProfileItems[nIndex].dwTotalTime) return 0.0f;

	return (float) m_ProfileItems[nIndex].dwCalledCount / ((float) m_ProfileItems[nIndex].dwTotalTime / 1000.0f);
}

const char* MStatisticsProfiler::GetName(int nIndex)
{
	return m_ProfileItems[nIndex].szName;
}


#endif

MTimeConstraint::MTimeConstraint( unsigned int ms, const char* szFileName/*=NULL*/, int nLine/*=0*/ ) : m_nStartTime(timeGetTime()), m_nMaxMs(ms), m_bAlreadyCheck(false)
{
	m_szFileName[0] = 0;
	m_nFileLine = 0;

	if (szFileName != NULL) 
	{
		strcpy_s(m_szFileName, szFileName);
		m_nFileLine = nLine;
	}
}

MTimeConstraint::~MTimeConstraint()
{
	Check(true);
}

bool MTimeConstraint::Check(bool bOutputLog)
{
	if (!m_bAlreadyCheck && GetElapsedTime() > m_nMaxMs) 
	{
		if (bOutputLog)
		{
			mlog("%s(%d) : 설정한 실행 시간(%ums)을 초과하였습니다. 경과 시간(%ums)\n", 
				m_szFileName, m_nFileLine, m_nMaxMs, timeGetTime() - m_nStartTime);
		}
		m_bAlreadyCheck = true;
		return true;
	}
	return false;
}

unsigned int MTimeConstraint::GetElapsedTime()
{
	return (timeGetTime() - m_nStartTime);
}