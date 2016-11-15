#include "stdafx.h"
#include "XNetDebuger.h"
#include "XNonPlayer.h"
#include "RConfiguration.h"
#include "XModuleEntity.h"
#include "XModuleUI.h"
#include "XMyPlayer.h"
#include "XMovableDebugger.h"
#include <vector>
#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"
#include "XPost_GM.h"

using namespace std;
using namespace rs3;

RCapsule testCapsule[4];
RCapsule testAttackCapsule;
bool bDraw = false;

vector<MVector3> vNavigationVertices;
vector<MVector3> vObstacleVertices;
vector<MVector3> vCollisionVertices;

bool XNetDebuger::m_bInited = false;
MPathFinder XNetDebuger::PathFinder;

struct stDebugCombatCalc
{
	int		nIndex;
	wstring	strType;
	wstring	strMessage;
};
vector<stDebugCombatCalc> vecDebugCombatCalc;
vector<wstring> vecNPCMonitorString;
static const int NPC_MONITOR_MAX_LINE = 30;


#include "XPlayer.h"

wchar_t szDamageText[2][256] = {L"", L""};

void XNetDebuger::Handle( wstring& strKey, int nParam, wstring& text, MCommand* pCommand )
{
	if (!m_bInited) return;

	if (strKey == L"birdkr")
	{
		BirdHandle(nParam, text);
	}
	else if (strKey == L"MovePathTrace")
	{
		MUID uidTarget;
		uidTarget.Low = nParam;
		uidTarget.High = _wtoi(text.c_str());

		MCommandParameter* pParam = pCommand->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;
		vec3* pPathList = (vec3*)pParam->GetPointer();

		gvar.Debug.mapNPCMovePathList.erase(uidTarget);
		vector<vec3>& vecEachPathList = gvar.Debug.mapNPCMovePathList[uidTarget];
		vecEachPathList.assign(pPathList, pPathList+pParam->GetBlobCount());
		gvar.Debug.mapNPCMovePathFailed[uidTarget] = false;
	}
	else if (strKey == L"UpdatePosTrace")
	{
		MCommandParameter* pParam = pCommand->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;
		TD_DEBUG_POS* pTD = (TD_DEBUG_POS*)pParam->GetPointer();

		gvar.Debug.vecActorPos.clear();
		for (int i=0; i<pParam->GetBlobCount(); ++i)
		{
			gvar.Debug.vecActorPos.push_back(pTD[i]);
		}
	}
	else if (strKey == L"MovePathFailed")
	{
		MUID uidTarget;
		uidTarget.Low = nParam;
		uidTarget.High = _wtoi(text.c_str());

		MCommandParameter* pParam = pCommand->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;
		vec3* pPathList = (vec3*)pParam->GetPointer();

		gvar.Debug.mapNPCMovePathFailed[uidTarget] = true;
		gvar.Debug.mapNPCMovePathFailedPos[uidTarget] = *(vec3*)pParam->GetPointer();
	}
	else if (strKey == L"LoadNaviMesh")
	{
		wstring strFilename = L"Zone/" + text + L"/" + text;
		wstring strFullname = MLocale::ConvAnsiToUTF16(REngine::GetConfig().GetDataPath(MLocale::ConvUTF16ToAnsi(strFilename.c_str()).c_str()).c_str());

		CreateNavigationTriangles(strFullname);
	}
	else if (strKey == L"TEST_SHOW_CLIENT_NPC_PICKING")
	{
		MCommandParameter* pParam = pCommand->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;
		vec3* pPathList = (vec3*)pParam->GetPointer();

		//gvar.Debug.mapNPCPickingList[pPathList[0]] =  pair<vec3, vec3>(pPathList[0], pPathList[1]);

		if (nParam)
		{
			gvar.Debug.mapNPCValidPickingList.push_back(pair<vec3, vec3>(pPathList[0], pPathList[1]));
		}
		else
		{
			if (gvar.Debug.bWarpToPickingPos)
			{
				vec3 vNewPos = pPathList[0];
				vec3 vDir;

				XPlayer* user = static_cast<XPlayer*>(gg.omgr->Find(XGetMyUID()));
				if (user)
				{
					user->SetPosition(vNewPos);
				}

				gvar.Debug.bWarpToPickingPos = false;
			}

			gvar.Debug.mapNPCPickingList.push_back(pair<vec3, vec3>(pPathList[0], pPathList[1]));
		}

		while (gvar.Debug.mapNPCValidPickingList.size() > 10)
		{
			gvar.Debug.mapNPCValidPickingList.erase(gvar.Debug.mapNPCValidPickingList.begin());
		}

		while (gvar.Debug.mapNPCPickingList.size() > 10)
		{
			gvar.Debug.mapNPCPickingList.erase(gvar.Debug.mapNPCPickingList.begin());
		}
	}
	else if (strKey == L"TEST_SHOW_NPC_SIGNAL_SPHERE")
	{
		MUID uidTarget;
		uidTarget.Low = nParam;
		uidTarget.High = _wtoi(text.c_str());

		MCommandParameter* pParam = pCommand->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;
		TD_DEBUG_SPHERE* pPathList = (TD_DEBUG_SPHERE*)pParam->GetPointer();

		gvar.Debug.mapNPCSignalSphere[uidTarget] = *pPathList;
	}
	else if (strKey == L"monitor")
	{
		//NPCMonitor(nParam, text);

		SetNPCMonitorString(nParam, text);
	}
	else if (strKey == L"damage")
	{
#if 0
		global.ui->OnSystemMsg(text);
#endif
	}
	else if (strKey == L"pos_sync")
	{
		MUID uidTarget;
		uidTarget.Low = nParam;
		uidTarget.High = _wtoi(text.c_str());

		MCommandParameter* pParam = pCommand->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;
		TD_DEBUG_POS* pPosInfos = (TD_DEBUG_POS*)pParam->GetPointer();
		int nBlobCount = pParam->GetBlobCount();

		if (XMovableDebugReporter_Server::GetInstancePtr())
		{
			XMovableDebugReporter_Server::GetInstancePtr()->OnRecvPosSync(pPosInfos, nBlobCount);
		}
	}
	else if(strKey == L"TraceProjectileHitCapsule" ||
		strKey == L"KnockBackMoveCapsule")
	{
		MUID uidTarget;
		uidTarget.Low = nParam;
		uidTarget.High = _wtoi(text.c_str());

		MCommandParameter* pParam = pCommand->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;

		TD_CAPSULE* pCapsuleInfos = (TD_CAPSULE*)pParam->GetPointer();
		int nBlobCount = pParam->GetBlobCount();

		for(int i = 0; i < nBlobCount; i++)
		{
			TD_CAPSULE* pCapsuleInfo = &pCapsuleInfos[i];

			RCapsule capsuleRender;
			capsuleRender.top = pCapsuleInfo->top;
			capsuleRender.bottom = pCapsuleInfo->bottom;
			capsuleRender.height= pCapsuleInfo->height;
			capsuleRender.radius = pCapsuleInfo->radius;
	
			REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugCapsule(capsuleRender, 1.0f, D3DCOLOR_RGBA(255, 255, 0, 64));
		}
	}
	else if(strKey == L"DrawActorCapsules")
	{
		MUID uidTarget;
		uidTarget.Low = nParam;
		uidTarget.High = _wtoi(text.c_str());

		MCommandParameter* pParam = pCommand->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;

		TD_CAPSULE* pCapsuleInfos = (TD_CAPSULE*)pParam->GetPointer();
		int nBlobCount = pParam->GetBlobCount();

		vector<RCapsule> vecCapsules;
		for(int i = 0; i < nBlobCount; i++)
		{
			TD_CAPSULE* pCapsuleInfo = &pCapsuleInfos[i];

			RCapsule capsuleRender;
			capsuleRender.top = pCapsuleInfo->top;
			capsuleRender.bottom = pCapsuleInfo->bottom;
			capsuleRender.height= pCapsuleInfo->height;
			capsuleRender.radius = pCapsuleInfo->radius;

			vecCapsules.push_back(capsuleRender);
		}

		gvar.Debug.mapActorCapsule[uidTarget] = vecCapsules;
	}
	else if(strKey == L"Clear_DrawActorCapsules")
	{
		gvar.Debug.mapActorCapsule.clear();
	}
}

