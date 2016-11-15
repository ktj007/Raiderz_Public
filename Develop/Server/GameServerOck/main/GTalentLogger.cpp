#include "StdAfx.h"
#include "GTalentLogger.h"
#include "GTalent.h"
#include "GEntityActor.h"

void __cdecl GTalentLogger::Log( const wchar_t* pFormat, ... )
{
	wchar_t temp[1024];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);
	mlog(temp);
}

void GTalentLogger::OnStart( GTalent* pTalent )
{
	PFI_B(3610, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnStart>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3610);
}

void GTalentLogger::OnExit( GTalent* pTalent )
{
	PFI_B(3620, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnExit>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3620);
}

void GTalentLogger::OnEnterPhasePrepare( GTalent* pTalent )
{
	PFI_B(3630, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnEnterPhasePrepare>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3630);
}

void GTalentLogger::OnEnterPhaseAct( GTalent* pTalent )
{
	PFI_B(3640, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnEnterPhaseAct>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3640);
}

void GTalentLogger::OnEnterPhaseCanceled( GTalent* pTalent )
{
	PFI_B(3650, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnEnterPhaseCanceled>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3650);
}

void GTalentLogger::OnEnterPhaseFinish( GTalent* pTalent )
{
	PFI_B(3660, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnEnterPhaseFinish>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3660);
}

void GTalentLogger::OnEnterPhaseExtra( GTalent* pTalent )
{
	PFI_B(3670, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnEnterPhaseExtra>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3670);
}

void GTalentLogger::OnLeavePhaseAct( GTalent* pTalent )
{
	PFI_B(3680, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnLeavePhaseAct>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3680);
}

void GTalentLogger::OnLeavePhaseCanceled( GTalent* pTalent )
{
	PFI_B(3690, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnLeavePhaseCanceled>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3690);
}

void GTalentLogger::OnLeavePhasePrepare( GTalent* pTalent )
{
	PFI_B(3691, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnLeavePhasePrepare>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3691);
}

void GTalentLogger::OnLeavePhaseFinish( GTalent* pTalent )
{
	PFI_B(3692, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnLeavePhaseFinish>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3692);
}

void GTalentLogger::OnLeavePhaseExtra( GTalent* pTalent )
{
	PFI_B(3693, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnLeavePhaseExtra>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3693);
}

void GTalentLogger::OnUpdatePhasePrepare( GTalent* pTalent, float fDelta )
{
}

void GTalentLogger::OnUpdatePhaseAct( GTalent* pTalent, float fDelta )
{
}

void GTalentLogger::OnUpdatePhaseExtra( GTalent* pTalent, float fDelta )
{
}

void GTalentLogger::OnUpdatePhasePreAct( GTalent* pTalent, float fDelta )
{

}

void GTalentLogger::OnEnterPhaseExtra2( GTalent* pTalent )
{
	PFI_B(3694, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnEnterPhaseExtra2>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3694);
}

void GTalentLogger::OnLeavePhaseExtra2( GTalent* pTalent )
{
	PFI_B(3695, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnLeavePhaseExtra2>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3695);
}

void GTalentLogger::OnUpdatePhaseExtra2( GTalent* pTalent, float fDelta )
{

}

void GTalentLogger::OnEnterPhaseExtra3( GTalent* pTalent )
{
	PFI_B(3696, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnEnterPhaseExtra3>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3696);
}

void GTalentLogger::OnLeavePhaseExtra3( GTalent* pTalent )
{
	PFI_B(3697, "GTalentLogger::OnStart");
	Log(L"[TalentLog] Owner:%I64d, ID:%d, Elasped:%.1f, Acttime:%.1f, <OnLeavePhaseExtra3>\n", pTalent->GetOwner()->GetUID(), pTalent->GetID(), pTalent->GetElapsedTime(), pTalent->GetActElapsedTime());
	PFI_E(3697);
}

void GTalentLogger::OnUpdatePhaseExtra3( GTalent* pTalent, float fDelta )
{

}