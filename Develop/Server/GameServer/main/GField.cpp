#include "stdafx.h"
#include "GField.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "CCommandTable.h"
#include "GSpawnManager.h"
#include "GNPCMgr.h"
#include "GNPCInfoMgr.h"
#include "GPathFinder.h"
#include "GCollisionTree.h"
#include "GConst.h"
#include "GWeatherMgr.h"
#include "GServer.h"
#include "GCommand.h"
#include "GScriptGroup_Field.h"
#include "GNullGlueField.h"
#include "GGlueField.h"
#include "GFieldSystem.h"
#include "GFieldInfoMgr.h"
#include "GFieldAIEventInvoker.h"
#include "GNPCInteraction.h"
#include "GPlayerFieldDynamic.h"
#include "GTestSystem.h"
#include "GNPCMinion.h"
#include "GConditionsSystem.h"
#include "GFieldSensor.h"
#include "GPickingZTable.h"
#include "GFieldGrid.h"
#include "CSNPCInfoHelper.h"
#include "GCommandCenter.h"
#include "GNPCRewardOwner.h"
#include "GQPTeamCountSender.h"
#include "GQuestPVP.h"
#include "GNPCRewarder.h"
#include "GDuelMgr.h"
#include "GFieldNPCSessionMgr.h"
#include "GCombatMonitorPort.h"
#include "GQPKillRewarder.h"
#include "GQPKillRewardOwner.h"
#include "GHateTable.h"
#include "GLootSystem.h"
#include "GLootModifyHandler.h"
#include "GEntityBPart.h"
#include "GNPCLoot.h"
#include "GPartySystem.h"

GField::GField()
: m_pDelegator(NULL)
, m_pPathMesh(NULL)
{
	_ASSERT(0 && L"Glue에 바인딩 하기 위한 생성자. 사용하지 마세요.");
}

GField::GField(const MUID& uid, GWeatherMgr* pWeatherMgr) 
: m_UID(uid)
, m_pInfo(NULL)
, m_nLastUpdateChangeField(0)
, m_bRegisteredManager(false)
, m_bDeleteMe(false)
, m_pWeatherMgr(pWeatherMgr)
, m_nFatigueMod(1)
, m_pDelegator(NULL)
, m_pFieldSensor(new GFieldSensor())
, m_pPathMesh(NULL)
, m_pFieldGrid(NULL)
, m_pQuestPVP(NULL)
, m_pNPCSession(NULL)
, m_pCombatMonitorPort(NULL)
{
	m_pEntityMgr = new GEntityMgr();
	m_pEntityMgr->Create();

	m_pSpawn = new GSpawnManager(this);

	m_rSensorUpdate.SetTickTime(0.1f);

	m_pAILodEventInvoker = new GFieldAIEventInvoker(this);

	m_pFieldGrid = new GFieldGrid();
	m_pQuestPVP = new GQuestPVP(this);
	m_pDuelMgr = new GDuelMgr(this);
	m_pNPCSession = new GFieldNPCSessionMgr();
}

GField::~GField()
{
	DetachCombatMonitor();

	SAFE_DELETE(m_pFieldSensor);
	SAFE_DELETE(m_pAILodEventInvoker);
	SAFE_DELETE(m_pDelegator);
	SAFE_DELETE(m_pEntityMgr);
	SAFE_DELETE(m_pSpawn);
	SAFE_DELETE(m_pFieldGrid);
	SAFE_DELETE(m_pQuestPVP);
	SAFE_DELETE(m_pDuelMgr);
	SAFE_DELETE(m_pNPCSession);
}

void GField::Update(float fDelta)
{
	PFI_B(81, "GField::Update - GEntityMgr:Update");
	m_pEntityMgr->Update(fDelta);
	PFI_E(81);


	PFI_B(82, "GField::Update - GSpawnManager:Update");
	if (m_bRegisteredManager)
		m_pSpawn->Update(fDelta);
	PFI_E(82);

	m_PathContextGroup.Update(fDelta);
	m_pQuestPVP->Update(fDelta);

	UpdateTimer(fDelta);
	m_pDuelMgr->Update(fDelta);
	m_pNPCSession->Update(fDelta);
}

const MUID GField::GetUID(void) const
{
	return m_UID;
}

