#include "stdafx.h"
#include "XHitEffectPosCalculator.h"
#include "XTalentHelper.h"
#include "XModuleCollision.h"
#include "XModuleEntity.h"
#include "XCharacter.h"
#include "XCameraManager.h"

//#ifdef _DEBUG
//#define HITEFFECT_POS_DEBUG
#ifdef HITEFFECT_POS_DEBUG
#include "RSceneManager.h"
#include "RDebugPassRenderer.h"
#endif
//#endif

#define HITEFFECT_COL_TEST_LEN	50.0f
#define HITEFFECT_POS_BACK_LEN	20.0f
#define HITEFFECT_MIN_MAX_SIZE	100000.0f

///////////////////////////////////
vec3 XHitEffectPosCalculator::GetPos( XObject* pVictim, CSMeshInfo* cHitterMeshInfo, XObject* pAttacker, CSMeshInfo* cAttackerMeshInfo, int nTalentID )
{
	vec3 vRetHitPos = pVictim->GetPosition();
	vRetHitPos.z += 150.0f;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pAttacker);

	if (CheckCalcEnabled(pAttacker, pTalentInfo, cAttackerMeshInfo, cHitterMeshInfo) == false)
	{
		return vRetHitPos;
	}

	MCapsule AttackerCapsule	= cAttackerMeshInfo->m_vecColCapsules[0].m_Capsule.Multiply(pAttacker->GetWorldTransform());

	//////////////////////////////////////////////////////////////////////////
	// 뉴
	XModuleCollision* pVictimCollision = pVictim->GetModuleCollision();
	XModuleCollision* pAttackerCollision = pAttacker->GetModuleCollision();
	if(pVictimCollision == NULL ||
		pAttackerCollision == NULL)
		return vRetHitPos;

	// 1. 이것부터 해보자 - 탤런트 정보에 보여줄 히트 이펙트 위치가 있다면...
	if(CalHitPositionByHitEffectPos(pVictimCollision, pAttacker, pTalentInfo, vRetHitPos))
	{
		return vRetHitPos;
	}

	// 2. 위 내용에 충족하지 못하다면...
	float fVictimColMax_z = 0.0f;
	float fVictimColMin_z = 0.0f;
	vector<MCapsule> vecVictimColCapsule;
	GetVictimHitEffectCapsule(pVictimCollision, cHitterMeshInfo, pAttacker, AttackerCapsule, vecVictimColCapsule, fVictimColMax_z, fVictimColMin_z);

	vec3 vNearTalentHit;
	vec3 vNearCol;
	float fMax_z = 0.0f;
	float fMin_z = 0.0f;
	vRetHitPos = CalHitPositionByTalentHitCapsule(pVictim->GetModuleCollision(), pAttacker, pTalentInfo, AttackerCapsule.top, vecVictimColCapsule, vNearTalentHit, vNearCol, &fMax_z, &fMin_z);

	// 3. 플레이어만 하자.
	if(pAttacker->GetEntityType() == ETID_PLAYER)
	{
		// 나보다 큰 몹에만 하자
		// 히트이펙트 캡슐이 있다면 하자
		float fAttackerArea, fAttackerHeight;
		float fVictimArea, fVictimHeight;
		GetObjectMaxSizeInfo(pAttacker, pAttackerCollision, cAttackerMeshInfo, fAttackerArea, fAttackerHeight);
		GetObjectMaxSizeInfo(pVictim, pVictimCollision, cHitterMeshInfo, fVictimArea, fVictimHeight);

		if((fAttackerHeight < fVictimHeight &&
			fAttackerArea < fVictimArea) ||
			pVictimCollision->IsHaveHitEffectCount())	
		{
			vec3 vModelHitPos = vRetHitPos;

			// 3 - 1. 가상 히트 캡슐
			MCapsule virtualHitCapsule;
			CalHitPositionByVirtualHitCapsule(pAttacker, AttackerCapsule, vecVictimColCapsule, pTalentInfo, vRetHitPos, &virtualHitCapsule);

			// 3 - 2. Pick
			if(vModelHitPos.z > fMax_z)
				vModelHitPos.z = fMax_z;
			else if(vModelHitPos.z < fMin_z)
				vModelHitPos.z = fMin_z;
			vModelHitPos = CalHitPositionByModelPick(pAttacker, pVictimCollision, AttackerCapsule, vRetHitPos);

			vRetHitPos = vModelHitPos;
		}
	}

	// 최종 Min_z, Max_z 조절, 탤런트 히트 캡슐 높이를 벗어날 수 없다.
	if(fMax_z != 0.0f &&
		fMin_z != 0.0f)
	{
		if(vRetHitPos.z > fMax_z)
			vRetHitPos.z = fMax_z;
		else if(vRetHitPos.z < fMin_z)
			vRetHitPos.z = fMin_z;
	}

	// 더 최종 히트 충돌 캡슐 높이를 벗어날 수 없다.
	if(fVictimColMax_z != 0.0f)
	{
		if(vRetHitPos.z > fVictimColMax_z)
			vRetHitPos.z = fVictimColMax_z;
		else if(vRetHitPos.z < fVictimColMin_z)
			vRetHitPos.z = fVictimColMin_z;
	}

