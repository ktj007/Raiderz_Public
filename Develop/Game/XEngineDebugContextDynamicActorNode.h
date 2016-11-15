#pragma once

#include "XBaseUnitTestHelper.h"
#include "XEngineDebugContext.h"
#include "MPhysX4R3/MPxDynamicActorNodeTC.h"

#include "RDebugPassRenderer.h"

class XEngineDebugContextDynamicActorNode : public XEngineDebugContext
{
public:
	XEngineDebugContextDynamicActorNode()
		: XEngineDebugContext(XEngineDebugContext::DCT_FREE_WORLD)
	{
		m_bNeedToReset = false;
		m_fAniReserve = -1.f;

		m_vStartPos.Set(500,500,0);
		m_vStartDir.Set(1,1,0);
		m_vStartDir.Normalize();

		m_vStartPosPlayer = m_vStartPos + (300.f * m_vStartDir);
		m_vStartDirPlayer = -m_vStartDir;
	}

	virtual ~XEngineDebugContextDynamicActorNode(){}

	virtual void OnUpdate(float fDelta)
	{
		if (m_fAniReserve > 0.f)
		{
			m_fAniReserve += fDelta;
			if(m_fAniReserve > 0.1f)
			{
				StartDynamicActorNode();
				m_fAniReserve = -1.f;
			}
		}

		global.smgr->GetPassRenderer<RDebugPassRenderer>()->AddDebugLine(vec3(5057.6919, 4628.7607, 0), vec3(5057.6919, 4628.7607, 150.00000), 0.001f, 0xffffffff);
		global.smgr->GetPassRenderer<RDebugPassRenderer>()->AddDebugLine(vec3(5248.9121, 4689.0840, 62.034668), vec3(5098.0376, 4647.9277, 296.03467), 0.001f, 0xffffffff);

		RMatrix mat1, mat2, mat3, mat4;
		mat1.SetTranslationMatrix(vec3(5057.6919, 4628.7607, 0));
		mat2.SetTranslationMatrix(vec3(5057.6919, 4628.7607, 150.00000));
		global.smgr->GetPassRenderer<RDebugPassRenderer>()->AddDebugSphere(mat1, 40, 0.001, 0xffffffff);
		global.smgr->GetPassRenderer<RDebugPassRenderer>()->AddDebugSphere(mat2, 40, 0.001, 0xffffffff);

		mat3.SetTranslationMatrix(vec3(5248.9121, 4689.0840, 62.034668));
		mat4.SetTranslationMatrix(vec3(5098.0376, 4647.9277, 296.03467));
		global.smgr->GetPassRenderer<RDebugPassRenderer>()->AddDebugSphere(mat3, 110, 0.001, 0xffffffff);
		global.smgr->GetPassRenderer<RDebugPassRenderer>()->AddDebugSphere(mat4, 110, 0.001, 0xffffffff);

		RMatrix mat5;
		mat5.SetTranslationMatrix(vec3((5248.9121+5098.0376)*0.5, (4689.0840+4647.9277)*0.5, (62.034668+296.03467)*0.5));
		global.smgr->GetPassRenderer<RDebugPassRenderer>()->AddDebugSphere(mat5, 110, 0.001, 0xffffffff);

		class Test
		{
		public:
			static float GetDistanceSegment( const MVector3 &A, const MVector3 &B,const MVector3 &C,const MVector3 &D, bool* bParallel = NULL )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				// from Essential Mathematics for Games & Interactive Applications, 489~495
				//
				//////////////////////////////////////////////////////////////////////////

				MVector3 w0 = A - C;
				MVector3 u = B - A;
				MVector3 v = D - C;

				//
				//(w0 종점)							(wc 종점)
				// A-----------u vector, s_c--------B
				// |								|
				// w0 vector					wc vector
				// |								|
				// C-----------v vector, t_c--------D
				//
				// wc : [A,B], [C,D] 선분위의 최단거리를 잇는 선분
				// s_c : [A,B] 상에서 A를 시작점, u 백터를 방향백터로 할때 wc의 종점이 있게하는 상수
				// t_c : [C,D] 상에서 C를 시작점, v 백터를 방향백터로 할때 wc의 시작점이 있게하는 상수
				// t_c * v + wc = w0 + s_c * u
				//		i.e) wc = w0 + s_c*u - t_c*v
				// 양변에, u, v를 dot
				//		i.e) u dot ( wc ) = u dot (w0 + s_c*u - t_c*v) ( = u dot w0 + s_c * (u dot u) - t_c * (u dot v) )
				//		i.e) v dot ( wc ) = v dot (w0 + s_c*u - t_c*v) ( = v dot w0 + s_c * (v dot u) - t_c * (v dot v) )
				// wc는 최단을 이으므로 u와 v에 모두 수직
				//		i.e) wc dot u = 0, wc dot v = 0
				//		i.e) 0 = u dot w0 + s_c * (u dot u) - t_c * (u dot v) = v dot w0 + s_c * (v dot u) - t_c * (v dot v)

				// temp a,b,c,d,e
				float a = u.DotProduct(u);
				float b = u.DotProduct(v);
				float c = v.DotProduct(v);
				float d = u.DotProduct(w0);
				float e = v.DotProduct(w0);

				// parameters to compute s_c, t_c
				float sn, sd, tn, td;

				// denon이 0 이면 u, v가 평행
				//		i.e) | u dot v | == ||u||*||v||
				float denom = a*c - b*b;
				if (fabs(denom) < FLT_EPSILON)	// 평행하는 경우
				{
					// clamp s_c to 0
					sd = td = c;
					sn = 0.0f;
					tn = e;
				}
				else
				{
					// clamp s_c within [0,1]
					sd = td = denom;
					sn = b*e - c*d;
					tn = a*e - b*d;

					// clamp s_c to 0
					if (sn < 0.0f)
					{
						sn = 0.0f;
						tn = e;
						td = c;
					}
					// clamp s_c to 1
					else if (sn > sd)
					{
						sn = sd;
						tn = e + b;
						td = c;
					}
				}

				float t_c, s_c;
				// clamp t_c within [0,1]
				// clamp t_c to 0
				if (tn < 0.0f)
				{
					t_c = 0.0f;
					// clamp s_c to 0
					if ( -d < 0.0f )
					{
						s_c = 0.0f;
					}
					// clamp s_c to 1
					else if ( -d > a )
					{
						s_c = 1.0f;
					}
					else
					{
						s_c = -d/a;
					}
				}
				// clamp t_c to 1
				else if (tn > td)
				{
					t_c = 1.0f;
					// clamp s_c to 0
					if ( (-d+b) < 0.0f )
					{
						s_c = 0.0f;
					}
					// clamp s_c to 1
					else if ( (-d+b) > a )
					{
						s_c = 1.0f;
					}
					else
					{
						s_c = (-d+b)/a;
					}
				}
				else
				{
					t_c = tn/td;
					s_c = sn/sd;
				}

				// compute difference vector and distance (not squared)
				MVector3 wc = w0 + s_c*u - t_c*v;
				return wc.Length();


				//return wc.Dot(wc);
				//MVector3 AB = b - a;
				//MVector3 CD = d - c;
				//MVector3 H = AB.CrossProduct(CD);
				//MVector3 N = AB.CrossProduct(H);

				//MVector3 h1 = H.Normalize() * 100 + a;
				//MVector3 n1 = N.Normalize() * 100 + a;
				//float CDDotN = CD.DotProduct(N);

				//if (fabs(CDDotN) < FLT_EPSILON)					
				//{
				//	// 두 선분이 평행이다.
				//	float f1 = MMath::GetDistancePointLineSegment(c, a, b);
				//	float f2 = MMath::GetDistancePointLineSegment(d, a, b);

				//	if(bParallel)
				//		*bParallel = true;

				//	if (f1 < f2)
				//		return f1;
				//	else
				//		return f2;
				//}
				//else
				//{
				//	// 교점이 생긴다. 
				//	// I0 -> 선분 ab위의, 선분 cd와 가장 가까운 점
				//	// I1 -> 선분 cd위의, 선분 ab와 가장 가까운 점

				//	float  t1 = (a - c).DotProduct(N) / CDDotN;
				//	t1 = (t1 < 0.0f)? 0.0f : (t1 > 1.0f)? 1.0f : t1;
				//	MVector3 I1 = c + t1 * CD;

				//	float  t0 = (I1 - a).DotProduct(AB) / (AB.DotProduct(AB));
				//	t0 = (t0 < 0.0f)? 0.0f : (t0 > 1.0f)? 1.0f : t0;
				//	MVector3 I0 = a + t0 * AB;

				//	if(bParallel)
				//		*bParallel = false;

				//	return I0.DistanceTo(I1);
				//}
			}
		};

