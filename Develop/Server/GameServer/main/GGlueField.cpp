#include "stdafx.h"
#include "GGlueField.h"
#include "GField.h"
#include "GNullGlueNPC.h"
#include "GEntityNPC.h"
#include "GScriptUtil.h"
#include "MLocale.h"
#include "GGlobal.h"
#include "GMsgCommandFacade.h"
#include "GQuestPVP.h"
#include "GNullGlueQuestPVP.h"
#include "GConst.h"
#include "GFieldNPCSession.h"
#include "GFieldNPCSessionMgr.h"

GGlueField::GGlueField(GField* pOwner)
{
	m_pOwner = pOwner;
}

GGlueField::~GGlueField()
{
	// do nothing
}

GField* GGlueField::GetOwnerField() const
{
	return m_pOwner;
}

bool GGlueField::IsInvalidOwner() const
{
	return m_pOwner == NULL;
}

const string GGlueField::GetLuaTableName()
{
	if (NULL != GetOwnerField())
	{
		return MAKE_LUATABLE_FIELD(GetOwnerField()->GetID());	
	}

	return "";	
}

bool GGlueField::IsNull(void) const
{
	if (IsInvalidOwner())		return true;
	return false;
}

int GGlueField::GetID()
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerField()->GetID();
}

GGlueNPC* GGlueField::Spawn(int nNPCID, vec3 vStartPos)
{
	if (GConst::DEBUG_DISABLE_DEFAULT_FIELD_SPAWN)
		return GNullGlueNPC::GetNull();

	if (IsInvalidOwner())		return GNullGlueNPC::GetNull();
	
	GEntityNPC* pNpc = GetOwnerField()->Spawn(nNPCID, vStartPos);
	return TO_SAFE_LUAPTR(pNpc);	
}

void GGlueField::Despawn(int nNPCID, bool bSpawnEnable)
{
	if (IsInvalidOwner())		return;

	GetOwnerField()->Despawn(nNPCID, bSpawnEnable);
}

void GGlueField::DespawnNow( int nNPCID, bool bSpawnEnable )
{
	if (IsInvalidOwner())		return;

	GetOwnerField()->DespawnNow(nNPCID, bSpawnEnable);
}

GGlueNPC* GGlueField::SpawnLimited(int nNPCID, vec3 vStartPos, float fLimitTime)
{
	if (GConst::DEBUG_DISABLE_DEFAULT_FIELD_SPAWN)
		return GNullGlueNPC::GetNull();

	if (IsInvalidOwner())		return GNullGlueNPC::GetNull();

	GEntityNPC* pNpc = GetOwnerField()->Spawn(nNPCID, vStartPos, 0.0f, fLimitTime);
	return TO_SAFE_LUAPTR(pNpc);	
}

void GGlueField::SpawnDelay(int nNPCID, vec3 vStartPos, float fDelayTime)
{
	if (GConst::DEBUG_DISABLE_DEFAULT_FIELD_SPAWN)
		return;

	if (IsInvalidOwner())		return;

	GetOwnerField()->Spawn(nNPCID, vStartPos, fDelayTime);
}

int GGlueField::GetNPCCount(int nNPCID)
{
	if (IsInvalidOwner())		return 0;
	return GetOwnerField()->GetNPCQty(nNPCID);
}

GGlueNPC* GGlueField::GetNPC(int nNPCID)
{
	if (IsInvalidOwner())		return 0;

	GEntityNPC* pNPC = GetOwnerField()->FindOneNPCByID(nNPCID);
	return TO_SAFE_LUAPTR(pNPC);
}

void GGlueField::EnableInteraction(int nNPCID)
{
	if (IsInvalidOwner())		return;
	return GetOwnerField()->EnableInteraction(nNPCID);
}

void GGlueField::DisableInteraction(int nNPCID)
{
	if (IsInvalidOwner())		return;
	return GetOwnerField()->DisableInteraction(nNPCID);
}

GGlueNPC* GGlueField::SpawnByID(unsigned long nSpawnID)
{
	if (GConst::DEBUG_DISABLE_DEFAULT_FIELD_SPAWN)
		return GNullGlueNPC::GetNull();

	if (IsInvalidOwner())		return GNullGlueNPC::GetNull();
	GEntityNPC* pNpc = GetOwnerField()->Spawn(nSpawnID);
	return TO_SAFE_LUAPTR(pNpc);	
}

void GGlueField::SpawnDelayByID(unsigned long nSpawnID, float fDelayTime)
{
	if (GConst::DEBUG_DISABLE_DEFAULT_FIELD_SPAWN)
		return;

	if (IsInvalidOwner())		return;
	GetOwnerField()->Spawn(nSpawnID, fDelayTime);	
}

