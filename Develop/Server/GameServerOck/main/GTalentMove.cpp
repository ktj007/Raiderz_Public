#include "StdAfx.h"
#include "GTalentMove.h"
#include "GDef.h"
#include "GTalentInfo.h"
#include "GEntityActor.h"
#include "GGlobal.h"
#include "GPathFinder.h"
#include "GField.h"
#include "GTalent.h"
#include "GAIRoomMgr.h"
#include "GTestSystem.h"
#include "GEntityNPC.h"

//////////////////////////////////////////////////////////////////////////
//
//	GTalentMove
//
//////////////////////////////////////////////////////////////////////////

GTalentMove::GTalentMove() 
: m_nLastMoveIndex(-1)
, m_bBuiltOK(false)
, m_pActor(NULL)
, m_pField(NULL)
, m_pMoveTalentInfo(NULL)
{
}

void GTalentMove::Finish( float fElapsedTime )
{
	if (!m_bBuiltOK)	return;

	Update(fElapsedTime);
	m_pActor->SetPosToGroundVertical();
}

bool GTalentMove::IsValid()
{
	if (!m_pMoveTalentInfo->IsMoving())	
		return true;

	if (m_pMoveTalentInfo->m_vExtraPos == vec3::ZERO &&
		m_pMoveTalentInfo->m_vExtra2Pos == vec3::ZERO &&
		m_pMoveTalentInfo->m_vExtra3Pos == vec3::ZERO &&
		m_pMoveTalentInfo->m_fExtraRot == 0.0f &&
		m_pMoveTalentInfo->m_fExtra2Rot == 0.0f &&
		m_pMoveTalentInfo->m_fExtra3Rot == 0.0f &&
		m_pMoveTalentInfo->m_MovingInfoList.empty())
	{
		return false;
	}

	if (!m_pMoveTalentInfo->m_MovingInfoList.empty())
	{
		mat4 matTM;
		matTM.SetLocalMatrix(vec3::ZERO, -m_vInitDir, vec3::AXISZ);
		mat4 matRot = mat4::IDENTITY;

		GTalentInfo::_TALENT_MOVE_INFO infoLastMove = m_pMoveTalentInfo->m_MovingInfoList.back();
		vec3 vLastPos = m_vInitPos + infoLastMove.vPos * matTM;

		if (!m_pField->PickingZ(vLastPos, vLastPos)) 
		{
			gsys.pTestSystem->PickingLog(m_pActor->GetUID(), L"pick_fail: GTalentMove::IsValid");
			return false;
		}

		float fDiffZ = abs(vLastPos.z - m_vInitPos.z);
		if (fDiffZ > PICKING_ADDED_VALUE_Z * 3)
		{
			//		mlog("이동탤런트 실패(ID:%d): 높이차이가 너무 난다. (%.1f)\n", m_pMoveTalentInfo->m_nID, fDiffZ);
			return false;	// 높이차이가 너무 난다. (2미터 이상이면 무시)
		}

		gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vLastPos);
		//if (gsys.pPathFinder->IsValidPosition(m_pField->GetPathMesh(), vLastPos) != true)
		//{
		//	//		mlog("이동탤런트 실패(ID:%d): invalid tar_pos. (%.1f %.1f %.1f)\n", m_pMoveTalentInfo->m_nID, vLastPos.x, vLastPos.y, vLastPos.z);
		//	return false;
		//}

		// AI Room 체크
		if (m_pActor->IsNPC())
		{
			GEntityNPC* OwnerNPC = ToEntityNPC(m_pActor);
			if (OwnerNPC->GetChrInfo()->nAltitude != AT_SKY)	// 공중에 있지 않을 때만 체크함
			{
				vec3 vEndPos = vLastPos;
				vEndPos.z += OwnerNPC->GetColRadius(); // 충돌구 만큼 높이를 올려줌
				if (!gmgr.pAIRoomMgr->IsValid(OwnerNPC->GetGroupID(), vEndPos))
				{
					return false;
				}
			}
		}
	}
		
	return true;
}

void GTalentMove::BuildMoveInfo()
{
	mat4 matTM;
	matTM.SetLocalMatrix(vec3::ZERO, -m_vInitDir, vec3::AXISZ);
	mat4 matRot = mat4::IDENTITY;

	for (size_t i=0; i<m_pMoveTalentInfo->m_MovingInfoList.size(); ++i)
	{
		CSTalentInfo::_TALENT_MOVE_INFO infoProtoMove = m_pMoveTalentInfo->m_MovingInfoList[i];
		MOVEINFO info;

		info.vPos = m_vInitPos + infoProtoMove.vPos * matTM;
		if (!m_pField->PickingZ(info.vPos, info.vPos))
		{
			gsys.pTestSystem->PickingLog(m_pActor->GetUID(), L"pick_fail: GTalentMove::BuildMoveInfo");
			continue;
		}

		// z축을 중심으로 시계 반대방향으로 회전
		matRot.SetRotationZ(infoProtoMove.fRot + MMath::PI);	
		info.vDir = m_vInitDir * matRot;
		info.vDir.Normalize();

		m_vecMoveInfo.push_back(info);
	}

	m_bBuiltOK = true;
}

int GTalentMove::GetMoveIndex( float fElapsedTime)
{
	int nIndex = (int)(fElapsedTime * 10.0f);
	if (nIndex < 0) return -1;	// -1 인덱스일때는 이동하지 말아야한다.

	int nMaxIndex = (int)m_vecMoveInfo.size();
	if (nIndex >= nMaxIndex) nIndex = nMaxIndex-1;

	return nIndex;
}

