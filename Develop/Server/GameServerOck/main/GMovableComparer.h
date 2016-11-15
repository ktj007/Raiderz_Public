#pragma once

class GEntityPlayer;

class GMovableComparer
{
	struct Record
	{
		MPoint ptSector;
		int nNPCQty;
		int nPlayerQty;
		vec3 vPos;
	};
public:
	GMovableComparer(GEntityPlayer* pPlayer);
	~GMovableComparer(void);

	void SaveTimeSlice(const wchar_t* pszName);

private:
	void Start();
	void Stop();
	void Report( float fElapsedTime, const Record& recordOld, const Record& recordNew );
	void CreateRecord(Record& record);
private:
	MTimeChecker2 m_timerTotal;
	GEntityPlayer* m_pPlayer;
	Record m_recordOld;
	vector<pair<float,const wchar_t*>> m_vecSlices;
	float m_fPrevElapsed;
	static float s_fWorstTime;
};
