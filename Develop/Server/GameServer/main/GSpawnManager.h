#ifndef _GSPAWN_H
#define _GSPAWN_H

#include "MMemPool.h"
#include "CSWorldInfo.h"
#include "CSTriggerInfo.h"


class GSpawn;
class GField;
class GNPCInfo;
class GEntityNPC;
class GSpawnDelay;
class GDespawnAuto;
struct SPAWN_INFO;

class GSpawnManager : public MMemPool<GSpawnManager>
{
public:

public:
				GSpawnManager(GField* pField);
	virtual		~GSpawnManager();

	void		Init();
	void		Fini();
	void		AddSpawn(const SPAWN_ID nID, const SPAWN_INFO* pSpawnInfo);

	void		Update(float fDelta);
	void		Update_Spawn(float fDelta);
	void		Update_SpawnDelay(float fDelta);
	void		Update_DespawnAuto(float fDelta);

	// NPC가 죽었을 때 호출
	void		OnNPCDie(SPAWN_ID nID);

	// SPAWN_ID로 스폰
	GEntityNPC* Spawn(SPAWN_ID nSpawnID, float fSpawnDelayTime=0.0f, float fDespawnTime=0.0f);
	// NPC ID로 스폰
	GEntityNPC*	Spawn(int nNPCID, const vec3& vPos, const vec3& vDir, float fSpawnDelayTime=0.0f, float fDespawnTime=0.0f, SpawnDetailInfo* pDetailInfo=NULL);
	
	// 디스폰시킴
	GEntityNPC* Despawn(SPAWN_ID nSpawnID, bool bSpawnEnable=true);
	// 스폰정보 리셋
	void		Reset(SPAWN_ID nSpawnID);
	// 스폰의 NPC 인스턴스 반환
	GEntityNPC* GetNPCInstance(SPAWN_ID nSpawnID);
	// 활성화여부 설정
	void		Enable(SPAWN_ID nSpawnID, bool bSwitch);
	// 활성화여부 반환
	bool		IsEnable(SPAWN_ID nSpawnID);

	// 필드의 시간이 바뀌면 콜백
	void OnTimeChangedDetail( int nTime );
	// 필드의 시간대가 바뀌면 콜백
	void OnTimeChanged( GAME_TIME_TYPE nOldTime, GAME_TIME_TYPE nNewTime );
	// 필드의 날씨가 바뀌면 콜백
	void OnWeatherChanged( GAME_WEATHER_TYPE nOldWeather, GAME_WEATHER_TYPE nNewWeather );

	// 일정 시간뒤에 스폰시킬게 없는지
	bool		IsSpawnDelayEmpty();

	// 스폰된 NPC인지 확인
	bool IsSpawnedNPC( SPAWN_ID nSpawnID, MUID uidNPC );
	
	void		ActivateSpawnGroup(int nGroupID);

	const SPAWN_INFO* GetSpawnInfo( unsigned long nSpawnID );

private:
	GSpawn*		NewSpawn();
	void		AddSpawnDelay(int nNPCID, const vec3& vPos, const vec3& vDir, float fDelayTime);
	void		AddSpawnDelay(SPAWN_ID nSpawnID, float fDelayTime);
	void		AddDespawnAuto(MUID uidNPC, float fDespawnTime);
	void		AddDespawnAuto(SPAWN_ID nSpawnID, float fDespawnTime);
	GSpawn*		Find(SPAWN_ID nSpawnID);	

private:
	typedef map<SPAWN_ID, GSpawn*>	SPAWN_MAP;
	typedef list<GSpawnDelay*>		SPAWNDELAY_LIST;
	typedef list<GDespawnAuto*>		DESPAWNAUTO_LIST;
	SPAWN_MAP					m_mapSpawns;
	SPAWNDELAY_LIST				m_listSpawnDelays;
	DESPAWNAUTO_LIST			m_listDespawnAuto;
	GField*						m_pField;
	vector<GSpawn*>				m_vecUnspawneds;
};


#endif