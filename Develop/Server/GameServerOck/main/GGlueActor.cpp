#include "stdafx.h"
#include "GGlueActor.h"
#include "GEntityActor.h"
#include "GField.h"
#include "GNullGlueField.h"
#include "GScriptUtil.h"
#include "GGlobal.h"
#include "GTestSystem.h"
#include "MLocale.h"
#include "GModuleBuff.h"
#include "GScriptGroup_Field.h"

GGlueActor::GGlueActor(GEntityActor* pOwner)
{
	m_pOwner = pOwner;
	m_szName[0] = '\0';
}

GGlueActor::~GGlueActor()
{
	// do nothing
}

GEntityActor* GGlueActor::GetOwnerActor() const
{
	return m_pOwner;
}

bool GGlueActor::IsInvalidOwner() const
{
	return m_pOwner == NULL;
}

bool GGlueActor::IsNull(void) const
{
	if (IsInvalidOwner())		return true;
	return false;
}

bool GGlueActor::Warp(vec3 vPos)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vPos);

	if (IsInvalidOwner())		return false;
	GetOwnerActor()->Warp(vPos);
	return true;
}

bool GGlueActor::CheckBuff(int nBuffID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerActor()->HasBuff(nBuffID);
}

int GGlueActor::GetDebuffCount(void)
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerActor()->GetDebuffQty();
}

bool GGlueActor::GainBuff(int nBuffID )
{
	if (IsInvalidOwner())		return false;
	return GetOwnerActor()->GainBuff(nBuffID );
}

bool GGlueActor::GainBuffDetail(int nBuffID, float fDuration, float fPeriod )
{
	if (IsInvalidOwner())		return false;
	return GetOwnerActor()->GainBuffDetail(nBuffID, fDuration, fPeriod );
}

bool GGlueActor::RemoveBuff(int nBuffID)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerActor()->RemoveBuff(nBuffID);
}

void GGlueActor::RemoveAllBuff()
{
	if (IsInvalidOwner())		return;
	GetOwnerActor()->GetModuleBuff()->LazyLostAll();
}

MUID GGlueActor::GetUID(void)
{
	if (IsInvalidOwner())		return 0;
	return GetOwnerActor()->GetUID();
}

vec3 GGlueActor::GetDir(void)
{
	if (IsInvalidOwner())		return vec3::ZERO;
	return GetOwnerActor()->GetDir();
}

vec3 GGlueActor::GetPos(void)
{
	if (IsInvalidOwner())		return vec3::ZERO;
	return GetOwnerActor()->GetPos();
}

float GGlueActor::DistanceTo(GGlueActor* pTargetActorDelegator)
{
	if (IsInvalidOwner())		return -1.0f;

	if (pTargetActorDelegator == NULL || pTargetActorDelegator->IsNull())
	{
		return -1.0f;
	}

	GEntityActor* pActor = pTargetActorDelegator->GetOwnerActor();
	return GetOwnerActor()->DistanceTo(pActor);
}

vec3 GGlueActor::GetFowardDir( GGlueActor* pTargetActorDelegator )
{
	if (IsInvalidOwner())		return vec3::ZERO;

	if (pTargetActorDelegator == NULL || pTargetActorDelegator->IsNull())
	{
		return vec3::ZERO;
	}

	vec3 vDir = pTargetActorDelegator->GetPos() - GetOwnerActor()->GetPos();
	vDir.z = 0.0f;
	vDir.Normalize();

	return vDir;
}

MF_STATE GGlueActor::GetCurrentMotionFactor(void)
{
	if (IsInvalidOwner())		return MF_NONE;
	return GetOwnerActor()->GetCurrentMotionFactor();
}

int GGlueActor::GetFieldID(void)
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerActor()->GetFieldID();
}

bool GGlueActor::IsPlayer(void)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerActor()->IsPlayer();
}

bool GGlueActor::IsNPC(void)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerActor()->IsNPC();
}

bool GGlueActor::SetMFModValue(int nType, float fValue)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerActor()->SetMFModValue(nType, fValue);
}

bool GGlueActor::SetMFModWeight(int nType, float fWeight)
{
	if (IsInvalidOwner())		return false;
	return GetOwnerActor()->SetMFModWeight(nType, fWeight);
}

GGlueField* GGlueActor::GetField(void)
{
	if (IsInvalidOwner())		return GNullGlueField::GetNull();
	GField* pField = GetOwnerActor()->GetField();
	return TO_SAFE_LUAPTR(pField);
}

int GGlueActor::GetMaxHP(void)
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerActor()->GetMaxHP();
}

int GGlueActor::GetMaxEN(void)
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerActor()->GetMaxEN();
}

int GGlueActor::GetMaxSTA(void)
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerActor()->GetMaxSTA();
}

int GGlueActor::GetHP(void)
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerActor()->GetHP();
}

int GGlueActor::GetEN(void)
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerActor()->GetEN();
}

int GGlueActor::GetSTA(void)
{
	if (IsInvalidOwner())		return -1;
	return GetOwnerActor()->GetSTA();
}

bool GGlueActor::Narration(const char* szText)
{
	if (IsInvalidOwner())		return false;

	GetOwnerActor()->Narration(MLocale::ConvAnsiToUCS2(szText).c_str());
	return true;
}

bool GGlueActor::NarrationNow(const char* szText)
{
	if (IsInvalidOwner())		return false;

	GetOwnerActor()->NarrationNow(MLocale::ConvAnsiToUCS2(szText).c_str());
	return true;
}

const char* GGlueActor::GetName(void)
{
	if (IsInvalidOwner())		return "unknown_name";
	
	strncpy_s(m_szName, MLocale::ConvUTF16ToAnsi(GetOwnerActor()->GetName()).c_str(), _TRUNCATE);	
	return m_szName;
}

bool GGlueActor::IsRear(GGlueActor* pTarget)
{
	if (IsInvalidOwner())		
		return false;
	if (pTarget == NULL || pTarget->IsNull())
		return false;

	return pTarget->GetOwnerActor()->IsBack(GetOwnerActor()->GetPos());
}

bool GGlueActor::IsFront( GGlueActor* pTarget )
{
	return !IsRear(pTarget);
}

bool GGlueActor::IsDead() const
{
	if (IsInvalidOwner())		
		return true;

	return GetOwnerActor()->IsDead();
}

void GGlueActor::MakePeace()
{
	if (IsInvalidOwner())		
		return;

	GetOwnerActor()->doMakePeace();
}

int GGlueActor::GetLevel() const
{
	if (IsInvalidOwner())		
		return -1;

	return GetOwnerActor()->GetLevel();
}

void GGlueActor::RunFieldEvent( const char* pszName )
{
	if (IsInvalidOwner())		
		return;

	GField* pField = GetOwnerActor()->GetField();
	if (!pField)
		return;

	GetScriptField()->OnEvent(pField->GetDelegator(), this, pszName);
}
