#pragma once

#pragma push_macro("_SECURE_SCL")
#pragma push_macro("_HAS_ITERATOR_DEBUGGING")
#pragma warning ( disable : 4005)

#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0

#include "RLib.h"
#include <set>

///////////////////////////////////////////////////////////////////////////////////////////
// MStatisticsProfiler //////////

/// MProfiler보다 정밀한 프로파일러.
/// - _DO_NOT_USE_PROFILER 디파인이나 _PUBLISH 디파인이 걸려있으면 프로파일링을 하지 않는다.
class CML2_API MStatisticsProfiler
{
public:
	struct MPROFILEITEM 
	{
		int nIndex;
		char szName[256];
		DWORD dwStartTime;
		DWORD dwTotalTime;
		DWORD dwMinTime;
		DWORD dwMaxTime;
		DWORD dwCalledCount;
		DWORD* arrayElapsedTimeData;
		int		nElapsedTimeDataIndex;
		double	nElapsedTimeStandardDeviation;		// 경과 시간 표준 편차
	};
private:
	friend void MInitStatisticsProfile(bool bEnabled, bool _bRecordData);

	static const int	MAX_RECORD_SAMPLING = 10000;
	static const int	MAX_PROFILE_COUNT = 10000;

	bool				m_bEnabled;
	DWORD				m_dwEnableTime; // init으로 enable을 활성화해준 시각
	MPROFILEITEM		m_ProfileItems[MAX_PROFILE_COUNT];	
	DWORD				GetNowTime();
	std::set<int>		m_setLineWhenAnalysis;
	// by pok
	bool				m_bRecordSampling;
	int					m_nMaxElapsedTimeDataIndex;

	MStatisticsProfiler() : m_bEnabled(false), m_dwEnableTime(0), m_bRecordSampling(false) {}
	void PutLine( FILE * file );
	void SaveSampling( const char* filename );
public:
	~MStatisticsProfiler();

	void Init(bool bEnabled=true, bool bRecord = false);
	void Reset();
	void FinalAnalysis(const char* filename);

	void SetEnabled(bool bEnabled)	{ m_bEnabled = bEnabled; }
	void ResetSampling();		///< 경과 시간 샘플링 데이타 초기화

	void BeginProfile(int nIndex, const char* szName);
	void EndProfile(int nIndex);
	void AddLineForAnalysis(int nIndex);

	DWORD GetTotalTimeByIndex(int nIndex);
	float GetFrameByIndex(int nIndex);
	const char* GetName(int nIndex);
};
extern CML2_API MStatisticsProfiler*	g_pDefaultStatisticsProfiler;

inline void MInitStatisticsProfile(bool bEnabled=true, bool _bRecordData = false)
{
	if( g_pDefaultStatisticsProfiler == NULL )
		g_pDefaultStatisticsProfiler = new MStatisticsProfiler();

	g_pDefaultStatisticsProfiler->Init(bEnabled, _bRecordData);
}
inline void MFinalizeStatisticsProfile()
{
	if(g_pDefaultStatisticsProfiler)
	{
		delete g_pDefaultStatisticsProfiler;
		g_pDefaultStatisticsProfiler = NULL;
	}
}
inline void MResetStatisticsProfile()
{
	if( g_pDefaultStatisticsProfiler == NULL ) return;

	g_pDefaultStatisticsProfiler->Reset();
}

inline void MBeginStatisticsProfile(int nIndex, const char *szName)
{
	if( g_pDefaultStatisticsProfiler != NULL )
	{
		g_pDefaultStatisticsProfiler->BeginProfile(nIndex, szName);
	}
}
inline void MBeginStatisticsProfile(int nIndex, const char *szName, bool bLine)
{
	if( g_pDefaultStatisticsProfiler != NULL )
	{
		g_pDefaultStatisticsProfiler->BeginProfile(nIndex, szName);
		if (bLine) g_pDefaultStatisticsProfiler->AddLineForAnalysis(nIndex);
	}
}
inline void MEndStatisticsProfile(int nIndex)
{
	if( g_pDefaultStatisticsProfiler != NULL )
		g_pDefaultStatisticsProfiler->EndProfile(nIndex);
}
inline void MSaveStatisticsProfile(const char *filename, bool _bHalt = true)
{
	if( g_pDefaultStatisticsProfiler != NULL )
		g_pDefaultStatisticsProfiler->FinalAnalysis(filename);

	if( _bHalt )
		MFinalizeStatisticsProfile();
}
inline void MEnableStatisticsProfile(bool bEnabled)
{
	if( g_pDefaultStatisticsProfiler != NULL )
		g_pDefaultStatisticsProfiler->SetEnabled(bEnabled);
}

