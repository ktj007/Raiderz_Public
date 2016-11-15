#ifndef _GFIELD_H
#define _GFIELD_H

#include "MUID.h"
#include "GDef.h"
#include "GMessage.h"
#include "GFieldInfo.h"
#include "MTime.h"
#include "GEntityMgr.h"
#include "MPathCollision.h"
#include "GPathContextGroup.h"
#include "GScriptTimer.h"
#include "GFieldGridRouter.h"
#include "CTransData.h"

class GEntitySync;
class GEntityMgr;
class GEntityPlayer;
class GEntityNPC;
class GBuffEntity;
class GFieldInfo;
class GFieldMgr;
class GWeatherMgr;
class GNullGlueField;
class GGlueField;
class GFieldAIEventInvoker;
struct TD_UPDATE_CACHE_PLAYER;
struct TD_UPDATE_CACHE_NPC;
struct SpawnDetailInfo;
enum GAME_WEATHER_TYPE;
enum QUESTPVP_TEAM;
class GFieldSensor;
struct SPathMesh;
class GFieldGrid;
class GQuestPVP;
class GDuelMgr;
class GFieldNPCSessionMgr;
class GCombatMonitorPort;

typedef tr1::unordered_map<int, GEntityPlayer*> MAP_PLYAERCID;

/// 필드(맵)
/// - GFIeld 이외의 다른 클래스에서 GEntityMgr, GWeatherMgr, GSectorMgr 클래스에 직접 접근은 할 수 없습니다.
class GField
{
	friend class GEntity;
	friend class GEntityNPC;
	friend class GSpawnManager;
	friend class GFieldMgr;
	friend class GFieldGridRouter;
public:
	typedef GNullGlueField NullType;	

						GField();
						GField(const MUID& uid, GWeatherMgr* pWeatherMgr);
	virtual				~GField();

	bool				Create(int nFieldID);
	virtual bool		Create(const GFieldInfo* pFieldInfo);
			void		OnCreated();
	virtual void		Destroy();
	void				DeleteMe();
	virtual void		Update(float fDelta);
	void				Clear();

	const MUID			GetUID(void) const;
	const int			GetID(void) const;
	const GFieldInfo*	GetInfo() const { return m_pInfo; }
	int					GetZoneID();
	const MARKER_INFO*	FindMarkerInfo(unsigned long nID) const;
	SPathMesh*			GetPathMesh()	{ return m_pPathMesh; }
	int					GetIntroCutsceneID() const;

	// 필드에 진입할 수 있는지 여부
	virtual bool		IsEnterable();
	
	GAME_WEATHER_TYPE	GetCurrentWeather();
	GAME_TIME_TYPE		GetCurrentTime();
	int					GetCurrentTimeDetail();

	int					GetFatigueMod()	{ return m_nFatigueMod; }

	virtual bool		IsDynamicField()	{ return false; }
	virtual bool		IsSharedField()		{ return false; }

	void				ReservedEnter(GEntityPlayer* pPlayer, vec3 vecDestPos, vec3 vecDestDir);
	void				CancelPreservedEnter(GEntityPlayer* pPlayer);

	void				RemovePlayerCID(int nCID);

	GSpawnManager*		GetSpawn() { return m_pSpawn; }
	void				ChangeWeather(GAME_WEATHER_TYPE nWeatherType);

	void				RoutePlayBGM(const wchar_t* pszSoundname);
	// 엔티티 관련 함수 --------------------------------------
	GEntity*		FindEntity(const MUID& uidEntity) const;
	GEntityPlayer*	FindPlayer(const MUID& uidPlayer) const;
	GEntityPlayer*	FindPlayerByCID(int nCID) const;
	GEntityNPC*		FindNPC(const MUID& uidNPC) const;
	GEntityNPC*		FindNPC(const UIID nNPCUIID) const;
	vector<GEntityNPC*>	FindNPCByID(int nNPCID) const;
	GEntityNPC*		FindOneNPCByID(int nNPCID) const;
	GEntityActor*	FindActor(const MUID& uidActor) const;
	GBuffEntity*	FindBuffEntity(const MUID& uidMagicArea) const;
	GEntityMgr*		GetEntityMgr() const;
	int				GetEntityQty(ENTITY_TYPEID nType);
	int				GetPlayerQty();
	int				GetNPCQty(int nNPCID);	
	void			GetNeighborNPC(const vec3& vPos, const float fRange, vector<GEntityNPC*>& vecOutNeighborNPC);
	bool			KickAllPlayers();
	bool			HasReservedEnterPlayers()		{ return !m_setReservedEnterPlayers.empty(); }
	bool			ExistPlayer()					{ return m_pEntityMgr->GetEntityQty(ETID_PLAYER) > 0; }
	void			ClearEntities(bool bExceptPlayers);

	void			UpdateTimer( float fDelta );

	GEntityMgr::ENTITY_UID_MAP&	GetEntityMap();
	GEntityNPC*		SpawnNPC(int nNPCID, unsigned long nMarerID);
	GEntityNPC*		SpawnNPC(int nNPCID, const SPAWN_INFO* pSpawnInfo, const vec3& pos, const vec3& dir, int nStaticID=0, SpawnDetailInfo* pDetailInfo=NULL);

	// 충돌 관련 함수 ---------------------------------------v
	virtual bool	PickingZ(vec3& outPos, vec3 vOriginPos, float fPickAddedOriginZ = PICKING_ADDED_VALUE_Z);
	bool			Pick(vec3& vOrigin, vec3& vTo, vec3* pOutPosition=NULL);

