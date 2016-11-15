#pragma once

#include "XNPCInfo.h"

struct XDummyAgentInfo;
class XBirdClient;
struct TD_UPDATE_CACHE_NPC;

class XNPCForDummy
{
public:
	MUID UID;
	UIID nUIID;
	vec3 vPos;
	uint32 nLastLiveTime;
	bool bDead;
	XNPCInfo* pNPCInfo;
	XNPCForDummy();
	void Touch();
};

class XDummySpawnMgr
{
public:
	XDummySpawnMgr(XDummyAgentInfo* pAgentInfo, bool bOptimize=true);

	void Update(float fDelta, XBirdClient* pClient, vec3 vPosition);

	minet::MCommandResult OnCommand(MCommand* pCommand);
	
	XNPCForDummy* GetNPC(UIID nUIID);
	XNPCForDummy* GetNPC(MUID nUID);
	XNPCForDummy* GetNearestNPC(vec3 vPos, bool bDead=false, int nFactionID=0);
	XNPCForDummy* GetRandomNearestNPC(vec3 vPos, bool bDead=false, int nFactionID=0);
	void InsertNPC(XNPCForDummy* pNewNPC);
	void DeleteNPC(UIID nUIID);

private:
	void Init(XDummyAgentInfo* pAgentInfo);
	void Spawn(float fDelta, XBirdClient* pClient, vec3 vPosition);
	void RemoveInvalidNPC(float fDelta, vec3 vCurrPos);	
	int RandomMonsterID();

private:
	bool			m_bAutoSpawn;
	vector<int>		m_vecNPCID;
	float			m_fSpawnTick;
	int				m_nMaxSpawn;

	MRegulator					m_SpawnRegulator;
	MRegulator					m_RemoveInvalidNPCRegulator;

	typedef map<UIID, XNPCForDummy*>	XNPCForDummyMap;

	map<UIID, XNPCForDummy*>	m_mapNPCForDummy;
	bool						m_bOptimize;
	
};