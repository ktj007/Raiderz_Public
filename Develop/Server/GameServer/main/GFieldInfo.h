#pragma once

#include "MTypes.h"
#include "MXml.h"
#include <string>
#include <map>
#include <vector>
#include "CSFieldInfo.h"
#include "CSWorldInfo.h"
#include "CSTriggerInfo.h"
#include "SFieldInfo.h"

using namespace std;

#define MAX_SPAWN_RATE	1000

class GFieldInfoMgr;
class GEntityActor;
class GEntityNPC;
class GField;
class GCollisionTree;
class GFieldInfoLoader;
class GFieldAttrGrid;
class GPickingZTable;

// 각 맵에 들어갈 정보들(DB나 xml로부터 읽어낼 듯)

struct SPAWN_NPC_INFO
{
	bool bDefaultSet;	///< 아무것도 선택되지 않으면 이 셋이 선택
	int nPickRate;		///< Pick rate
	int nID;			///< 필드에서의 NPC 식별값

	SPAWN_NPC_INFO(): bDefaultSet(false), nID(0), nPickRate(0) 									{}
	SPAWN_NPC_INFO(int nID): bDefaultSet(true), nID(nID), nPickRate(MAX_SPAWN_RATE) 			{}
	SPAWN_NPC_INFO(int nID, int nPickRate): bDefaultSet(false), nID(nID), nPickRate(nPickRate) 	{}
};


/// 스폰 정보
struct SPAWN_INFO : public MARKER_INFO
{
	bool			bEnable;								///< 실행중인지 여부
	int				nStartTime;								///< 필드가 생성된후 스폰을 시작할 시각 (sec)
	unsigned int	nRespawnTime;							///< 리스폰까지의 시간
	bool			bRandomDir;								///< 방향을 설정할지 여부
	float			fRadius;								///< 생성 범위
	float			fRoamRadius;							///< 배회 범위
	float			fRoamDepth;								///< 배회 범위의 Z범위
	int				nGroupID;								///< NPC Group ID
	bool			bEnableTimeList[MAX_TIME_TYPE];			///< 스폰 가능한 시간대 목록
	bool			bEnableWeatherList[MAX_WEATHER_TYPE];	///< 스폰 가능한 날씨 목록
	pair<int,int>	pairEnableTime;							///< 정밀한 시간대조건 (시작시간, 끝시간)
	bool			bUseConditionTime;						///< 시간대 조건 사용
	bool			bUseConditionWeather;					///< 날씨 조건 사용
	GATHER_TYPE		nGatherType;							///< 채집일 경우 채집 타입 - 채집이 아니면 GATHER_NONE
	bool			bCheckSpawnPointValidation;				///< 스폰되는 위치 유효성 판단할지 여부(네비게이션 메쉬)
	vector<SPAWN_NPC_INFO> vecNPCInfoList;					///< 생성될 NPC 집합	
	float			fCombatRadius;							///< 전투 유지될 범위 (범위에서 벗어나면 전투 종료)
	

	SPAWN_INFO()
		: bRandomDir(true)
		, bEnable(true)
		, nStartTime(0)
		, fRadius(0.0f)
		, fRoamRadius(0.0f)
		, nRespawnTime(0)
		, nGroupID(INVALID_SPAWN_GROUP_ID)
		, fRoamDepth(200.f)
		, bCheckSpawnPointValidation(true)
		, bUseConditionTime(false)
		, bUseConditionWeather(false)
		, pairEnableTime(-1, -1)
		, nGatherType(GATHER_NONE)
		, fCombatRadius(0.0f)
	{
		memset(bEnableTimeList, 0, sizeof(bEnableTimeList));
		memset(bEnableWeatherList, 0, sizeof(bEnableWeatherList));
	}
};

/// 필드 날씨 정보 노드
struct GFieldWeatherInfoNode
{
	GAME_WEATHER_TYPE	nWeather;
	int					nPercent;

	GFieldWeatherInfoNode() : nWeather(DEFAULT_GAME_WEATHER), nPercent(0) {}
};

/// 필드의 날씨 정보
struct GFieldWeatherInfo
{
	GAME_WEATHER_TYPE				nDefault;
	vector<GFieldWeatherInfoNode>	vecNodes;
	vector<GAME_WEATHER_TYPE>		vecRandomData;
	
	GFieldWeatherInfo() : nDefault(DEFAULT_GAME_WEATHER) {}
	void Clear();
};


typedef map<unsigned long, SPAWN_INFO> SPAWN_INFO_MAP;
typedef map<unsigned long, MARKER_INFO> MARKER_INFO_MAP_BY_ID;
typedef map<unsigned long, SENSOR_INFO> SENSOR_INFO_MAP_BY_ID;
typedef map<int, vec3> SPAWNPOS_INFO_MAP;

struct FIELD_GROUP_INFO;