const int GField::GetID() const
{
	if (!m_pInfo)	return -1;
	return m_pInfo->m_nFieldID;
}

int GField::GetZoneID()
{
	if (!m_pInfo)	return -1;
	return m_pInfo->m_nZoneID;
}

const MARKER_INFO* GField::FindMarkerInfo(unsigned long nID) const
{
	if (!m_pInfo)	return NULL;
	return m_pInfo->FindMarker(nID);
}

GEntity* GField::FindEntity(const MUID& uidEntity) const
{
	return m_pEntityMgr->FindEntity(uidEntity);
}

GEntityPlayer* GField::FindPlayer(const MUID& uidPlayer) const
{
	if (!m_pEntityMgr)
		return NULL;

	return m_pEntityMgr->FindPlayer(uidPlayer);
}

GEntityPlayer* GField::FindPlayerByCID(int nCID) const
{
	MAP_PLYAERCID::const_iterator itor = m_mapPlayerCID.find(nCID);
	if (m_mapPlayerCID.end() == itor) return NULL;

	return (*itor).second;
}

GEntityNPC* GField::FindNPC(const MUID& uidNPC) const
{
	return m_pEntityMgr->FindNPC(uidNPC);
}

GEntityNPC* GField::FindNPC(const UIID nNPCUIID) const
{
	return m_pEntityMgr->FindNPC(nNPCUIID);
}

vector<GEntityNPC*> GField::FindNPCByID( int nNPCID ) const
{
	return m_pEntityMgr->FindNPCByID(nNPCID);
}

GEntityNPC* GField::FindOneNPCByID(int nNPCID) const
{
	return m_pEntityMgr->FindOneNPCByID(nNPCID);
}

GBuffEntity* GField::FindBuffEntity(const MUID& uidMagicArea) const
{
	return m_pEntityMgr->FindBuffEntity(uidMagicArea);
}

GEntityActor* GField::FindActor(const MUID& uidActor) const
{
	return m_pEntityMgr->FindActor(uidActor);
}

void GField::GetNeighborNPC(const vec3& vPos, const float fRange, vector<GEntityNPC*>& vecOutNeighborNPC )
{
	m_pEntityMgr->GetNeighborNPC(vPos, fRange, vecOutNeighborNPC );
}

int GField::GetEntityQty(ENTITY_TYPEID nType)
{
	return m_pEntityMgr->GetEntityQty(nType);
}

int GField::GetPlayerQty()
{
	return GetEntityQty(ETID_PLAYER)+static_cast<int>(m_setReservedEnterPlayers.size());
}

int GField::GetNPCQty(int nNPCID)
{
	return m_pEntityMgr->GetLiveNPCCount(nNPCID);
}

GEntityMgr::ENTITY_UID_MAP& GField::GetEntityMap()
{ 
	return m_pEntityMgr->GetEntityMap(); 
}

bool GField::Create(const GFieldInfo* pFieldInfo)
{
	PFI_BLOCK_THISFUNC(2504);

	VALID_RET(pFieldInfo, false);
	VALID_RET(!m_pInfo, false);

	m_pInfo = pFieldInfo;	
	
	m_pSpawn->Init();
	m_PathContextGroup.Init(m_pInfo->m_nZoneID);
	m_pFieldGrid->Init(GetUID(), m_pInfo);
	InitSensor();

	CreateDelegator();

	m_pPathMesh = gsys.pPathFinder->GetMesh(m_pInfo->m_nZoneID);

	return true;
}

bool GField::Create(int nFieldID)
{
	PFI_BLOCK_THISFUNC(2504);
	VALID_RET(!m_pInfo, false);

	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	Create(pFieldInfo);

	return true;
}

void GField::OnCreated()
{
	Luacallback_OnCreate();
}

void GField::Destroy()
{
	m_pEntityMgr->Destroy();

	Luacallback_OnDestroy();
}

void GField::Luacallback_OnCreate()
{
	GetScriptField()->OnCreate(GetDelegator());
}

void GField::Luacallback_OnDestroy()
{
	GetScriptField()->OnDestroy(GetDelegator());
}

