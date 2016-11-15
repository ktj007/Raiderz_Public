#include "stdafx.h"
//#include "GJob_Curve.h"
//#include "GModuleAI.h"
//#include "GEntityNPC.h"
//
//GJob_Curve::GJob_Curve(GModuleAI* pmAI, vec3 vTarPos) 
//: GJob(pmAI)
//, m_vTarPos(vTarPos)
//{
//	
//}
//
//GJob_Curve::~GJob_Curve()
//{
//
//}
//
//bool GJob_Curve::OnStart()
//{
//	float			fSpeed=0.0f;
//	bool			bRun=false;
//
//	SetRunningTask(m_TaskBroker.New_Curve(GetOwnerNPC(), m_vTarPos));
//
//	return true;
//}
//
//GJobResult GJob_Curve::OnRun(float fDelta)
//{
//	return JR_RUNNING;
//}
//
//void GJob_Curve::OnComplete()
//{
//	GetOwnerNPC()->RouteMoveStop();
//}
//
//void GJob_Curve::OnCancel()
//{
//	GetOwnerNPC()->RouteMoveStop();
//}