#ifdef HITEFFECT_POS_DEBUG
	if(pAttacker->GetUID() == XGetMyUID())
	{
		RMatrix mat2;
		mat2.MakeIdentity();
		mat2.SetTranslation(vRetHitPos);
		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat2, 3, 20, D3DCOLOR_XRGB(255, 255, 255));
	}
#endif

	return vRetHitPos;
}

bool XHitEffectPosCalculator::CheckCalcEnabled( XObject* pAttacker, XTalentInfo* pTalentInfo, CSMeshInfo* cAttackerMeshInfo, CSMeshInfo* pVictimMeshInfo )
{
	if(pAttacker == NULL || pAttacker->GetActor() == NULL)
		return false;

	if(pTalentInfo == NULL)
		return false;

	if(pTalentInfo->m_nSkillType == ST_MAGIC || pTalentInfo->m_nSkillType == ST_ARCHERY)
	{
		return false;
	}

	if(cAttackerMeshInfo == NULL || 
		cAttackerMeshInfo->m_vecColCapsules.empty() ||
		pVictimMeshInfo == NULL)
	{

		return false;
	}

	return true;
}

void XHitEffectPosCalculator::GetObjectMaxSizeInfo( XObject* pObject, XModuleCollision* pObjCollision,CSMeshInfo* cMeshInfo, float& fOutArea, float& fOutHeight )
{
	float fMaxArea = 0.0f;
	float fMaxHeight = 0.0f;
	for(vector<CSColCapsule>::iterator itCol = cMeshInfo->m_vecColCapsules.begin(); itCol != cMeshInfo->m_vecColCapsules.end(); ++itCol)
	{
		MCapsule colCapsule	= pObjCollision->CalCollisionCapsuleAnimation(&(*itCol), pObject);
		float fArea = (colCapsule.height + (colCapsule.radius * 2.0f)) * colCapsule.radius;
		float fHeight = (colCapsule.top.z + colCapsule.radius) - pObject->GetPosition().z;

		if(fMaxArea < fArea)
			fMaxArea = fArea;

		if(fMaxHeight < fHeight)
			fMaxHeight = fHeight;
	}

	fOutArea = fMaxArea;
	fOutHeight = fMaxHeight;
}

