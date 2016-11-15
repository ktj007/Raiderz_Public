#include "stdafx.h"
#include "GSpawn.h"
#include "GField.h"
#include "GFieldInfo.h"		// for SPAWN_INFO
#include "GDef.h"
#include "GEntityNPC.h"
#include "GEntityMgr.h"

GSpawn::GSpawn()
: m_pField(NULL)
, m_pSpawnInfo(NULL)
, m_bEnable(true)
, m_isActivateUpdate(true)
{

}

void GSpawn::Init( GField* pField, const SPAWN_INFO* pSpawnInfo )
{
	VALID(pField);
	m_pField = pField;
	m_pSpawnInfo = pSpawnInfo;

	float fStartTime = -1.0f;
	VALID (pSpawnInfo);
	m_bEnable = pSpawnInfo->bEnable;
	fStartTime = (float)pSpawnInfo->nStartTime;
	VALID (fStartTime >= 0.0f);

	Enable(m_bEnable);
	SetRespawnTime(fStartTime);

	
	// 그룹ID가 설정되어 있으면 기본 업데이트는 중지상태로 시작
	if (pSpawnInfo->nGroupID != INVALID_SPAWN_GROUP_ID)
	{
		m_isActivateUpdate = false;
	}
}

void GSpawn::Reset()
{
	SetRespawnTime();
	m_uidNPC.SetInvalid();
	m_rgrSpawn.Start();
}

bool GSpawn::Update( float fDelta )
{
	if (!m_bEnable)						
		return false;	// 비활성화 상태

	if (IsSpawnInfoNULL())
		return false;

	if (!IsMatchTime())
		return false; // 시간대 조건이 맞지 않음

	if (!IsMatchTimeDetail())
		return false; // 시간 조건이 맞지 않음

	if (!IsMatchWeather())
		return false;	// 날씨 조건이 맞지 않음

	if (!m_rgrSpawn.IsReady(fDelta))	
		return false;	// 리스폰 필요시간이 충분치 않음

	if (!IsGatherEntitySpawnEnabled())
		return false; // 필드 최대 채집 인스턴스 갯수를 넘었다.

	return Spawn() != NULL;
}

GEntityNPC* GSpawn::Spawn()
{
	if (m_uidNPC.IsValid())	
		return NULL;	// 이미 스폰되어있음

	VALID_RET(m_pField, NULL);
	VALID_RET(m_pField->GetInfo(), NULL);

	vec3 vPos, vDir;
	MakeSpawnPosition(m_pSpawnInfo, vPos, vDir);

	GEntityNPC* pNPC = m_pField->SpawnNPC(PickNpcType(), m_pSpawnInfo, vPos, vDir);
	if (!pNPC) 
	{
		Enable(false);
		return NULL;
	}

	m_uidNPC = pNPC->GetUID();
	m_rgrSpawn.Stop();
	
	pNPC->SetActivateUpdate(m_isActivateUpdate);
	pNPC->SetSpawnID(m_pSpawnInfo->nID);

	return pNPC;
}

GEntityNPC* GSpawn::Despawn()
{
	if (m_uidNPC.IsInvalid())	return NULL;	// 스폰되어있지 않음

	GEntityNPC* pNPC = GetNPCInstance();
	VALID_RET(pNPC, NULL);
	pNPC->DeleteMe();
	
	Reset();

	return pNPC;
}

GEntityNPC* GSpawn::GetNPCInstance()
{
	return m_pField->FindNPC(m_uidNPC);
}

void GSpawn::Enable( bool bEnable )
{
	if (bEnable)
	{
		m_bEnable = true;
		m_rgrSpawn.Start();
	}
	else
	{
		m_bEnable = false;
		m_rgrSpawn.Stop();
	}

	SetRespawnTime(0.0f);	// 리스폰타임 초기화
}

void GSpawn::OnTimeChangedDetail( int nTime )
{
	if (!m_pSpawnInfo)						return;
	if (!m_bEnable)							return;
	if (m_pSpawnInfo->pairEnableTime.first == -1)		return;
	if (m_pSpawnInfo->pairEnableTime.second == -1)		return;

	if (m_pSpawnInfo->pairEnableTime.first <= nTime &&
		m_pSpawnInfo->pairEnableTime.second >= nTime)
	{
		Spawn();
	}
	else
	{
		Despawn();
	}
}

void GSpawn::OnTimeChanged( GAME_TIME_TYPE nOldTime, GAME_TIME_TYPE nNewTime )
{
	if (!m_pSpawnInfo)						return;
	if (!m_pSpawnInfo->bUseConditionTime)	return;
	if (!m_bEnable)							return;

	if (m_pSpawnInfo->bEnableTimeList[nNewTime])
	{
		Spawn();
	}
	else
	{
		Despawn();
	}
}