void XNetDebuger::DebugRender()
{
	if (!m_bInited) return;

	BirdDebugRender();
	NPCMovePathRender();
	ActorPosUpdateRender();
	PickingRender();
	SignalSphereRender();
	HitCapsules();
}

void XNetDebuger::Init()
{
	if (m_bInited) return;

	PathFinder.Create();
	vNavigationVertices.clear();
	vObstacleVertices.clear();
	vecDebugCombatCalc.clear();
	vecNPCMonitorString.resize(NPC_MONITOR_MAX_LINE);

	m_bInited = true;
}

void XNetDebuger::CreateNavigationTriangles(std::wstring &strName)
{
	if (!PathFinder.AddZone(0, MLocale::ConvUTF16ToAnsi(strName.c_str())))
		return;

	vNavigationVertices.clear();
	int nFaceCount = PathFinder.GetNavigationMeshFaceCount(0);
	vNavigationVertices.resize(nFaceCount*3);
	PathFinder.GetNavigationMeshFaceList(0,&vNavigationVertices[0], nFaceCount);

	vObstacleVertices.clear();
	PathFinder.GetStaticObstacleDebugMesh(0, vObstacleVertices);

	PathFinder.ClearZone();
}

void XNetDebuger::BirdHandle( int nParam, wstring& text )
{
	return;

	bDraw = true;
	vec3 t, b;
	float r=0.0f;
	int ret = swscanf(text.c_str(), L"%f %f %f %f %f %f %f", &t.x, &t.y, &t.z, &b.x, &b.y, &b.z, &r);

	if (nParam == 1)
	{
		testAttackCapsule.top = t;
		testAttackCapsule.bottom = b;
		testAttackCapsule.radius = r;
	}
	if (nParam >= 2)
	{
		testCapsule[nParam-2].top = t;
		testCapsule[nParam-2].bottom = b;
		testCapsule[nParam-2].radius = r;
	}
}