vec3 XHitEffectPosCalculator::CalHitPositionByTalentHitCapsule( XModuleCollision* pVictimModuleCollision, XObject* pAttacker, XTalentInfo* pTalentInfo, vec3 vBasisPos, vector<MCapsule>& vecColCapsule, vec3& vOutNearTalentHit, vec3& vOutNearCol, float* pfTalentHitMax_z, float* pfTalentHitMin_z )
{
	vec3 vResult = vBasisPos;
	float fMinLen = HITEFFECT_MIN_MAX_SIZE;
	bool bCol = false;

	vOutNearTalentHit = vOutNearCol = vBasisPos;

	// 공격자 - 탤런트 히트 캡슐
	float fMax_z = -HITEFFECT_MIN_MAX_SIZE; 
	float fMin_z = HITEFFECT_MIN_MAX_SIZE;
	int nSegInde = 0;
	int nCapsuleIndex = 0;
	for(vector<CSHitSegment>::iterator itTalentHit_seg = pTalentInfo->m_HitInfo.m_vSegments.begin(); itTalentHit_seg != pTalentInfo->m_HitInfo.m_vSegments.end(); ++itTalentHit_seg, ++nSegInde)
	{
		for(vector<MCapsule>::iterator itTalentHit_capsule = itTalentHit_seg->m_vCapsules.begin(); itTalentHit_capsule != itTalentHit_seg->m_vCapsules.end(); ++itTalentHit_capsule, ++nCapsuleIndex)
		{
			MCapsule TalentHit			= XTalentHelper::GetTalentHitCapule(pTalentInfo, nSegInde, nCapsuleIndex, pAttacker->GetWorldTransform());

			// Min, Max 높이값
			if(TalentHit.top.z > TalentHit.bottom.z &&
				TalentHit.top.z > fMax_z)
			{
				fMax_z = TalentHit.top.z;
				if(TalentHit.bottom.z < fMin_z)
					fMin_z = TalentHit.bottom.z;
			}
			else if(TalentHit.bottom.z > fMax_z)
			{
				fMax_z = TalentHit.bottom.z;
				if(TalentHit.top.z < fMin_z)
					fMin_z = TalentHit.top.z;
			}

			// 피격자 - 메쉬 충돌 캡슐
			for(vector<MCapsule>::iterator itCol_capsule = vecColCapsule.begin(); itCol_capsule != vecColCapsule.end(); ++itCol_capsule)
			{
				float fradius_sum			= TalentHit.radius + itCol_capsule->radius;
				bool bFromParallel			= false;
				bool bTargetParallel		= false;
				vec3 vNearestPoint_TalentHit= MMath::GetNearestPointOnLine(TalentHit.bottom, TalentHit.top, itCol_capsule->bottom, itCol_capsule->top, &bFromParallel);
				vec3 vNearestPoint_Col		= MMath::GetNearestPointOnLine(itCol_capsule->bottom, itCol_capsule->top, TalentHit.bottom, TalentHit.top, &bTargetParallel);

				// 가까운 실린더로 정보 입력
				float fdiff			= (vNearestPoint_TalentHit - vNearestPoint_Col).Length();
				if(fMinLen > fdiff)
				{
					fMinLen = fdiff;

					vec3 vCapsuleDir = (pAttacker->GetPosition() - vNearestPoint_Col).Normalize();
					XMath::NormalizeZ0(vCapsuleDir);

					vResult = vNearestPoint_Col + (vCapsuleDir * itCol_capsule->radius);

					vOutNearTalentHit = vNearestPoint_TalentHit;
					vOutNearCol = vNearestPoint_Col;
					bCol = true;
				}
			}
		}
	}
	
	if(pfTalentHitMax_z)
		*pfTalentHitMax_z = fMax_z;
	if(pfTalentHitMin_z)
	{
		float fTemp = (fMax_z - fMin_z) / 2.0f;
		*pfTalentHitMin_z = fMin_z + fTemp;
	}

	return vResult;
}