void GField::InitSensor()
{
	assert(m_pInfo);
	if (!m_pInfo)		return;

	m_setEnableSensor.clear();

	for each (const SENSOR_INFO_MAP_BY_ID::value_type& data in m_pInfo->GetSensorInfoContainer())
	{
		const int& nID = data.first;
		const SENSOR_INFO& sensor = data.second;
		if (sensor.bStartingEnable)
		{
			EnableSensor(nID, true);
		}
	}
}

void GField::ResetTimer()
{
	m_ScriptTimer.Clear();
}

bool GField::IsEnableSensor(unsigned long nSensorID) const
{
	assert(GetInfo());
	if (!GetInfo())									return false;

	const SENSOR_INFO* pSensor = GetInfo()->FindSensor(nSensorID);
	if (!pSensor)									return false;

	set<int>::const_iterator iter = m_setEnableSensor.find(nSensorID);
	
	return iter != m_setEnableSensor.end();
}

bool GField::EnableSensor(unsigned long nSensorID, bool bEnable)
{
	assert(GetInfo());
	if (!GetInfo())									return false;

	const SENSOR_INFO* pSensor = GetInfo()->FindSensor(nSensorID);
	if (!pSensor)									return false;

	set<int>::const_iterator iter = m_setEnableSensor.find(nSensorID);

	bool bChanged = false;
	if (!bChanged && 
		bEnable && 
		iter == m_setEnableSensor.end())
	{
		m_setEnableSensor.insert(nSensorID);

		bChanged = true;
	}

	if (!bChanged && 
		!bEnable && 
		iter != m_setEnableSensor.end())
	{
		m_setEnableSensor.erase(nSensorID);

		vector<GEntityActor*> vecActors = m_pEntityMgr->GetActors();
		for each (GEntityActor* pActor in vecActors)
		{
			pActor->RemoveEnteredSensorID(nSensorID);
		}

		bChanged = true;
	}
	// 이미 같은 형태이다.
	if (!bChanged)				return false;

	UpdateUsableSensor(pSensor);

	return true;
}

const SENSOR_INFO* GField::GetSensor( unsigned long nSensorID )
{
	return GetInfo()->FindSensor(nSensorID);
}

void GField::AddEntity(GEntity* pEntity)
{
	m_pEntityMgr->AddEntity(pEntity);
	
	if (pEntity->IsPlayer())
	{
		GEntityPlayer *pEntityPlayer = static_cast<GEntityPlayer*>(pEntity);

		CancelPreservedEnter(pEntityPlayer);

		m_mapPlayerCID.insert(MAP_PLYAERCID::value_type(pEntityPlayer->GetCID(), pEntityPlayer));		
		m_pAILodEventInvoker->EnterPlayer();
	}
	else if(pEntity->IsNPC())
	{
		gmgr.pNPCMgr->AddNPC(ToEntityNPC(pEntity));
	}

	OnAddEntity(pEntity);
}

void GField::RemoveEntity(GEntity* pEntity)
{
	VALID(pEntity);
	
	OnRemoveEntity(pEntity);

	m_PathContextGroup.OnRemoveEntity(pEntity);
	
	if (pEntity->IsPlayer())
	{
		GEntityPlayer *pEntityPlayer = static_cast<GEntityPlayer*>(pEntity);

		if (pEntityPlayer->GetPlayerInfo())
		{
			RemovePlayerCID(pEntityPlayer->GetCID());
			m_pAILodEventInvoker->LeavePlayer();
		}
	}
	else if (pEntity->IsNPC())
	{
		gmgr.pNPCMgr->RemoveNPC(ToEntityNPC(pEntity));
	}

	m_pEntityMgr->RemoveEntity(pEntity, false);
}

void GField::ReservedEnter(GEntityPlayer* pPlayer, vec3 vecDestPos, vec3 vecDestDir)
{
	VALID(pPlayer);

	pPlayer->LeaveFromField();
	m_setReservedEnterPlayers.insert(pPlayer->GetUID());
	pPlayer->GetPlayerField().GetFieldEntry().ReserveWarp(GetUID(), vecDestPos, vecDestDir); 
}

void GField::CancelPreservedEnter(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	m_setReservedEnterPlayers.erase(pPlayer->GetUID());
	pPlayer->GetPlayerField().GetFieldEntry().ClearReserveWarp();
}

