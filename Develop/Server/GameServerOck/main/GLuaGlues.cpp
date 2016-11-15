#include "StdAfx.h"
#include "GLuaGlues.h"
#include "GField.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "GSpawnManager.h"
#include "GNPCInfoMgr.h"
#include "GFieldSystem.h"
#include "GMessageID.h"
#include "GQuestSystem.h"
#include "GTrainingSystem.h"
#include "GCommandCenter.h"
#include "GConditionsSystem.h"
#include "GFatigueSystem.h"
#include "GInteractionSystem.h"
#include "GFieldMgr.h"
#include "GCommand.h"
#include "GPlayerObjectManager.h"
#include "GStrings.h"
#include "GMath.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GNullGlueNPC.h"
#include "GNullGluePlayer.h"
#include "GFieldInfoMgr.h"
#include "GJobMgr.h"
#include "GTestSystem.h"
#include "GMsgCommandFacade.h"
#include "GConst.h"
#include "GAIMonitorPort.h"


string GLuaGlues::sm_strFormatResult;


GLuaGlues::GLuaGlues(void)
{
}

GLuaGlues::~GLuaGlues(void)
{
}

vec3 GLuaGlues::LocalPosToWorldPos(vec3 vDir, vec3 vCenterPos, vec3 vAdjPos)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vDir);
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vCenterPos);
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vAdjPos);

	return GMath::LocalPosToWorldPos(vDir, vCenterPos, vAdjPos);
}

float GLuaGlues::GetDistance(vec3 vPos1, vec3 vPos2)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vPos1);
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vPos2);

	return vPos1.DistanceTo(vPos2);
}

MWLua::table GLuaGlues::GetPolygonPoints(vec3 vCenter, vec3 vDir, float fDistance, int nPolyQty)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vCenter);
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vDir);

	vector<vec3> vecPos;
	GMath::GetAnglePos(vCenter, vDir, fDistance, nPolyQty, vecPos);

	MWLua::table t(WLUA->GetState());

	char buf[64];
	for (int i=0; i<(int)vecPos.size(); ++i)
	{
		_itoa_s(i+1, buf, 10);
		t.set(buf, vecPos[i]);
	}

	return t;
}

vec3 GLuaGlues::GetDistancePoint(vec3 vPos, vec3 vDir, float fDistance)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vPos);
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vDir);

	return vPos + (vDir * fDistance);
}

void GLuaGlues::ReserveUserData(int nNPCID, int nSize)
{
	GEntityNPC::ReserveUserdata(nNPCID, nSize);
}

const char* GLuaGlues::Format(const char* szFormat, const char* param0, const char* param1, const char* param2, const char* param3, const char* param4, const char* param5, const char* param6)
{
	// 인자 문자열 연결 - $$KEY{param=...|...|...}
	
	sm_strFormatResult = szFormat;
	string strSeparator = "|";
	
	if (param0 != NULL)
	{
		sm_strFormatResult += "{param=";
		sm_strFormatResult += param0;

		if (param1 != NULL)	sm_strFormatResult += strSeparator + param1;
		if (param2 != NULL)	sm_strFormatResult += strSeparator + param2;
		if (param3 != NULL)	sm_strFormatResult += strSeparator + param3;
		if (param4 != NULL)	sm_strFormatResult += strSeparator + param4;
		if (param5 != NULL)	sm_strFormatResult += strSeparator + param5;
		if (param6 != NULL)	sm_strFormatResult += strSeparator + param6;

		sm_strFormatResult += "}";
	}

	return sm_strFormatResult.c_str();
}

GEntityNPC* GLuaGlues::GetNPC(MUID nID)
{
	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(nID);
	if (!pNPC)
	{
		mlog("GLuaGlues::GetNPC(): invalid npc uid.\n");
		return false;
	}

	return pNPC;
}

/*
GEntityNPC* GLuaGlues::GetSpawnObject(int nFieldID, int nSpawnID, MUID uidField)
{
	GField* pField = NULL;
	
	if (uidField.IsInvalid())
		return NULL;

	pField = gmgr.pFieldMgr->GetField(uidField);
	assert(pField);
	if (pField == NULL) return NULL;

	MUID uidNPC = pField->GetSpawn()->GetSpawnInstanceUID(nSpawnID);
	if (uidNPC.IsInvalid())	return NULL;

	GEntityNPC* pNPC = static_cast<GEntityNPC*>(pField->GetEntity(uidNPC));
	assert(pNPC);
	if (pNPC == NULL) return NULL;

	return pNPC;
}
*/
int GLuaGlues::GetHP(MUID nID)
{
	GEntityNPC* pNPC = GetNPC(nID);

	if(pNPC != NULL)
		return pNPC->GetHP();

	return 0;
}

int GLuaGlues::GetEN(MUID nID)
{
	GEntityNPC* pNPC = GetNPC(nID);

	if(pNPC != NULL)
			return pNPC->GetEN();

	return 0;
}

int GLuaGlues::GetSTA(MUID nID)
{
	GEntityNPC* pNPC = GetNPC(nID);

	if(pNPC != NULL)
		return pNPC->GetSTA();

	return 0;
}