void XNetDebuger::BirdDebugRender()
{
	if (!bDraw) return;

	XRenderHelper::SetRenderState();

	MMatrix matWorld = mat4::IDENTITY;

	for (int i = 0; i < 4; i++)
		RRenderHelper::RenderCapsule(matWorld, testCapsule[i], 0x66FFFF00);
	RRenderHelper::RenderCapsule(matWorld, testAttackCapsule, 0x66FF00FF);

	XRenderHelper::EndRenderState();
}

void XNetDebuger::NPCMovePathRender()
{
	if (!gvar.Debug.bRenderMoveDebug) return;

	map<MUID, vector<vec3> >::iterator iter = gvar.Debug.mapNPCMovePathList.begin();
	MMatrix matWorld = mat4::IDENTITY;
	
	XRenderHelper::SetRenderState();
	vec3 vOldPos;
	float fCylinderHeight = 150.0f;
	wchar_t buff[128];

	int nIterCounter = 0;
	for (iter;
		iter != gvar.Debug.mapNPCMovePathList.end();
		++iter)
	{
		nIterCounter++;

		MUID uidTarget = iter->first;
		vector<vec3>& vecEachPathList = iter->second;

		XNonPlayer* pNPC = gg.omgr->FindNPC(uidTarget);
		if (NULL == pNPC) continue;

		if (gvar.Debug.mapNPCMovePathFailed.find(uidTarget) != gvar.Debug.mapNPCMovePathFailed.end() &&
			gvar.Debug.mapNPCMovePathFailedPos.find(uidTarget) != gvar.Debug.mapNPCMovePathFailedPos.end())
		{
			if (gvar.Debug.mapNPCMovePathFailed[uidTarget])
			{
				// path build failed
				matWorld.SetTranslation(pNPC->GetPosition());
				XRenderHelper::RenderRing(matWorld, pNPC->GetRadius()*2, fCylinderHeight, 0xaaff0000);
//				vecEachPathList.clear();

				vec3 vPoint = gvar.Debug.mapNPCMovePathFailedPos[uidTarget];
				matWorld.SetTranslation(vPoint);
				RRenderHelper::RenderCylinder(matWorld, 10, fCylinderHeight, 0xffff00aa);

				vec3 vTextPos = vPoint;
				vTextPos.z += fCylinderHeight+5.0f;
				XRenderHelper::DrawText(vec3(vTextPos), MCOLOR(0x66aaffaa), L"Destination");

				matWorld = mat4::IDENTITY;
				vec3 vNewPos = vPoint;
				vec3 vOldPos = pNPC->GetPosition();
				vNewPos.z += fCylinderHeight/2;
				vOldPos.z += fCylinderHeight/2;
				XRenderHelper::RenderLine(matWorld, vOldPos, vNewPos, 0xffff00aa);

				continue;
			}
		}

		for (size_t i=0; i<vecEachPathList.size(); i++)
		{
			vec3 vPoint = vecEachPathList[i];

			DWORD nColor = 0x663300aa;
			if (i == 0)
			{
				nColor = 0xff0000FF;				
				swprintf_s(buff, 128, L"Start");
			}
			else if (i == vecEachPathList.size()-1)
			{
				nColor = 0xffff0000;
				swprintf_s(buff, 128, L"End");
			}
			else
			{
				const DWORD colors[10] = 
				{
					0xFF1177BB,
					0xFF9a2230,
					0xFFEE55AA,
					0xFF235908,
					0xFFAA9234,
					0xFF62AACC,
					0xFF99BB22,
					0xFF44aaFF,
					0xFF99FF9a,
					0xFF86F0A0
				};

				nColor = colors[nIterCounter-1 % 10];
				swprintf_s(buff, 128, L"%d", i);
			}

			matWorld.SetTranslation(vPoint);

			RRenderHelper::RenderCylinder(matWorld, 10, fCylinderHeight, nColor);

			matWorld = mat4::IDENTITY;
			vec3 vNewPos = vPoint;
			if (i==0)
			{
				vOldPos = pNPC->GetPosition();
				vNewPos.z += fCylinderHeight/2;
				vOldPos.z += fCylinderHeight/2;
				XRenderHelper::RenderLine(matWorld, vOldPos, vNewPos, nColor);
			}
			else if (i > 0)
			{
				
				vNewPos.z += fCylinderHeight/2;
				vOldPos.z += fCylinderHeight/2;
				XRenderHelper::RenderLine(matWorld, vOldPos, vNewPos, nColor);
			}

			vOldPos = vPoint;
		}
	}

	// Navigation Mesh Render
	if ( !vNavigationVertices.empty())
	{
		RDevice *pDevice = REngine::GetDevicePtr();
		pDevice->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		RRenderHelper::RenderTriangles(RMatrix::IDENTITY,&vNavigationVertices[0], vNavigationVertices.size()/3
			, D3DCOLOR_ARGB(90, 0, 0, 255));
		pDevice->SetBlending(RBLEND_NONE);
	}
	if ( !vObstacleVertices.empty())
	{
		RDevice *pDevice = REngine::GetDevicePtr();

		RRenderHelper::RenderTriangles(RMatrix::IDENTITY,&vObstacleVertices[0], vObstacleVertices.size()/3
			, D3DCOLOR_ARGB(90, 250, 1, 1));
	}

	XRenderHelper::EndRenderState();


	// test
#if 0
	XRenderHelper::SetRenderState();

	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();

	iter = gvar.Debug.mapNPCMovePathList.begin();
	for (iter;
		iter != gvar.Debug.mapNPCMovePathList.end();
		++iter)
	{
		MUID uidTarget = iter->first;

		XNonPlayer* pNPC = gg.omgr->FindNPC(uidTarget);
		if (NULL == pNPC) continue;

		wchar_t text[512];
		sprintf_s(text, "Dist = %.1f", pMyPlayer->GetPosition().DistanceTo(pNPC->GetPosition()));
		pNPC->OnScreenDialog_Debug("", text);
	}
	XRenderHelper::EndRenderState();
#endif

	return;

	// test
	XRenderHelper::SetRenderState();

	int nAgentNum = gg.omgr->GetCount(ETID_NPC);
	nAgentNum = min(10, nAgentNum);
	const float toler_factors[10] =
	{
		1.0f,
		1.0f,
		1.0f,
		1.2f,
		1.2f,
		1.5f,
		1.5f,
		1.8f,
		1.8f,
		2.0f
	};

	float fTolerFactor = toler_factors[nAgentNum];

	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();

	vec3 vMyPos = pMyPlayer->GetPosition() + vec3(0,0,3);
	matWorld.SetTranslation(vMyPos);

	float fRadius = 120.0f * fTolerFactor;
	DWORD nColor = 0xFFAAAA00;
	RRenderHelper::RenderRing(matWorld, fRadius, 50.0f, nColor);

	vMyPos = pMyPlayer->GetPosition() + vec3(0,0,4);
	matWorld.SetTranslation(vMyPos);

	fRadius = 180.0f * fTolerFactor;
	nColor = 0xFFCCCC00;
	RRenderHelper::RenderRing(matWorld, fRadius, 40.0f, nColor);

	vMyPos = pMyPlayer->GetPosition() + vec3(0,0,5);
	matWorld.SetTranslation(vMyPos);

	fRadius = 220.0f * fTolerFactor;
	nColor = 0xFFEEEE00;
	RRenderHelper::RenderRing(matWorld, fRadius, 30.0f, nColor);

	XRenderHelper::EndRenderState();
}

