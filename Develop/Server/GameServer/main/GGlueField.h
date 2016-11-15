#pragma once

#include "MWLua.h"

class GField;
class GGlueNPC;
class GGlueQuestPVP;
class GFieldNPCSession;

class GGlueField : public MTestMemPool<GGlueField>
{
public:
	GGlueField(GField* pOwner);
	virtual ~GGlueField();

	GField* GetOwnerField() const;
	bool IsInvalidOwner() const;

	const string GetLuaTableName();

public:
	virtual bool IsNull(void) const;	
	int GetID();

	GGlueNPC*	Spawn(int nNPCID, vec3 vStartPos=vec3::ZERO);
	void		Despawn(int nNPCID, bool bSpawnEnable);
	void		DespawnNow(int nNPCID, bool bSpawnEnable);
	GGlueNPC*	SpawnLimited(int nNPCID, vec3 vStartPos, float fLimitTime);
	void		SpawnDelay(int nNPCID, vec3 vStartPos, float fDelay);
	int			GetNPCCount(int nNPCID);
	GGlueNPC*	GetNPC(int nNPCID);
	void		EnableInteraction(int nNPCID);
	void		DisableInteraction(int nNPCID);

	GGlueNPC*	SpawnByID(unsigned long nSpawnID);
	void		SpawnDelayByID(unsigned long nSpawnID, float fDelayTime);
	GGlueNPC*	DespawnByID(unsigned long nSpawnID, bool bSpawnEnable);
	GGlueNPC*	GetSpawnNPC(unsigned long nSpawnID);

	void ActivateSpawnGroup(int nGroupID);

	bool EnableSensor(int nSensorID);
	bool DisableSensor(int nSensorID);
	bool CheckSensor(int nSensorID);

	bool SetTimer(int nTimerID, float fIntervalTime, bool bRepeat);
	bool KillTimer(int nTimerID);

	vec3 GetMarkerPos(int nMarkerID);
	vec3 GetMarkerDir(int nMarkerID);	

	void PlayBGM(const char* pszSoundname);
	void StopBGM();

	void Notice( const char* pszMsg );
	void NoticeToField( const char* pszMsg );

	void ChangeWeather(int nWeatherType);
	GGlueQuestPVP* GetQuestPVP();

	GFieldNPCSession* MakeSession(const char* szName, MWLua::table tableNPCs);
	GFieldNPCSession* FindSession(const char* szName);
protected:	
	GField* m_pOwner;
};