void GField::RemovePlayerCID(int nCID)
{
	MAP_PLYAERCID::iterator itor = m_mapPlayerCID.find(nCID);
	if (m_mapPlayerCID.end() != itor)
	{
		m_mapPlayerCID.erase(itor);
	}
}

MUID GField::NewUID()
{
	return gsys.pServer->NewUID();
}

GEntityNPC* GField::SpawnNPC(int nNPCID, unsigned long nMarerID)
{
	const MARKER_INFO* pMarkerInfo = GetInfo()->FindMarker(nMarerID);
	if (NULL == pMarkerInfo) return NULL;

	return SpawnNPC(nNPCID, NULL, pMarkerInfo->vPoint, pMarkerInfo->vDir);
}

GEntityNPC* GField::SpawnNPC(int nNPCID, const SPAWN_INFO* pSpawnInfo, const vec3& pos, const vec3& dir, int nStaticID, SpawnDetailInfo* pDetailInfo )
{
	PFI_BLOCK_THISFUNC(6501);

	GNPCInfo* pNPCInfo = gmgr.pNPCInfoMgr->Find(nNPCID);
	if (!pNPCInfo)
	{
		dlog("Spawn Failed: invalid npc id. (field:%d / id:%d)\n", GetID(), nNPCID);
		return NULL;
	}

	GEntityNPC* pNPC = NULL;
	if (pDetailInfo &&
		pDetailInfo->bpart.bEnable)
	{
		GEntityBPart* pBPart = new GEntityBPart();
		pBPart->SetBPartInfo(pDetailInfo->bpart.nNPCID, pDetailInfo->bpart.nPartID, pDetailInfo->bpart.nFlag);
		pNPC = pBPart;
	}

	if (pNPC == NULL)
	{
		pNPC = new GEntityNPC_Leaf();
	}
	
	pNPC->Create(NewUID());

	if (pNPC->InitFromNPCInfo(pNPCInfo, pSpawnInfo, nStaticID) == false)
	{
		pNPC->Drop();
		dlog("Spawn Failed: npc initialize failed. (field:%d / id:%d)\n", GetID(), nNPCID);
		return NULL;
	}

	// 추가정보 설정가
	if (pDetailInfo)
	{
		if (pDetailInfo->uidParent.IsValid())
		{
			pNPC->GetNPCMinion().SetMasterUID(pDetailInfo->uidParent);
		}
		
		if (pDetailInfo->lootable.bEnable)
		{
			pNPC->DontRewardExp();
			pNPC->DontRewardQuest();
			pNPC->GetNPCLoot().SetBPartLootID(pDetailInfo->bpart.nLootID);

			vector<int> vecBeneficiaryCID;
			MUID uidParty;			

			GEntityPlayer* pRewarder = FindPlayer(pDetailInfo->lootable.uidRewarder);
			if (pRewarder)
			{
				vecBeneficiaryCID.push_back(pRewarder->GetCID());
				uidParty = pRewarder->GetPartyUID();

				if (pDetailInfo->lootable.bPartyReward)
				{
					vecBeneficiaryCID = gsys.pPartySystem->CollectNeighborMemberCID(pRewarder);
				}
			}

			GEntityPlayer* pCorpse = FindPlayer(pDetailInfo->lootable.uidCorpseForPlayer);
			if (pCorpse)
			{
				pCorpse->GetQPKillRewardOwner().GetBeneficiaryAndParty(vecBeneficiaryCID, uidParty);
			}

			pNPC->GetNPCRewardOwner().InsertBeneficiaryAndParty(vecBeneficiaryCID, uidParty);
			pNPC->GetNPCRewarder().RewardByDie(pNPC);
			pNPC->doDieForced();	
		}
	}

	// 그룹ID 기록
	if (pSpawnInfo && pSpawnInfo->nGroupID != INVALID_SPAWN_GROUP_ID)
	{
		pNPC->SetGroupID(pSpawnInfo->nGroupID);
	}	

	vec3 vSpawnPos = pos;
	if (CSNPCInfoHelper::IsUsingGravityNPC(pNPCInfo->nNpcType))
	{
		PickingZ(vSpawnPos, vSpawnPos);
	}
	pNPC->SetDir(dir);
	pNPC->SetFacingDir(dir);
	pNPC->EnterToField(this, vSpawnPos, dir);	
	
	if (pDetailInfo &&
		pDetailInfo->lootable.bEnable &&
		pDetailInfo->lootable.bRouteLootable)
	{
		gsys.pLootSystem->GetModifyHandler().RouteInsertLootableNPC(pNPC);
	}

	return pNPC;
}

