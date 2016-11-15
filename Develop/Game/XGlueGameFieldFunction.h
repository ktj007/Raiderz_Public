#pragma once

class XGlueGameFieldFunction
{
public:

	//필드
	//void IsNull(void) const{;	}
	//void GetID(){;}
	//void Spawn(int nNPCID, vec3 vStartPos) {}
	//void SpawnByID(unsigned long nSpawnID) {}
	//void DespawnByID(unsigned long nSpawnID){}
	//void GetSpawnNPC(unsigned long nSpawnID){;}
	//void EnableSensor(int nSensorID){;}
	//void DisableSensor(int nSensorID){;}
	//void  CheckSensor(int nSensorID){;}
	//void GetNPCCount(int nNPCID){;}
	//void  SetTimer(int nTimerID, float fIntervalTime, bool bRepeat){;}
	//void KillTimer(int nTimerID){;}
	//void GetMarkerPos(int nMarkerID){;}
	//void GetMarkerDir(int nMarkerID){;}

	//채널
	void	ReqChannelList();
	void	ChangeChannel(int nChannelID);
	void	ChangeChannelCancel();

	//날씨
	//void	ChangeWeather(int nWeather);
	//void	ChangeCurrentWeather();
	//void	ChangeCustomWeather( const char* szEnvname, int nTime, int nWeather);

	//사운드

};
