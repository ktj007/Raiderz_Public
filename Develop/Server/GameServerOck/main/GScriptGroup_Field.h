#pragma once

#include "GScriptUtil.h"

class GGlueField;
class GFieldInfo;
class GGlueActor;
class GGlueNPC;
class GGluePlayer;

class GScriptGroup_Field : public MTestMemPool<GScriptGroup_Field>
{
public:
	GScriptGroup_Field(void);
	~GScriptGroup_Field(void);

	void RegisterGlues();

	void Load(GFieldInfo* pFieldInfo);
	bool HasSensorScript(int nFieldID, int nSensorID);

	bool OnCreate(GGlueField* pField);
	bool OnDestroy(GGlueField* pField);
	bool OnTimeChanged(GGlueField* pField, int nNewTime);
	bool OnWeatherChanged(GGlueField* pField, int nNextWeather);
	bool OnTimer(GGlueField* pField, int nTimerID);

	bool OnSensorEnter(GGlueField* pField, int nSensorID, GGlueActor* pActor);
	bool OnSensorLeave(GGlueField* pField, int nSensorID, GGlueActor* pActor);
	bool OnSensorInteract(GGlueField* pField, int nSensorID, GGluePlayer* pPlayer);
	bool OnSensorTalent(GGlueField* pField, int nSensorID, GGlueActor* pActor, int nTalentID);

	bool OnMarkerArrive(GGlueField* pField, unsigned long nMarkerID, GGlueNPC* pNPC);

	bool OnSpawn(GGlueField* pField, GGlueNPC* pNPC, int nSpawnID);	
	bool OnDie(GGlueField* pField, GGlueNPC* pNPC, int nSpawnID);

	bool OnEndCutscene( GGlueField* pField, GGluePlayer* pPlayer, int nCutsceneID );
	
	bool OnEvent( GGlueField* pField, GGlueActor* pActor, const char* pszName );
private:
	bool PreCheck();
private:
	GScriptUtil m_ScriptUtil;
};