GTalentMove::MOVEINFO GTalentMove::GetMoveInfo(int nIndex)
{
	VALID_RET(nIndex >= 0, MOVEINFO());
	VALID_RET(nIndex < (int)m_vecMoveInfo.size(), MOVEINFO());

	return m_vecMoveInfo.at(nIndex);
}

void GTalentMove::Update(float fElapsedTime)
{
	PFC_THISFUNC;
	if (!m_bBuiltOK)	return;

	int nIndex = GetMoveIndex(fElapsedTime);
	if (nIndex < 0)					return;
	if (nIndex <= m_nLastMoveIndex) return;

	MOVEINFO infoMove = GetMoveInfo(nIndex);
	m_pActor->SetPos(infoMove.vPos);
	m_pActor->SetDir(infoMove.vDir);
	m_pActor->SetFacingDir(infoMove.vDir);

	m_nLastMoveIndex = nIndex;	
}

void GTalentMove::OnStart( GTalent* pTalent )
{
	m_pActor = pTalent->GetOwner();
	m_pField = m_pActor->GetField();
	m_pMoveTalentInfo = pTalent->GetInfo();
	m_vInitPos = pTalent->GetInitPos();
	m_vInitDir = pTalent->GetInitDir();

	if (!IsValid())
	{
		pTalent->CancelForce(false);
	}
}

void GTalentMove::OnEnterPhasePrepare( GTalent* pTalent )
{
	PFI_B(3420, "GTalentMove::OnEnterPhasePrepare");

	Update(pTalent->GetActElapsedTime());
	PFI_E(3420);
}

void GTalentMove::OnEnterPhaseAct( GTalent* pTalent )
{
	m_vInitPos = pTalent->GetInitPos();
	m_vInitDir = pTalent->GetInitDir();

	BuildMoveInfo();
}

void GTalentMove::OnUpdatePhaseAct( GTalent* pTalent, float fDelta )
{
	// 여기서 할일은 PreAct에서 수행.
	// 판정보다 Move Update가 먼저 수행되어야 하기 때문
}

void GTalentMove::OnLeavePhaseExtra( GTalent* pTalent )
{
	if (!m_bBuiltOK)	return;

	PFI_B(3430, "GTalentMove::OnLeavePhaseExtra");

	GTalentInfo* pTalentInfo = pTalent->GetInfo();
	VALID(pTalentInfo);

	mat4 matTM;
	matTM.SetLocalMatrix(vec3::ZERO, -m_pActor->GetDir(), vec3::AXISZ);
	vec3 vPos = m_pActor->GetPos() + m_pMoveTalentInfo->m_vExtraPos * matTM;

	matTM.SetRotationZ(m_pMoveTalentInfo->m_fExtraRot + MMath::PI);	
	vec3 vDir = m_pActor->GetPos() * matTM;
	vDir.Normalize();

	m_pActor->SetPos(vPos);
	m_pActor->SetDir(vDir);
	PFI_E(3430);
}

void GTalentMove::OnLeavePhaseExtra2( GTalent* pTalent )
{
	if (!m_bBuiltOK)	return;

	PFI_B(3440, "GTalentMove::OnLeavePhaseExtra2");

	mat4 matTM;
	matTM.SetLocalMatrix(vec3::ZERO, -m_pActor->GetDir(), vec3::AXISZ);
	vec3 vPos = m_pActor->GetPos() + m_pMoveTalentInfo->m_vExtra2Pos * matTM;

	matTM.SetRotationZ(m_pMoveTalentInfo->m_fExtra2Rot + MMath::PI);	
	vec3 vDir = m_pActor->GetPos() * matTM;
	vDir.Normalize();

	m_pActor->SetPos(vPos);
	m_pActor->SetDir(vDir);
	PFI_E(3440);
}

void GTalentMove::OnLeavePhaseExtra3( GTalent* pTalent )
{
	if (!m_bBuiltOK)	return;

	PFI_B(3450, "GTalentMove::OnLeavePhaseExtra3");

	mat4 matTM;
	matTM.SetLocalMatrix(vec3::ZERO, -m_pActor->GetDir(), vec3::AXISZ);
	vec3 vPos = m_pActor->GetPos() + m_pMoveTalentInfo->m_vExtra3Pos * matTM;

	matTM.SetRotationZ(m_pMoveTalentInfo->m_fExtra3Rot + MMath::PI);	
	vec3 vDir = m_pActor->GetPos() * matTM;
	vDir.Normalize();

	m_pActor->SetPos(vPos);
	m_pActor->SetDir(vDir);
	PFI_E(3450);
}

void GTalentMove::OnLeavePhaseCanceled( GTalent* pTalent )
{
	PFI_B(3460, "GTalentMove::OnLeavePhaseCanceled");

	Finish(pTalent->GetActElapsedTime());
	PFI_E(3460);
}

void GTalentMove::OnLeavePhaseFinish( GTalent* pTalent )
{
	PFI_B(3470, "GTalentMove::OnLeavePhaseFinish");

	Finish(100000.0f);
	PFI_E(3470);
}

void GTalentMove::OnUpdatePhasePreAct( GTalent* pTalent, float fDelta )
{
	PFI_B(3480, "GTalentMove::OnUpdatePhasePreAct");

	Update(pTalent->GetActElapsedTime());
	PFI_E(3480);
}

GTalentMove::MOVEINFO GTalentMove::GetExpectedMoveInfo( float fActElapsedTime )
{
	MOVEINFO infoMove;

	int nIndex = GetMoveIndex(fActElapsedTime);
	if ((nIndex < 0) || (nIndex < m_nLastMoveIndex))
	{
		infoMove.vPos = m_vInitPos;
		infoMove.vDir = m_vInitDir;
	}
	else
	{
		infoMove = GetMoveInfo(nIndex);
	}

	return infoMove;	
}