bool GField::PickingZ(vec3& outPos, vec3 vOriginPos, float fPickAddedOriginZ)
{
	PFN_THISFUNC;
	PFI_BLOCK_THISFUNC(6004);

	if (!m_pInfo)
		return false;

	if (!m_pInfo->GetPickingZTable())
		return false;

	PFI_B(9095, "GField::PickingZ-GTestSystem::CheckNaN");
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vOriginPos);
	PFI_E(9095);
	
	outPos = vOriginPos;
	float fPosZ = m_pInfo->GetPickingZTable()->Pick(vOriginPos.x, vOriginPos.y, vOriginPos.z+fPickAddedOriginZ);
	if (fPosZ == INVALID_PICKING_HEIGHT)
		return false;

	outPos.z = fPosZ;
	return true;
}

bool GField::PickingZ_raw(vec3& outPos, vec3 vOriginPos, float fPickAddedOriginZ)
{
	PFI_B_THISFUNC(9094);

	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vOriginPos);

	// 네비게이션 메쉬 높이 검사
	float fHeight = 0.0f;
	if (gsys.pPathFinder->GetHeightAtPosition(&fHeight, GetPathMesh(), vOriginPos)==PATH_SUCCESS)
	{
		outPos = vOriginPos;
		outPos.z = fHeight;
		return true;
	}

	if (m_pInfo->m_pCollisionTree == NULL) 
	{
		gsys.pTestSystem->PickingLog(GetUID(), L"pick_fail_reason: m_pInfo->m_pCollisionTree == NULL");
		return false;
	}

	if ((vOriginPos.x <= m_pInfo->MinX()) || (vOriginPos.x >= m_pInfo->MaxX()) ||
		(vOriginPos.y <= m_pInfo->MinY()) || (vOriginPos.y >= m_pInfo->MaxY()) ) 
	{
		gsys.pTestSystem->PickingLog(GetUID(), L"pick_fail_reason: 맵 범위 이탈");
		return false;
	}

	vec3 vToPos = vOriginPos;
	const float SAFE_GAP = 1.0f;
	float maxz = 999999.f;
	float minz = -1.f;
	if (m_pInfo->m_pCollisionTree)
	{
		maxz = m_pInfo->m_pCollisionTree->GetBoundingBox().maxz;
		minz = m_pInfo->m_pCollisionTree->GetBoundingBox().minz;
	}

	bool bSuccess = true;
	int query_counter=0;

	outPos = vOriginPos;

	float fAddedValueZ = fPickAddedOriginZ;

	const int MAX_QUERY = 3;
	do 
	{
		// 지형과 충돌처리
		rs3::RCollisionPickInfo CollInfo;
		CollInfo.m_inPickOrigin = vOriginPos;
		CollInfo.m_inPickTo		= vToPos;

		CollInfo.m_inPickOrigin.z	= min(vOriginPos.z + fAddedValueZ, maxz);
		CollInfo.m_inPickTo.z		= minz - SAFE_GAP;

		if (m_pInfo->m_pCollisionTree->Pick(CollInfo) == true)
		{
			outPos.z = CollInfo.m_outPosition.z;
			bSuccess = true;
		}
		else
		{
			bSuccess = false;
			//return false;

			fAddedValueZ += PICKING_ADDED_VALUE_Z;
			if (query_counter >= MAX_QUERY-1)
			{
				fAddedValueZ = maxz - vOriginPos.z - 100.0f;
			}

			if (vOriginPos.z > m_pInfo->m_pCollisionTree->GetBoundingBox().maxz)
				break;
		}
		query_counter++;

#ifdef _DEBUG
		if (bSuccess == false)
		{
			blog("GField::GetGroundVerticalPos Pick 실패: zone = %s, pos = %.1f %.1f %.1f\n", m_pInfo->m_strZoneName.c_str(),
				vOriginPos.x, vOriginPos.y, vOriginPos.z);
		}
#endif
	} while (!bSuccess && query_counter<MAX_QUERY);

	return bSuccess;
}

