#pragma once

/// 월드 정보
class XGlobalVar_World
{
protected:
	int							m_nFieldID;		///< 필드 ID
	int							m_nIntroCutsceneID;///< 인트로 컷씬 ID

	bool						m_bNextMapIsSameZone;
	vec3						m_vNextMapStartPos;		///< 진입할 맵의 시작 위치	
	vec3						m_vNextMapStartDir;		///< 진입할 맵의 시작 방향	
public:

	int							nChannelID;		///< 채널 ID
	vector< pair< int, int>>	ChannelList;	///< 채널 리스트
	GAME_TIME_TYPE				nGameTime;
	GAME_WEATHER_TYPE			nWeather;
	uint32						nDynamicFieldExpiredTime;

	// 진입할 월드 정보
	void SetNextMapInfo(vec3 vPos, vec3 vDir)
	{
		m_vNextMapStartPos = vPos;
		m_vNextMapStartDir = vDir;
	}
	void SetNextMapInfo(int nFieldID, vec3 vPos, vec3 vDir)
	{
		m_nFieldID = nFieldID;
		m_vNextMapStartPos = vPos;
		m_vNextMapStartDir = vDir;
	}
	void SetFieldID(int nFieldID)
	{
		m_nFieldID = nFieldID;
	}
	void SetIntroCutSceneID(int nIntroCutsceneID)
	{
		m_nIntroCutsceneID = nIntroCutsceneID;
	}

	XGlobalVar_World()
	{
		m_nFieldID = -1;
		nChannelID = -1;
		m_nIntroCutsceneID = -1;
		nGameTime = TIME_DAYTIME;
		nWeather = WEATHER_SUNNY;
		nDynamicFieldExpiredTime = 0;
		m_bNextMapIsSameZone = false;

		m_vNextMapStartPos=vec3(0,0,0);
		m_vNextMapStartDir=vec3(0,-1,0);
	}

	vec3 GetNextMapStartPos()	{ return m_vNextMapStartPos; }
	vec3 GetNextMapStartDir()	{ return m_vNextMapStartDir; }
	int GetFieldID()			{ return m_nFieldID; }
	int GetIntroCutSceneID()			{ return m_nIntroCutsceneID; }
};
