#pragma once

#include "MTime.h"			// for MRegulator
#include "CSWorldInfo.h"	// for GAME_TIME_TYPE, GAME_WEATHER_TYPE

struct SPAWN_INFO;
class GField;
class GEntityNPC;

class GSpawn : public MTestMemPool<GSpawn>
{
public:
	GSpawn();

	// 필드가 처음 실행될때 호출
	void Init(GField* pField, const SPAWN_INFO* pSpawnInfo);
	// 스폰정보 초기화
	void Reset();
	// 매틱마다 호출 (반환값은 스폰 됐는지 여부)
	bool Update(float fDelta);

	// 스폰된 NPC인지 확인
	bool IsSpawnedNPC(MUID uidNPC) const;

	// 스폰시킴
	GEntityNPC* Spawn();
	// 디스폰시킴
	GEntityNPC* Despawn();
	// 스폰의 NPC 인스턴스 반환
	GEntityNPC* GetNPCInstance();

	// 스폰의 활성화
	void Enable(bool bEnable);
	// 스폰의 활성화 여부
	bool IsEnable()	{ return m_bEnable; }

	// 필드의 시간이 바뀌면 콜백
	void OnTimeChangedDetail( int nTime );
	// 필드의 시간대가 바뀌면 콜백
	void OnTimeChanged( GAME_TIME_TYPE nOldTime, GAME_TIME_TYPE nNewTime );
	// 필드의 날씨가 바뀌면 콜백
	void OnWeatherChanged( GAME_WEATHER_TYPE nOldWeather, GAME_WEATHER_TYPE nNewWeather );

	// 다음에 리스폰할 시간 설정
	void SetRespawnTime(float fNextTime);

	int GetGroupID();
	
	// 스폰 NPC 업데이트 활성화 여부
	void ActivateUpdate();	
	bool IsActivateUpdate();

	const SPAWN_INFO* GetInfo() { return m_pSpawnInfo; }

private:
	// NPC 타입을 랜덤으로 고르기
	int PickNpcType();
	// 지형좌표 피킹
	void MakeSpawnPosition(const SPAWN_INFO* pSpawnInfo, vec3& pos, vec3& dir);

	// 다음에 리스폰할 시간 설정
	void SetRespawnTime();	

	// m_pSpawnInfo가 널인지 여부
	bool IsSpawnInfoNULL() { return (m_pSpawnInfo == NULL); }

	// 필드 시간대 조건이 맞는지 여부
	bool IsMatchTime();
	// 필드 시간 조건이 맞는지 여부
	bool IsMatchTimeDetail();
	// 필드 시간대 조건이 맞는지 여부
	bool IsMatchWeather();
	// 채집 아이템일 경우 필드 최대 인스턴스를 넘는지 여부
	bool IsGatherEntitySpawnEnabled();

private:
	// 스폰 레귤레이터
	MRegulator		m_rgrSpawn;
	// 스폰 정보
	const SPAWN_INFO*		m_pSpawnInfo;
	// 스폰이 위치된 필드 (피킹하기 위해 필요)
	GField*			m_pField;		
	// 스폰된 NPC UID
	MUID			m_uidNPC;		
	// 활성화 여부
	bool			m_bEnable;
	// 스폰 NPC 업데이트 활성화 여부
	bool			m_isActivateUpdate;
};
