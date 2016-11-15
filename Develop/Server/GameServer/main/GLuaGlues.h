#pragma once

typedef map<wstring, vector<int> > MAP_NPC_EVENTLIST;
//typedef map<int, wstring > MAP_NPC_EVENTHANDLER;

class GEntityNPC;
class GFieldInfo;
struct MARKER_INFO;
class GSpawnManager;
class GTalentInfoMgr;
class GNPCInfoMgr;
class GNPCMgr;
class GFieldInfoMgr;
class GFieldSystem;
class GEntityPlayer;
class GGlueActor;
class GGlueNPC;
class GGluePlayer;

class GLuaGlues
{
public:
	GLuaGlues(void);
	~GLuaGlues(void);

	// util
	static vec3 LocalPosToWorldPos(vec3 vDir, vec3 vCenterPos, vec3 vAdjPos);
	static string CastString(const char* szStr);
	static float GetDistance(vec3 vPos1, vec3 vPos2);
	static MWLua::table GetPolygonPoints(vec3 vCenter, vec3 vDir, float fDistance, int nPolyQty);
	static vec3 GetDistancePoint(vec3 vPos, vec3 vDir, float fDistance);
	static void ReserveUserData(int nNPCID, int nSize);
	static const char* Format(const char* szFormat, const char* param0, const char* param1, const char* param2, const char* param3, const char* param4, const char* param5, const char* param6);
	// util

	
	static GEntityNPC* GetNPC(MUID nID);
	static int GetHP(MUID nID);
	static int GetEN(MUID nID);
	static int GetSTA(MUID nID);

	static const MARKER_INFO* GetMarker(int nFieldID, int nID);
	

	// Actor
	static int GetEnemyHP(MUID eID);
	static bool IsIdle(MUID nID);
	static vec3 GetPos(MUID nID);
	static void SetPosition(MARKER_INFO Marker, MUID uidPlayer, bool bGroupMove);

	// NPC
	static void DoAttack(MUID nID);
	static void DoMove(MUID nID, vec3 vecTar);


	// Log
	static void OutDebugUID(MUID uid);
	static void OutDebugStr(const char* szStr);
	static void OutDebugInt(int n);
	static void GLog(const char* szStr);
	static void GLog0(const char* szStr);
	static void GLog1(const char* szStr);
	static void GLog2(const char* szStr);
	static void GLog3(const char* szStr);
	static void GLogFilter(const char* szFilter, const char* szStr);

	static MAP_NPC_EVENTLIST mapNPCEvents;
//	static MAP_NPC_EVENTHANDLER mapNPCHandler;

	//static bool	IsFindMarker(int nFieldID, int nID);
	
	//static MARKER_INFO* GetMarker(wstring strName);
	
	static void SetFatigue(GEntityPlayer* pPlayer, int nFatiguePercent);
	static GGlueNPC* AsNPC(GGlueActor* pActor);
	static GGluePlayer* AsPlayer(GGlueActor* pActor);

private:
	static string sm_strFormatResult;
};
