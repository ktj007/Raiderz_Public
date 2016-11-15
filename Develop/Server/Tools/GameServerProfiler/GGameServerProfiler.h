#pragma once

// MProfiler2의 AutoTest용 버젼
//	만든이유: 추가 인스턴스로 MPROFILEITEM를 10000개 더 만들었더니 스택오버플로우 발생 -_-;

class GGameServerProfiler
{
private:
	friend void MInitProfile(bool bEnabled, bool _bRecordData);

	static const int	MAX_RECORD_SAMPLING = 10000;
	static const int	MAX_PROFILE_COUNT = 10000;

	struct MPROFILEITEM 
	{
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

	bool				m_bEnabled;
	DWORD				m_dwEnableTime; // init으로 enable을 활성화해준 시각
	vector<MPROFILEITEM>	m_vecProfileImtems;
	std::set<int>		m_setLineWhenAnalysis;
	// by pok
	bool				m_bRecordSampling;
	int					m_nMaxElapsedTimeDataIndex;

	DWORD				GetNowTime();
	void PutLine( FILE * file );
	void SaveSampling( const char* filename );
public:
	GGameServerProfiler();
	~GGameServerProfiler();

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
};