	// 센서 관련 함수 -------------------------------------v
	void			InitSensor();
	bool			IsEnableSensor(unsigned long nSensorID) const;
	bool			EnableSensor(unsigned long nSensorID, bool bEnable);


	void ResetTimer();


	void UpdateUsableSensor(const SENSOR_INFO* pSensor) const;
	const SENSOR_INFO*	GetSensor(unsigned long nSensorID);
	//void			MakeUsableSensorInfo(GEntityPlayer* pReqPlayer, vector<TD_TRIGGER_SENSOR_INFO>& vecSensors);

	// 길찾기 관련 함수 -------------------------------------v
	void		RegisterToPathGroup(MUID& uidTarget, GEntityNPC* pEntityNPC)		{ m_PathContextGroup.Register(uidTarget, pEntityNPC); }
	void		RevokeFromPathGroup(GEntityNPC* pEntityNPC)							{ m_PathContextGroup.Revoke(pEntityNPC); }

	// 섹터 관련 함수 ---------------------------------------v
	GFieldGrid& GetGrid() const;
	GFieldSensor& GetFieldSensor() const;

	inline bool IsValidPosition(vec3& pos);

	// 타이머 관련 함수
	bool SetTimer(int nTimerID, float fIntervalTime, bool bIsInterval=false);
	bool KillTimer(int nTimerID);
	bool IsTimer(int nTimerID);

	// 인터랙션 관련 함수
	void EnableInteraction(int nNPCID);
	void DisableInteraction(int nNPCID);

	// 콜백 관련 함수
	void Luacallback_OnCreate();
	void Luacallback_OnDestroy();
	virtual void OnWeatherChanged();
	void OnTimeChanged( GAME_TIME_TYPE nOldTime, GAME_TIME_TYPE nNewTime );					
	void OnTimeChangedDetail( int nTime );
	void OnElapsedTime(const vector<int>& vecUpdatedTimers);

	// 결투 관리자 반환
	GDuelMgr*		 GetDuelMgr() const { return m_pDuelMgr; }

	// for test
	size_t GetPathContextGroupCount() { return m_PathContextGroup.GetContextCount(); }
	
	// Functions for Lua
	GEntityNPC*	Spawn(int nNPCID, vec3 vStartPos, float fDelayTime=0.0f, float fDespawnTime=0.0f);
	GEntityNPC* Spawn(unsigned long nSpawnID, float fDelayTime=0.0f, float fDespawnTime=0.0f);

	void		Despawn(int nNPCID, bool bSpawnEnable);
	GEntityNPC* Despawn(unsigned long nSpawnID, bool bSpawnEnable);	
	void		DespawnNow(int nNPCID, bool bSpawnEnable);
	
	GEntityNPC* GetSpawnNPC(unsigned long nSpawnID);
	vec3 GetMarkerPos(int nMarkerID);
	vec3 GetMarkerDir(int nMarkerID);

	void CreateDelegator();	
	GGlueField* GetDelegator(void);

	bool PickingZ_raw(vec3& outPos, vec3 vOriginPos, float fPickAddedOriginZ = PICKING_ADDED_VALUE_Z);

	void ActivateSpawnGroup(int nGroupID);

	GQuestPVP* GetQuestPVP();

	bool CheckLuaSpawnCount(int nNPCID);
	bool CheckLuaSpawnCount(unsigned long nSpawnID);

	virtual void OnEntityEntered(GEntity* pEntity);
	virtual void OnEntityLeaved(GEntity* pEntity);

	GFieldNPCSessionMgr*			GetSession() { return m_pNPCSession; }

	void AttachCombatMonitor(GCombatMonitorPort* pCombatMonitorPort);
	void DetachCombatMonitor();
protected:
	virtual MUID NewUID();

	virtual void AddEntity(GEntity* pEntity);
	virtual void RemoveEntity(GEntity* pEntity);

	virtual void OnAddEntity(GEntity* pEntity);
	virtual void OnRemoveEntity(GEntity* pEntity);
	
protected:
	set<MUID>						m_setReservedEnterPlayers;
	const GFieldInfo*				m_pInfo;			///< 맵 정보
	GEntityMgr*						m_pEntityMgr;		///< 엔티티 관리자
	GWeatherMgr*					m_pWeatherMgr;
	GFieldGrid*						m_pFieldGrid;
	GSpawnManager*					m_pSpawn;	
	GFieldNPCSessionMgr*			m_pNPCSession;
	GGlueField*						m_pDelegator;
	
private:
	GScriptTimer					m_ScriptTimer;
	MUID							m_UID;					///< 필드 UID	
	GPathContextGroup				m_PathContextGroup;
	GFieldSensor*					m_pFieldSensor;
	MRegulator						m_rSensorUpdate;
	int								m_nLastUpdateChangeField; ///< 마지막으로 맵 변경 업데이트를 실행한 시각
	set<int>						m_setEnableSensor;
	bool							m_bRegisteredManager;
	bool							m_bDeleteMe;
	int								m_nFatigueMod;				///< 피로도 감소율(필드마다 캐릭터 피로도 감소율이 다르다)

	MAP_PLYAERCID					m_mapPlayerCID;
		
	GFieldAIEventInvoker*			m_pAILodEventInvoker;
	SPathMesh*						m_pPathMesh;
	GQuestPVP*						m_pQuestPVP;
	GDuelMgr*						m_pDuelMgr;

	GCombatMonitorPort*				m_pCombatMonitorPort;
};


inline bool GField::IsValidPosition(vec3& pos)
{
	if (pos.x<m_pInfo->MinX() || pos.x>m_pInfo->MaxX() || pos.y<m_pInfo->MinY() || pos.y>m_pInfo->MaxY())
	{
		return false;
	}
	return true;
}



#endif