void XHitEffectPosCalculator::GetVictimHitEffectCapsule( XModuleCollision* pVictimModuleCollision, CSMeshInfo* pVictimMeshInfo, XObject* pAttacker, MCapsule AttackerCapsule, vector<MCapsule>& outvecColCapsule, float& fColMax_z, float& fColMin_z )
{
	MCapsule colCapsule;
	//vec3 voutPos;
	//float fDist;

	vec3 vAttackerCapsulePickPos = AttackerCapsule.GetCenter();

	//// 1. 공격자 방향으로 PickRay로 캡슐 가져오기
	//if(pVictimModuleCollision->PickRayToColCapsule(vAttackerCapsulePickPos, pAttacker->GetDirection(), 150.0f, colCapsule, voutPos, fDist))
	//{
	//	outvecColCapsule.push_back(colCapsule);
	//	return;
	//}

	// 2. 충돌이 없다면... 전체 가져오기
	if(pVictimMeshInfo == NULL)
		return;

	//for(vector<CSColCapsule>::iterator itHitEffect_capsule = pVictimMeshInfo->m_vecHitEffectInfo.begin(); itHitEffect_capsule != pVictimMeshInfo->m_vecHitEffectInfo.end(); ++itHitEffect_capsule)
	//{
	//	MCapsule hitEffectCapsule = pVictimModuleCollision->CalCollisionCapsuleAnimation(&(*itHitEffect_capsule), pVictimModuleCollision->GetOwner());
	//	outvecColCapsule.push_back(hitEffectCapsule);
	//}

	// 히트 이펙트 캡슐이 없으면 충돌 캡슐로 가져온다.
	if(outvecColCapsule.empty())
	{
		for(vector<CSColCapsule>::iterator itCol_capsule = pVictimMeshInfo->m_vecColCapsules.begin(); itCol_capsule != pVictimMeshInfo->m_vecColCapsules.end(); ++itCol_capsule)
		{
			colCapsule	= pVictimModuleCollision->CalCollisionCapsuleAnimation(&(*itCol_capsule), pVictimModuleCollision->GetOwner());
			outvecColCapsule.push_back(colCapsule);

			if(colCapsule.top.z + colCapsule.radius > fColMax_z)
				fColMax_z = colCapsule.top.z + colCapsule.radius;
			
			if(fColMin_z == 0.0f || colCapsule.bottom.z - colCapsule.radius < fColMin_z)
				fColMin_z = colCapsule.bottom.z - colCapsule.radius;
		}
	}
}

vec3 XHitEffectPosCalculator::CalHitPositionByModelPick( XObject* pAttacker, XModuleCollision* pVictimModuleCollision, MCapsule AttackerCapsule, vec3 vRetHitPos, float* pfTalentHitMax_z, float* pfTalentHitMin_z )
{
	if(global.camera->GetPrimaryCamera() == NULL)
		return vRetHitPos;

	if(pfTalentHitMax_z &&
		pfTalentHitMin_z)
	{
		if(vRetHitPos.z > *pfTalentHitMax_z)
			vRetHitPos.z = *pfTalentHitMax_z;
		else if(vRetHitPos.z < *pfTalentHitMin_z)
			vRetHitPos.z = *pfTalentHitMin_z;
	}

	vec3 vAttackerPickPos = pAttacker->GetPosition();
	vAttackerPickPos.z = vRetHitPos.z;
	vec3 vAttackerPickDir = (vRetHitPos - vAttackerPickPos).Normalize();
	float fHitLen = (vRetHitPos - vAttackerPickPos).Length();

#ifdef HITEFFECT_POS_DEBUG
	RMatrix mat2;
	mat2.MakeIdentity();
	mat2.SetTranslation(vRetHitPos);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat2, 3, 20, D3DCOLOR_XRGB(255, 0, 0));
#endif


	vec3 vPickTo = vRetHitPos + (vAttackerPickDir * HITEFFECT_COL_TEST_LEN);
	vec3 vPickFrom = vRetHitPos - (vAttackerPickDir * (fHitLen + HITEFFECT_COL_TEST_LEN));

	bool bOpposite = false;