/// 필드 정보
/// - 순수하게 필드 정보값만 가지고 있다.
class GFieldInfo : public SFieldInfo, public MTestMemPool<GFieldInfo>
{
	friend class GFieldInfoMgr;
	friend class GField;
	friend class GFieldInfoLoader;
	friend class GFieldInfoValidator;
public:
	int						m_nZoneID;		///< 존 ID
	wstring					m_strZoneName;	///< 존 이름
	wstring					m_strZoneFile;	///< 존 파일 이름
	int						m_nParentID;	// 부모맵id
	GFieldWeatherInfo		m_WeatherInfo;	// 날씨 정보
	FIELD_TYPE				m_nType;
	float					m_fSectorHeight;	///< 섹터라우팅 및 어그로 허용높이차
	int						m_nMaxGatherInstance[MAX_GATHER_TYPE];		///< 필드당 최대 채집 오브젝트 인스턴스 갯수 - 0이면 무제한
	FIELD_GROUP_INFO*		m_pFieldGroup;
	int						m_nQPVPNPCID1;
	int						m_nQPVPNPCID2;
	int						m_nQPVPRebirthMarkerID1;
	int						m_nQPVPRebirthMarkerID2;
	int						m_nIntroCutsceneID;

private:
	float					m_fMinX;
	float					m_fMaxX;
	float					m_fMinY;
	float					m_fMaxY;
public:
	GFieldInfo();
	virtual ~GFieldInfo();

	void Init(const wstring& strZonePath, const wstring& strFile);
	void						Reset();
	void						RemoveSpawnTime();

	const GAME_WEATHER_TYPE		GetRandomWeather() const;
	const SPAWN_INFO_MAP&		GetSpawnInfo() const{ return m_mapSpawn; }
	vec3						GetSpawnPos(int nNPCID);

	const SENSOR_INFO_MAP_BY_ID& GetSensorInfoContainer() const	{ return m_mapSensorByID; }	

	const MARKER_INFO*			FindMarker(unsigned long nID) const;
	const SENSOR_INFO*			FindSensor(int nID) const;
	const SPAWN_INFO*			FindSpawnInfo(unsigned long nID) const;

	const MARKER_INFO*			GetFirstMarker(void) const;
	void						InsertMarkerInfo(const MARKER_INFO& markerInfo);
	void						InsertSpawnInfo(const SPAWN_INFO& info);
	void						InsertSensor( SENSOR_INFO& info );

	void						AutoAssign_IsSensorScript();
	GCollisionTree*				GetCollisionTree() { return m_pCollisionTree; }
	const bool					IsWeatherNotChanged() const { return m_WeatherInfo.vecRandomData.empty(); }

	bool						IsSectorHeightLimitField() const;							///< 필드에 허용 높이차가 적용되었는지 여부
	bool						IsAllowSectorHeightLimit(float fDifferenceHeight) const;	///< 해당 높이차가 필드에서 허용하는 높이차인지 묻는다

	void ChangeSensorSetting( SENSOR_INFO& source_info);

	bool IsLoadingCollision();
	bool IsLoadingPath();

	GFieldAttrGrid*				GetAttrGrid() const			{ return m_pAttrGrid; }
	void SetPickingZTable(GPickingZTable* pTable)			{ m_pPickingZTable = pTable; }
	GPickingZTable*				GetPickingZTable() const	{ return m_pPickingZTable; }

	float MinX() const;
	float MinY() const;
	float MaxX() const;
	float MaxY() const;
	void SetFieldMinXY_ForTest(float x, float y);
	void SetFieldMaxXY_ForTest(float x, float y);

	void Cooking();

	bool IsInWarpSensor(vec3 vPos);
	const MARKER_INFO* GetLeaveMarker(int nToFieldID) const;

	bool IsPortalTalentUsable() const	{ return m_bPotralTalentUsable; }

	const set<int>& CollectQuestNPC() const	{ return m_setQuestNPC; }

protected:
	GCollisionTree*		m_pCollisionTree;		// 충돌맵
	GFieldAttrGrid*		m_pAttrGrid;
	GPickingZTable*		m_pPickingZTable;

	// 중복맵이 아니다... 나중에 생각.

	SPAWN_INFO_MAP			m_mapSpawn;
	MARKER_INFO_MAP_BY_ID	m_mapMarkerByID;
	SENSOR_INFO_MAP_BY_ID	m_mapSensorByID;

	SPAWNPOS_INFO_MAP		m_mapSpawnPos;		// SPAWN_INFO_MAP과 다르게 키값이 NPCID, 첫번째 위치만 가지고 있다.
	set<int>				m_setQuestNPC;		// 퀘스트 관련된 NPC(시작, 보상)

	// 또 뭐가 있을까

	bool LoadCollision(wstring& strZoneFilename);
	bool LoadPath(const wstring& strZonePath, const wstring& strPathFile);
	void InitWeather();
	bool LoadCommon(wstring& strZoneFilename);
};

struct FIELD_GROUP_INFO : public MTestMemPool<FIELD_GROUP_INFO>
{
	FIELD_GROUP_INFO()
	: nID(0)
	, pStartFieldInfo(NULL)
	, pStartMarkerInfo(NULL)
	, bValidation(true)
	, bChallengerQuest(false)
	{
	}

	int					nID;
	const GFieldInfo*	pStartFieldInfo;
	const MARKER_INFO*	pStartMarkerInfo;
	vector<const GFieldInfo*>	vecFieldInfo;
	set<int>			setRunningQuest;	// 해당 필드 그룹에서 수행되는 퀘스트들
	bool				bValidation;
	bool				bChallengerQuest; // 도전자퀘스트가 수행되는 곳인지 여부
};