void XNetDebuger::ActorPosUpdateRender()
{
	if (!gvar.Debug.bRenderMoveDebug) return;

	const float fCylinderHeight = 30;
	const float fCylinderRadius = 15.0f;		///< pathengine "small" shape의 사이즈가 50
	XRenderHelper::SetRenderState();

	vector<TD_DEBUG_POS>::iterator iter = gvar.Debug.vecActorPos.begin();
	for (iter;
		iter != gvar.Debug.vecActorPos.end();
		++iter)
	{
		TD_DEBUG_POS infoDebug = *iter;

		XObject* pObj = gg.omgr->Find(infoDebug.uidOwner);
		if (NULL == pObj) continue;
		
		MMatrix matWorld = mat4::IDENTITY;
		matWorld.SetTranslation(infoDebug.vPoint);
		RRenderHelper::RenderCylinder(matWorld, fCylinderRadius, fCylinderHeight, 0x88ffff00);

		vec3 vClientPos = pObj->GetPosition();
		infoDebug.vPoint.z += fCylinderHeight/2;
		vClientPos.z += fCylinderHeight/2;
		matWorld = mat4::IDENTITY;

		// 클라 위치의 소유자와 실린더를 연결하는 선
		XRenderHelper::RenderLine(matWorld, infoDebug.vPoint, vClientPos, 0xffffff00);
		
		// 타겟의 선
		XActor* pTarget = gg.omgr->FindActor(infoDebug.uidTarget);
		if (pTarget)
		{
			vec3 vTarPos = pTarget->GetModuleEntity()->GetPosition();
			vTarPos.z += fCylinderHeight/2;
			XRenderHelper::RenderLine(matWorld, vTarPos, vClientPos, 0xffff0000);
		}
		

		// 방향 화살표
		RVector up = RVector(0, 0, 1);
		matWorld.SetLocalMatrix(infoDebug.vPoint, up, infoDebug.vDir );
		RRenderHelper::RenderAxis(matWorld, 80.f, 2.0f, 8.0f, 4.0f);
		//RRenderHelper::RenderArrow(infoDebug.vPoint, infoDebug.vDir, 2.0f, 80.0f, 8.0f, 4.0f, 0xff00ff00);
	}

	XRenderHelper::EndRenderState();
}