int GLuaGlues::GetEnemyHP(MUID eID)
{
	return 100;
}

void GLuaGlues::OutDebugUID(MUID uid)
{
	wchar_t str[64]=L"";
	swprintf_s(str, L"%I64u\n", uid);
	OutputDebugStr(str);
}

void GLuaGlues::OutDebugStr(const char* szStr)
{
	wchar_t str[64]=L"";
	swprintf_s(str, L"%s\n", MLocale::ConvAnsiToUCS2(szStr).c_str());
	OutputDebugStr(str);
}

void GLuaGlues::OutDebugInt(int n)
{
	wchar_t str[64]=L"";
	swprintf_s(str, L"%d\n", n);
	OutputDebugStr(str);
}

void GLuaGlues::DoAttack(MUID nID)
{
	GEntityNPC* pNPC = GetNPC(nID);

	GMessage msg(GMSG_AI_EVT_STARTCOMBAT);
	pNPC->Message(msg);
}	

bool GLuaGlues::IsIdle(MUID nID)
{
	GEntityNPC* pNPC = GetNPC(nID);
	assert(pNPC);

	GJobMgr* pJobMgr = pNPC->GetJobMgr();
	return pJobMgr->IsEmpty();
}

vec3 GLuaGlues::GetPos(MUID nID)
{
	GEntityNPC* pNPC = GetNPC(nID);
	assert(pNPC);

	return pNPC->GetPos();
}

void GLuaGlues::DoMove(MUID nID, vec3 vecTar)
{
	GEntityNPC* pNPC = GetNPC(nID);
	assert(pNPC);
	assert(pNPC->GetNPCInfo());

	pNPC->Move(vecTar);
}


const MARKER_INFO* GLuaGlues::GetMarker(int nFieldID, int nID)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (pFieldInfo == NULL)		
	{
		if (IsRunForTest())
		{
			char text[1024] = "";
			mlog3(text, "GLuaGlues::GetMarker() - 필드가 존재하지 않습니다. (요청 FieldID:%d, MarkerID:%d)", nFieldID, nID);
		}

		return NULL;
	}


	
	const MARKER_INFO* pMarker = pFieldInfo->FindMarker(nID);
	if (!pMarker)	
	{
		if (IsRunForTest())
		{
			char text[1024] = "";
			mlog3(text, "GLuaGlues::GetMarker() - 마커가 존재하지 않습니다. (요청 FieldID:%d, MarkerID:%d)", nFieldID, nID);
		}

		return NULL;
	}

	return pMarker;
}

void GLuaGlues::SetPosition(MARKER_INFO Marker, MUID uidPlayer, bool bGroupMove)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pEntityPlayer) return;

	pEntityPlayer->GetPlayerField().Gate(Marker.nFieldID, Marker.vPoint, Marker.vDir);
}

void GLuaGlues::GLog(const char* szStr)
{
	MGetDefaultLogger()->Output(MLOG_FILE, szStr, MLOG_LEVEL_DEBUG);
	mlog(szStr);
	GAIMonitorPort::Inst().SendGameLogToViewer(MLOG_LEVEL_DEBUG, szStr);
}

void GLuaGlues::GLog0(const char* szStr)
{
	mlog0(szStr);
	GAIMonitorPort::Inst().SendGameLogToViewer(MLOG_LEVEL_DEBUG, szStr);
}

void GLuaGlues::GLog1(const char* szStr)
{
	mlog1(szStr);
	GAIMonitorPort::Inst().SendGameLogToViewer(MLOG_LEVEL_INFORMATION, szStr);
}

void GLuaGlues::GLog2(const char* szStr)
{
	mlog2(szStr);
	GAIMonitorPort::Inst().SendGameLogToViewer(MLOG_LEVEL_WARNING, szStr);
}

void GLuaGlues::GLog3(const char* szStr)
{
	mlog3(szStr);
	GAIMonitorPort::Inst().SendGameLogToViewer(MLOG_LEVEL_ERROR, szStr);
}

void GLuaGlues::GLogFilter(const char* szFilter, const char* szStr)
{
	mlog_filter(szFilter, szStr);
}

//유틸
string GLuaGlues::CastString(const char* szStr)
{
	return szStr;
}

void GLuaGlues::SetFatigue(GEntityPlayer* pPlayer, int nFatiguePercent)
{
	gsys.pFatigueSystem->SetFatigueByCommand(pPlayer, nFatiguePercent);
}


GGlueNPC* GLuaGlues::AsNPC(GGlueActor* pActor)
{
	if (!pActor)				return GNullGlueNPC::GetNull();
	if (pActor->IsNull())		return GNullGlueNPC::GetNull();
	if (!pActor->IsNPC())		return GNullGlueNPC::GetNull();
	return static_cast<GGlueNPC*>(pActor);
}

GGluePlayer* GLuaGlues::AsPlayer(GGlueActor* pActor)
{
	if (!pActor)				return GNullGluePlayer::GetNull();
	if (pActor->IsNull())		return GNullGluePlayer::GetNull();
	if (!pActor->IsPlayer())	return GNullGluePlayer::GetNull();
	return static_cast<GGluePlayer*>(pActor);
}
