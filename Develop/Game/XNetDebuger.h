#pragma once

// Navigation-mesh 렌더링을 위해 ...
#include "MPathFinder.h"

class XNetDebuger
{
private:
	static bool m_bInited;
	static void BirdHandle(int nParam, wstring& text);
	static void BirdDebugRender();
	static void NPCMovePathRender();
	static void ActorPosUpdateRender();
	static void SignalSphereRender();
	static void PickingRender();
	static void CreateNavigationTriangles(wstring &strName);
	static void NPCMonitor(int nParam, wstring& text);
	static void SetNPCMonitorString(int nParam, wstring& text);
	static void HitCapsules();
	
	static MPathFinder PathFinder;
public:
	static void Init();
	static void Handle(wstring& strKey, int nParam, wstring& text, MCommand* pCommand);
	static void DebugRender();

	static void DebugCombatCalc(wstring& strType, wstring& strMessage);
	static void DebugCombatRender();

	static void DebugCombatRender(MDrawContext* pDC, MRECT r);
	static void DebugNPCMonitorRender(MDrawContext* pDC, MRECT r);

	static void SaveMonitorLog();

	static void PostAIMonitorLockTarget(UIID uiidTarget);
};