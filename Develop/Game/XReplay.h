#pragma once

#include "FileInfo.h"
#include "CSWorldInfo.h"

struct XObserverCommandItem 
{
	float fTime;
	MCommand *pCommand;
};

class XObserverCommandList : public list<XObserverCommandItem*> 
{
public:
	XObserverCommandList() {}
	~XObserverCommandList() { Destroy(); }
	void Destroy();
	void Assign(XObserverCommandList& t);	// 대상으로부터 복사
};

class XRecorder;
class XReplay;

#define REC_FILE_ID			0xa5c1308b
#define REC_FILE_VERSION	1


/// 리플레이 파일 - 이건 나중에 zip으로 되어야 한다.
class XReplayFile
{
private:
	FILE*		m_pFile;
	bool		m_bWrite;
public:
	XReplayFile();
	~XReplayFile();
	bool Open(const wchar_t* szFileName, bool bWrite=false);
	void Close();
	int Read(void* pBuffer,size_t nByte);
	int Write(void* pBuffer,size_t nByte);
};

/// 리플레이 저장하는 클래스
class XReplaySaver
{
private:
	XReplayFile		m_File;
	XRecorder*		m_pRecorder;
	wstring MakeAutoFileName();
	bool SaveHeader();
	bool SaveGameInfo();
	bool SaveCommandStream();
public:
	XReplaySaver(XRecorder* pRecorder);
	bool Save(const wchar_t* szFileName);
};

/// 리플레이 읽어오는 클래스
class XReplayLoader
{
private:
	XReplayFile				m_File;
	XReplay*				m_pReplay;
	bool LoadHeader();
	bool LoadGameInfo();
	bool LoadCommandStream();
	MCommand* CreateCommandFromStream(char* pStream);
public:
	XReplayLoader(XReplay* pReplay);
	bool Load(const wchar_t* szFileName);
};


/////////////////

/// 리플레이에서 사용하는 정보
struct XReplayGameInfo
{
	float						fStartTime;
	float						fFinishTime;
	int							nFieldID;
	GAME_TIME_TYPE				nGameTime;
	GAME_WEATHER_TYPE			nWeather;
	MUID						MyUID;
	UIID						nUIID;
	XObserverCommandList		ReplayCommandList;

	XReplayGameInfo() {	Clear();}
	~XReplayGameInfo(){	Clear();}
	void Clear()
	{
		fStartTime = 0.0f;
		fFinishTime = 0.0f;
		nFieldID = 0;
		nGameTime = TIME_DAYTIME;
		nWeather = WEATHER_SUNNY;

		MyUID.SetZero();
		nUIID = UIID_INVALID;

		while(ReplayCommandList.size())
		{
			XObserverCommandItem *pItem = *ReplayCommandList.begin();
			SAFE_DELETE(pItem->pCommand);
			SAFE_DELETE(pItem);
			ReplayCommandList.pop_front();
		}
		ReplayCommandList.clear();
	}
};

/// 레코더
class XRecorder
{
private:
	friend class XReplaySaver;
	XReplaySaver*			m_pSaver;

	bool					m_bRecording;
	wchar_t					m_szFileName[_MAX_PATH];
	XReplayGameInfo			m_GameInfo;		/// 게임 정보

	vector<TD_UPDATE_CACHE_PLAYER>		m_PlayerInfos;
	vector<TD_UPDATE_CACHE_NPC>			m_NPCInfos;
	vector<TD_UPDATE_CACHE_BUFFENTITY>	m_MagicAreaInfo;

	TD_MYINFO				m_MyInfo;
	vector<TD_ITEM>			m_MyInventory;
	vector<TD_ITEM>			m_MyEquipment;
	vector<TD_ITEM_SLOT_INFO> m_MyTDReservedDummySlotInfo;
	vector<int>				m_MyTalent;
	vector<TD_PLAYERQUEST>	m_MyQuest;
	vector<TD_PALETTE>		m_MyPalette;
	vector<TD_FACTION>		m_MyFaction;
	vector<int>				m_MyRecipeID;

	vector<TD_BUFF_REMAIN_TIME> m_MyBuffRemainTime;
	vector<TD_TALENT_REMAIN_COOLTIME> m_MyTalentRemainCoolTime;
	float					m_fMyAutoRebirthRemainTime;

	void RecordEntityInfoCommand();
	void RecordMyInfoCommand();
	bool PostFilterCommand( MCommand* pCommand );
	void InsertCommand( MCommand* pCommand );
	void SetupGameInfo();
	void SetupMyInfo();
	void SetupObjectsInfo();
	void SetupMyPalette();
public:
	XRecorder();
	~XRecorder();
	void Clear();
	bool Start(const wchar_t* filename=NULL);
	void Stop();
	void Record(MCommand* pCommand);
	bool IsRecording() { return m_bRecording; }
};

/// 리플레이 플레이어
class XReplay
{
private:
	friend class XReplayLoader;
	XReplayLoader*			m_pLoader;

	int						m_nVersion;
	double					m_fElapsedTime;
	bool					m_bDone;
	bool					m_bPause;
	double					m_Speed;

	XReplayGameInfo			m_GameInfo;
	XObserverCommandList	m_ReplayCommandList;		// 실제로 이것을 가지고 리플레이한다.

public:
	XReplay();
	~XReplay();
	bool Load(const wchar_t* szFileName);
	void Play();
	void Update(float fDelta);
	void SetPause(bool bPause);

	bool IsPlaying();
	bool IsDone() { return m_bDone; }
	float GetPlayTime()				{ return m_fElapsedTime; }
	float GetMaxTime()				{ return m_GameInfo.fFinishTime - m_GameInfo.fStartTime; }
	void SetSpeed(double speed)		{ m_Speed = speed; }
	double GetSpeed()				{ return m_Speed; }
	XReplayGameInfo& GetGameInfo()	{ return m_GameInfo; }
};