void GSpawn::OnWeatherChanged( GAME_WEATHER_TYPE nOldWeather, GAME_WEATHER_TYPE nNewWeather )
{
	if (!m_pSpawnInfo)							return;
	if (!m_pSpawnInfo->bUseConditionWeather)	return;
	if (!m_bEnable)								return;
	

	if (m_pSpawnInfo->bEnableWeatherList[nNewWeather])
	{
		Spawn();
	}
	else
	{
		Despawn();
	}
}

int GSpawn::PickNpcType()
{
	int nPickID = INVALID_NPC_ID;
	int nPickValue = RandomNumber(0, MAX_SPAWN_RATE);

	for (size_t i=0; i<m_pSpawnInfo->vecNPCInfoList.size(); i++)
	{
		const SPAWN_NPC_INFO& info = m_pSpawnInfo->vecNPCInfoList[i];

		if (nPickValue <= info.nPickRate)
		{
			// 당첨
			nPickID = info.nID;
			break;
		}

		nPickValue -= ((long long)(nPickValue-info.nPickRate)<0)?0:info.nPickRate;
	}	

	return nPickID;
}

void GSpawn::MakeSpawnPosition(const SPAWN_INFO* m_pSpawnInfo, vec3& pos, vec3& dir)
{
	VALID(m_pSpawnInfo);

	if (m_pSpawnInfo->fRadius == 0.0f)
	{
		pos = m_pSpawnInfo->vPoint;
	}
	else
	{
		float fRadius = RandomNumber(0.0f, MMath::PI*2.0f);
		float fDistance = RandomNumber(0.0f, m_pSpawnInfo->fRadius);

		MVector2 vTarPos = MMath::GetArcPosition(fRadius, fDistance);

		pos.x = m_pSpawnInfo->vPoint.x + vTarPos.x;
		pos.y = m_pSpawnInfo->vPoint.y + vTarPos.y;
		pos.z = m_pSpawnInfo->vPoint.z;
	}

	if (m_pSpawnInfo->bRandomDir)
	{
		dir = vec3(RandomNumber(-1.0f, 1.0f), RandomNumber(-1.0f, 1.0f), 0); 
	}
	else
	{
		dir = m_pSpawnInfo->vDir;
	}
	dir.Normalize();
}

void GSpawn::SetRespawnTime()
{
	float fRespawnTime = 0.0f;
	if (m_pSpawnInfo)
	{
		fRespawnTime = (float)m_pSpawnInfo->nRespawnTime;
	}

	SetRespawnTime(fRespawnTime);
}

void GSpawn::SetRespawnTime( float fNextTime )
{
	m_rgrSpawn.SetTickTime(fNextTime);
}

bool GSpawn::IsMatchTime()
{
	VALID_RET(m_pField, false);
	if (!m_pSpawnInfo->bUseConditionTime)	return true;
		
	return m_pSpawnInfo->bEnableTimeList[m_pField->GetCurrentTime()];
}

bool GSpawn::IsMatchTimeDetail()
{
	VALID_RET(m_pField, false);
	if (m_pSpawnInfo->pairEnableTime.first == -1)	return true;
	if (m_pSpawnInfo->pairEnableTime.second == -1)	return true;

	if (m_pField->GetInfo() && m_pField->GetInfo()->m_TimeInfo.bFixed)
	{
		return true;
	}

	int nTime = m_pField->GetCurrentTimeDetail();
	return (m_pSpawnInfo->pairEnableTime.first <= nTime &&
			m_pSpawnInfo->pairEnableTime.second >= nTime);
}

bool GSpawn::IsMatchWeather()
{
	VALID_RET(m_pField, false);
	if (!m_pSpawnInfo->bUseConditionWeather)	return true;

	return m_pSpawnInfo->bEnableWeatherList[m_pField->GetCurrentWeather()];
}

bool GSpawn::IsSpawnedNPC( MUID uidNPC ) const
{
	return m_uidNPC == uidNPC;
}

bool GSpawn::IsGatherEntitySpawnEnabled()
{
	if (m_pSpawnInfo->nGatherType != GATHER_NONE)
	{
		int nMaxGatherInstance = m_pField->GetInfo()->m_nMaxGatherInstance[m_pSpawnInfo->nGatherType];
		int nCurrInstance = m_pField->GetEntityMgr()->GetGatherEntityCount(m_pSpawnInfo->nGatherType);

		if (nMaxGatherInstance != 0 && nCurrInstance >= nMaxGatherInstance)
		{
			return false;
		}
	}

	return true;
}

bool GSpawn::IsActivateUpdate()
{
	return m_isActivateUpdate;
}

void GSpawn::ActivateUpdate()
{
	m_isActivateUpdate = true;
}

int GSpawn::GetGroupID()
{
	if (m_pSpawnInfo == NULL)	return INVALID_SPAWN_GROUP_ID;

	return m_pSpawnInfo->nGroupID;
}
