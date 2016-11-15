#pragma once

#include "GFieldGrid.h"

class GEntityPlayer;
struct TD_UPDATE_CACHE_PLAYER;
struct TD_UPDATE_CACHE_NPC;
struct TD_UPDATE_CACHE_PBART;
struct TD_UPDATE_CACHE_BUFFENTITY;
struct TD_TRIGGER_SENSOR_INFO;

class GFieldGridEntityInfoMaker
{
	typedef const vector<GFieldGrid::Sector*> GFieldGridSectorVec;
public:
	void MakeSensorInfos(GEntityPlayer* pPlayer, vector<TD_TRIGGER_SENSOR_INFO>& outvecSensorList);
	void MakeBuffInfos(GEntityPlayer* pPlayer, GFieldGridSectorVec& vecCells, vector<TD_UPDATE_CACHE_BUFFENTITY>& outvecMagicArearInfoList);
	void MakeNPCInfos(GEntityPlayer* pPlayer, GFieldGridSectorVec& vecCells, vector<TD_UPDATE_CACHE_NPC>& outvecNPCInfoList, vector<TD_UPDATE_CACHE_PBART>& outvecEntityBPartInfoNode);
	void MakePlayerInfos(GEntityPlayer* pPlayer, GFieldGridSectorVec& vecCells, vector<TD_UPDATE_CACHE_PLAYER>& outvecPlayerInfoList);
	
};