bool GField::Pick(vec3& vOrigin, vec3& vTo, vec3* pOutPosition)
{
	if (m_pInfo == NULL || m_pInfo->m_pCollisionTree == NULL) return false;
	if (vOrigin == vTo)	 return false;

	rs3::RCollisionPickInfo info;
	info.m_inPickOrigin = vOrigin;
	info.m_inPickTo = vTo;

	if (m_pInfo->m_pCollisionTree->Pick(info) == true)
	{
		if (pOutPosition)
		{
			*pOutPosition = info.m_outPosition;
		}

		return true;
	}

	return false;
}


bool GField::IsEnterable()
{
	if (m_bDeleteMe)		return false;
	return true;
}

GAME_WEATHER_TYPE GField::GetCurrentWeather()
{
	if (NULL == m_pWeatherMgr) return WEATHER_INVALID;

	return m_pWeatherMgr->GetCurrentWeather();
}

GAME_TIME_TYPE GField::GetCurrentTime()
{
	if (m_pInfo && m_pInfo->m_TimeInfo.bFixed)
	{
		return m_pInfo->m_TimeInfo.nFixedTime;
	}

	if (NULL == gmgr.pEnvManager) return TIME_INVALID;

	return gmgr.pEnvManager->GetCurrentTime();
}

int GField::GetCurrentTimeDetail()
{
	if (NULL == gmgr.pEnvManager) return 0;

	return gmgr.pEnvManager->GetCurrentTimeHour();
}



void GField::DeleteMe()
{
	m_bDeleteMe = true;
}


bool GField::KickAllPlayers()
{
	vector<GEntityPlayer*> vecPlayers = m_pEntityMgr->GetPlayers();
	for (size_t i=0; i<vecPlayers.size(); i++)
	{
		if (vecPlayers[i]->GetPlayerField().GetFieldDynamic().GateToEnterPos(vecPlayers[i]->GetFieldID()) == NULL)
			return false;
	}

	return true;
}

void GField::ChangeWeather(GAME_WEATHER_TYPE nWeatherType)
{
	m_pWeatherMgr->ChangeWeather(nWeatherType);
	OnWeatherChanged();
}

void GField::OnWeatherChanged()
{
	if (!GetInfo())	return;
	if (NULL == m_pWeatherMgr) return;

	GAME_WEATHER_TYPE nOldWeather = m_pWeatherMgr->GetOldWeather();
	GAME_WEATHER_TYPE nNextWeather = m_pWeatherMgr->GetCurrentWeather();

	if (nOldWeather == nNextWeather) return;

	GetSpawn()->OnWeatherChanged(nOldWeather, nNextWeather);

	// Route
	MCommand* pNewCommand = MakeNewCommand(MC_FIELD_SET_WEATHER, 1, NEW_CHAR((int8)nNextWeather));
	
	GFieldGridRouter router;
	router.RouteToField(this, pNewCommand);

	// Callback
	GetScriptField()->OnWeatherChanged(GetDelegator(), (int)nNextWeather);
}

void GField::OnTimeChangedDetail( int nTime )
{
	GetSpawn()->OnTimeChangedDetail(nTime);
}


void GField::OnTimeChanged( GAME_TIME_TYPE nOldTime, GAME_TIME_TYPE nNewTime )
{
	GetSpawn()->OnTimeChanged(nOldTime, nNewTime);

	// Callback
	GetScriptField()->OnTimeChanged(GetDelegator(), (int)nNewTime);
}

bool GField::SetTimer( int nTimerID, float fIntervalTime, bool bRepeat/*=false*/ )
{
	return m_ScriptTimer.SetTimer(nTimerID, fIntervalTime, bRepeat);
}

bool GField::KillTimer( int nTimerID )
{
	return m_ScriptTimer.KillTimer(nTimerID);
}

bool GField::IsTimer(int nTimerID)
{
	return m_ScriptTimer.IsSet(nTimerID);
}

void GField::OnElapsedTime( const vector<int>& vecUpdatedTimers )
{
	PFC_THISFUNC;
	for each (int nTimerID in vecUpdatedTimers)
	{
		if (!GetScriptField()->OnTimer(GetDelegator(), nTimerID))
		{
			dlog("GField::OnElapsedTime(): 타이머 콜백함수가 없습니다. (Field: %d, TimerID: %d)\n", GetID(), nTimerID);
		}
	}
}

