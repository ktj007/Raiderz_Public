#pragma once

enum QUESTPVP_EVENT_REWARD;
enum QUESTPVP_TEAM;

class GQPESpawnInfo : public MTestMemPool<GQPESpawnInfo>
{	
public:
	GQPESpawnInfo(void);	

public:
	SPAWN_ID	m_nSpawnID;
	float		m_fDelayTime;
	float		m_fDespawnTime;
};

class GQPEItemInfo : public MTestMemPool<GQPEItemInfo>
{	
public:
	GQPEItemInfo(void);
	
public:
	int						m_nItemID;
	int						m_nAmount;
};

class GQPEBeginInfo : public MTestMemPool<GQPEBeginInfo>
{	
public:
	~GQPEBeginInfo();

public:
	vector<wstring>			m_vecStrNotice;
	vector<GQPESpawnInfo*>	m_vecSpawnInfo;	
};

class GQPEEndInfo : public MTestMemPool<GQPEEndInfo>
{	
public:
	GQPEEndInfo();
	~GQPEEndInfo();

public:
	QUESTPVP_TEAM			m_nWinnerTeam;
	vector<wstring>			m_vecStrNotice;
	vector<GQPESpawnInfo*>	m_vecSpawnInfo;	
	vector<int>				m_vecDespawnNPCID;
	int						m_nMoney;
	int						m_nXP;
	vector<GQPEItemInfo*>	m_vecItemInfo;
};

class GQPEventInfo : public MTestMemPool<GQPEventInfo>
{	
public:
	GQPEventInfo(void);
	~GQPEventInfo(void);

	GQPEEndInfo* GetEndInfo(QUESTPVP_TEAM nWinnerTeam);

public:
	int						m_nID;
	wstring					m_strTitle;
	GQPEBeginInfo			m_BeginInfo;
	vector<GQPEEndInfo*>	m_vecEndInfo;
};