void XNetDebuger::PickingRender()
{
	float fCylinderHeight = 150;
	MMatrix matWorld = mat4::IDENTITY;
	XRenderHelper::SetRenderState();

	{
		DWORD nColor = 0xffff0000;

		vector<pair<vec3, vec3> >::iterator iter = gvar.Debug.mapNPCPickingList.begin();
		for (iter;
			iter != gvar.Debug.mapNPCPickingList.end();
			++iter)
		{
			vec3 vOrigin = iter->first;
			vec3 vTo = iter->second;

			//mmlog("vOrigin = %.1f/%.1f/%.1f\n", 
			//	vOrigin.x, vOrigin.y, vOrigin.z);

			fCylinderHeight = vOrigin.z - vTo.z;

			matWorld.SetTranslation(vTo);
			RRenderHelper::RenderCylinder(matWorld, 4, fCylinderHeight, nColor);
		}
	}
	
	{
		DWORD nColor = 0xff0000ff;

		vector<pair<vec3, vec3> >::iterator iter = gvar.Debug.mapNPCValidPickingList.begin();
		for (iter;
			iter != gvar.Debug.mapNPCValidPickingList.end();
			++iter)
		{
			vec3 vOrigin = iter->first;
			vec3 vTo = iter->second;

			//mmlog("vOrigin = %.1f/%.1f/%.1f\n", 
			//	vOrigin.x, vOrigin.y, vOrigin.z);

			fCylinderHeight = vOrigin.z - vTo.z;

			matWorld.SetTranslation(vTo);
			RRenderHelper::RenderCylinder(matWorld, 4, fCylinderHeight, nColor);
		}
	}

	XRenderHelper::EndRenderState();
}