void GField::ClearEntities(bool bExceptPlayers)
{
	m_pEntityMgr->Clear(bExceptPlayers);
}

void GField::Despawn(int nNPCID, bool bSpawnEnable)
{
	vector<GEntityNPC*> vecDespawnNPC = m_pEntityMgr->FindNPCByID(nNPCID);
	for each (GEntityNPC* pDespawnNPC in vecDespawnNPC)
	{
		pDespawnNPC->Despawn(bSpawnEnable);
	}
}

void GField::DespawnNow( int nNPCID, bool bSpawnEnable )
{
	vector<GEntityNPC*> vecDespawnNPC = m_pEntityMgr->FindNPCByID(nNPCID);
	for each (GEntityNPC* pDespawnNPC in vecDespawnNPC)
	{
		pDespawnNPC->DespawnNow(bSpawnEnable);
	}
}

GEntityNPC* GField::Spawn(int nNPCID, vec3 vStartPos, float fDelayTime, float fDespawnTime)
{
	if (false == CheckLuaSpawnCount(nNPCID)) return NULL;

	return GetSpawn()->Spawn(nNPCID, vStartPos, vec3(0.0f,1.0f,0.0f), fDelayTime, fDespawnTime);
}

GEntityNPC* GField::Spawn( unsigned long nSpawnID, float fDelayTime, float fDespawnTime )
{
	if (false == CheckLuaSpawnCount(nSpawnID)) return NULL;

	return GetSpawn()->Spawn(nSpawnID, fDelayTime, fDespawnTime);
}

bool GField::CheckLuaSpawnCount( int nNPCID )
{
	GNPCInfo* pNPCInfo = gmgr.pNPCInfoMgr->Find(nNPCID);
	if (NULL == pNPCInfo) return false;

	if (0 == pNPCInfo->m_nLuaSpawnCount) return true;	
	if (pNPCInfo->m_nLuaSpawnCount > GetNPCQty(nNPCID)) return true;
	
	return false;
}

bool GField::CheckLuaSpawnCount( unsigned long nSpawnID )
{
	const SPAWN_INFO* pSpawnInfo = GetSpawn()->GetSpawnInfo(nSpawnID);
	if (NULL == pSpawnInfo) return false;

	for each (const SPAWN_NPC_INFO& spawnNPCInfo in pSpawnInfo->vecNPCInfoList)
	{
		if (false == CheckLuaSpawnCount(spawnNPCInfo.nID))
		{
			return false;
		}
	}

	return true;
}


GEntityNPC* GField::Despawn(unsigned long nSpawnID, bool bSpawnEnable)
{
	return GetSpawn()->Despawn(nSpawnID, bSpawnEnable);
}

GEntityNPC* GField::GetSpawnNPC( unsigned long nSpawnID )
{
	return GetSpawn()->GetNPCInstance(nSpawnID);
}

vec3 GField::GetMarkerPos( int nMarkerID )
{
	const MARKER_INFO* pMarkerInfo = m_pInfo->FindMarker(nMarkerID);
	if (NULL == pMarkerInfo) return vec3::ZERO;

	return pMarkerInfo->vPoint;
}

vec3 GField::GetMarkerDir( int nMarkerID )
{
	const MARKER_INFO* pMarkerInfo = m_pInfo->FindMarker(nMarkerID);
	if (NULL == pMarkerInfo) return vec3::ZERO;

	return pMarkerInfo->vDir;
}

void GField::CreateDelegator()
{
	m_pDelegator = new GGlueField(this);
}

GGlueField* GField::GetDelegator(void)
{
	VALID_RET(m_pDelegator, NULL);
	return m_pDelegator;
}

void GField::EnableInteraction(int nNPCID) 
{
	vector<GEntityNPC*> vecNPC = FindNPCByID(nNPCID);

	for each (GEntityNPC* pNPC in vecNPC)
	{
		pNPC->GetNPCInteraction().Enable();
	}
}

void GField::DisableInteraction(int nNPCID)
{
	vector<GEntityNPC*> vecNPC = FindNPCByID(nNPCID);

	for each (GEntityNPC* pNPC in vecNPC)
	{
		pNPC->GetNPCInteraction().Disable();
	}
}