#ifdef HITEFFECT_POS_DEBUG
	RMatrix mat;
	mat.MakeIdentity();
	mat.SetTranslation(vPickFrom);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 3, 20, D3DCOLOR_XRGB(0, 0, 255));
	mat.SetTranslation(vPickTo);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 3, 20, D3DCOLOR_XRGB(0, 255, 255));
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugLine( vPickFrom, vPickTo, 20, D3DCOLOR_XRGB(0, 0, 255));
#endif
	MCapsule outCapsule;
	vec3 vPickPos;
	float fOutDis;
	if(pVictimModuleCollision->PickRayToHitEffectCapsule(vPickFrom, vAttackerPickDir, HITEFFECT_MIN_MAX_SIZE, outCapsule, vPickPos, fOutDis))
	{
		// 플레이어와의 방향이 다른가?
		vec3 vDirTemp = vAttackerPickDir;
		vec3 vDirTemp2 = (vPickPos - vPickFrom).Normalize();
		XMath::NormalizeZ0(vDirTemp);
		XMath::NormalizeZ0(vDirTemp2);

		if(vDirTemp.DotProduct(vDirTemp2) < 0)
		{
			// 반대 방향
			bOpposite = true;
		}
		else
		{
			vRetHitPos = vPickPos;
			vRetHitPos -= vAttackerPickDir * HITEFFECT_POS_BACK_LEN;
			#ifdef HITEFFECT_POS_DEBUG
				mat.SetTranslation(vPickPos);
				REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 3, 20, D3DCOLOR_XRGB(255, 255, 0));
			#endif
		}

	}

	// 반대로 Picking
	if(bOpposite)
	{
		vec3 vTemp = vPickTo;
		vPickTo = vPickFrom;
		vPickFrom = vTemp;
		if (pVictimModuleCollision->PickRayToHitEffectCapsule(vPickFrom, vAttackerPickDir, HITEFFECT_MIN_MAX_SIZE, outCapsule, vPickPos, fOutDis))
		{
			vRetHitPos = vPickPos;
			vRetHitPos += vAttackerPickDir * HITEFFECT_POS_BACK_LEN;
			#ifdef HITEFFECT_POS_DEBUG
				mat.SetTranslation(vPickPos);
				REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 3, 20, D3DCOLOR_XRGB(255, 0, 255));
			#endif
		}
	}

	return vRetHitPos;
}