GGlueNPC* GGlueField::DespawnByID(unsigned long nSpawnID, bool bSpawnEnable)
{
	if (IsInvalidOwner())		return GNullGlueNPC::GetNull();
	GEntityNPC* pNpc = GetOwnerField()->Despawn(nSpawnID, bSpawnEnable);
	return TO_SAFE_LUAPTR(pNpc);
}

GGlueNPC* GGlueField::GetSpawnNPC(unsigned long nSpawnID)
{
	if (IsInvalidOwner())		return GNullGlueNPC::GetNull();
	GEntityNPC* pNpc = GetOwnerField()->GetSpawnNPC(nSpawnID);
	return TO_SAFE_LUAPTR(pNpc);	
}

bool GGlueField::EnableSensor(int nSensorID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerField()->EnableSensor(nSensorID, true);
}

bool GGlueField::DisableSensor(int nSensorID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerField()->EnableSensor(nSensorID, false);
}

bool GGlueField::CheckSensor(int nSensorID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerField()->IsEnableSensor(nSensorID);
}

bool GGlueField::SetTimer(int nTimerID, float fIntervalTime, bool bRepeat)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerField()->SetTimer(nTimerID, fIntervalTime, bRepeat);
}

bool GGlueField::KillTimer(int nTimerID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerField()->KillTimer(nTimerID);
}

vec3 GGlueField::GetMarkerPos(int nMarkerID)
{
	if (IsInvalidOwner())		return vec3::ZERO;
	return GetOwnerField()->GetMarkerPos(nMarkerID);
}

vec3 GGlueField::GetMarkerDir(int nMarkerID)
{
	if (IsInvalidOwner())		return vec3::ZERO;
	return GetOwnerField()->GetMarkerDir(nMarkerID);
}

void GGlueField::ActivateSpawnGroup(int nGroupID)
{
	if (IsInvalidOwner())		return;
	
	GetOwnerField()->ActivateSpawnGroup(nGroupID);
}

void GGlueField::PlayBGM( const char* pszSoundname )
{
	if (IsInvalidOwner())		return;

	GetOwnerField()->RoutePlayBGM(MLocale::ConvAnsiToUCS2(pszSoundname).c_str());
}

void GGlueField::StopBGM()
{
	if (IsInvalidOwner())		return;

	GetOwnerField()->RoutePlayBGM(L"");
}


void GGlueField::Notice( const char* pszMsg )
{
	if (IsInvalidOwner())		return;

	gsys.pMsgCommandFacade->RouteNoticeMsgToAll(MLocale::ConvAnsiToUCS2(pszMsg).c_str());
}

void GGlueField::NoticeToField( const char* pszMsg )
{
	if (IsInvalidOwner())		return;

	gsys.pMsgCommandFacade->RouteNoticeMsgToField(m_pOwner, MLocale::ConvAnsiToUCS2(pszMsg).c_str());
}

void GGlueField::ChangeWeather( int nWeatherType )
{
	if (IsInvalidOwner())		return;

	if (nWeatherType < 0 || nWeatherType >= MAX_WEATHER_TYPE ) return;

	GAME_WEATHER_TYPE nGameWeatherType = GAME_WEATHER_TYPE(nWeatherType);

	GetOwnerField()->ChangeWeather(nGameWeatherType);
}

GGlueQuestPVP* GGlueField::GetQuestPVP()
{
	if (IsInvalidOwner())		return GNullGlueQuestPVP::GetNull();

	GQuestPVP* pQuestPVP = GetOwnerField()->GetQuestPVP();
	return TO_SAFE_LUAPTR(pQuestPVP);
}

GFieldNPCSession* GGlueField::MakeSession(const char* szName, MWLua::table tableNPCs)
{
	if (IsInvalidOwner())		return NULL;

	if (!szName)
		return NULL;
	
	GFieldNPCSession* session = GFieldNPCSession::New(szName, m_pOwner->GetUID());
	if (!m_pOwner->GetSession()->Add(session))
	{
		delete session;
		return NULL;
	}

	tableNPCs.iteration();
	GGlueNPC* pNPC;
	while (tableNPCs.next<GGlueNPC*>(&pNPC))  
	{ 
		session->AddNPC_ForGlue(pNPC);
	} 

	return session;
}

GFieldNPCSession* GGlueField::FindSession(const char* szName)
{
	if (IsInvalidOwner())		return NULL;

	return m_pOwner->GetSession()->Find(szName);
}