void XNetDebuger::SignalSphereRender()
{
	XRenderHelper::SetRenderState();

	map<MUID, TD_DEBUG_SPHERE >::iterator iter = gvar.Debug.mapNPCSignalSphere.begin();
	for (iter;
		iter != gvar.Debug.mapNPCSignalSphere.end();
		++iter)
	{
		const TD_DEBUG_SPHERE& sphere = iter->second;

		MMatrix matWorld = mat4::IDENTITY;
		matWorld.SetTranslation(sphere.vPoint);
		RRenderHelper::RenderSphere(matWorld, sphere.nRange, 0xffeeddcc);
	}
	
	XRenderHelper::EndRenderState();
}

void XNetDebuger::SetNPCMonitorString(int nParam, wstring& text)
{
	if (!m_bInited) return;

	if (nParam >= (int)vecNPCMonitorString.size()) return;

	vecNPCMonitorString[nParam] = text;
}

void XNetDebuger::DebugNPCMonitorRender(MDrawContext* pDC, MRECT r)
{
	int LINE_PIXEL = 20;
	int nLineCount = 0;

	pDC->SetColor(255, 255, 255);
	for (int i = 0; i < (int)vecNPCMonitorString.size(); i++)
	{
		pDC->Text(r.x, r.y+LINE_PIXEL*nLineCount++, MLocale::ConvUTF16ToAnsi(vecNPCMonitorString[i].c_str()).c_str());
	}
}

void XNetDebuger::NPCMonitor(int nParam, wstring& text)
{
	if(XScreenDebugger::GetInstance()->GetSelLayer()!=XScreenDebugger::LAYER5) return;

	XScreenDebugger::GetInstance()->SetReleaseTextOnDraw(false, XScreenDebugger::LAYER5);

	static int st_nLastLine = -1;

	if (nParam < st_nLastLine)
	{
		XScreenDebugger::GetInstance()->ReleaseText(XScreenDebugger::LAYER5);
	}
	st_nLastLine = nParam;

	const int LINE_PIXEL = 20;
	_ScrText5(420, (nParam*LINE_PIXEL), text.c_str());
}

void XNetDebuger::DebugCombatCalc( wstring& strType, wstring& strMessage )
{
	if (!m_bInited) return;

	int nDebugCombatSize = vecDebugCombatCalc.size();
	int nNextIndex = 0;

	if(nDebugCombatSize > 0)
		nNextIndex = vecDebugCombatCalc[nDebugCombatSize - 1].nIndex + 1;

	if(nNextIndex >= 1000)
		nNextIndex = 0;

	stDebugCombatCalc CombatData;
	CombatData.nIndex		= nNextIndex;
	CombatData.strType		= strType;
	CombatData.strMessage	= strMessage;

	vecDebugCombatCalc.push_back(CombatData);

	if(vecDebugCombatCalc.size() > 10)
	{
		vecDebugCombatCalc.erase(vecDebugCombatCalc.begin());
	}
}

void XNetDebuger::DebugCombatRender()
{
	if (!m_bInited) return;

	MDrawContext* pDC = global.ui->GetDC();
	pDC->BeginDraw();
	pDC->SetColor(MCOLOR(0x99000000));
	MRECT rt = MRECT(20, 40, 400, 630);
	pDC->FillRectangle(rt);

	XScreenDebugger::GetInstance()->SetLine(50, XScreenDebugger::LAYER5);

	for(vector<stDebugCombatCalc>::iterator it = vecDebugCombatCalc.begin(); it != vecDebugCombatCalc.end(); it++)
	{
		int nCount = it->nIndex;
		wstring strType = it->strType;
		wstring strMessage = it->strMessage;

		_ScrLine5(21, L"%d. %s", nCount, strType.c_str());

		// 맨 첫 글자가 구분자인 경우 무시
		wstring::size_type lastPos = strMessage.find_first_not_of(L"\n", 0);
		// 구분자가 아닌 첫 글자를 찾는다
		wstring::size_type pos     = strMessage.find_first_of(L"\n", lastPos);

		while (wstring::npos != pos || wstring::npos != lastPos)
		{
			_ScrLine5(70, L"%s", strMessage.substr(lastPos, pos - lastPos).c_str());

			lastPos = strMessage.find_first_not_of(L"\n", pos);
			pos = strMessage.find_first_of(L"\n", lastPos);
		}
	}

	pDC->EndDraw();
}