bool XHitEffectPosCalculator::CalHitPositionByVirtualHitCapsule( XObject* pAttacker, MCapsule& AttackerCapsule, vector<MCapsule>& vecColCapsule, XTalentInfo* pTalentInfo, vec3& vRetHitPos, MCapsule* pOutVirtualHitCapsule /*= NULL*/ )
{
	// 공격자 - 탤런트 히트 캡슐
	int nSegInde = 0;
	int nCapsuleIndex = 0;
	float fMaxRadius = 0.0f;
	float fMaxLen = 0.0f;
	bool bMakeCapsule = false;
	float fMax_z = -HITEFFECT_MIN_MAX_SIZE; 
	float fMin_z = HITEFFECT_MIN_MAX_SIZE;
	vec3 vMaxPoint;

	vec3 vStart = AttackerCapsule.top;
	vec3 vEnd = AttackerCapsule.top + pAttacker->GetDirection() * 300.0f;
	for(vector<CSHitSegment>::iterator itTalentHit_seg = pTalentInfo->m_HitInfo.m_vSegments.begin(); itTalentHit_seg != pTalentInfo->m_HitInfo.m_vSegments.end(); ++itTalentHit_seg, ++nSegInde)
	{
		for(vector<MCapsule>::iterator itTalentHit_capsule = itTalentHit_seg->m_vCapsules.begin(); itTalentHit_capsule != itTalentHit_seg->m_vCapsules.end(); ++itTalentHit_capsule, ++nCapsuleIndex)
		{
			MCapsule TalentHit			= XTalentHelper::GetTalentHitCapule(pTalentInfo, nSegInde, nCapsuleIndex, pAttacker->GetWorldTransform());

			vStart.z = vEnd.z = TalentHit.top.z;
			vec3 vTop = MMath::GetNearestPointOnLine(TalentHit.top, vStart, vEnd);
			float fTopLen = (vTop - vStart).Length();

			vStart.z = vEnd.z = TalentHit.bottom.z;
			vec3 vBottom = MMath::GetNearestPointOnLine(TalentHit.bottom, vStart, vEnd);
			float fBottomLen = (vBottom - vStart).Length();

			if(fTopLen > fBottomLen &&
				fTopLen > fMaxLen)
			{
				vMaxPoint = vTop;
				fMaxLen = fTopLen;
			}
			else if(fBottomLen > fMaxLen)
			{
				vMaxPoint = vBottom;
				fMaxLen = fBottomLen;
			}

			// Min, Max
			if(TalentHit.top.z > TalentHit.bottom.z &&
				TalentHit.top.z > fMax_z)
			{
				fMax_z = TalentHit.top.z;
				if(TalentHit.bottom.z < fMin_z)
					fMin_z = TalentHit.bottom.z;
			}
			else if(TalentHit.bottom.z > fMax_z)
			{
				fMax_z = TalentHit.bottom.z;
				if(TalentHit.top.z < fMin_z)
					fMin_z = TalentHit.top.z;
			}

			if(TalentHit.radius > fMaxRadius)
				fMaxRadius = TalentHit.radius;

			bMakeCapsule = true;
		}
	}

	MCapsule capVirtualCapsule = AttackerCapsule;
	if(bMakeCapsule)
	{
		capVirtualCapsule.top = capVirtualCapsule.bottom = vMaxPoint;
		
		capVirtualCapsule.top.x = capVirtualCapsule.bottom.x = pAttacker->GetPosition().x;
		capVirtualCapsule.top.y = capVirtualCapsule.bottom.y = pAttacker->GetPosition().y;

		capVirtualCapsule.top.z = fMax_z - (fMaxRadius / 2.0f);
		capVirtualCapsule.bottom.z = fMin_z + (fMaxRadius / 2.0f);
		capVirtualCapsule.radius = fMaxRadius * 0.6f;
		capVirtualCapsule.CalcHeight();

#ifdef HITEFFECT_POS_DEBUG
		//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugCapsule(capVirtualCapsule, 10, D3DCOLOR_ARGB(100, 255, 255, 255));
#endif
	}
	else
		return false;

	if(pOutVirtualHitCapsule)
		*pOutVirtualHitCapsule = capVirtualCapsule;

	// 피격자 - 메쉬 충돌 캡슐
	float fMinLen = FLT_MAX;
	bool bCol = false;
	MCapsule capVictimCol;
	for(vector<MCapsule>::iterator itCol_capsule = vecColCapsule.begin(); itCol_capsule != vecColCapsule.end(); ++itCol_capsule)
	{
		float fradius_sum			= capVirtualCapsule.radius + itCol_capsule->radius;
		bool bFromParallel			= false;
		bool bTargetParallel		= false;
		vec3 vNearestPoint_TalentHit= MMath::GetNearestPointOnLine(capVirtualCapsule.bottom, capVirtualCapsule.top, itCol_capsule->bottom, itCol_capsule->top, &bFromParallel);
		vec3 vNearestPoint_Col		= MMath::GetNearestPointOnLine(itCol_capsule->bottom, itCol_capsule->top, capVirtualCapsule.bottom, capVirtualCapsule.top, &bTargetParallel);

		// 충돌 검사
		float fdiff			= (vNearestPoint_TalentHit - vNearestPoint_Col).Length();
		if(fradius_sum >= fdiff)
		{
			vec3 vCapsuleDir = (pAttacker->GetPosition() - vNearestPoint_Col).Normalize();
			XMath::NormalizeZ0(vCapsuleDir);

			vec3 vColSurface = vNearestPoint_Col + vCapsuleDir * itCol_capsule->radius;

			float fTestLen = (vNearestPoint_TalentHit - vColSurface).Length();
			if(fTestLen < fMinLen)
			{
				fMinLen = fdiff;
				vRetHitPos = vColSurface;
				capVictimCol = *itCol_capsule;
				bCol = true;
			}
		}
	}

	return bCol;
}