inline void MStatisticsProfileLine(int nIndex)
{
	if( g_pDefaultStatisticsProfiler != NULL )
		g_pDefaultStatisticsProfiler->AddLineForAnalysis(nIndex);
}

inline DWORD MStatisticsProfileGetTotalTimeByIndex(int nIndex)
{
#ifdef _USE_PROFILER
	if( g_pDefaultStatisticsProfiler != NULL )
		return g_pDefaultStatisticsProfiler->GetTotalTimeByIndex(nIndex);
#endif

	return 0;
}

inline float MStatisticsProfileGetFrameByIndex(int nIndex)
{
#ifdef _USE_PROFILER
	if( g_pDefaultStatisticsProfiler != NULL )
		return g_pDefaultStatisticsProfiler->GetFrameByIndex(nIndex);
#endif
	return 0;
}

inline const char* MStatisticsProfileGetProfileName(int nIndex)
{
	if( g_pDefaultStatisticsProfiler != NULL )
		return g_pDefaultStatisticsProfiler->GetName(nIndex);

	return NULL;
}

class MStatisticsProfilerScopeObj
{
private:
	MStatisticsProfilerScopeObj(const MStatisticsProfilerScopeObj& rhs);
	MStatisticsProfilerScopeObj &operator=(const MStatisticsProfilerScopeObj& rhs);
	int m_nIndex;
public:
	explicit MStatisticsProfilerScopeObj(int nIndex,const char *szName, bool bLine=false) : m_nIndex(nIndex)
	{
		MBeginStatisticsProfile(nIndex, szName, bLine);
	}
	~MStatisticsProfilerScopeObj()
	{
		MEndStatisticsProfile(m_nIndex);
	}
};


/// 해당 스코프에서 설정한 시간을 넘으면 로그 출력
class MTimeConstraint
{
private:
	void operator=(MTimeConstraint const&); 
	MTimeConstraint(MTimeConstraint const&);

	unsigned int const	m_nStartTime;
	unsigned int const	m_nMaxMs;
	char				m_szFileName[256];
	int					m_nFileLine;
	bool				m_bAlreadyCheck;
public:
	MTimeConstraint(unsigned int ms, const char* szFileName=NULL, int nLine=0);
	~MTimeConstraint();
	bool Check(bool bOutputLog=true);
	unsigned int GetElapsedTime();
};


#ifdef _USE_PROFILER

#define PFI_B(i, name)				MBeginStatisticsProfile(i, name)
#define PFI_B_LINE(i, name)			MBeginStatisticsProfile(i, name, true)
#define PFI_B_THISFUNC(i)			MBeginStatisticsProfile(i, __FUNCTION__)
#define PFI_B_THISFUNC_LINE(i)		MBeginStatisticsProfile(i, __FUNCTION__, true)
#define PFI_E(i)					MEndStatisticsProfile(i)

#define PFI_BLOCK(i, name)			MStatisticsProfilerScopeObj profiler_scope_obj(i, name)
#define PFI_BLOCK_LINE(i, name)		MStatisticsProfilerScopeObj profiler_scope_obj(i, name, true)
#define PFI_BLOCK_THISFUNC(i)		MStatisticsProfilerScopeObj profiler_scope_obj(i, __FUNCTION__ )
#define PFI_BLOCK_THISFUNC_LINE(i)	MStatisticsProfilerScopeObj profiler_scope_obj(i, __FUNCTION__ , true)

#define PFI_TIME_CONSTRAINT(ms)		MTimeConstraint profile__timeConstraint__(ms, __FILE__, __LINE__)
#define PFI_PROFILE_TIME_IS_OVER()	profile__timeConstraint__.Check(false)
#define PFI_TIME_ELAPSED()			profile__timeConstraint__.GetElapsedTime()

#else

#define PFI_B(i, name)				;
#define PFI_B_LINE(i, name)			;
#define PFI_B_THISFUNC(i)			;
#define PFI_B_THISFUNC_LINE(i)		;
#define PFI_E(i)					;

#define PFI_BLOCK(i, name)			;
#define PFI_BLOCK_LINE(i, name)		;
#define PFI_BLOCK_THISFUNC(i)		;
#define PFI_BLOCK_THISFUNC_LINE		;

#define PFI_TIME_CONSTRAINT(ms)		;
#define PFI_PROFILE_TIME_IS_OVER()	;
#define PFI_TIME_ELAPSED()			;

#endif

#define PFI_GT(i)					MStatisticsProfileGetTotalTimeByIndex(i)
#define PFI_GF(i)					MStatisticsProfileGetFrameByIndex(i)


#pragma pop_macro("_SECURE_SCL")
#pragma pop_macro("_HAS_ITERATOR_DEBUGGING")