void XNetDebuger::HitCapsules()
{
	if (!m_bInited) return;

	const float fCylinderHeight = 30;
	const float fCylinderRadius = 15.0f;		///< pathengine "small" shape의 사이즈가 50
	XRenderHelper::SetRenderState();

	map<MUID, vector<RCapsule> >::iterator iter = gvar.Debug.mapActorCapsule.begin();
	for (iter;
		iter != gvar.Debug.mapActorCapsule.end();
		++iter)
	{
		MUID uidOwner = iter->first;
		vector<RCapsule> infoDebug = iter->second;

		XObject* pObj = gg.omgr->Find(uidOwner);
		if (NULL == pObj) continue;

		for each (const RCapsule& each in infoDebug)
		{
			MMatrix matWorld = mat4::IDENTITY;
			//matWorld.SetTranslation(infoDebug.vPoint);
			RRenderHelper::RenderCapsule(matWorld, each, 0x8800ff00);
		}
	}

	XRenderHelper::EndRenderState();
}

void XNetDebuger::DebugCombatRender(MDrawContext* pDC, MRECT r)
{
	if (!m_bInited) return;

	int LINE_PIXEL = 20;
	int nLineCount = 1;
	wchar_t szBuf[512];

	pDC->SetColor( 255, 255, 255);
	for(vector<stDebugCombatCalc>::iterator it = vecDebugCombatCalc.begin(); it != vecDebugCombatCalc.end(); it++)
	{
		int nCount = it->nIndex;
		wstring strType = it->strType;
		wstring strMessage = it->strMessage;

		swprintf_s(szBuf, 512, L"%d. %s", nCount, strType.c_str());
		pDC->Text(r.x, r.y+LINE_PIXEL*nLineCount++, MLocale::ConvUTF16ToAnsi(szBuf).c_str());

		// 맨 첫 글자가 구분자인 경우 무시
		wstring::size_type lastPos = strMessage.find_first_not_of(L"\n", 0);
		// 구분자가 아닌 첫 글자를 찾는다
		wstring::size_type pos     = strMessage.find_first_of(L"\n", lastPos);

		while (wstring::npos != pos || wstring::npos != lastPos)
		{
			swprintf_s(szBuf, 512, L"%s", strMessage.substr(lastPos, pos - lastPos).c_str());
			pDC->Text(r.x, r.y+LINE_PIXEL*nLineCount++, MLocale::ConvUTF16ToAnsi(szBuf).c_str());

			lastPos = strMessage.find_first_not_of(L"\n", pos);
			pos = strMessage.find_first_of(L"\n", lastPos);
		}
	}
}

void XNetDebuger::SaveMonitorLog()
{
	if (!m_bInited) return;

	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	wchar_t temp[1024] = {0,};
	swprintf_s(temp, L"%02d-%02d-%02d.%03d"
		, stCurTime.wHour
		, stCurTime.wMinute
		, stCurTime.wSecond
		, stCurTime.wMilliseconds);

	wchar_t szFullFileName[_MAX_PATH];
	swprintf_s(szFullFileName, L"NPCMonitor %s.txt", temp);

	FILE* fp;
	errno_t err = _wfopen_s(&fp, szFullFileName, L"wt");
	if ( 0 != err ) return;

	for each (wstring Log in vecNPCMonitorString)
	{
		fputws(Log.c_str(), fp);
		fputws(L"\n", fp);
	}

	fclose(fp);
}

void XNetDebuger::PostAIMonitorLockTarget( UIID uiidTarget )
{
	if (gvar.MyInfo.ChrInfo.nPlayerGrade <= PLAYER_GRADE_GM) return;
	if (!gvar.Network.bLoginToGameServer) return;

	XPostDebugString(L"aimon", (int)uiidTarget, L"");
}