bool XHitEffectPosCalculator::CalHitPositionByHitEffectPos( XModuleCollision* pVictimModuleCollision, XObject* pAttacker, XTalentInfo* pTalentInfo, vec3& vOutHitPos )
{
	XTalentEffectInfo* pTalentEffectInfo = pTalentInfo->GetEffectDataMgr()->Get(HIT_EFFECT_POSDIR);

	if(pTalentEffectInfo == NULL)
		return false;

	float fCurAniTime = pAttacker->GetActor()->GetCurFrameToSec();

	// 제일 가까운 시간을 가져온다.
	float fTempNearTime = FLT_MAX;
	float fHitTime = 0.f;
	for(vector<CSHitSegment>::iterator itSeg = pTalentInfo->m_HitInfo.m_vSegments.begin(); itSeg != pTalentInfo->m_HitInfo.m_vSegments.end(); itSeg++)
	{
		float fCalNear = abs(itSeg->m_fCheckTime - fCurAniTime);
		if(fCalNear < fTempNearTime)
		{
			fTempNearTime = fCalNear;
			fHitTime = itSeg->m_fCheckTime;
		}
	}

	wstring n = MLocale::ConvAnsiToUTF16(pAttacker->GetActor()->GetMeshName().c_str());
	wstring nn = n.substr(0, n.rfind(L"."));

	vec3 vHitLocalPos = pTalentInfo->GetSwordHitPos(fHitTime, nn);

	if(vHitLocalPos == vec3::ZERO)
		return false;

	//////////////////////////////////////////////////////////////////////////
	// 준비
	vec3 vPickFrom = pAttacker->GetPosition();
	vPickFrom.z += vHitLocalPos.z;

	vec3 vPickTo =  vPickFrom;
	pAttacker->GetWorldTransform().TransformVect(vHitLocalPos, vPickTo);

	vec3 vPickDir = (vPickTo - vPickFrom).Normalize();

	// 어라 방향이 제로라면... 캐릭터 방향으로 하자.
	if(vPickDir == vec3::ZERO)
		vPickDir = pAttacker->GetDirection();
	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugLine( vPickFrom, vPickTo, 20, D3DCOLOR_XRGB(0, 0, 255));
	//////////////////////////////////////////////////////////////////////////
	// Pick
	bool bCol = false;
	
	MCapsule outCapsule;
	vec3 outPickPos;
	float fOutDis;
	if(pVictimModuleCollision->PickRayToHitEffectCapsule(vPickFrom, vPickDir, HITEFFECT_MIN_MAX_SIZE, outCapsule, outPickPos, fOutDis))
	{
		bCol = true;
		vOutHitPos = outPickPos;

		//RMatrix mat2;
		//mat2.MakeIdentity();
		//mat2.SetTranslation(vOutHitPos);
		//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat2, 3, 20, D3DCOLOR_XRGB(255, 0, 255));
	}

	// 충돌이 안되었다면... 일반 캡슐로 해보자.
	if(bCol == false)
	{
		if(pVictimModuleCollision->PickRayToColCapsule(vPickFrom, vPickDir, HITEFFECT_MIN_MAX_SIZE, outCapsule, outPickPos, fOutDis))
		{
			bCol = true;
			vOutHitPos = vPickFrom + (vPickDir * fOutDis);

			//RMatrix mat2;
			//mat2.MakeIdentity();
			//mat2.SetTranslation(vOutHitPos);
			//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat2, 3, 20, D3DCOLOR_XRGB(0, 255, 0));
		}
		else
		{
			// 이것도 실패냐~~ 그럼 이펙트 위치에서 그냥 뿌립니다.
			// 단, 충돌 캡슐이 한개인 경우에만 합니다.
			CSMeshInfo * pMeshInfo = pVictimModuleCollision->GetMeshInfo();
			if(pMeshInfo == NULL ||
				(pMeshInfo && pMeshInfo->m_vecColCapsules.size() == 1))
			{
				vOutHitPos = vPickTo;
				bCol = true;
			}
		}
	}

	if(bCol)
	{
		vOutHitPos -= (vPickDir * HITEFFECT_POS_BACK_LEN);
	}

	return bCol;
}