void GField::UpdateUsableSensor(const SENSOR_INFO* pSensor) const
{
	VALID(pSensor);

	GFieldGrid::EntitySelector* grid_selector = 
		GetGrid().GetEntitySelector();

	GVectorMUID vecNeighborPlayers;
	grid_selector->GetPlayers(vecNeighborPlayers.Vector(), pSensor->vPoint);

	for each (const MUID& each in vecNeighborPlayers.Vector())
	{
		GEntityPlayer* pEach = FindPlayer(each);
		if (!pEach) 
			continue;
		pEach->UpdateUsableSensor(true);
	}
}

GFieldGrid& GField::GetGrid() const
{
	DCHECK(m_pFieldGrid);
	return *m_pFieldGrid;
}

GFieldSensor& GField::GetFieldSensor() const
{
	DCHECK(m_pFieldSensor);
	return *m_pFieldSensor;
}

GEntityMgr* GField::GetEntityMgr() const
{
	DCHECK(m_pEntityMgr);
	return m_pEntityMgr;
}

void GField::ActivateSpawnGroup(int nGroupID)
{
	// 스폰정보 변경
	m_pSpawn->ActivateSpawnGroup(nGroupID);

	// 이미 스폰된 NPC정보 변경
	m_pEntityMgr->ActiveSpawnGroup(nGroupID);
}

void GField::RoutePlayBGM( const wchar_t* pszSoundname )
{
	MCommand* pNewCommand = MakeNewCommand(MC_ENV_PLAY_BGM, 1, NEW_WSTR(pszSoundname));
	GFieldGridRouter router;
	router.RouteToField(this, pNewCommand);
}

void GField::UpdateTimer( float fDelta )
{
	if (m_ScriptTimer.HasTimer())
	{
		if (m_ScriptTimer.Update(fDelta))
		{
			OnElapsedTime(m_ScriptTimer.GetUpdatedTimers());
		}
	}
}

void GField::OnEntityEntered( GEntity* pEntity )
{
	VALID(pEntity);
	AddEntity(pEntity);
}

void GField::OnEntityLeaved( GEntity* pEntity )
{
	VALID(pEntity);
	RemoveEntity(pEntity);
}

GQuestPVP* GField::GetQuestPVP()
{
	return m_pQuestPVP;
}

void GField::Clear()
{
	ClearEntities(true);
	Update(0.0f);			// tick for delete entities
	GetSpawn()->Fini();
	ResetTimer();
	m_pQuestPVP->Reset();
	m_pDuelMgr->Fini();
	GetSession()->Clear();
}

void GField::AttachCombatMonitor( GCombatMonitorPort* pCombatMonitorPort )
{
	VALID(pCombatMonitorPort);

	if (m_pCombatMonitorPort)
	{
		DetachCombatMonitor();
	}
	
	m_pCombatMonitorPort = pCombatMonitorPort;
	m_pCombatMonitorPort->Prepare();
	m_pCombatMonitorPort->SetField(this);

	// 모든 NPC 옵저버 붙이기
	vector<GEntityActor*> vecActors = m_pEntityMgr->GetActors();
	for each (GEntityActor* each in vecActors)
	{
		m_pCombatMonitorPort->AttachSubject(each);
	}

	m_pCombatMonitorPort->Start();
}

void GField::DetachCombatMonitor()
{
	if (m_pCombatMonitorPort)
	{
		m_pCombatMonitorPort->Reset();
	}

	m_pCombatMonitorPort = NULL;
}

void GField::OnAddEntity( GEntity* pEntity )
{
	VALID(pEntity);

	if (pEntity->IsActor() &&
		m_pCombatMonitorPort)
	{
		m_pCombatMonitorPort->AttachSubject(ToEntityActor((pEntity)));
	}
}

void GField::OnRemoveEntity( GEntity* pEntity )
{
	VALID(pEntity);

	if (pEntity->IsActor() &&
		m_pCombatMonitorPort)
	{
		m_pCombatMonitorPort->DettachSubject(ToEntityActor((pEntity)));
	}
}

int GField::GetIntroCutsceneID() const
{
	if (!m_pInfo)
		return INVALID_ID;

	return m_pInfo->m_nIntroCutsceneID;
}
