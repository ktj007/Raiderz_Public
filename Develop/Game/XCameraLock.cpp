#include "StdAfx.h"
#include "XCameraLock.h"
#include "XCameraManager.h"
#include "XModuleEntity.h"
#include "XCharacter.h"

#define CAMERA_LOCK_START_MOVE_SPEED	0.25f
#define CAMERA_LOCK_END_MOVE_SPEED		0.2f

XCameraLock::XCameraLock( void )
{
	Init();
}

XCameraLock::~XCameraLock( void )
{

}

void XCameraLock::Init()
{
	m_fElapsedTime		= 0.0f;
	m_fMoveTime			= 0.0f;
	m_fDis				= 0.0f;

	m_fDurationTime		= 0.0f;
	m_bCameraLock		= false;
	m_bCameraLockMove	= false;
	m_bCameraLockEnd	= false;
	m_bWithOutLimitTime	= false;

	m_vStartPos			= vec3::ZERO;
	m_vStartDir			= vec3::ZERO;
	m_vEndPos			= vec3::ZERO;
	m_vEndDir			= vec3::ZERO;

	m_uidTarget.SetZero();
	m_strBoneName.clear();
}

void XCameraLock::Start( RCameraSceneNode* pCameraSceneNode, XPolarCoord* pCur, MUID& uidOwner, MUID& uidTarget, wstring& strBoneName, float& fDurationTime )
{
	if (gg.omgr == NULL) return;
	XObject* pTargetObject = gg.omgr->Find(uidTarget);
	XObject* pOwnerObject = gg.omgr->Find(uidOwner);
	if (pTargetObject == NULL || pOwnerObject == NULL) 
		return;

	m_uidTarget = uidTarget;
	m_uidOwner = uidOwner;
	m_strBoneName = strBoneName;

	m_bCameraLock = true;
	m_bCameraLockMove = false;
	m_bCameraLockEnd = false;
	m_bWithOutLimitTime	= false;

	m_fElapsedTime = CAMERA_LOCK_START_MOVE_SPEED;
	m_fMoveTime = CAMERA_LOCK_START_MOVE_SPEED;
	m_fDurationTime = fDurationTime;

	if(fDurationTime == -1)
		m_bWithOutLimitTime = true;

	m_BackViewPolarCoord = *pCur;

	m_fDis			= pCur->fRadius;

	// 이동 계산
	GetCameraLockPolarCoord(m_uidOwner, m_uidTarget, m_strBoneName, m_vEndPos, m_vEndDir);

	m_vStartPos		= pCameraSceneNode->GetPosition();
	m_vStartDir		= pCameraSceneNode->GetDirection();
}

void XCameraLock::End( XPolarCoord* pCur, RCameraSceneNode* pCameraSceneNode )
{
	if(m_bCameraLock == false)
		return;

	m_bCameraLock = false;
	m_bCameraLockMove = false;
	m_bCameraLockEnd = true;
	m_bWithOutLimitTime	= false;

	vec3 vDir = pCameraSceneNode->GetDirection() * m_fDis;
	//vec3 vDir = m_vStartDir * m_fDis;

	MMath::CartesianToSpherical(vDir, pCur->fRadius, pCur->fPhi, pCur->fTheta);

	pCur->fRadius = m_fDis;
}

bool XCameraLock::IsCameraLockActive()
{
	if(m_bCameraLock && m_bCameraLockMove == false)
		return true;

	return false;
}

bool XCameraLock::IsCameraLockStartEnd()
{
	if(m_bCameraLockMove)
		return true;

	return false;
}

void XCameraLock::CalcPosDirForCameraLock( float fDelta, XPolarCoord* pCur, RCameraSceneNode* pCameraSceneNode, vec3& vPosition, vec3& vDirection )
{
	float fphi = 0.0f;
	float ftheta = 0.0f;
	if(GetCameraLockPolarCoord(m_uidOwner, m_uidTarget, m_strBoneName, vPosition, vDirection) == false)
		return;

	pCur->fRadius = m_fDis;

	UpdateDurationTime(fDelta, pCur, pCameraSceneNode);
}

void XCameraLock::StartEndUpdate( float fDelta, XPolarCoord* pCur, RCameraSceneNode* pCameraSceneNode, vec3 vTar, vec3& vPosition, vec3& vDirection )
{
	if(m_bCameraLockMove)
	{
		m_fElapsedTime += fDelta;

		float t = m_fElapsedTime / m_fMoveTime;

		if(t >= 1.0f)
		{
			t = 1.0f;
			m_bCameraLockMove = false;
		}

		pCur->fRadius = m_fDis;
		m_vStartPos.InterpolateTo(m_vEndPos, t, vPosition);
		m_vStartDir.InterpolateTo(m_vEndDir, t, vDirection);

		UpdateDurationTime(fDelta, pCur, pCameraSceneNode);
	}
}

bool XCameraLock::GetCameraLockPolarCoord( MUID uidOwner, MUID uidTarget, wstring strBoneNam, vec3& vPosition, vec3& vDirection )
{
	if (gg.omgr == NULL) 
		return false;

	XObject* pTargetObject = gg.omgr->Find(uidTarget);
	XObject* pOwnerObject = gg.omgr->Find(uidOwner);
	if (pTargetObject == NULL || pOwnerObject == NULL) 
		return false;

	// 이동 계산
	vec3 vTargetPos = pTargetObject->GetPosition();
	if(m_strBoneName.empty() == false)
	{
		RMatrix matOut;
		if(pTargetObject->GetActor()->GetActorNodeTransform(m_strBoneName.c_str(), matOut, E_TS_WORLD, true) != NULL)
			vTargetPos = matOut.GetTranslation();
	}

	vec3 vOnwer = pOwnerObject->GetPosition();
	vOnwer.z += XCONST::CAMERA_TARGET_HEIGHT;

	//vec3 vCalDir = (vOnwer - vTargetPos).Normalize();
	//vPosition = vOnwer + (vCalDir * m_fDis);
	//vDirection = (vOnwer - vPosition).Normalize();

	// 내 캐릭터를 왼쪽으로 비껴보이게 하기 위함
	vec3 vCalDir = (vOnwer - vTargetPos).Normalize();
	vec3 right = CrossProduct(vCalDir, UP_VECTOR);
	right.z = 0.0f;
	right.Normalize();

	vec3 vCalPos = vOnwer - (right * gvar.Debug.fTestCameraLockRightValue);
	vCalDir = (vCalPos - vTargetPos).Normalize();

	vPosition = vCalPos + (vCalDir * gvar.Debug.fTestCameraLockBackValue);
	vPosition.z += gvar.Debug.fTestCameraLockHeightValue;
	vDirection = (vTargetPos - vPosition).Normalize();

	return true;
}

void XCameraLock::UpdateDurationTime( float fDelta, XPolarCoord* pCur, RCameraSceneNode* pCameraSceneNode )
{
	if(m_bWithOutLimitTime == true)
		return;

	m_fDurationTime -= fDelta;

	if(m_fDurationTime <= 0.0f && m_bCameraLockEnd == false)
	{
		// 종료 처리
		End(pCur, pCameraSceneNode);
	}
}
//
//void XCameraLock::ForceEnd()
//{
//	Init();
//}