		//float d = Test::GetDistanceSegment(vec3(5057.6919, 4628.7607, 0), vec3(5057.6919, 4628.7607, 150.00000), vec3(5248.9121, 4689.0840, 62.034668), vec3(5098.0376, 4647.9277, 296.03467));
		float d = MMath::GetDistanceSegment(vec3(5057.6919, 4628.7607, 0), vec3(5057.6919, 4628.7607, 150.00000), vec3(5248.9121, 4689.0840, 62.034668), vec3(5098.0376, 4647.9277, 296.03467));
	}

	void StartDynamicActorNode()
	{
		XActor* pMonster = gg.omgr->FindActor(m_MonsterMUID);
		XModuleEntity* pEntity = pMonster->GetModuleEntity();
		MPxDynamicActorNodeTC* pController = pEntity->GetTransformController<MPxDynamicActorNodeTC>();

		// get group scenenode
		const char* pSzPartsName = "b_parts_01";
		RActorNode* pActorNode = pEntity->GetActor()->GetActorNode(pSzPartsName);

		// make frag list
		std::vector<std::string> vList;
		pController->MakeAndPrepareSimulationActorNodes(pActorNode, vList);

		// make force
		vec3 vPos = pActorNode->GetWorldPosition();
		vec3 vForce;
		pController->MakeUpwardRandomForce(vForce, NULL);

		// start simulation
		MPxTcDynamicActorNodesGroup* pGroup = pController->CreateDynamicActorNodesGroup(0);
		if (pGroup)
			pGroup->StartPhysicSimulation(vList, vForce, vPos, pSzPartsName);
	}

	void InitDynamicActorNodeWorld()
	{
		// 월드
		XWorld* pWorld = gg.currentgamestate->GetWorld();

		XWorldInitParam worldInitParam;
		worldInitParam.SetZone(L"BirdTown", true, false);

		pWorld->Init(worldInitParam);

		m_MonsterMUID = global.system->GenerateLocalUID();
		m_PlayerID = global.system->GenerateLocalUID();

		// NPC
		TD_UPDATE_CACHE_NPC td_update_cache_npc = XBaseUnitTestHelper::Make_TD_UPDATE_CACHE_NPC(m_MonsterMUID, 110034, m_vStartPos, m_vStartDir);
		gg.currentgamestate->InNPCs(&td_update_cache_npc, 1);
		XActor* pMonster = gg.omgr->FindActor(m_MonsterMUID);
		pMonster->GetModuleEntity()->SetScale(5.f);

		// Net Player
		TD_UPDATE_CACHE_PLAYER td_update_cache_player = XBaseUnitTestHelper::MakeSamle_TD_UPDATE_CACHE_PLAYER(m_PlayerID);
		gg.currentgamestate->InPlayer(td_update_cache_player.uid, &td_update_cache_player);

		// 사용가능하게 됨
		m_bContextUsable = true;

		// 카메라
		rs3::RCameraSceneNode* pCamera = global.smgr->GetPrimaryCamera();
		if (pCamera != NULL)
		{
			pCamera->SetTransform( RVector(1051.1499, 1928.9570, 577.73804), RVector(-0.42497706, -0.85046417, -0.31000835) );
			pCamera->UpdateTransform();
		}
	}

	virtual void OnCmd(const std::vector< std::wstring >& _refConsoleCmd)
	{
		_ASSERT(_refConsoleCmd.empty() == false);
		if (_refConsoleCmd[0] == L"init")
		{
			InitDynamicActorNodeWorld();
		}
		else
		{
			if (false == m_bContextUsable)
			{
				m_pEngineDebugger->PrintToConsole(L"다이나믹 액터노드를 테스트 할수 있는 환경 아님 - 'ed r init' 요망");
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			// 서브 커맨드
			//
			if (_refConsoleCmd[0] == L"a")
			{
				StartDynamicActorNode();
				return;
			}

			if( _refConsoleCmd[0] == L"ani")
			{
				m_fAniReserve = 0.001f;
				XActor* pMonster = gg.omgr->FindActor(m_MonsterMUID);
				pMonster->GetModuleEntity()->GetAnimationController()->SetAnimation(std::wstring(L"die"));
				return;
			}
		}
	}

	bool m_bNeedToReset;

	MUID m_MonsterMUID;
	MUID m_PlayerID;

	int m_nTalentID;
	int m_nGripPhase;
	float m_fElapsed;

	RVector m_vStartPos;
	RVector m_vStartDir;

	RVector m_vStartPosPlayer;
	RVector m_vStartDirPlayer;

	float m_fAniReserve;
};
